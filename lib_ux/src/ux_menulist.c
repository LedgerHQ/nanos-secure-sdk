
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

#ifndef TARGET_BLUE

void ux_menulist_refresh(unsigned int stack_slot);

unsigned int ux_menulist_button(unsigned int button_mask, unsigned int button_mask_counter) {
  UNUSED(button_mask_counter);

  switch(button_mask) {
    case BUTTON_EVT_RELEASED|BUTTON_LEFT:
      if (G_ux.menulist_getter(G_ux.menulist_current-1UL)) {
      	G_ux.menulist_current--;
      	ux_menulist_refresh(G_ux.stack_count-1);
      }
      break;
    case BUTTON_EVT_RELEASED|BUTTON_RIGHT:
      if (G_ux.menulist_getter(G_ux.menulist_current+1UL)) {
      	G_ux.menulist_current++;
      	ux_menulist_refresh(G_ux.stack_count-1);
      }
      break;
    case BUTTON_EVT_RELEASED|BUTTON_LEFT|BUTTON_RIGHT:
      G_ux.menulist_selector(G_ux.menulist_current);
      break;
  }
  return 0;
}

void ux_menulist_refresh(unsigned int stack_slot) {
  // set values
  int i;
  for (i = 0; i < 5; i++) {
    G_ux.menulist_params.lines[i] = G_ux.menulist_getter(G_ux.menulist_current+i-2);
  }
  // display
  ux_layout_nnbnn_init(stack_slot);
  // change callback to the menulist one
  G_ux.stack[stack_slot].button_push_callback = ux_menulist_button;
}

const ux_flow_step_t ux_menulist_conststep = {
  ux_menulist_refresh,
  &G_ux.menulist_params,
  NULL,
  NULL
};
UX_FLOW(
  ux_menulist_constflow,
  &ux_menulist_conststep
  );

void ux_menulist_init_select(unsigned int stack_slot, 
                      list_item_value_t getter, 
                      list_item_select_t selector, 
                      unsigned int selected_item_idx) {
  G_ux.menulist_current  = selected_item_idx;
  G_ux.menulist_getter = getter;
  G_ux.menulist_selector = selector;

  // ensure the current flow step reference the G_ux.menulist_params to ensure strings displayed correctly.
  // if not, then use the forged step (and display it if top of ux stack)
  if (ux_stack_get_step_params(stack_slot) != (void*)&G_ux.menulist_params) {
    ux_flow_init(stack_slot, ux_menulist_constflow, NULL);
  }
  else {
    ux_menulist_refresh(stack_slot);
  }
}

// based on a nnbnn layout
void ux_menulist_init(unsigned int stack_slot, 
                             list_item_value_t getter, 
                             list_item_select_t selector) {
	ux_menulist_init_select(stack_slot, getter, selector, 0);
}

#endif // TARGET_BLUE

#endif // HAVE_UX_FLOW
