/*******************************************************************************
*   Ledger Blue - Non secure firmware
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
*   See the License for the specific language governing permissions and
*   limitations under the License.
********************************************************************************/

#ifndef SEPROXYHAL_PROTOCOL_H
#define SEPROXYHAL_PROTOCOL_H

#define BLE_CMD_APDU 0x05
#define BLE_CHUNK_LENGTH 20
#define M24SR_CHUNK_LENGTH 0xF6

// EVENTS
#define SEPROXYHAL_TAG_SESSION_START_EVENT                                     \
    0x01 // <kind(1byte)=reqble|recovery|flashback> <featurebitmap(4bytesBE)>
         // <seproxyhalversion(L(1)V(L))>
#define SEPROXYHAL_TAG_SESSION_START_EVENT_REQBLE 0x01
#define SEPROXYHAL_TAG_SESSION_START_EVENT_RECOVERY 0x02
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_USB 0x00000001UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_BLE 0x00000002UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_TOUCH 0x00000004UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_BUTTON_COUNT 0x000000F0UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_BUTTON_COUNT_POS 4
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_SCREEN_MASK 0x00000F00UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_NO_SCREEN 0x00000000UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_SCREEN_BIG 0x00000100UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_SCREEN_SML 0x00000200UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_LEDRGB 0x00001000UL

#define SEPROXYHAL_TAG_BLE_PAIRING_ATTEMPT_EVENT 0x02
#define SEPROXYHAL_TAG_BLE_WRITE_REQUEST_EVENT 0x03
#define SEPROXYHAL_TAG_BLE_READ_REQUEST_EVENT 0x04
#define SEPROXYHAL_TAG_BUTTON_PUSH_EVENT 0x05
#define SEPROXYHAL_TAG_BUTTON_PUSH_ID_MASK                                     \
    0xFE // up to 7 physical buttons (bit is 1 when pressed, and 0 when
         // released)
#define SEPROXYHAL_TAG_BUTTON_PUSH_INTERVAL_MS                                 \
    100 // an event generated every x ms
#define SEPROXYHAL_TAG_NFC_FIELD_DETECTION_EVENT 0x06
#define SEPROXYHAL_TAG_NFC_APDU_RECEIVED_EVENT 0x07
#define SEPROXYHAL_TAG_BATTERY_NOTIFICATION_EVENT 0x08
#define SEPROXYHAL_TAG_M24SR_GPO_CHANGE_EVENT 0x09
#define SEPROXYHAL_TAG_M24SR_RESPONSE_APDU_EVENT 0x0A
#define SEPROXYHAL_TAG_BLE_NOTIFY_INDICATE_EVENT 0x0B
#define SEPROXYHAL_TAG_FINGER_EVENT 0x0C
#define SEPROXYHAL_TAG_FINGER_EVENT_TOUCH 0x01
#define SEPROXYHAL_TAG_FINGER_EVENT_RELEASE 0x02
#define SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT 0x0D
#define SEPROXYHAL_TAG_TICKER_EVENT 0x0E
#define SEPROXYHAL_TAG_USB_EVENT 0x0F // <connect/disconnect/suspend/resume>
#define SEPROXYHAL_TAG_USB_EVENT_RESET 0x01
#define SEPROXYHAL_TAG_USB_EVENT_SOF 0x02
#define SEPROXYHAL_TAG_USB_EVENT_SUSPENDED 0x04
#define SEPROXYHAL_TAG_USB_EVENT_RESUMED 0x08
#define SEPROXYHAL_TAG_USB_EP_XFER_EVENT                                       \
    0x10 // <epnum> <xferin/xferout/xfersetup> <datalength> <data>
#define SEPROXYHAL_TAG_USB_EP_XFER_SETUP 0x01
#define SEPROXYHAL_TAG_USB_EP_XFER_IN 0x02
#define SEPROXYHAL_TAG_USB_EP_XFER_OUT 0x04
#define SEPROXYHAL_TAG_BLE_CONNECTION_EVENT                                    \
    0x11 // <connected(1)|disconnected(0)>
#define SEPROXYHAL_TAG_UNSEC_CHUNK_EVENT 0x12
#define SEPROXYHAL_TAG_ACK_LINK_SPEED 0x13     // <ack=1|nack=0 (1byte)>
#define SEPROXYHAL_TAG_BLUENRG_RECV_EVENT 0x14 // <>

// COMMANDS
// #define SEPROXYHAL_TAG_MCU_BOOTLOADER              0x31 // DISABLED FOR
// SECURITY REASON
#define SEPROXYHAL_TAG_UNSEC_CHUNK_READ 0x32
#define SEPROXYHAL_TAG_SET_SCREEN_CONFIG                                       \
    0x3E // <flags(1byte):pwron(128)|rotation(0:0,90:2,180:4,270:6)|invert(1)>
         // <brightness percentage(1byte)>
#define SEPROXYHAL_TAG_SET_LINK_PROP 0x3F // <mode(1b):debug(1)>
#define SEPROXYHAL_TAG_BLUENRG_SEND                                            \
    0x40 // <CS mgmt: start (bit 1) | stop (bit 2)> <packet>
#define SEPROXYHAL_TAG_BLE_DEFINE_GENERIC_SETTING 0x41
#define SEPROXYHAL_TAG_BLE_DEFINE_SERVICE_SETTING 0x42
#define SEPROXYHAL_TAG_NFC_DEFINE_SERVICE_SETTING 0x43
#define SEPROXYHAL_TAG_BLE_RADIO_POWER 0x44
#define SEPROXYHAL_TAG_NFC_RADIO_POWER 0x45
#define SEPROXYHAL_TAG_SE_POWER_OFF 0x46
#define SEPROXYHAL_TAG_SCREEN_POWER 0x47
#define SEPROXYHAL_TAG_BLE_NOTIFY_INDICATE 0x48
#define SEPROXYHAL_TAG_BATTERY_LEVEL 0x49
#define SEPROXYHAL_TAG_SCREEN_DISPLAY 0x4A // wait for display_event after sent
#define SEPROXYHAL_TAG_DEVICE_OFF 0x4B
#define SEPROXYHAL_TAG_MORE_TIME 0x4C
#define SEPROXYHAL_TAG_M24SR_C_APDU 0x4D
#define SEPROXYHAL_TAG_SET_TICKER_INTERVAL 0x4E
#define SEPROXYHAL_TAG_USB_CONFIG                                              \
    0x4F // <connect/disconnect> <nbendpoints> [<epaddr>
         // <eptype:control/interrupt/bulk/isochrone/disabled> <epmps>]
#define SEPROXYHAL_TAG_USB_CONFIG_CONNECT 0x01
#define SEPROXYHAL_TAG_USB_CONFIG_DISCONNECT 0x02
#define SEPROXYHAL_TAG_USB_CONFIG_ADDR 0x03
#define SEPROXYHAL_TAG_USB_CONFIG_ENDPOINTS 0x04
#define SEPROXYHAL_TAG_USB_CONFIG_TYPE_DISABLED 0x00
#define SEPROXYHAL_TAG_USB_CONFIG_TYPE_CONTROL 0x01
#define SEPROXYHAL_TAG_USB_CONFIG_TYPE_INTERRUPT 0x02
#define SEPROXYHAL_TAG_USB_CONFIG_TYPE_BULK 0x03
#define SEPROXYHAL_TAG_USB_CONFIG_TYPE_ISOCHRONOUS 0x04
#define SEPROXYHAL_TAG_USB_EP_PREPARE                                          \
    0x50 // <epnum> <direction:setup/in/out/stall/unstall> <expected_length>
         // <data>
#define SEPROXYHAL_TAG_USB_EP_PREPARE_DIR_SETUP 0x10
#define SEPROXYHAL_TAG_USB_EP_PREPARE_DIR_IN 0x20
#define SEPROXYHAL_TAG_USB_EP_PREPARE_DIR_OUT 0x30
#define SEPROXYHAL_TAG_USB_EP_PREPARE_DIR_STALL 0x40
#define SEPROXYHAL_TAG_USB_EP_PREPARE_DIR_UNSTALL 0x80
#define SEPROXYHAL_TAG_SET_LED 0x51 // <ledID(1byte)> <ledcolorARGB(4byte)>

// STATUS
#define SEPROXYHAL_TAG_STATUS_MASK 0x60
#define SEPROXYHAL_TAG_GENERAL_STATUS 0x60
#define SEPROXYHAL_TAG_GENERAL_STATUS_LAST_COMMAND 0x0000
//#define SEPROXYHAL_TAG_GENERAL_STATUS_MORE_COMMAND 0x0001 // it's a status,
//but it shall be a command instead to avoid perturbating the simple seproxyhal
//bus logic
//#define SEPROXYHAL_TAG_GENERAL_STATUS_ERROR        0x0002 // shall be a
//command instead
#define SEPROXYHAL_TAG_PAIRING_STATUS 0x61
#define SEPROXYHAL_TAG_BLE_READ_RESPONSE_STATUS 0x62
#define SEPROXYHAL_TAG_NFC_READ_RESPONSE_STATUS 0x63
#define SEPROXYHAL_TAG_BLE_NOTIFY_INDICATE_STATUS 0x64
#define SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS 0x65
#define SEPROXYHAL_TAG_PRINTF_STATUS 0x66
#define SEPROXYHAL_TAG_SET_LINK_SPEED 0x67 // <mhz(1byte)> <etu(1byte)>
#define SEPROXYHAL_TAG_SCREEN_ANIMATION_STATUS                                 \
    0x68 // <kind(1byte)> <parameters(variable)> // replied with a display
         // processed event when done
#define SEPROXYHAL_TAG_SCREEN_ANIMATION_STATUS_VERTICAL_SPLIT_SLIDE            \
    0x00 // param[0:1](BE) = split Y coordinate, param[2:3](BE) = animation
         // duration in ms

#endif
