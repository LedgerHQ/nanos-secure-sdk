/**
  ******************************************************************************
  * @file    usbd_ccid_core.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    31-January-2014
  * @brief   This file provides all the CCID core functions.
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
#ifndef _USB_CCID_CORE_H_
#define _USB_CCID_CORE_H_

#include "os.h"
#include "os_io_seproxyhal.h"

#ifdef HAVE_USB_CLASS_CCID

/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_ccid_impl.h"
#include "sc_itf.h"

/* Exported defines ----------------------------------------------------------*/

#define REQUEST_ABORT                  0x01
#define REQUEST_GET_CLOCK_FREQUENCIES  0x02
#define REQUEST_GET_DATA_RATES         0x03


/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t  USBD_CCID_Init (USBD_HandleTypeDef  *pdev, 
                            uint8_t cfgidx);

uint8_t  USBD_CCID_DeInit (USBD_HandleTypeDef  *pdev, 
                              uint8_t cfgidx);

uint8_t  USBD_CCID_Setup (USBD_HandleTypeDef  *pdev, 
                             USBD_SetupReqTypedef *req);

uint8_t  USBD_CCID_DataIn (USBD_HandleTypeDef  *pdev, 
                              uint8_t epnum);

uint8_t  USBD_CCID_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum, uint8_t* buffer);

#endif // HAVE_USB_CLASS_CCID

#endif  /* _USB_CCID_CORE_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
