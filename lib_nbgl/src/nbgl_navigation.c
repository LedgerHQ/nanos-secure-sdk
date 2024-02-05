
/**
 * @file nbgl_navigation.c
 * @brief The construction of a navigation bar with buttons super-object
 *
 */

#ifdef HAVE_SE_TOUCH

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_debug.h"
#include "nbgl_draw.h"
#include "nbgl_obj.h"
#include "glyphs.h"

/*********************
 *      DEFINES
 *********************/
#define INTERNAL_SMALL_MARGIN 8

#ifdef TARGET_STAX
#define BORDER_COLOR      LIGHT_GRAY
#define NAVIGATION_HEIGHT (BUTTON_DIAMETER + 2 * BORDER_MARGIN)
#else  // TARGET_STAX
#define BORDER_COLOR      WHITE
#define NAVIGATION_HEIGHT 100
#endif  // TARGET_STAX

/**********************
 *      TYPEDEFS
 **********************/
enum {
    EXIT_BUTTON_INDEX = 0,
    PREVIOUS_PAGE_INDEX,
    NEXT_PAGE_INDEX,
    NB_MAX_CHILDREN
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void configButtons(nbgl_container_t *navContainer, uint8_t navNbPages, uint8_t navActivePage)
{
    nbgl_button_t *buttonPrevious = (nbgl_button_t *) navContainer->children[PREVIOUS_PAGE_INDEX];
    nbgl_button_t *buttonNext     = (nbgl_button_t *) navContainer->children[NEXT_PAGE_INDEX];

    buttonPrevious->foregroundColor = (navActivePage == 0) ? LIGHT_GRAY : BLACK;
    buttonNext->foregroundColor     = (navActivePage == (navNbPages - 1)) ? LIGHT_GRAY : BLACK;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief function to be called when any of the controls in navigation bar is touched
 *
 * @param obj touched object (button)
 * @param eventType type of touch (only TOUCHED is accepted)
 * @param nbPages number of pages for navigation (if < 2, no navigation keys)
 * @param activePage current active page
 * @return true if actually navigated (page change)
 */
bool nbgl_navigationCallback(nbgl_obj_t      *obj,
                             nbgl_touchType_t eventType,
                             uint8_t          nbPages,
                             uint8_t         *activePage)
{
    nbgl_container_t *navContainer;

    if (eventType != TOUCHED) {
        return false;
    }
    navContainer = (nbgl_container_t *) obj->parent;

    if (obj == navContainer->children[EXIT_BUTTON_INDEX]) {
        // fake page when Quit button is touched
        *activePage = EXIT_PAGE;
        return true;
    }
    else if (obj == navContainer->children[PREVIOUS_PAGE_INDEX]) {
        if (*activePage > 0) {
            *activePage = *activePage - 1;
            configButtons(navContainer, nbPages, *activePage);
            return true;
        }
    }
    else if (obj == navContainer->children[NEXT_PAGE_INDEX]) {
        if (*activePage < (nbPages - 1)) {
            *activePage = *activePage + 1;
            configButtons(navContainer, nbPages, *activePage);
            return true;
        }
    }
    return false;
}

/**
 * @brief This function creates a full navigation bar "object", with buttons and returns it as a
 * container
 *
 * @param nbPages max number of pages for navigation (if < 2, no navigation keys)
 * @param activePage active page at start-up in [0-(nbPages-1)]
 * @param withExitKey if set to true, on page 0 the left key is replaced by a exit key
 * @param layer layer (screen) to create the navigation bar in
 *
 * @return the created navigation bar container object
 */
nbgl_container_t *nbgl_navigationPopulate(uint8_t nbPages,
                                          uint8_t activePage,
                                          bool    withExitKey,
                                          uint8_t layer)
{
    nbgl_button_t    *button;
    nbgl_container_t *navContainer;

    navContainer = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layer);
#ifdef TARGET_STAX
    navContainer->obj.area.width = SCREEN_WIDTH - 2 * BORDER_MARGIN;
#else   // TARGET_STAX
    navContainer->obj.area.width = SCREEN_WIDTH;
#endif  // TARGET_STAX
    navContainer->obj.area.height = NAVIGATION_HEIGHT;
    navContainer->layout          = HORIZONTAL;
    navContainer->nbChildren      = NB_MAX_CHILDREN;
    navContainer->children = (nbgl_obj_t **) nbgl_containerPoolGet(navContainer->nbChildren, layer);

    if (withExitKey) {
        button                  = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layer);
        button->innerColor      = WHITE;
        button->borderColor     = BORDER_COLOR;
        button->obj.area.width  = BUTTON_DIAMETER;
        button->obj.area.height = BUTTON_DIAMETER;
        button->radius          = BUTTON_RADIUS;
#ifdef TARGET_STAX
        button->icon = &C_cross32px;
#else   // TARGET_STAX
        button->icon                 = &C_cross40px;
        button->obj.alignmentMarginX = (nbPages > 1) ? 8 : 0;
#endif  // TARGET_STAX

        button->obj.alignment                     = (nbPages > 1) ? MID_LEFT : CENTER;
        button->obj.touchMask                     = (1 << TOUCHED);
        button->obj.touchId                       = BOTTOM_BUTTON_ID;
        navContainer->children[EXIT_BUTTON_INDEX] = (nbgl_obj_t *) button;
    }
    if (nbPages <= 1) {
        return navContainer;
    }
    // create previous page button
    button              = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layer);
    button->innerColor  = WHITE;
    button->borderColor = BORDER_COLOR;
#ifdef TARGET_STAX
    button->obj.area.width
        = (SCREEN_WIDTH - (2 * BORDER_MARGIN + 2 * INTERNAL_SMALL_MARGIN + BUTTON_DIAMETER)) / 2;
    if (!withExitKey) {
        button->obj.area.width += BUTTON_DIAMETER / 2;
    }
#else   // TARGET_STAX
    button->obj.area.width = BUTTON_DIAMETER;
#endif  // TARGET_STAX
    button->obj.area.height = BUTTON_DIAMETER;
    button->radius          = BUTTON_RADIUS;
#ifdef TARGET_STAX
    button->icon = &C_leftArrow32px;
    // align either on the right of Exit key, or on the inner left of the container
    if (withExitKey) {
        button->obj.alignmentMarginX = INTERNAL_SMALL_MARGIN;
        button->obj.alignment        = MID_RIGHT;
        button->obj.alignTo          = navContainer->children[EXIT_BUTTON_INDEX];
    }
    else {
        button->obj.alignment = MID_LEFT;
    }
#else   // TARGET_STAX
    button->icon           = &C_Back40px;
    // align on the right of the container, leaving space for "Next" button
    button->obj.alignment        = MID_RIGHT;
    button->obj.alignmentMarginX = 16 + BUTTON_DIAMETER + 8;
#endif  // TARGET_STAX
    button->obj.touchMask                       = (1 << TOUCHED);
    button->obj.touchId                         = LEFT_BUTTON_ID;
    navContainer->children[PREVIOUS_PAGE_INDEX] = (nbgl_obj_t *) button;

    // create next page button
    button                  = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layer);
    button->innerColor      = WHITE;
    button->borderColor     = BORDER_COLOR;
    button->foregroundColor = BLACK;
#ifdef TARGET_STAX
    button->obj.area.width
        = (SCREEN_WIDTH - (2 * BORDER_MARGIN + 2 * INTERNAL_SMALL_MARGIN + BUTTON_DIAMETER)) / 2;
    if (!withExitKey) {
        button->obj.area.width += BUTTON_DIAMETER / 2;
    }
#else   // TARGET_STAX
    button->obj.area.width       = BUTTON_DIAMETER;
#endif  // TARGET_STAX
    button->obj.area.height = BUTTON_DIAMETER;
    button->radius          = BUTTON_RADIUS;
#ifdef TARGET_STAX
    button->icon = &C_rightArrow32px;
    // on Stax, align next button on the right of left one
    button->obj.alignmentMarginX = INTERNAL_SMALL_MARGIN;
    button->obj.alignTo          = navContainer->children[PREVIOUS_PAGE_INDEX];
#else   // TARGET_STAX
    button->icon                 = &C_Next40px;
#endif  // TARGET_STAX
    button->obj.alignment                   = MID_RIGHT;
    button->obj.touchMask                   = (1 << TOUCHED);
    button->obj.touchId                     = RIGHT_BUTTON_ID;
    navContainer->children[NEXT_PAGE_INDEX] = (nbgl_obj_t *) button;

    // configure enabling/disabling of button
    configButtons(navContainer, nbPages, activePage);

    return navContainer;
}

#endif  // HAVE_SE_TOUCH
