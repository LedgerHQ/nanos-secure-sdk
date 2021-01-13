
#include "ux.h"
#include "string.h"

#ifdef TARGET_NANOX
// requires BAGL font table to compute line width

#ifdef HAVE_BAGL
/*********************************************************************************
 * 4 text lines
 */
#define LINE_FONT BAGL_FONT_OPEN_SANS_REGULAR_11px
#define PIXEL_PER_LINE 114

void ux_layout_bnnn_paging_reset(void);

static const bagl_element_t ux_layout_bnnn_paging_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL, 0, 0, 0, NULL, NULL, NULL},

  {{BAGL_ICON                           , 0x01,   2,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_left, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_ICON                           , 0x02, 122,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, (const char*)&C_icon_right, 0, 0, 0, NULL, NULL, NULL },

  {{BAGL_LABELINE                       , 0x03,   0,  15, 128,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x10,   (128-PIXEL_PER_LINE)/2,  29, PIXEL_PER_LINE,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, LINE_FONT|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x11,   (128-PIXEL_PER_LINE)/2,  43, PIXEL_PER_LINE,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, LINE_FONT|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x12,   (128-PIXEL_PER_LINE)/2,  57, PIXEL_PER_LINE,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, LINE_FONT|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
};

static const bagl_element_t* ux_layout_bnnn_paging_prepro(const bagl_element_t* element) {
  // don't display if null
  const ux_layout_bnnn_paging_params_t* params = (const ux_layout_bnnn_paging_params_t*)ux_stack_get_current_step_params();

	// ocpy element before any mod
	os_memmove(&G_ux.tmp_element, element, sizeof(bagl_element_t));

  // for dashboard, setup the current application's name
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

    case 0x03:
      // display 
    	if (params->title) {
        SPRINTF(G_ux.string_buffer, (G_ux.layout_paging.count>1)?"%s (%d/%d)":"%s", STRPIC(params->title), G_ux.layout_paging.current+1, G_ux.layout_paging.count);
    	}
      else {
        SPRINTF(G_ux.string_buffer, "%d/%d", G_ux.layout_paging.current+1, G_ux.layout_paging.count);
      }
      G_ux.tmp_element.text = G_ux.string_buffer;
      break;

    case 0x10:
    case 0x11:
    case 0x12: {
      unsigned int lineidx = element->component.userid&0xF;
    	if (G_ux.layout_paging.lengths[lineidx]) {
    		SPRINTF(G_ux.string_buffer, "%.*s", G_ux.layout_paging.lengths[lineidx], STRPIC(params->text) + G_ux.layout_paging.offsets[lineidx]);
        G_ux.tmp_element.text = G_ux.string_buffer;
    	}
      break;
    }
  }
  return &G_ux.tmp_element;
}

static unsigned int ux_layout_bnnn_paging_button_callback(unsigned int button_mask, unsigned int button_mask_counter);

static unsigned int is_word_delim(unsigned char c) {
  // return !((c >= 'a' && c <= 'z') 
  //       || (c >= 'A' && c <= 'Z')
  //       || (c >= '0' && c <= '9'));
  return c == ' ' || c == '\n' || c == '\t' || c == '-' || c == '_';
}

// return the number of pages to be displayed when current page to show is -1
unsigned int ux_layout_bnnn_paging_compute(unsigned int stack_slot, unsigned int page_to_display) {
  const ux_layout_bnnn_paging_params_t* params = (const ux_layout_bnnn_paging_params_t*)ux_stack_get_step_params(stack_slot);

  // reset length and offset of lines
  os_memset(&G_ux.layout_paging.offsets, 0, sizeof(G_ux.layout_paging.offsets));
  os_memset(&G_ux.layout_paging.lengths, 0, sizeof(G_ux.layout_paging.lengths));

  // a page has been asked, but no page exists
  if (page_to_display != -1UL && G_ux.layout_paging.count == 0) {
    return 0;
  }

  // compute offset/length of text of each line for the current page
  unsigned int page = 0;
  unsigned int line = 0;
  const char* start = STRPIC(params->text);
  const char* start2 = start;
  const char* end = start + strlen(start);
  while (start < end) {
    unsigned int len = 0;
    unsigned int linew = 0; 
    const char* last_word_delim = start;
    // not reached end of content
    while (start + len < end
      // line is not full
      && linew <= PIXEL_PER_LINE) {
      // compute new line length
      linew = bagl_compute_line_width(LINE_FONT, 0, start, len+1, BAGL_ENCODING_LATIN1);
      //if (start[len] )
      if (linew > PIXEL_PER_LINE) {
        // we got a full line
        break;
      }
      unsigned char c = start[len];
      if (is_word_delim(c)) {
        last_word_delim = &start[len];
      }
      len++;
      // new line, don't go further
      if (c == '\n') {
        break;
      }
    }

    // if not splitting line onto a word delimiter, then cut at the previous word_delim, adjust len accordingly (and a wor delim has been found already)
    if (start + len < end && last_word_delim != start && len) {
      // if line split within a word
      if ((!is_word_delim(start[len-1]) && !is_word_delim(start[len]))) {
        len = last_word_delim - start;
      }
    }

    // fill up the paging structure
    if (page_to_display != -1UL && G_ux.layout_paging.current == page && G_ux.layout_paging.current < G_ux.layout_paging.count) {
      G_ux.layout_paging.offsets[line] = start - start2;
      G_ux.layout_paging.lengths[line] = len;

      // won't compute all pages, we reached the one to display
      if (line >= UX_LAYOUT_PAGING_LINE-1) {
        return page;
      }
    }

    // prepare for next line
    start += len;

    // skip to next line/page
    line++;
    if (line >= UX_LAYOUT_PAGING_LINE && start < end) {
      page++;
      line = 0;
    }
  }
  return page+1;
}

// redisplay current page
void ux_layout_bnnn_paging_redisplay(unsigned int stack_slot) {

  // request offsets and lengths of lines for the current page
  ux_layout_bnnn_paging_compute(stack_slot, G_ux.layout_paging.current);

  G_ux.stack[stack_slot].element_arrays[0].element_array = ux_layout_bnnn_paging_elements;
  G_ux.stack[stack_slot].element_arrays[0].element_array_count = ARRAYLEN(ux_layout_bnnn_paging_elements);
  G_ux.stack[stack_slot].element_arrays_count = 1;
  G_ux.stack[stack_slot].screen_before_element_display_callback = ux_layout_bnnn_paging_prepro;
  G_ux.stack[stack_slot].button_push_callback = ux_layout_bnnn_paging_button_callback;
  ux_stack_display(stack_slot);
}

static void ux_layout_bnnn_paging_next(void) {
  if (G_ux.layout_paging.current == G_ux.layout_paging.count-1) {
    ux_flow_next();
  }
  else {
    // display next page, count the number of char to fit in the next page
    G_ux.layout_paging.current++;
    ux_layout_bnnn_paging_redisplay(G_ux.stack_count-1);
  }
}

static void ux_layout_bnnn_paging_prev(void) {
  if (G_ux.layout_paging.current == 0) {
    ux_flow_prev();
  }
  else {
    // display previous page, count the number of char to fit in the previous page
    G_ux.layout_paging.current--;
    ux_layout_bnnn_paging_redisplay(G_ux.stack_count-1);
  }
}

static unsigned int ux_layout_bnnn_paging_button_callback(unsigned int button_mask, unsigned int button_mask_counter) {
  UNUSED(button_mask_counter);
  switch(button_mask) {
    case BUTTON_EVT_RELEASED|BUTTON_LEFT:
      ux_layout_bnnn_paging_prev();
      break;
    case BUTTON_EVT_RELEASED|BUTTON_RIGHT:
      ux_layout_bnnn_paging_next();
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

unsigned short bagl_compute_line_width(unsigned short font_id, unsigned short width, const void * text, unsigned char text_length, unsigned char text_encoding);

void ux_layout_bnnn_paging_init(unsigned int stack_slot) {

  // depending flow browsing direction, select the correct page to display
  if (G_ux.flow_stack[stack_slot].index < G_ux.flow_stack[stack_slot].prev_index) {
    // ask the paging to start at the last page
    ux_layout_bnnn_paging_reset();
    G_ux.layout_paging.current = -1UL;
  }
  else if (G_ux.flow_stack[stack_slot].index > G_ux.flow_stack[stack_slot].prev_index) {
    // open the first page
    ux_layout_bnnn_paging_reset();
  }
  else {
    // don't reset paging
  }

  // store params
  ux_stack_init(stack_slot);
  const ux_layout_bnnn_paging_params_t* params = (const ux_layout_bnnn_paging_params_t*)ux_stack_get_step_params(stack_slot);

  // compute number of chars to display from the params complete string 
  if (params->text == NULL /* && strlen(STRPIC(params->text)) == 0*/) {
    // nothgin to display
    ux_layout_bnnn_paging_reset();
    return;
  }

  // count total number of pages
  G_ux.layout_paging.count = ux_layout_bnnn_paging_compute(stack_slot, -1UL); // at least one page

  if (G_ux.layout_paging.count == 0) {
    ux_layout_bnnn_paging_reset();
  }

  // if (start != end) {
  //   ux_layout_bnnn_paging_reset();
  // }

  // perform displaying the last page as requested (-1UL in prevstep hook does this)
  if (G_ux.layout_paging.count && G_ux.layout_paging.current > G_ux.layout_paging.count-1UL) {
    G_ux.layout_paging.current = G_ux.layout_paging.count-1;
  }

  ux_layout_bnnn_paging_redisplay(stack_slot);
}

// function callable externally which reset the paging (to be called before init when willing to redisplay the first page)
void ux_layout_bnnn_paging_reset(void) {
  os_memset(&G_ux.layout_paging, 0, sizeof(G_ux.layout_paging));
}

#endif // HAVE_BAGL

#endif // TARGET_NANOX
