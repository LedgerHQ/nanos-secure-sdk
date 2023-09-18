/**
 * @file nbgl_front.h
 * @brief Font screen low-Level driver API, to draw elementary forms
 *
 */

#ifndef NBGL_FRONT_H
#define NBGL_FRONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_frontDrawRect(const nbgl_area_t *area);
void nbgl_frontDrawHorizontalLine(const nbgl_area_t *area, uint8_t mask, color_t lineColor);
void nbgl_frontDrawImage(const nbgl_area_t    *area,
                         const uint8_t        *buffer,
                         nbgl_transformation_t transformation,
                         nbgl_color_map_t      colorMap);
void nbgl_frontDrawImageFile(const nbgl_area_t *area,
                             const uint8_t     *buffer,
                             nbgl_color_map_t   colorMap,
                             const uint8_t     *uzlib_chunk_buffer);
void nbgl_frontDrawImageRle(const nbgl_area_t *area,
                            const uint8_t     *buffer,
                            uint32_t           buffer_len,
                            color_t            fore_color,
                            uint8_t            nb_skipped_bytes);
void nbgl_frontRefreshArea(const nbgl_area_t  *area,
                           nbgl_refresh_mode_t mode,
                           nbgl_post_refresh_t post_refresh);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_FRONT_H */
