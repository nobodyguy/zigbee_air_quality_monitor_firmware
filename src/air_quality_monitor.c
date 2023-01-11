/*
 * Copyright (c) 2023 Jan Gnip
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/logging/log.h>

#include "air_quality_monitor.h"

LOG_MODULE_DECLARE(app, CONFIG_ZIGBEE_AIR_QUALITY_MONITOR_LOG_LEVEL);

int air_quality_monitor_init(void)
{
	int err = sensor_init();

	if (err)
	{
		LOG_ERR("Failed to initialize sensor: %d", err);
	}

	return err;
}

int air_quality_monitor_check_air_quality(void)
{
	int err = sensor_update_measurements();

	if (err)
	{
		LOG_ERR("Failed to update sensor");
	}

	return err;
}

int air_quality_monitor_update_temperature(void)
{
	int err = 0;

	float measured_temperature = 0.0f;
	int16_t temperature_attribute = 0;

	err = sensor_get_temperature(&measured_temperature);
	if (err)
	{
		LOG_ERR("Failed to get sensor temperature: %d", err);
	}
	else
	{
		/* Convert measured value to attribute value, as specified in ZCL */
		temperature_attribute = (int16_t)(measured_temperature *
										  ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER);
		LOG_INF("Attribute T:%10d", temperature_attribute);

		/* Set ZCL attribute */
		zb_zcl_status_t status = zb_zcl_set_attr_val(AIR_QUALITY_MONITOR_ENDPOINT_NB,
													 ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
													 ZB_ZCL_CLUSTER_SERVER_ROLE,
													 ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
													 (zb_uint8_t *)&temperature_attribute,
													 ZB_FALSE);
		if (status)
		{
			LOG_ERR("Failed to set ZCL attribute: %d", status);
			err = status;
		}
	}

	return err;
}

int air_quality_monitor_update_humidity(void)
{
	int err = 0;

	float measured_humidity = 0.0f;
	int16_t humidity_attribute = 0;

	err = sensor_get_humidity(&measured_humidity);
	if (err)
	{
		LOG_ERR("Failed to get sensor humidity: %d", err);
	}
	else
	{
		/* Convert measured value to attribute value, as specified in ZCL */
		humidity_attribute = (int16_t)(measured_humidity *
									   ZCL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_MULTIPLIER);
		LOG_INF("Attribute H:%10d", humidity_attribute);

		zb_zcl_status_t status = zb_zcl_set_attr_val(
			AIR_QUALITY_MONITOR_ENDPOINT_NB,
			ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
			ZB_ZCL_CLUSTER_SERVER_ROLE,
			ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID,
			(zb_uint8_t *)&humidity_attribute,
			ZB_FALSE);
		if (status)
		{
			LOG_ERR("Failed to set ZCL attribute: %d", status);
			err = status;
		}
	}

	return err;
}

int air_quality_monitor_update_co2(void)
{
	int err = 0;

	uint16_t measured_co2 = 0;
	float co2_attribute = 0.0;

	err = sensor_get_co2(&measured_co2);
	if (err)
	{
		LOG_ERR("Failed to get sensor co2: %d", err);
	}
	else
	{
		co2_attribute = 0.0015;
		LOG_INF("Attribute CO2:%10f", co2_attribute);

		zb_zcl_status_t status = zb_zcl_set_attr_val(
			AIR_QUALITY_MONITOR_ENDPOINT_NB,
			ZB_ZCL_CLUSTER_ID_CONCENTRATION_MEASUREMENT,
			ZB_ZCL_CLUSTER_SERVER_ROLE,
			ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_ID,
			(zb_uint8_t *)&co2_attribute,
			ZB_FALSE);
		if (status)
		{
			LOG_ERR("Failed to set ZCL attribute: %d", status);
			err = status;
		}
	}

	return err;
}
