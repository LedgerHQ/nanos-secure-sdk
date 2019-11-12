
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

/*********************************************************************************
 * 4 text lines
 */

const bagl_element_t ux_layout_nnbnn_elements[] = {
#ifdef TARGET_NANOX
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL},

  // up / down
  {{BAGL_ICON                           , 0x01,   0,  30,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_up},
  {{BAGL_ICON                           , 0x02, 120,  30,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_down},

  {{BAGL_LABELINE                       , 0x10,   0,   4, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x11,   0,  20, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x12,   0,  36, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},  
  {{BAGL_LABELINE                       , 0x13,   0,  52, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x14,   0,  68, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
#else // TARGET_NANOX
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL},

  // up / down
  {{BAGL_ICON                           , 0x01,   0,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_up},
  {{BAGL_ICON                           , 0x02, 120,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_down},

  {{BAGL_LABELINE                       , 0x11,   0,   3, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x12,   0,  19, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},  
  {{BAGL_LABELINE                       , 0x13,   0,  35, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
#endif // TARGET_NANOX
};

const bagl_element_t* ux_layout_nnbnn_prepro(const bagl_element_t* element) {
  // don't display if null
  const ux_layout_strings_params_t* params = (const ux_layout_strings_params_t*)ux_stack_get_current_step_params();

	// ocpy element before any mod
	os_memmove(&G_ux.tmp_element, element, sizeof(bagl_element_t));

  // for dashboard, setup the current application's name
  switch (element->component.userid) {

    case 0x01:
  		if (!params->lines[1]) {
  			return NULL;
  		}
  		break;

  	case 0x02:
  		if (!params->lines[3]) {
  			return NULL;
  		}
  		break;

    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
      G_ux.tmp_element.text = params->lines[G_ux.tmp_element.component.userid&0xF];
      break;
  }
  return &G_ux.tmp_element;
}

void ux_layout_nnbnn_init(unsigned int stack_slot) {
  ux_stack_init(stack_slot);
  G_ux.stack[stack_slot].element_arrays[0].element_array = ux_layout_nnbnn_elements;
  G_ux.stack[stack_slot].element_arrays[0].element_array_count = ARRAYLEN(ux_layout_nnbnn_elements);
  G_ux.stack[stack_slot].element_arrays_count = 1;
  G_ux.stack[stack_slot].screen_before_element_display_callback = ux_layout_nnbnn_prepro;
  G_ux.stack[stack_slot].button_push_callback = ux_flow_button_callback;
  ux_stack_display(stack_slot);
}

#endif // HAVE_UX_FLOW
