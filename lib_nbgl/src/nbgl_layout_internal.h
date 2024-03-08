/**
 * @file nbgl_layout_internal.h
 * @brief Internal functions/constants of NBGL layout layer
 *
 */

#ifndef NBGL_LAYOUT_INTERNAL_H
#define NBGL_LAYOUT_INTERNAL_H

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
// internal margin, between sub-items
#define INTERNAL_MARGIN 8

#define SMALL_BUTTON_HEIGHT 64

/**
 * @brief Max number of complex objects with callback retrievable from pool
 *
 */
#define LAYOUT_OBJ_POOL_LEN 10

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    nbgl_obj_t  *obj;
    uint8_t      token;   // user token, attached to callback
    uint8_t      index;   // index within the token
    tune_index_e tuneId;  // if not @ref NBGL_NO_TUNE, a tune will be played
} layoutObj_t;

typedef enum {
    SWIPE_USAGE_NAVIGATION,
    SWIPE_USAGE_CUSTOM,
    NB_SWIPE_USAGE
} nbgl_swipe_usage_t;

/**
 * @brief Structure containing all information about the current layout.
 * @note It shall not be used externally
 *
 */
typedef struct nbgl_layoutInternal_s {
    bool modal;           ///< if true, means the screen is a modal
    bool withLeftBorder;  ///< if true, draws a light gray left border on the whole height of the
                          ///< screen
    uint8_t layer;  ///< if >0, puts the layout on top of screen stack (modal). Otherwise puts on
                    ///< background (for apps)
    uint8_t      nbChildren;  ///< number of children in above array
    nbgl_obj_t **children;    ///< children for main screen

    uint8_t                 nbPages;     ///< number of pages for navigation bar
    uint8_t                 activePage;  ///< index of active page for navigation bar
    nbgl_layoutHeaderType_t headerType;  ///< type of header
    nbgl_layoutFooterType_t footerType;  ///< type of footer
    nbgl_container_t
        *headerContainer;  // container used to store header (progress, back, empty space...)
    nbgl_container_t *footerContainer;  // container used to store footer (buttons, nav....)
    nbgl_text_area_t *tapText;
    nbgl_layoutTouchCallback_t callback;  // user callback for all controls
    // This is the pool of callback objects, potentially used by this layout
    layoutObj_t callbackObjPool[LAYOUT_OBJ_POOL_LEN];
    // number of callback objects used by the whole layout in callbackObjPool
    uint8_t nbUsedCallbackObjs;

    nbgl_container_t  *container;
    nbgl_swipe_usage_t swipeUsage;
} nbgl_layoutInternal_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void         keyboardInit(void);
bool         keyboardSwipeCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType);
void         layoutAddObject(nbgl_layoutInternal_t *layout, nbgl_obj_t *obj);
layoutObj_t *layoutAddCallbackObj(nbgl_layoutInternal_t *layout,
                                  nbgl_obj_t            *obj,
                                  uint8_t                token,
                                  tune_index_e           tuneId);
void         layoutNavigationPopulate(nbgl_container_t *navContainer,
                                      uint8_t           nbPages,
                                      uint8_t           activePage,
                                      bool              withExitKey,
                                      bool              withBackKey,
                                      bool              withPageIndicator,
                                      uint8_t           layer);
bool         layoutNavigationCallback(nbgl_obj_t      *obj,
                                      nbgl_touchType_t eventType,
                                      uint8_t          nbPages,
                                      uint8_t         *activePage);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_LAYOUT_INTERNAL_H */
