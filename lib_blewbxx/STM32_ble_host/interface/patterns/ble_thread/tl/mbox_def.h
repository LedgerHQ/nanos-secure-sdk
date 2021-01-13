/**
 ******************************************************************************
 * @file    mbox_def.h
 * @author  MCD Application Team
 * @brief   Mailbox definition
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
#ifndef __MBOX_H
#define __MBOX_H

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * This file shall be identical between the CPU1 and the CPU2
   */

  /**
   *********************************************************************************
   * TABLES
   *********************************************************************************
   */

  /**
   * Version
   * [0:3]   = Build - 0: Untracked - 15:Released - x: Tracked version
   * [4:7]   = branch - 0: Mass Market - x: ...
   * [8:15]  = Subversion
   * [16:23] = Version minor
   * [24:31] = Version major
   *
   * Memory Size
   * [0:7]   = Flash ( Number of 4k sector)
   * [8:15]  = Reserved ( Shall be set to 0 - may be used as flash extension )
   * [16:23] = SRAM2b ( Number of 1k sector)
   * [24:31] = SRAM2a ( Number of 1k sector)
   */
  typedef struct
  {
    uint32_t    Version;
  } MB_SafeBootInfoTable_t;

  typedef struct
  {
    uint32_t    Version;
    uint32_t    MemorySize;
    uint32_t    RssInfo;
  } MB_RssInfoTable_t;

  typedef struct
  {
    uint32_t    Version;
    uint32_t    MemorySize;
    uint32_t    ThreadInfo;
    uint32_t    BleInfo;
  } MB_WirelessFwInfoTable_t;

  typedef struct
  {
    MB_SafeBootInfoTable_t      SafeBootInfoTable;
    MB_RssInfoTable_t           RssInfoTable;
    MB_WirelessFwInfoTable_t    WirelessFwIinfoTable;
  } MB_DeviceInfoTable_t;

  typedef struct
  {
    uint8_t     *pcmd_buffer;
    uint8_t     *pcs_buffer;
    uint8_t     *pevt_queue;
  } MB_BleTable_t;

  typedef struct
  {
    uint8_t   *notack_buffer;
    uint8_t   *clicmdrsp_buffer;
    uint8_t   *otcmdrsp_buffer;
  } MB_ThreadTable_t;

  /**
   * msg
   * [0:7]   = cmd/evt
   * [8:31] = Reserved
   */
  typedef struct
  {
    uint8_t   *pcmd_buffer;
    uint8_t   *sys_queue;
  } MB_SysTable_t;

  typedef struct
  {
    uint8_t     *spare_ble_buffer;
    uint8_t     *spare_sys_buffer;
    uint8_t     *blepool;
    uint32_t    blepoolsize;
    uint8_t     *pevt_free_buffer_queue;
    uint8_t     *traces_evt_pool;
    uint32_t    tracespoolsize;
  } MB_MemManagerTable_t;

  typedef struct
  {
    uint8_t   *traces_queue;
  } MB_TracesTable_t;


  typedef struct
  {
    MB_DeviceInfoTable_t    *p_device_info_table;
    MB_BleTable_t           *p_ble_table;
    MB_ThreadTable_t        *p_thread_table;
    MB_SysTable_t           *p_sys_table;
    MB_MemManagerTable_t   *p_mem_manager_table;
    MB_TracesTable_t        *p_traces_table;
  } MB_RefTable_t;

#ifdef __cplusplus
}
#endif

/**
 *********************************************************************************
 * IPCC CHANNELS
 *********************************************************************************
 */

/** CPU1 */
#define HW_IPCC_BLE_CMD_CHANNEL                         LL_IPCC_CHANNEL_1
#define HW_IPCC_SYSTEM_CMD_RSP_CHANNEL                  LL_IPCC_CHANNEL_2
#define HW_IPCC_THREAD_OT_CMD_RSP_CHANNEL               LL_IPCC_CHANNEL_3
#define HW_IPCC_THREAD_CLI_CMD_CHANNEL                  LL_IPCC_CHANNEL_5
#define HW_IPCC_MM_RELEASE_BUFFER_CHANNEL               LL_IPCC_CHANNEL_4

/** CPU2 */
#define HW_IPCC_BLE_EVENT_CHANNEL                       LL_IPCC_CHANNEL_1
#define HW_IPCC_SYSTEM_EVENT_CHANNEL                    LL_IPCC_CHANNEL_2
#define HW_IPCC_THREAD_NOTIFICATION_ACK_CHANNEL         LL_IPCC_CHANNEL_3
#define HW_IPCC_TRACES_CHANNEL                          LL_IPCC_CHANNEL_4
#define HW_IPCC_THREAD_CLI_NOTIFICATION_ACK_CHANNEL     LL_IPCC_CHANNEL_5

#endif /*__MBOX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
