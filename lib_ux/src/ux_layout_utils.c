
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
    G_ux.stack[stack_slot].ticker_interval = ms; // restart
  }
}

const bagl_element_t* ux_layout_strings_prepro(const bagl_element_t* element) {
  // don't display if null
  const ux_layout_strings_params_t* params = (const ux_layout_strings_params_t*)ux_stack_get_current_step_params();
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

    default:
      if (G_ux.tmp_element.component.userid&0xF0) {
        G_ux.tmp_element.text = params->lines[G_ux.tmp_element.component.userid&0xF];
      }
      break;
  }
  return &G_ux.tmp_element;
}

#endif // HAVE_UX_FLOW
