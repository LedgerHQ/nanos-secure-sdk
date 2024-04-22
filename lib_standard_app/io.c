/*****************************************************************************
 *   (c) 2021 Ledger SAS.
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
 *****************************************************************************/

#include <stdint.h>
#include <string.h>

#include "os.h"
#include "io.h"
#include "write.h"

#ifdef HAVE_SWAP
#include "swap.h"
#endif

// TODO: Temporary workaround, at some point all status words should be defined by the SDK and
// removed from the application
#define SW_OK                    0x9000
#define SW_WRONG_RESPONSE_LENGTH 0xB000

uint8_t G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

/**
 * Variable containing the length of the APDU response to send back.
 */
static uint32_t G_output_len = 0;

/**
 * IO state (READY, RECEIVING, WAITING).
 */
static io_state_e G_io_state = READY;

#ifdef HAVE_BAGL
WEAK void io_seproxyhal_display(const bagl_element_t *element)
{
    io_seproxyhal_display_default(element);
}
#endif  // HAVE_BAGL

// This function can be used to declare a callback to SEPROXYHAL_TAG_TICKER_EVENT in the application
WEAK void app_ticker_event_callback(void) {}

WEAK uint8_t io_event(uint8_t channel)
{
    (void) channel;

    switch (G_io_seproxyhal_spi_buffer[0]) {
        case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
            UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer);
            break;
        case SEPROXYHAL_TAG_STATUS_EVENT:
            if (G_io_apdu_media == IO_APDU_MEDIA_USB_HID &&  //
                !(U4BE(G_io_seproxyhal_spi_buffer, 3) &      //
                  SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED)) {
                THROW(EXCEPTION_IO_RESET);
            }
            __attribute__((fallthrough));
        case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
            UX_DISPLAYED_EVENT({});
            break;
        case SEPROXYHAL_TAG_TICKER_EVENT:
            app_ticker_event_callback();
            UX_TICKER_EVENT(G_io_seproxyhal_spi_buffer, {});
            break;
        default:
            UX_DEFAULT_EVENT();
            break;
    }

    if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
    }

    return 1;
}

WEAK uint16_t io_exchange_al(uint8_t channel, uint16_t tx_len)
{
    switch (channel & ~(IO_FLAGS)) {
        case CHANNEL_KEYBOARD:
            break;
        case CHANNEL_SPI:
            if (tx_len) {
                io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

                if (channel & IO_RESET_AFTER_REPLIED) {
                    halt();
                }

                return 0;
            }
            else {
                return io_seproxyhal_spi_recv(G_io_apdu_buffer, sizeof(G_io_apdu_buffer), 0);
            }
        default:
            THROW(INVALID_PARAMETER);
    }

    return 0;
}

WEAK void io_init()
{
    // Reset length of APDU response
    G_output_len = 0;
    G_io_state   = READY;
}

WEAK int io_recv_command()
{
    int ret = -1;

    switch (G_io_state) {
        case READY:
            ret        = io_exchange(CHANNEL_APDU | IO_CONTINUE_RX, G_output_len);
            G_io_state = RECEIVED;
            break;
        case RECEIVED:
            G_io_state = WAITING;
            ret        = io_exchange(CHANNEL_APDU | IO_ASYNCH_REPLY, G_output_len);
            G_io_state = RECEIVED;
            break;
        case WAITING:
            G_io_state = READY;
            ret        = -1;
            break;
    }

    return ret;
}

WEAK int io_send_response_buffers(const buffer_t *rdatalist, size_t count, uint16_t sw)
{
    int ret = -1;

    G_output_len = 0;
    if (rdatalist && count > 0) {
        for (size_t i = 0; i < count; i++) {
            const buffer_t *rdata = &rdatalist[i];

            if (!buffer_copy(rdata,
                             G_io_apdu_buffer + G_output_len,
                             sizeof(G_io_apdu_buffer) - G_output_len - 2)) {
                return io_send_sw(SW_WRONG_RESPONSE_LENGTH);
            }
            G_output_len += rdata->size - rdata->offset;
            if (count > 1) {
                PRINTF("<= FRAG (%u/%u) RData=%.*H\n", i + 1, count, rdata->size, rdata->ptr);
            }
        }
        PRINTF("<= SW=%04X | RData=%.*H\n", sw, G_output_len, G_io_apdu_buffer);
    }
    else {
        PRINTF("<= SW=%04X | RData=\n", sw);
    }

    write_u16_be(G_io_apdu_buffer, G_output_len, sw);
    G_output_len += 2;

#ifdef HAVE_SWAP
    // If we are in swap mode and have validated a TX, we send it and immediately quit
    if (G_called_from_swap && G_swap_response_ready) {
        PRINTF("Swap answer is processed. Send it\n");
        if (io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, G_output_len) == 0) {
            swap_finalize_exchange_sign_transaction(sw == SW_OK);
        }
        else {
            PRINTF("Unrecoverable\n");
            os_sched_exit(-1);
        }
    }
#endif  // HAVE_SWAP

    switch (G_io_state) {
        case READY:
            ret = -1;
            break;
        case RECEIVED:
#ifdef STANDARD_APP_SYNC_RAPDU
            // Send synchronously the APDU response.
            // This is needed to send the response before displaying synchronous
            // status message on the screen.
            // This is not always done to spare the RAM (stack) on LNS.
            __attribute__((fallthrough));
#else
            G_io_state = READY;
            ret        = 0;
            break;
#endif
        case WAITING:
            ret          = io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, G_output_len);
            G_output_len = 0;
            G_io_state   = READY;
            break;
    }

    return ret;
}

#ifdef STANDARD_APP_SYNC_RAPDU
WEAK bool io_recv_and_process_event(void)
{
    int apdu_state = G_io_app.apdu_state;

    os_io_seph_recv_and_process(0);

    // If an APDU was received in previous os_io_seph_recv_and_process call and
    // is waiting to be processed, return true
    if (apdu_state == APDU_IDLE && G_io_app.apdu_state != APDU_IDLE) {
        return true;
    }

    return false;
}
#endif
