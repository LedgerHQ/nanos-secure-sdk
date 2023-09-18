
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2022 Ledger
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

#include "ux.h"
#include "os_utils.h"
#include <string.h>

#ifdef HAVE_UX_FLOW

// clang-format off
const bagl_element_t ux_layout_pn_elements[] = {
#if (BAGL_WIDTH==128 && BAGL_HEIGHT==64)
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, .text=NULL},

  {{BAGL_ICON                           , 0x01,   2,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_left },
  {{BAGL_ICON                           , 0x02, 122,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_right },

  {{BAGL_ICON                           , 0x10,  57,  17,  14,  14, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL },
  {{BAGL_LABELINE                       , 0x11,   0,  44, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL },
#elif (BAGL_WIDTH==128 && BAGL_HEIGHT==32)
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, .text=NULL},

  // left/right icons
  {{BAGL_ICON                           , 0x01,   2,  12,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_left },
  {{BAGL_ICON                           , 0x02, 122,  12,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_right },

  {{BAGL_ICON                           , 0x10,  56,  2,  16,  16, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL },
  {{BAGL_LABELINE                       , 0x11,   0, 28, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL },
#else
  #error "BAGL_WIDTH/BAGL_HEIGHT not defined"
#endif
};
// clang-format on

const bagl_element_t *ux_layout_pb_prepro(const bagl_element_t *element);

void ux_layout_pn_init(unsigned int stack_slot)
{
    ux_stack_init(stack_slot);
    G_ux.stack[stack_slot].element_arrays[0].element_array        = ux_layout_pn_elements;
    G_ux.stack[stack_slot].element_arrays[0].element_array_count  = ARRAYLEN(ux_layout_pn_elements);
    G_ux.stack[stack_slot].element_arrays_count                   = 1;
    G_ux.stack[stack_slot].screen_before_element_display_callback = ux_layout_pb_prepro;
    G_ux.stack[stack_slot].button_push_callback                   = ux_flow_button_callback;
    ux_stack_display(stack_slot);
}

#endif  // HAVE_UX_FLOW
