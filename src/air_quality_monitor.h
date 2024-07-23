/*
 * Copyright (c) 2023 Jan Gnip
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef AIR_QUALITY_MONITOR_H
#define AIR_QUALITY_MONITOR_H

#include <zcl/zb_zcl_temp_measurement_addons.h>
#include <zcl/zb_zcl_basic_addons.h>

#include "zcl/zb_zcl_concentration_measurement.h"

/* Zigbee Cluster Library 4.4.2.2.1.1: MeasuredValue = 100x temperature in degrees Celsius */
#define ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER 100
/* Zigbee Cluster Library 4.7.2.1.1: MeasuredValue = 100x water content in % */
#define ZCL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_MULTIPLIER 100
#define ZCL_CO2_MEASUREMENT_MEASURED_VALUE_MULTIPLIER 0.000001

/* Measurements ranges scaled for attribute values */
#define AIR_QUALITY_MONITOR_ATTR_TEMP_MIN ( \
	SENSOR_TEMP_CELSIUS_MIN *               \
	ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER)
#define AIR_QUALITY_MONITOR_ATTR_TEMP_MAX ( \
	SENSOR_TEMP_CELSIUS_MAX *               \
	ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER)
#define AIR_QUALITY_MONITOR_ATTR_TEMP_TOLERANCE ( \
	SENSOR_TEMP_CELSIUS_TOLERANCE *               \
	ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER)
#define AIR_QUALITY_MONITOR_ATTR_HUMIDITY_MIN ( \
	SENSOR_HUMIDITY_PERCENT_MIN *               \
	ZCL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_MULTIPLIER)
#define AIR_QUALITY_MONITOR_ATTR_HUMIDITY_MAX ( \
	SENSOR_HUMIDITY_PERCENT_MAX *               \
	ZCL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_MULTIPLIER)

/* Number chosen for the single endpoint provided by air quality monitor */
#define AIR_QUALITY_MONITOR_ENDPOINT_NB 1

/* Temperature sensor device version */
#define ZB_HA_DEVICE_VER_TEMPERATURE_SENSOR 0
/* Basic, identify, temperature, humidity, measurement */
#define ZB_HA_AIR_QUALITY_MONITOR_IN_CLUSTER_NUM 5
/* Identify */
#define ZB_HA_AIR_QUALITY_MONITOR_OUT_CLUSTER_NUM 1

/* Temperature, humidity, co2, ???linkquality??? */
#define ZB_HA_AIR_QUALITY_MONITOR_REPORT_ATTR_COUNT 4

#define ZB_HA_DECLARE_AIR_QUALITY_MONITOR_CLUSTER_LIST(                              \
	cluster_list_name,                                                               \
	basic_attr_list,                                                                 \
	identify_client_attr_list,                                                       \
	identify_server_attr_list,                                                       \
	temperature_measurement_attr_list,                                               \
	humidity_measurement_attr_list,                                                  \
	concentration_measurement_attr_list)                                             \
	zb_zcl_cluster_desc_t cluster_list_name[] =                                      \
		{                                                                            \
			ZB_ZCL_CLUSTER_DESC(                                                     \
				ZB_ZCL_CLUSTER_ID_BASIC,                                             \
				ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                   \
				(basic_attr_list),                                                   \
				ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
				ZB_ZCL_MANUF_CODE_INVALID),                                          \
			ZB_ZCL_CLUSTER_DESC(                                                     \
				ZB_ZCL_CLUSTER_ID_IDENTIFY,                                          \
				ZB_ZCL_ARRAY_SIZE(identify_server_attr_list, zb_zcl_attr_t),         \
				(identify_server_attr_list),                                         \
				ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
				ZB_ZCL_MANUF_CODE_INVALID),                                          \
			ZB_ZCL_CLUSTER_DESC(                                                     \
				ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                                  \
				ZB_ZCL_ARRAY_SIZE(temperature_measurement_attr_list, zb_zcl_attr_t), \
				(temperature_measurement_attr_list),                                 \
				ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
				ZB_ZCL_MANUF_CODE_INVALID),                                          \
			ZB_ZCL_CLUSTER_DESC(                                                     \
				ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,                          \
				ZB_ZCL_ARRAY_SIZE(humidity_measurement_attr_list, zb_zcl_attr_t),    \
				(humidity_measurement_attr_list),                                    \
				ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
				ZB_ZCL_MANUF_CODE_INVALID),                                          \
			ZB_ZCL_CLUSTER_DESC(                                                     \
				ZB_ZCL_CLUSTER_ID_CONCENTRATION_MEASUREMENT,                          \
				ZB_ZCL_ARRAY_SIZE(concentration_measurement_attr_list, zb_zcl_attr_t),    \
				(concentration_measurement_attr_list),                                    \
				ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
				ZB_ZCL_MANUF_CODE_INVALID),                                          \
			ZB_ZCL_CLUSTER_DESC(                                                     \
				ZB_ZCL_CLUSTER_ID_IDENTIFY,                                          \
				ZB_ZCL_ARRAY_SIZE(identify_client_attr_list, zb_zcl_attr_t),         \
				(identify_client_attr_list),                                         \
				ZB_ZCL_CLUSTER_CLIENT_ROLE,                                          \
				ZB_ZCL_MANUF_CODE_INVALID),                                          \
	}

#define ZB_ZCL_DECLARE_AIR_QUALITY_MONITOR_DESC(            \
	ep_name,                                                \
	ep_id,                                                  \
	in_clust_num,                                           \
	out_clust_num)                                          \
	ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);    \
	ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)     \
	simple_desc_##ep_name =                                 \
		{                                                   \
			ep_id,                                          \
			ZB_AF_HA_PROFILE_ID,                            \
			ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID,             \
			ZB_HA_DEVICE_VER_TEMPERATURE_SENSOR,            \
			0,                                              \
			in_clust_num,                                   \
			out_clust_num,                                  \
			{                                               \
				ZB_ZCL_CLUSTER_ID_BASIC,                    \
				ZB_ZCL_CLUSTER_ID_IDENTIFY,                 \
				ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,         \
				ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, \
				ZB_ZCL_CLUSTER_ID_CONCENTRATION_MEASUREMENT,\
				ZB_ZCL_CLUSTER_ID_IDENTIFY,                 \
			}}

#define ZB_HA_DECLARE_AIR_QUALITY_MONITOR_EP(ep_name, ep_id, cluster_list) \
	ZB_ZCL_DECLARE_AIR_QUALITY_MONITOR_DESC(                               \
		ep_name,                                                           \
		ep_id,                                                             \
		ZB_HA_AIR_QUALITY_MONITOR_IN_CLUSTER_NUM,                          \
		ZB_HA_AIR_QUALITY_MONITOR_OUT_CLUSTER_NUM);                        \
	ZBOSS_DEVICE_DECLARE_REPORTING_CTX(                                    \
		reporting_info##ep_name,                                           \
		ZB_HA_AIR_QUALITY_MONITOR_REPORT_ATTR_COUNT);                      \
	ZB_AF_DECLARE_ENDPOINT_DESC(                                           \
		ep_name,                                                           \
		ep_id,                                                             \
		ZB_AF_HA_PROFILE_ID,                                               \
		0,                                                                 \
		NULL,                                                              \
		ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),            \
		cluster_list,                                                      \
		(zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,                 \
		ZB_HA_AIR_QUALITY_MONITOR_REPORT_ATTR_COUNT, reporting_info##ep_name, 0, NULL)

struct zb_zcl_humidity_measurement_attrs_t
{
	zb_int16_t measure_value;
	zb_int16_t min_measure_value;
	zb_int16_t max_measure_value;
};

struct zb_zcl_concentration_measurement_attrs_t
{
	zb_int16_t measure_value;
	zb_int16_t min_measure_value;
	zb_int16_t max_measure_value;
	zb_uint16_t tolerance;
};

struct zb_device_ctx
{
	zb_zcl_basic_attrs_ext_t basic_attr;
	zb_zcl_identify_attrs_t identify_attr;
	zb_zcl_temp_measurement_attrs_t temp_attrs;
	struct zb_zcl_humidity_measurement_attrs_t humidity_attrs;
	struct zb_zcl_concentration_measurement_attrs_t concentration_attrs;
};

/**
 * @brief Initializes HW sensor used for performing measurements.
 *
 * @note Has to be called before other functions can be used.
 *
 * @return void
 */
void air_quality_monitor_init(void);

/**
 * @brief Updates internal measurements performed by sensor.
 *
 * @note It has to be called each time a fresh measurements are required.
 *	 It does not change any ZCL attributes.
 *
 * @return 0 if success, error code if failure.
 */
int air_quality_monitor_check_air_quality(void);

/**
 * @brief Updates ZCL temperature attribute using value obtained during last air quality check.
 *
 * @return 0 if success, error code if failure.
 */
int air_quality_monitor_update_temperature(void);

/**
 * @brief Updates ZCL relative humidity attribute using value obtained during last air quality check.
 *
 * @return 0 if success, error code if failure.
 */
int air_quality_monitor_update_humidity(void);

/**
 * @brief Updates ZCL co2 attribute using value obtained during last air quality check.
 *
 * @return 0 if success, error code if failure.
 */
int air_quality_monitor_update_co2(void);

#endif /* AIR_QUALITY_MONITOR_H */
