
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

#include "ux.h"
#include "ux_layout_common.h"

#ifdef HAVE_UX_FLOW

/*********************************************************************************
 * 2 text lines
 * Uses bb layout
 */

const bagl_element_t* ux_layout_nn_prepro(const bagl_element_t* element) {
  const bagl_element_t* e = ux_layout_strings_prepro(element);
  if (e && G_ux.tmp_element.component.userid >= 0x10) {
    G_ux.tmp_element.component.font_id = BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER;
  }
  return e;
}

void ux_layout_nn_init(unsigned int stack_slot) {
  ux_layout_bb_init_common(stack_slot);
  G_ux.stack[stack_slot].screen_before_element_display_callback = ux_layout_nn_prepro;
  ux_stack_display(stack_slot);
}

#endif // HAVE_UX_FLOW
