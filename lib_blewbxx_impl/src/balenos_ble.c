
#include "os.h"
#include "os_io_seproxyhal.h"
#include "ux.h"

#include "balenos_ble.h"
#include "lpm.h"

// 2 sec in between each packet, else consider a connection breakdown is detected
#define BLE_TIMEOUT_XFER_MS 2000

#define BLE_SCAN_INTERVAL_MIN_MS 30
#define BLE_SCAN_INTERVAL_MAX_MS 60

#define BLE_SLAVE_CONN_INTERVAL_MIN_MS 15
#define BLE_SLAVE_CONN_INTERVAL_MAX_MS 30

#define SCAN_MS_TO_UNIT(ms) ((unsigned int)((float)ms/(float)0.625))
#define CONN_MS_TO_UNIT(ms) ((unsigned int)((float)ms/(float)1.250))

struct ble_state_s G_io_ble;

void ble_pairing_passkey_uxreq(void);
void ble_pairing_numcomp_uxreq(unsigned int code);
void ble_pairing_cancel_uxreq(void);


/** @addtogroup BlueNRG_Shield
 *  @{
 */

#define BLE_TIMEOUT_CONNECTION_MS 5000UL
// defined by the characteristic length, use minimal to be wide usable
#define BLE_CHUNK_LENGTH_B 20
#define MAX_MTU_SIZE  156

#define DEFAULT_ADVNAME "Nano X"

/** @defgroup BlueNRG_Shield_Sample_Application
 *  @brief Sample application for BlueNR // not used for now
#define THROW(x) for(;;); Shield on STM32 Nucleo boards.
 *  @{
 */

/* Private defines ------------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
#define BDADDR_SIZE 6
#define DEVICE_NAME_LENGTH 23
#define DYNAMIC_MTU //If uncomemnted, activates the dynamic MTU feature (exchange max MTU between client and server upon connection)
/* Private variables ---------------------------------------------------------*/
// #define HAVE_CUSTOM_MAC


/**
 * The server-peripheral mac : leet(Wallet-000002)
 * Little endian encoding
 */
#ifdef HAVE_CUSTOM_MAC
const unsigned char SERVER_BDADDR_CONST[] = {0x02, 0x00, 0x00, 0xe7, 0x11, 0x3A};
unsigned char SERVER_BDADDR[CONFIG_DATA_PUBADDR_LEN];
#endif // HAVE_CUSTOM_MAC
char DEFAULT_NAME[DEVICE_NAME_LENGTH+1]; //+1 for AD_TYPE

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
  do {\
        uuid_struct.uuid128[0] = uuid_0; uuid_struct.uuid128[1] = uuid_1; uuid_struct.uuid128[2] = uuid_2; uuid_struct.uuid128[3] = uuid_3; \
        uuid_struct.uuid128[4] = uuid_4; uuid_struct.uuid128[5] = uuid_5; uuid_struct.uuid128[6] = uuid_6; uuid_struct.uuid128[7] = uuid_7; \
        uuid_struct.uuid128[8] = uuid_8; uuid_struct.uuid128[9] = uuid_9; uuid_struct.uuid128[10] = uuid_10; uuid_struct.uuid128[11] = uuid_11; \
        uuid_struct.uuid128[12] = uuid_12; uuid_struct.uuid128[13] = uuid_13; uuid_struct.uuid128[14] = uuid_14; uuid_struct.uuid128[15] = uuid_15; \
        }while(0)

// original: d973f2e0-b19e-11e2-9e96-0800200c9a66
// new:      13d63400-2C97-0004-0000-4c6564676572
  
const uint8_t service_uuid[16] = {0x72,0x65,0x67,0x64,0x65,0x4c,0x00,0x00,0x04,0x00,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidTX[16] =   {0x72,0x65,0x67,0x64,0x65,0x4c,0x01,0x00,0x04,0x00,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidRX[16] =   {0x72,0x65,0x67,0x64,0x65,0x4c,0x02,0x00,0x04,0x00,0x97,0x2C,0x00,0x34,0xD6,0x13,};

void BLE_diversify_name_address() {

  /*
  uint32_t uid=0xB007BABE;
  //, uid1, uid2, uid3;
  const char hexdigits[] = "0123456789ABCDEF";
  const char* UID = (char*)0x1FFF7590;
  memcpy(&uid1, UID, 4);
  memcpy(&uid2, UID + 4, 4);
  memcpy(&uid3, UID + 8, 4);
  uid = uid1 ^ uid2 ^ uid3;
  */
  unsigned char se_serial[8];
  memset(se_serial, 0, sizeof(se_serial));
  os_serial(se_serial, sizeof(se_serial));
  se_serial[0] ^= se_serial[2];
  se_serial[1] ^= se_serial[3];
  se_serial[0] ^= se_serial[4];
  se_serial[1] ^= se_serial[5];
  se_serial[0] ^= se_serial[6];
  se_serial[1] ^= se_serial[7];
  
  // DEFAULT_NAME[0]=AD_TYPE_SHORTENED_LOCAL_NAME;
  // sprintf(&DEFAULT_NAME[1], DEFAULT_ADVNAME);
  
  snprintf(DEFAULT_NAME+1, sizeof(DEFAULT_NAME)-1, "Nano X %02X%02X", se_serial[0], se_serial[1]);
  DEFAULT_NAME[0]=AD_TYPE_SHORTENED_LOCAL_NAME;

//  sprintf(DEFAULT_NAME, "Balenos-%08X",uid);

#ifdef HAVE_CUSTOM_MAC
  // if no MAC yet defined, then generate a new one and store it.
  if (os_perso_isonboarded() == BOLOS_UX_OK) {
    if(!os_setting_get(OS_SETTING_BLEMACADR, SERVER_BDADDR, sizeof(SERVER_BDADDR))) {
      cx_rng(SERVER_BDADDR, sizeof(SERVER_BDADDR));
      os_setting_set(OS_SETTING_BLEMACADR, SERVER_BDADDR, sizeof(SERVER_BDADDR));
    }
  } else {
    // when not onboarded, the MAC is always random :)
    cx_rng(SERVER_BDADDR, sizeof(SERVER_BDADDR));
  }
  //memcpy(&SERVER_BDADDR[0], &uid, 4);
#endif // HAVE_CUSTOM_MAC
}


unsigned int BLE_make_discoverable(char* discovered_name) {
  tBleStatus ret ;
  uint8_t ScanRespData[31];
  memset(ScanRespData, 0x00, 31);
  // Add service UUID in scan response data
  ScanRespData[0] = 17;
  ScanRespData[1] = AD_TYPE_128_BIT_SERV_UUID;
  memcpy(&ScanRespData[2], service_uuid, 17);
  // Add slave prefererd connexion interval in scan response data
  ScanRespData[18] = 5;
  ScanRespData[19] = AD_TYPE_SLAVE_CONN_INTERVAL;
  ScanRespData[20] = CONN_MS_TO_UNIT(BLE_SLAVE_CONN_INTERVAL_MIN_MS); // *1.25ms
  ScanRespData[21] = 0;
  ScanRespData[22] = CONN_MS_TO_UNIT(BLE_SLAVE_CONN_INTERVAL_MAX_MS); // *1.25ms
  ScanRespData[23] = 0;
  

  discovered_name = DEFAULT_NAME;

  // set device name
  ret = aci_gatt_update_char_value(G_io_ble.gap_service_handle, G_io_ble.gap_dev_name_char_handle, 0, strlen(discovered_name), (uint8_t *)discovered_name);

  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }

  // Add 128 bits service uuid list in scan response
  #warning compute the real value depending on the name's length
  ret = hci_le_set_scan_response_data(24, ScanRespData);
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }
  
  ret = aci_gap_set_discoverable(ADV_IND,
                  // low power leverage
                  //500 * 625/1000, 500 * 625 / 1000,
                  SCAN_MS_TO_UNIT(BLE_SCAN_INTERVAL_MIN_MS), SCAN_MS_TO_UNIT(BLE_SCAN_INTERVAL_MAX_MS),
                  PUBLIC_ADDR, NO_WHITE_LIST_USE,
                  strlen(discovered_name), (unsigned char*)discovered_name,
                  0, NULL,
                  //6*1250/1000, 50*1250/1000); // (50) bigger than this it's far too long
                  0,0);
                  //6, 25); // (50) bigger than this it's far too long
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }
  return 0;
}

/**
 * Upon first reply of the bluenrg we must setup the BLE operating parameters
 */
unsigned int BLE_setup(char *discovered_name) {
  tBleStatus ret;

  G_io_ble.apdu_transport_busy_sending = 0;
  G_io_ble.apdu_transport_lock = 0;
  G_io_ble.ble_chunk_length = BLE_CHUNK_LENGTH_B;

  //SVCCTL_handlers_init(); // OTO osef

  // reset the stack to allow for rentering discoverable state
  if (hci_reset() != BLE_STATUS_SUCCESS){
    return 1;
  }

  BLE_diversify_name_address();


#ifdef HAVE_CUSTOM_MAC
  // probably not a good idea to do this each at each startup
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                  CONFIG_DATA_PUBADDR_LEN,
                  SERVER_BDADDR);
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }
#else
  // send the 00's address to use the MCU's self UID as the address
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                  CONFIG_DATA_PUBADDR_LEN,
                  (unsigned char*)"\x00\x00\x00\x00\x00\x00");
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }
#endif // HAVE_CUSTOM_MAC

  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }

  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0x00, DEVICE_NAME_LENGTH+1, &G_io_ble.gap_service_handle,
                         &G_io_ble.gap_dev_name_char_handle,
                         &G_io_ble.gap_appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }

  ret= aci_gap_set_io_capability(IO_CAP_DISPLAY_YES_NO);

  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }

//  ret = aci_gap_clear_security_db();
//  if (ret != BLE_STATUS_SUCCESS)
//  {
//    APP_DBG_MSG("aci_gap_clear_security_db() failed: %x\r\n", ret);
//  }

  // limit exchanges and data security to limit power, moreover apdus are public (f)
  ret = aci_gap_set_authentication_requirement(BONDING,
                MITM_PROTECTION_REQUIRED,
                0x01, //SC_SUUPORT
                0x01, //KEYPRESS SUPPORT
                8,
                16,
                DONOT_USE_FIXED_PIN_FOR_PAIRING,
                123456,
                0x00);

  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }

  /**
  * Register the event handler to the BLE controller
  */
  // SVCCTL_RegisterSvcHandler(HCI_Event_CB); // OTO done directly in the balenos_userevtrx

  ret = aci_gatt_add_service(UUID_TYPE_128, (Service_UUID_t*)service_uuid, PRIMARY_SERVICE, 9, (uint16_t*)&G_io_ble.service_handle); /* original is 9?? */
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }
  #ifdef DYNAMIC_MTU
    ret =  aci_gatt_add_char(G_io_ble.service_handle, UUID_TYPE_128, (Char_UUID_t*)charUuidTX, MAX_MTU_SIZE-3, CHAR_PROP_NOTIFY/*|CHAR_PROP_READ*/, ATTR_PERMISSION_NONE /*ATTR_PERMISSION_ENCRY_READ*/, 0,
                 10, 1, (uint16_t*)&G_io_ble.tx_characteristic_handle);
  #else
    ret =  aci_gatt_add_char(G_io_ble.service_handle, UUID_TYPE_128, (Char_UUID_t*)charUuidTX, BLE_CHUNK_LENGTH_B, CHAR_PROP_NOTIFY/*|CHAR_PROP_READ*/, ATTR_PERMISSION_NONE /*ATTR_PERMISSION_ENCRY_READ*/, 0,
                 10, 1, (uint16_t*)&G_io_ble.tx_characteristic_handle);
  #endif

  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }
  #ifdef DYNAMIC_MTU
    ret =  aci_gatt_add_char(G_io_ble.service_handle, UUID_TYPE_128, (Char_UUID_t*)charUuidRX, MAX_MTU_SIZE-3, CHAR_PROP_WRITE/*|CHAR_PROP_WRITE_WITHOUT_RESP*/, /*ATTR_PERMISSION_NONE*/ ATTR_PERMISSION_AUTHEN_WRITE, GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                 10, 1, (uint16_t*)&G_io_ble.rx_characteristic_handle);
  #else
    ret =  aci_gatt_add_char(G_io_ble.service_handle, UUID_TYPE_128, (Char_UUID_t*)charUuidRX, BLE_CHUNK_LENGTH_B, CHAR_PROP_WRITE/*|CHAR_PROP_WRITE_WITHOUT_RESP*/, /*ATTR_PERMISSION_NONE*/ ATTR_PERMISSION_AUTHEN_WRITE, GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                 10, 1, (uint16_t*)&G_io_ble.rx_characteristic_handle);
  #endif

  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }

  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,7); // max power
  //ret = aci_hal_set_tx_power_level(0, 0); // lowest power
  if (ret != BLE_STATUS_SUCCESS) {
    return 1;
  }

  G_io_ble.apdu_length = 0; // no apdu received just yet

  if (BLE_make_discoverable(discovered_name)){
    return 1;
  }

  return 0;
}

void BLE_power(unsigned char powered, const char *discovered_name) {

  // only allow BLE operations, when not in plane mode
  memset(&G_io_ble, 0, sizeof(G_io_ble));

  // ensure stalled UX is not requesting something utterly invalid as of now
  ble_pairing_cancel_uxreq();

  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLE_RADIO_POWER;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 1;
  G_io_seproxyhal_spi_buffer[3] = powered ? 3 : 0;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);
  
  G_io_ble.powered = powered;

  if (powered && ! G_io_app.plane_mode) {
    hci_init(NULL, NULL);
    BLE_setup((char*)discovered_name);
  }
  else {
    G_io_ble.powered = 0; // force disable when plane mode
  }

  G_io_app.ble_xfer_timeout = 0;
}

void io_hal_ble_wipe_pairing_db(void) {
  // wipe the bonding info
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLE_RADIO_POWER;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 1;
  G_io_seproxyhal_spi_buffer[3] = SEPROXYHAL_TAG_BLE_RADIO_POWER_ACTION_DBWIPE;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);
  G_io_ble.powered = 0;
}

void Balenos_UserEvtRx( void * pPayload )
{
  SVCCTL_UserEvtFlowStatus_t svctl_return_status;
  tHCI_UserEvtRxParam *pParam;

  pParam = (tHCI_UserEvtRxParam *)pPayload;

  svctl_return_status = HCI_Event_CB((void *)&(pParam->pckt->evtserial));
  if (svctl_return_status != SVCCTL_UserEvtFlowDisable)
{
    pParam->status = HCI_TL_UserEventFlow_Enable;
}
  else
{
    pParam->status = HCI_TL_UserEventFlow_Disable;
  }
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  addr : Address of peer device
 * @param  handle : Connection handle
 * @retval None
 */
void GAP_ConnectionComplete_CB(uint8_t addr_type, uint8_t addr[6], uint16_t handle)
{
  /*
  screen_printf("client connected\n");
  screen_update();
  */
  UNUSED(addr_type);
  UNUSED(addr);

  G_io_ble.connection_handle = handle;
  G_io_ble.ble_chunk_length = BLE_CHUNK_LENGTH_B;
  // memcpy(G_io_ble.Peer_Address, addr, 6);
  // G_io_ble.Peer_Address_Type = addr_type;
  
  aci_l2cap_connection_parameter_update_req(G_io_ble.connection_handle,
                    CONN_MS_TO_UNIT(BLE_SLAVE_CONN_INTERVAL_MIN_MS),  // 6*1.25 = 7.5ms
                    CONN_MS_TO_UNIT(BLE_SLAVE_CONN_INTERVAL_MAX_MS),  // 6*1.25 = 7.5ms
                    0,
                    BLE_TIMEOUT_CONNECTION_MS/10);

  G_io_ble.client_link_established = FALSE;
  G_io_app.ble_xfer_timeout = 0;
}
/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None
 * @retval None
 */
void GAP_DisconnectionComplete_CB(void)
{
  G_io_app.ble_xfer_timeout = 0;
  G_io_ble.client_link_established = FALSE;
  G_io_ble.connection_handle = 0;
  // prepare reconnection
  THROW(EXCEPTION_IO_RESET);
}


/**
 * @brief  This function is used to send data related to the sample service
 *         (to be sent over the air to the remote board).
 * @param  data_buffer : pointer to data to be sent
 * @param  Nb_bytes : number of bytes to send
 * @retval None
 */
//// OTO handle = G_io_ble.tx_characteristic_handle for default apdu tranport
void BLE_send(unsigned short handle, uint8_t* data_buffer, uint8_t Nb_bytes)
{
  unsigned char ret;
  G_io_app.ble_xfer_timeout = BLE_TIMEOUT_XFER_MS;
  retry:
  ret = aci_gatt_update_char_value(G_io_ble.service_handle, handle, 0, Nb_bytes, data_buffer);
  // likely the link could be busy
  if(ret==BLE_STATUS_INSUFFICIENT_RESOURCES || ret == BLE_STATUS_BUSY ||ret == BLE_STATUS_PENDING) {
    // TODO go low power, before retrying
    goto retry;
  }
  G_io_app.ble_xfer_timeout = 0;
}
/**
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */

#define BLE_OFFSET_TAG 0
#define BLE_OFFSET_SEQH 1
#define BLE_OFFSET_SEQL 2
#define BLE_SEQ0_OFFSET_LENH 3
#define BLE_SEQ0_OFFSET_LENL 4
#define BLE_SEQ0_OFFSET_DATA 5 // TAG, SEQ2, LEN2, DATA
#define BLE_SEQN_OFFSET_DATA 3 // TAG, SEQ2, DATA

// instruction supported
#define BLE_TAG_VERSION 0
#define BLE_TAG_INIT 1
#define BLE_TAG_ECHO 2
#define BLE_TAG_APDU 5
#define BLE_TAG_GET_MTU 8
// #define BLE_TAG_REQ_MTU 9
#define BLE_TAG_ERROR 0xE

#define os_memmove memmove
#define os_memset memset

/**
 * Function called when a complete command apdu has been received and is ready for processing
 */
void BLE_protocol_command_apdu_CB() {
  G_io_app.apdu_length = G_io_ble.apdu_length;
}

/**
 * Receive and concat Command APDU chunks
 * Calls
 */
void BLE_protocol_recv(unsigned char data_length, unsigned char* att_data) {
  unsigned short l = data_length;

  // nothing delayed so far
  G_io_ble.delayed_update_char_length = 0;

  if (l==0){
    G_io_ble.delayed_update_char_buffer[1] = 0x67;
    // att_data[]
    goto error_no_buf;
  }

  // not processable, must be resent by the client (a chunk is being output btw)
  if (G_io_ble.apdu_transport_lock) {
    return;
  }

  // process the chunk content
  switch(att_data[BLE_OFFSET_TAG]) {
  case BLE_TAG_APDU:
    // won't process if busy sending (out of it)
    if (G_io_ble.apdu_transport_busy_sending) {
      APP_DBG_MSG("busy\n");
      return;
    }

    if (data_length < 3 || att_data[BLE_OFFSET_SEQH] != (G_io_ble.apdu_transport_seq>>8)
        || att_data[BLE_OFFSET_SEQL] != (G_io_ble.apdu_transport_seq&0xFF)) {
      // ignore packet
      APP_DBG_MSG("wrong seq");
      G_io_ble.delayed_update_char_buffer[1] = 0;
      goto error_no_buf;
    }

    // tag, seq
    l -= 1+2;

    // append the received chunk to the current command apdu
    if (G_io_ble.apdu_transport_seq == 0) 
    {
      if (data_length < 5) {
        G_io_ble.delayed_update_char_buffer[1] = 8;
        goto error_no_buf;
      }
      /// This is the apdu first chunk
      // total apdu size to receive
      G_io_ble.apdu_transport_remlen = (att_data[BLE_SEQ0_OFFSET_LENH]<<8)+(att_data[BLE_SEQ0_OFFSET_LENL]&0xFF);
      G_io_ble.apdu_transport_ptr = G_io_apdu_buffer;
      // check for invalid length encoding (more data in chunk that announced in the total apdu)
      // total length
      l -= 2;
      // compute remaining size to receive
      G_io_ble.apdu_length = G_io_ble.apdu_transport_remlen;

      if (data_length < 6 || G_io_ble.apdu_length > sizeof(G_io_apdu_buffer)) {
        APP_DBG_MSG("length error");
        G_io_ble.delayed_update_char_buffer[1] = 1;
        goto error_no_buf;
      }
      // in case of invalid formatting
      if (l > G_io_ble.apdu_transport_remlen) {
        l = G_io_ble.apdu_transport_remlen;
      }
      // copy data
      //os_memmove(G_io_ble.apdu_transport_ptr, att_data+BLE_TAG_APDU_OFFSET_DATA_SEQ0, l);
      att_data+=BLE_SEQ0_OFFSET_DATA;
      G_io_app.ble_xfer_timeout = BLE_TIMEOUT_XFER_MS;
    }
    else 
    {
      if (data_length < 4) {
        G_io_ble.delayed_update_char_buffer[1] = 7;
        goto error_no_buf;
      }

      // check for invalid length encoding (more data in chunk that announced in the total apdu)
      if (l > G_io_ble.apdu_transport_remlen) {
        l = G_io_ble.apdu_transport_remlen;
      }

      /// This is a following chunk
      // append content
      //os_memmove(G_io_ble.apdu_transport_ptr, att_data+BLE_TAG_APDU_OFFSET_DATA, l);
      att_data+=BLE_SEQN_OFFSET_DATA;
      G_io_app.ble_xfer_timeout = BLE_TIMEOUT_XFER_MS;
    }
    // factorize (f)
    os_memmove(G_io_ble.apdu_transport_ptr, att_data, l);

    // advance
    G_io_ble.apdu_transport_ptr += l;
    G_io_ble.apdu_transport_remlen -= l;
    G_io_ble.apdu_transport_seq ++;

    // the APDU has been completely received
    if(G_io_ble.apdu_transport_remlen==0) {
      G_io_app.ble_xfer_timeout = 0;
      BLE_protocol_command_apdu_CB();
    }
    break;

  case BLE_TAG_VERSION: // get version ID
  {
    if (data_length < 5 || att_data[BLE_OFFSET_SEQH] != 0
        || att_data[BLE_OFFSET_SEQL] != 0) {
      // ignore packet
      G_io_ble.delayed_update_char_buffer[1] = 3;
      goto error_no_buf;
    }

    // TAG,SEQ2,LEN2
    // do not reset the current apdu reception if any
    os_memset(G_io_ble.delayed_update_char_buffer, 0, 9); // PROTOCOL VERSION is 0
    G_io_ble.delayed_update_char_buffer[BLE_SEQ0_OFFSET_LENL]=4;
    // send the response
    G_io_ble.delayed_update_char_length = 9;
    goto reset_state;
  }
  case BLE_TAG_ECHO: // ECHO|PING
  {
    if (data_length < 3 || att_data[BLE_OFFSET_SEQH] != 0
        || att_data[BLE_OFFSET_SEQL] != 0) {
      // ignore packet
      G_io_ble.delayed_update_char_buffer[1] = 4;
      goto error_no_buf;
    }

    G_io_ble.delayed_update_char_length = MIN(data_length, sizeof(G_io_ble.delayed_update_char_buffer));
    os_memmove(G_io_ble.delayed_update_char_buffer, att_data, G_io_ble.delayed_update_char_length);
    break;
  }
  case BLE_TAG_INIT: // (RE)INIT communication machine state
  {
    if (data_length < 3 || att_data[BLE_OFFSET_SEQH] != 0
        || att_data[BLE_OFFSET_SEQL] != 0) {
      // ignore packet
      G_io_ble.delayed_update_char_buffer[1] = 5;
      goto error_no_buf;
    }

    G_io_ble.delayed_update_char_length = MIN(data_length, sizeof(G_io_ble.delayed_update_char_buffer));
    os_memmove(G_io_ble.delayed_update_char_buffer, att_data, G_io_ble.delayed_update_char_length);
    goto reset_state;
  }
#ifdef DYNAMIC_MTU
  case BLE_TAG_GET_MTU: // get version ID
  {
    if (data_length < 5 || att_data[BLE_OFFSET_SEQH] != 0
        || att_data[BLE_OFFSET_SEQL] != 0) {
      // ignore packet
      G_io_ble.delayed_update_char_buffer[1] = 6;
      goto error_no_buf;
    }
    os_memset(G_io_ble.delayed_update_char_buffer, 0, 6);
    G_io_ble.delayed_update_char_buffer[0]=BLE_TAG_GET_MTU;
    G_io_ble.delayed_update_char_buffer[BLE_SEQ0_OFFSET_LENL]=1;
    G_io_ble.delayed_update_char_buffer[5]=G_io_ble.ble_chunk_length;
    G_io_ble.delayed_update_char_length = 6;
    break;
  }
#endif // DYNAMIC_MTU
  // case BLE_TAG_REQ_MTU:
  // {
  //   if (data_length < 5 || att_data[BLE_OFFSET_SEQH] != 0
  //       || att_data[BLE_OFFSET_SEQL] != 0) {
  //     // ignore packet
  //     os_memset(buf, 0, 5);
  //     buf[0] = 10;
  //     goto error_no_buf;
  //   }
  //   os_memset(buf, 0, 6); 
  //   buf[BLE_SEQ0_OFFSET_LENL]=1;
  //   buf[0]=BLE_TAG_REQ_MTU;
  //   buf[5]=aci_gatt_exchange_config(G_io_ble.connection_handle);
  //   BLE_send(G_io_ble.tx_characteristic_handle, buf, 6);
  //   goto reset_state;
  //   break;
  // }
  default: {
    // send error
    G_io_ble.delayed_update_char_buffer[1] = att_data[0];
  
  error_no_buf:
    G_io_ble.delayed_update_char_length = 5;
    os_memset(G_io_ble.delayed_update_char_buffer+2, 0, 3);
    G_io_ble.delayed_update_char_buffer[0]=BLE_TAG_ERROR;

  reset_state:
    // reset state upon error
    G_io_ble.apdu_transport_seq = 0;
    G_io_ble.apdu_transport_remlen = 0;
    G_io_ble.apdu_transport_ptr = G_io_apdu_buffer;
    G_io_ble.apdu_transport_busy_sending = 0; // make sure the BLE is not sending, if sending then exits the send loop
    G_io_ble.apdu_transport_lock = 0;
    G_io_ble.apdu_length = 0;
    break;
  }
  }
}

/**
 * Split and send the Response APDU chunks
 */
#define BLE_protocol_send_RET_OK 0
#define BLE_protocol_send_RET_BUSY 1
#define BLE_protocol_send_RET_ABORTED 2
// TODO ensure not being stuck in case disconnect or timeout during reply.
// add a timeout for reply operation
char BLE_protocol_send(unsigned char* response_apdu, unsigned short response_apdu_length) {
  unsigned char chunk[G_io_ble.ble_chunk_length];

  // error if already sending (inception)
  if (G_io_ble.apdu_transport_busy_sending) {
    return 1;
  }

  G_io_ble.apdu_transport_busy_sending = 1;

  G_io_ble.apdu_transport_ptr = response_apdu;
  G_io_ble.apdu_transport_remlen = response_apdu_length;
  G_io_ble.apdu_transport_seq = 0;
  // mark capdu consumed
  G_io_app.apdu_length = 0;

  // perform send
  while(G_io_ble.apdu_transport_remlen) {
    unsigned short l;

    // keep the channel identifier
    chunk[BLE_OFFSET_TAG] = BLE_TAG_APDU;
    chunk[BLE_OFFSET_SEQH] = G_io_ble.apdu_transport_seq>>8;
    chunk[BLE_OFFSET_SEQL] = G_io_ble.apdu_transport_seq;

    if (G_io_ble.apdu_transport_seq == 0) {
      l = ((G_io_ble.apdu_transport_remlen>G_io_ble.ble_chunk_length-BLE_SEQ0_OFFSET_DATA) ? G_io_ble.ble_chunk_length-BLE_SEQ0_OFFSET_DATA : G_io_ble.apdu_transport_remlen);
      chunk[BLE_SEQ0_OFFSET_LENH] = G_io_ble.apdu_transport_remlen>>8;
      chunk[BLE_SEQ0_OFFSET_LENL] = G_io_ble.apdu_transport_remlen;
      os_memmove(chunk+BLE_SEQ0_OFFSET_DATA, G_io_ble.apdu_transport_ptr, l);
      G_io_ble.apdu_transport_ptr += l;
      G_io_ble.apdu_transport_remlen -= l;
      l += BLE_SEQ0_OFFSET_DATA;
    }
    else {
      l = ((G_io_ble.apdu_transport_remlen>G_io_ble.ble_chunk_length-BLE_SEQN_OFFSET_DATA) ? G_io_ble.ble_chunk_length-BLE_SEQN_OFFSET_DATA : G_io_ble.apdu_transport_remlen);
      os_memmove(chunk+BLE_SEQN_OFFSET_DATA, G_io_ble.apdu_transport_ptr, l);
      G_io_ble.apdu_transport_ptr += l;
      G_io_ble.apdu_transport_remlen -= l;
      l += BLE_SEQN_OFFSET_DATA;
    }
    // prepare next chunk numbering
    G_io_ble.apdu_transport_seq++;

    // check if isr is performing before colliding
    G_io_ble.apdu_transport_lock = 1;
    // TAG_INIT called under isr
    if (!G_io_ble.apdu_transport_busy_sending) {
      G_io_ble.apdu_transport_lock = 0;
      // aborted by previous interrupt
      return 2;
    }
    // in mutual exclusion with isr
    BLE_send(G_io_ble.tx_characteristic_handle, chunk, l);
    // release the seal (f)
    G_io_ble.apdu_transport_lock = 0;
  }

  // prepare for a new command reception
  G_io_ble.apdu_transport_seq = 0;
  G_io_ble.apdu_transport_busy_sending = 0;
  G_io_app.apdu_state = APDU_IDLE;
  G_io_app.apdu_length = 0;

  //OK
  return 0;
}

void ble_pairing_ux_reply_passkey(unsigned int userstatus);
void ble_pairing_ux_reply_numcomp(unsigned int userstatus);

long unsigned int ble_generate_rng_u32(void) {
  return cx_rng_u32_inline();
}

unsigned int ble_generate_pairing_code(void) {
  // generate a 6 digit pairing code
  G_io_ble.pairing_code = cx_rng_u32_range_func(0, 1000000, ble_generate_rng_u32) ; // ensure a 6 digit code
  G_io_ble.pairing_code_len = 6;
  return G_io_ble.pairing_code;
}

#ifndef HAVE_BOLOS

void ble_pairing_passkey_uxreq(void) {
  unsigned int code = ble_generate_pairing_code();
  G_ux_params.u.pairing_request.type = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST_PASSKEY;
  G_ux_params.u.pairing_request.pairing_info_len = 6;
  SPRINTF(G_ux_params.u.pairing_request.pairing_info, "%06d", code);
  G_ux_params.ux_id = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST;
  G_ux_params.len = sizeof(G_ux_params.u.pairing_request);
  G_ux.asynchmodal_end_callback = ble_pairing_ux_reply_passkey;
  os_ux(&G_ux_params); // start asynch modal request to the user to accept the pairing before replying to the client
}

void ble_pairing_numcomp_uxreq(unsigned int code) {
  G_io_ble.pairing_code = code;
  G_io_ble.pairing_code_len = 6;
  G_ux_params.u.pairing_request.type = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST_NUMCOMP;
  G_ux_params.u.pairing_request.pairing_info_len = 6;
  SPRINTF(G_ux_params.u.pairing_request.pairing_info, "%06d", G_io_ble.pairing_code);
  G_ux_params.ux_id = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST;
  G_ux_params.len = sizeof(G_ux_params.u.pairing_request);
  G_ux.asynchmodal_end_callback = ble_pairing_ux_reply_numcomp;
  os_ux(&G_ux_params); // start asynch modal request to the user to accept the pairing before replying to the client
}

void ble_pairing_cancel_uxreq(void) {
  unsigned int ulen = G_ux_params.len; // backup ux len
  // cancel pairing window if not done yet
  G_ux_params.ux_id = BOLOS_UX_ASYNCHMODAL_PAIRING_CANCEL;
  G_ux_params.len = 0;
  G_ux.asynchmodal_end_callback = NULL;
  os_ux(&G_ux_params);
  G_ux_params.len = ulen;
}
#endif //HAVE_BOLOS


void ble_pairing_ux_reply_passkey(unsigned int userstatus) {
  ble_pairing_cancel_uxreq();
  if (userstatus == BOLOS_UX_OK) {
    aci_gap_pass_key_resp(G_io_ble.connection_handle, G_io_ble.pairing_code);
  }
  else {
    // discarded by user, disconnect BLE, and reenable discovering
    BLE_power(1, NULL);
  }
}

void ble_pairing_ux_reply_numcomp(unsigned int userstatus) {
  ble_pairing_cancel_uxreq();
  if(userstatus == BOLOS_UX_OK) {
    aci_gap_numeric_comparison_value_confirm_yesno(G_io_ble.connection_handle, 0x01);

  }
  else {
    // discarded by user, disconnect BLE, and reenable discovering
    BLE_power(1, NULL);
  }
}

/**
 * @brief  This function is called whenever there is an ACI event to be processed.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  pckt  Pointer to the ACI packet
 * @retval None
 */
SVCCTL_UserEvtFlowStatus_t HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  tBleStatus ret;

  switch(event_pckt->evt){

  case EVT_DISCONN_COMPLETE:
    {
      //hci_disconnection_complete_event_rp0 *evt = (void*) event_pckt->data;)
      ble_pairing_cancel_uxreq();
      //l("ble disc: %x", evt->Reason);
      GAP_DisconnectionComplete_CB();
      break;
    }


  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      switch(evt->subevent)
      {
        case EVT_LE_CONN_COMPLETE:
        {
          hci_le_connection_complete_event_rp0 *cc = (void *)evt->data;
          GAP_ConnectionComplete_CB(cc->Peer_Address_Type, cc->Peer_Address, cc->Connection_Handle);
          ble_pairing_cancel_uxreq();
          break;
        }

        case EVT_LE_CONN_UPDATE_COMPLETE:
        {
          // hci_le_connection_update_complete_event_rp0 *cuc = (void *)evt->data;
          break;
        }

        default :
        {
          break;
        }

      }
      break;
    }
  case 0x08: //ENCRYPT change
    {
      // hci_encryption_change_event_rp0 *evt = (void *)event_pckt->data;
      // APP_DBG_MSG("EVT_ENCRYPT_CHANGE\r\n\tstatus = %x\r\n\thandle = %x\r\n\tencrypt = %x\r\n", evt->Status, evt->Connection_Handle, evt->Encryption_Enabled);
      break;
    }

  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      switch(blue_evt->ecode){
      #ifdef DYNAMIC_MTU
        case EVT_BLUE_ATT_EXCHANGE_MTU_RESP:
        {
          aci_att_exchange_mtu_resp_event_rp0 *evt = (aci_att_exchange_mtu_resp_event_rp0*)blue_evt->data;
          G_io_ble.ble_chunk_length = evt->Server_RX_MTU-3;
          break;
        }
      #endif

      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          aci_gatt_attribute_modified_event_rp0 *evt = (aci_gatt_attribute_modified_event_rp0*)blue_evt->data;
          if (evt->Attr_Handle == G_io_ble.tx_characteristic_handle + 2) {
            // tx char handle attribute is triggered, the client registered to notification reception
            if(evt->Attr_Data[0] == 0x01 && evt->Attr_Data[1] == 0) {
              G_io_ble.notification_reg_handle = evt->Attr_Handle;
              // G_io_seproxyhal_events |= SEPROXYHAL_EVENT_BLE_NOTIFIFICATION_REGISTER;
              G_io_ble.client_link_established = TRUE;
            }
            else if(evt->Attr_Data[0] == 0x01 && evt->Attr_Data[1] == 0) {
//            G_io_ble.client_link_established = FALSE;
              G_io_ble.notification_unreg_handle = evt->Attr_Handle;
              // G_io_seproxyhal_events |= SEPROXYHAL_EVENT_BLE_NOTIFIFICATION_UNREGISTER;
            }
          }
        }
        break;

        case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
        {
          APP_DBG_MSG("EVT_BLUE_GATT_WRITE_PERMIT_REQ");
          aci_gatt_write_permit_req_event_rp0 *evt = (aci_gatt_write_permit_req_event_rp0*)blue_evt->data;

          if(evt->Attribute_Handle == G_io_ble.rx_characteristic_handle + 1){
            G_io_ble.delayed_update_char_length = 0;
            // check if client completely connected
            if (G_io_ble.client_link_established) {
              // decapsulate BLE framing to reconstruct the command.
              // command set :
              //  - reset
              //  - DFU
              //  - apdu transport (to deliver the SE, whom will afterwards drive the screen and request a pin code
              BLE_protocol_recv(evt->Data_Length, evt->Data);
            }

            // accept the write request
            // Accept it before processing its data to avoid deadlock in spi access within attribute modified
            ret = aci_gatt_write_resp( evt->Connection_Handle,
                                     evt->Attribute_Handle,
                                     0, // accept the write
                                     0, // no error
                                     evt->Data_Length, // TODO must check it's the size of the chunk
                                     evt->Data);
            if (ret != BLE_STATUS_SUCCESS){
              return ret;
            }

            // execute delayed send if any
            if (G_io_ble.delayed_update_char_length) {
              BLE_send(G_io_ble.tx_characteristic_handle, G_io_ble.delayed_update_char_buffer, G_io_ble.delayed_update_char_length);
            }
          }
        }
        break;
        case EVT_BLUE_L2CAP_CONNECTION_UPDATE_RESP:
        {
          APP_DBG_MSG("EVT_BLUE_L2CAP_CONNECTION_UPDATE_RESP\r\n\thandle = %x\r\n\tstatus = %x\r\n", *(uint16_t*)blue_evt->data, *(uint8_t*)blue_evt->data+2);
          break;
        }
        case EVT_BLUE_GAP_BOND_LOST:
        {
          ble_pairing_cancel_uxreq();
          GAP_DisconnectionComplete_CB();
          break;
        }
        case EVT_BLUE_GAP_PAIRING_CMPLT:
        {
          volatile aci_gap_pairing_complete_event_rp0 *evt = (aci_gap_pairing_complete_event_rp0*)blue_evt->data;
          APP_DBG_MSG("EVT_BLUE_GAP_PAIRING_CMPLT\r\n\thandle = %x\r\n\tstatus = %x\r\n\treason = %x\r\n", evt->Connection_Handle, evt->Status, evt->Reason);
          switch(evt->Status)
          {
            case SM_PAIRING_SUCCESS:
            {
              break;
            }
            case SM_PAIRING_TIMEOUT:
            {
              ret = aci_gap_terminate(G_io_ble.connection_handle, 0x05);
              if (ret != BLE_STATUS_SUCCESS){
                APP_DBG_MSG("aci_gap_terminate failure err = %x\r\n", ret);
              }
              break;
            }
            case SM_PAIRING_FAILED:
            {
              ret = aci_gap_terminate(G_io_ble.connection_handle, 0x05);
              if (ret != BLE_STATUS_SUCCESS){
                APP_DBG_MSG("aci_gap_terminate failure err = %x\r\n", ret);
              }
              break;
            }
            default :
            {
              // printf("unknown status\r\n");
              break;
            }
          }
          ble_pairing_cancel_uxreq();
          break;
        }
        case EVT_BLUE_GAP_PASS_KEY_REQUEST:
        {
          ble_pairing_passkey_uxreq();
          break;
        }
        case 0x0409:         // ACI_GAP_NUMERIC_COMPARISON_VALUE_EVENT
        {
          uint32_t * numComparison;
          numComparison = (uint32_t *) (blue_evt->data+2); // wow alignment !!, on cortex m0 that would be awesome :D
          ble_pairing_numcomp_uxreq(numComparison[0]);
          break;
        }

       /*case EVT_BLUE_GATT_TX_POOL_AVAILABLE:
        G_io_ble.tx_pool_available = 1;
        break;*/

      default:
        APP_DBG_MSG("[ERROR] UNHANDLED EVT_VENDOR %x\n", blue_evt->ecode);
        break;
      }
    }
    break;
  default :
    APP_DBG_MSG("[ERROR] UNHANDLED EVENT %x\n", event_pckt->evt);
    break;
  }
  return SVCCTL_EvtAckFlowEnable;
}
