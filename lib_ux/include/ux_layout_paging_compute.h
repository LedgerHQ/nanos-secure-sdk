#ifndef UX_LAYOUT_PAGING_COMPUTE_H
#define UX_LAYOUT_PAGING_COMPUTE_H

#include "ux_layouts.h"

#ifdef HAVE_UX_FLOW

#define PIXEL_PER_LINE 114

#define PAGING_FORMAT_NN 0x00
#define PAGING_FORMAT_BN 0xF0
#define PAGING_FORMAT_NB 0x0F
#define PAGING_FORMAT_BB (PAGING_FORMAT_BN | PAGING_FORMAT_NB)

#if defined(HAVE_INDEXED_STRINGS)
#define LINE_BUFFER_SIZE (32 - 1)

#define MAX_PAGING_LINE_COUNT (UX_LAYOUT_PAGING_LINE_COUNT + 1)
#else  // defined(HAVE_INDEXED_STRINGS)
#define MAX_PAGING_LINE_COUNT UX_LAYOUT_PAGING_LINE_COUNT
#endif  // defined(HAVE_INDEXED_STRINGS)

typedef struct {
    unsigned int current;
    unsigned int count;
#if defined(HAVE_INDEXED_STRINGS)
    unsigned int paging_title;
#endif  // defined(HAVE_INDEXED_STRINGS)
    unsigned char format;
#if defined(HAVE_INDEXED_STRINGS)
    unsigned char string_buffer_id;
#endif  // defined(HAVE_INDEXED_STRINGS)
    unsigned short offsets[MAX_PAGING_LINE_COUNT];
    unsigned short lengths[MAX_PAGING_LINE_COUNT];
#if defined(HAVE_INDEXED_STRINGS)
    unsigned char fond_ids[MAX_PAGING_LINE_COUNT];
    char          line_buffer[LINE_BUFFER_SIZE + 1];
#endif  // defined(HAVE_INDEXED_STRINGS)
} ux_layout_paging_state_t;

#if !defined(HAVE_SE_SCREEN)
uint8_t se_get_cropped_length(const char *text,
                              uint8_t     text_length,
                              uint32_t    width_limit_in_pixels,
                              uint8_t     text_format);

#if defined(HAVE_INDEXED_STRINGS)
unsigned int se_compute_line_width_light(const char *text,
                                         uint8_t     text_length,
                                         uint8_t     text_format);
#endif  //! defined(HAVE_INDEXED_STRINGS)
#endif  // !defined(HAVE_SE_SCREEN)
unsigned int ux_layout_paging_compute(const char               *text_to_split,
                                      unsigned int              page_to_display,
                                      ux_layout_paging_state_t *paging_state,
                                      bagl_font_id_e            font);

#endif  // HAVE_UX_FLOW

#endif  // UX_LAYOUT_PAGING_COMPUTE_H
