
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

#ifndef OS_IO_SEPROXYHAL_H
#define OS_IO_SEPROXYHAL_H

#include "os.h"

#ifdef OS_IO_SEPROXYHAL

#include "seproxyhal_protocol.h"
#include "ux.h"

// helper macro to swap values, without intermediate value
#define SWAP(a, b)                                                             \
  {                                                                            \
    a ^= b;                                                                    \
    b ^= a;                                                                    \
    a ^= b;                                                                    \
  }

extern unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

SYSCALL void io_seph_send(const unsigned char *buffer PLENGTH(length),
                          unsigned short length);

// return 1 if the previous seproxyhal exchange has been terminated with a
// status (packet which starts with 011x xxxx) else 0, which means the exchange
// needs to be closed.
SYSCALL unsigned int io_seph_is_status_sent(void);

// not to be called by application (application is triggered using io_event
// instead), resered for seproxyhal
#define IO_CACHE 1
SYSCALL unsigned short io_seph_recv(unsigned char *buffer PLENGTH(maxlength),
                                    unsigned short maxlength,
                                    unsigned int flags);

#define io_seproxyhal_spi_send io_seph_send
#define io_seproxyhal_spi_is_status_sent io_seph_is_status_sent
#define io_seproxyhal_spi_recv io_seph_recv

// HAL init, not meant to be called by applications, which shall call
// ::io_seproxyhal_init instead
void io_seph_init(void);

// init all (io/ux/etc)
void io_seproxyhal_init(void);

// only reinit ux related globals
void io_seproxyhal_init_ux(void);

// only init button handling related variables (not to be done when switching
// screen to avoid the release triggering unwanted behavior)
void io_seproxyhal_init_button(void);

// delegate function for generic io_exchange
unsigned short io_exchange_al(unsigned char channel_and_flags,
                              unsigned short tx_len);

// Allow application to overload how the io_exchange function automatically
// replies to get app name and version
unsigned int os_io_seproxyhal_get_app_name_and_version(void);

// for delegation of Native NFC / USB
unsigned char io_event(unsigned char channel);

#ifdef HAVE_BLE
void BLE_power(unsigned char powered, const char *discovered_name);

void io_hal_ble_wipe_pairing_db(void);
#endif // HAVE_BLE

void USB_power(unsigned char enabled);

void io_seproxyhal_handle_usb_event(void);
void io_seproxyhal_handle_usb_ep_xfer_event(void);
uint16_t io_seproxyhal_get_ep_rx_size(uint8_t epnum);

// process event for io protocols when waiting for a ux operation to end
// return 1 when event replied, 0 else
unsigned int io_seproxyhal_handle_event(void);

// reply a general status last command
void io_seproxyhal_general_status(void);

// reply a MORE COMMANDS status for the proxyhal to wait for more data later
void os_io_seproxyhal_general_status_processing(void);

// legacy function to send over EP 0x82
void io_usb_send_apdu_data(unsigned char *buffer, unsigned short length);
void io_usb_send_apdu_data_ep0x83(unsigned char *buffer, unsigned short length);

// trigger a transfer over an usb endpoint and waits for it to occur if timeout
// is != 0
void io_usb_send_ep(unsigned int ep, unsigned char *buffer,
                    unsigned short length, unsigned int timeout);

void io_usb_ccid_reply(unsigned char *buffer, unsigned short length);

#define NO_TIMEOUT (0UL)
// Function that allow applications to modulate the APDU handling timeout
// timeout is defaulty disabled.
// having an APDU handling timeout is useful to solve multiple media
// interactions.
void io_set_timeout(unsigned int timeout);

typedef enum {
  APDU_IDLE,
  APDU_BLE,
  APDU_BLE_WAIT_NOTIFY,
  APDU_NFC_M24SR,
  APDU_NFC_M24SR_SELECT,
  APDU_NFC_M24SR_FIRST,
  APDU_NFC_M24SR_RAPDU,
  APDU_USB_HID,
  APDU_USB_CCID,
  APDU_U2F,
  APDU_RAW,
  APDU_USB_WEBUSB,
} io_apdu_state_e;

#ifdef HAVE_BLE_APDU
void BLE_protocol_recv(unsigned char data_length, unsigned char *att_data);

#define BLE_protocol_send_RET_OK 0
#define BLE_protocol_send_RET_BUSY 1
#define BLE_protocol_send_RET_ABORTED 2
// TODO ensure not being stuck in case disconnect or timeout during reply.
// add a timeout for reply operation
char BLE_protocol_send(unsigned char *response_apdu,
                       unsigned short response_apdu_length);
#endif // HAVE_BLE_APDU

#ifdef HAVE_IO_U2F
#include "u2f_service.h"
extern u2f_service_t G_io_u2f;
#endif // HAVE_IO_U2F

/**
 *  Ledger Bluetooth Low Energy APDU Protocol
 *  Characteristic content:
 *  [______________________________]
 *   TT SSSS VVVV................VV
 *
 *  All fields are big endian encoded.
 *  TT: 1 byte content tag
 *  SSSS: 2 bytes sequence number, big endian encoded (start @ 0).
 *  VVVV..VV: variable length content. When SSSS is 0, the first two bytes
 * encodes in big endian the total length of the APDU to transport.
 *
 *  Command/Response APDU are split in chunks to fill up the bluetooth's
 * characteristic
 *
 *  APDU are using either standard or extended header. up to the application to
 * check the total received length and the lc field
 *
 *  Tags:
 *  Direction:*  T:0x05 S=<sequence-idx-U2BE>
 * V=<seq==0?totallength(U2BE):NONE><apducontent> APDU (command/response)
 * packet.
 *
 * Example:
 * --------
 *   Wrapping of Command APDU:
 *     E0 FF 12 13 14
 *     15 16 17 18 19 1A 1B 1C
 *     1D 1E 1F 20 21 22 23 24
 *     25 26 27 28 29 2A 2B 2C
 *     2D 2E 2F 30 31 32 33 34
 *     35
 *   Result in 3 chunks (20 bytes at most):
 *     0500000026E0FF12131415161718191A1B1C1D1E
 *     0500011F202122232425262728292A2B2C2D2E2F
 *     050002303132333435
 *
 *
 *   Wrapping of Response APDU:
 *     15 16 17 18 19 1a 1b 1c
 *     1d 1e 1f 20 21 22 23 24
 *     25 26 27 28 29 2a 2b 2c
 *     2d 2e 2f 30 31 32 33 34
 *     35 90 00
 *   Result in 3 chunks (20 bytes at most):
 *     050000002315161718191a1b1c1d1e1f20212223
 *     0500012425262728292a2b2c2d2e2f3031323334
 *     050002359000
 */

// avoid typing the size each time
#define SPRINTF(strbuf, ...) snprintf(strbuf, sizeof(strbuf), __VA_ARGS__)

#define ARRAYLEN(array) (sizeof(array) / sizeof(array[0]))
#define INARRAY(elementptr, array)                                             \
  ((unsigned int)elementptr >= (unsigned int)array &&                          \
   (unsigned int)elementptr < ((unsigned int)array) + sizeof(array))

/**
 * Wait until a UX call returns a definitve status. Handle all event packets in
 * between
 */
unsigned int os_ux_blocking(bolos_ux_params_t *params);

void mcu_usb_printc(unsigned char c);

/**
 * Global type that enables to map memory onto the application zone instead of
 * over the os for os side
 */
typedef struct io_seph_s {
  io_apdu_state_e apdu_state; // by default
  unsigned short apdu_length; // total length to be received
  unsigned short io_flags;    // flags to be set when calling io_exchange
  io_apdu_media_t apdu_media;

  unsigned int ms;

#ifdef HAVE_IO_USB
  unsigned char usb_ep_xfer_len[IO_USB_MAX_ENDPOINTS];
  struct {
    unsigned short timeout; // up to 64k milliseconds (64 sec)
  } usb_ep_timeouts[IO_USB_MAX_ENDPOINTS];
#endif // HAVE_IO_USB

#ifdef HAVE_BLE_APDU
  unsigned short ble_xfer_timeout;
#endif // HAVE_BLE_APDU

#ifdef TARGET_NANOX
  unsigned int display_status_sent;
  // cached here to avoid unavailable zone deref within IO task
  unsigned int plane_mode;
#endif // TARGET_NANOX

} io_seph_app_t;

extern io_seph_app_t G_io_app;

// deprecated
#define G_io_apdu_media G_io_app.apdu_media
// deprecated
#define G_io_apdu_state G_io_app.apdu_state

#ifdef HAVE_IO_TASK
/**
 * IO task entry point
 */
void io_task(void);
/**
 * IO task initializez
 */
void io_start(void);
#endif // HAVE_IO_TASK

void io_seproxyhal_setup_ticker(unsigned int interval_ms);
void io_seproxyhal_power_off(void);
void io_seproxyhal_se_reset(void);
void io_seproxyhal_disable_io(void);
void io_seproxyhal_disable_ble(void);

/**
 * Function to ensure a I/O channel is not timeouting waiting for operations
 * after a long time without SEPH packet exchanges
 */
void io_seproxyhal_io_heartbeat(void);

#endif // OS_IO_SEPROXYHAL

#endif // OS_IO_SEPROXYHAL_H
