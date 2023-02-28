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
void nbgl_frontDrawRect(nbgl_area_t *area);
void nbgl_frontDrawHorizontalLine(nbgl_area_t *area, uint8_t mask, color_t lineColor);
void nbgl_frontDrawImage(nbgl_area_t *area, uint8_t* buffer, nbgl_transformation_t transformation, nbgl_color_map_t colorMap);
void nbgl_frontDrawImageFile(nbgl_area_t *area, uint8_t* buffer, nbgl_color_map_t colorMap, uint8_t *uzlib_chunk_buffer);
void nbgl_frontDrawImageRle(nbgl_area_t *area, uint8_t *buffer, uint32_t buffer_len, color_t fore_color);

void nbgl_frontRefreshArea(nbgl_area_t * area, nbgl_refresh_mode_t mode);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_FRONT_H */
