#ifndef WDT_SUPERVISOR_H_
#define WDT_SUPERVISOR_H_


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/sys/atomic.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/* --- Build-time defaults (can be overridden in prj.conf via -Dflags if needed) --- */
#ifndef WDT_SUP_MAX_WATCHES
#define WDT_SUP_MAX_WATCHES 8
#endif


#ifndef WDT_SUP_STACK_SIZE
#define WDT_SUP_STACK_SIZE 1024
#endif


#ifndef WDT_SUP_THREAD_PRIORITY
/* Lower number = higher priority; make it reasonably high so it gets CPU time */
#define WDT_SUP_THREAD_PRIORITY 3
#endif


/* Zephyr device node for WDT; on nRF SoCs this is typically wdt0 */
#ifndef WDT_SUP_DT_NODE
#define WDT_SUP_DT_NODE DT_NODELABEL(wdt0)
#endif


/* Handle identifying a registered watch (thread/worker) */
typedef int wdt_sup_watch_t; /* -1 means invalid */


/*
* Initialize HW Watchdog and start supervisor thread.
*
* @param wdt_window_ms Max window (timeout) in milliseconds for HW WDT channel.
* @param supervisor_period_ms Period (ms) of supervisor loop. Must be << wdt_window_ms.
* @param flags WDT flags, e.g. WDT_FLAG_RESET_SOC | WDT_FLAG_PAUSE_HALTED_BY_DBG.
*
* @return 0 on success, negative errno on error.
*/
int wdt_sup_init(uint32_t wdt_window_ms, uint32_t supervisor_period_ms, uint32_t flags);


/*
* Register a logical watch with a per-thread threshold. The supervisor will only feed the HW WDT
* if all active watches have been updated (heartbeat) within their threshold.
*
* @param name Optional short name for logs (copied, truncated to internal buffer).
* @param threshold_ms Max allowed staleness of this watch's heartbeat.
*
* @return watch id (>=0) on success, -ENOMEM if table full, or -EINVAL on bad input.
*/
wdt_sup_watch_t wdt_sup_register(const char *name, uint32_t threshold_ms);


/* Update heartbeat for a given watch (call from the healthy thread regularly). */
void wdt_sup_heartbeat(wdt_sup_watch_t id);


/* Change threshold at runtime (optional). */
void wdt_sup_set_threshold(wdt_sup_watch_t id, uint32_t threshold_ms);


/* Pause supervisor feeding (true) or resume (false). Useful for controlled testing). */
void wdt_sup_pause(bool pause);


/* Force a fail-safe: stop feeding permanently so WDT will expire. */
void wdt_sup_force_expire(void);


#ifdef __cplusplus
}
#endif


#endif /* WDT_SUPERVISOR_H_ */



