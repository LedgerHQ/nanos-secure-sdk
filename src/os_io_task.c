#include "decorators.h"
#include "exceptions.h"
#include "os_io_seproxyhal.h"
#include "os_task.h"
#include "os_types.h"

#include <string.h>

#ifdef HAVE_BLE
#include "ledger_ble.h"
bolos_ux_asynch_callback_t G_io_asynch_ux_callback;
#endif // HAVE_BLE

#ifdef HAVE_BOLOS
#ifdef HAVE_BLE
# if (IO_SEPROXYHAL_BUFFER_SIZE_B != 192) && (defined(BOLOS_RELEASE) || !defined(DEBUG_VARIABLE_SPI_SIZE))
#  error IO_SEPROXYHAL_BUFFER_SIZE_B size is invalid
# endif // (IO_SEPROXYHAL_BUFFER_SIZE_B != 192) && (defined(BOLOS_RELEASE) || !defined(DEBUG_VARIABLE_SPI_SIZE))
#else // !HAVE_BLE
# if (IO_SEPROXYHAL_BUFFER_SIZE_B != 128) && (defined(BOLOS_RELEASE) || !defined(DEBUG_VARIABLE_SPI_SIZE))
#  error IO_SEPROXYHAL_BUFFER_SIZE_B size is invalid
# endif // (IO_SEPROXYHAL_BUFFER_SIZE_B != 128) && (defined(BOLOS_RELEASE) || !defined(DEBUG_VARIABLE_SPI_SIZE))
#endif // !HAVE_BLE

// Buffer dedicated to the MCU <-> SE data transfer.
unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];
io_seph_app_t G_io_app;
#endif // HAVE_BOLOS

#ifdef HAVE_IO_TASK
void io_start(void) {
#else // HAVE_IO_TASK
void bolos_init_ios(void) {
#endif // HAVE_IO_TASK

  // os_allow_protected_ram();
  // unsigned int feat = G_os.seproxyhal_features;
  // os_deny_protected_ram();

  io_seproxyhal_init();

  USB_power(0);

#ifdef HAVE_BLE
  memset(&G_io_asynch_ux_callback, 0, sizeof(G_io_asynch_ux_callback));
  LEDGER_BLE_init();
#endif // HAVE_BLE

  io_seproxyhal_init();

  USB_power(1);
}


#ifdef HAVE_IO_TASK
void io_process(void) {
  for (;;) {
    if (!(G_io_app.io_flags & IO_FINISHED)) {
      G_io_app.apdu_length = io_exchange(CHANNEL_APDU | G_io_app.io_flags, G_io_app.apdu_length);
      // mark IO as ended
      G_io_app.io_flags |= IO_FINISHED;
    }
    else {
      // pump packets (process all through handle_events)
      os_io_seph_recv_and_process(0);
    }
    // we have finished our call, notify the other tasks
    os_sched_yield(BOLOS_TRUE);
  } 
}

void io_task(void) {
  for(;;) {
    BEGIN_TRY {
      TRY {
        io_start();
        io_process();
      }
      CATCH_ALL {
        // any error leading here is triggering an IO stack reset
        os_sched_yield(EXCEPTION_IO_RESET);
      }
      FINALLY {
      }
    }
    END_TRY;
  }
}
#endif // HAVE_IO_TASK
