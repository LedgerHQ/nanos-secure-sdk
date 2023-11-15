/**
 * @file nbgl_buttons.c
 * Implementation of buttons management in NBGL
 */

#ifndef HAVE_SE_TOUCH
/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "nbgl_obj.h"
#include "nbgl_debug.h"
#include "nbgl_buttons.h"
#include "nbgl_screen.h"
#include "os_pic.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t  gButtonMask            = 0;
static uint32_t gButtonSameMaskCounter = 0;

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static nbgl_buttonEvent_t maskToEvent(uint8_t mask)
{
    nbgl_buttonEvent_t event = INVALID_BUTTON_EVENT;
    switch (mask) {
        case RELEASED_MASK | LEFT_BUTTON | RIGHT_BUTTON:
            event = BUTTON_BOTH_PRESSED;
            break;

        case LEFT_BUTTON | RIGHT_BUTTON:
            event = BUTTON_BOTH_TOUCHED;
            break;

        case CONTINUOUS_MASK | LEFT_BUTTON:
            event = BUTTON_LEFT_CONTINUOUS_PRESSED;
            break;

        case RELEASED_MASK | LEFT_BUTTON:
            event = BUTTON_LEFT_PRESSED;
            break;

        case CONTINUOUS_MASK | RIGHT_BUTTON:
            event = BUTTON_RIGHT_CONTINUOUS_PRESSED;
            break;

        case RELEASED_MASK | RIGHT_BUTTON:
            event = BUTTON_RIGHT_PRESSED;
            break;
    }

    return event;
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Function to be called periodically to check touchscreen state
 * and coordinates
 * @param buttonState state of both buttons (only 2 LSB are used)
 * @param currentTimeMs current time in ms
 */
void nbgl_buttonsHandler(uint8_t buttonState, uint32_t currentTimeMs)
{
    uint8_t  button_mask;
    uint32_t button_same_mask_counter;

    (void) currentTimeMs;
    // enable speeded up long push (continuous)
    if (buttonState == gButtonMask) {
        if (buttonState == 0) {
            // nothing to be done when both buttons released twice in a row
            return;
        }
        // each 100ms ~
        gButtonSameMaskCounter++;
    }

    // append the button mask
    button_mask = gButtonMask | buttonState;

    // pre reset variable due to os_sched_exit
    button_same_mask_counter = gButtonSameMaskCounter;

    if (buttonState == 0) {
        // reset next state when both buttons are released
        gButtonMask            = 0;
        gButtonSameMaskCounter = 0;

        // notify button released event
        button_mask |= RELEASED_MASK;
    }
    else {
        gButtonMask = button_mask;
    }

    // reset counter when button mask changes
    if (buttonState != gButtonMask) {
        gButtonSameMaskCounter = 0;
    }

    // if the same button(s) is pressed more than 800 ms
    if (button_same_mask_counter >= CONTINOUS_PRESS_THRESHOLD) {
        // fast bit when pressing and timing is right (tag the event every 300ms)
        if ((button_same_mask_counter % CONTINUOUS_PRESS_PERIOD) == 0) {
            button_mask |= CONTINUOUS_MASK;
        }

        // discard the release event after a fastskip has been detected, to avoid strange at release
        // behavior and also to enable user to cancel an operation by starting triggering the fast
        // skip
        button_mask &= ~RELEASED_MASK;
    }

    nbgl_screen_t *topScreen = (nbgl_screen_t *) nbgl_screenGetTop();
    if ((topScreen != NULL) && (topScreen->buttonCallback != NULL)) {
        nbgl_buttonEvent_t event = maskToEvent(button_mask);
        if (event != INVALID_BUTTON_EVENT) {
            topScreen->buttonCallback(topScreen, event);
        }
    }
}

void nbgl_buttonsReset(void)
{
    // no button push so far
    gButtonMask            = 0;
    gButtonSameMaskCounter = 0;
}
#endif  // HAVE_SE_TOUCH
