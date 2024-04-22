/**
 * @file nbgl_fonts.h
 * Fonts types of the new BOLOS Graphical Library
 *
 */

#ifndef NBGL_FONTS_H
#define NBGL_FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
// return the number of pages to be displayed when current page to show is 0
int nbgl_font_compute_paging(const char  *text_to_split,
                             uint32_t     line_to_display,
                             uint32_t     width_limit_in_pixels,
                             bool         bold,
                             const char **line_start,
                             uint8_t     *line_len);

static inline int nbgl_font_compute_nb_page(const char *text_to_split,
                                            uint32_t    width_limit_in_pixels,
                                            bool        bold)
{
    return nbgl_font_compute_paging(text_to_split, 0, width_limit_in_pixels, bold, NULL, NULL);
}

/**********************
 *      MACROS
 **********************/
#define IS_UNICODE(__value) ((__value) > 0xF0)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_FONTS_H */
