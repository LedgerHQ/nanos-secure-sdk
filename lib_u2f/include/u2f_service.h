/*
*******************************************************************************
*   Portable FIDO U2F implementation
*   (c) 2016 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*   limitations under the License.
********************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef __U2F_SERVICE_H__

#define __U2F_SERVICE_H__

struct u2f_service_t;

typedef void (*u2fHandle_t)(struct u2f_service_t *service, uint8_t *inputBuffer,
                            uint8_t *channel);
typedef void (*u2fPromptUserPresence_t)(struct u2f_service_t *service,
                                        bool enroll,
                                        uint8_t *applicationParameter);
typedef void (*u2fTimer_t)(struct u2f_service_t *service);

typedef enum {
    U2F_IDLE,
    U2F_HANDLE_SEGMENTED,
    U2F_PROCESSING_COMMAND,
    U2F_SENDING_RESPONSE,
    U2F_SENDING_ERROR,
} u2f_transport_state_t;

typedef enum {
    U2F_MEDIA_NONE,
    U2F_MEDIA_USB,
    U2F_MEDIA_NFC,
    U2F_MEDIA_BLE
} u2f_transport_media_t;

typedef struct u2f_service_t {
    // Internal

    uint8_t channel[4];
    u2f_transport_media_t media;

    // buffer reference used when transport layer is reset, to process and receive the next message
    uint8_t *transportReceiveBuffer;
    // length of the receive buffer to avoid overflows
    uint16_t transportReceiveBufferLength;

    uint8_t transportChannel[4];
    uint16_t transportOffset;
    uint16_t transportLength;
    uint8_t transportPacketIndex;
    uint8_t *transportBuffer;
    u2f_transport_state_t transportState;
    u2f_transport_media_t transportMedia;

    u2fTimer_t timeoutFunction;
    uint32_t timerInterval;
    uint32_t seqTimeout;
    bool requireKeepalive;
    uint32_t keepaliveTimeout;

    uint8_t sendCmd;
} u2f_service_t;

void u2f_message_complete(u2f_service_t *service);

#endif
