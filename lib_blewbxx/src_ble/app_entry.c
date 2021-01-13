/**
 ******************************************************************************
 * @file    app_entry.c
 * @author  MCD Application Team
 * @brief   Entry point of the Application
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


/* Includes ------------------------------------------------------------------*/
#include "app_common.h"

#include "app_entry.h"

#include "app_ble.h"

#include "ble.h"
#include "tl.h"

#include "scheduler.h"

#include "lpm.h"



/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define POOL_SIZE (CFG_TLBLE_EVT_QUEUE_LENGTH*4*DIVC(( sizeof(TL_PacketHeader_t) + TL_BLE_EVENT_FRAME_SIZE ), 4))

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TL_CmdPacket_t BleCmdBuffer;
TL_CmdPacket_t SystemCmdBuffer;

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/*static void appe_Tl_Init( void );
static void Led_Init( void );
static void Button_Init( void );*/


void Balenos_Tl_Init( void )
{
  // OTO: done on the MCU side
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
