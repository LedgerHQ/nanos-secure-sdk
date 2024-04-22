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
#include <stdint.h>

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

/**
 * @brief The different pressed buttons
 *
 */
#define LEFT_BUTTON   0x01  ///< Left button event
#define RIGHT_BUTTON  0x02  ///< Right button event
#define BOTH_BUTTONS  0x03  ///< Both buttons event
#define RELEASED_MASK 0x80  ///< released (see LSB bits to know what buttons are released)
#define CONTINUOUS_MASK \
    0x40  ///< if set, means that the button(s) is continuously pressed (this event is sent every
          ///< 300ms after the first 800ms)

typedef enum {
    BUTTON_LEFT_PRESSED = 0,          ///< Sent when Left button is released
    BUTTON_RIGHT_PRESSED,             ///< Send when Right button is released
    BUTTON_LEFT_CONTINUOUS_PRESSED,   ///< Send when Left button is continuouly pressed (sent every
                                      ///< 300ms after the first 800ms)
    BUTTON_RIGHT_CONTINUOUS_PRESSED,  ///< Send when Right button is continuouly pressed (sent every
                                      ///< 300ms after the first 800ms)
    BUTTON_BOTH_PRESSED,              ///< Sent when both buttons are released
    BUTTON_BOTH_TOUCHED,              ///< Sent when both buttons are touched
    INVALID_BUTTON_EVENT
} nbgl_buttonEvent_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_buttonsHandler(uint8_t buttonState);
void nbgl_buttonsReset(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_BUTTONS_H */
