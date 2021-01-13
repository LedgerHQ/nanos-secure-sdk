#include "ux.h"

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
  G_ux.menulist_params.line1 = G_ux.menulist_getter(G_ux.menulist_current-2);
  G_ux.menulist_params.line2 = G_ux.menulist_getter(G_ux.menulist_current-1);
  G_ux.menulist_params.line3 = G_ux.menulist_getter(G_ux.menulist_current);
  G_ux.menulist_params.line4 = G_ux.menulist_getter(G_ux.menulist_current+1);
  G_ux.menulist_params.line5 = G_ux.menulist_getter(G_ux.menulist_current+2);
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
