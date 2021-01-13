#include "ux.h"
#include "string.h"

// return true (stack slot +1) if an element 
unsigned int ux_stack_is_element_array_present(const bagl_element_t* element_array) {
  unsigned int i,j;
  for (i=0;i</*ARRAYLEN(G_ux.stack)*/G_ux.stack_count;i++) {
    for (j=0; j < G_ux.stack[i].element_arrays_count; j++) {
      if (G_ux.stack[i].element_arrays[j].element_array == element_array) {
        return i+1;
      }
    }
  }
  return 0;
}

unsigned int ux_stack_push(void) {
  // only push if an available slot exists
  if (G_ux.stack_count < ARRAYLEN(G_ux.stack)) {
    os_memset(&G_ux.stack[G_ux.stack_count], 0, sizeof(G_ux.stack[0]));
#ifdef HAVE_UX_FLOW
    os_memset(&G_ux.flow_stack[G_ux.stack_count], 0, sizeof(G_ux.flow_stack[0]));
#endif // HAVE_UX_FLOW
    G_ux.stack_count++;
  }
  // return the stack top index
  return G_ux.stack_count-1;
}
unsigned int ux_stack_pop(void) {
  unsigned int exit_code = BOLOS_UX_OK;
  // only pop if more than two stack entry (0 and 1,top is an index not a count)
  if (G_ux.stack_count > 0) {
    G_ux.stack_count--;
    exit_code = G_ux.stack[G_ux.stack_count].exit_code_after_elements_displayed;
    // wipe popped slot
    os_memset(&G_ux.stack[G_ux.stack_count], 0, sizeof(G_ux.stack[0]));
#ifdef HAVE_UX_FLOW
    os_memset(&G_ux.flow_stack[G_ux.stack_count], 0, sizeof(G_ux.flow_stack[0]));
#endif // HAVE_UX_FLOW
  }

  // prepare output code when popping the last stack screen
  if (G_ux.stack_count==0) {
    G_ux.exit_code = exit_code;
  }
  // ask for a complete redraw (optimisation due to blink must be avoided as we're returning from a modal, and within the bolos ux screen stack)
  else {
    // prepare to redraw the slot when asked
    G_ux.stack[G_ux.stack_count-1].element_index = 0;
    G_ux.stack[G_ux.stack_count-1].displayed = 0;  
  }
  // return the stack top index (or -1 if no top)
  return G_ux.stack_count-1; 
}

void ux_stack_redisplay(void) {
  // an ux step has been relayout on the screen, don't flicker by redisplaying here
  if (ux_flow_relayout()) {
    return; 
  }

  // check if any screen is schedule for displaying
  if (G_ux.stack_count > 0 && G_ux.stack_count <= ARRAYLEN(G_ux.stack)) { 
    G_ux.stack[G_ux.stack_count-1].element_index = 0;
    G_ux.stack[G_ux.stack_count-1].displayed = 0;  
    ux_stack_display(G_ux.stack_count-1);
  }
  // else return redraw for the app
  else if (G_ux.stack_count == 0) {
    if (G_ux.exit_code == BOLOS_UX_OK) {
      G_ux.exit_code = BOLOS_UX_REDRAW;
    }
  }
}

void ux_stack_insert(unsigned int stack_slot) {
  if (stack_slot >= ARRAYLEN(G_ux.stack)) {
    return; // arbitrary, this is feil that's it
  }
  // not a full stack
  if (G_ux.stack_count < ARRAYLEN(G_ux.stack)) {

    // if not inserting as top of stack, then perform move
    if (stack_slot != ARRAYLEN(G_ux.stack)-1) {
      os_memmove(&G_ux.stack[stack_slot+1], &G_ux.stack[stack_slot], (ARRAYLEN(G_ux.stack)-(stack_slot+1))*sizeof(G_ux.stack[0]));
    }
    memset(&G_ux.stack[stack_slot], 0, sizeof(ux_stack_slot_t));
#ifdef HAVE_UX_FLOW
    memset(&G_ux.flow_stack[stack_slot], 0, sizeof(ux_flow_state_t));
#endif // HAVE_UX_FLOW
    // add the slot
    G_ux.stack_count++;
    // slot not wiped
  }
  // feil, that's it
}

void ux_stack_remove(unsigned int stack_slot) {
  if (stack_slot > ARRAYLEN(G_ux.stack)-1) {
    stack_slot = ARRAYLEN(G_ux.stack)-1;
  }

  // removing something not in stack
  if (stack_slot >= G_ux.stack_count) {
    return;
  }

  // before: | screenz | removed screen | other screenz |
  // after:  | screenz | other screenz |

  if (stack_slot != ARRAYLEN(G_ux.stack)-1) {
    os_memmove(&G_ux.stack[stack_slot], &G_ux.stack[stack_slot+1], (ARRAYLEN(G_ux.stack)-(stack_slot+1))*sizeof(G_ux.stack[0]));
  }

  // wipe last slot
  ux_stack_pop();
}

// common code for all screens
void ux_stack_init(unsigned int stack_slot) {
  // reinit ux behavior (previous touched element, button push state)
  io_seproxyhal_init_ux(); // glitch upon ux_stack_display for a button being pressed in a previous screen

  /*
  // mismatch, the stack slot is not accessible !!
  if (G_ux.stack_count<stack_slot+1) {
    reset();
  }
  */

  // wipe the slot to be displayed just in case
  os_memset(&G_ux.stack[stack_slot], 0, sizeof(G_ux.stack[0]));
  
  // init current screen state
  G_ux.stack[stack_slot].exit_code_after_elements_displayed = BOLOS_UX_CONTINUE;
}

// check to process keyboard callback before screen generic callback
const bagl_element_t* ux_stack_display_element_callback(const bagl_element_t* element) {
  const bagl_element_t* el;
  if (G_ux.stack_count) {
    if (G_ux.stack[G_ux.stack_count-1].screen_before_element_display_callback) {
      el = G_ux.stack[G_ux.stack_count-1].screen_before_element_display_callback(element);
      if (!el) {
        return 0;
      }
      // legacy forvery old ux on blue and nano s
      if ((unsigned int)el != 1) {
        element = el;
      }
    }
  }
  // consider good to be displayed by default
  return element;
}

#ifdef TARGET_NANOX
void ux_stack_display_elements(ux_stack_slot_t* slot) {
  unsigned int elem_idx;
  unsigned int total_element_count;
  const bagl_element_t* element;
  unsigned int i;

  total_element_count=0;
  // can't display UX of the app, when PIN is not validated
  if (!slot->displayed 
#ifndef HAVE_BOLOS
    && (os_perso_isonboarded() != BOLOS_UX_OK || os_global_pin_is_validated() == BOLOS_UX_OK)
#endif // HAVE_BOLOS
    ) {
    for(i = 0 ; i < slot->element_arrays_count ; i++) {
      // compute elem_idx in the current array (element_index refers to the total number of element to display)
      elem_idx=slot->element_index - total_element_count;
      total_element_count += slot->element_arrays[i].element_array_count;
      // check if we're sending from this array or not
      while (elem_idx < slot->element_arrays[i].element_array_count) {
        const bagl_element_t* el;
        // pre inc before callback to allow callback to change the next element to be drawn
        slot->element_index++;

        element = &slot->element_arrays[i].element_array[elem_idx];
        el = ux_stack_display_element_callback(element);
        if (!el) {
          // skip display if requested to
          if (
            //!io_seproxyhal_spi_is_status_sent() &&
           G_ux.exit_code != BOLOS_UX_CONTINUE) {
            return;
          }
        }
        else {
          // legacy support
          if ((unsigned int)el != 1) {
            element = el;
          }
          io_seproxyhal_display(element);
        }
        elem_idx++;
      }
    }

    // TODO draw the status bar

    if (slot->element_index >= total_element_count) {
      // if screen has special stuff todo on exit
      screen_update();
      // screen marked as displayed
      slot->displayed = 1;

      // check if a displayed callback is requiring redraw
      if (slot->displayed_callback) {
        // if screen displayed callback requested one more round, then set CONTINUE exit code
        if (!slot->displayed_callback(0)) {
          slot->displayed = 0;
          G_ux.exit_code = BOLOS_UX_CONTINUE;
          return; // break;
        }
      }
      G_ux.exit_code = slot->exit_code_after_elements_displayed;
    }
  }
}
#endif // TARGET_NANOX

// common code for all screens
void ux_stack_display(unsigned int stack_slot) {
  // don't display any elements of a previous screen replacement
  if(G_ux.stack_count > 0 && stack_slot == G_ux.stack_count-1) {
    io_seproxyhal_init_ux();
    // at worse a redisplay of the current screen has been requested, ensure to redraw it correctly
    G_ux.stack[stack_slot].displayed = 0;
    G_ux.stack[stack_slot].element_index = 0;
#ifdef TARGET_NANOX
    ux_stack_display_elements(&G_ux.stack[stack_slot]); // on balenos, no need to wait for the display processed event
#endif // TARGET_NANOX
  }
  // asking to redraw below top screen (likely the app below the ux)
  else if (stack_slot == -1UL || G_ux.stack_count == 0) {
    if (G_ux.exit_code == BOLOS_UX_OK) {
      G_ux.exit_code = BOLOS_UX_REDRAW;
    }
  }
  // else don't draw (in stack insertion)
}
