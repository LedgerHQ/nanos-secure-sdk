
/* @BANNER@ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef __U2F_IO_H__

#define __U2F_IO_H__

#include "u2f_service.h"
#include "os.h"

#define EXCEPTION_DISCONNECT 0x80

/**
 * Request an IO message to be dispatched in the required media.
 */
void u2f_io_send(uint8_t *buffer, uint16_t length, u2f_transport_media_t media);

#endif
