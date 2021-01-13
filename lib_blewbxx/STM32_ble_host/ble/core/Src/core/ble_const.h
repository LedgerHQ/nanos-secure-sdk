/*****************************************************************************
 * @file    ble_const.h
 * @author  MCD Application Team
 * @brief   This file contains the definitions which are compiler dependent.
 *****************************************************************************
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

#ifndef BLE_CONST_H__
#define BLE_CONST_H__


#include "hal_types.h"


/* Return status */
#define BLE_STATUS_SUCCESS             (0x00)
#define BLE_STATUS_ERROR               (0x47)
#define BLE_STATUS_TIMEOUT             (0xFF)


/* Maximum payload of HCI commands that can be sent.
 * Change this value if needed. This value can be up to 255. */
#define HCI_MAX_PAYLOAD_SIZE              128


struct hci_request 
{
  uint16_t ogf;
  uint16_t ocf;
  int      event;
  void*    cparam;
  int      clen;
  void*    rparam;
  int      rlen;
};

extern int hci_send_req( struct hci_request *req, BOOL async );


/* Byte order conversions */
#define htob( d, n )  (d)     /* LE */
#define btoh( d, n )  (d)     /* LE */


#endif /* ! BLE_CONST_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE***/
