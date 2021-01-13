
#pragma once

#include "stdint.h"

//#include "main.h"
#include "ble.h"
#include "app_entry.h"
#include "sm.h"
#include "hci_tl.h"
#include "svc_ctl.h"
#include "ble_types.h"
#include "seproxyhal_protocol.h"

void BLE_power(unsigned char powered, const char *discovered_name);
SVCCTL_UserEvtFlowStatus_t HCI_Event_CB(void *pckt);
void Balenos_UserEvtRx( void * pPayload );

unsigned int ble_generate_pairing_code(void);

typedef struct ble_state_s {
  unsigned int powered;
  
  // public
  unsigned short gap_service_handle;
  unsigned short gap_dev_name_char_handle;
  unsigned short gap_appearance_char_handle;

  unsigned short connection_handle;
  unsigned char ble_chunk_length;
  unsigned short service_handle, tx_characteristic_handle, rx_characteristic_handle;
  unsigned short notification_reg_handle;
  unsigned short notification_unreg_handle;
  
  unsigned char client_link_established;
  unsigned short apdu_transport_remlen;
  unsigned short apdu_transport_seq;
  unsigned char* apdu_transport_ptr;
  unsigned char apdu_transport_busy_sending;
  unsigned char apdu_transport_lock;

  unsigned char* last_discovered_name;
  unsigned int apdu_length;

  unsigned int pairing_code_len;
  unsigned int pairing_code;

  unsigned int delayed_update_char_length;
  unsigned char delayed_update_char_buffer[9];

} ble_state_t;
extern ble_state_t G_io_ble;

// Debug macro definitions
//#define DEBUG_LIST
