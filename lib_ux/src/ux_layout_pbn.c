
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2022 Ledger
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

const bagl_element_t* ux_layout_pbn_prepro(const bagl_element_t* element) {
  const bagl_element_t* e = ux_layout_pbb_prepro(element);
  if (e && G_ux.tmp_element.component.userid == 0x11) {
    // The centering depends on the target.
#if (BAGL_WIDTH==128 && BAGL_HEIGHT==64)
    G_ux.tmp_element.component.font_id = BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER;
#elif (BAGL_WIDTH==128 && BAGL_HEIGHT==32)
    G_ux.tmp_element.component.font_id = BAGL_FONT_OPEN_SANS_REGULAR_11px;
#else
  #error "BAGL_WIDTH/BAGL_HEIGHT not defined"
#endif
  }
  return e;
}

void ux_layout_pbn_init(unsigned int stack_slot) {
  ux_layout_pbb_init_common(stack_slot);
  G_ux.stack[stack_slot].screen_before_element_display_callback = ux_layout_pbn_prepro;
  ux_stack_display(stack_slot);
}

#endif // HAVE_UX_FLOW
