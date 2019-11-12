
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

#include "ux.h"

#ifdef HAVE_UX_FLOW

#ifdef TARGET_NANOX
/*********************************************************************************
 * 1 bold text line
 * 3 text lines
 */

const bagl_element_t ux_layout_bnnn_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL},

  {{BAGL_ICON                           , 0x01,   2,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_left},
  {{BAGL_ICON                           , 0x02, 122,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_right},

  {{BAGL_LABELINE                       , 0x10,   0,  15, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x11,   0,  29, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x12,   0,  43, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x13,   0,  57, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
};

/*
const bagl_element_t* ux_layout_bnnn_prepro(const bagl_element_t* element) {
	// don't display if null
  const ux_layout_bnnn_params_t* params = (const ux_layout_bnnn_params_t*)ux_stack_get_current_step_params();
	// ocpy element before any mod
	os_memmove(&G_ux.tmp_element, element, sizeof(bagl_element_t));

  // for dashboard, setup the current application's name
  switch (element->component.userid) {
  	case 0x01:
  		if (ux_flow_is_first()) {
  			return NULL;
  		}
  		break;

  	case 0x02:
  		if (ux_flow_is_last()) {
  			return NULL;
  		}
  		break;

    case 0x10:
      G_ux.tmp_element.text = params->line1;
      break;
    case 0x11:
      G_ux.tmp_element.text = params->line2;
      break;
    case 0x12:
      G_ux.tmp_element.text = params->line3;
      break;
    case 0x13:
      G_ux.tmp_element.text = params->line3;
      break;
  }
  return &G_ux.tmp_element;
}
*/

void ux_layout_bnnn_init(unsigned int stack_slot) {
  ux_stack_init(stack_slot);
  G_ux.stack[stack_slot].element_arrays[0].element_array = ux_layout_bnnn_elements;
  G_ux.stack[stack_slot].element_arrays[0].element_array_count = ARRAYLEN(ux_layout_bnnn_elements);
  G_ux.stack[stack_slot].element_arrays_count = 1;
  G_ux.stack[stack_slot].screen_before_element_display_callback = ux_layout_strings_prepro;
  G_ux.stack[stack_slot].button_push_callback = ux_flow_button_callback;
  ux_stack_display(stack_slot);
}
#endif // TARGET_NANOX

#endif // HAVE_UX_FLOW
