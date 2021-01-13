#include "ux.h"

/*********************************************************************************
 * 4 text lines
 */

const bagl_element_t ux_layout_nnbnn_elements[] = {
#ifdef TARGET_NANOX
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL, 0, 0, 0, NULL, NULL, NULL},

  // up / down
  {{BAGL_ICON                           , 0x01,   0,  30,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_up, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_ICON                           , 0x02, 120,  30,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_down, 0, 0, 0, NULL, NULL, NULL },

  {{BAGL_LABELINE                       , 0x10,   0,   4, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x11,   0,  20, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x12,   0,  36, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },  
  {{BAGL_LABELINE                       , 0x13,   0,  52, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x14,   0,  68, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
#else // TARGET_NANOX
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL, 0, 0, 0, NULL, NULL, NULL},

  // up / down
  {{BAGL_ICON                           , 0x01,   0,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_up, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_ICON                           , 0x02, 120,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_down, 0, 0, 0, NULL, NULL, NULL },

  {{BAGL_LABELINE                       , 0x11,   0,   3, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x12,   0,  19, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },  
  {{BAGL_LABELINE                       , 0x13,   0,  35, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
#endif // TARGET_NANOX
};

const bagl_element_t* ux_layout_nnbnn_prepro(const bagl_element_t* element) {
  // don't display if null
  const ux_layout_nnbnn_params_t* params = (const ux_layout_nnbnn_params_t*)ux_stack_get_current_step_params();

	// ocpy element before any mod
	os_memmove(&G_ux.tmp_element, element, sizeof(bagl_element_t));

  // for dashboard, setup the current application's name
  switch (element->component.userid) {

    case 0x01:
  		if (!params->line2) {
  			return NULL;
  		}
  		break;

  	case 0x02:
  		if (!params->line4) {
  			return NULL;
  		}
  		break;

#ifdef TARGET_NANOX
    case 0x10:
    	if (params->line1) {
    		G_ux.tmp_element.text = params->line1;
    	}
      break;
#endif // TARGET_NANOX

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

#ifdef TARGET_NANOX
    case 0x14:
      if (params->line4) {
        G_ux.tmp_element.text = params->line5;
      }
      break;
#endif // TARGET_NANOX
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

