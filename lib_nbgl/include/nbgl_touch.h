/**
 * @file nbgl_touch.h
 * TouchScreen management of the new BOLOS Graphical Library
 *
 */

#ifndef NBGL_TOUCH_H
#define NBGL_TOUCH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_types.h"
#include "nbgl_obj.h"

/*********************
 *      DEFINES
 *********************/
// duration of a short touch on touch panel (in ms)
#define SHORT_TOUCH_DURATION 0
// duration of a long touch on touch panel (in ms)
#define LONG_TOUCH_DURATION  1500
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void        nbgl_touchHandler(nbgl_touchStatePosition_t *touchEvent, uint32_t currentTimeMs);
bool        nbgl_touchGetTouchedPosition(nbgl_obj_t                 *obj,
                                         nbgl_touchStatePosition_t **firstPos,
                                         nbgl_touchStatePosition_t **lastPos);
uint32_t    nbgl_touchGetTouchDuration(nbgl_obj_t *obj);
nbgl_obj_t *nbgl_touchGetObjectFromId(nbgl_obj_t *obj, uint8_t id);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_TOUCH_H */
