
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2022 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "os.h"
#include "os_settings.h"
#include "os_io_seproxyhal.h"

#include "ble_hci_le.h"
#include "ble_hal_aci.h"
#include "ble_gap_aci.h"
#include "ble_l2cap_aci.h"
#include "ble_gatt_aci.h"
#include "ble_legacy.h"

#include "lcx_rng.h"

#include "ledger_protocol.h"
#include "ledger_ble.h"

/* Private enumerations ------------------------------------------------------*/
typedef enum {
	BLE_STATE_INITIALIZING,
	BLE_STATE_INITIALIZED,
	BLE_STATE_CONFIGURE_ADVERTISING,
	BLE_STATE_CONNECTING,
	BLE_STATE_CONNECTED,
	BLE_STATE_DISCONNECTING,
} ble_state_t;


typedef enum {
	BLE_INIT_STEP_IDLE,
	BLE_INIT_STEP_RESET,
	BLE_INIT_STEP_STATIC_ADDRESS,
	BLE_INIT_STEP_GATT_INIT,
	BLE_INIT_STEP_GAP_INIT,
	BLE_INIT_STEP_SET_IO_CAPABILITIES,
	BLE_INIT_STEP_SET_AUTH_REQUIREMENTS,
	BLE_INIT_STEP_ADD_SERVICE,
	BLE_INIT_STEP_ADD_NOTIFICATION_CHARACTERISTIC,
	BLE_INIT_STEP_ADD_WRITE_CHARACTERISTIC,
	BLE_INIT_STEP_ADD_WRITE_COMMAND_CHARACTERISTIC,
	BLE_INIT_STEP_SET_TX_POWER_LEVEL,
	BLE_INIT_STEP_CONFIGURE_ADVERTISING,
	BLE_INIT_STEP_END,
} ble_init_step_t;

typedef enum {
	BLE_CONFIG_ADV_STEP_IDLE,
	BLE_CONFIG_ADV_STEP_SET_ADV_DATAS,
	BLE_CONFIG_ADV_STEP_SET_SCAN_RSP_DATAS,
	BLE_CONFIG_ADV_STEP_SET_GAP_DEVICE_NAME,
	BLE_CONFIG_ADV_STEP_START,
	BLE_CONFIG_ADV_STEP_END,
} ble_config_adv_step_t;

/* Private types, structures, unions -----------------------------------------*/
typedef struct {
	uint16_t connection_handle;
	uint8_t  role_slave;
	uint8_t  peer_address_random;
	uint8_t  peer_address[6];
	uint16_t conn_interval;
	uint16_t conn_latency;
	uint16_t supervision_timeout;
	uint8_t  master_clock_accuracy;
	uint8_t  tx_phy;
	uint8_t  rx_phy;
	uint16_t max_tx_octets;
	uint16_t max_tx_time;
	uint16_t max_rx_octets;
	uint16_t max_rx_time;
	uint8_t  encrypted;
} ble_connection_t;

typedef struct {
	// General
	ble_state_t state;
	char        device_name[20+1];
	char        device_name_length;
	uint8_t     random_address[CONFIG_DATA_RANDOM_ADDRESS_LEN];

	// Init
	ble_init_step_t init_step;

	// Advertising configuration
	ble_config_adv_step_t adv_step;
	uint8_t               adv_enable;

	// HCI
	uint16_t hci_cmd_opcode;

	// GAP
	uint16_t         gap_service_handle;
	uint16_t         gap_device_name_characteristic_handle;
	uint16_t         gap_appearance_characteristic_handle;
	uint8_t          advertising_enabled;
	ble_connection_t connection;
	uint16_t         pairing_code;
	uint8_t          pairing_in_progress;

	// L2CAP
	uint8_t connection_updated;

	// ATT/GATT
	uint16_t ledger_gatt_service_handle;
	uint16_t ledger_gatt_notification_characteristic_handle;
	uint16_t ledger_gatt_write_characteristic_handle;
	uint16_t ledger_gatt_write_cmd_characteristic_handle;
	uint8_t  notifications_enabled;

	// PAIRING
	uint8_t clear_pairing;

	// APDU
	uint8_t  wait_write_resp_ack;
	uint16_t apdu_buffer_length;
	uint8_t  apdu_buffer[IO_APDU_BUFFER_SIZE];

	// TRANSFER MODE
	uint8_t  transfer_mode_enable;
	uint8_t  resp_length;
	uint8_t  resp[2];

} ledger_ble_data_t;

/* Private defines------------------------------------------------------------*/
#define MAX_MTU_SIZE 156

#define BLE_SLAVE_CONN_INTERVAL_MIN 12  // 15ms
#define BLE_SLAVE_CONN_INTERVAL_MAX 24  // 30ms

#define BLE_ADVERTISING_INTERVAL_MIN 48 // 30ms
#define BLE_ADVERTISING_INTERVAL_MAX 96 // 60ms

#ifdef HAVE_PRINTF
#define LOG_BLE PRINTF
#else // !HAVE_PRINTF
#define LOG_BLE(...)
#endif // !HAVE_PRINTF

/* Private macros-------------------------------------------------------------*/

/* Private functions prototypes ----------------------------------------------*/
static void get_device_name(void);
static void configure_advertising_mngr(uint16_t opcode);
static void init_mngr(uint16_t opcode, uint8_t *buffer, uint16_t length);
static void hci_evt_cmd_complete(uint8_t *buffer, uint16_t length);
static void hci_evt_le_meta_evt(uint8_t *buffer, uint16_t length);
static void hci_evt_vendor(uint8_t *buffer, uint16_t length);
static void end_pairing_ux(uint8_t pairing_ok);
static void ask_user_pairing_numeric_comparison(uint32_t code);
static void rsp_user_pairing_numeric_comparison(unsigned int status);
static void ask_user_pairing_passkey(void);
static void rsp_user_pairing_passkey(unsigned int status);
static void attribute_modified(uint8_t *buffer, uint16_t length);
static void write_permit_request(uint8_t *buffer, uint16_t length);
static void advertising_enable(uint8_t enable);
static void start_advertising(void);
static void notify_chunk(void);
static void check_transfer_mode(uint8_t enable);

/* Exported variables --------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#ifdef TARGET_STAX
const uint8_t service_uuid[16] = {0x72,0x65,0x67,0x64,0x65,0x4c,0x00,0x00,0x04,0x60,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidTX[16]   = {0x72,0x65,0x67,0x64,0x65,0x4c,0x01,0x00,0x04,0x60,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidRX[16]   = {0x72,0x65,0x67,0x64,0x65,0x4c,0x02,0x00,0x04,0x60,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidRX2[16]  = {0x72,0x65,0x67,0x64,0x65,0x4c,0x03,0x00,0x04,0x60,0x97,0x2C,0x00,0x34,0xD6,0x13,};
#endif

#ifdef TARGET_NANOX
const uint8_t service_uuid[16] = {0x72,0x65,0x67,0x64,0x65,0x4c,0x00,0x00,0x04,0x00,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidTX[16]   = {0x72,0x65,0x67,0x64,0x65,0x4c,0x01,0x00,0x04,0x00,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidRX[16]   = {0x72,0x65,0x67,0x64,0x65,0x4c,0x02,0x00,0x04,0x00,0x97,0x2C,0x00,0x34,0xD6,0x13,};
const uint8_t charUuidRX2[16]  = {0x72,0x65,0x67,0x64,0x65,0x4c,0x03,0x00,0x04,0x00,0x97,0x2C,0x00,0x34,0xD6,0x13,};
#endif

static ledger_protocol_t ledger_protocol_data;
static ledger_ble_data_t ledger_ble_data;

/* Private functions ---------------------------------------------------------*/
static void get_device_name(void)
{
	memset(ledger_ble_data.device_name, 0,
	       sizeof(ledger_ble_data.device_name));
	ledger_ble_data.device_name_length = os_setting_get(OS_SETTING_DEVICENAME,
	                                                    (uint8_t*)ledger_ble_data.device_name,
	                                                    sizeof(ledger_ble_data.device_name)-1);
}

static void configure_advertising_mngr(uint16_t opcode)
{
	if (  (ledger_ble_data.hci_cmd_opcode != 0xFFFF)
	    &&(opcode != ledger_ble_data.hci_cmd_opcode)
	   ) {
		// Unexpected event => BLE_TODO
		return;
	}

	uint8_t buffer[31];
	uint8_t index = 0;

	if (ledger_ble_data.adv_step == BLE_CONFIG_ADV_STEP_IDLE) {
		ledger_ble_data.connection.connection_handle = 0xFFFF;
		ledger_ble_data.advertising_enabled          = 0;
		LOG_BLE("CONFIGURE ADVERTISING START\n");
	}
	else if (ledger_ble_data.adv_step == (BLE_CONFIG_ADV_STEP_END-1)) {
		ledger_ble_data.advertising_enabled = 1;
	}

	ledger_ble_data.adv_step++;
	if (  (ledger_ble_data.adv_step == (BLE_CONFIG_ADV_STEP_END-1))
	    &&(!ledger_ble_data.adv_enable)
	   ) {
		ledger_ble_data.adv_step++;
	}

	switch (ledger_ble_data.adv_step) {

	case BLE_CONFIG_ADV_STEP_SET_ADV_DATAS:
		// Flags
		buffer[index++] = 2;
		buffer[index++] = AD_TYPE_FLAGS;
		buffer[index++] = FLAG_BIT_BR_EDR_NOT_SUPPORTED | FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE;

		// Complete Local Name
		get_device_name();
		buffer[index++] = ledger_ble_data.device_name_length+1;
		buffer[index++] = AD_TYPE_COMPLETE_LOCAL_NAME;
		memcpy(&buffer[index], ledger_ble_data.device_name, ledger_ble_data.device_name_length);
		index += ledger_ble_data.device_name_length;

		ledger_ble_data.hci_cmd_opcode = 0xfc8e;
		aci_gap_update_adv_data(index, buffer);
		break;

	case BLE_CONFIG_ADV_STEP_SET_SCAN_RSP_DATAS:
		// Incomplete List of 128-bit Service UUIDs
		buffer[index++] = sizeof(service_uuid)+1;
		buffer[index++] = AD_TYPE_128_BIT_SERV_UUID;
		memcpy(&buffer[index], service_uuid, sizeof(service_uuid));
		index += sizeof(service_uuid);

		// Slave Connection Interval Range
		buffer[index++] = 5;
		buffer[index++] = AD_TYPE_SLAVE_CONN_INTERVAL;
		buffer[index++] = BLE_SLAVE_CONN_INTERVAL_MIN;
		buffer[index++] = 0;
		buffer[index++] = BLE_SLAVE_CONN_INTERVAL_MAX;
		buffer[index++] = 0;

		ledger_ble_data.hci_cmd_opcode = 0x2009;
		hci_le_set_scan_response_data(index, buffer);
		break;

	case BLE_CONFIG_ADV_STEP_SET_GAP_DEVICE_NAME:
		ledger_ble_data.hci_cmd_opcode = 0xfd06;
		aci_gatt_update_char_value(ledger_ble_data.gap_service_handle,
		                           ledger_ble_data.gap_device_name_characteristic_handle,
		                           0,
		                           ledger_ble_data.device_name_length,
		                           (uint8_t*)ledger_ble_data.device_name);
		break;

	case BLE_CONFIG_ADV_STEP_START:
		ledger_ble_data.hci_cmd_opcode = 0xfc83;
		advertising_enable(1);
		break;

	default:
		LOG_BLE("CONFIGURE ADVERTISING END\n");
		if (ledger_ble_data.state == BLE_STATE_CONFIGURE_ADVERTISING) {
			ledger_ble_data.state = BLE_STATE_INITIALIZED;
		}
		break;
	}
}

static void init_mngr(uint16_t opcode, uint8_t *buffer, uint16_t length)
{
	UNUSED(length);

	if (  (ledger_ble_data.hci_cmd_opcode != 0xFFFF)
	    &&(opcode != ledger_ble_data.hci_cmd_opcode)
	   ) {
		// Unexpected event => BLE_TODO
		return;
	}

	if (ledger_ble_data.init_step == BLE_INIT_STEP_IDLE) {
		LOG_BLE("INIT START\n");
	}
	else if (  (length >= 6)
	         &&(ledger_ble_data.init_step == BLE_INIT_STEP_GAP_INIT)
	        ) {
		ledger_ble_data.gap_service_handle                    = U2LE(buffer, 1);
		ledger_ble_data.gap_device_name_characteristic_handle = U2LE(buffer, 3);
		ledger_ble_data.gap_appearance_characteristic_handle  = U2LE(buffer, 5);
	}
	else if (  (length >= 2)
	         &&(ledger_ble_data.init_step == BLE_INIT_STEP_ADD_SERVICE)
	        ) {
		ledger_ble_data.ledger_gatt_service_handle = U2LE(buffer, 1);
	}
	else if (  (length >= 2)
	         &&(ledger_ble_data.init_step == BLE_INIT_STEP_ADD_NOTIFICATION_CHARACTERISTIC)
	        ) {
		ledger_ble_data.ledger_gatt_notification_characteristic_handle = U2LE(buffer, 1);
	}
	else if (  (length >= 2)
	         &&(ledger_ble_data.init_step == BLE_INIT_STEP_ADD_WRITE_CHARACTERISTIC)
	        ) {
		ledger_ble_data.ledger_gatt_write_characteristic_handle = U2LE(buffer, 1);
	}
	else if (  (length >= 2)
	         &&(ledger_ble_data.init_step == BLE_INIT_STEP_ADD_WRITE_COMMAND_CHARACTERISTIC)
	        ) {
		ledger_ble_data.ledger_gatt_write_cmd_characteristic_handle = U2LE(buffer, 1);
	}
	else if (ledger_ble_data.init_step == BLE_INIT_STEP_CONFIGURE_ADVERTISING) {
		ledger_ble_data.adv_enable = !os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
		configure_advertising_mngr(opcode);
		if (ledger_ble_data.adv_step != BLE_CONFIG_ADV_STEP_END) {
			return;
		}
	}

	ledger_ble_data.init_step++;

	switch (ledger_ble_data.init_step) {

	case BLE_INIT_STEP_RESET:
		hci_reset();
		break;

	case BLE_INIT_STEP_STATIC_ADDRESS:
		ledger_ble_data.hci_cmd_opcode = 0xfc0c;
		aci_hal_write_config_data(CONFIG_DATA_RANDOM_ADDRESS_OFFSET,
		                          CONFIG_DATA_RANDOM_ADDRESS_LEN,
		                          ledger_ble_data.random_address);
		break;

	case BLE_INIT_STEP_GATT_INIT:
		ledger_ble_data.hci_cmd_opcode = 0xfd01;
		aci_gatt_init();
		break;

	case BLE_INIT_STEP_GAP_INIT:
		ledger_ble_data.hci_cmd_opcode = 0xfc8a;
		aci_gap_init(GAP_PERIPHERAL_ROLE,
		             PRIVACY_DISABLED,
		             sizeof(ledger_ble_data.device_name)-1,
		             &ledger_ble_data.gap_service_handle,
		             &ledger_ble_data.gap_device_name_characteristic_handle,
		             &ledger_ble_data.gap_appearance_characteristic_handle);
		break;

	case BLE_INIT_STEP_SET_IO_CAPABILITIES:
		ledger_ble_data.hci_cmd_opcode = 0xfc85;
		aci_gap_set_io_capability(IO_CAP_DISPLAY_YES_NO);
		break;

	case BLE_INIT_STEP_SET_AUTH_REQUIREMENTS:
		ledger_ble_data.hci_cmd_opcode = 0xfc86;
		aci_gap_set_authentication_requirement(BONDING,
		                                       MITM_PROTECTION_REQUIRED,
		                                       0x01, // LE Secure connections pairing supported but optional
		                                       KEYPRESS_NOT_SUPPORTED,
		                                       MIN_ENCRY_KEY_SIZE+1,
		                                       MAX_ENCRY_KEY_SIZE,
		                                       DONOT_USE_FIXED_PIN_FOR_PAIRING, 0,
		                                       STATIC_RANDOM_ADDR);
		break;

	case BLE_INIT_STEP_ADD_SERVICE:
		ledger_ble_data.hci_cmd_opcode = 0xfd02;
		aci_gatt_add_service(UUID_TYPE_128,
		                     (const Service_UUID_t *) service_uuid,
		                     PRIMARY_SERVICE,
		                     9,
		                     &ledger_ble_data.ledger_gatt_service_handle);
		break;

	case BLE_INIT_STEP_ADD_NOTIFICATION_CHARACTERISTIC:
		ledger_ble_data.hci_cmd_opcode = 0xfd04;
		aci_gatt_add_char(ledger_ble_data.ledger_gatt_service_handle,
		                  UUID_TYPE_128,
		                  (const Char_UUID_t *) charUuidTX,
		                  MAX_MTU_SIZE,
		                  CHAR_PROP_NOTIFY,
		                  ATTR_PERMISSION_AUTHEN_WRITE,
		                  GATT_DONT_NOTIFY_EVENTS,
		                  MAX_ENCRY_KEY_SIZE,
		                  CHAR_VALUE_LEN_VARIABLE,
		                  &ledger_ble_data.ledger_gatt_notification_characteristic_handle);
		break;

	case BLE_INIT_STEP_ADD_WRITE_CHARACTERISTIC:
		ledger_ble_data.hci_cmd_opcode = 0xfd04;
		aci_gatt_add_char(ledger_ble_data.ledger_gatt_service_handle,
		                  UUID_TYPE_128,
		                  (const Char_UUID_t *) charUuidRX,
		                  MAX_MTU_SIZE,
		                  CHAR_PROP_WRITE,
		                  ATTR_PERMISSION_AUTHEN_WRITE,
		                  GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
		                  MAX_ENCRY_KEY_SIZE,
		                  CHAR_VALUE_LEN_VARIABLE,
		                  &ledger_ble_data.ledger_gatt_write_characteristic_handle);
		break;

	case BLE_INIT_STEP_ADD_WRITE_COMMAND_CHARACTERISTIC:
		ledger_ble_data.hci_cmd_opcode = 0xfd04;
		aci_gatt_add_char(ledger_ble_data.ledger_gatt_service_handle,
		                  UUID_TYPE_128,
		                  (const Char_UUID_t*)charUuidRX2,
		                  MAX_MTU_SIZE,
		                  CHAR_PROP_WRITE_WITHOUT_RESP,
		                  ATTR_PERMISSION_AUTHEN_WRITE,
		                  GATT_NOTIFY_ATTRIBUTE_WRITE,
		                  MAX_ENCRY_KEY_SIZE,
		                  CHAR_VALUE_LEN_VARIABLE,
		                  &ledger_ble_data.ledger_gatt_write_cmd_characteristic_handle);
		break;

	case BLE_INIT_STEP_SET_TX_POWER_LEVEL:
		ledger_ble_data.hci_cmd_opcode = 0xfc0f;
		aci_hal_set_tx_power_level(1,     // High power (ignored)
#ifdef TARGET_STAX
		                           0x19); // 0 dBm
#else // !TARGET_STAX
		                           0x07); // -14.1 dBm
#endif // !TARGET_STAX
		break;

	case BLE_INIT_STEP_CONFIGURE_ADVERTISING:
		ledger_ble_data.hci_cmd_opcode = 0xFFFF;
		ledger_ble_data.adv_step       = BLE_CONFIG_ADV_STEP_IDLE;
		ledger_ble_data.adv_enable     = !os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
		configure_advertising_mngr(0);
		break;

	case BLE_INIT_STEP_END:
		LOG_BLE("INIT END\n");
		if (ledger_ble_data.clear_pairing == 0xC1) {
			ledger_ble_data.clear_pairing = 0;
			aci_gap_clear_security_db();
		}
		G_io_app.ble_ready = 1;
		ledger_ble_data.state = BLE_STATE_INITIALIZED;
		break;

	default:
		break;
	}
}

static void hci_evt_cmd_complete(uint8_t *buffer, uint16_t length)
{
	if (length < 3) {
		return;
	}

	uint16_t opcode = U2LE(buffer, 1);

	if (ledger_ble_data.state == BLE_STATE_INITIALIZING) {
		init_mngr(opcode, &buffer[3], length);
	}
	else if (ledger_ble_data.state == BLE_STATE_CONFIGURE_ADVERTISING) {
		configure_advertising_mngr(opcode);
	}
	else if (opcode == 0xfd26) {
		// ACI_GATT_WRITE_RESP
		if (ledger_ble_data.wait_write_resp_ack != 0) {
			ledger_ble_data.wait_write_resp_ack = 0;
			if (ledger_protocol_data.tx_chunk_length >= 2) {
				G_io_app.ble_xfer_timeout = 2000;
				notify_chunk();
			}
		}
	}
	else if (opcode == 0xfd06) {
		// ACI_GATT_UPDATE_CHAR_VALUE
		ledger_protocol_data.tx_chunk_length = 0;
		if (ledger_ble_data.transfer_mode_enable) {
			if (  (ledger_protocol_data.rx_apdu_length)
			    &&(ledger_protocol_data.rx_apdu_status == APDU_STATUS_COMPLETE)
			   ) {
				memcpy(G_io_apdu_buffer,
				       ledger_protocol_data.rx_apdu_buffer,
				       ledger_protocol_data.rx_apdu_length);
				G_io_app.apdu_length = ledger_protocol_data.rx_apdu_length;
				G_io_app.apdu_state  = APDU_BLE;
				ledger_protocol_data.rx_apdu_length = 0;
			}
		}
		else {
			if (ledger_protocol_data.tx_apdu_buffer) {
				LEDGER_PROTOCOL_tx(NULL, 0);
				notify_chunk();
			}
			if (!ledger_protocol_data.tx_apdu_buffer) {
				ledger_protocol_data.tx_chunk_length = 0;
				G_io_app.ble_xfer_timeout = 0;
				G_io_app.apdu_state       = APDU_IDLE;
				if (  (!ledger_ble_data.connection_updated)
				    &&(ledger_ble_data.connection.conn_interval > BLE_SLAVE_CONN_INTERVAL_MIN)
				   ) {
					ledger_ble_data.connection_updated = 1;
					aci_l2cap_connection_parameter_update_req(ledger_ble_data.connection.connection_handle,
					                                          BLE_SLAVE_CONN_INTERVAL_MIN, BLE_SLAVE_CONN_INTERVAL_MIN,
					                                          ledger_ble_data.connection.conn_latency,
					                                          ledger_ble_data.connection.supervision_timeout);
				}
			}
		}
		G_io_app.apdu_media = IO_APDU_MEDIA_BLE;
	}
	else if (  (opcode == 0xfc81)
	         ||(opcode == 0xfc83)
	        ) {
		LOG_BLE("HCI_LE_SET_ADVERTISE_ENABLE %04X %d %d\n", ledger_ble_data.connection.connection_handle,
		                                                    G_io_app.disabling_advertising,
		                                                    G_io_app.enabling_advertising);
		if (ledger_ble_data.connection.connection_handle != 0xFFFF) {
			if (G_io_app.disabling_advertising) {
				// Connected & ordered to disable ble, force disconnection
				end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_FAILED);
				LEDGER_BLE_init();
			}
		}
		else if (G_io_app.disabling_advertising) {
			ledger_ble_data.advertising_enabled = 0;
			if (G_io_app.name_changed) {
				start_advertising();
			}
		}
		else if (G_io_app.enabling_advertising) {
			ledger_ble_data.advertising_enabled = 1;
		}
		else {
			ledger_ble_data.advertising_enabled = 1;
		}
		G_io_app.disabling_advertising = 0;
		G_io_app.enabling_advertising  = 0;
	}
	else if (opcode == 0xfca5) {
		LOG_BLE("ACI_GAP_NUMERIC_COMPARISON_VALUE_CONFIRM_YESNO\n");
	}
	else if (opcode == 0xfc94) {
		LOG_BLE("ACI_GAP_CLEAR_SECURITY_DB\n");
	}
	else if (opcode == 0xfd25) {
		LOG_BLE("ACI_GATT_CONFIRM_INDICATION\n");
	}
	else {
		LOG_BLE("HCI EVT CMD COMPLETE 0x%04X\n", opcode);
	}
}

static void hci_evt_le_meta_evt(uint8_t *buffer, uint16_t length)
{
	if (!length) {
		return;
	}

	switch (buffer[0]) {

	case HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE:
		ledger_ble_data.connection.connection_handle     = U2LE(buffer, 2);
		ledger_ble_data.connection.role_slave            = buffer[4];
		ledger_ble_data.connection.peer_address_random   = buffer[5];
		memcpy(ledger_ble_data.connection.peer_address,   &buffer[6], 6);
		ledger_ble_data.connection.conn_interval         = U2LE(buffer, 12);
		ledger_ble_data.connection.conn_latency          = U2LE(buffer, 14);
		ledger_ble_data.connection.supervision_timeout   = U2LE(buffer, 16);
		ledger_ble_data.connection.master_clock_accuracy = buffer[18];
		ledger_ble_data.connection.encrypted             = 0;
		ledger_ble_data.transfer_mode_enable             = 0;
		LOG_BLE("LE CONNECTION COMPLETE %04X - %04X- %04X- %04X\n", ledger_ble_data.connection.connection_handle,
		                                                            ledger_ble_data.connection.conn_interval,
		                                                            ledger_ble_data.connection.conn_latency,
		                                                            ledger_ble_data.connection.supervision_timeout);
		ledger_protocol_data.mtu              = ATT_MTU-3+2;
		ledger_ble_data.notifications_enabled = 0;
		ledger_ble_data.advertising_enabled   = 0;
		ledger_protocol_data.mtu_negotiated   = 0;
		ledger_ble_data.connection_updated    = 0;
		break;

	case HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE:
		ledger_ble_data.connection.connection_handle     = U2LE(buffer, 2);
		ledger_ble_data.connection.conn_interval         = U2LE(buffer, 4);
		ledger_ble_data.connection.conn_latency          = U2LE(buffer, 6);
		ledger_ble_data.connection.supervision_timeout   = U2LE(buffer, 8);
		LOG_BLE("LE CONNECTION UPDATE %04X - %04X- %04X- %04X\n", ledger_ble_data.connection.connection_handle,
		                                                          ledger_ble_data.connection.conn_interval,
		                                                          ledger_ble_data.connection.conn_latency,
		                                                          ledger_ble_data.connection.supervision_timeout);
		break;

	case HCI_LE_DATA_LENGTH_CHANGE_SUBEVT_CODE:
		if (U2LE(buffer, 1) == ledger_ble_data.connection.connection_handle) {
			ledger_ble_data.connection.max_tx_octets = U2LE(buffer, 3);
			ledger_ble_data.connection.max_tx_time   = U2LE(buffer, 5);
			ledger_ble_data.connection.max_rx_octets = U2LE(buffer, 7);
			ledger_ble_data.connection.max_rx_time   = U2LE(buffer, 9);
			LOG_BLE("LE DATA LENGTH CHANGE %04X - %04X- %04X- %04X\n", ledger_ble_data.connection.max_tx_octets,
			                                                           ledger_ble_data.connection.max_tx_time,
			                                                           ledger_ble_data.connection.max_rx_octets,
			                                                           ledger_ble_data.connection.max_rx_time);
		}
		break;

	case HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE:
		if (U2LE(buffer, 2) == ledger_ble_data.connection.connection_handle) {
			ledger_ble_data.connection.tx_phy = buffer[4];
			ledger_ble_data.connection.rx_phy = buffer[5];
			LOG_BLE("LE PHY UPDATE %02X - %02X\n", ledger_ble_data.connection.tx_phy,
			                                       ledger_ble_data.connection.rx_phy);
		}
		break;

	default:
		LOG_BLE("HCI LE META 0x%02X\n", buffer[0]);
		break;
	}
}

static void hci_evt_vendor(uint8_t *buffer, uint16_t length)
{
	if (length < 4) {
		return;
	}

	uint16_t opcode = U2LE(buffer, 0);

	if (U2LE(buffer, 2) != ledger_ble_data.connection.connection_handle) {
		return;
	}

	switch (opcode) {

	case ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE:
		LOG_BLE("PAIRING");
		switch (buffer[4]) {

		case SMP_PAIRING_STATUS_SUCCESS:
			LOG_BLE(" SUCCESS\n");
			end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_SUCCESS);
			break;

		case SMP_PAIRING_STATUS_SMP_TIMEOUT:
			LOG_BLE(" TIMEOUT\n");
			end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_TIMEOUT);
			break;

		case SMP_PAIRING_STATUS_PAIRING_FAILED:
			LOG_BLE(" FAILED : %02X\n", buffer[5]);
			if (buffer[5] == 0x08) { // UNSPECIFIED_REASON
				end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CANCELLED_FROM_REMOTE);
			}
			else {
				end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_FAILED);
			}
			break;

		default:
			end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_FAILED);
			break;
		}
		ledger_ble_data.pairing_in_progress = 0;
		break;

	case ACI_GAP_PASS_KEY_REQ_VSEVT_CODE:
		LOG_BLE("PASSKEY REQ\n");
		ask_user_pairing_passkey();
		break;

	case ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE:
		LOG_BLE("NUMERIC COMP : %d\n", U4LE(buffer, 4));
		ask_user_pairing_numeric_comparison(U4LE(buffer, 4));
		break;

	case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
		attribute_modified(&buffer[4], length-4);
		break;

	case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
		ledger_protocol_data.mtu            = U2LE(buffer, 4)-3+2;
		ledger_protocol_data.mtu_negotiated = 1;
		LOG_BLE("MTU : %d\n", U2LE(buffer, 4));
		break;

	case ACI_L2CAP_CONNECTION_UPDATE_RESP_VSEVT_CODE:
		LOG_BLE("CONNECTION UPDATE RESP %d\n", buffer[4]);
		break;

	case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
		write_permit_request(&buffer[4], length-4);
		break;

	case ACI_GATT_INDICATION_VSEVT_CODE:
		LOG_BLE("INDICATION EVT\n");
		aci_gatt_confirm_indication(ledger_ble_data.connection.connection_handle);
		break;

	case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
		LOG_BLE("PROCEDURE COMPLETE\n");
		break;

	case ACI_GATT_PROC_TIMEOUT_VSEVT_CODE:
		LOG_BLE("PROCEDURE TIMEOUT\n");
		end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_FAILED);
		LEDGER_BLE_init();
		break;

	default:
		LOG_BLE("HCI VENDOR 0x%04X\n", opcode);
		break;
	}
}

static void end_pairing_ux(uint8_t pairing_ok)
{
	bolos_ux_params_t ux_params;

	LOG_BLE("end_pairing_ux : %d (%d)\n", pairing_ok, ledger_ble_data.pairing_in_progress);
	if (ledger_ble_data.pairing_in_progress) {
		ux_params.ux_id = BOLOS_UX_ASYNCHMODAL_PAIRING_CANCEL;
		ux_params.u.pairing_status.pairing_ok = pairing_ok;
		ux_params.len = sizeof(ux_params.u.pairing_status);
		G_io_asynch_ux_callback.asynchmodal_end_callback = NULL;
		os_ux(&ux_params);
	}
}

static void ask_user_pairing_numeric_comparison(uint32_t code)
{
	bolos_ux_params_t ux_params;

	ux_params.u.pairing_request.type = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST_NUMCOMP;
	ux_params.u.pairing_request.pairing_info_len = 6;
	SPRINTF(ux_params.u.pairing_request.pairing_info, "%06d", (unsigned int)code);
	ux_params.ux_id = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST;
	ux_params.len = sizeof(ux_params.u.pairing_request);
	G_io_asynch_ux_callback.asynchmodal_end_callback = rsp_user_pairing_numeric_comparison;
	ledger_ble_data.pairing_in_progress = 1;
	os_ux(&ux_params);
}

static void rsp_user_pairing_numeric_comparison(unsigned int status)
{
	if (status == BOLOS_UX_OK) {
		end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CONFIRM_CODE_YES);
		aci_gap_numeric_comparison_value_confirm_yesno(ledger_ble_data.connection.connection_handle, 1);
	}
	else if (status == BOLOS_UX_IGNORE) {
		ledger_ble_data.pairing_in_progress = 0;
		aci_gap_numeric_comparison_value_confirm_yesno(ledger_ble_data.connection.connection_handle, 0);
	}
	else {
		end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CONFIRM_CODE_NO);
		aci_gap_numeric_comparison_value_confirm_yesno(ledger_ble_data.connection.connection_handle, 0);
	}
}

static void ask_user_pairing_passkey(void)
{
	bolos_ux_params_t ux_params;

	ledger_ble_data.pairing_code = cx_rng_u32_range_func(0, 1000000, cx_rng_u32);
	ux_params.u.pairing_request.type = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST_PASSKEY;
	ux_params.u.pairing_request.pairing_info_len = 6;
	SPRINTF(ux_params.u.pairing_request.pairing_info, "%06d", ledger_ble_data.pairing_code);
	ux_params.ux_id = BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST;
	ux_params.len = sizeof(ux_params.u.pairing_request);
	G_io_asynch_ux_callback.asynchmodal_end_callback = rsp_user_pairing_passkey;
	ledger_ble_data.pairing_in_progress = 1;
	os_ux(&ux_params);
}

static void rsp_user_pairing_passkey(unsigned int status)
{
	if (status != BOLOS_UX_OK) { // BLE_TODO
		end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_ACCEPT_PASSKEY);
		ledger_ble_data.pairing_code = cx_rng_u32_range_func(0, 1000000, cx_rng_u32);
	}
	else {
		end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CANCEL_PASSKEY);
	}
	aci_gap_pass_key_resp(ledger_ble_data.connection.connection_handle,
	                      ledger_ble_data.pairing_code);
}

static void attribute_modified(uint8_t *buffer, uint16_t length)
{
	if (length < 6) {
		return;
	}

	uint16_t att_handle      = U2LE(buffer, 0);
	uint16_t offset          = U2LE(buffer, 2);
	uint16_t att_data_length = U2LE(buffer, 4);

	if (  (att_handle == ledger_ble_data.ledger_gatt_notification_characteristic_handle+2)
	    &&(att_data_length == 2)
	    &&(offset == 0)
	   ) {
		// Peer device registering/unregistering for notifications
		if (U2LE(buffer, 6) != 0) {
			LOG_BLE("REGISTERED FOR NOTIFICATIONS\n");
			ledger_ble_data.notifications_enabled = 1;
			if (!ledger_protocol_data.mtu_negotiated) {
				aci_gatt_exchange_config(ledger_ble_data.connection.connection_handle);
			}
		}
		else {
			LOG_BLE("NOT REGISTERED FOR NOTIFICATIONS\n");
			ledger_ble_data.notifications_enabled = 0;
		}
	}
	else if (  (att_handle == ledger_ble_data.ledger_gatt_write_cmd_characteristic_handle+1)
	         &&(ledger_ble_data.notifications_enabled)
	         &&(ledger_ble_data.connection.encrypted)
	         &&(att_data_length)
	   ) {
		LOG_BLE("WRITE CMD %d\n", length-4);
		buffer[4] = 0xDE;
		buffer[5] = 0xF1;
		LEDGER_PROTOCOL_rx(&buffer[4], length-4);

		if (ledger_protocol_data.rx_apdu_status == APDU_STATUS_COMPLETE) {
			check_transfer_mode(G_io_app.transfer_mode);
			if (ledger_ble_data.transfer_mode_enable) {
				if (U2BE(ledger_ble_data.resp, 0) != SWO_SUCCESS) {
					LOG_BLE("Transfer failed 0x%04x\n", U2BE(ledger_ble_data.resp, 0));
					G_io_app.transfer_mode = 0;
					check_transfer_mode(G_io_app.transfer_mode);
					memcpy(G_io_apdu_buffer,
					       ledger_protocol_data.rx_apdu_buffer,
					       ledger_protocol_data.rx_apdu_length);
					G_io_app.apdu_length = ledger_protocol_data.rx_apdu_length;
					G_io_app.apdu_state  = APDU_BLE;
					ledger_protocol_data.rx_apdu_length = 0;
					ledger_protocol_data.rx_apdu_status = APDU_STATUS_WAITING;
					G_io_app.apdu_media                 = IO_APDU_MEDIA_BLE;
				}
				else if (ledger_ble_data.resp_length) {
					LEDGER_PROTOCOL_tx(ledger_ble_data.resp, ledger_ble_data.resp_length);
					ledger_ble_data.resp_length = 0;
					notify_chunk();
				}
			}
			else {
				memcpy(G_io_apdu_buffer,
				       ledger_protocol_data.rx_apdu_buffer,
				       ledger_protocol_data.rx_apdu_length);
				G_io_app.apdu_length = ledger_protocol_data.rx_apdu_length;
				G_io_app.apdu_state  = APDU_BLE;
				ledger_protocol_data.rx_apdu_length = 0;
				ledger_protocol_data.rx_apdu_status = APDU_STATUS_WAITING;
				G_io_app.apdu_media                 = IO_APDU_MEDIA_BLE;
			}
		}
		else if (ledger_protocol_data.tx_chunk_length >= 2) {
			G_io_app.ble_xfer_timeout = 2000;
			notify_chunk();
			G_io_app.apdu_state = APDU_BLE;
		}
	}
	else {
		LOG_BLE("ATT MODIFIED %04X %d bytes at offset %d\n", att_handle, att_data_length, offset);
	}
}

static void write_permit_request(uint8_t *buffer, uint16_t length)
{
	if (length < 3) {
		return;
	}

	uint16_t att_handle  = U2LE(buffer, 0);
	uint8_t  data_length = buffer[2];

	ledger_ble_data.wait_write_resp_ack = 1;

	if (  (att_handle == ledger_ble_data.ledger_gatt_write_characteristic_handle+1)
	    &&(ledger_ble_data.notifications_enabled)
	    &&(ledger_ble_data.connection.encrypted)
	    &&(data_length)
	   ) {
		buffer[1] = 0xDE;
		buffer[2] = 0xF1;
		LEDGER_PROTOCOL_rx(&buffer[1], length-1);
		aci_gatt_write_resp(ledger_ble_data.connection.connection_handle,
		                    att_handle,
		                    0,
		                    HCI_SUCCESS_ERR_CODE,
		                    data_length,
		                    &buffer[3]);
		if (ledger_protocol_data.rx_apdu_status == APDU_STATUS_COMPLETE) {
			memcpy(G_io_apdu_buffer,
			       ledger_protocol_data.rx_apdu_buffer,
			       ledger_protocol_data.rx_apdu_length);
			G_io_app.apdu_length = ledger_protocol_data.rx_apdu_length;
			ledger_protocol_data.rx_apdu_length = 0;
			ledger_protocol_data.rx_apdu_status = APDU_STATUS_WAITING;
			G_io_app.apdu_media = IO_APDU_MEDIA_BLE; // for application code
			G_io_app.apdu_state = APDU_BLE; // for next call to io_exchange
		}
	}
	else {
		LOG_BLE("ATT WRITE %04X %d bytes\n", att_handle, data_length);
		ledger_protocol_data.tx_chunk_length = 0;
		aci_gatt_write_resp(ledger_ble_data.connection.connection_handle,
		                    att_handle,
		                    0,
		                    HCI_SUCCESS_ERR_CODE,
		                    data_length,
		                    &buffer[3]);
	}
}

static void advertising_enable(uint8_t enable)
{
	if (enable) {
		uint8_t buffer[31];

		get_device_name();
		buffer[0] = AD_TYPE_COMPLETE_LOCAL_NAME;
		memcpy(&buffer[1], ledger_ble_data.device_name, ledger_ble_data.device_name_length);
		aci_gap_set_discoverable(ADV_IND,
		                         BLE_ADVERTISING_INTERVAL_MIN,
		                         BLE_ADVERTISING_INTERVAL_MAX,
		                         RANDOM_ADDR,
		                         NO_WHITE_LIST_USE,
		                         ledger_ble_data.device_name_length+1,
		                         buffer,
		                         0,
		                         NULL,
		                         0,
		                         0);
	}
	else {
		aci_gap_set_non_discoverable();
	}
}

static void start_advertising(void)
{
	if (G_io_app.name_changed) {
		G_io_app.name_changed = 0;
		ledger_ble_data.state    = BLE_STATE_CONFIGURE_ADVERTISING;
		ledger_ble_data.adv_step = BLE_CONFIG_ADV_STEP_IDLE;
	}
	else {
		ledger_ble_data.state    = BLE_STATE_CONFIGURE_ADVERTISING;
		ledger_ble_data.adv_step = BLE_CONFIG_ADV_STEP_START-1;
	}
	ledger_ble_data.hci_cmd_opcode = 0xFFFF;
	ledger_ble_data.adv_enable     = !os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
	configure_advertising_mngr(0);
}

static void notify_chunk(void)
{
	if (ledger_protocol_data.tx_chunk_length >= 2) {
		aci_gatt_update_char_value(ledger_ble_data.ledger_gatt_service_handle,
		                           ledger_ble_data.ledger_gatt_notification_characteristic_handle,
		                           0,
		                           ledger_protocol_data.tx_chunk_length-2,
		                           &ledger_protocol_data.tx_chunk[2]);
	}
}

static void check_transfer_mode(uint8_t enable)
{
	if (ledger_ble_data.transfer_mode_enable != enable) {
		LOG_BLE("LEDGER_BLE_set_transfer_mode %d\n", enable);
	}

	if (  (ledger_ble_data.transfer_mode_enable == 0)
	    &&(enable != 0)
	   ) {
		ledger_ble_data.resp_length = 2;
		U2BE_ENCODE(ledger_ble_data.resp, 0, SWO_SUCCESS);
	}

	ledger_ble_data.transfer_mode_enable = enable;
}

/* Exported functions --------------------------------------------------------*/
void LEDGER_BLE_init(void)
{
	G_io_app.enabling_advertising  = 0;
	G_io_app.disabling_advertising = 0;

	if (ledger_ble_data.clear_pairing == 0xC1) {
		memset(&ledger_ble_data, 0, sizeof(ledger_ble_data));
		ledger_ble_data.clear_pairing = 0xC1;
	}
	else {
		memset(&ledger_ble_data, 0, sizeof(ledger_ble_data));
	}

	LEDGER_BLE_get_mac_address(ledger_ble_data.random_address);
	ledger_ble_data.hci_cmd_opcode = 0xFFFF;
	ledger_ble_data.state          = BLE_STATE_INITIALIZING;
	ledger_ble_data.init_step      = BLE_INIT_STEP_IDLE;

	memset(&ledger_protocol_data, 0, sizeof(ledger_protocol_data));
	ledger_protocol_data.rx_apdu_buffer            = ledger_ble_data.apdu_buffer;
	ledger_protocol_data.rx_apdu_buffer_max_length = sizeof(ledger_ble_data.apdu_buffer);
	LEDGER_PROTOCOL_init(&ledger_protocol_data);

	init_mngr(0, NULL, 0);
}

void LEDGER_BLE_send(uint8_t* packet, uint16_t packet_length)
{
	if (  (ledger_ble_data.transfer_mode_enable != 0)
	    &&(packet_length == 2)
	   ) {
		G_io_app.apdu_state         = APDU_IDLE;
		ledger_ble_data.resp_length = 2;
		ledger_ble_data.resp[0]     = packet[0];
		ledger_ble_data.resp[1]     = packet[1];
		if (ledger_protocol_data.rx_apdu_length) {
			LEDGER_PROTOCOL_tx(packet, packet_length);
			notify_chunk();
		}
	}
	else {
		if (  (ledger_ble_data.resp_length != 0)
		    &&(U2BE(ledger_ble_data.resp, 0) != SWO_SUCCESS)
		   ) {
			LEDGER_PROTOCOL_tx(ledger_ble_data.resp, ledger_ble_data.resp_length);
		}
		else {
			LEDGER_PROTOCOL_tx(packet, packet_length);
		}
		ledger_ble_data.resp_length = 0;

		if (ledger_ble_data.wait_write_resp_ack == 0) {
			notify_chunk();
		}
	}
}

void LEDGER_BLE_receive(void)
{
	if (G_io_seproxyhal_spi_buffer[3] == HCI_EVENT_PKT_TYPE) {
		switch (G_io_seproxyhal_spi_buffer[4]) {

		case HCI_DISCONNECTION_COMPLETE_EVT_CODE:
			LOG_BLE("HCI DISCONNECTION COMPLETE code %02X\n", G_io_seproxyhal_spi_buffer[9]);
			ledger_ble_data.connection.connection_handle = 0xFFFF;
			ledger_ble_data.advertising_enabled          = 0;
			ledger_ble_data.connection.encrypted         = 0;
			end_pairing_ux(BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_FAILED);
			if (G_io_seproxyhal_spi_buffer[9] != 0x28) { // Error code : Instant Passed
				start_advertising();
			}
			else {
				// Workaround to avoid unexpected start advertising event in loop (seems to be a bug in the stack)
				LEDGER_BLE_init();
			}
			break;

		case HCI_ENCRYPTION_CHANGE_EVT_CODE:
			if (U2LE(G_io_seproxyhal_spi_buffer, 7) == ledger_ble_data.connection.connection_handle) {
				if (G_io_seproxyhal_spi_buffer[9]) {
					LOG_BLE("Link encrypted\n");
					ledger_ble_data.connection.encrypted = 1;
				}
				else {
					LOG_BLE("Link not encrypted\n");
					ledger_ble_data.connection.encrypted = 0;
				}
			}
			else {
				LOG_BLE("HCI ENCRYPTION CHANGE EVT %d on connection handle \n", G_io_seproxyhal_spi_buffer[9],
				                                                                U2LE(G_io_seproxyhal_spi_buffer, 7));
			}
			break;

		case HCI_COMMAND_COMPLETE_EVT_CODE:
			hci_evt_cmd_complete(&G_io_seproxyhal_spi_buffer[6],
			                     G_io_seproxyhal_spi_buffer[5]);
			break;

		case HCI_COMMAND_STATUS_EVT_CODE:
			LOG_BLE("HCI COMMAND_STATUS\n");
			break;

		case HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVT_CODE:
			LOG_BLE("HCI KEY_REFRESH_COMPLETE\n");
			break;

		case HCI_LE_META_EVT_CODE:
			hci_evt_le_meta_evt(&G_io_seproxyhal_spi_buffer[6],
			                    G_io_seproxyhal_spi_buffer[5]);
			break;

		case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
			hci_evt_vendor(&G_io_seproxyhal_spi_buffer[6],
			               G_io_seproxyhal_spi_buffer[5]);
			break;

		default:
			break;
		}
	}
}

void LEDGER_BLE_enable_advertising(uint8_t enable)
{
	if (  (G_io_app.name_changed)
	    &&(G_io_app.ble_ready)
	    &&(!enable)
	    &&(ledger_ble_data.connection.connection_handle != 0xFFFF)
	   ) {
		G_io_app.name_changed = 0;
	}
	else if (G_io_app.ble_ready) {
		if (enable) {
			G_io_app.enabling_advertising = 1;
			G_io_app.disabling_advertising = 0;
		}
		else {
			G_io_app.enabling_advertising = 0;
			G_io_app.disabling_advertising = 1;
		}
		advertising_enable(enable);
	}
}

void LEDGER_BLE_reset_pairings(void)
{
	if (G_io_app.ble_ready) {
		if (ledger_ble_data.connection.connection_handle != 0xFFFF) {
			// Connected => force disconnection before clearing
			ledger_ble_data.clear_pairing = 0xC1;
			LEDGER_BLE_init();
		}
		else {
			aci_gap_clear_security_db();
		}
	}
}

int hci_send_req(struct hci_request *p_cmd, uint8_t async)
{
	UNUSED(async);

	uint16_t opcode = ((p_cmd->ocf) & 0x03ff) | ((p_cmd->ogf) << 10);

	G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLE_SEND;
	G_io_seproxyhal_spi_buffer[1] = (p_cmd->clen+2)>>8;
	G_io_seproxyhal_spi_buffer[2] = (p_cmd->clen+2);
	G_io_seproxyhal_spi_buffer[3] = opcode >>8;
	G_io_seproxyhal_spi_buffer[4] = opcode;
	memmove(G_io_seproxyhal_spi_buffer+5, p_cmd->cparam, p_cmd->clen);
	io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3+p_cmd->clen+2);

	return 0;
}

void BLE_power(unsigned char powered, const char *discovered_name) {
	UNUSED(discovered_name);

	LOG_BLE("BLE_power %d\n", powered);
	if (powered) {
		LEDGER_BLE_init();
	}
}
