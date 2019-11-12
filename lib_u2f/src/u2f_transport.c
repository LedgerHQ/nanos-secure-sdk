
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
#include "u2f_service.h"
#include "u2f_transport.h"
#include "u2f_processing.h"
#include "u2f_io.h"

#include "os.h"
#include "os_io_seproxyhal.h"

#define U2F_MASK_COMMAND 0x80
#define U2F_COMMAND_HEADER_SIZE 3

static const uint8_t BROADCAST_CHANNEL[] = {0xff, 0xff, 0xff, 0xff};
static const uint8_t FORBIDDEN_CHANNEL[] = {0x00, 0x00, 0x00, 0x00};

#warning TODO take into account the INIT during SEGMENTED message correctly (avoid erasing the first part of the apdu buffer when doing so)

// init
void u2f_transport_reset(u2f_service_t* service) {
    service->transportState = U2F_IDLE;
    service->transportOffset = 0;
    service->transportMedia = 0;
    service->transportPacketIndex = 0;
    service->fakeChannelTransportState = U2F_IDLE;
    service->fakeChannelTransportOffset = 0;
    service->fakeChannelTransportPacketIndex = 0;    
    service->sending = false;
    service->waitAsynchronousResponse = U2F_WAIT_ASYNCH_IDLE;
    // reset the receive buffer to allow for a new message to be received again (in case transmission of a CODE buffer the previous reply)
    service->transportBuffer = service->transportReceiveBuffer;
}

/**
 * Initialize the u2f transport and provide the buffer into which to store incoming message
 */
void u2f_transport_init(u2f_service_t *service, uint8_t* message_buffer, uint16_t message_buffer_length) {
    service->transportReceiveBuffer = message_buffer;
    service->transportReceiveBufferLength = message_buffer_length;
    u2f_transport_reset(service);
}

/**
 * Reply an error at the U2F transport level (take into account the FIDO U2F framing)
 */
static void u2f_transport_error(u2f_service_t *service, char errorCode) {
    //u2f_transport_reset(service); // warning reset first to allow for U2F_io sent call to u2f_transport_sent internally on eventless platforms
    G_io_usb_ep_buffer[8] = errorCode;

    // ensure the state is set to error sending to allow for special treatment in case reply is not read by the receiver
    service->transportState = U2F_SENDING_ERROR;
    service->transportPacketIndex = 0;
    service->transportBuffer = G_io_usb_ep_buffer + 8;
    service->transportOffset = 0;
    service->transportLength = 1;
    service->sendCmd = U2F_STATUS_ERROR;
    // pump the first message, with the reception media
    u2f_transport_sent(service, service->media);
}

/**
 * Function called when the previously scheduled message to be sent on the media is effectively sent.
 * And a new message can be scheduled.
 */
void u2f_transport_sent(u2f_service_t* service, u2f_transport_media_t media) {

    // don't process when replying to anti timeout requests
    if (!u2f_message_repliable(service)) {
        // previous mark packet as sent
        service->sending = false;
        return;
    }

    // previous mark packet as sent
    service->sending = false;

    // if idle (possibly after an error), then only await for a transmission 
    if (service->transportState != U2F_SENDING_RESPONSE 
        && service->transportState != U2F_SENDING_ERROR) {
        // absorb the error, transport is erroneous but that won't hurt in the end.
        // also absorb the fake channel user presence check reply ack
        //THROW(INVALID_STATE);
        return;
    }
    if (service->transportOffset < service->transportLength) {
        uint16_t mtu = (media == U2F_MEDIA_USB) ? USB_SEGMENT_SIZE : BLE_SEGMENT_SIZE;
        uint16_t channelHeader =
            (media == U2F_MEDIA_USB ? 4 : 0);
        uint8_t headerSize =
            (service->transportPacketIndex == 0 ? (channelHeader + 3)
                                                : (channelHeader + 1));
        uint16_t blockSize = ((service->transportLength - service->transportOffset) >
                                      (mtu - headerSize)
                                  ? (mtu - headerSize)
                                  : service->transportLength - service->transportOffset);
        uint16_t dataSize = blockSize + headerSize;
        uint16_t offset = 0;
        // Fragment
        if (media == U2F_MEDIA_USB) {
            os_memmove(G_io_usb_ep_buffer, service->channel, 4);
            offset += 4;
        }
        if (service->transportPacketIndex == 0) {
            G_io_usb_ep_buffer[offset++] = service->sendCmd;
            G_io_usb_ep_buffer[offset++] = (service->transportLength >> 8);
            G_io_usb_ep_buffer[offset++] = (service->transportLength & 0xff);
        } else {
            G_io_usb_ep_buffer[offset++] = (service->transportPacketIndex - 1);
        }
        if (service->transportBuffer != NULL) {
            os_memmove(G_io_usb_ep_buffer + headerSize,
                       service->transportBuffer + service->transportOffset, blockSize);
        }
        service->transportOffset += blockSize;
        service->transportPacketIndex++;
        u2f_io_send(G_io_usb_ep_buffer, dataSize, media);
    }
    // last part sent
    else if (service->transportOffset == service->transportLength) {
        u2f_transport_reset(service);
        // we sent the whole response (even if we haven't yet received the ack for the last sent usb in packet)
        G_io_app.apdu_state = APDU_IDLE;
    }
}

void u2f_transport_send_usb_user_presence_required(u2f_service_t *service) {
    uint16_t offset = 0;
    service->sending = true;
    os_memmove(G_io_usb_ep_buffer, service->channel, 4);
    offset += 4;
    G_io_usb_ep_buffer[offset++] = U2F_CMD_MSG;
    G_io_usb_ep_buffer[offset++] = 0;
    G_io_usb_ep_buffer[offset++] = 2;
    G_io_usb_ep_buffer[offset++] = 0x69;
    G_io_usb_ep_buffer[offset++] = 0x85;
    u2f_io_send(G_io_usb_ep_buffer, offset, U2F_MEDIA_USB);
}

void u2f_transport_send_wink(u2f_service_t *service) {
    uint16_t offset = 0;
    service->sending = true;
    os_memmove(G_io_usb_ep_buffer, service->channel, 4);
    offset += 4;
    G_io_usb_ep_buffer[offset++] = U2F_CMD_WINK;
    G_io_usb_ep_buffer[offset++] = 0;
    G_io_usb_ep_buffer[offset++] = 0;
    u2f_io_send(G_io_usb_ep_buffer, offset, U2F_MEDIA_USB);
}

bool u2f_transport_receive_fakeChannel(u2f_service_t *service, uint8_t *buffer, uint16_t size) {
    if (service->fakeChannelTransportState == U2F_INTERNAL_ERROR) {
        return false;
    }
    if (memcmp(service->channel, buffer, 4) != 0) {
        goto error;
    }
    if (service->fakeChannelTransportOffset == 0) {        
        uint16_t commandLength = U2BE(buffer, 4+1) + U2F_COMMAND_HEADER_SIZE;
        // Some buggy implementations can send a WINK here, reply it gently
        if (buffer[4] == U2F_CMD_WINK) {
            u2f_transport_send_wink(service);
            return true;
        }

        if (commandLength != service->transportLength) {
            goto error;
        }
        if (buffer[4] != U2F_CMD_MSG) {
            goto error;
        }
        service->fakeChannelTransportOffset = MIN(size - 4, service->transportLength);
        service->fakeChannelTransportPacketIndex = 0;
        service->fakeChannelCrc = cx_crc16_update(0, buffer + 4, service->fakeChannelTransportOffset);
    }
    else {
        if (buffer[4] != service->fakeChannelTransportPacketIndex) {
            goto error;
        }
        uint16_t xfer_len = MIN(size - 5, service->transportLength - service->fakeChannelTransportOffset);
        service->fakeChannelTransportPacketIndex++;
        service->fakeChannelTransportOffset += xfer_len;
        service->fakeChannelCrc = cx_crc16_update(service->fakeChannelCrc, buffer + 5, xfer_len);   
    }
    if (service->fakeChannelTransportOffset >= service->transportLength) {
        if (service->fakeChannelCrc != service->commandCrc) {
            goto error;
        }
        service->fakeChannelTransportState = U2F_FAKE_RECEIVED;
        service->fakeChannelTransportOffset = 0;
        // reply immediately when the asynch response is not yet ready
        if (service->waitAsynchronousResponse == U2F_WAIT_ASYNCH_ON) {
            u2f_transport_send_usb_user_presence_required(service);
            // response sent
            service->fakeChannelTransportState = U2F_IDLE;
        }
    }
    return true;
error:
    service->fakeChannelTransportState = U2F_INTERNAL_ERROR;
    // don't hesitate here, the user will have to exit/rerun the app otherwise.
    THROW(EXCEPTION_IO_RESET);
    return false;    
}


/** 
 * Function that process every message received on a media.
 * Performs message concatenation when message is splitted.
 */
void u2f_transport_received(u2f_service_t *service, uint8_t *buffer,
                          uint16_t size, u2f_transport_media_t media) {
    uint16_t channelHeader = (media == U2F_MEDIA_USB ? 4 : 0);
    uint16_t xfer_len;
    service->media = media;

    // Handle a busy channel and avoid reentry
    if (service->transportState == U2F_SENDING_RESPONSE) {
        u2f_transport_error(service, ERROR_CHANNEL_BUSY);
        goto error;
    }
    if (service->waitAsynchronousResponse != U2F_WAIT_ASYNCH_IDLE) {
        if (!u2f_transport_receive_fakeChannel(service, buffer, size)) {
            u2f_transport_error(service, ERROR_CHANNEL_BUSY);
            goto error;
        }
        return;
    }
    
    // SENDING_ERROR is accepted, and triggers a reset => means the host hasn't consumed the error.
    if (service->transportState == U2F_SENDING_ERROR) {
        u2f_transport_reset(service);
    }

    if (size < (1 + channelHeader)) {
        // Message to short, abort
        u2f_transport_error(service, ERROR_PROP_MESSAGE_TOO_SHORT);
        goto error;
    }
    if (media == U2F_MEDIA_USB) {
        // hold the current channel value to reply to, for example, INIT commands within flow of segments.
        os_memmove(service->channel, buffer, 4);
    }

    // no previous chunk processed for the current message
    if (service->transportOffset == 0
        // on USB we could get an INIT within a flow of segments.
        || (media == U2F_MEDIA_USB && os_memcmp(service->transportChannel, service->channel, 4) != 0) ) {
        if (size < (channelHeader + 3)) {
            // Message to short, abort
            u2f_transport_error(service, ERROR_PROP_MESSAGE_TOO_SHORT);
            goto error;
        }
        // check this is a command, cannot accept continuation without previous command
        if ((buffer[channelHeader+0]&U2F_MASK_COMMAND) == 0) {
            // Not a command packet, abort
            u2f_transport_error(service, ERROR_INVALID_SEQ);
            goto error;
        }

        // If waiting for a continuation on a different channel, reply BUSY
        // immediately
        if (media == U2F_MEDIA_USB) {
            if ((service->transportState == U2F_HANDLE_SEGMENTED) &&
                (os_memcmp(service->channel, service->transportChannel, 4) !=
                 0) &&
                (buffer[channelHeader] != U2F_CMD_INIT)) {
                // special error case, we reply but don't change the current state of the transport (ongoing message for example)
                //u2f_transport_error_no_reset(service, ERROR_CHANNEL_BUSY);
                uint16_t offset = 0;
                // Fragment
                if (media == U2F_MEDIA_USB) {
                    os_memmove(G_io_usb_ep_buffer, service->channel, 4);
                    offset += 4;
                }
                G_io_usb_ep_buffer[offset++] = U2F_STATUS_ERROR;
                G_io_usb_ep_buffer[offset++] = 0;
                G_io_usb_ep_buffer[offset++] = 1;
                G_io_usb_ep_buffer[offset++] = ERROR_CHANNEL_BUSY;
                u2f_io_send(G_io_usb_ep_buffer, offset, media);
                goto error;
            }
        }
        // If a command was already sent, and we are not processing a INIT
        // command, abort
        if ((service->transportState == U2F_HANDLE_SEGMENTED) &&
            !((media == U2F_MEDIA_USB) &&
              (buffer[channelHeader] == U2F_CMD_INIT))) {
            // Unexpected continuation at this stage, abort
            u2f_transport_error(service, ERROR_INVALID_SEQ);
            goto error;
        }
        // Check the length
        uint16_t commandLength = U2BE(buffer, channelHeader + 1);
        if (commandLength > (service->transportReceiveBufferLength - 3)) {
            // Overflow in message size, abort
            u2f_transport_error(service, ERROR_INVALID_LEN);
            goto error;
        }
        // Check if the command is supported
        switch (buffer[channelHeader]) {
        case U2F_CMD_PING:
        case U2F_CMD_MSG:
            if (media == U2F_MEDIA_USB) {
                if (u2f_is_channel_broadcast(service->channel) ||
                    u2f_is_channel_forbidden(service->channel)) {
                    u2f_transport_error(service, ERROR_INVALID_CID);
                    goto error;
                }
            }
            // no channel for BLE
            break;
        case U2F_CMD_INIT:
            if (media != U2F_MEDIA_USB) {
                // Unknown command, abort
                u2f_transport_error(service, ERROR_INVALID_CMD);
                goto error;
            }

            if (u2f_is_channel_forbidden(service->channel)) {
                u2f_transport_error(service, ERROR_INVALID_CID);
                goto error;
            }

            break;
        default:
            // Unknown command, abort
            u2f_transport_error(service, ERROR_INVALID_CMD);
            goto error;
        }

        // Ok, initialize the buffer
        //if (buffer[channelHeader] != U2F_CMD_INIT) 
        {
            xfer_len = MIN(size - (channelHeader), U2F_COMMAND_HEADER_SIZE+commandLength);
            os_memmove(service->transportBuffer, buffer + channelHeader, xfer_len);
            if (media == U2F_MEDIA_USB) {
                service->commandCrc = cx_crc16_update(0, service->transportBuffer, xfer_len);
            }
            service->transportOffset = xfer_len;
            service->transportLength = U2F_COMMAND_HEADER_SIZE+commandLength;
            service->transportMedia = media;
            // initialize the response
            service->transportPacketIndex = 0;
            os_memmove(service->transportChannel, service->channel, 4);
        }
    } else {
        // Continuation
        if (size < (channelHeader + 2)) {
            // Message to short, abort
            u2f_transport_error(service, ERROR_PROP_MESSAGE_TOO_SHORT);
            goto error;
        }
        if (media != service->transportMedia) {
            // Mixed medias
            u2f_transport_error(service, ERROR_PROP_MEDIA_MIXED);
            goto error;
        }
        if (service->transportState != U2F_HANDLE_SEGMENTED) {
            // Unexpected continuation at this stage, abort
            // TODO : review the behavior is HID only
            if (media == U2F_MEDIA_USB) {
                u2f_transport_reset(service);
                goto error;
            } else {
                u2f_transport_error(service, ERROR_INVALID_SEQ);
                goto error;
            }
        }
        if (media == U2F_MEDIA_USB) {
            // Check the channel
            if (os_memcmp(buffer, service->channel, 4) != 0) {
                u2f_transport_error(service, ERROR_CHANNEL_BUSY);
                goto error;
            }
        }
        // also discriminate invalid command sent instead of a continuation
        if (buffer[channelHeader] != service->transportPacketIndex) {
            // Bad continuation packet, abort
            u2f_transport_error(service, ERROR_INVALID_SEQ);
            goto error;
        }
        xfer_len = MIN(size - (channelHeader + 1), service->transportLength - service->transportOffset);
        os_memmove(service->transportBuffer + service->transportOffset, buffer + channelHeader + 1, xfer_len);
        if (media == U2F_MEDIA_USB) {
            service->commandCrc = cx_crc16_update(service->commandCrc, service->transportBuffer + service->transportOffset, xfer_len);
        }        
        service->transportOffset += xfer_len;
        service->transportPacketIndex++;
    }
    // See if we can process the command
    if ((media != U2F_MEDIA_USB) &&
        (service->transportOffset >
         (service->transportLength + U2F_COMMAND_HEADER_SIZE))) {
        // Overflow, abort
        u2f_transport_error(service, ERROR_INVALID_LEN);
        goto error;
    } else if (service->transportOffset >= service->transportLength) {
        // switch before the handler gets the opportunity to change it again
        service->transportState = U2F_PROCESSING_COMMAND;
        // internal notification of a complete message received
        u2f_message_complete(service);
    } else {
        // new segment received, reset the timeout for the current piece
        service->seqTimeout = 0;
        service->transportState = U2F_HANDLE_SEGMENTED;
    }
error:
    return;
}

bool u2f_is_channel_broadcast(uint8_t *channel) {
    return (os_memcmp(channel, BROADCAST_CHANNEL, 4) == 0);
}

bool u2f_is_channel_forbidden(uint8_t *channel) {
    return (os_memcmp(channel, FORBIDDEN_CHANNEL, 4) == 0);
}

/**
 * Auto reply hodl until the real reply is prepared and sent
 */
void u2f_message_set_autoreply_wait_user_presence(u2f_service_t* service, bool enabled) {

    if (enabled) {
        // start replying placeholder until user presence validated
        if (service->waitAsynchronousResponse == U2F_WAIT_ASYNCH_IDLE) {
            service->waitAsynchronousResponse = U2F_WAIT_ASYNCH_ON;
            u2f_transport_send_usb_user_presence_required(service);
        }
    }
    // don't set to REPLY_READY when it has not been enabled beforehand
    else if (service->waitAsynchronousResponse == U2F_WAIT_ASYNCH_ON) {
        service->waitAsynchronousResponse = U2F_WAIT_ASYNCH_REPLY_READY;
    }
}

bool u2f_message_repliable(u2f_service_t* service) {
    // no more asynch replies
    // finished receiving the command
    // and not sending a user presence required status
    return service->waitAsynchronousResponse == U2F_WAIT_ASYNCH_IDLE
        || (service->waitAsynchronousResponse != U2F_WAIT_ASYNCH_ON 
            && service->fakeChannelTransportState == U2F_FAKE_RECEIVED 
            && service->sending == false)
        ;
}

void u2f_message_reply(u2f_service_t *service, uint8_t cmd, uint8_t *buffer, uint16_t len) {

    // if U2F is not ready to reply, then gently avoid replying
    if (u2f_message_repliable(service)) 
    {
        service->transportState = U2F_SENDING_RESPONSE;
        service->transportPacketIndex = 0;
        service->transportBuffer = buffer;
        service->transportOffset = 0;
        service->transportLength = len;
        service->sendCmd = cmd;
        if (service->transportMedia != U2F_MEDIA_BLE) {
            // pump the first message
            u2f_transport_sent(service, service->transportMedia);
        }
        else {
            while (G_io_app.apdu_state != APDU_IDLE) {
                u2f_transport_sent(service, service->transportMedia);       
            }
        }
    }
}


#endif
