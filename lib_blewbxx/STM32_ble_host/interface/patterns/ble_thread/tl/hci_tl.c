/**
 ******************************************************************************
 * @file    hci_tl.c
 * @author  MCD Application Team
 * @brief   Function for managing HCI interface.
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

#include "os_io_seproxyhal.h"
#include "balenos_ble.h"

/* Includes ------------------------------------------------------------------*/
#include "stm32_wpan_common.h"
#include "hal_types.h"
#include "ble_const.h"
#include "compiler.h"

#include "stm_list.h"
#include "tl.h"
#include "hci_tl.h"


/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/**
 * The default HCI layer timeout is set to 33s
 */
#define HCI_TL_DEFAULT_TIMEOUT (33000)

/* Private macros ------------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_DRIVER_CONTEXT
 */
//PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static volatile uint8_t hci_timer_id;
//PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") 
static tListNode HciAsynchEventQueue;
//PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") 
static volatile HCI_TL_CmdStatus_t HCICmdStatus;
//PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static TL_CmdPacket_t *pCmdBuffer;
//PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") 
/**
 * END of Section BLE_DRIVER_CONTEXT
 */

static tHciContext hciContext;
static tListNode HciCmdEventQueue;
//static void (* StatusNotCallBackFunction) (HCI_TL_CmdStatus_t status);

/* Private function prototypes -----------------------------------------------*/
static void Cmd_SetStatus(HCI_TL_CmdStatus_t hcicmdstatus);
static HCI_TL_CmdStatus_t CmdGetStatus( void );
void SendCmd(uint16_t opcode, uint8_t plen, void *param);
static void TlEvtReceived(TL_EvtPacket_t *hcievt);
//static void TlInit( TL_CmdPacket_t * p_cmdbuffer );

extern void Balenos_UserEvtRx( void * pPayload );

/* Interface ------- ---------------------------------------------------------*/
void hci_init(void(* UserEvtRx)(void* pData), void* pConf)
{
  UNUSED(UserEvtRx);
  UNUSED(pConf);
  LST_init_head (&HciCmdEventQueue);
  LST_init_head (&HciAsynchEventQueue);
  return;
}

__weak void TL_MM_EvtDone(TL_EvtPacket_t* pckt) {
  // OTO: nothing to do no LL here
  UNUSED(pckt);
}

void hci_user_evt_proc(void)
{
  TL_EvtPacket_t *phcievtbuffer;
  tHCI_UserEvtRxParam UserEvtRxParam;

  /**
   * It is more secure to use LST_remove_head()/LST_insert_head() compare to LST_get_next_node()/LST_remove_node()
   * in case the user overwrite the header where the next/prev pointers are located
   */

  while((LST_is_empty(&HciAsynchEventQueue) == FALSE))
  {
    LST_remove_head ( &HciAsynchEventQueue, (tListNode **)&phcievtbuffer );

    UserEvtRxParam.pckt = phcievtbuffer;
    Balenos_UserEvtRx((void *)&UserEvtRxParam);
    TL_MM_EvtDone( phcievtbuffer );
  }

  return;
}

void hci_resume_flow( void )
{
  /**
   * It is better to go through the background process as it is not sure from which context this API may
   * be called
   */
  hci_notify_asynch_evt((void*) &HciAsynchEventQueue);

  return;
}

int hci_send_req(struct hci_request *p_cmd, BOOL async)
{
  UNUSED(async);
  uint16_t opcode;
  uint32_t tickstart = 0;
  TL_CcEvt_t  *pcommand_complete_event;
  TL_CsEvt_t    *pcommand_status_event;
  TL_EvtPacket_t *pevtpacket;
  uint8_t hci_cmd_complete_return_parameters_length;


  Cmd_SetStatus(HCI_TL_CmdBusy);
  opcode = ((p_cmd->ocf) & 0x03ff) | ((p_cmd->ogf) << 10);
  SendCmd(opcode, p_cmd->clen, p_cmd->cparam); // SE -> MCU here
  // wait a reply from the MCU
  io_seproxyhal_general_status();

  // don't wait more a reply than reasonable to avoid stalling the 
  // code if the BLE module is not working correctly
  while(CmdGetStatus() == HCI_TL_CmdBusy)
  {
    unsigned int is_ticker_event, tick;
    if (!io_seproxyhal_spi_is_status_sent()) {
      io_seproxyhal_general_status();
    }

    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

    is_ticker_event = (G_io_seproxyhal_spi_buffer[0] == SEPROXYHAL_TAG_TICKER_EVENT);
    tick = U4BE(G_io_seproxyhal_spi_buffer, 3);

    // process IOs, and BLE fetch, ble queue is updated through common code
    io_seproxyhal_handle_event();

    /**
     * Process Cmd Event
     */
    while(LST_is_empty(&HciCmdEventQueue) == FALSE)
    {
      // log_debug_int_nw("n %x ", HciCmdEventQueue.next);
      // log_debug_int_nw("p %x ", HciCmdEventQueue.next);

      LST_remove_head (&HciCmdEventQueue, (tListNode **)&pevtpacket);

      if(pevtpacket->evtserial.evt.evtcode == TL_BLEEVT_CS_OPCODE)
      {
        pcommand_status_event = (TL_CsEvt_t*)pevtpacket->evtserial.evt.payload;
        if(pcommand_status_event->cmdcode == opcode)
        {
          *(uint8_t *)(p_cmd->rparam) = pcommand_status_event->status;
        }

        if(pcommand_status_event->numcmd != 0)
        {
          Cmd_SetStatus(HCI_TL_CmdAvailable);
        }
      }
      else
      {
        pcommand_complete_event = (TL_CcEvt_t*)pevtpacket->evtserial.evt.payload;

        if(pcommand_complete_event->cmdcode == opcode)
        {
          hci_cmd_complete_return_parameters_length = pevtpacket->evtserial.evt.plen - TL_EVT_CC_HDR_SIZE;
          p_cmd->rlen = MIN(hci_cmd_complete_return_parameters_length, p_cmd->rlen);
          memcpy(p_cmd->rparam, pcommand_complete_event->payload, p_cmd->rlen);
        }

        if(pcommand_complete_event->numcmd != 0)
        {
          Cmd_SetStatus(HCI_TL_CmdAvailable);
        }
      }

      TL_MM_EvtDone(pevtpacket);
    }

    // timeout ?
    if (is_ticker_event && (CmdGetStatus() == HCI_TL_CmdBusy)) {
      if (tickstart==0) {
        tickstart = tick;
      }
      // don't signal timeout if the event has been closed by handle event to avoid sending commands after a status has been issued
      else if ((tick - tickstart) > BLE_COMMAND_TIMEOUT_MS) {
        return -BLE_STATUS_TIMEOUT;
      }
    }
  }

  return 0;
}

static HCI_TL_CmdStatus_t CmdGetStatus(void)
{
  return HCICmdStatus;
}

void SendCmd(uint16_t opcode, uint8_t plen, void *param)
{
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLE_SEND;
  G_io_seproxyhal_spi_buffer[1] = (plen+2)>>8;
  G_io_seproxyhal_spi_buffer[2] = (plen+2);
  G_io_seproxyhal_spi_buffer[3] = opcode >>8;
  G_io_seproxyhal_spi_buffer[4] = opcode;
  os_memmove(G_io_seproxyhal_spi_buffer+5, param, plen);
  //PRINTF("< %.*H\n", 3+plen+2, G_io_seproxyhal_spi_buffer);
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3+plen+2);
}

static void Cmd_SetStatus(HCI_TL_CmdStatus_t hcicmdstatus)
{
  if(hcicmdstatus == HCI_TL_CmdBusy)
  {
    //StatusNotCallBackFunction(HCI_TL_CmdBusy);
    HCICmdStatus = HCI_TL_CmdBusy;
  }
  else
  {
    HCICmdStatus = HCI_TL_CmdAvailable;
    //StatusNotCallBackFunction(HCI_TL_CmdAvailable);
  }

  return;
}

static void TlEvtReceived(TL_EvtPacket_t *hcievt)
{
  if ( ((hcievt->evtserial.evt.evtcode) == TL_BLEEVT_CS_OPCODE) 
    || ((hcievt->evtserial.evt.evtcode) == TL_BLEEVT_CC_OPCODE ) )
  {
    // only one slot in those list, as the source packet is not from a pool
    if (LST_is_empty(&HciCmdEventQueue)) {
      LST_insert_tail(&HciCmdEventQueue, (tListNode *)hcievt);
      //hci_cmd_resp_release(0); /**< Notify the application a full Cmd Event has been received */
    }
  }
  else
  {
    if (LST_is_empty(&HciAsynchEventQueue)) {
      LST_insert_tail(&HciAsynchEventQueue, (tListNode *)hcievt);
      //hci_notify_asynch_evt((void*) &HciAsynchEventQueue); /**< Notify the application a full HCI event has been received */
      // in case of an asynch event, process it (we're not under interrupt here)
      hci_user_evt_proc();
    }
  }

  return;
}

// SEPH packet handler for BLE_RECV_EVENT
struct {
  unsigned int alignement_gap;
  TL_CmdPacket_t received_packet;
} G_io_ble_alignment;
void io_seproxyhal_handle_ble_event(void) {

  if (G_io_ble.powered) {
    // blue recv event has been cache in g_io_seproxyhal_spi_buffer
    os_memmove(&G_io_ble_alignment.received_packet.cmdserial, G_io_seproxyhal_spi_buffer+3, MIN(sizeof(TL_CmdSerial_t), U2BE(G_io_seproxyhal_spi_buffer, 1)));
    //PRINTF("> %.*H\n", U2BE(G_io_seproxyhal_spi_buffer, 1)+3, G_io_seproxyhal_spi_buffer);  
    // process the packet immediately
    TlEvtReceived((TL_EvtPacket_t*)&G_io_ble_alignment.received_packet);
  }
}

