
/**
 * @file nbgl_side.h
 * @brief Side screen low-Level driver API, to draw elementary forms
 *
 */

#ifndef NBGL_SIDE_H
#define NBGL_SIDE_H

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
/**
 * Width of the side screen in pixels
 */
#define SIDE_SCREEN_WIDTH 96

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_sideDrawRect(nbgl_area_t *area);
void nbgl_sideDrawHorizontalLine(nbgl_area_t *area, uint8_t mask, color_t lineColor);
void nbgl_sideDrawImage(nbgl_area_t          *area,
                        uint8_t              *buffer,
                        nbgl_transformation_t transformation,
                        nbgl_color_map_t      colorMap);
void nbgl_sideRefreshArea(nbgl_area_t *area, nbgl_post_refresh_t post_refresh);

/**********************
 *      VARIABLES
 **********************/

extern uint8_t ramBuffer[];

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_SIDE_H */
