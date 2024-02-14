/**
 * @file nbgl_flow.h
 * @brief Flow construction API of NBGL
 *
 */

#ifndef NBGL_FLOW_H
#define NBGL_FLOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "nbgl_step.h"
#include "nbgl_obj.h"
#include "nbgl_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/**
 * @brief type shared externally
 *
 */
typedef void *nbgl_flow_t;

/**
 * @brief prototype of function to be called when a step is using a callback on "double-key" action
 */
typedef void (*nbgl_stepCallback_t)(void);

/**
 * @brief Structure containing all specific information when creating a NBGL step.
 */
typedef struct nbgl_stepDesc_s {
    nbgl_stepCallback_t init;      ///< if not NULL, function to be called when the step is entered
    nbgl_stepCallback_t callback;  ///< if not NULL, function to be called on "double-key" action
    const char         *text;      ///< text to display in step (can be multi-pages if icon == NULL)
    const char         *subText;   ///< sub-text to display in step (NULL most of the time)
    const nbgl_icon_details_t *icon;  ///< icon to display in step (text must be single-page)
#ifdef HAVE_LANGUAGE_PACK
    UX_LOC_STRINGS_INDEX textId;  ///< text Id to display in step (if text field is NULL)
#endif                            // HAVE_LANGUAGE_PACK
} nbgl_stepDesc_t;

typedef nbgl_stepDesc_t nbgl_pageContent_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

nbgl_flow_t nbgl_flowDraw(const nbgl_stepDesc_t *steps,
                          uint8_t                nbSteps,
                          uint8_t                initStep,
                          bool                   loop,
                          bool                   modal);
void        nbgl_flowRelease(nbgl_flow_t flow);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_FLOW_H */
