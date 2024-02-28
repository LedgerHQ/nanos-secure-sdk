
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
#include "os_print.h"
#include "os_helpers.h"
#include "glyphs.h"

/*********************
 *      DEFINES
 *********************/
#define INTERNAL_SMALL_MARGIN 8

#ifdef TARGET_STAX
#define BORDER_COLOR                  LIGHT_GRAY
#define NAVIGATION_HEIGHT             (BUTTON_DIAMETER + 2 * BORDER_MARGIN)
#define NAV_BUTTON_HEIGHT             BUTTON_DIAMETER
#define NAV_BUTTON_WIDTH              128
#define NAV_BUTTON_WIDTH_WITHOUT_EXIT 172
#else  // TARGET_STAX
#define BORDER_COLOR                  WHITE
#define NAVIGATION_HEIGHT             96
#define NAV_BUTTON_HEIGHT             NAVIGATION_HEIGHT
#define NAV_BUTTON_WIDTH              104
#define NAV_BUTTON_WIDTH_WITHOUT_EXIT NAV_BUTTON_WIDTH
#endif  // TARGET_STAX

/**********************
 *      TYPEDEFS
 **********************/
enum {
    EXIT_BUTTON_INDEX = 0,
    PREVIOUS_PAGE_INDEX,
    NEXT_PAGE_INDEX,
    PAGE_INDICATOR_INDEX,
    NB_MAX_CHILDREN
};

/**********************
 *  STATIC VARIABLES
 **********************/
#ifndef TARGET_STAX
static char navText[11];  // worst case is "ccc of nnn"
#endif                    // TARGET_STAX

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

    if (buttonPrevious) {
        buttonPrevious->foregroundColor = (navActivePage == 0) ? LIGHT_GRAY : BLACK;
    }
    if (navNbPages > 1) {
        buttonNext->foregroundColor = (navActivePage == (navNbPages - 1)) ? LIGHT_GRAY : BLACK;
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief function to be called when any of the controls in navigation bar is touched
 *
 * @param obj touched object (button or container)
 * @param eventType type of touch (only TOUCHED or SWIPED is accepted)
 * @param nbPages number of pages for navigation (if < 2, no navigation keys)
 * @param activePage current active page
 * @return true if actually navigated (page change)
 */
bool nbgl_navigationCallback(nbgl_obj_t      *obj,
                             nbgl_touchType_t eventType,
                             uint8_t          nbPages,
                             uint8_t         *activePage)
{
    // if direct touch of buttons within the navigation bar, the given obj is
    // the touched object
    if (eventType == TOUCHED) {
        nbgl_container_t *navContainer = (nbgl_container_t *) obj->parent;

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
            if ((nbPages < 2) || (*activePage < (nbPages - 1))) {
                *activePage = *activePage + 1;
                configButtons(navContainer, nbPages, *activePage);
                return true;
            }
        }
    }
    // otherwise the given object is the navigation container itself
    else if (eventType == SWIPED_RIGHT) {
        if (*activePage > 0) {
            *activePage = *activePage - 1;
            configButtons((nbgl_container_t *) obj, nbPages, *activePage);
            return true;
        }
    }
    else if (eventType == SWIPED_LEFT) {
        if ((nbPages < 2) || (*activePage < (nbPages - 1))) {
            *activePage = *activePage + 1;
            configButtons((nbgl_container_t *) obj, nbPages, *activePage);
            return true;
        }
    }
    return false;
}

/**
 * @brief This function creates a full navigation bar "object", with buttons and returns it as a
 * container
 *
 * @param navContainer container used for the objects of the navigation
 * @param nbPages max number of pages for navigation (if < 2, no navigation keys)
 * @param activePage active page at start-up in [0-(nbPages-1)]
 * @param withExitKey if set to true, an exit key is added on the left
 * @param withBackKey if set to false, the back key is not drawn
 * @param withPageIndicator if set to true, "<activePage+1> on <nbPages>" is added between
 * navigation arrows (if more than 1 page)
 * @param layer layer (screen) to create the navigation bar in
 *
 */
void nbgl_navigationPopulate(nbgl_container_t *navContainer,
                             uint8_t           nbPages,
                             uint8_t           activePage,
                             bool              withExitKey,
                             bool              withBackKey,
                             bool              withPageIndicator,
                             uint8_t           layer)
{
    nbgl_button_t *button;

#ifdef TARGET_STAX
    UNUSED(withPageIndicator);
#endif  // TARGET_STAX

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
    // create previous page button (back)
    if (withBackKey) {
        button              = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layer);
        button->innerColor  = WHITE;
        button->borderColor = BORDER_COLOR;
        if (withExitKey) {
            button->obj.area.width = NAV_BUTTON_WIDTH;
        }
        else {
            button->obj.area.width = NAV_BUTTON_WIDTH_WITHOUT_EXIT;
        }
        button->obj.area.height = NAV_BUTTON_HEIGHT;
        button->radius          = BUTTON_RADIUS;
#ifdef TARGET_STAX
        button->icon = &LEFT_ARROW_ICON;
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
        button->icon                 = &C_Back40px;
        // align on the right of the container, leaving space for "Next" button
        button->obj.alignment        = MID_RIGHT;
        button->obj.alignmentMarginX = NAV_BUTTON_WIDTH;
#endif  // TARGET_STAX
        button->obj.touchMask                       = (1 << TOUCHED);
        button->obj.touchId                         = LEFT_BUTTON_ID;
        navContainer->children[PREVIOUS_PAGE_INDEX] = (nbgl_obj_t *) button;
    }

    // create next page button
    button                  = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layer);
    button->innerColor      = WHITE;
    button->borderColor     = BORDER_COLOR;
    button->foregroundColor = BLACK;
    if (withExitKey) {
        button->obj.area.width = NAV_BUTTON_WIDTH;
    }
    else {
        button->obj.area.width = NAV_BUTTON_WIDTH_WITHOUT_EXIT;
    }
    button->obj.area.height = NAV_BUTTON_HEIGHT;
    button->radius          = BUTTON_RADIUS;
#ifdef TARGET_STAX
    button->icon = &RIGHT_ARROW_ICON;
    // on Stax, align next button on the right of left one
    button->obj.alignmentMarginX = INTERNAL_SMALL_MARGIN;
    button->obj.alignTo          = navContainer->children[PREVIOUS_PAGE_INDEX];
#else   // TARGET_STAX
    button->icon = &C_Next40px;
#endif  // TARGET_STAX
    button->obj.alignment                   = MID_RIGHT;
    button->obj.touchMask                   = (1 << TOUCHED);
    button->obj.touchId                     = RIGHT_BUTTON_ID;
    navContainer->children[NEXT_PAGE_INDEX] = (nbgl_obj_t *) button;

    // potentially create page indicator (with a text area)
#ifndef TARGET_STAX
    if (withPageIndicator && withBackKey && (nbPages > 1)) {
        nbgl_text_area_t *textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layer);

        SPRINTF(navText, "%d of %d", activePage + 1, nbPages);

        textArea->obj.alignment                      = BOTTOM_RIGHT;
        textArea->textColor                          = BLACK;
        textArea->obj.area.width                     = 109;
        textArea->text                               = navText;
        textArea->fontId                             = SMALL_REGULAR_FONT;
        textArea->obj.area.height                    = NAV_BUTTON_HEIGHT;
        textArea->textAlignment                      = CENTER;
        textArea->obj.alignment                      = MID_RIGHT;
        textArea->obj.alignmentMarginX               = NAV_BUTTON_WIDTH - 15;
        navContainer->children[PAGE_INDICATOR_INDEX] = (nbgl_obj_t *) textArea;
        if (withBackKey) {
            navContainer->children[PREVIOUS_PAGE_INDEX]->alignmentMarginX += 84;
        }
    }
#endif  // TARGET_STAX

    // configure enabling/disabling of button
    configButtons(navContainer, nbPages, activePage);

    return;
}

#endif  // HAVE_SE_TOUCH
