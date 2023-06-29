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
  nbgl_tickerCallback_t tickerCallback; ///< callback called when ticker timer is fired. Set to NULL for no ticker
  uint32_t tickerValue; ///< timer initial value, in ms (should be multiple of 100 ms). Set to 0 for no ticker
  uint32_t tickerIntervale; ///< for periodic timers, the intervale in ms to rearm the timer (should be multiple of 100 ms). Set to 0 for one-shot timers
} nbgl_screenTickerConfiguration_t;

/**
 * @brief struct to represent a screen (@ref SCREEN type)
 * @note Only for internal usage
 * @note inherits from container
 *
 */
typedef struct PACKED__ nbgl_screen_s {
    nbgl_container_t container; ///< common part
    nbgl_screenTickerConfiguration_t ticker; ///< ticker configuration
    nbgl_touchCallback_t touchCallback; ///< function to be called on events defined in touchMask field in each sub-object
    struct nbgl_screen_s *next; ///< pointer to screen on top of this one (or NULL is this screen is top of stack)
    struct nbgl_screen_s *previous; ///< pointer to screen on bottom of this one (or NULL is this screen is bottom of stack)
} nbgl_screen_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
unsigned int nbgl_screen_reinit(void);
void nbgl_screenRedraw(void);
nbgl_obj_t *nbgl_screenGetTop(void);
uint8_t nbgl_screenGetCurrentStackSize(void);
bool nbgl_screenContainsObj(nbgl_obj_t *obj);

int nbgl_screenSet(nbgl_obj_t*** elements, uint8_t nbElements, const nbgl_screenTickerConfiguration_t *ticker, nbgl_touchCallback_t touchCallback);
int nbgl_screenUpdateNbElements(uint8_t screenIndex, uint8_t nbElements);
int nbgl_screenUpdateBackgroundColor(uint8_t screenIndex, color_t color);
int nbgl_screenUpdateTicker(uint8_t screenIndex, const nbgl_screenTickerConfiguration_t *ticker);
nbgl_obj_t **nbgl_screenGetElements(uint8_t screenIndex);
int nbgl_screenRelease(void);
int nbgl_screenPush(nbgl_obj_t*** elements, uint8_t nbElements, const nbgl_screenTickerConfiguration_t *ticker, nbgl_touchCallback_t touchCallback);
int nbgl_screenPop(uint8_t screenIndex);
int nbgl_screenReset(void);
void nbgl_screenHandler(uint32_t intervaleMs);


/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_SCREEN_H */
