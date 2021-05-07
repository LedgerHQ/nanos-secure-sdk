#pragma once

#include "bolos_target.h"
#include "os_math.h"

/* ----------------------------------------------------------------------- */
/* -                            GLOBALS                                  - */
/* ----------------------------------------------------------------------- */

// the global apdu buffer
#ifdef HAVE_IO_U2F
#define IMPL_IO_APDU_BUFFER_SIZE (3 + 32 + 32 + 15 + 255)
#else
#define IMPL_IO_APDU_BUFFER_SIZE (5 + 255)
#endif

#ifdef CUSTOM_IO_APDU_BUFFER_SIZE
#define IO_APDU_BUFFER_SIZE                                                    \
  MAX(IMPL_IO_APDU_BUFFER_SIZE, CUSTOM_IO_APDU_BUFFER_SIZE)
#else
#define IO_APDU_BUFFER_SIZE IMPL_IO_APDU_BUFFER_SIZE
#endif
extern unsigned char G_io_apdu_buffer[IO_APDU_BUFFER_SIZE];

// send tx_len bytes (atr or rapdu) and retrieve the length of the next command
// apdu (over the requested channel)
#define CHANNEL_APDU 0
#define CHANNEL_KEYBOARD 1
#define CHANNEL_SPI 2
#define IO_RESET_AFTER_REPLIED 0x80
#define IO_RECEIVE_DATA 0x40
#define IO_RETURN_AFTER_TX 0x20
#define IO_ASYNCH_REPLY                                                        \
  0x10 // avoid apdu state reset if tx_len == 0 when we're expected to reply
#define IO_FINISHED 0x08 // inter task communication value
#define IO_FLAGS 0xF8
unsigned short io_exchange(unsigned char channel_and_flags,
                           unsigned short tx_len);

typedef enum {
  IO_APDU_MEDIA_NONE = 0, // not correctly in an apdu exchange
  IO_APDU_MEDIA_USB_HID = 1,
  IO_APDU_MEDIA_BLE,
  IO_APDU_MEDIA_NFC,
  IO_APDU_MEDIA_USB_CCID,
  IO_APDU_MEDIA_USB_WEBUSB,
  IO_APDU_MEDIA_RAW,
  IO_APDU_MEDIA_U2F,
} io_apdu_media_t;

#ifndef USB_SEGMENT_SIZE
#ifdef IO_HID_EP_LENGTH
#define USB_SEGMENT_SIZE IO_HID_EP_LENGTH
#else
#error IO_HID_EP_LENGTH and USB_SEGMENT_SIZE not defined
#endif
#endif
#ifndef BLE_SEGMENT_SIZE
#define BLE_SEGMENT_SIZE USB_SEGMENT_SIZE
#endif

// common usb endpoint buffer
extern unsigned char
    G_io_usb_ep_buffer[MAX(USB_SEGMENT_SIZE, BLE_SEGMENT_SIZE)];

/**
 * Return 1 when the event has been processed, 0 else
 */
// io callback in the application called when an interrupt based channel has
// received data to be processed
unsigned char io_event(unsigned char channel);
