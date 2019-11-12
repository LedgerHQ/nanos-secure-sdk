#pragma once

#ifdef HAVE_UX_FLOW

/**
 * Common definition for factorized layouts
 */

const bagl_element_t* ux_layout_bb_prepro(const bagl_element_t* element);
void ux_layout_bb_init_common(unsigned int stack_slot);

const bagl_element_t* ux_layout_pbb_prepro(const bagl_element_t* element);
void ux_layout_pbb_init_common(unsigned int stack_slot);

#endif // HAVE_UX_FLOW
