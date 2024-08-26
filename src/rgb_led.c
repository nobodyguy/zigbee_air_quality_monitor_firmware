/*
 * Copyright (c) 2024 Jan Gnip
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "rgb_led.h"

LOG_MODULE_REGISTER(rgb_led, CONFIG_ZIGBEE_AIR_QUALITY_MONITOR_LOG_LEVEL);

#define STRIP_NODE DT_ALIAS(led_strip)
#define STRIP_NUM_PIXELS DT_PROP(DT_ALIAS(led_strip), chain_length)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(0xff, 0x00, 0x00), /* red */
	RGB(0x00, 0xff, 0x00), /* green */
	RGB(0xff, 0x80, 0x00), /* orange */
};

struct led_rgb pixel;
size_t current_color = SIZE_MAX;
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

void rgb_led_init(void)
{
	if (device_is_ready(strip)) {
		LOG_DBG("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return;
	}
}

void rgb_led_update_color(size_t color)
{
	current_color = color;
	memset(&pixel, 0x00, sizeof(pixel));
	memcpy(&pixel, &colors[current_color], sizeof(struct led_rgb));
	int rc = led_strip_update_rgb(strip, &pixel, STRIP_NUM_PIXELS);
	if (rc) {
		LOG_ERR("couldn't update strip: %d", rc);
	}
}

void rgb_led_red(void)
{
	if (current_color != 0) {
		rgb_led_update_color(0);
	}
}

void rgb_led_green(void)
{
	if (current_color != 1) {
		rgb_led_update_color(1);
	}
}

void rgb_led_orange(void)
{
	if (current_color != 2) {
		rgb_led_update_color(2);
	}
}