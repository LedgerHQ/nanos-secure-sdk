/**
 * @file nbgl_screen.h
 * @brief API to manage screens
 *
 */

#ifndef NBGL_SCREEN_H
#define NBGL_SCREEN_H

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

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief prototype of function to be called when a timer on screen is fired
 */
typedef void (*nbgl_tickerCallback_t)(void);

/**
 * @brief struct to configure a screen layer
 *
 */
typedef struct PACKED__ nbgl_screenTickerConfiguration_s {
    nbgl_tickerCallback_t
        tickerCallback;  ///< callback called when ticker timer is fired. Set to NULL for no ticker
    uint32_t tickerValue;  ///< timer initial value, in ms (should be multiple of 100 ms). Set to 0
                           ///< for no ticker
    uint32_t tickerIntervale;  ///< for periodic timers, the intervale in ms to rearm the timer
                               ///< (should be multiple of 100 ms). Set to 0 for one-shot timers
} nbgl_screenTickerConfiguration_t;

/**
 * @brief struct to represent a screen (@ref SCREEN type)
 * @note Only for internal usage
 * @note inherits from container
 *
 */
typedef struct PACKED__ nbgl_screen_s {
    nbgl_container_t                 container;  ///< common part
    nbgl_screenTickerConfiguration_t ticker;     ///< ticker configuration
#ifdef HAVE_SE_TOUCH
    nbgl_touchCallback_t
        touchCallback;  ///< function to be called on events defined in touchMask of each objects
#else                   // HAVE_SE_TOUCH
    nbgl_buttonCallback_t buttonCallback;
#endif                  // HAVE_SE_TOUCH
    struct nbgl_screen_s
        *next;  ///< pointer to screen on top of this one (or NULL is this screen is top of stack)
    struct nbgl_screen_s *previous;  ///< pointer to screen on bottom of this one (or NULL is this
                                     ///< screen is bottom of stack)
    uint8_t index;                   ///< index in screenStack array
} nbgl_screen_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_screen_reinit(void);
void nbgl_wait_pipeline(void);

#ifdef HAVE_DISPLAY_FAST_MODE
void nbgl_screen_update_temperature(uint8_t temp_degrees);
#endif  // HAVE_DISPLAY_FAST_MODE

#ifdef HAVE_CONFIGURABLE_DISPLAY_FAST_MODE
void nbgl_screen_config_fast_mode(uint8_t fast_mode_setting);
#endif  // HAVE_CONFIGURABLE_DISPLAY_FAST_MODE

void        nbgl_screenRedraw(void);
nbgl_obj_t *nbgl_screenGetAt(uint8_t screenIndex);
nbgl_obj_t *nbgl_screenGetTop(void);
uint8_t     nbgl_screenGetCurrentStackSize(void);
bool        nbgl_screenContainsObj(nbgl_obj_t *obj);
nbgl_obj_t *nbgl_screenContainsObjType(nbgl_screen_t *screen, nbgl_obj_type_t type);

#ifdef HAVE_SE_TOUCH
int nbgl_screenSet(nbgl_obj_t                           ***elements,
                   uint8_t                                 nbElements,
                   const nbgl_screenTickerConfiguration_t *ticker,
                   nbgl_touchCallback_t                    touchCallback);
#else   // HAVE_SE_TOUCH
int nbgl_screenSet(nbgl_obj_t                           ***elements,
                   uint8_t                                 nbElements,
                   const nbgl_screenTickerConfiguration_t *ticker,
                   nbgl_buttonCallback_t                   buttonCallback);
#endif  // HAVE_SE_TOUCH
int nbgl_screenUpdateNbElements(uint8_t screenIndex, uint8_t nbElements);
int nbgl_screenUpdateBackgroundColor(uint8_t screenIndex, color_t color);
int nbgl_screenUpdateTicker(uint8_t screenIndex, const nbgl_screenTickerConfiguration_t *ticker);
nbgl_obj_t **nbgl_screenGetElements(uint8_t screenIndex);
int          nbgl_screenRelease(void);
#ifdef HAVE_SE_TOUCH
int nbgl_screenPush(nbgl_obj_t                           ***elements,
                    uint8_t                                 nbElements,
                    const nbgl_screenTickerConfiguration_t *ticker,
                    nbgl_touchCallback_t                    touchCallback);
#else   // HAVE_SE_TOUCH
int nbgl_screenPush(nbgl_obj_t                           ***elements,
                    uint8_t                                 nbElements,
                    const nbgl_screenTickerConfiguration_t *ticker,
                    nbgl_buttonCallback_t                   buttonCallback);
#endif  // HAVE_SE_TOUCH
int  nbgl_screenPop(uint8_t screenIndex);
int  nbgl_screenReset(void);
void nbgl_screenHandler(uint32_t intervaleMs);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_SCREEN_H */
