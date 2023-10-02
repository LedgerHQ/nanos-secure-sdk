
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

#ifndef __U2F_TRANSPORT_H__

#define __U2F_TRANSPORT_H__

#include "u2f_service.h"

// Shared commands
#define U2F_CMD_PING 0x81
#define U2F_CMD_MSG 0x83
#define CTAP2_CMD_CBOR 0x90 // FIDO2
#define CTAP2_CMD_CANCEL 0x91 // FIDO2

// USB only commands
#define U2F_CMD_INIT 0x86
#define U2F_CMD_LOCK 0x84
#define U2F_CMD_WINK 0x88

// BLE only commands
#define U2F_CMD_KEEPALIVE 0x82
#define KEEPALIVE_REASON_PROCESSING 0x01
#define KEEPALIVE_REASON_TUP_NEEDED 0x02

#define U2F_STATUS_ERROR 0xBF
#define CTAP2_STATUS_KEEPALIVE 0xBB // FIDO2

// Shared errors
#define ERROR_NONE 0x00
#define ERROR_INVALID_CMD 0x01
#define ERROR_INVALID_PAR 0x02
#define ERROR_INVALID_LEN 0x03
#define ERROR_INVALID_SEQ 0x04
#define ERROR_MSG_TIMEOUT 0x05
#define ERROR_OTHER 0x7f
// USB only errors
#define ERROR_CHANNEL_BUSY 0x06
#define ERROR_LOCK_REQUIRED 0x0a
#define ERROR_INVALID_CID 0x0b
// CTAP2 errors 
#define ERROR_CBOR_UNEXPECTED_TYPE 0x11
#define ERROR_INVALID_CBOR 0x12
#define ERROR_MISSING_PARAMETER 0x14
#define ERROR_LIMIT_EXCEEDED 0x15
#define ERROR_UNSUPPORTED_EXTENSION 0x16
#define ERROR_CREDENTIAL_EXCLUDED 0x19
#define ERROR_PROCESSING 0x21
#define ERROR_INVALID_CREDENTIAL 0x22
#define ERROR_USER_ACTION_PENDING 0x23
#define ERROR_OPERATION_PENDING 0x24
#define ERROR_NO_OPERATIONS 0x25
#define ERROR_UNSUPPORTED_ALGORITHM 0x26
#define ERROR_OPERATION_DENIED 0x27
#define ERROR_KEY_STORE_FULL 0x28
#define ERROR_NO_OPERATION_PENDING 0x2A
#define ERROR_UNSUPPORTED_OPTION 0x2B
#define ERROR_INVALID_OPTION 0x2C
#define ERROR_KEEPALIVE_CANCEL 0x2D
#define ERROR_NO_CREDENTIALS 0x2E
#define ERROR_USER_ACTION_TIMEOUT 0x2F
#define ERROR_NOT_ALLOWED 0x30
#define ERROR_PIN_INVALID 0x31
#define ERROR_PIN_BLOCKED 0x32
#define ERROR_PIN_AUTH_INVALID 0x33
#define ERROR_PIN_AUTH_BLOCKED 0x34
#define ERROR_PIN_NOT_SET 0x35
#define ERROR_PIN_REQUIRED 0x36
#define ERROR_PIN_POLICY_VIOLATION 0x37
#define ERROR_PIN_TOKEN_EXPIRED 0x38
#define ERROR_REQUEST_TOO_LARGE 0x39
#define ERROR_ACTION_TIMEOUT 0x3A
#define ERROR_UP_REQUIRED 0x3B
// Proprietary errors
#define ERROR_PROP_UNKNOWN_COMMAND 0x80
#define ERROR_PROP_COMMAND_TOO_LONG 0x81
#define ERROR_PROP_INVALID_CONTINUATION 0x82
#define ERROR_PROP_UNEXPECTED_CONTINUATION 0x83
#define ERROR_PROP_CONTINUATION_OVERFLOW 0x84
#define ERROR_PROP_MESSAGE_TOO_SHORT 0x85
#define ERROR_PROP_UNCONSISTENT_MSG_LENGTH 0x86
#define ERROR_PROP_UNSUPPORTED_MSG_APDU 0x87
#define ERROR_PROP_INVALID_DATA_LENGTH_APDU 0x88
#define ERROR_PROP_INTERNAL_ERROR_APDU 0x89
#define ERROR_PROP_INVALID_PARAMETERS_APDU 0x8A
#define ERROR_PROP_INVALID_DATA_APDU 0x8B
#define ERROR_PROP_DEVICE_NOT_SETUP 0x8C
#define ERROR_PROP_MEDIA_MIXED 0x8D
#define ERROR_PROP_RPID_MEDIA_DENIED 0x8E

/**
 * Initialize the U2F transport library
 * Incoming messages are store in the provided message buffer. It can be overlapped with io APDU buffer to save RAM (but won't allow for multiple IO model (no mutual exclusion in the buffer access))
 */
void u2f_transport_init(u2f_service_t *service, uint8_t* message_buffer, uint16_t message_buffer_length);

/** 
 * Function to be called when an IO message has been sent.
 */
void u2f_transport_sent(u2f_service_t *service, u2f_transport_media_t media);

/**
 * Function to be called when an IO message has been received.
 */
void u2f_transport_received(u2f_service_t *service, uint8_t *buffer,
                          uint16_t size, u2f_transport_media_t media);

/**
 * Returns 0 if the provided channel buffer is not a broadcast channel
 */
bool u2f_is_channel_broadcast(uint8_t *channel);

/**
 * Returns 0 if the provided channel buffer is not a forbidden channel
 */
bool u2f_is_channel_forbidden(uint8_t *channel);

/**
 * Send a CTAP 2 KEEPALIVE command while processing a CBOR message
 */
void u2f_transport_ctap2_send_keepalive(u2f_service_t *service, uint8_t reason);

#endif
