
/* @BANNER@ */

#ifdef HAVE_IO_U2F

#include <stdint.h>
#include <string.h>
#include "os.h"
#include "cx.h"
#include "u2f_service.h"
#include "u2f_transport.h"
#include "u2f_processing.h"
#include "u2f_impl.h"

#include "os_io_seproxyhal.h"

void u2f_io_send(uint8_t *buffer, uint16_t length, u2f_transport_media_t media) {
    if (media == U2F_MEDIA_USB) {
        os_memmove(G_io_usb_ep_buffer, buffer, length);
        // wipe the remaining to avoid :
        // 1/ data leaks
        // 2/ invalid junk
        os_memset(G_io_usb_ep_buffer+length, 0, sizeof(G_io_usb_ep_buffer)-length);
    }
    switch (media) {
    case U2F_MEDIA_USB:
        io_usb_send_ep(U2F_EPIN_ADDR, G_io_usb_ep_buffer, USB_SEGMENT_SIZE, 0);
        break;
#ifdef HAVE_BLE
    case U2F_MEDIA_BLE:
        BLE_protocol_send(buffer, length);
        break;
#endif
    default:
        PRINTF("Request to send on unsupported media %d\n", media);
        break;
    }
}

#endif // HAVE_IO_U2F
