#pragma once

#include <stdint.h>

#include "ux.h"
#include "os_io_seproxyhal.h"

#include "types.h"
#include "common/buffer.h"

#ifdef HAVE_BAGL
void io_seproxyhal_display(const bagl_element_t *element);
#endif  // HAVE_BAGL

/**
 * IO callback called when an interrupt based channel has received
 * data to be processed.
 *
 * @return 1 if success, 0 otherwise.
 *
 */
uint8_t io_event(uint8_t channel);

uint16_t io_exchange_al(uint8_t channel, uint16_t tx_len);

/**
 * Initialize the APDU I/O state.
 *
 * This function must be called before calling any other I/O function.
 */
void io_init(void);

/**
 * Receive APDU command in G_io_apdu_buffer.
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int io_recv_command(void);

/**
 * Send APDU response (response data + status word) by filling
 * G_io_apdu_buffer.
 *
 * @param[in] rdata
 *   Buffer with APDU response data.
 * @param[in] sw
 *   Status word of APDU response.
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int io_send_response(const buffer_t *rdata, uint16_t sw);

/**
 * Send APDU response (only status word) by filling
 * G_io_apdu_buffer.
 *
 * @param[in] sw
 *   Status word of APDU response.
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int io_send_sw(uint16_t sw);
