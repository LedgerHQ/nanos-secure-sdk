#include "ux.h"

static unsigned int ux_layout_ticker_callback(unsigned int unused) {
  UNUSED(unused);
  // when the callback is called, then the slot is forcefully the top of stack
  G_ux.stack[G_ux.stack_count-1].ticker_interval = 0;
  G_ux.stack[G_ux.stack_count-1].ticker_value = 0;
  G_ux.stack[G_ux.stack_count-1].ticker_callback = NULL;
  // auto validate the step
  ux_flow_validate();
  return 0;
}

void ux_layout_set_timeout(unsigned int stack_slot, unsigned int ms) {
  if (ms) {
    G_ux.stack[stack_slot].ticker_callback = ux_layout_ticker_callback;
    G_ux.stack[stack_slot].ticker_value = ms;
    G_ux.stack[stack_slot].ticker_interval = ms; // no restart
  }
}
