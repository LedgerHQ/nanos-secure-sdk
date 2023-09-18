#pragma once

#ifdef HAVE_UX_FLOW

/**
 * Common definition for factorized layouts
 */

typedef void (*ux_layout_paging_redisplay_t)(unsigned int stack_slot);

void ux_layout_bb_init_common(unsigned int stack_slot);

const bagl_element_t *ux_layout_pbb_prepro(const bagl_element_t *element);
void                  ux_layout_pbb_init_common(unsigned int stack_slot);

#if defined(HAVE_INDEXED_STRINGS)
void ux_layout_pages_init_common(unsigned int                 stack_slot,
                                 const char                  *text,
                                 ux_layout_paging_redisplay_t redisplay);
#endif  // defined(HAVE_INDEXED_STRINGS)

void                  ux_layout_pages_display_init(const char *text);
const bagl_element_t *ux_layout_pages_display_element(const bagl_element_t *element,
                                                      const char           *text);

#endif  // HAVE_UX_FLOW
