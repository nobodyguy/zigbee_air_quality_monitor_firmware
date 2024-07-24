/*
 * Copyright (c) 2023 Jan Gnip
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <ram_pwrdn.h>
#include <zb_nrf_platform.h>
#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zephyr/kernel.h>
#include <zigbee/zigbee_app_utils.h>
#include <zigbee/zigbee_error_handler.h>
#include <dk_buttons_and_leds.h>

#ifdef CONFIG_USB_DEVICE_STACK
#include <zephyr/usb/usb_device.h>
#endif /* CONFIG_USB_DEVICE_STACK */

#include "zb_range_extender.h"
#include "air_quality_monitor.h"

/* Manufacturer name (32 bytes). */
#define ZIGBEE_MANUF_NAME "DIY"

/* Model number assigned by manufacturer (32-bytes long string). */
#define ZIGBEE_MODEL_ID "AirQualityMonitor_v1.0"

/* First 8 bytes specify the date of manufacturer of the device
 * in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific.
 */
#define ZIGBEE_DATE_CODE "20240722"

/* Air quality check period */
#define AIR_QUALITY_CHECK_PERIOD_MSEC (1000 * CONFIG_AIR_MONITOR_CHECK_PERIOD_SECONDS)

/* Delay for first air quality check */
#define AIR_QUALITY_CHECK_INITIAL_DELAY_MSEC (1000 * CONFIG_FIRST_AIR_MONITOR_CHECK_DELAY_SECONDS)

/* Time of LED on state while blinking for identify mode */
#define IDENTIFY_LED_BLINK_TIME_MSEC 500

/* LED indicating that device successfully joined Zigbee network */
#define ZIGBEE_NETWORK_STATE_LED DK_LED2

/* LED used for device identification */
#define IDENTIFY_LED DK_LED1

/* Button used to enter the Identify mode. */
#define IDENTIFY_MODE_BUTTON DK_BTN1_MSK

/* Button to start Factory Reset */
#define FACTORY_RESET_BUTTON IDENTIFY_MODE_BUTTON

BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
	     "Console device is not ACM CDC UART device");
LOG_MODULE_REGISTER(app, CONFIG_ZIGBEE_AIR_QUALITY_MONITOR_LOG_LEVEL);

/* Stores all cluster-related attributes */
static struct zb_device_ctx dev_ctx;

/* Attributes setup */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list, &dev_ctx.basic_attr.zcl_version,
				     &dev_ctx.basic_attr.app_version,
				     &dev_ctx.basic_attr.stack_version,
				     &dev_ctx.basic_attr.hw_version, dev_ctx.basic_attr.mf_name,
				     dev_ctx.basic_attr.model_id, dev_ctx.basic_attr.date_code,
				     &dev_ctx.basic_attr.power_source,
				     dev_ctx.basic_attr.location_id, &dev_ctx.basic_attr.ph_env,
				     dev_ctx.basic_attr.sw_ver);

/* Declare attribute list for Identify cluster (client). */
ZB_ZCL_DECLARE_IDENTIFY_CLIENT_ATTRIB_LIST(identify_client_attr_list);

/* Declare attribute list for Identify cluster (server). */
ZB_ZCL_DECLARE_IDENTIFY_SERVER_ATTRIB_LIST(identify_server_attr_list,
					   &dev_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(temperature_measurement_attr_list,
					    &dev_ctx.temp_attrs.measure_value,
					    &dev_ctx.temp_attrs.min_measure_value,
					    &dev_ctx.temp_attrs.max_measure_value,
					    &dev_ctx.temp_attrs.tolerance);

ZB_ZCL_DECLARE_REL_HUMIDITY_MEASUREMENT_ATTRIB_LIST(humidity_measurement_attr_list,
						    &dev_ctx.humidity_attrs.measure_value,
						    &dev_ctx.humidity_attrs.min_measure_value,
						    &dev_ctx.humidity_attrs.max_measure_value,
						    &dev_ctx.humidity_attrs.tolerance);

ZB_ZCL_DECLARE_CONCENTRATION_MEASUREMENT_ATTRIB_LIST(concentration_measurement_attr_list,
						     &dev_ctx.concentration_attrs.measure_value,
						     &dev_ctx.concentration_attrs.min_measure_value,
						     &dev_ctx.concentration_attrs.max_measure_value,
						     &dev_ctx.concentration_attrs.tolerance);

/* Clusters setup */
ZB_HA_DECLARE_AIR_QUALITY_MONITOR_CLUSTER_LIST(air_quality_monitor_cluster_list, basic_attr_list,
					       identify_client_attr_list, identify_server_attr_list,
					       temperature_measurement_attr_list,
					       humidity_measurement_attr_list,
					       concentration_measurement_attr_list);

/* Endpoint setup (single) */
ZB_HA_DECLARE_AIR_QUALITY_MONITOR_EP(air_quality_monitor_ep, AIR_QUALITY_MONITOR_ENDPOINT_NB,
				     air_quality_monitor_cluster_list);

/* Device context */
ZBOSS_DECLARE_DEVICE_CTX_1_EP(air_quality_monitor_ctx, air_quality_monitor_ep);

static void mandatory_clusters_attr_init(void)
{
	/* Basic cluster attributes */
	dev_ctx.basic_attr.zcl_version = ZB_ZCL_VERSION;
	dev_ctx.basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE;
	dev_ctx.basic_attr.app_version = 0x01;
	dev_ctx.basic_attr.stack_version = 0x03;
	dev_ctx.basic_attr.hw_version = 0x01;

	/* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte
	 * should contain string length without trailing zero.
	 *
	 * For example "test" string will be encoded as:
	 *   [(0x4), 't', 'e', 's', 't']
	 */
	ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.mf_name, ZIGBEE_MANUF_NAME,
			      ZB_ZCL_STRING_CONST_SIZE(ZIGBEE_MANUF_NAME));

	ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.model_id, ZIGBEE_MODEL_ID,
			      ZB_ZCL_STRING_CONST_SIZE(ZIGBEE_MODEL_ID));

	ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.date_code, ZIGBEE_DATE_CODE,
			      ZB_ZCL_STRING_CONST_SIZE(ZIGBEE_DATE_CODE));

	/* Identify cluster attributes */
	dev_ctx.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
}

static void measurements_clusters_attr_init(void)
{
	/* Temperature */
	dev_ctx.temp_attrs.measure_value = ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_UNKNOWN;
	dev_ctx.temp_attrs.min_measure_value = ZB_ZCL_TEMP_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE;
	dev_ctx.temp_attrs.max_measure_value = ZB_ZCL_TEMP_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE;
	dev_ctx.temp_attrs.tolerance = ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_MAX_VALUE;

	/* Humidity */
	dev_ctx.humidity_attrs.measure_value = ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_UNKNOWN;
	dev_ctx.humidity_attrs.min_measure_value =
		ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE;
	dev_ctx.humidity_attrs.max_measure_value =
		ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE;
	dev_ctx.humidity_attrs.tolerance = ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_TOLERANCE_MAX_VALUE;

	/* CO2 */
	dev_ctx.concentration_attrs.measure_value =
		ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_UNKNOWN;
	dev_ctx.concentration_attrs.min_measure_value =
		ZB_ZCL_CONCENTRATION_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE;
	dev_ctx.concentration_attrs.max_measure_value =
		ZB_ZCL_CONCENTRATION_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE; // 10 000ppm
	dev_ctx.concentration_attrs.tolerance = 0.0001f; // 100 ppm
}

/**@brief Function to toggle the identify LED
 *
 * @param  bufid  Unused parameter, required by ZBOSS scheduler API.
 */
static void toggle_identify_led(zb_bufid_t bufid)
{
	static int blink_status;

	dk_set_led(IDENTIFY_LED, (++blink_status) % 2);
	ZB_SCHEDULE_APP_ALARM(toggle_identify_led, bufid, ZB_MILLISECONDS_TO_BEACON_INTERVAL(100));
}

/**@brief Function to handle identify notification events on the first endpoint.
 *
 * @param  bufid  Unused parameter, required by ZBOSS scheduler API.
 */
static void identify_cb(zb_bufid_t bufid)
{
	zb_ret_t zb_err_code;

	if (bufid) {
		/* Schedule a self-scheduling function that will toggle the LED */
		ZB_SCHEDULE_APP_CALLBACK(toggle_identify_led, bufid);
	} else {
		/* Cancel the toggling function alarm and turn off LED */
		zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(toggle_identify_led, ZB_ALARM_ANY_PARAM);
		ZVUNUSED(zb_err_code);

		dk_set_led(IDENTIFY_LED, 0);
	}
}

/**@brief Starts identifying the device.
 *
 * @param  bufid  Unused parameter, required by ZBOSS scheduler API.
 */
static void start_identifying(zb_bufid_t bufid)
{
	ZVUNUSED(bufid);

	if (ZB_JOINED()) {
		/* Check if endpoint is in identifying mode,
		 * if not put desired endpoint in identifying mode.
		 */
		if (dev_ctx.identify_attr.identify_time ==
		    ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE) {
			zb_ret_t zb_err_code =
				zb_bdb_finding_binding_target(AIR_QUALITY_MONITOR_ENDPOINT_NB);

			if (zb_err_code == RET_OK) {
				LOG_INF("Enter identify mode");
			} else if (zb_err_code == RET_INVALID_STATE) {
				LOG_WRN("RET_INVALID_STATE - Cannot enter identify mode");
			} else {
				ZB_ERROR_CHECK(zb_err_code);
			}
		} else {
			LOG_INF("Cancel identify mode");
			zb_bdb_finding_binding_target_cancel();
		}
	} else {
		LOG_WRN("Device not in a network - cannot enter identify mode");
	}
}

/**@brief Callback for button events.
 *
 * @param[in]   button_state  Bitmask containing buttons state.
 * @param[in]   has_changed   Bitmask containing buttons
 *                            that have changed their state.
 */
static void button_changed(uint32_t button_state, uint32_t has_changed)
{
	if (IDENTIFY_MODE_BUTTON & has_changed) {
		if (IDENTIFY_MODE_BUTTON & button_state) {
			/* Button changed its state to pressed */
		} else {
			/* Button changed its state to released */
			if (was_factory_reset_done()) {
				/* The long press was for Factory Reset */
				LOG_DBG("After Factory Reset - ignore button release");
			} else {
				/* Button released before Factory Reset */

				/* Start identification mode */
				ZB_SCHEDULE_APP_CALLBACK(start_identifying, 0);
			}
		}
	}

	check_factory_reset_button(button_state, has_changed);
}

/**@brief Function for initializing LEDs and Buttons. */
static void gpio_init(void)
{
	int err;

	err = dk_buttons_init(button_changed);
	if (err) {
		LOG_ERR("Cannot init buttons (err: %d)", err);
	}

	err = dk_leds_init();
	if (err) {
		LOG_ERR("Cannot init LEDs (err: %d)", err);
	}
}

static void check_air_quality(zb_bufid_t bufid)
{
	ZVUNUSED(bufid);

	int err = air_quality_monitor_check_air_quality();

	if (err) {
		LOG_ERR("Failed to check air quality: %d", err);
	} else {
		err = air_quality_monitor_update_temperature();
		if (err) {
			LOG_ERR("Failed to update temperature: %d", err);
		}

		err = air_quality_monitor_update_humidity();
		if (err) {
			LOG_ERR("Failed to update humidity: %d", err);
		}

		err = air_quality_monitor_update_co2();
		if (err) {
			LOG_ERR("Failed to update co2: %d", err);
		}
	}

	zb_ret_t zb_err = ZB_SCHEDULE_APP_ALARM(
		check_air_quality, 0,
		ZB_MILLISECONDS_TO_BEACON_INTERVAL(AIR_QUALITY_CHECK_PERIOD_MSEC));
	if (zb_err) {
		LOG_ERR("Failed to schedule app alarm: %d", zb_err);
	}
}

void zboss_signal_handler(zb_bufid_t bufid)
{
	zb_zdo_app_signal_hdr_t *signal_header = NULL;
	zb_zdo_app_signal_type_t signal = zb_get_app_signal(bufid, &signal_header);
	zb_ret_t err = RET_OK;

	zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

	/* Detect ZBOSS startup */
	switch (signal) {
	case ZB_ZDO_SIGNAL_SKIP_STARTUP:
		/* ZBOSS framework has started - schedule first air quality check */
		err = ZB_SCHEDULE_APP_ALARM(
			check_air_quality, 0,
			ZB_MILLISECONDS_TO_BEACON_INTERVAL(AIR_QUALITY_CHECK_INITIAL_DELAY_MSEC));
		if (err) {
			LOG_ERR("Failed to schedule app alarm: %d", err);
		}
		break;
	default:
		break;
	}

	/* Let default signal handler process the signal*/
	ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));

	/*
	 * All callbacks should either reuse or free passed buffers.
	 * If bufid == 0, the buffer is invalid (not passed).
	 */
	if (bufid) {
		zb_buf_free(bufid);
	}
}

void main_usb_init()
{
	if (usb_enable(NULL) != 0) {
		LOG_ERR("Failed to enable USB");
	}
}

void main(void)
{
#if defined(CONFIG_USB_DEVICE_STACK)
	main_usb_init();
#endif

	gpio_init();
	register_factory_reset_button(FACTORY_RESET_BUTTON);

	air_quality_monitor_init();

	/* Register device context (endpoint) */
	ZB_AF_REGISTER_DEVICE_CTX(&air_quality_monitor_ctx);

	/* Init Basic and Identify attributes */
	mandatory_clusters_attr_init();

	/* Init measurements-related attributes */
	measurements_clusters_attr_init();

	/* Register callback to identify notifications */
	ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(AIR_QUALITY_MONITOR_ENDPOINT_NB, identify_cb);

	/* Enable Sleepy End Device behavior */
	zb_set_rx_on_when_idle(ZB_FALSE);
	if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
		power_down_unused_ram();
	}

	//zb_bdb_set_legacy_device_support(-1);

	/* Start Zigbee stack */
	zigbee_enable();

	/* Start identification mode */
	zb_ret_t err = ZB_SCHEDULE_APP_CALLBACK(start_identifying, 0);

	if (err) {
		LOG_ERR("Failed to schedule app callback: %d", err);
	}
}
