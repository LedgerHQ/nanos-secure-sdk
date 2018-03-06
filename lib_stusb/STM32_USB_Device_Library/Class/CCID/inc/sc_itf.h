/**
  ******************************************************************************
  * @file    sc_itf.h  
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Evaluation board specific configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SC_ITF_H
#define __SC_ITF_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_ccid_if.h"

#ifdef HAVE_USB_CLASS_CCID

#include "usbd_ccid_cmd.h"


/* Exported macro ------------------------------------------------------------*/
#define MAX_EXTRA_GUARD_TIME (0xFF - DEFAULT_EXTRA_GUARDTIME)

 /* Following macros are used for SC_XferBlock command */
#define XFER_BLK_SEND_DATA     1     /* Command is for issuing the data  */
#define XFER_BLK_RECEIVE_DATA  2     /* Command is for receiving the data */
#define XFER_BLK_NO_DATA       3     /* Command type is No data exchange  */

/* Exported functions ------------------------------------------------------- */
/* APPLICATION LAYER ---------------------------------------------------------*/
#define SC_VOLTAGE_1_8V 1
#define SC_VOLTAGE_3V 2
#define SC_VOLTAGE_5V 3
uint8_t SC_AnswerToReset (uint8_t voltage, uint8_t* atr_buffer);
//uint8_t SC_GetState (void);
//void SC_SetState (uint8_t scState);
uint8_t SC_Detect(void);
void SC_Poweroff(void);
void SC_ConfigDetection (void);
void SC_SaveVoltage (uint8_t );
void SC_UpdateParams (void);
void SC_InitParams (void);
uint8_t SC_SetParams (Protocol0_DataStructure_t* );
uint8_t SC_ExecuteEscape (uint8_t* escapePtr, uint32_t escapeLen, 
                          uint8_t* responseBuff,
                          uint16_t* responseLen);
uint8_t SC_SetClock (uint8_t bClockCommand);
uint8_t SC_XferBlock (uint8_t* ptrBlock, uint32_t blockLen, uint16_t* expectedLen);
uint8_t SC_Request_GetClockFrequencies(uint8_t* pbuf, uint16_t* len);
uint8_t SC_Request_GetDataRates(uint8_t* pbuf, uint16_t* len);
uint8_t SC_T0Apdu(uint8_t bmChanges, uint8_t bClassGetResponse, 
                  uint8_t bClassEnvelope);
uint8_t SC_Mechanical(uint8_t bFunction);
uint8_t SC_SetDataRateAndClockFrequency(uint32_t dwClockFrequency, 
                                        uint32_t dwDataRate); 
uint8_t SC_Secure(uint32_t dwLength, uint8_t bBWI, uint16_t wLevelParameter, 
                    uint8_t* pbuf, uint32_t* returnLen );

#endif // HAVE_USB_CLASS_CCID

#endif /* __SC_ITF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
