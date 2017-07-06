/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : hci.h
* Author             : AMS - HEA&RF BU
* Version            : V1.0.0
* Date               : 19-July-2012
* Description        : Constants and functions for HCI layer. See Bluetooth Core
*                       v 4.0, Vol. 2, Part E.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __HCI_H_
#define __HCI_H_

#include "hal_types.h"
#include "link_layer.h"
#include <list.h>

#ifndef HCI_READ_PACKET_SIZE
#define HCI_READ_PACKET_SIZE   128 //71
#endif // HCI_READ_PACKET_SIZE


/*** Data types ***/

/* structure used to read received data */
typedef struct _tHciDataPacket
{
    tListNode currentNode;
	uint8_t dataBuff[HCI_READ_PACKET_SIZE];
    uint8_t data_len;
}tHciDataPacket;

/**
 * @defgroup HCI_Error_codes HCI Error codes
 * @{
 */
#define HCI_UNKNOWN_COMMAND					0x01
#define HCI_NO_CONNECTION					0x02
#define HCI_HARDWARE_FAILURE				0x03
#define HCI_PAGE_TIMEOUT					0x04
#define HCI_AUTHENTICATION_FAILURE			0x05
#define HCI_PIN_OR_KEY_MISSING				0x06
#define HCI_MEMORY_FULL						0x07
#define HCI_CONNECTION_TIMEOUT				0x08
#define HCI_MAX_NUMBER_OF_CONNECTIONS		0x09
#define HCI_MAX_NUMBER_OF_SCO_CONNECTIONS	0x0a
#define HCI_ACL_CONNECTION_EXISTS			0x0b
#define HCI_COMMAND_DISALLOWED				0x0c
#define HCI_REJECTED_LIMITED_RESOURCES		0x0d
#define HCI_REJECTED_SECURITY				0x0e
#define HCI_REJECTED_PERSONAL				0x0f
#define HCI_HOST_TIMEOUT					0x10
#define HCI_UNSUPPORTED_FEATURE				0x11
#define HCI_INVALID_PARAMETERS				0x12
#define HCI_OE_USER_ENDED_CONNECTION		0x13
#define HCI_OE_LOW_RESOURCES				0x14
#define HCI_OE_POWER_OFF					0x15
#define HCI_CONNECTION_TERMINATED			0x16
#define HCI_REPEATED_ATTEMPTS				0x17
#define HCI_PAIRING_NOT_ALLOWED				0x18
#define HCI_UNKNOWN_LMP_PDU					0x19
#define HCI_UNSUPPORTED_REMOTE_FEATURE		0x1a
#define HCI_SCO_OFFSET_REJECTED				0x1b
#define HCI_SCO_INTERVAL_REJECTED			0x1c
#define HCI_AIR_MODE_REJECTED				0x1d
#define HCI_INVALID_LMP_PARAMETERS			0x1e
#define HCI_UNSPECIFIED_ERROR				0x1f
#define HCI_UNSUPPORTED_LMP_PARAMETER_VALUE	0x20
#define HCI_ROLE_CHANGE_NOT_ALLOWED			0x21
#define HCI_LMP_RESPONSE_TIMEOUT			0x22
#define HCI_LMP_ERROR_TRANSACTION_COLLISION	0x23
#define HCI_LMP_PDU_NOT_ALLOWED				0x24
#define HCI_ENCRYPTION_MODE_NOT_ACCEPTED	0x25
#define HCI_UNIT_LINK_KEY_USED				0x26
#define HCI_QOS_NOT_SUPPORTED				0x27
#define HCI_INSTANT_PASSED					0x28
#define HCI_PAIRING_NOT_SUPPORTED			0x29
#define HCI_TRANSACTION_COLLISION			0x2a
#define HCI_QOS_UNACCEPTABLE_PARAMETER		0x2c
#define HCI_QOS_REJECTED					0x2d
#define HCI_CLASSIFICATION_NOT_SUPPORTED	0x2e
#define HCI_INSUFFICIENT_SECURITY			0x2f
#define HCI_PARAMETER_OUT_OF_RANGE			0x30
#define HCI_ROLE_SWITCH_PENDING				0x32
#define HCI_SLOT_VIOLATION					0x34
#define HCI_ROLE_SWITCH_FAILED				0x35
#define HCI_EIR_TOO_LARGE					0x36
#define HCI_SIMPLE_PAIRING_NOT_SUPPORTED	0x37
#define HCI_HOST_BUSY_PAIRING				0x38
#define HCI_CONN_REJ_NO_CH_FOUND			0x39
#define HCI_CONTROLLER_BUSY					0x3A
#define HCI_UNACCEPTABLE_CONN_INTERV		0x3B
#define HCI_DIRECTED_ADV_TIMEOUT			0x3C
#define HCI_CONN_TERM_MIC_FAIL				0x3D
#define HCI_CONN_FAIL_TO_BE_ESTABL			0x3E
#define HCI_MAC_CONN_FAILED					0x3F
/**
 * @}
 */


/*
 * HCI library functions.
 * Each function returns 0 in case of success, otherwise one of the error codes.
 */

int hci_reset(void);

int hci_disconnect(uint16_t	handle, uint8_t reason);

int hci_le_set_advertise_enable(uint8_t enable);

int hci_le_set_advertising_parameters(uint16_t min_interval, uint16_t max_interval, uint8_t advtype,
		uint8_t own_bdaddr_type, uint8_t direct_bdaddr_type, const tBDAddr direct_bdaddr, uint8_t chan_map,
		uint8_t filter);

int hci_le_set_scan_parameters(uint8_t	type, uint16_t interval,
                               uint16_t window, uint8_t own_bdaddr_type,
                               uint8_t	filter);

int hci_le_set_scan_enable(uint8_t enable, uint8_t filter_dup);

int hci_le_set_advertising_data(uint8_t length, const uint8_t data[]);

int hci_le_set_scan_resp_data(uint8_t length, const uint8_t data[]);

int hci_le_rand(uint8_t random_number[8]);

int hci_le_read_advertising_channel_tx_power(int8_t *tx_power_level);

int hci_acl_data(const uint8_t * data, uint16_t len);

int hci_le_set_random_address(tBDAddr bdaddr);

int hci_read_bd_addr(tBDAddr bdaddr);

int hci_le_read_white_list_size(uint8_t *size);

int hci_le_clear_white_list();

int hci_le_add_device_to_white_list(uint8_t	bdaddr_type, tBDAddr bdaddr);

int hci_le_remove_device_from_white_list(uint8_t bdaddr_type, tBDAddr bdaddr);

int hci_le_encrypt(uint8_t key[16], uint8_t plaintextData[16], uint8_t encryptedData[16]);

int hci_le_ltk_request_reply(uint8_t key[16]);

int hci_le_ltk_request_neg_reply();

int hci_le_read_buffer_size(uint16_t *pkt_len, uint8_t *max_pkt);

int hci_le_create_connection(uint16_t interval,	uint16_t window, uint8_t initiator_filter, uint8_t peer_bdaddr_type,
                             const tBDAddr peer_bdaddr,	uint8_t	own_bdaddr_type, uint16_t min_interval,	uint16_t max_interval,
                             uint16_t latency,	uint16_t supervision_timeout, uint16_t min_ce_length, uint16_t max_ce_length);

int hci_read_transmit_power_level(uint16_t *conn_handle, uint8_t type, int8_t * tx_level);

int hci_read_rssi(uint16_t *conn_handle, int8_t * rssi);

int hci_le_read_local_supported_features(uint8_t *features);

int hci_le_read_channel_map(uint16_t conn_handle, uint8_t ch_map[5]);

int hci_le_read_supported_states(uint8_t states[8]);

int hci_le_receiver_test(uint8_t frequency);

int hci_le_transmitter_test(uint8_t frequency, uint8_t length, uint8_t payload);

int hci_le_test_end(uint16_t *num_pkts);

int hci_le_read_local_version(uint8_t *hci_version, uint16_t *hci_revision, uint8_t *lmp_pal_version, 
			      uint16_t *manufacturer_name, uint16_t *lmp_pal_subversion);

/**
 * Initialization function. Must be done before any data can be received from
 * BLE controller.
 */
void HCI_Init(void);

/**
 * Callback used to pass events to application.
 *
 * @param[in] pckt    The event.
 *
 */
extern void HCI_Event_CB(void *pckt);

/**
 * Processing function that must be called after an event is received from
 * HCI interface. Must be called outside ISR. It will call HCI_Event_CB if
 * necessary.
*/
void HCI_Process(void);


/**
 * @brief Check if queue of HCI event is empty or not.
 * @note This funtion can be used to check if the event queue from BlueNRG is empty. This
 *        is useful when checking if it is safe to go to sleep. 
 * @return TRUE if event queue is empty. FALSE otherwhise.
 */
uint8_t HCI_Queue_Empty(void);

/**
 * Iterrupt service routine that must be called when the BlueNRG 
 * reports a packet received or an event to the host through the 
 * BlueNRG interrupt line.
 */
void HCI_recv_packet(unsigned char* packet_buffer, unsigned int packet_length);

#endif /* __HCI_H_ */
