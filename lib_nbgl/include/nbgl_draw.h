/**
 * @file nbgl_draw.h
 * @brief Middle Level API of the new BOLOS Graphical Library
 *
 */

#ifndef NBGL_DRAW_H
#define NBGL_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_types.h"
#include "nbgl_fonts.h"

/*********************
 *      DEFINES
 *********************/
#define QR_V4_NB_PIX_SIZE   33 // qr V4 number of pixels
#define QR_V10_NB_PIX_SIZE  57 // qr V10 number of pixels
#define QR_MAX_PIX_SIZE     QR_V10_NB_PIX_SIZE // we support up to V10 qr code version

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_drawRoundedRect(nbgl_area_t *area, nbgl_radius_t radius, color_t innerColor);
void nbgl_drawRoundedBorderedRect(nbgl_area_t *area, nbgl_radius_t radius, uint8_t stroke, color_t innerColor, color_t borderColor);
void nbgl_drawText(nbgl_area_t *area, const char* text, uint16_t textLen, nbgl_font_id_e fontId, color_t fontColor);
void nbgl_drawQrCode(nbgl_area_t *area, uint8_t version, const char *text, color_t backgroundColor);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_DRAW_H */
