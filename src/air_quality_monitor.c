/*
 * Copyright (c) 2024 Jan Gnip
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <sensor/scd4x/scd4x.h>

#include "air_quality_monitor.h"

LOG_MODULE_DECLARE(app, CONFIG_ZIGBEE_AIR_QUALITY_MONITOR_LOG_LEVEL);

#if !DT_HAS_COMPAT_STATUS_OKAY(sensirion_scd4x)
#error "No sensirion,scd4x compatible node found in the device tree"
#endif

static const struct device *scd = DEVICE_DT_GET_ANY(sensirion_scd4x);

void air_quality_monitor_init(void)
{
	if (scd == NULL || device_is_ready(scd) == false) {
		LOG_ERR("Failed to initialize SCD4X device");
	}
}

int air_quality_monitor_check_air_quality(void)
{
	int err = sensor_sample_fetch(scd);

	if (err) {
		LOG_ERR("Failed to fetch sample from SCD4X device");
	}

	return err;
}

int air_quality_monitor_update_temperature(void)
{
	int err = 0;

	double measured_temperature = 0.0;
	int16_t temperature_attribute = 0;

	struct sensor_value sensor_value;
	err = sensor_channel_get(scd, SENSOR_CHAN_AMBIENT_TEMP, &sensor_value);
	measured_temperature = sensor_value_to_double(&sensor_value);
	if (err) {
		LOG_ERR("Failed to get sensor temperature: %d", err);
	} else {
		/* Convert measured value to attribute value, as specified in ZCL */
		temperature_attribute =
			(int16_t)(measured_temperature *
				  ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER);
		LOG_INF("Attribute T:%10d", temperature_attribute);

		/* Set ZCL attribute */
		zb_zcl_status_t status = zb_zcl_set_attr_val(
			AIR_QUALITY_MONITOR_ENDPOINT_NB, ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
			ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
			(zb_uint8_t *)&temperature_attribute, ZB_FALSE);
		if (status) {
			LOG_ERR("Failed to set ZCL attribute: %d", status);
			err = status;
		}
	}

	return err;
}

int air_quality_monitor_update_humidity(void)
{
	int err = 0;

	double measured_humidity = 0.0;
	int16_t humidity_attribute = 0;

	struct sensor_value sensor_value;
	err = sensor_channel_get(scd, SENSOR_CHAN_HUMIDITY, &sensor_value);
	measured_humidity = sensor_value_to_double(&sensor_value);
	if (err) {
		LOG_ERR("Failed to get sensor humidity: %d", err);
	} else {
		/* Convert measured value to attribute value, as specified in ZCL */
		humidity_attribute = (int16_t)(measured_humidity *
					       ZCL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_MULTIPLIER);
		LOG_INF("Attribute H:%10d", humidity_attribute);

		zb_zcl_status_t status = zb_zcl_set_attr_val(
			AIR_QUALITY_MONITOR_ENDPOINT_NB, ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
			ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID,
			(zb_uint8_t *)&humidity_attribute, ZB_FALSE);
		if (status) {
			LOG_ERR("Failed to set ZCL attribute: %d", status);
			err = status;
		}
	}

	return err;
}

int air_quality_monitor_update_co2(double* co2)
{
	int err = 0;

	double measured_co2 = 0.0;
	float co2_attribute = 0.0;

	struct sensor_value sensor_value;
	err = sensor_channel_get(scd, SENSOR_CHAN_CO2, &sensor_value);
	measured_co2 = sensor_value_to_double(&sensor_value);
	*co2 = measured_co2;
	if (err) {
		LOG_ERR("Failed to get sensor co2: %d", err);
	} else {
		/* Convert measured value to attribute value, as specified in ZCL */
		co2_attribute = measured_co2 * ZCL_CO2_MEASUREMENT_MEASURED_VALUE_MULTIPLIER;
		LOG_INF("Attribute CO2:%10f", co2_attribute);

		zb_zcl_status_t status =
			zb_zcl_set_attr_val(AIR_QUALITY_MONITOR_ENDPOINT_NB,
					    ZB_ZCL_CLUSTER_ID_CONCENTRATION_MEASUREMENT,
					    ZB_ZCL_CLUSTER_SERVER_ROLE,
					    ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_ID,
					    (zb_uint8_t *)&co2_attribute, ZB_FALSE);
		if (status) {
			LOG_ERR("Failed to set ZCL attribute: %d", status);
			err = status;
		}
	}

	return err;
}

int air_quality_monitor_calibrate(void)
{
	sensirion_scd4x_stop_periodic_measurement(scd);
	sensirion_scd4x_calibrate(scd);
	sensirion_scd4x_start_periodic_measurement(scd);
	return 0;
}
