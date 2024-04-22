/**
 * @file nbgl_lns.h
 * @brief API to manage screens
 *
 */

#ifndef NBGL_LNS_H
#define NBGL_LNS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

// #include "nbgl_obj.h"
#include <stdint.h>
#include <stdbool.h>
#include "bagl.h"
#include "nbgl_buttons.h"

/**********************
 * Replace nbgl_obj.h
 **********************/
/**
 * @brief to represent a boolean state.
 */
typedef enum {
    OFF_STATE,
    ON_STATE
} nbgl_state_t;

/**********************
 * Replace nbgl_types.h
 **********************/
typedef bagl_icon_details_t nbgl_icon_details_t;

/**********************
 * Replace nbgl_step.h
 **********************/

/**
 * get the "position" of a step within a flow of several steps
 * @param _step step index from which to get the position
 * @param _nb_steps number of steps in the flow
 */
#define GET_POS_OF_STEP(_step, _nb_steps) \
    (_nb_steps < 2)                       \
        ? SINGLE_STEP                     \
        : ((_step == 0) ? FIRST_STEP      \
                        : ((_step == (_nb_steps - 1)) ? LAST_STEP : NEITHER_FIRST_NOR_LAST_STEP))

/**
 * @brief possible position for a step in a flow
 *
 */
enum {
    SINGLE_STEP,                  ///< single step flow
    FIRST_STEP,                   ///< first in a multiple steps flow
    LAST_STEP,                    ///< last in a multiple steps flow
    NEITHER_FIRST_NOR_LAST_STEP,  ///< neither first nor last in a multiple steps flow
};

///< When the flow is navigated from first to last step
#define FORWARD_DIRECTION  0x00
///< When the flow is navigated from last to first step
#define BACKWARD_DIRECTION 0x08

#define STEP_POSITION_MASK 0x07

/**
 * @brief this type contains nbgl_layoutNavIndication_t in its LSBs
 * and direction in its MSB (using @ref FORWARD_DIRECTION and @ref BACKWARD_DIRECTION)
 *
 */
typedef uint8_t nbgl_stepPosition_t;

/**********************
 * Replace nbgl_screen.h
 **********************/
/**
 * @brief prototype of function to be called when a timer on screen is fired
 */
typedef void (*nbgl_tickerCallback_t)(void);

/**
 * @brief struct to configure a screen layer
 *
 */
typedef struct nbgl_screenTickerConfiguration_s {
    nbgl_tickerCallback_t
        tickerCallback;  ///< callback called when ticker timer is fired. Set to NULL for no ticker
    uint32_t tickerValue;  ///< timer initial value, in ms (should be multiple of 100 ms). Set to 0
                           ///< for no ticker
    uint32_t tickerIntervale;  ///< for periodic timers, the intervale in ms to rearm the timer
                               ///< (should be multiple of 100 ms). Set to 0 for one-shot timers
} nbgl_screenTickerConfiguration_t;

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief prototype of function to be called when buttons are touched on a screen
 * @param event type of button event
 */
typedef void (*nbgl_lnsButtonCallback_t)(nbgl_buttonEvent_t event);

typedef struct {
    const char                *text;
    const char                *subtext;
    const nbgl_icon_details_t *icon;
    nbgl_stepPosition_t        pos;
    uint8_t                    centered : 1;
    uint8_t                    bold : 1;
    uint8_t                    vertical_nav : 1;
} nbgl_lnsScreenContent_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void nbgl_screenRedraw(void);
void nbgl_refresh(void);
void nbgl_objAllowDrawing(bool enable);
void nbgl_processUxDisplayedEvent(void);

void nbgl_lns_buttonCallback(nbgl_buttonEvent_t buttonEvent);
void nbgl_screenHandler(uint32_t intervaleMs);

void nbgl_screenDraw(nbgl_lnsScreenContent_t          *content,
                     nbgl_lnsButtonCallback_t          onActionCallback,
                     nbgl_screenTickerConfiguration_t *ticker);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_LNS_H */
