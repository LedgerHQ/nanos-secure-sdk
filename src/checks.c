
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2020 Ledger
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

#if !defined(HAVE_BOLOS)

#include "os.h"
#include "ux.h"
#include "ux_layouts.h"
#include "checks.h"

// This label ultimately comes from the application link.
extern unsigned int const _install_parameters;

// This function is the button callback associated with the 'ui_audited_elements' array below.
static unsigned int ui_audited_elements_button(unsigned int button_mask, unsigned int button_mask_counter) {

  // As soon as the user presses both buttons, we reinitialize the UX and buttons,
  // and the hand is given back to the normal process.
  if ((button_mask & (BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT)) == (BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT)) {
    G_ux.stack[0].button_push_callback = NULL;
  }
  return 0;
}

// This array is to be displayed under specific circumstances, right at the launch of an application.
const bagl_element_t ui_audited_elements[] = {
  // Erasure of the whole screen,
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL},
  // First line of text,
  {
    {BAGL_LABELINE, 0x01, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
    "Pending",
  },
  // Last line of text.
  {
    {BAGL_LABELINE, 0x01, 0, 24, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
    "Ledger review",
  },
};

// This function is called at the end of the seph initialization.
// It checks the install parameters of the application to be run, and if this area contains the
// CHECK_NOT_AUDITED_TLV_TAG tag with the CHECK_NOT_AUDITED_TLV_VAL value, a specific display
// is triggered before the actual application's splash screen.
void check_audited_app(void) {
  unsigned char     data = BOLOS_FALSE;
  unsigned char*    buffer = &data;
  unsigned int      length = os_parse_bertlv((unsigned char*)(&_install_parameters),
                                             CHECK_NOT_AUDITED_MAX_LEN,
                                             NULL,
                                             CHECK_NOT_AUDITED_TLV_TAG,
                                             0x00,
                                             (void**)&buffer,
                                             sizeof(data));

  // We trigger the associated behaviour only when the tag was present and the value corresponds to
  // the expected one.
  if (   (length)
      && (CHECK_NOT_AUDITED_TLV_VAL == data))
  {
    // We reserve the first slot for this display.
    ux_stack_push();
    ux_stack_init(0);

    // We trigger the additional display and wait for it to be completed.
    UX_DISPLAY(ui_audited_elements, NULL);
    UX_WAIT_DISPLAYED();

    io_seproxyhal_general_status();

    // We wait for the button callback pointer to wiped, and we process the incoming MCU events in the
    // meantime. This callback will be wiped within the actual 'ui_audited_elements_button' function,
    // as soon as the user presses both buttons.
    do {
      io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
      io_seproxyhal_handle_event();
    } while (io_seproxyhal_spi_is_status_sent() && G_ux.stack[0].button_push_callback);

    // We pop the reserved slot but we do not care about the returned value (since we do not need it for
    // further displays at the moment) and reinitialize the UX and buttons.
    ux_stack_pop();
    io_seproxyhal_init_ux();
    io_seproxyhal_init_button();

    // Now we can wait for the next MCU status and exit.
    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
  }
}

#endif // !defined(HAVE_BOLOS)
