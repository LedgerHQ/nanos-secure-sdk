/**
  ******************************************************************************
  * @file    ble_gap.h
  * @author  AMS - VMA RF Application team
  * @version V1.0.0
  * @date    04-September-2018
  * @brief   Header file for BLE core's GAP layer constants
  ******************************************************************************
  * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */

#ifndef __GAP_H__
#define __GAP_H__

#include <link_layer.h>

/**
 *@addtogroup GAP GAP
 *@brief API for GAP layer.
 *@{
 */

/**
 * @name GAP UUIDs
 * @{
 */
#define GAP_SERVICE_UUID                        (0x1800)
#define DEVICE_NAME_UUID						(0x2A00)
#define APPEARANCE_UUID							(0x2A01)
#define PERIPHERAL_PRIVACY_FLAG_UUID			(0x2A02)
#define RECONNECTION_ADDR_UUID					(0x2A03)
#define PERIPHERAL_PREFERRED_CONN_PARAMS_UUID	(0x2A04)
/**
 * @}
 */

/**
 * @name Characteristic value lengths
 * @{
 */
#define DEVICE_NAME_CHARACTERISTIC_LEN			        (8)
#define APPEARANCE_CHARACTERISTIC_LEN			        (2)
#define PERIPHERAL_PRIVACY_CHARACTERISTIC_LEN	        (1)
#define RECONNECTION_ADDR_CHARACTERISTIC_LEN			(6)
#define PERIPHERAL_PREF_CONN_PARAMS_CHARACTERISTIC_LEN	(8)
/**
 * @}
 */

/*------------- AD types for adv data and scan response data ----------------*/

/**
 * @defgroup AD_Types AD Types
 * @brief AD Types
 * @{
 */

/* FLAGS AD type */
#define AD_TYPE_FLAGS							(0x01)
/* flag bits */
/**
 * @anchor Flags_AD_Type_bits
 * @name Flags AD Type bits
 * @brief Bits in Flags AD Type
 * @{
 */
#define FLAG_BIT_LE_LIMITED_DISCOVERABLE_MODE	(0x01)
#define FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE	(0x02)
#define FLAG_BIT_BR_EDR_NOT_SUPPORTED			(0x04)
#define FLAG_BIT_LE_BR_EDR_CONTROLLER			(0x08)
#define FLAG_BIT_LE_BR_EDR_HOST					(0x10)
/**
 * @}
 */

/**
 * @name Service UUID AD types
 * @{
 */
#define AD_TYPE_16_BIT_SERV_UUID				(0x02)
#define AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST		(0x03)
#define AD_TYPE_32_BIT_SERV_UUID				(0x04)
#define AD_TYPE_32_BIT_SERV_UUID_CMPLT_LIST		(0x05)
#define AD_TYPE_128_BIT_SERV_UUID				(0x06)
#define AD_TYPE_128_BIT_SERV_UUID_CMPLT_LIST	(0x07)
/**
 * @}
 */

/* LOCAL NAME AD types */
/**
 * @name Local name AD types
 * @{
 */
#define AD_TYPE_SHORTENED_LOCAL_NAME			(0x08)
#define AD_TYPE_COMPLETE_LOCAL_NAME			    (0x09)
/**
 * @}
 */

/* TX power level AD type*/
#define AD_TYPE_TX_POWER_LEVEL					(0x0A)

/* Class of device */
#define AD_TYPE_CLASS_OF_DEVICE					(0x0D)

/* security manager TK value AD type */
#define AD_TYPE_SEC_MGR_TK_VALUE				(0x10)

/* security manager OOB flags */
#define AD_TYPE_SEC_MGR_OOB_FLAGS				(0x11)

/* slave connection interval AD type */
#define AD_TYPE_SLAVE_CONN_INTERVAL				(0x12)

/* service solicitation UUID list Ad types*/
/**
 * @name Service solicitation UUID list AD types
 * @{
 */
#define AD_TYPE_SERV_SOLICIT_16_BIT_UUID_LIST	(0x14)
#define AD_TYPE_SERV_SOLICIT_32_BIT_UUID_LIST	(0x1F)
#define AD_TYPE_SERV_SOLICIT_128_BIT_UUID_LIST	(0x15)
/**
 * @}
 */

/* service data AD type */
#define AD_TYPE_SERVICE_DATA					(0x16)	

/* manufaturer specific data AD type */
#define AD_TYPE_MANUFACTURER_SPECIFIC_DATA		(0xFF)

/**
 * @}
 */

#define MAX_ADV_DATA_LEN						(31)

#define DEVICE_NAME_LEN			  				(7)
#define BD_ADDR_SIZE							(6)

/**
 * @name Privacy flag values
 * @{
 */
#define PRIVACY_ENABLED							(0x02)
#define PRIVACY_DISABLED						(0x00)
/**
 * @}
 */

/**
 * @name Intervals
 * Intervals in terms of 625 micro sec
 * @{
 */
#define DIR_CONN_ADV_INT_MIN					(0x190)/*250ms*/
#define DIR_CONN_ADV_INT_MAX					(0x320)/*500ms*/
#define UNDIR_CONN_ADV_INT_MIN					(0x800)/*1.28s*/
#define UNDIR_CONN_ADV_INT_MAX					(0x1000)/*2.56s*/
#define LIM_DISC_ADV_INT_MIN					(0x190)/*250ms*/
#define LIM_DISC_ADV_INT_MAX					(0x320)/*500ms*/
#define GEN_DISC_ADV_INT_MIN					(0x800)/*1.28s*/
#define GEN_DISC_ADV_INT_MAX					(0x1000)/*2.56s*/
/**
 * @}
 */

/**
 * @name Timeout values
 * @{
 */
#define LIM_DISC_MODE_TIMEOUT					(180000)/* 180 seconds. according to the errata published */
#define PRIVATE_ADDR_INT_TIMEOUT				(900000)/* 15 minutes */
/**
 * @}
 */

/**
 * @anchor gap_roles
 * @name GAP Roles
 * @{
*/
#define GAP_PERIPHERAL_ROLE						(0x01)
#define GAP_BROADCASTER_ROLE					        (0x02)
#define GAP_CENTRAL_ROLE						(0x04)
#define GAP_OBSERVER_ROLE						(0x08)

/**
 * @}
 */

/**
 * @anchor gap_procedure_codes
 * @name GAP procedure codes
 * Procedure codes for EVT_BLUE_GAP_PROCEDURE_COMPLETE event
 * and aci_gap_terminate_gap_procedure() command.
 * @{
 */
#define GAP_LIMITED_DISCOVERY_PROC                  (0x01)
#define GAP_GENERAL_DISCOVERY_PROC                  (0x02)
#define GAP_NAME_DISCOVERY_PROC                     (0x04)
#define GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC      (0x08)
#define GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC   (0x10)
#define GAP_SELECTIVE_CONNECTION_ESTABLISHMENT_PROC (0x20)
#define GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC    (0x40)
#define GAP_OBSERVATION_PROC                        (0x80)

/**
 * @}
 */

/**
 * @}
 */

#endif /* __GAP_H__ */
