/**
  ******************************************************************************
  * @file    hw.h
  * @author  MCD Application Team
  * @brief   Hardware
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_H
#define __HW_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Includes ------------------------------------------------------------------*/


  /******************************************************************************
   * HW LOW POWER
   ******************************************************************************/
  /**
   * Stop Mode configuration
   * The values of enum shall be kept unchanged
   */
  typedef enum
  {
    hw_lpm_stopmode0,
    hw_lpm_stopmode1,
    hw_lpm_stopmode2,
  } HW_LPM_StopModeConf_t;

  /**
   * Off Mode configuration
   * The values of enum shall be kept unchanged
   */
  typedef enum
  {
    hw_lpm_standby,
    hw_lpm_shutdown,
  } HW_LPM_OffModeConf_t;

  void HW_LPM_SleepMode(void);
  void HW_LPM_StopMode(HW_LPM_StopModeConf_t configuration);
  void HW_LPM_OffMode(HW_LPM_OffModeConf_t configuration);


  /******************************************************************************
   * HW IPCC
   ******************************************************************************/
  void HW_IPCC_Enable( void );
  void HW_IPCC_Init( void );
  void HW_IPCC_Rx_Handler( void );
  void HW_IPCC_Tx_Handler( void );

  void HW_IPCC_BLE_Init( void );
  void HW_IPCC_BLE_SendCmd( void );
  void HW_IPCC_MM_SendFreeBuf( void (*cb)( void ) );
  void HW_IPCC_BLE_RxEvtNot( void );

  void HW_IPCC_SYS_Init( void );
  void HW_IPCC_SYS_SendCmd( void );
  void HW_IPCC_SYS_CmdEvtNot( void );
  void HW_IPCC_FWU_Init( void );
  void HW_IPCC_FWU_SendReq( void );
  void HW_IPCC_SYS_EvtNot( void );

  void HW_IPCC_THREAD_Init( void );
  void HW_IPCC_OT_SendCmd( void );
  void HW_IPCC_CLI_SendCmd( void );
  void HW_IPCC_THREAD_SendAck( void );
  void HW_IPCC_OT_CmdEvtNot( void );
  void HW_IPCC_CLI_CmdEvtNot( void );
  void HW_IPCC_THREAD_EvtNot( void );
  void HW_IPCC_THREAD_CliSendAck( void );
  void HW_IPCC_THREAD_CliEvtNot( void );

  void HW_IPCC_TRACES_Init( void );
  void HW_IPCC_TRACES_EvtNot( void );


#ifdef __cplusplus
}
#endif

#endif /*__HW_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
