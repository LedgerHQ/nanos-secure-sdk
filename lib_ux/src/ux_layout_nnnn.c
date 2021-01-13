#include "ux.h"

#ifdef TARGET_NANOX
/*********************************************************************************
 * 4 text lines
 */

const bagl_element_t ux_layout_nnnn_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL, 0, 0, 0, NULL, NULL, NULL},

  {{BAGL_ICON                           , 0x01,   2,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_left, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_ICON                           , 0x02, 122,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_right, 0, 0, 0, NULL, NULL, NULL },

  {{BAGL_LABELINE                       , 0x10,   0,  15, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x11,   0,  29, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x12,   0,  43, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x13,   0,  57, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
};

const bagl_element_t* ux_layout_nnnn_prepro(const bagl_element_t* element) {
  // don't display if null
  const ux_layout_nnnn_params_t* params = (const ux_layout_nnnn_params_t*)ux_stack_get_current_step_params();

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
    	if (params->line1) {
    		G_ux.tmp_element.text = params->line1;
    	}
      break;

    case 0x11:
    	if (params->line2) {
    		G_ux.tmp_element.text = params->line2;
    	}
      break;

    case 0x12:
    	if (params->line3) {
    		G_ux.tmp_element.text = params->line3;
    	}
      break;

    case 0x13:
    	if (params->line4) {
    		G_ux.tmp_element.text = params->line4;
    	}
      break;
  }
  return &G_ux.tmp_element;
}

void ux_layout_nnnn_init(unsigned int stack_slot) {
  ux_stack_init(stack_slot);
  G_ux.stack[stack_slot].element_arrays[0].element_array = ux_layout_nnnn_elements;
  G_ux.stack[stack_slot].element_arrays[0].element_array_count = ARRAYLEN(ux_layout_nnnn_elements);
  G_ux.stack[stack_slot].element_arrays_count = 1;
  G_ux.stack[stack_slot].screen_before_element_display_callback = ux_layout_nnnn_prepro;
  G_ux.stack[stack_slot].button_push_callback = ux_flow_button_callback;
  ux_stack_display(stack_slot);
}

#endif // TARGET_NANOX
