#include <string.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(wdt_supervisor, CONFIG_WDT_LOG_LEVEL);

#include "wdt_supervisor.h"

#if !DT_NODE_HAS_STATUS(WDT_SUP_DT_NODE, okay)
#error "WDT_SUP_DT_NODE is not okay; check your SoC's WDT devicetree label (wdt0)."
#endif

#define NAME_MAX_LEN 16

struct watch_entry {
    atomic_t last_seen_ms;     /* updated by heartbeats */
    uint32_t threshold_ms;     /* max allowed staleness */
    bool     active;           /* whether entry is in use */
    char     name[NAME_MAX_LEN];
};

static const struct device *s_wdt_dev;
static int                  s_wdt_channel = -1;
static uint32_t             s_wdt_window_ms;
static uint32_t             s_period_ms;
static atomic_t             s_paused;          /* 0/1 */
static atomic_t             s_force_expire;    /* 0/1 */
static struct watch_entry   s_watches[WDT_SUP_MAX_WATCHES];

/* Supervisor thread data */
K_THREAD_STACK_DEFINE(s_wdt_sup_stack, WDT_SUP_STACK_SIZE);
static struct k_thread s_wdt_sup_thread;
static k_tid_t         s_wdt_sup_tid;

/* Forward */
static void wdt_supervisor_main(void *, void *, void *);

static inline uint32_t now_ms(void)
{
    return (uint32_t)k_uptime_get();
}

int wdt_sup_init(uint32_t wdt_window_ms, uint32_t supervisor_period_ms, uint32_t flags)
{
    if (wdt_window_ms == 0 || supervisor_period_ms == 0 || supervisor_period_ms >= wdt_window_ms) {
        return -EINVAL;
    }

    s_wdt_dev = DEVICE_DT_GET(WDT_SUP_DT_NODE);
    if (!device_is_ready(s_wdt_dev)) {
        return -ENODEV;
    }

    /* Clear registry */
    for (int i = 0; i < WDT_SUP_MAX_WATCHES; ++i) {
        s_watches[i].active = false;
        s_watches[i].threshold_ms = 0;
        atomic_set(&s_watches[i].last_seen_ms, 0);
        memset(s_watches[i].name, 0, sizeof(s_watches[i].name));
    }

    s_wdt_window_ms = wdt_window_ms;
    s_period_ms = supervisor_period_ms;
    atomic_set(&s_paused, 0);
    atomic_set(&s_force_expire, 0);

    struct wdt_timeout_cfg cfg = {
        .window = { .min = 0, .max = wdt_window_ms },
        .callback = NULL,      /* NULL → reset SoC on timeout */
        .flags = flags,
    };

    s_wdt_channel = wdt_install_timeout(s_wdt_dev, &cfg);
    if (s_wdt_channel < 0) {
        return s_wdt_channel;
    }

    int err = wdt_setup(s_wdt_dev, 0);
    if (err) {
        return err;
    }

    /* Seed last_seen to now so we don't instantly trip before first heartbeat. */
    uint32_t t0 = now_ms();
    for (int i = 0; i < WDT_SUP_MAX_WATCHES; ++i) {
        atomic_set(&s_watches[i].last_seen_ms, (atomic_val_t)t0);
    }

    /* Start supervisor thread */
    s_wdt_sup_tid = k_thread_create(&s_wdt_sup_thread,
                                    s_wdt_sup_stack, K_THREAD_STACK_SIZEOF(s_wdt_sup_stack),
                                    wdt_supervisor_main,
                                    NULL, NULL, NULL,
                                    WDT_SUP_THREAD_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(s_wdt_sup_tid, "wdt_sup");

    return 0;
}

wdt_sup_watch_t wdt_sup_register(const char *name, uint32_t threshold_ms)
{
    if (threshold_ms == 0) {
        return -EINVAL;
    }
    for (int i = 0; i < WDT_SUP_MAX_WATCHES; ++i) {
        if (!s_watches[i].active) {
            s_watches[i].active = true;
            s_watches[i].threshold_ms = threshold_ms;
            atomic_set(&s_watches[i].last_seen_ms, (atomic_val_t)now_ms());
            if (name && name[0] != '\0') {
                strlcpy(s_watches[i].name, name, sizeof(s_watches[i].name));
            } else {
                s_watches[i].name[0] = '\0';
            }
            return i;
        }
    }
    return -ENOMEM;
}

void wdt_sup_heartbeat(wdt_sup_watch_t id)
{
    if (id < 0 || id >= WDT_SUP_MAX_WATCHES) {
        return;
    }
    if (!s_watches[id].active) {
        return;
    }
    atomic_set(&s_watches[id].last_seen_ms, (atomic_val_t)now_ms());
}

void wdt_sup_set_threshold(wdt_sup_watch_t id, uint32_t threshold_ms)
{
    if (id < 0 || id >= WDT_SUP_MAX_WATCHES || threshold_ms == 0) {
        return;
    }
    if (!s_watches[id].active) {
        return;
    }
    s_watches[id].threshold_ms = threshold_ms;
}

void wdt_sup_pause(bool pause)
{
    atomic_set(&s_paused, pause ? 1 : 0);
}

void wdt_sup_force_expire(void)
{
    atomic_set(&s_force_expire, 1);
}

static bool all_watches_healthy(uint32_t now)
{
    bool any_active = false;
    for (int i = 0; i < WDT_SUP_MAX_WATCHES; ++i) {
        if (!s_watches[i].active) {
            continue;
        }
        any_active = true;
        uint32_t seen = (uint32_t)atomic_get(&s_watches[i].last_seen_ms);
        uint32_t thr  = s_watches[i].threshold_ms;
        if ((now - seen) > thr) {
            if (s_watches[i].name[0]) {
                LOG_ERR("WDT watch '%s' missed: now-seen=%u ms > thr=%u ms", s_watches[i].name, (unsigned)(now - seen), (unsigned)thr);
            } else {
                LOG_ERR("WDT watch %d missed: now-seen=%u ms > thr=%u ms", i, (unsigned)(now - seen), (unsigned)thr);
            }
            return false;
        }
    }
    /* If no watches are active yet, keep feeding to avoid premature resets. */
    return true || any_active;
}

static void wdt_supervisor_main(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    while (1) {
        uint32_t now = now_ms();
        if (!atomic_get(&s_paused) && !atomic_get(&s_force_expire)) {
            if (all_watches_healthy(now)) {
                (void)wdt_feed(s_wdt_dev, s_wdt_channel);
            } else {
                /* Intentionally skip feeding → SoC will reset after window expires */
            }
        }
        k_sleep(K_MSEC(s_period_ms));
    }
}