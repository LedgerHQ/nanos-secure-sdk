/**
 * @file nbgl_debug.h
 * @brief debug traces management
 *
 */

#ifndef NBGL_DEBUG_H
#define NBGL_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

//#define WITH_STDIO 1

/*********************
 *      INCLUDES
 *********************/
#ifdef WITH_STDIO
#include <stdio.h>
#include <stdlib.h>
#endif

/*********************
 *      DEFINES
 *********************/
enum {
    LOW_LOGGER,
    DRAW_LOGGER,
    OBJ_LOGGER,
    OBJ_POOL_LOGGER,
    SCREEN_LOGGER,
    LAYOUT_LOGGER,
    PAGE_LOGGER,
    TOUCH_LOGGER,
    APP_LOGGER,
    UX_LOGGER,
    MISC_LOGGER
};
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern unsigned long gLogger;

/**********************
 *      MACROS
 **********************/
#ifdef WITH_STDIO
#define LOG_DEBUG(__logger,...) {\
   if (gLogger&(1<<__logger)) printf(__VA_ARGS__); \
}
#define LOG_WARN(__logger,...) printf(__VA_ARGS__)
#define LOG_FATAL(__logger,...) {printf(__VA_ARGS__);exit(-1);}

#else // WITH_STDIO
#ifdef NBGL_DEBUG
#include <os.h>
#define LOG_DEBUG(__logger,...)  do { PRINTF(__VA_ARGS__);  } while(0)
#define LOG_WARN(__logger,...) do { PRINTF(__VA_ARGS__); } while(0)
#define LOG_FATAL(__logger,...) do { PRINTF(__VA_ARGS__); halt(); } while(0)
#else
#define LOG_DEBUG(__logger,...)
#define LOG_WARN(__logger,...)
#define LOG_FATAL(__logger,...)
#endif // NBGL_DEBUG
#endif // WITH_STDIO



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_DEBUG_H */
