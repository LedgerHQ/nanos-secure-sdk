/**
  ******************************************************************************
  * @file    usbd_ccid_if.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    31-January-2014
  * @brief   This file provides all the functions prototypes for USB CCID
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
#ifndef __USBD_CCID_IF_H
#define __USBD_CCID_IF_H

#include "usbd_core.h"

#ifdef HAVE_USB_CLASS_CCID


/* Exported defines ----------------------------------------------------------*/
/* Bulk-only Command Block Wrapper */
//#define ABDATA_SIZE 261
#define CCID_HEADER_SIZE 10


#define CCID_INT_BUFF_SIZ 2

#define CARD_SLOT_FITTED  1
#define CARD_SLOT_REMOVED  0

#define BULK_MAX_PACKET_SIZE 0x40
#define CCID_IN_EP_SIZE   0x40
#define INTR_MAX_PACKET_SIZE 8
#define CCID_MESSAGE_HEADER_SIZE 10
#define CCID_NUMBER_OF_SLOTS     1       
                         /* Number of SLOTS. For single card, this value is 1 */

/* Following Parameters used in PC_to_RDR_IccPowerOn */
#define VOLTAGE_SELECTION_AUTOMATIC  0xFF
#define VOLTAGE_SELECTION_3V  0x02
#define VOLTAGE_SELECTION_5V  0x01
#define VOLTAGE_SELECTION_1V8 0x03

#define		PC_TO_RDR_ICCPOWERON		0x62
#define		PC_TO_RDR_ICCPOWEROFF		0x63
#define		PC_TO_RDR_GETSLOTSTATUS		0x65
#define		PC_TO_RDR_XFRBLOCK	        0x6F
#define		PC_TO_RDR_GETPARAMETERS		0x6C
#define		PC_TO_RDR_RESETPARAMETERS	0x6D
#define		PC_TO_RDR_SETPARAMETERS		0x61
#define		PC_TO_RDR_ESCAPE		0x6B
#define		PC_TO_RDR_ICCCLOCK		0x6E
#define		PC_TO_RDR_T0APDU		0x6A
#define		PC_TO_RDR_SECURE		0x69
#define		PC_TO_RDR_MECHANICAL		0x71
#define		PC_TO_RDR_ABORT			0x72
#define		PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY		0x73

#define		RDR_TO_PC_DATABLOCK		0x80
#define		RDR_TO_PC_SLOTSTATUS		0x81
#define		RDR_TO_PC_PARAMETERS		0x82
#define		RDR_TO_PC_ESCAPE		0x83
#define		RDR_TO_PC_DATARATEANDCLOCKFREQUENCY		0x84

#define		RDR_TO_PC_NOTIFYSLOTCHANGE	0x50
#define		RDR_TO_PC_HARDWAREERROR		0x51

#define         OFFSET_INT_BMESSAGETYPE  0
#define         OFFSET_INT_BMSLOTICCSTATE 1
#define         SLOT_ICC_PRESENT 0x01  
                /* LSb : (0b = no ICC present, 1b = ICC present) */

#define         SLOT_ICC_CHANGE  0x02  /* MSb : (0b = no change, 1b = change) */
/*****************************************************************************/
/*********************** CCID Bulk Transfer State machine ********************/
/*****************************************************************************/
#define CCID_STATE_IDLE                      0       /* Idle state */
#define CCID_STATE_DATA_OUT                  1       /* Data Out state */
#define CCID_STATE_RECEIVE_DATA              2
#define CCID_STATE_SEND_RESP                 3
#define CCID_STATE_DATAIN                    4
#define CCID_STATE_UNCORRECT_LENGTH          5

#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

/* Exported types ------------------------------------------------------------*/
#pragma pack(1)
typedef union ccid_bulk_header_u {
  #pragma pack(1)
  struct {
    uint8_t bMessageType; /* Offset = 0*/
    uint32_t dwLength;    /* Offset = 1, The length field (dwLength) is the length  
                            of the message not including the 10-byte header.*/
    uint8_t bSlot;        /* Offset = 5*/
    uint8_t bSeq;         /* Offset = 6*/
    uint8_t bSpecific_0;  /* Offset = 7*/
    uint8_t bSpecific_1;  /* Offset = 8*/
    uint8_t bSpecific_2;  /* Offset = 9*/
  } bulkout;
  #pragma pack(1)
  struct {
    uint8_t bMessageType;   /* Offset = 0  Same as Bulk OUT*/
    uint32_t dwLength;      /* Offset = 1  Same as Bulk OUT*/
    uint8_t bSlot;          /* Offset = 5, Same as Bulk-OUT message */ 
    uint8_t bSeq;           /* Offset = 6, Same as Bulk-OUT message */
    uint8_t bStatus;        /* Offset = 7, Slot status as defined in § 6.2.6*/
    uint8_t bError;         /* Offset = 8, Slot error  as defined in § 6.2.6*/
    uint8_t bSpecific;      /* Offset = 9*/
  } bulkin;
} ccid_bulk_header_t;

#pragma pack(1)
typedef struct 
{ 
  ccid_bulk_header_t header;
} Ccid_bulk_data_t; 
#pragma pack()


#pragma pack()

typedef struct 
{ 
  __IO uint8_t SlotStatus;
  __IO uint8_t SlotStatusChange;
} Ccid_SlotStatus_t; 


typedef struct 
{ 
  __IO uint8_t bAbortRequestFlag; 
  __IO uint8_t bSeq; 
  __IO uint8_t bSlot;
} usb_ccid_param_t; 


#pragma pack(1)
typedef struct _Protocol0_DataStructure_t
{
  uint8_t bmFindexDindex;
  uint8_t bmTCCKST0;
  uint8_t bGuardTimeT0;
  uint8_t bWaitingIntegerT0;
  uint8_t bClockStop;
} Protocol0_DataStructure_t;
#pragma pack()

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint8_t voltage;  /* Voltage for the Card Already Selected */
  uint8_t USART_GuardTime;
  uint8_t SC_A2R_FiDi;
  uint8_t SC_hostFiDi;
  uint8_t USART_DefaultGuardTime;
  uint32_t USART_BaudRate;
} SC_Param_t;

/* Includes ------------------------------------------------------------------*/
#include "usbd_ccid_core.h"


typedef struct usb_class_ccid_s {
  uint8_t Ccid_BulkState;
  uint8_t ccid_card_inserted;
  #ifdef HAVE_CCID_INTERRUPT
  uint8_t UsbIntMessageBuffer[INTR_MAX_PACKET_SIZE];  /* data buffer*/
  __IO uint8_t PrevXferComplete_IntrIn;
  #endif // HAVE_CCID_INTERRUPT
  usb_ccid_param_t usb_ccid_param;

  uint8_t* pUsbMessageBuffer;
  uint32_t UsbMessageLength;
  Ccid_SlotStatus_t Ccid_SlotStatus;
  Protocol0_DataStructure_t Protocol0_DataStructure;
  //Ccid_bulk_data_t Ccid_bulk_data;
  ccid_bulk_header_t bulk_header;

  SC_Param_t SC_Param;
} usb_class_ccid_t;
extern usb_class_ccid_t G_io_ccid;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
void CCID_BulkMessage_In (USBD_HandleTypeDef  *pdev, 
                     uint8_t epnum);

void CCID_BulkMessage_Out (USBD_HandleTypeDef  *pdev, 
                            uint8_t epnum, uint8_t* buffer, uint16_t buflen);

void CCID_ReceiveCmdHeader(uint8_t* pDst, uint8_t u8length);
void CCID_CmdDecode(USBD_HandleTypeDef  *pdev);

void CCID_IntMessage(USBD_HandleTypeDef  *pdev);
void CCID_Init(USBD_HandleTypeDef  *pdev);
void CCID_DeInit(USBD_HandleTypeDef  *pdev);

uint8_t CCID_IsIntrTransferComplete(void);
void CCID_SetIntrTransferStatus (uint8_t );
void Transfer_Data_Request(void);
void Set_CSW (uint8_t CSW_Status, uint8_t Send_Permission);

void io_usb_ccid_set_card_inserted(unsigned int inserted);

#endif // HAVE_USB_CLASS_CCID

#endif /* __USBD_CCID_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
