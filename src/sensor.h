/*
 * Copyright (c) 2023 Jan Gnip
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <zephyr/drivers/sensor.h>

/* Measurements ranges for dummy sensor */
#define SENSOR_TEMP_CELSIUS_MIN (-40)
#define SENSOR_TEMP_CELSIUS_MAX (85)
#define SENSOR_TEMP_CELSIUS_TOLERANCE (1)
#define SENSOR_HUMIDITY_PERCENT_MIN (10)
#define SENSOR_HUMIDITY_PERCENT_MAX (90)
#define SENSOR_HUMIDITY_PERCENT_TOLERANCE (3)
#define SENSOR_CO2_PPM_MIN (0)
#define SENSOR_CO2_PPM_MAX (10000)
#define SENSOR_CO2_PPM_TOLERANCE (100)

/**
 * @brief Initializes dummy sensor.
 *
 * @note Has to be called before other functions are used.
 *
 * @return 0 if success, error code if failure.
 */
int sensor_init(void);

/**
 * @brief Updates and stores internally data measured by sensor.
 *
 * @note A single call updates temperature, humidity and co2 values at once.
 *
 * @return 0 if success, error code if failure.
 */
int sensor_update_measurements(void);

/**
 * @brief Provides last measured value of temperature.
 *
 * @note Call sensor_update_measurements() to update the value.
 *
 * @param temperature [out] temperature value in Celsiuss degrees.
 *
 * @return 0 if success, error code if failure.
 */
int sensor_get_temperature(float *temperature);

/**
 * @brief Provides last measured value of relative humidity.
 *
 * @note Call sensor_update_measurements() to update the value.
 *
 * @param humidity [out] relative humidity value in %.
 *
 * @return 0 if success, error code if failure.
 */
int sensor_get_humidity(float *humidity);

/**
 * @brief Provides last measured value of co2.
 *
 * @note Call sensor_update_measurements() to update the value.
 *
 * @param co2 [out] co2 concentration in ppm.
 *
 * @return 0 if success, error code if failure.
 */
int sensor_get_co2(uint16_t *co2);

#endif /* SENSOR_H */
