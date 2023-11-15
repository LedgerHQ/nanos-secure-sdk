/**
 * @file nbgl_buttons.h
 * Buttons management of NBGL
 *
 */

#ifndef NBGL_BUTTONS_H
#define NBGL_BUTTONS_H

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
///< Time after the beginning of continuous press on button(s) after which "continuous press" event
///< start to be sent (in 100ms)
#define CONTINOUS_PRESS_THRESHOLD 8
///< Periodicity of "continuous press" events (in 100ms)
#define CONTINUOUS_PRESS_PERIOD   3

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_buttonsHandler(uint8_t buttonState, uint32_t currentTimeMs);
void nbgl_buttonsReset(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_BUTTONS_H */
