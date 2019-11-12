
#include "os.h"
#include "os_io_seproxyhal.h"
#include "ux.h"
 
// current ux_menu context (could be pluralised if multiple nested levels of menu are required within bolos_ux for example)
ux_menu_state_t ux_menu;

const bagl_element_t ux_menu_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x80,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL},

  // icons
  {{BAGL_ICON                           , 0x81,   3,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_UP   }, NULL},
  {{BAGL_ICON                           , 0x82, 118,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_DOWN }, NULL},
  

  // previous setting name
  {{BAGL_LABELINE                       , 0x41,  14,   3, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  // next setting name
  {{BAGL_LABELINE                       , 0x42,  14,  35, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},

  // current setting (x to be adjusted if icon is present etc)
  {{BAGL_ICON                           , 0x10,  14,   9,   0,   0, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, NULL},
  // single line layout
  {{BAGL_LABELINE                       , 0x20,  14,  19, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},

  // 2 lines layout + icon
  {{BAGL_LABELINE                       , 0x21,  14,  12, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x22,  14,  26, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},

};

const ux_menu_entry_t* ux_menu_get_entry (unsigned int entry_idx) {
  if (ux_menu.menu_iterator) {
    return ux_menu.menu_iterator(entry_idx);
  } 
  return &ux_menu.menu_entries[entry_idx];
} 

const bagl_element_t* ux_menu_element_preprocessor(const bagl_element_t* element) {
  //todo avoid center alignment when text_x or icon_x AND text_x are not 0
  os_memmove(&ux_menu.tmp_element, element, sizeof(bagl_element_t));

  // ask the current entry first, to setup other entries
  const ux_menu_entry_t* current_entry = (const ux_menu_entry_t*)PIC(ux_menu_get_entry(ux_menu.current_entry));

  const ux_menu_entry_t* previous_entry = NULL;
  if (ux_menu.current_entry) {
    previous_entry = (const ux_menu_entry_t*)PIC(ux_menu_get_entry(ux_menu.current_entry-1));
  }
  const ux_menu_entry_t* next_entry = NULL;
  if (ux_menu.current_entry < ux_menu.menu_entries_count-1) {
    next_entry = (const ux_menu_entry_t*)PIC(ux_menu_get_entry(ux_menu.current_entry+1));
  }

  switch(element->component.userid) {
    case 0x81:
      if (ux_menu.current_entry == 0) {
        return NULL;
      }
      break;
    case 0x82:
      if (ux_menu.current_entry == ux_menu.menu_entries_count-1) {
        return NULL;
      }
      break;
    // previous setting name
    case 0x41:
      if (!current_entry
        ||current_entry->line2 != NULL 
        || current_entry->icon != NULL
        || ux_menu.current_entry == 0
        || ux_menu.menu_entries_count == 1 
        || !previous_entry
        || previous_entry->icon != NULL
        || previous_entry->line2 != NULL) {
        return 0;
      }
      ux_menu.tmp_element.text = previous_entry->line1;
      break;
    // next setting name
    case 0x42:
      if (!current_entry
        || current_entry->line2 != NULL 
        || current_entry->icon != NULL
        || ux_menu.current_entry == ux_menu.menu_entries_count-1
        || ux_menu.menu_entries_count == 1
        || !next_entry
        || next_entry->icon != NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = next_entry->line1;
      break;
    case 0x10:
      if (!current_entry || current_entry->icon == NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = (const char*)current_entry->icon;
      if (current_entry->icon_x) {
        ux_menu.tmp_element.component.x = current_entry->icon_x;
      }
      break;
    case 0x20:
      if (!current_entry || current_entry->line2 != NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = current_entry->line1;
      goto adjust_text_x;
    case 0x21:
      if (!current_entry || current_entry->line2 == NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = current_entry->line1;
      goto adjust_text_x;
    case 0x22:
      if (!current_entry || current_entry->line2 == NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = current_entry->line2;
    adjust_text_x:
      if (current_entry && current_entry->text_x) {
        ux_menu.tmp_element.component.x = current_entry->text_x;
        // discard the 'center' flag
        ux_menu.tmp_element.component.font_id = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px;
      }
      break;
  }
  // ensure prepro agrees to the element to be displayed
  if (ux_menu.menu_entry_preprocessor) {
    // menu is denied by the menu entry preprocessor
    return ux_menu.menu_entry_preprocessor(current_entry, &ux_menu.tmp_element);
  }

  return &ux_menu.tmp_element;
}

unsigned int ux_menu_elements_button (unsigned int button_mask, unsigned int button_mask_counter) {
  UNUSED(button_mask_counter);

  const ux_menu_entry_t* current_entry = (const ux_menu_entry_t*)PIC(ux_menu_get_entry(ux_menu.current_entry));

  switch (button_mask) {
    // enter menu or exit menu
    case BUTTON_EVT_RELEASED|BUTTON_LEFT|BUTTON_RIGHT:
      // menu is priority 1
      if (current_entry->menu) {
        // use userid as the pointer to current entry in the parent menu
        UX_MENU_DISPLAY(current_entry->userid, (const ux_menu_entry_t*)PIC(current_entry->menu), ux_menu.menu_entry_preprocessor);
        return 0;
      }
      // else callback
      else if (current_entry->callback) {
        ((ux_menu_callback_t)PIC(current_entry->callback))(current_entry->userid);
        return 0;
      }
      break;

    case BUTTON_EVT_FAST|BUTTON_LEFT:
    case BUTTON_EVT_RELEASED|BUTTON_LEFT:
      // entry 0 is the number of entries in the menu list
      if (ux_menu.current_entry == 0) {
        return 0;
      }
      ux_menu.current_entry--;
      goto redraw;

    case BUTTON_EVT_FAST|BUTTON_RIGHT:
    case BUTTON_EVT_RELEASED|BUTTON_RIGHT:
      // entry 0 is the number of entries in the menu list
      if (ux_menu.current_entry >= ux_menu.menu_entries_count-1) {
        return 0;
      }
      ux_menu.current_entry++;
    redraw:
#ifdef HAVE_BOLOS_UX
      screen_display_init(0);
#else
      UX_REDISPLAY();
#endif
      return 0;
  }
  return 1;
}

const ux_menu_entry_t UX_MENU_END_ENTRY = UX_MENU_END;

void ux_menu_display(unsigned int current_entry, 
                     const ux_menu_entry_t* menu_entries,
                     ux_menu_preprocessor_t menu_entry_preprocessor) {
  // reset to first entry
  ux_menu.menu_entries_count = 0;

  // count entries
  if (menu_entries) {
    for(;;) {
      if (os_memcmp(&menu_entries[ux_menu.menu_entries_count], &UX_MENU_END_ENTRY, sizeof(ux_menu_entry_t)) == 0) {
        break;
      }
      ux_menu.menu_entries_count++;
    }
  }

  if (current_entry != UX_MENU_UNCHANGED_ENTRY) {
    ux_menu.current_entry = current_entry;
    if (ux_menu.current_entry > ux_menu.menu_entries_count) {
      ux_menu.current_entry = 0;
    }
  }
  ux_menu.menu_entries = menu_entries;
  ux_menu.menu_entry_preprocessor = menu_entry_preprocessor;
  ux_menu.menu_iterator = NULL;

#ifdef HAVE_BOLOS_UX
  screen_state_init(0);

  // static dashboard content
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array = ux_menu_elements;
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count = ARRAYLEN(ux_menu_elements);
  G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

  // ensure the string_buffer will be set before each button is displayed
  G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback = ux_menu_element_preprocessor;
  G_bolos_ux_context.screen_stack[0].button_push_callback = ux_menu_elements_button;

  screen_display_init(0);
#else
  // display the menu current entry
  UX_DISPLAY(ux_menu_elements, ux_menu_element_preprocessor);
#endif
}

ux_turner_state_t ux_turner;

const bagl_element_t ux_turner_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL},

  // icons
  {{BAGL_ICON                           , 0x00,   3,  12,   7,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_CROSS  }, NULL},
  {{BAGL_ICON                           , 0x00, 117,  13,   8,   6, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_CHECK  }, NULL},

  // current setting (x to be adjusted if icon is present etc)
  {{BAGL_ICON                           , 0x03,   0,   9,  14,  14, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, NULL},

  // single line layout
  {{BAGL_LABELINE                       , 0x04,   0,  19, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  // 2 lines layout + icon
  {{BAGL_LABELINE                       , 0x05,   0,  12, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},
  {{BAGL_LABELINE                       , 0x06,   0,  26, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL},

};

const bagl_element_t* ux_turner_element_preprocessor(const bagl_element_t* element) {
  //todo avoid center alignment when text_x or icon_x AND text_x are not 0
  os_memmove(&ux_turner.tmp_element, element, sizeof(bagl_element_t));

  switch(element->component.userid) {

    case 0x03:
      if (ux_turner.steps[ux_turner.current_step].icon == NULL) {
        return NULL;
      }
      ux_turner.tmp_element.text = (const char*)ux_turner.steps[ux_turner.current_step].icon;
      if (ux_turner.steps[ux_turner.current_step].icon_x) {
        ux_turner.tmp_element.component.x = ux_turner.steps[ux_turner.current_step].icon_x;
      }
      break;
    case 0x04:
      if (ux_turner.steps[ux_turner.current_step].line2 != NULL) {
        return NULL;
      }
      if (ux_turner.steps[ux_turner.current_step].fontid1) {
        ux_turner.tmp_element.component.font_id = ux_turner.steps[ux_turner.current_step].fontid1;
      }
      ux_turner.tmp_element.text = ux_turner.steps[ux_turner.current_step].line1;
      goto adjust_text_x;
    case 0x05:
      if (ux_turner.steps[ux_turner.current_step].line2 == NULL) {
        return NULL;
      }
      if (ux_turner.steps[ux_turner.current_step].fontid1) {
        ux_turner.tmp_element.component.font_id = ux_turner.steps[ux_turner.current_step].fontid1;
      }
      ux_turner.tmp_element.text = ux_turner.steps[ux_turner.current_step].line1;
      goto adjust_text_x;
    case 0x06:
      if (ux_turner.steps[ux_turner.current_step].line2 == NULL) {
        return NULL;
      }
      if (ux_turner.steps[ux_turner.current_step].fontid2) {
        ux_turner.tmp_element.component.font_id = ux_turner.steps[ux_turner.current_step].fontid2;
      }
      ux_turner.tmp_element.text = ux_turner.steps[ux_turner.current_step].line2;
    adjust_text_x:
      if (ux_turner.steps[ux_turner.current_step].text_x) {
        ux_turner.tmp_element.component.x = ux_turner.steps[ux_turner.current_step].text_x;
      }
      break;
  }
  return &ux_turner.tmp_element;
}

unsigned int ux_turner_elements_button (unsigned int button_mask, unsigned int button_mask_counter) {
  return ux_turner.button_callback(button_mask, button_mask_counter);
}

#ifdef HAVE_BOLOS_UX
unsigned int ux_turner_ticker_bolos_ux(unsigned int ignored) {
  UNUSED(ignored);
  // switch to next step
  ux_turner.current_step=(ux_turner.current_step+1)%ux_turner.steps_count;
  // setup the next change
  G_bolos_ux_context.screen_stack[0].ticker_value = ux_turner.steps[ux_turner.current_step].next_step_ms;
  G_bolos_ux_context.screen_stack[0].ticker_interval = ux_turner.steps[ux_turner.current_step].next_step_ms;
  screen_display_init(0);
  return 0;
}
#else
void ux_turner_ticker(unsigned int elapsed_ms) {
  ux_turner.elapsed_ms -= MIN(ux_turner.elapsed_ms, elapsed_ms);
  if (ux_turner.elapsed_ms == 0) {
    // switch to next step
    ux_turner.current_step=(ux_turner.current_step+1)%ux_turner.steps_count;
    ux_turner.elapsed_ms = ux_turner.steps[ux_turner.current_step].next_step_ms;
    UX_DISPLAY(ux_turner_elements, ux_turner_element_preprocessor);
  }
}
#endif // HAVE_BOLOS_UX

void ux_turner_display(unsigned int current_step, 
                     const ux_turner_step_t* steps,
                     unsigned int steps_count,
                     button_push_callback_t button_callback) {
  // reset to first entry
  ux_turner.steps_count = steps_count;

  if (current_step != UX_TURNER_UNCHANGED_ENTRY) {
    ux_turner.current_step = current_step;
    if (ux_turner.current_step > ux_turner.steps_count) {
      ux_turner.current_step = 0;
    }
  }
  ux_turner.steps = steps;

  ux_turner.button_callback = button_callback;

#ifdef HAVE_BOLOS_UX
  screen_state_init(0);

  // static dashboard content
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array = ux_turner_elements;
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count = ARRAYLEN(ux_turner_elements);
  G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

  // ensure the string_buffer will be set before each button is displayed
  G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback = ux_turner_element_preprocessor;
  G_bolos_ux_context.screen_stack[0].button_push_callback = ux_turner_elements_button;
  G_bolos_ux_context.screen_stack[0].ticker_value = ux_turner.steps[ux_turner.current_step].next_step_ms;
  G_bolos_ux_context.screen_stack[0].ticker_interval = ux_turner.steps[ux_turner.current_step].next_step_ms;
  G_bolos_ux_context.screen_stack[0].ticker_callback = ux_turner_ticker_bolos_ux;

  screen_display_init(0);
#else
  ux_turner.elapsed_ms = ux_turner.steps[ux_turner.current_step].next_step_ms;
  // display the menu current entry
  UX_DISPLAY(ux_turner_elements, ux_turner_element_preprocessor);
#endif
}

void ux_check_status_default(unsigned int status) {
  // nothing to be done here by default.
  UNUSED(status);
}
