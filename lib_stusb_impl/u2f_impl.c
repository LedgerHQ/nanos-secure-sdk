
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#ifdef HAVE_IO_U2F

#include <stdint.h>
#include <string.h>
#include "os.h"
#include "os_io_seproxyhal.h"
#include "cx.h"
#include "u2f_service.h"
#include "u2f_transport.h"
#include "u2f_processing.h"

#define INIT_U2F_VERSION 0x02
#define INIT_DEVICE_VERSION_MAJOR 0
#define INIT_DEVICE_VERSION_MINOR 1
#define INIT_BUILD_VERSION 0
#define INIT_CAPABILITIES 0x00

#define FIDO_CLA 0x00
#define FIDO_INS_ENROLL 0x01
#define FIDO_INS_SIGN 0x02
#define U2F_HANDLE_SIGN_HEADER_SIZE (32+32+1)
#define FIDO_INS_GET_VERSION 0x03

#define FIDO_INS_PROP_GET_COUNTER 0xC0 // U2F_VENDOR_FIRST
#define FIDO_INS_PROP_GET_INFO    0xC1 // grab the max message buffer size on 1 byte

#define P1_SIGN_CHECK_ONLY 0x07
#define P1_SIGN_SIGN 0x03

#define SIGN_USER_PRESENCE_MASK 0x01

static const uint8_t SW_BUSY[] = {0x90, 0x01};
static const uint8_t SW_PROOF_OF_PRESENCE_REQUIRED[] = {0x69, 0x85};
static const uint8_t SW_BAD_KEY_HANDLE[] = {0x6A, 0x80};

static const uint8_t SW_UNKNOWN_INSTRUCTION[] = {0x6d, 0x00};
static const uint8_t SW_UNKNOWN_CLASS[] = {0x6e, 0x00};
static const uint8_t SW_WRONG_LENGTH[] = {0x67, 0x00};
static const uint8_t SW_INTERNAL[] = {0x6F, 0x00};

#ifdef U2F_PROXY_MAGIC

static const uint8_t U2F_VERSION[] = {'U', '2', 'F', '_', 'V', '2', 0x90, 0x00};

// take into account max header (u2f usb)
static const uint8_t INFO[] = {1 /*info format 1*/, (char)(IO_APDU_BUFFER_SIZE - U2F_HANDLE_SIGN_HEADER_SIZE - 3 - 4), 0x90, 0x00};


// proxy mode enroll issue an error
void u2f_apdu_enroll(u2f_service_t *service, uint8_t p1, uint8_t p2,
                       uint8_t *buffer, uint16_t length) {
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(buffer);
    UNUSED(length);

    u2f_message_reply(service, U2F_CMD_MSG, (uint8_t *)SW_INTERNAL, sizeof(SW_INTERNAL));
}

void u2f_apdu_sign(u2f_service_t *service, uint8_t p1, uint8_t p2,
                     uint8_t *buffer, uint16_t length) {
    UNUSED(p2);
    uint8_t keyHandleLength;
    uint8_t i;

    // can't process the apdu if another one is already scheduled in
    if (G_io_app.apdu_state != APDU_IDLE) {
        u2f_message_reply(service, U2F_CMD_MSG,
                  (uint8_t *)SW_BUSY,
                  sizeof(SW_BUSY));
        return;        
    }

    if (length < U2F_HANDLE_SIGN_HEADER_SIZE + 5 /*at least an apdu header*/) {
        u2f_message_reply(service, U2F_CMD_MSG,
                  (uint8_t *)SW_WRONG_LENGTH,
                  sizeof(SW_WRONG_LENGTH));
        return;
    }

    // Confirm immediately if it's just a validation call
    if (p1 == P1_SIGN_CHECK_ONLY) {
        u2f_message_reply(service, U2F_CMD_MSG,
                  (uint8_t *)SW_PROOF_OF_PRESENCE_REQUIRED,
                  sizeof(SW_PROOF_OF_PRESENCE_REQUIRED));
        return;
    }

    // Unwrap magic
    keyHandleLength = buffer[U2F_HANDLE_SIGN_HEADER_SIZE-1];
    
    // reply to the "get magic" question of the host
    if (keyHandleLength == 5) {
        // GET U2F PROXY PARAMETERS
        // this apdu is not subject to proxy magic masking
        // APDU is F1 D0 00 00 00 to get the magic proxy
        // RAPDU: <>
        if (os_memcmp(buffer+U2F_HANDLE_SIGN_HEADER_SIZE, "\xF1\xD0\x00\x00\x00", 5) == 0 ) {
            // U2F_PROXY_MAGIC is given as a 0 terminated string
            G_io_apdu_buffer[0] = sizeof(U2F_PROXY_MAGIC)-1;
            os_memmove(G_io_apdu_buffer+1, U2F_PROXY_MAGIC, sizeof(U2F_PROXY_MAGIC)-1);
            os_memmove(G_io_apdu_buffer+1+sizeof(U2F_PROXY_MAGIC)-1, "\x90\x00\x90\x00", 4);
            u2f_message_reply(service, U2F_CMD_MSG,
                              (uint8_t *)G_io_apdu_buffer,
                              G_io_apdu_buffer[0]+1+2+2);
            // processing finished. don't go further in the u2f msg processing
            return;
        }
    }
    

    for (i = 0; i < keyHandleLength; i++) {
        buffer[U2F_HANDLE_SIGN_HEADER_SIZE + i] ^= U2F_PROXY_MAGIC[i % (sizeof(U2F_PROXY_MAGIC)-1)];
    }
    // Check that it looks like an APDU
    if (length != U2F_HANDLE_SIGN_HEADER_SIZE + 5 + buffer[U2F_HANDLE_SIGN_HEADER_SIZE + 4]) {
        u2f_message_reply(service, U2F_CMD_MSG,
                  (uint8_t *)SW_BAD_KEY_HANDLE,
                  sizeof(SW_BAD_KEY_HANDLE));
        return;
    }

    // make the apdu available to higher layers
    os_memmove(G_io_apdu_buffer, buffer + U2F_HANDLE_SIGN_HEADER_SIZE, keyHandleLength);
    G_io_app.apdu_length = keyHandleLength;
    G_io_app.apdu_media = IO_APDU_MEDIA_U2F; // the effective transport is managed by the U2F layer
    G_io_app.apdu_state = APDU_U2F;

    // prepare for asynch reply
    u2f_message_set_autoreply_wait_user_presence(service, true);

    // don't reset the u2f processing command state, as we still await for the io_exchange caller to make the response call
    /*
    app_dispatch();
    if ((btchip_context_D.io_flags & IO_ASYNCH_REPLY) == 0) {
        u2f_proxy_response(service, btchip_context_D.outLength);
    }
    */
}

void u2f_apdu_get_version(u2f_service_t *service, uint8_t p1, uint8_t p2,
                            uint8_t *buffer, uint16_t length) {
    // screen_printf("U2F version\n");
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(buffer);
    UNUSED(length);
    u2f_message_reply(service, U2F_CMD_MSG, (uint8_t *)U2F_VERSION, sizeof(U2F_VERSION));
}

// Special command that returns the proxy 
void u2f_apdu_get_info(u2f_service_t *service, uint8_t p1, uint8_t p2,
                            uint8_t *buffer, uint16_t length) {
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(buffer);
    UNUSED(length);
    u2f_message_reply(service, U2F_CMD_MSG, (uint8_t *)INFO, sizeof(INFO));
}

#endif // U2F_PROXY_MAGIC

void u2f_handle_cmd_init(u2f_service_t *service, uint8_t *buffer,
                         uint16_t length, uint8_t *channelInit) {
    // screen_printf("U2F init\n");
    uint8_t channel[4];
    (void)length;
    if (u2f_is_channel_broadcast(channelInit)) {
        // cx_rng(channel, 4); // not available within the IO task
        U4BE_ENCODE(channel, 0, ++service->next_channel);
    } else {
        os_memmove(channel, channelInit, 4);
    }
    os_memmove(G_io_apdu_buffer, buffer, 8);
    os_memmove(G_io_apdu_buffer + 8, channel, 4);
    G_io_apdu_buffer[12] = INIT_U2F_VERSION;
    G_io_apdu_buffer[13] = INIT_DEVICE_VERSION_MAJOR;
    G_io_apdu_buffer[14] = INIT_DEVICE_VERSION_MINOR;
    G_io_apdu_buffer[15] = INIT_BUILD_VERSION;
    G_io_apdu_buffer[16] = INIT_CAPABILITIES;

    if (u2f_is_channel_broadcast(channelInit)) {
        os_memset(service->channel, 0xff, 4);
    } else {
        os_memmove(service->channel, channel, 4);
    }
    u2f_message_reply(service, U2F_CMD_INIT, G_io_apdu_buffer, 17);
}

void u2f_handle_cmd_ping(u2f_service_t *service, uint8_t *buffer,
                         uint16_t length) {
    // screen_printf("U2F ping\n");
    u2f_message_reply(service, U2F_CMD_PING, buffer, length);
}

void u2f_handle_cmd_msg(u2f_service_t *service, uint8_t *buffer,
                        uint16_t length) {
    // screen_printf("U2F msg\n");
    uint8_t cla = buffer[0];
    uint8_t ins = buffer[1];
    uint8_t p1 = buffer[2];
    uint8_t p2 = buffer[3];
    // in extended length buffer[4] must be 0
    uint32_t dataLength = /*(buffer[4] << 16) |*/ (buffer[5] << 8) | (buffer[6]);
    if (dataLength == (uint16_t)(length - 9) || dataLength == (uint16_t)(length - 7)) {
        // Le is optional
        // nominal case from the specification
    }
    // circumvent google chrome extended length encoding done on the last byte only (module 256) but all data being transferred
    else if (dataLength == (uint16_t)(length - 9)%256) {
        dataLength = length - 9;
    }
    else if (dataLength == (uint16_t)(length - 7)%256) {
        dataLength = length - 7;
    }    
    else { 
        // invalid size
        u2f_message_reply(service, U2F_CMD_MSG,
                  (uint8_t *)SW_WRONG_LENGTH,
                  sizeof(SW_WRONG_LENGTH));
        return;
    }

#ifndef U2F_PROXY_MAGIC

    // No proxy mode, just pass the APDU as it is to the upper layer
    os_memmove(G_io_apdu_buffer, buffer, length);
    G_io_app.apdu_length = length;
    G_io_app.apdu_media = IO_APDU_MEDIA_U2F; // the effective transport is managed by the U2F layer
    G_io_app.apdu_state = APDU_U2F;

#else // U2F_PROXY_MAGIC

    if (cla != FIDO_CLA) {
        u2f_message_reply(service, U2F_CMD_MSG,
                  (uint8_t *)SW_UNKNOWN_CLASS,
                  sizeof(SW_UNKNOWN_CLASS));
        return;
    }
    switch (ins) {
    case FIDO_INS_ENROLL:
        // screen_printf("enroll\n");
        u2f_apdu_enroll(service, p1, p2, buffer + 7, dataLength);
        break;
    case FIDO_INS_SIGN:
        // screen_printf("sign\n");
        u2f_apdu_sign(service, p1, p2, buffer + 7, dataLength);
        break;
    case FIDO_INS_GET_VERSION:
        // screen_printf("version\n");
        u2f_apdu_get_version(service, p1, p2, buffer + 7, dataLength);
        break;

    // only support by 
    case FIDO_INS_PROP_GET_INFO:
        u2f_apdu_get_info(service, p1, p2, buffer + 7, dataLength);
        break;

    default:
        // screen_printf("unsupported\n");
        u2f_message_reply(service, U2F_CMD_MSG,
                 (uint8_t *)SW_UNKNOWN_INSTRUCTION,
                 sizeof(SW_UNKNOWN_INSTRUCTION));
        return;
    }

#endif // U2F_PROXY_MAGIC
}

void u2f_message_complete(u2f_service_t *service) {
    uint8_t cmd = service->transportBuffer[0];
    uint16_t length = (service->transportBuffer[1] << 8) | (service->transportBuffer[2]);
    switch (cmd) {
    case U2F_CMD_INIT:
        u2f_handle_cmd_init(service, service->transportBuffer + 3, length, service->channel);
        break;
    case U2F_CMD_PING:
        u2f_handle_cmd_ping(service, service->transportBuffer + 3, length);
        break;
    case U2F_CMD_MSG:
        u2f_handle_cmd_msg(service, service->transportBuffer + 3, length);
        break;
    }
}

#endif
