/*
 * Copyright (c) 2023 Jan Gnip
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/logging/log.h>

#include "sensor.h"

LOG_MODULE_DECLARE(app, CONFIG_ZIGBEE_AIR_QUALITY_MONITOR_LOG_LEVEL);

int sensor_init(void)
{
	LOG_WRN("Dummy sensor initialized");
	return 0;
}

int sensor_update_measurements(void)
{
	LOG_WRN("Dummy sensor update measurements");
	return 0;
}

int sensor_get_temperature(float *temperature)
{
	int err = 0;

	if (temperature)
	{
		*temperature = 25.0;
	}
	else
	{
		LOG_ERR("NULL param");
		err = EINVAL;
	}

	return err;
}

int sensor_get_humidity(float *humidity)
{
	int err = 0;

	if (humidity)
	{
		*humidity = 55.0;
	}
	else
	{
		LOG_ERR("NULL param");
		err = EINVAL;
	}

	return err;
}
