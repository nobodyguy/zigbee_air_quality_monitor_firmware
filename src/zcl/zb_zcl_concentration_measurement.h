/*
 * Copyright (c) 2024 Jan Gnip
 * ZBOSS Zigbee 3.0
 *
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 *
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* PURPOSE: Concentration Measurement cluster defintions
*/

#ifndef ZB_ZCL_CONCENTRATION_MEASUREMENT_H
#define ZB_ZCL_CONCENTRATION_MEASUREMENT_H 1

#include <zboss_api.h>
#include <zboss_api_addons.h>

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_WATER_CONTENT_MEASUREMENT
 *  @{
 */

/* Cluster ZB_ZCL_WATER_CONTENT_MEASUREMENT */

/*! @name Water Content Measurement cluster attributes
    @{
*/

/*! @brief Water Content Measurement cluster attribute identifiers
    @see ZCL specification revision 7, Concentration MeasuremenWater Content Measurement Cluster 4.7.2.1
*/
enum zb_zcl_concentration_measurement_attr_e
{
  /** @brief MeasuredValue, ZCL specification revision 7 subsection 4.7.2.1.1 MeasuredValue Attribute */
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_ID     = 0x0000,
  /** @brief MinMeasuredValue, ZCL specification revision 7 subsection 4.7.2.1.2 MinMeasuredValue Attribute*/
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MIN_VALUE_ID = 0x0001,
  /** @brief MaxMeasuredValue, ZCL specification revision 7 subsection 4.7.2.1.3 MaxMeasuredValue Attribute*/
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MAX_VALUE_ID = 0x0002,
  /** The Tolerance attribute SHALL indicate the magnitude of the
   *  possible error that is associated with MeasuredValue, using
   *  the same units and resolution.
   *  @brief Tolerance, ZCL specification revision 7 subsection 4.7.2.1.4 Tolerance Attribute
   */
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_TOLERANCE_ID             = 0x0003,
};

/** @brief CO2 cluster ID, ZCL specification revision 8 subsection 4.13.1.3 Cluster Identifiers */
#define ZB_ZCL_CLUSTER_ID_CONCENTRATION_MEASUREMENT 0x040D

/** @brief Default value for Concentration cluster revision global attribute */
#define ZB_ZCL_CONCENTRATION_MEASUREMENT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief MeasuredValue attribute unknown value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_UNKNOWN        ((zb_uint32_t)0xFFFF)

/** @brief MinMeasuredValue attribute minimum value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MIN_VALUE_MIN_VALUE  0x0000

/** @brief MinMeasuredValue attribute maximum value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MIN_VALUE_MAX_VALUE  0x270F

/** @brief MinMeasuredValue attribute undefined value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MIN_VALUE_UNDEFINED  0xFFFF

/** @brief MaxMeasuredValue attribute minimum value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MAX_VALUE_MIN_VALUE  0x0001

/** @brief MaxMeasuredValue attribute maximum value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MAX_VALUE_MAX_VALUE  0x2710

/** @brief MaxMeasuredValue attribute value not defined */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MAX_VALUE_UNDEFINED  0xFFFF

/** @brief Default value for MeasurementValue attribute */
#define ZB_ZCL_CONCENTRATION_MEASUREMENT_VALUE_DEFAULT_VALUE ((zb_uint32_t)0xFFFF)

/** @brief Default value for MeasurementMinValue attribute */
#define ZB_ZCL_CONCENTRATION_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE ((zb_uint32_t)0xFFFF)

/** @brief Default value for MeasurementMaxValue attribute */
#define ZB_ZCL_CONCENTRATION_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE ((zb_uint32_t)0xFFFF)

/** @brief Tolerance attribute minimum value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_TOLERANCE_MIN_VALUE            0x0000

/** @brief Tolerance attribute maximum value */
#define ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_TOLERANCE_MAX_VALUE            0x0800

/** @cond internals_doc */
/*! @internal @name Concentration Measurement cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_ID,                \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,  \
  (void*) data_ptr                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MIN_VALUE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MIN_VALUE_ID,    \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (void*) data_ptr                                 \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MAX_VALUE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MAX_VALUE_ID,    \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (void*) data_ptr                                 \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_TOLERANCE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_TOLERANCE_ID,    \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                         \
  (void*) data_ptr                                 \
}

#define ZB_ZCL_CONCENTRATION_MEASUREMENT_REPORT_ATTR_COUNT 1

/*! @} */ /* Concentration Measurement cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Declare attribute list for Concentration Measurement cluster - server side
    @param attr_list - attribute list name
    @param value - pointer to variable to store MeasuredValue attribute
    @param min_value - pointer to variable to store MinMeasuredValue attribute
    @param max_value - pointer to variable to store MAxMeasuredValue attribute
*/
#define ZB_ZCL_DECLARE_CONCENTRATION_MEASUREMENT_ATTRIB_LIST(attr_list,          \
    value, min_value, max_value, tolerance)                                                \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_WATER_CONTENT_MEASUREMENT) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_VALUE_ID, (value))          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MIN_VALUE_ID, (min_value))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_MAX_VALUE_ID, (max_value))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONCENTRATION_MEASUREMENT_TOLERANCE_ID, (tolerance))  \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Concentration Measurement cluster attributes */

/*! @name Concentration Measurement cluster commands
    @{
*/

/*! @} */ /* Concentration Measurement cluster commands */

/*! @} */ /* ZCL Concentration Measurement cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_concentration_init_server(void);
void zb_zcl_concentration_init_client(void);
#define ZB_ZCL_CLUSTER_ID_CONCENTRATION_MEASUREMENT_SERVER_ROLE_INIT zb_zcl_concentration_init_server
#define ZB_ZCL_CLUSTER_ID_CONCENTRATION_MEASUREMENT_CLIENT_ROLE_INIT zb_zcl_concentration_init_client

#endif /* ZB_ZCL_CONCENTRATION_MEASUREMENT_H */
