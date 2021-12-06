
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2021 Ledger
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

#include "os_helpers.h"
#include "os_math.h"
#include "os_pic.h"
#include "os_print.h"
#include "os_utils.h"
#include "ux.h"
#include <string.h>

#ifdef HAVE_UX_FLOW

#include "ux_layout_common.h"

#ifdef HAVE_BAGL

/*********************************************************************************
 * 4 text lines
 */
#define LINE_FONT BAGL_FONT_OPEN_SANS_REGULAR_11px

typedef void (*ux_layout_paging_redisplay_t)(unsigned int stack_slot);

#if (BAGL_WIDTH==128 && BAGL_HEIGHT==64)
static const bagl_element_t ux_layout_paging_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL},

  {{BAGL_ICON                           , 0x01,   2,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_left},
  {{BAGL_ICON                           , 0x02, 122,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_right},

  {{BAGL_LABELINE                       , 0x10,   0,  15, 128,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x11,   (128-PIXEL_PER_LINE)/2,  29, PIXEL_PER_LINE,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, LINE_FONT|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x12,   (128-PIXEL_PER_LINE)/2,  43, PIXEL_PER_LINE,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, LINE_FONT|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x13,   (128-PIXEL_PER_LINE)/2,  57, PIXEL_PER_LINE,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, LINE_FONT|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
};
#endif // (BAGL_WIDTH==128 && BAGL_HEIGHT==64)

static const bagl_element_t* ux_layout_paging_prepro_common(const bagl_element_t* element, 
                                                            const char* title, 
                                                            const char* text) {
  
  // copy element before any mod
  memmove(&G_ux.tmp_element, element, sizeof(bagl_element_t));

  switch (element->component.userid) {
    case 0x01:
      // no step before AND no pages before
      if (ux_flow_is_first() && G_ux.layout_paging.current == 0) {
        return NULL;
      }
      break;

    case 0x02:
      if (ux_flow_is_last() && G_ux.layout_paging.current == G_ux.layout_paging.count -1 ) {
        return NULL;
      }
      break;

    case 0x10:
      // We set the boldness of the text.
      // display 
      if (title) {
        SPRINTF(G_ux.string_buffer, (G_ux.layout_paging.count>1)?"%s (%d/%d)":"%s", STRPIC(title), G_ux.layout_paging.current+1, G_ux.layout_paging.count);
      }
      else {
        SPRINTF(G_ux.string_buffer, "%d/%d", G_ux.layout_paging.current+1, G_ux.layout_paging.count);
      }

      G_ux.tmp_element.component.font_id = ((G_ux.layout_paging.format & PAGING_FORMAT_BN) == PAGING_FORMAT_BN) ? 
                                            (BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER)
                                          : (BAGL_FONT_OPEN_SANS_REGULAR_11px   | BAGL_FONT_ALIGNMENT_CENTER);
      G_ux.tmp_element.text = G_ux.string_buffer;
      break;

    case 0x11:
    case 0x12:
    case 0x13: {
      unsigned int lineidx = (element->component.userid&0xF)-1;
      if (
        lineidx < UX_LAYOUT_PAGING_LINE_COUNT && 
        G_ux.layout_paging.lengths[lineidx]) {
        SPRINTF(G_ux.string_buffer, 
                "%.*s", 
                // avoid overflow
                MIN(sizeof(G_ux.string_buffer)-1,G_ux.layout_paging.lengths[lineidx]), 
                (text ? STRPIC(text) : G_ux.externalText) + G_ux.layout_paging.offsets[lineidx]);
        G_ux.tmp_element.text = G_ux.string_buffer;

        G_ux.tmp_element.component.font_id = ((G_ux.layout_paging.format & PAGING_FORMAT_NB) == PAGING_FORMAT_NB) ?
                                              (BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER)
                                            : (BAGL_FONT_OPEN_SANS_REGULAR_11px   | BAGL_FONT_ALIGNMENT_CENTER);
      }
      break;
    }
  }
  return &G_ux.tmp_element;
}

static const bagl_element_t* ux_layout_paging_prepro_by_addr(const bagl_element_t* element) {
  // don't display if null
  const ux_layout_paging_params_t* params = (const ux_layout_paging_params_t*)ux_stack_get_current_step_params();

  return ux_layout_paging_prepro_common(element, params->title, params->text);
}

static const bagl_element_t* ux_layout_paging_prepro_by_func(const bagl_element_t* element) {
  // don't display if null
  const ux_layout_paging_func_params_t* params = (const ux_layout_paging_func_params_t*)ux_stack_get_current_step_params();

  return ux_layout_paging_prepro_common(element, params->get_title(), params->get_text());
}

// redisplay current page
void ux_layout_paging_redisplay_common(unsigned int stack_slot, const char* text, button_push_callback_t button_callback, bagl_element_callback_t prepro) {
  ux_stack_slot_t* slot = &G_ux.stack[stack_slot];
#if (BAGL_WIDTH==128 && BAGL_HEIGHT==64)
  slot->element_arrays[0].element_array = ux_layout_paging_elements;
  slot->element_arrays[0].element_array_count = ARRAYLEN(ux_layout_paging_elements);
  slot->element_arrays_count = 1;
#else
  ux_layout_bb_init_common(stack_slot);
#endif // (BAGL_WIDTH==128 && BAGL_HEIGHT==64)

  // request offsets and lengths of lines for the current page
  ux_layout_paging_compute(text, 
                           G_ux.layout_paging.current, 
                           &G_ux.layout_paging,
                           LINE_FONT);

  slot->screen_before_element_display_callback = prepro;
  slot->button_push_callback = button_callback;
  ux_stack_display(stack_slot);
}

static unsigned int ux_layout_paging_button_callback_by_addr(unsigned int button_mask, unsigned int button_mask_counter);
static unsigned int ux_layout_paging_button_callback_by_func(unsigned int button_mask, unsigned int button_mask_counter);


void ux_layout_paging_redisplay_by_addr(unsigned int stack_slot) {
  const ux_layout_paging_params_t* params = (const ux_layout_paging_params_t*)ux_stack_get_current_step_params();
  ux_layout_paging_redisplay_common(stack_slot, params->text, ux_layout_paging_button_callback_by_addr, ux_layout_paging_prepro_by_addr);
}

void ux_layout_paging_redisplay_by_func(unsigned int stack_slot) {
  const ux_layout_paging_func_params_t* params = (const ux_layout_paging_func_params_t*)ux_stack_get_current_step_params();
  ux_layout_paging_redisplay_common(stack_slot, params->get_text(), ux_layout_paging_button_callback_by_func, ux_layout_paging_prepro_by_func);
}


static void ux_layout_paging_next(ux_layout_paging_redisplay_t redisplay) {
  if (G_ux.layout_paging.current == G_ux.layout_paging.count-1) {
    ux_flow_next();
  }
  else {
    // display next page, count the number of char to fit in the next page
    G_ux.layout_paging.current++;
    redisplay(G_ux.stack_count-1);
  }
}

static void ux_layout_paging_prev(ux_layout_paging_redisplay_t redisplay) {
  if (G_ux.layout_paging.current == 0) {
    ux_flow_prev();
  }
  else {
    // display previous page, count the number of char to fit in the previous page
    G_ux.layout_paging.current--;
    redisplay(G_ux.stack_count-1);
  }
}

static unsigned int ux_layout_paging_button_callback_common(unsigned int button_mask, unsigned int button_mask_counter, ux_layout_paging_redisplay_t redisplay) {
  UNUSED(button_mask_counter);
  switch(button_mask) {
    case BUTTON_EVT_RELEASED|BUTTON_LEFT:
      ux_layout_paging_prev(redisplay);
      break;
    case BUTTON_EVT_RELEASED|BUTTON_RIGHT:
      ux_layout_paging_next(redisplay);
      break;
    case BUTTON_EVT_RELEASED|BUTTON_LEFT|BUTTON_RIGHT:
      if (G_ux.layout_paging.count == 0 
        || G_ux.layout_paging.count-1 == G_ux.layout_paging.current) {
        ux_flow_validate();
      }
      break;
  }
  return 0;
}

static unsigned int ux_layout_paging_button_callback_by_addr(unsigned int button_mask, unsigned int button_mask_counter) {
  return ux_layout_paging_button_callback_common(button_mask, button_mask_counter, ux_layout_paging_redisplay_by_addr);
}

static unsigned int ux_layout_paging_button_callback_by_func(unsigned int button_mask, unsigned int button_mask_counter) {
  return ux_layout_paging_button_callback_common(button_mask, button_mask_counter, ux_layout_paging_redisplay_by_func);
}


void ux_layout_paging_init_common(unsigned int stack_slot, const char* text, ux_layout_paging_redisplay_t redisplay) {

  // At this very moment, we don't want to get rid of the format, but keep
  // the one which has just been set (in case of direction backward or forward).
  unsigned int backup_format = G_ux.layout_paging.format;

  // depending flow browsing direction, select the correct page to display
  switch(ux_flow_direction()) {
    case FLOW_DIRECTION_BACKWARD:
      ux_layout_paging_reset();
      // ask the paging to start at the last page.
      // This step must be performed after the 'ux_layout_paging_reset' call,
      // thus we cannot mutualize the call with the one in the 'forward' case.
      G_ux.layout_paging.current = -1UL;
      break;
    case FLOW_DIRECTION_FORWARD:
      // open the first page
      ux_layout_paging_reset();
      break;
    case FLOW_DIRECTION_START:
      // shall already be at the first page
      break;
  }

  G_ux.layout_paging.format = backup_format;

  // store params
  ux_stack_init(stack_slot);

  // compute number of chars to display from the params complete string
  if ((text == NULL) && (G_ux.externalText == NULL)) {
    text = ""; // empty string to avoid disrupting the ux flow.
  }

  // count total number of pages
  G_ux.layout_paging.count = ux_layout_paging_compute(text, -1UL, &G_ux.layout_paging, LINE_FONT); // at least one page

  if (G_ux.layout_paging.count == 0) {
    ux_layout_paging_reset();
  }

  // if (start != end) {
  //   ux_layout_paging_reset();
  // }

  // perform displaying the last page as requested (-1UL in prevstep hook does this)
  if (G_ux.layout_paging.count && G_ux.layout_paging.current > G_ux.layout_paging.count-1UL) {
    G_ux.layout_paging.current = G_ux.layout_paging.count-1;
  }

  redisplay(stack_slot);
}

void ux_layout_paging_init(unsigned int stack_slot) {
  const ux_layout_paging_params_t* params = (const ux_layout_paging_params_t*)ux_stack_get_step_params(stack_slot);
  ux_layout_paging_init_common(stack_slot, params->text, ux_layout_paging_redisplay_by_addr);
}

void ux_layout_paging_func_init(unsigned int stack_slot) {
  const ux_layout_paging_func_params_t* params = (const ux_layout_paging_func_params_t*)ux_stack_get_step_params(stack_slot);
  if (params->get_text == NULL) {
    return;
  }
  ux_layout_paging_init_common(stack_slot, params->get_text(), ux_layout_paging_redisplay_by_func);
}

void ux_layout_xx_paging_init(unsigned int stack_slot, unsigned int format) {
  G_ux.layout_paging.format = format;
  ux_layout_paging_init(stack_slot);
}

void ux_layout_nn_paging_init(unsigned int stack_slot) {
  ux_layout_xx_paging_init(stack_slot, PAGING_FORMAT_NN);
}

void ux_layout_nb_paging_init(unsigned int stack_slot) {
  ux_layout_xx_paging_init(stack_slot, PAGING_FORMAT_NB);
}

void ux_layout_bn_paging_init(unsigned int stack_slot) {
  ux_layout_xx_paging_init(stack_slot, PAGING_FORMAT_BN);
}

void ux_layout_bb_paging_init(unsigned int stack_slot) {
  ux_layout_xx_paging_init(stack_slot, PAGING_FORMAT_BB);
}

// function callable externally which reset the paging (to be called before init when willing to redisplay the first page)
void ux_layout_paging_reset(void) {
  memset(&G_ux.layout_paging, 0, sizeof(G_ux.layout_paging));
}

#endif // HAVE_BAGL

#endif // HAVE_UX_FLOW
