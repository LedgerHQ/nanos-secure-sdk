
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

#ifndef SEPROXYHAL_PROTOCOL_H
#define SEPROXYHAL_PROTOCOL_H

#define BLE_CMD_APDU 0x05
#define BLE_CHUNK_LENGTH 20
#define M24SR_CHUNK_LENGTH 0xF6

// EVENTS
#define SEPROXYHAL_TAG_SESSION_START_EVENT                                     \
  0x01 // <kind(1byte)=reqble|recovery|flashback>
       // <featurebitmap(4bytesBE)>
       // <seproxyhalversion(L(1)V(L)) (blank when sent by the bootloader>
       // <mcu_bl_loadkeyid(l1v)>
       // <mcu_bl_version(l1v) (not included by BL)>
       // <mcu_seph_signkeyid(l1v)(not included by BL)>
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
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_BATTERY 0x00000008UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_ISET_MASK 0xF0000000UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_ISET_BASIC 0x00000000UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_ISET_MCUSEC 0x10000000UL
#define SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_ISET_MCUBL 0x20000000UL

#define SEPROXYHAL_TAG_BLE_PAIRING_ATTEMPT_EVENT 0x02
#define SEPROXYHAL_TAG_BLE_WRITE_REQUEST_EVENT 0x03
#define SEPROXYHAL_TAG_BLE_READ_REQUEST_EVENT 0x04
#define SEPROXYHAL_TAG_BUTTON_PUSH_EVENT 0x05
#define SEPROXYHAL_TAG_BUTTON_PUSH_ID_MASK                                     \
  0xFE // up to 7 physical buttons (bit is 1 when pressed, and 0 when released)
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
#define SEPROXYHAL_TAG_TICKER_EVENT 0x0E // <ms_since_power_on (4BE)>
#define SEPROXYHAL_TAG_USB_EVENT 0x0F    // <connect/disconnect/suspend/resume>
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
#define SEPROXYHAL_TAG_BLUENRG_RECV_EVENT 0x14 // <raw BLE transport packet>

#define SEPROXYHAL_TAG_STATUS_EVENT                                            \
  0x15 // <flags(4BE)> <screen_backlight_percentage(1B)> <ledcolorARGB(4B)>
       // <battery_voltage_mv(4BE)> <battery % (1B)>
#define SEPROXYHAL_TAG_STATUS_EVENT_FLAG_CHARGING 0x00000001
#define SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_ON 0x00000002
#define SEPROXYHAL_TAG_STATUS_EVENT_FLAG_BLE_ON 0x00000004
#define SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED 0x00000008

#define SEPROXYHAL_TAG_CAPDU_EVENT 0x16 // raw command apdu transport

#define SEPROXYHAL_TAG_I2C_EVENT                                               \
  0x17 // <kind(1):READ(0),WRITE(1),STOP(2),TIMEOUT(3)> <rawdata>
#define SEPROXYHAL_TAG_I2C_EVENT_KIND_READ 0x01
#define SEPROXYHAL_TAG_I2C_EVENT_KIND_WRITE 0x02
#define SEPROXYHAL_TAG_BLE_RECV_EVENT 0x18 // <hcipacket raw>
#define SEPROXYHAL_TAG_BOOTLOADER_RAPDU_EVENT                                  \
  0x19 // <RAPDU from the bootloader>

// COMMANDS
#ifdef HAVE_SEPROXYHAL_MCU
#define SEPROXYHAL_TAG_MCU 0x31 // <type>
#define SEPROXYHAL_TAG_MCU_TYPE_BOOTLOADER 0x00
#define SEPROXYHAL_TAG_MCU_TYPE_LOCK 0x01
#ifdef HAVE_MCU_PROTECT
#define SEPROXYHAL_TAG_MCU_TYPE_PROTECT                                        \
  0x02 // for instance ask RDP2 to be engaged
#endif // HAVE_MCU_PROTECT
#define SEPROXYHAL_TAG_MCU_BOOTLOADER SEPROXYHAL_TAG_MCU
#endif // HAVE_SEPROXYHAL_MCU
#define SEPROXYHAL_TAG_UNSEC_CHUNK_READ                                        \
  0x32 // <length:U2BE> <restart(bit1)|continue=0>
// available if SEPROXYHAL_TAG_SESSION_START_EVENT_FEATURE_ISET_MCUSEC
#define SEPROXYHAL_TAG_UNSEC_CHUNK_READ_EXT                                    \
  0x33 // <length:U2BE> <continue=0|restart(bit1)|specifyoffset(bit2)>
       // <offset:U4BE>
#define SEPROXYHAL_TAG_BLE_SEND 0x38 // <opcode(U2BE)> <hci packet payload>
#define SEPROXYHAL_TAG_SET_SCREEN_CONFIG                                       \
  0x3E // <flags(1byte):pwron(128)|rotation(0:0,90:2,180:4,270:6)|invert(1)>
       // <brightness percentage(1byte)>
#define SEPROXYHAL_TAG_SET_LINK_PROP 0x3F // <mode(1b):debug(1)>
#define SEPROXYHAL_TAG_BLUENRG_SEND                                            \
  0x40 // <CS mgmt: start (bit 1) | stop (bit 2)> <packet>
#define SEPROXYHAL_TAG_BLE_DEFINE_GENERIC_SETTING 0x41
#define SEPROXYHAL_TAG_BLE_DEFINE_SERVICE_SETTING 0x42
#define SEPROXYHAL_TAG_NFC_DEFINE_SERVICE_SETTING 0x43
#define SEPROXYHAL_TAG_BLE_RADIO_POWER 0x44 // <action(1byte)>
#define SEPROXYHAL_TAG_BLE_RADIO_POWER_ACTION_ON 0x02
#define SEPROXYHAL_TAG_BLE_RADIO_POWER_ACTION_DBWIPE 0x04
#define SEPROXYHAL_TAG_BLE_RADIO_POWER_ACTION_DBSAVE 0x08
#define SEPROXYHAL_TAG_BLE_RADIO_POWER_ACTION_DBLOAD 0x10
#define SEPROXYHAL_TAG_BLE_RADIO_POWER_ACTION_DBFREE 0x20
#define SEPROXYHAL_TAG_NFC_RADIO_POWER 0x45
#define SEPROXYHAL_TAG_SE_POWER_OFF 0x46
//#define SEPROXYHAL_TAG_SCREEN_POWER                0x47
#define SEPROXYHAL_TAG_BLE_NOTIFY_INDICATE 0x48
#define SEPROXYHAL_TAG_BATTERY_CHARGE 0x49 // <>
//#define SEPROXYHAL_TAG_SCREEN_DISPLAY              0x4A // wait for
//display_event after sent
#define SEPROXYHAL_TAG_DEVICE_OFF 0x4B
#define SEPROXYHAL_TAG_MORE_TIME 0x4C
#define SEPROXYHAL_TAG_M24SR_C_APDU 0x4D
#define SEPROXYHAL_TAG_SET_TICKER_INTERVAL 0x4E
#define SEPROXYHAL_TAG_USB_CONFIG 0x4F // <connect/disconnect/addr/endpoints(1)>
#define SEPROXYHAL_TAG_USB_CONFIG_CONNECT 0x01    // <>
#define SEPROXYHAL_TAG_USB_CONFIG_DISCONNECT 0x02 // <>
#define SEPROXYHAL_TAG_USB_CONFIG_ADDR 0x03       // <usb device address(1)>
#define SEPROXYHAL_TAG_USB_CONFIG_ENDPOINTS                                    \
  0x04 // <nbendpoints to configure(1)> [<epaddr>
       // <eptype:control/interrupt/bulk/isochrone/disabled> <epmps>]
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
#define SEPROXYHAL_TAG_REQUEST_STATUS                                          \
  0x52 // no args, request power levels of all peripherals and current charging
       // state or not if a battery is present.
#define SEPROXYHAL_TAG_RAPDU 0x53 // raw response apdu transport
#define SEPROXYHAL_TAG_I2C_XFER                                                \
  0x54 // <flags:b0=Start,b1=Stop,b2=Moreexpected> <address+R/w>
       // <write:rawdata,read=length(1B)>
#define SEPROXYHAL_TAG_PRINTF 0x5F // <bytes to push to the printf buffer>

// STATUS
#define SEPROXYHAL_TAG_STATUS_MASK 0x60
#define SEPROXYHAL_TAG_GENERAL_STATUS 0x60
#define SEPROXYHAL_TAG_GENERAL_STATUS_LAST_COMMAND 0x0000
//#define SEPROXYHAL_TAG_GENERAL_STATUS_MORE_COMMAND 0x0001 // it's a status,
//but it shall be a command instead to avoid perturbating the simple seproxyhal
//bus logic #define SEPROXYHAL_TAG_GENERAL_STATUS_ERROR        0x0002 // shall
//be a command instead
#define SEPROXYHAL_TAG_PAIRING_STATUS 0x61
#define SEPROXYHAL_TAG_BLE_READ_RESPONSE_STATUS 0x62
#define SEPROXYHAL_TAG_NFC_READ_RESPONSE_STATUS 0x63
#define SEPROXYHAL_TAG_BLE_NOTIFY_INDICATE_STATUS 0x64
#define SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS                                   \
  0x65 // <bagl_component_t little endian>
#define SEPROXYHAL_TAG_PRINTF_STATUS 0x66
#define SEPROXYHAL_TAG_SET_LINK_SPEED 0x67 // <mhz(1byte)> <etu(1byte)>
#define SEPROXYHAL_TAG_SCREEN_ANIMATION_STATUS                                 \
  0x68 // <kind(1byte)> <parameters(variable)> // replied with a display
       // processed event when done
#define SEPROXYHAL_TAG_SCREEN_ANIMATION_STATUS_VERTICAL_SPLIT_SLIDE            \
  0x00 // param[0:1](BE) = split Y coordinate, param[2:3](BE) = animation
       // duration in ms

#ifdef TARGET_BLUE
#define SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS                               \
  0x69 // <start:0|next:1> [start? <x> <y> <w> <h> <bitperpixel> <color_count*4
       // bytes (LE encoding)>] <icon bitmap (row scan, packed, LE)>
#define SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS_START 0x00
#define SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS_CONT 0x01
#endif

#define SEPROXYHAL_TAG_BOOTLOADER_CAPDU_STATUS 0x6A // <CAPDU to the bootloader>

#endif
