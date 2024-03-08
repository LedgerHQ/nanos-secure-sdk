/**
 * @file nbgl_layout_internal_nanos.h
 * @brief Internal functions/constants of NBGL layout layer
 *
 */

#ifndef NBGL_LAYOUT_INTERNAL_NANOS_H
#define NBGL_LAYOUT_INTERNAL_NANOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nbgl_layout.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
/**
 * @brief Max number of complex objects with callback retrievable from pool
 *
 */
#define LAYOUT_OBJ_POOL_LEN 10

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief Structure containing all information about the current layout.
 * @note It shall not be used externally
 *
 */
typedef struct nbgl_layoutInternal_s {
    bool    modal;  ///< if true, means the screen is a modal
    uint8_t layer;  ///< if >0, puts the layout on top of screen stack (modal). Otherwise puts on
                    ///< background (for apps)
    uint8_t      nbChildren;  ///< number of children in above array
    nbgl_obj_t **children;    ///< children for main screen

    nbgl_layoutButtonCallback_t callback;  // user callback for all controls
} nbgl_layoutInternal_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void layoutAddObject(nbgl_layoutInternal_t *layout, nbgl_obj_t *obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_LAYOUT_INTERNAL_NANOS_H */
