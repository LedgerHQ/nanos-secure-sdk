/**
 * @file nbgl_layout.c
 * @brief Implementation of predefined layouts management for Applications
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdlib.h>
#include "nbgl_debug.h"
#include "nbgl_front.h"
#include "nbgl_layout.h"
#include "nbgl_obj.h"
#include "nbgl_draw.h"
#include "nbgl_screen.h"
#include "nbgl_touch.h"
#include "glyphs.h"
#include "os_pic.h"
#include "os_helpers.h"
#include "lcx_rng.h"

/*********************
 *      DEFINES
 *********************/
// half internal margin, between items
#define INTERNAL_SPACE  16
// internal margin, between sub-items
#define INTERNAL_MARGIN 8
// inner margin, between buttons
#define INNER_MARGIN    12

#define NB_MAX_LAYOUTS 3

// used by container
#ifdef HAVE_SE_TOUCH
#define NB_MAX_CONTAINER_CHILDREN 20
#endif  // HAVE_SE_TOUCH

// used by screen
#define NB_MAX_SCREEN_CHILDREN 7

/**
 * @brief Max number of complex objects with callback retrievable from pool
 *
 */
#ifdef HAVE_SE_TOUCH
#define LAYOUT_OBJ_POOL_LEN 10

#define TAG_VALUE_ICON_WIDTH 32

#ifdef TARGET_STAX
#define RADIO_CHOICE_HEIGHT 88
#define FOOTER_HEIGHT       80
#define BAR_INTERVALE       12
#define BACK_KEY_WIDTH      88
#else  // TARGET_STAX
#define RADIO_CHOICE_HEIGHT 92
#define FOOTER_HEIGHT       80
#define BAR_INTERVALE       16
#define BACK_KEY_WIDTH      104
#endif  // TARGET_STAX

#define SMALL_BUTTON_HEIGHT 64
#endif  // HAVE_SE_TOUCH

// refresh period of the spinner, in ms
#define SPINNER_REFRESH_PERIOD 400

#ifdef HAVE_SE_TOUCH
#ifdef TARGET_STAX
#define FIRST_BUTTON_INDEX 0
#else   // TARGET_STAX
// for suggestion buttons, on Europa there are other objects than buttons
enum {
    PAGE_INDICATOR_INDEX = 0,
    LEFT_HALF_INDEX,   // half disc displayed on the bottom left
    RIGHT_HALF_INDEX,  // half disc displayed on the bottom right
    FIRST_BUTTON_INDEX
};
#endif  // TARGET_STAX
#endif  // HAVE_SE_TOUCH

/**********************
 *      MACROS
 **********************/
#define ASSERT_ENOUGH_PLACE_FOR_MAIN_PANEL_CHILD(__layout)                    \
    {                                                                         \
        if ((__layout)->panel.nbChildren == (NB_MAX_MAIN_PANEL_CHILDREN - 1)) \
            return NO_MORE_OBJ_ERROR;                                         \
    }

/**********************
 *      TYPEDEFS
 **********************/

#ifdef HAVE_SE_TOUCH
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
#endif  // HAVE_SE_TOUCH

/**
 * @brief Structure containing all information about the current layout.
 * @note It shall not be used externally
 *
 */
typedef struct nbgl_layoutInternal_s {
    bool modal;  ///< if true, means the screen is a modal
#ifdef HAVE_SE_TOUCH
    bool withLeftBorder;  ///< if true, draws a light gray left border on the whole height of the
                          ///< screen
#endif                    // HAVE_SE_TOUCH
    uint8_t layer;  ///< if >0, puts the layout on top of screen stack (modal). Otherwise puts on
                    ///< background (for apps)
    uint8_t      nbChildren;  ///< number of children in above array
    nbgl_obj_t **children;    ///< children for main screen

#ifdef HAVE_SE_TOUCH
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
#else   // HAVE_SE_TOUCH
    nbgl_layoutButtonCallback_t callback;  // user callback for all controls
#endif  // HAVE_SE_TOUCH

} nbgl_layoutInternal_t;

/**********************
 *      VARIABLES
 **********************/

/**
 * @brief array of layouts, if used by modal
 *
 */
static nbgl_layoutInternal_t gLayout[NB_MAX_LAYOUTS] = {0};

#ifdef HAVE_SE_TOUCH
#ifdef NBGL_KEYBOARD
static nbgl_button_t *choiceButtons[NB_MAX_SUGGESTION_BUTTONS];
static char           numText[5];
static uint8_t        nbActiveButtons;
#ifndef TARGET_STAX
static nbgl_image_t *partialButtonImages[2];
#endif  // TARGET_STAX
#endif  // NBGL_KEYBOARD

// numbers of touchable controls for the whole page
static uint8_t nbTouchableControls = 0;
#endif  // HAVE_SE_TOUCH

/**********************
 *  STATIC PROTOTYPES
 **********************/

#ifdef HAVE_SE_TOUCH
#ifdef HAVE_DISPLAY_FAST_MODE
// Unit step in % of touchable progress bar
#define HOLD_TO_APPROVE_STEP_PERCENT     (10)
// Duration in ms the user must hold the progress bar
// to make it progress HOLD_TO_APPROVE_STEP_PERCENT %.
// This duration must be higher than the screen refresh duration.
#define HOLD_TO_APPROVE_STEP_DURATION_MS (150)
#else
#define HOLD_TO_APPROVE_STEP_PERCENT     (17)
#define HOLD_TO_APPROVE_STEP_DURATION_MS (400)
#endif  // HAVE_DISPLAY_FAST_MODE

static inline uint8_t get_hold_to_approve_percent(uint32_t touch_duration)
{
    uint8_t current_step_nb = (touch_duration / HOLD_TO_APPROVE_STEP_DURATION_MS) + 1;
    return (current_step_nb * HOLD_TO_APPROVE_STEP_PERCENT);
}

#if (!defined(TARGET_STAX) && defined(NBGL_KEYBOARD))
// function used on Europa to display (or not) beginning of next button and/or end of
// previous button, and update buttons when swipping
static bool updateSuggestionButtons(nbgl_container_t *container,
                                    nbgl_touchType_t  eventType,
                                    uint8_t           currentLeftButtonIndex)
{
    bool    needRefresh = false;
    uint8_t page        = 0;
    if ((eventType == SWIPED_LEFT) && (currentLeftButtonIndex < (uint32_t) (nbActiveButtons - 2))) {
        // shift all buttons on the left if there are still at least 2 buttons to display
        currentLeftButtonIndex += 2;
        container->children[FIRST_BUTTON_INDEX]
            = (nbgl_obj_t *) choiceButtons[currentLeftButtonIndex];
        if (currentLeftButtonIndex < (uint32_t) (nbActiveButtons - 1)) {
            container->children[FIRST_BUTTON_INDEX + 1]
                = (nbgl_obj_t *) choiceButtons[currentLeftButtonIndex + 1];
        }
        else {
            container->children[FIRST_BUTTON_INDEX + 1] = NULL;
        }
        page        = currentLeftButtonIndex / 2;
        needRefresh = true;
    }
    else if ((eventType == SWIPED_RIGHT) && (currentLeftButtonIndex > 1)) {
        // shift all buttons on the left if we are not already displaying the 2 first ones
        currentLeftButtonIndex -= 2;
        container->children[FIRST_BUTTON_INDEX]
            = (nbgl_obj_t *) choiceButtons[currentLeftButtonIndex];
        container->children[FIRST_BUTTON_INDEX + 1]
            = (nbgl_obj_t *) choiceButtons[currentLeftButtonIndex + 1];
        page        = currentLeftButtonIndex / 2;
        needRefresh = true;
    }
    // align left button on the left
    if (nbActiveButtons > 0) {
        container->children[FIRST_BUTTON_INDEX]->alignmentMarginX = BORDER_MARGIN;
        container->children[FIRST_BUTTON_INDEX]->alignment        = TOP_LEFT;
        container->children[FIRST_BUTTON_INDEX]->alignTo          = (nbgl_obj_t *) container;
    }

    // align right button on left one
    if (container->children[FIRST_BUTTON_INDEX + 1] != NULL) {
        container->children[FIRST_BUTTON_INDEX + 1]->alignmentMarginX = INTERNAL_MARGIN;
        container->children[FIRST_BUTTON_INDEX + 1]->alignment        = MID_RIGHT;
        container->children[FIRST_BUTTON_INDEX + 1]->alignTo
            = container->children[FIRST_BUTTON_INDEX];
    }

    // on Europa, the first child is used by the progress indicator, displayed if more that 2
    // buttons
    nbgl_page_indicator_t *indicator
        = (nbgl_page_indicator_t *) container->children[PAGE_INDICATOR_INDEX];
    indicator->activePage = page;

    // if not on the first button, display end of previous button
    if (currentLeftButtonIndex > 0) {
        container->children[LEFT_HALF_INDEX] = (nbgl_obj_t *) partialButtonImages[0];
    }
    else {
        container->children[LEFT_HALF_INDEX] = NULL;
    }
    // if not on the last button, display beginning of next button
    if (currentLeftButtonIndex < (nbActiveButtons - 2)) {
        container->children[RIGHT_HALF_INDEX] = (nbgl_obj_t *) partialButtonImages[1];
    }
    else {
        container->children[RIGHT_HALF_INDEX] = NULL;
    }
    return needRefresh;
}
#endif  // TARGET_STAX

// function used to retrieve the concerned layout and layout obj matching the given touched obj
static bool getLayoutAndLayoutObj(nbgl_obj_t             *obj,
                                  nbgl_layoutInternal_t **layout,
                                  layoutObj_t           **layoutObj)
{
    uint8_t i = NB_MAX_LAYOUTS;

    // parse all layouts (starting with modals) to find the object
    *layout = NULL;
    while (i > 0) {
        i--;
        if (gLayout[i].nbChildren > 0) {
            uint8_t j;

            // search index of obj in this layout
            for (j = 0; j < gLayout[i].nbUsedCallbackObjs; j++) {
                if (obj == gLayout[i].callbackObjPool[j].obj) {
                    LOG_DEBUG(LAYOUT_LOGGER,
                              "getLayoutAndLayoutObj(): obj found in layout[%d], index = %d, "
                              "nbUsedCallbackObjs = %d\n",
                              i,
                              j,
                              gLayout[i].nbUsedCallbackObjs);
                    *layout    = &gLayout[i];
                    *layoutObj = &(gLayout[i].callbackObjPool[j]);
                    return true;
                }
            }
        }
    }
    // not found
    return false;
}

static void radioTouchCallback(nbgl_obj_t            *obj,
                               nbgl_touchType_t       eventType,
                               nbgl_layoutInternal_t *layout);
static void longTouchCallback(nbgl_obj_t            *obj,
                              nbgl_touchType_t       eventType,
                              nbgl_layoutInternal_t *layout,
                              layoutObj_t           *layoutObj);

// callback for most touched object
static void touchCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType)
{
    nbgl_layoutInternal_t *layout;
    layoutObj_t           *layoutObj;
    bool                   needRefresh = false;

    if (obj == NULL) {
        return;
    }
    LOG_DEBUG(LAYOUT_LOGGER, "touchCallback(): eventType = %d, obj = %p\n", eventType, obj);
    if (getLayoutAndLayoutObj(obj, &layout, &layoutObj) == false) {
        // try with parent, if existing
        if (getLayoutAndLayoutObj(obj->parent, &layout, &layoutObj) == false) {
            LOG_WARN(
                LAYOUT_LOGGER,
                "touchCallback(): eventType = %d, obj = %p, no active layout or obj not found\n",
                eventType,
                obj);
            return;
        }
    }

    // case of swipe
    if (((eventType == SWIPED_UP) || (eventType == SWIPED_DOWN) || (eventType == SWIPED_LEFT)
         || (eventType == SWIPED_RIGHT))
        && (obj->type == CONTAINER)) {
#if (!defined(TARGET_STAX) && defined(NBGL_KEYBOARD))
        // try if suggestions buttons
        nbgl_container_t *container = (nbgl_container_t *) obj;
        if (((eventType == SWIPED_LEFT) || (eventType == SWIPED_RIGHT))
            && (container->nbChildren == (nbActiveButtons + FIRST_BUTTON_INDEX))
            && (nbActiveButtons > 2)) {
            uint32_t i = 0;
            while (i < (uint32_t) nbActiveButtons) {
                if (container->children[FIRST_BUTTON_INDEX] == (nbgl_obj_t *) choiceButtons[i]) {
                    break;
                }
                i++;
            }

            if (i < (uint32_t) nbActiveButtons) {
                if (updateSuggestionButtons(container, eventType, i)) {
                    nbgl_redrawObject((nbgl_obj_t *) container, NULL, false);
                    nbgl_refresh();
                }

                return;
            }
        }
#endif  // TARGET_STAX
        if (layout->swipeUsage == SWIPE_USAGE_CUSTOM) {
            layoutObj->index = eventType;
        }
        else if (layout->swipeUsage == SWIPE_USAGE_NAVIGATION) {
            nbgl_container_t *navContainer;
            if (layout->footerType == FOOTER_NAV) {
                navContainer = (nbgl_container_t *) layout->footerContainer;
            }
            else if (layout->footerType == FOOTER_TEXT_AND_NAV) {
                navContainer = (nbgl_container_t *) layout->footerContainer->children[1];
            }
            else {
                return;
            }

            if (nbgl_navigationCallback(
                    (nbgl_obj_t *) navContainer, eventType, layout->nbPages, &layout->activePage)
                == false) {
                // navigation was impossible
                return;
            }
            layoutObj->index = layout->activePage;
        }
    }

    // case of navigation bar
    if (((obj->parent == (nbgl_obj_t *) layout->footerContainer)
         && (layout->footerType == FOOTER_NAV))
        || ((obj->parent->type == CONTAINER)
            && (obj->parent->parent == (nbgl_obj_t *) layout->footerContainer)
            && (layout->footerType == FOOTER_TEXT_AND_NAV))) {
        if (nbgl_navigationCallback(obj, eventType, layout->nbPages, &layout->activePage)
            == false) {
            // navigation was impossible
            return;
        }
        layoutObj->index = layout->activePage;
    }

    // case of switch
    if ((obj->type == CONTAINER) && (((nbgl_container_t *) obj)->nbChildren >= 2)
        && (((nbgl_container_t *) obj)->children[1] != NULL)
        && (((nbgl_container_t *) obj)->children[1]->type == SWITCH)) {
        nbgl_switch_t *lSwitch = (nbgl_switch_t *) ((nbgl_container_t *) obj)->children[1];
        lSwitch->state         = (lSwitch->state == ON_STATE) ? OFF_STATE : ON_STATE;
        nbgl_redrawObject((nbgl_obj_t *) lSwitch, false, false);
        // refresh will be done after tune playback
        needRefresh = true;
        // index is used for state
        layoutObj->index = lSwitch->state;
    }
    // case of radio
    else if ((obj->type == CONTAINER) && (((nbgl_container_t *) obj)->nbChildren == 2)
             && (((nbgl_container_t *) obj)->children[1] != NULL)
             && (((nbgl_container_t *) obj)->children[1]->type == RADIO_BUTTON)) {
        radioTouchCallback(obj, eventType, layout);
        return;
    }
    // case of long press
    else if ((obj->type == CONTAINER) && (((nbgl_container_t *) obj)->nbChildren == 4)
             && (((nbgl_container_t *) obj)->children[3] != NULL)
             && (((nbgl_container_t *) obj)->children[3]->type == PROGRESS_BAR)) {
        longTouchCallback(obj, eventType, layout, layoutObj);
        return;
    }
    LOG_DEBUG(LAYOUT_LOGGER, "touchCallback(): layout->callback = %p\n", layout->callback);
    if (layout->callback != NULL) {
#ifdef HAVE_PIEZO_SOUND
        if (layoutObj->tuneId < NBGL_NO_TUNE) {
            io_seproxyhal_play_tune(layoutObj->tuneId);
        }
#endif  // HAVE_PIEZO_SOUND
        if (needRefresh) {
            nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
        }
        layout->callback(layoutObj->token, layoutObj->index);
    }
}

// callback for long press button
static void longTouchCallback(nbgl_obj_t            *obj,
                              nbgl_touchType_t       eventType,
                              nbgl_layoutInternal_t *layout,
                              layoutObj_t           *layoutObj)
{
    nbgl_container_t *container = (nbgl_container_t *) obj;
    // 4th child of container is the progress bar
    nbgl_progress_bar_t *progressBar = (nbgl_progress_bar_t *) container->children[3];

    LOG_DEBUG(LAYOUT_LOGGER,
              "longTouchCallback(): eventType = %d, obj = %p, gLayout[1].nbChildren = %d\n",
              eventType,
              obj,
              gLayout[1].nbChildren);

    // case of pressing a long press button
    if (eventType == TOUCHING) {
        uint32_t touchDuration = nbgl_touchGetTouchDuration(obj);

        // Compute the new progress bar state in %
        uint8_t new_state = get_hold_to_approve_percent(touchDuration);

        // Ensure the callback is triggered once,
        // when the progress bar state reaches 100%
        bool trigger_callback = (new_state >= 100) && (progressBar->state < 100);

        // Cap progress bar state at 100%
        if (new_state >= 100) {
            new_state = 100;
        }

        // Update progress bar state
        if (new_state != progressBar->state) {
            progressBar->state = new_state;
            nbgl_redrawObject((nbgl_obj_t *) progressBar, false, false);
            // Ensure progress bar is fully drawn
            // before calling the callback.
            nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_FAST_REFRESH,
                                               POST_REFRESH_FORCE_POWER_ON);
        }

        if (trigger_callback) {
            // End of progress bar reached: trigger callback
            if (layout->callback != NULL) {
                layout->callback(layoutObj->token, layoutObj->index);
            }
        }
    }
    // case of releasing a long press button (or getting out of it)
    else if ((eventType == TOUCH_RELEASED) || (eventType == OUT_OF_TOUCH)) {
        progressBar->state = 0;
        nbgl_redrawObject((nbgl_obj_t *) progressBar, false, false);
        nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_REFRESH, POST_REFRESH_FORCE_POWER_OFF);
    }
}

// callback for radio button touch
static void radioTouchCallback(nbgl_obj_t            *obj,
                               nbgl_touchType_t       eventType,
                               nbgl_layoutInternal_t *layout)
{
    uint8_t i = NB_MAX_LAYOUTS, radioIndex = 0, foundRadio = 0xFF, foundRadioIndex;

    if (eventType != TOUCHED) {
        return;
    }

    i = 0;
    // parse all objs to find all containers of radio buttons
    while (i < layout->nbUsedCallbackObjs) {
        if ((obj == (nbgl_obj_t *) layout->callbackObjPool[i].obj)
            && (layout->callbackObjPool[i].obj->type == CONTAINER)) {
            nbgl_radio_t *radio
                = (nbgl_radio_t *) ((nbgl_container_t *) layout->callbackObjPool[i].obj)
                      ->children[1];
            nbgl_text_area_t *textArea
                = (nbgl_text_area_t *) ((nbgl_container_t *) layout->callbackObjPool[i].obj)
                      ->children[0];
            foundRadio      = i;
            foundRadioIndex = radioIndex;
            // set text as active (black and bold)
            textArea->textColor = BLACK;
            textArea->fontId    = SMALL_BOLD_FONT;
            // ensure that radio button is ON
            radio->state = ON_STATE;
            // redraw container
            nbgl_redrawObject((nbgl_obj_t *) obj, NULL, false);
        }
        else if ((layout->callbackObjPool[i].obj->type == CONTAINER)
                 && (((nbgl_container_t *) layout->callbackObjPool[i].obj)->nbChildren == 2)
                 && (((nbgl_container_t *) layout->callbackObjPool[i].obj)->children[1]->type
                     == RADIO_BUTTON)) {
            nbgl_radio_t *radio
                = (nbgl_radio_t *) ((nbgl_container_t *) layout->callbackObjPool[i].obj)
                      ->children[1];
            nbgl_text_area_t *textArea
                = (nbgl_text_area_t *) ((nbgl_container_t *) layout->callbackObjPool[i].obj)
                      ->children[0];
            radioIndex++;
            // set to OFF the one that was in ON
            if (radio->state == ON_STATE) {
                radio->state = OFF_STATE;
                // set text it as inactive (gray and normal)
                textArea->textColor = DARK_GRAY;
                textArea->fontId    = SMALL_REGULAR_FONT;
                // redraw container
                nbgl_redrawObject((nbgl_obj_t *) layout->callbackObjPool[i].obj, NULL, false);
            }
        }
        i++;
    }
    // call callback after redraw to avoid asynchronicity
    if (foundRadio != 0xFF) {
        if (layout->callback != NULL) {
#ifdef HAVE_PIEZO_SOUND
            if (layout->callbackObjPool[foundRadio].tuneId < NBGL_NO_TUNE) {
                io_seproxyhal_play_tune(layout->callbackObjPool[foundRadio].tuneId);
            }
            nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
#endif  // HAVE_PIEZO_SOUND
            layout->callback(layout->callbackObjPool[foundRadio].token, foundRadioIndex);
        }
    }
}
#else  // HAVE_SE_TOUCH
static void buttonCallback(nbgl_screen_t *screen, nbgl_buttonEvent_t buttonEvent)
{
    uint8_t                i      = NB_MAX_LAYOUTS;
    nbgl_layoutInternal_t *layout = NULL;

    // parse all layouts (starting with modals) to find the object
    while (i > 0) {
        i--;
        if ((screen->index == gLayout[i].layer) && (gLayout[i].nbChildren > 0)) {
            // found
            layout = &gLayout[i];
            break;
        }
    }
    if (layout == NULL) {
        LOG_WARN(
            LAYOUT_LOGGER,
            "touchCallback(): screen->index = %d, buttonEvent = %d, no matching active layout\n",
            screen->index,
            buttonEvent);
        return;
    }

#ifdef NBGL_KEYPAD
    // special case of keypad
    nbgl_obj_t *kpd = nbgl_screenContainsObjType(screen, KEYPAD);
    if (kpd) {
        nbgl_keypadCallback(kpd, buttonEvent);
        return;
    }
    else
#endif  // NBGL_KEYPAD
#ifdef NBGL_KEYBOARD
    {
        nbgl_obj_t *kbd = nbgl_screenContainsObjType(screen, KEYBOARD);
        if (kbd) {
            nbgl_keyboardCallback(kbd, buttonEvent);
            return;
        }
    }
#endif  // NBGL_KEYBOARD
    if (layout->callback != NULL) {
        layout->callback((nbgl_layout_t *) layout, buttonEvent);
    }
}
#endif  // HAVE_SE_TOUCH

#ifdef HAVE_SE_TOUCH

// callback for spinner ticker
static void spinnerTickerCallback(void)
{
    nbgl_spinner_t        *spinner;
    uint8_t                i = 0;
    nbgl_layoutInternal_t *layout;

    // gLayout[1] is on top of gLayout[0] so if gLayout[1] is active, it must catch the event
    if (gLayout[1].nbChildren > 0) {
        layout = &gLayout[1];
    }
    else {
        layout = &gLayout[0];
    }

    // get index of obj
    while (i < layout->container->nbChildren) {
        if (layout->container->children[i]->type == SPINNER) {
            spinner = (nbgl_spinner_t *) layout->container->children[i];
            spinner->position++;
            spinner->position &= 3;  // modulo 4
            nbgl_redrawObject((nbgl_obj_t *) spinner, NULL, false);
            nbgl_refreshSpecial(BLACK_AND_WHITE_FAST_REFRESH);
            return;
        }
        i++;
    }
}

static nbgl_line_t *createHorizontalLine(uint8_t layer)
{
    nbgl_line_t *line;

    line                  = (nbgl_line_t *) nbgl_objPoolGet(LINE, layer);
    line->lineColor       = LIGHT_GRAY;
    line->obj.area.width  = SCREEN_WIDTH;
    line->obj.area.height = 4;
    line->direction       = HORIZONTAL;
    line->thickness       = 1;
    return line;
}

static nbgl_line_t *createLeftVerticalLine(uint8_t layer)
{
    nbgl_line_t *line;

    line                  = (nbgl_line_t *) nbgl_objPoolGet(LINE, layer);
    line->lineColor       = LIGHT_GRAY;
    line->obj.area.width  = 1;
    line->obj.area.height = SCREEN_HEIGHT;
    line->direction       = VERTICAL;
    line->thickness       = 1;
    line->obj.alignment   = MID_LEFT;
    return line;
}

// function adding a layout object in the callbackObjPool array for the given layout, and
// configuring it
static layoutObj_t *addCallbackObj(nbgl_layoutInternal_t *layout,
                                   nbgl_obj_t            *obj,
                                   uint8_t                token,
                                   tune_index_e           tuneId)
{
    layoutObj_t *layoutObj = NULL;

    if (layout->nbUsedCallbackObjs < (LAYOUT_OBJ_POOL_LEN - 1)) {
        layoutObj = &layout->callbackObjPool[layout->nbUsedCallbackObjs];
        layout->nbUsedCallbackObjs++;
        layoutObj->obj    = obj;
        layoutObj->token  = token;
        layoutObj->tuneId = tuneId;
    }

    return layoutObj;
}

/**
 * @brief adds the given obj to the main container
 *
 * @param layout
 * @param obj
 */
static void addObjectToLayout(nbgl_layoutInternal_t *layout, nbgl_obj_t *obj)
{
    if (layout->container->nbChildren == NB_MAX_CONTAINER_CHILDREN) {
        LOG_FATAL(LAYOUT_LOGGER, "addObjectToLayout(): No more object\n");
    }
    layout->container->children[layout->container->nbChildren] = obj;
    layout->container->nbChildren++;
}

/**
 * @brief add the swipe feature to the main container
 *
 * @param layout the current layout
 * @param token the token that will be used as argument of the callback
 * @param swipesMask the type of swipes to be handled by the container
 * @param usage usage of the swipe (custom or navigation)
 * @return >= 0 if OK
 */
static int addSwipeInternal(nbgl_layoutInternal_t *layoutInt,
                            uint16_t               swipesMask,
                            nbgl_swipe_usage_t     usage,
                            uint8_t                token,
                            tune_index_e           tuneId)
{
    layoutObj_t *obj;

    if ((swipesMask & SWIPE_MASK) == 0) {
        return -1;
    }

    obj = addCallbackObj(layoutInt, (nbgl_obj_t *) layoutInt->container, token, tuneId);
    if (obj == NULL) {
        return -1;
    }
    layoutInt->container->obj.touchMask = swipesMask;
    layoutInt->swipeUsage               = usage;

    return 0;
}

#else   // HAVE_SE_TOUCH

/**
 * @brief adds the given obj to the layout
 *
 * @param layout
 * @param obj
 */
static void addObjectToLayout(nbgl_layoutInternal_t *layout, nbgl_obj_t *obj)
{
    if (layout->nbChildren == NB_MAX_SCREEN_CHILDREN) {
        LOG_FATAL(LAYOUT_LOGGER, "addObjectToLayout(): No more object\n");
    }
    layout->children[layout->nbChildren] = obj;
    layout->nbChildren++;
}
#endif  // HAVE_SE_TOUCH

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#ifdef HAVE_SE_TOUCH
/**
 * @brief returns a layout of the given type. The layout is reset
 *
 * @param description description of layout
 * @return a pointer to the corresponding layout
 */
nbgl_layout_t *nbgl_layoutGet(const nbgl_layoutDescription_t *description)
{
    nbgl_layoutInternal_t *layout = NULL;

    // find an empty layout in the proper "layer"
    if (description->modal) {
        if (gLayout[1].nbChildren == 0) {
            layout = &gLayout[1];
        }
        else if (gLayout[2].nbChildren == 0) {
            layout = &gLayout[2];
        }
    }
    else {
        // automatically "release" a potentially opened non-modal layout
        gLayout[0].nbChildren = 0;
        layout                = &gLayout[0];
    }
    if (layout == NULL) {
        LOG_WARN(LAYOUT_LOGGER, "nbgl_layoutGet(): impossible to get a layout!\n");
        return NULL;
    }

    // reset globals
    memset(layout, 0, sizeof(nbgl_layoutInternal_t));

    nbTouchableControls = 0;
#ifdef NBGL_KEYBOARD
    nbActiveButtons = 0;
#endif  // NBGL_KEYBOARD

    layout->callback       = (nbgl_layoutTouchCallback_t) PIC(description->onActionCallback);
    layout->modal          = description->modal;
    layout->withLeftBorder = description->withLeftBorder;
    if (description->modal) {
        layout->layer = nbgl_screenPush(&layout->children,
                                        NB_MAX_SCREEN_CHILDREN,
                                        &description->ticker,
                                        (nbgl_touchCallback_t) touchCallback);
    }
    else {
        nbgl_screenSet(&layout->children,
                       NB_MAX_SCREEN_CHILDREN,
                       &description->ticker,
                       (nbgl_touchCallback_t) touchCallback);
        layout->layer = 0;
    }
    layout->container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layout->layer);
    layout->container->obj.area.width  = SCREEN_WIDTH;
    layout->container->obj.area.height = SCREEN_HEIGHT;
    layout->container->layout          = VERTICAL;
    layout->container->children = nbgl_containerPoolGet(NB_MAX_CONTAINER_CHILDREN, layout->layer);
    // main container is always the second object, leaving space for header
    layout->children[1] = (nbgl_obj_t *) layout->container;
    layout->nbChildren  = 2;

    // if a tap text is defined, make the container tapable and display this text in gray
    if (description->tapActionText != NULL) {
        layoutObj_t *obj;

        obj = &layout->callbackObjPool[layout->nbUsedCallbackObjs];
        layout->nbUsedCallbackObjs++;
        obj->obj                         = (nbgl_obj_t *) layout->container;
        obj->token                       = description->tapActionToken;
        obj->tuneId                      = description->tapTuneId;
        layout->container->obj.touchMask = (1 << TOUCHED);
        layout->container->obj.touchId   = WHOLE_SCREEN_ID;

        // create 'tap to continue' text area
        layout->tapText                  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, 0);
        layout->tapText->localized       = false;
        layout->tapText->text            = PIC(description->tapActionText);
        layout->tapText->textColor       = DARK_GRAY;
        layout->tapText->fontId          = SMALL_REGULAR_FONT;
        layout->tapText->obj.area.width  = SCREEN_WIDTH - 2 * BORDER_MARGIN;
        layout->tapText->obj.area.height = nbgl_getFontLineHeight(layout->tapText->fontId);
        layout->tapText->textAlignment   = CENTER;
#ifdef TARGET_STAX
        layout->tapText->obj.alignmentMarginY = BORDER_MARGIN;
#else   // TARGET_STAX
        layout->tapText->obj.alignmentMarginY    = 30;
#endif  // TARGET_STAX
        layout->tapText->obj.alignment = BOTTOM_MIDDLE;
    }

    return (nbgl_layout_t *) layout;
}

/**
 * @brief Creates a swipe interaction on the main container
 *
 * @param layout the current layout
 * @param swipesMask the type of swipes to be handled by the container
 * @param text the text in gray to display at bottom of the main container (can be NULL)
 * @param token the token that will be used as argument of the callback
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is pressed
 * @return >= 0 if OK
 */

int nbgl_layoutAddSwipe(nbgl_layout_t *layout,
                        uint16_t       swipesMask,
                        const char    *text,
                        uint8_t        token,
                        tune_index_e   tuneId)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddSwipe():\n");
    if (layout == NULL) {
        return -1;
    }

    if (text) {
        // create 'tap to continue' text area
        layoutInt->tapText                  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, 0);
        layoutInt->tapText->localized       = false;
        layoutInt->tapText->text            = PIC(text);
        layoutInt->tapText->textColor       = DARK_GRAY;
        layoutInt->tapText->fontId          = SMALL_REGULAR_FONT;
        layoutInt->tapText->obj.area.width  = AVAILABLE_WIDTH;
        layoutInt->tapText->obj.area.height = nbgl_getFontLineHeight(layoutInt->tapText->fontId);
        layoutInt->tapText->textAlignment   = CENTER;
#ifdef TARGET_STAX
        layoutInt->tapText->obj.alignmentMarginY = BORDER_MARGIN;
#else   // TARGET_STAX
        layoutInt->tapText->obj.alignmentMarginY = 30;
#endif  // TARGET_STAX
        layoutInt->tapText->obj.alignment = BOTTOM_MIDDLE;
    }
    return addSwipeInternal(layoutInt, swipesMask, SWIPE_USAGE_CUSTOM, token, tuneId);
}

/**
 * @brief Creates a Top-right button in the top right corner of the top panel
 *
 * @param layout the current layout
 * @param icon icon configuration
 * @param token the token that will be used as argument of the callback
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddTopRightButton(nbgl_layout_t             *layout,
                                 const nbgl_icon_details_t *icon,
                                 uint8_t                    token,
                                 tune_index_e               tuneId)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    layoutObj_t           *obj;
    nbgl_button_t         *button;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddTopRightButton():\n");
    if (layout == NULL) {
        return -1;
    }
    button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
    obj    = addCallbackObj(layoutInt, (nbgl_obj_t *) button, token, tuneId);
    if (obj == NULL) {
        return -1;
    }

    button->obj.area.width       = BUTTON_DIAMETER;
    button->obj.area.height      = BUTTON_DIAMETER;
    button->radius               = BUTTON_RADIUS;
    button->obj.alignmentMarginX = BORDER_MARGIN;
    button->obj.alignmentMarginY = BORDER_MARGIN;
    button->foregroundColor      = BLACK;
    button->innerColor           = WHITE;
    button->borderColor          = LIGHT_GRAY;
    button->obj.touchMask        = (1 << TOUCHED);
    button->obj.touchId          = TOP_RIGHT_BUTTON_ID;
    button->icon                 = PIC(icon);
    button->obj.alignment        = TOP_RIGHT;

    // add to screen
    layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t *) button;
    layoutInt->nbChildren++;

    return 0;
}

/**
 * @brief Creates a navigation bar on bottom of main container
 *
 * @param layout the current layout
 * @param info structure giving the description of the navigation bar
 * @return the height of the control if OK
 */
int nbgl_layoutAddNavigationBar(nbgl_layout_t *layout, const nbgl_layoutNavigationBar_t *info)
{
    nbgl_layoutFooter_t footerDesc;
    footerDesc.type                   = FOOTER_NAV;
    footerDesc.separationLine         = info->withSeparationLine;
    footerDesc.navigation.activePage  = info->activePage;
    footerDesc.navigation.nbPages     = info->nbPages;
    footerDesc.navigation.withExitKey = info->withExitKey;
    footerDesc.navigation.withBackKey = info->withBackKey;
    footerDesc.navigation.token       = info->token;
    footerDesc.navigation.tuneId      = info->tuneId;
    return nbgl_layoutAddExtendedFooter(layout, &footerDesc);
}

#else   // HAVE_SE_TOUCH
/**
 * @brief returns a layout of the given type. The layout is reset
 *
 * @param description description of layout
 * @return a pointer to the corresponding layout
 */
nbgl_layout_t *nbgl_layoutGet(const nbgl_layoutDescription_t *description)
{
    nbgl_layoutInternal_t *layout = NULL;

    // find an empty layout in the proper "layer"
    if (description->modal) {
        if (gLayout[1].nbChildren == 0) {
            layout = &gLayout[1];
        }
        else if (gLayout[2].nbChildren == 0) {
            layout = &gLayout[2];
        }
    }
    else {
        // automatically "release" a potentially opened non-modal layout
        gLayout[0].nbChildren = 0;
        layout                = &gLayout[0];
    }
    if (layout == NULL) {
        LOG_WARN(LAYOUT_LOGGER, "nbgl_layoutGet(): impossible to get a layout!\n");
        return NULL;
    }

    // reset globals
    memset(layout, 0, sizeof(nbgl_layoutInternal_t));

    layout->callback = (nbgl_layoutButtonCallback_t) PIC(description->onActionCallback);
    layout->modal    = description->modal;
    if (description->modal) {
        layout->layer = nbgl_screenPush(&layout->children,
                                        NB_MAX_SCREEN_CHILDREN,
                                        &description->ticker,
                                        (nbgl_buttonCallback_t) buttonCallback);
    }
    else {
        nbgl_screenSet(&layout->children,
                       NB_MAX_SCREEN_CHILDREN,
                       &description->ticker,
                       (nbgl_buttonCallback_t) buttonCallback);
        layout->layer = 0;
    }

    return (nbgl_layout_t *) layout;
}

/**
 * @brief Creates navigation arrows on side(s) of the screen
 *
 * @param layout the current layout
 * @param info structure giving the description of the navigation
 * @return >= 0 if OK
 */
int nbgl_layoutAddNavigation(nbgl_layout_t *layout, nbgl_layoutNavigation_t *info)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddNavigation():\n");
    if (layout == NULL) {
        return -1;
    }

    nbgl_image_t *image;
    if (info->indication & LEFT_ARROW) {
        image                  = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        image->foregroundColor = WHITE;
        image->buffer          = (info->direction == HORIZONTAL_NAV) ? &C_icon_left : &C_icon_up;
        image->obj.area.bpp    = NBGL_BPP_1;
        image->obj.alignment   = MID_LEFT;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) image);
    }
    if (info->indication & RIGHT_ARROW) {
        image                  = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        image->foregroundColor = WHITE;
        image->buffer          = (info->direction == HORIZONTAL_NAV) ? &C_icon_right : &C_icon_down;
        image->obj.area.bpp    = NBGL_BPP_1;
        image->obj.alignment   = MID_RIGHT;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) image);
    }
    return 0;
}
#endif  // HAVE_SE_TOUCH

#ifdef HAVE_SE_TOUCH

/**
 * @brief Creates a centered button at bottom of main container
 * @brief incompatible with navigation bar
 *
 * @param layout the current layout
 * @param icon icon inside the round button
 * @param token used as parameter of userCallback when button is touched
 * @param separationLine if set to true, adds a light gray separation line on top of the container
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddBottomButton(nbgl_layout_t             *layout,
                               const nbgl_icon_details_t *icon,
                               uint8_t                    token,
                               bool                       separationLine,
                               tune_index_e               tuneId)
{
    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddBottomButton():\n");
    nbgl_layoutFooter_t footerDesc;
    footerDesc.type                  = FOOTER_SIMPLE_BUTTON;
    footerDesc.separationLine        = separationLine;
    footerDesc.button.fittingContent = false;
    footerDesc.button.icon           = PIC(icon);
    footerDesc.button.text           = NULL;
    footerDesc.button.token          = token;
    footerDesc.button.tuneId         = tuneId;
    return nbgl_layoutAddExtendedFooter(layout, &footerDesc);
}

/**
 * @brief Creates a touchable bar in main panel
 *
 * @param layout the current layout
 * @param barLayout the properties of the bar
 * @return the height of the bar if OK
 */
int nbgl_layoutAddTouchableBar(nbgl_layout_t *layout, const nbgl_layoutBar_t *barLayout)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    layoutObj_t           *obj;
    nbgl_text_area_t      *textArea;
    nbgl_container_t      *container;
    color_t                color      = (barLayout->inactive != true) ? BLACK : LIGHT_GRAY;
    int16_t                usedHeight = 0;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddTouchableBar():\n");
    if (layout == NULL) {
        return -1;
    }
    // main text is mandatory
    if (barLayout->text == NULL) {
        LOG_FATAL(LAYOUT_LOGGER, "nbgl_layoutAddTouchableBar(): main text is mandatory\n");
    }

    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    obj = addCallbackObj(layoutInt, (nbgl_obj_t *) container, barLayout->token, barLayout->tuneId);
    if (obj == NULL) {
        return -1;
    }

    // get container children (up to 4: text +  left+right icons +  sub text)
    container->children   = nbgl_containerPoolGet(4, layoutInt->layer);
    container->nbChildren = 0;

    container->obj.area.width       = AVAILABLE_WIDTH;
    container->obj.area.height      = barLayout->height;
    container->layout               = HORIZONTAL;
    container->obj.alignmentMarginX = BORDER_MARGIN;
    container->obj.alignment        = NO_ALIGNMENT;
    container->obj.alignTo          = NULL;
    // the bar can only be touched if not inactive AND if one of the icon is present
    // otherwise it is seen as a title
    if ((barLayout->inactive != true)
        && ((barLayout->iconLeft != NULL) || (barLayout->iconRight != NULL))) {
        container->obj.touchMask = (1 << TOUCHED);
        container->obj.touchId   = CONTROLS_ID + nbTouchableControls;
        nbTouchableControls++;
    }

    // allocate main text because always present
    textArea                 = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor      = color;
    textArea->text           = PIC(barLayout->text);
    textArea->onDrawCallback = NULL;
    textArea->fontId         = SMALL_BOLD_FONT;
    textArea->wrapping       = true;
    textArea->obj.area.width = container->obj.area.width;
    if (barLayout->iconLeft != NULL) {
        // reduce text width accordingly
        textArea->obj.area.width
            -= ((nbgl_icon_details_t *) PIC(barLayout->iconLeft))->width + BAR_INTERVALE;
        if (barLayout->centered) {
            textArea->obj.area.width
                -= ((nbgl_icon_details_t *) PIC(barLayout->iconLeft))->width + BAR_INTERVALE;
        }
    }
    if (barLayout->iconRight != NULL) {
        // reduce text width accordingly
        textArea->obj.area.width -= ((nbgl_icon_details_t *) PIC(barLayout->iconRight))->width;
    }
    textArea->obj.area.height = nbgl_getTextHeightInWidth(
        textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
    usedHeight              = textArea->obj.area.height;
    textArea->style         = NO_STYLE;
    textArea->obj.alignment = MID_LEFT;

    if (barLayout->centered != true) {
        textArea->textAlignment = MID_LEFT;
    }
    else {
        textArea->textAlignment = CENTER;
    }
    container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
    container->nbChildren++;

    // allocate left icon if present
    if (barLayout->iconLeft != NULL) {
        nbgl_image_t *imageLeft    = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        imageLeft->foregroundColor = color;
        imageLeft->buffer          = PIC(barLayout->iconLeft);
        // align at the left of text
        imageLeft->obj.alignment                   = MID_LEFT;
        imageLeft->obj.alignTo                     = (nbgl_obj_t *) textArea;
        imageLeft->obj.alignmentMarginX            = BAR_INTERVALE;
        container->children[container->nbChildren] = (nbgl_obj_t *) imageLeft;
        container->nbChildren++;

        textArea->obj.alignmentMarginX = imageLeft->buffer->width + BAR_INTERVALE;

        if (imageLeft->buffer->height > usedHeight) {
            usedHeight = imageLeft->buffer->height;
        }
    }
    // allocate right icon if present
    if (barLayout->iconRight != NULL) {
        nbgl_image_t *imageRight    = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        imageRight->foregroundColor = color;
        imageRight->buffer          = PIC(barLayout->iconRight);
        // align at the right of text
        imageRight->obj.alignment = MID_RIGHT;
        imageRight->obj.alignTo   = (nbgl_obj_t *) textArea;

        container->children[container->nbChildren] = (nbgl_obj_t *) imageRight;
        container->nbChildren++;

        if (imageRight->buffer->height > usedHeight) {
            usedHeight = imageRight->buffer->height;
        }
    }
    if (barLayout->subText != NULL) {
        nbgl_text_area_t *subTextArea
            = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);

        subTextArea->textColor                     = color;
        subTextArea->text                          = PIC(barLayout->subText);
        subTextArea->textAlignment                 = MID_LEFT;
        subTextArea->fontId                        = SMALL_REGULAR_FONT;
        subTextArea->style                         = NO_STYLE;
        subTextArea->wrapping                      = true;
        subTextArea->obj.alignment                 = MID_LEFT;
        subTextArea->obj.area.width                = container->obj.area.width;
        subTextArea->obj.area.height               = nbgl_getTextHeightInWidth(subTextArea->fontId,
                                                                 subTextArea->text,
                                                                 subTextArea->obj.area.width,
                                                                 subTextArea->wrapping);
        container->children[container->nbChildren] = (nbgl_obj_t *) subTextArea;
        container->nbChildren++;
        container->obj.area.height += subTextArea->obj.area.height;

        // modify alignments to have sub-text under (icon left - text - icon right)
        textArea->obj.alignmentMarginY    = -(subTextArea->obj.area.height + 16) / 2;
        subTextArea->obj.alignmentMarginY = (usedHeight + 16) / 2;
    }

    // set this new container as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return container->obj.area.height;
}

/**
 * @brief Creates a switch with the given text and its state
 *
 * @param layout the current layout
 * @param switchLayout description of the parameters of the switch
 * @return height of the control if OK
 */
int nbgl_layoutAddSwitch(nbgl_layout_t *layout, const nbgl_layoutSwitch_t *switchLayout)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    layoutObj_t           *obj;
    nbgl_text_area_t      *textArea;
    nbgl_text_area_t      *subTextArea;
    nbgl_switch_t         *switchObj;
    nbgl_container_t      *container;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddSwitch():\n");
    if (layout == NULL) {
        return -1;
    }
    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    obj       = addCallbackObj(
        layoutInt, (nbgl_obj_t *) container, switchLayout->token, switchLayout->tuneId);
    if (obj == NULL) {
        return -1;
    }

    // get container children
    container->children             = nbgl_containerPoolGet(3, layoutInt->layer);
    container->obj.area.width       = AVAILABLE_WIDTH;
    container->layout               = VERTICAL;
    container->obj.alignmentMarginX = BORDER_MARGIN;
    container->obj.alignment        = NO_ALIGNMENT;
    container->obj.touchMask        = (1 << TOUCHED);
    container->obj.touchId          = CONTROLS_ID + nbTouchableControls;
    nbTouchableControls++;

    // text must be single line
    textArea                  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor       = BLACK;
    textArea->text            = PIC(switchLayout->text);
    textArea->textAlignment   = MID_LEFT;
    textArea->fontId          = SMALL_BOLD_FONT;
    textArea->obj.area.width  = container->obj.area.width - C_switch_60_40.width;
    textArea->obj.area.height = nbgl_getFontHeight(textArea->fontId);
    textArea->obj.alignment   = TOP_LEFT;
#ifdef TARGET_STAX
    textArea->obj.alignmentMarginY = BORDER_MARGIN;
#else   // TARGET_STAX
    textArea->obj.alignmentMarginY = 28;
#endif  // TARGET_STAX
    container->obj.area.height += textArea->obj.area.height + textArea->obj.alignmentMarginY;
    container->children[0] = (nbgl_obj_t *) textArea;

    switchObj                = (nbgl_switch_t *) nbgl_objPoolGet(SWITCH, layoutInt->layer);
    switchObj->onColor       = BLACK;
    switchObj->offColor      = LIGHT_GRAY;
    switchObj->state         = switchLayout->initState;
    switchObj->obj.alignment = MID_RIGHT;
    switchObj->obj.alignTo   = (nbgl_obj_t *) textArea;
    container->children[1]   = (nbgl_obj_t *) switchObj;

    if (switchLayout->subText != NULL) {
        subTextArea            = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        subTextArea->textColor = BLACK;
        subTextArea->text      = PIC(switchLayout->subText);
        subTextArea->textAlignment   = MID_LEFT;
        subTextArea->fontId          = SMALL_REGULAR_FONT;
        subTextArea->wrapping        = true;
        subTextArea->obj.area.width  = container->obj.area.width;
        subTextArea->obj.area.height = nbgl_getTextHeightInWidth(subTextArea->fontId,
                                                                 subTextArea->text,
                                                                 subTextArea->obj.area.width,
                                                                 subTextArea->wrapping);
        subTextArea->obj.alignment   = NO_ALIGNMENT;
#ifdef TARGET_STAX
        subTextArea->obj.alignmentMarginY = INNER_MARGIN;
#else   // TARGET_STAX
        subTextArea->obj.alignmentMarginY = 14;
#endif  // TARGET_STAX
        container->children[2] = (nbgl_obj_t *) subTextArea;
        container->nbChildren  = 3;
#ifdef TARGET_STAX
        container->obj.area.height
            += subTextArea->obj.area.height + subTextArea->obj.alignmentMarginY + BORDER_MARGIN;
#else   // TARGET_STAX
        container->obj.area.height
            += subTextArea->obj.area.height + subTextArea->obj.alignmentMarginY + 26;
#endif  // TARGET_STAX
    }
    else {
        container->nbChildren = 2;
#ifdef TARGET_STAX
        container->obj.area.height += BORDER_MARGIN;
#else   // TARGET_STAX
        container->obj.area.height += 28;
#endif  // TARGET_STAX
    }
    // set this new container as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return container->obj.area.height;
}

/**
 * @brief Creates an area with given text and sub text (in gray)
 *
 * @param layout the current layout
 * @param text main text (in small bold font), optional
 * @param subText description under main text (in small regular font), optional
 * @return height of the control if OK
 */
int nbgl_layoutAddText(nbgl_layout_t *layout, const char *text, const char *subText)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_text_area_t      *textArea;
    nbgl_text_area_t      *subTextArea;
    uint16_t               fullHeight = 0;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddText():\n");
    if (layout == NULL) {
        return -1;
    }
    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

    // get container children
    if (text != NULL) {
        container->nbChildren++;
    }
    if (subText != NULL) {
        container->nbChildren++;
    }

    container->children       = nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
    container->obj.area.width = AVAILABLE_WIDTH;

    if (text != NULL) {
        textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);

        textArea->textColor     = BLACK;
        textArea->text          = PIC(text);
        textArea->textAlignment = MID_LEFT;
        textArea->fontId        = SMALL_BOLD_FONT;
        textArea->style         = NO_STYLE;
        textArea->wrapping      = true;
        textArea->obj.alignment = NO_ALIGNMENT;
#ifdef TARGET_STAX
        textArea->obj.alignmentMarginY = BORDER_MARGIN;
#else   // TARGET_STAX
        textArea->obj.alignmentMarginY = 28;
#endif  // TARGET_STAX
        textArea->obj.area.width  = container->obj.area.width;
        textArea->obj.area.height = nbgl_getTextHeightInWidth(
            textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;
        container->children[0] = (nbgl_obj_t *) textArea;
    }
    if (subText != NULL) {
        subTextArea            = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        subTextArea->textColor = BLACK;
        subTextArea->text      = PIC(subText);
        subTextArea->fontId    = SMALL_REGULAR_FONT;
        subTextArea->style     = NO_STYLE;
        subTextArea->wrapping  = true;
        subTextArea->obj.area.width  = container->obj.area.width;
        subTextArea->obj.area.height = nbgl_getTextHeightInWidth(
            subTextArea->fontId, subTextArea->text, subTextArea->obj.area.width, false);
        subTextArea->textAlignment = MID_LEFT;
        subTextArea->obj.alignment = NO_ALIGNMENT;
        if (text != NULL) {
#ifdef TARGET_STAX
            subTextArea->obj.alignmentMarginY = INTERNAL_MARGIN;
            fullHeight += BORDER_MARGIN;
#else   // TARGET_STAX
            subTextArea->obj.alignmentMarginY = 14;
            fullHeight += 26;  // under the subText
#endif  // TARGET_STAX
            container->children[1] = (nbgl_obj_t *) subTextArea;
        }
        else {
#ifdef TARGET_STAX
            subTextArea->obj.alignmentMarginY = BORDER_MARGIN;
            fullHeight += BORDER_MARGIN;
#else   // TARGET_STAX
            subTextArea->obj.alignmentMarginY = 26;
            fullHeight += 26;  // under the subText
#endif  // TARGET_STAX
            container->children[0] = (nbgl_obj_t *) subTextArea;
        }
        fullHeight += subTextArea->obj.area.height + subTextArea->obj.alignmentMarginY;
    }
    else {
#ifdef TARGET_STAX
        fullHeight += BORDER_MARGIN;
#else   // TARGET_STAX
        fullHeight += 28;
#endif  // TARGET_STAX
    }
    container->obj.area.height      = fullHeight;
    container->layout               = VERTICAL;
    container->obj.alignmentMarginX = BORDER_MARGIN;
    container->obj.alignment        = NO_ALIGNMENT;
    // set this new obj as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return container->obj.area.height;
}

/**
 * @brief Creates an area with given text in 32px font (in Black)
 *
 * @param layout the current layout
 * @param text text to be displayed (auto-wrap)
 * @return >= 0 if OK
 */
int nbgl_layoutAddLargeCaseText(nbgl_layout_t *layout, const char *text)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_area_t      *textArea;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddLargeCaseText():\n");
    if (layout == NULL) {
        return -1;
    }
    textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);

    textArea->textColor       = BLACK;
    textArea->text            = PIC(text);
    textArea->textAlignment   = MID_LEFT;
    textArea->fontId          = LARGE_MEDIUM_FONT;
    textArea->obj.area.width  = AVAILABLE_WIDTH;
    textArea->wrapping        = true;
    textArea->obj.area.height = nbgl_getTextHeightInWidth(
        textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
    textArea->style                = NO_STYLE;
    textArea->obj.alignment        = NO_ALIGNMENT;
    textArea->obj.alignmentMarginX = BORDER_MARGIN;
    textArea->obj.alignmentMarginY = BORDER_MARGIN;
#ifdef TARGET_STAX
    // if first object of container, increase the margin from top
    if (layoutInt->container->nbChildren == 0) {
        textArea->obj.alignmentMarginY += BORDER_MARGIN;
    }
#endif  // TARGET_STAX

    // set this new obj as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);

    return 0;
}

/**
 * @brief Creates a list of radio buttons (on the right)
 *
 * @param layout the current layout
 * @param choices structure giving the list of choices and the current selected one
 * @return >= 0 if OK
 */
int nbgl_layoutAddRadioChoice(nbgl_layout_t *layout, const nbgl_layoutRadioChoice_t *choices)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    layoutObj_t           *obj;
    uint8_t                i;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddRadioChoice():\n");
    if (layout == NULL) {
        return -1;
    }
    for (i = 0; i < choices->nbChoices; i++) {
        nbgl_container_t *container;
        nbgl_text_area_t *textArea;
        nbgl_radio_t     *button;
        nbgl_line_t      *line;

        container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
        textArea  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        button    = (nbgl_radio_t *) nbgl_objPoolGet(RADIO_BUTTON, layoutInt->layer);

        obj = addCallbackObj(layoutInt, (nbgl_obj_t *) container, choices->token, choices->tuneId);
        if (obj == NULL) {
            return -1;
        }

        // get container children (max 2)
        container->nbChildren      = 2;
        container->children        = nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
        container->obj.area.width  = AVAILABLE_WIDTH;
        container->obj.area.height = RADIO_CHOICE_HEIGHT;
        container->obj.alignment   = NO_ALIGNMENT;
        container->obj.alignmentMarginX = BORDER_MARGIN;
        container->obj.alignTo          = (nbgl_obj_t *) NULL;

        // init button for this choice
        button->activeColor = BLACK;
        button->borderColor = LIGHT_GRAY;
#ifdef TARGET_STAX
        button->obj.alignmentMarginX = 4;
#endif  // TARGET_STAX
        button->obj.alignTo    = (nbgl_obj_t *) container;
        button->obj.alignment  = MID_RIGHT;
        button->state          = OFF_STATE;
        container->children[1] = (nbgl_obj_t *) button;

        // init text area for this choice
        if (choices->localized == true) {
            textArea->localized = true;
#if defined(HAVE_LANGUAGE_PACK)
            textArea->textId = choices->nameIds[i];
#endif  // defined(HAVE_LANGUAGE_PACK)
        }
        else {
            textArea->text = PIC(choices->names[i]);
        }
        textArea->textAlignment = MID_LEFT;
#ifdef TARGET_STAX
        textArea->obj.area.width = container->obj.area.width - 40 - 16;
#else   // TARGET_STAX
        textArea->obj.area.width = container->obj.area.width - 40;
#endif  // TARGET_STAX
        textArea->style         = NO_STYLE;
        textArea->obj.alignment = MID_LEFT;
        textArea->obj.alignTo   = (nbgl_obj_t *) container;
        container->children[0]  = (nbgl_obj_t *) textArea;

        // whole container should be touchable
        container->obj.touchMask = (1 << TOUCHED);
        container->obj.touchId   = CONTROLS_ID + nbTouchableControls;
        nbTouchableControls++;

        // highlight init choice
        if (i == choices->initChoice) {
            button->state       = ON_STATE;
            textArea->textColor = BLACK;
            textArea->fontId    = SMALL_BOLD_FONT;
        }
        else {
            button->state       = OFF_STATE;
            textArea->textColor = DARK_GRAY;
            textArea->fontId    = SMALL_REGULAR_FONT;
        }
        textArea->obj.area.height = nbgl_getFontHeight(textArea->fontId);

        line                       = createHorizontalLine(layoutInt->layer);
        line->obj.alignmentMarginY = -4;
        line->offset               = 3;

        // set these new objs as child of main container
        addObjectToLayout(layoutInt, (nbgl_obj_t *) container);
        addObjectToLayout(layoutInt, (nbgl_obj_t *) line);
    }

    return 0;
}

/**
 * @brief Creates an area on the center of the main panel, with a possible icon/image,
 * a possible text in black under it, and a possible text in gray under it
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddCenteredInfo(nbgl_layout_t *layout, const nbgl_layoutCenteredInfo_t *info)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_text_area_t      *textArea   = NULL;
    nbgl_image_t          *image      = NULL;
    uint16_t               fullHeight = 0;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddCenteredInfo():\n");
    if (layout == NULL) {
        return -1;
    }

    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

    // get container children (max 5 if PLUGIN_INFO)
    container->children
        = nbgl_containerPoolGet((info->style == PLUGIN_INFO) ? 5 : 4, layoutInt->layer);
    container->nbChildren = 0;

    if (info->icon != NULL) {
        image                  = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        image->foregroundColor = BLACK;
        image->buffer          = PIC(info->icon);
        image->obj.area.bpp    = NBGL_BPP_1;
        image->obj.alignment   = TOP_MIDDLE;
        image->obj.alignTo     = NULL;

        fullHeight += image->buffer->height;
        if ((info->style != PLUGIN_INFO)) {
            container->children[container->nbChildren] = (nbgl_obj_t *) image;
            container->nbChildren++;
        }
    }
    if (info->text1 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = BLACK;
        textArea->text          = PIC(info->text1);
        textArea->textAlignment = CENTER;
        if (info->style != NORMAL_INFO) {
            textArea->fontId = LARGE_MEDIUM_FONT;
        }
        else {
            textArea->fontId = SMALL_BOLD_FONT;
        }
        textArea->wrapping        = true;
        textArea->obj.area.width  = AVAILABLE_WIDTH;
        textArea->obj.area.height = nbgl_getTextHeightInWidth(
            textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);

        textArea->style = NO_STYLE;
        if (container->nbChildren > 0) {
            textArea->obj.alignment = BOTTOM_MIDDLE;
            textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
#ifdef TARGET_STAX
            textArea->obj.alignmentMarginY = BORDER_MARGIN + 4;
#else   // TARGET_STAX
            textArea->obj.alignmentMarginY = BOTTOM_BORDER_MARGIN;
#endif  // TARGET_STAX
        }
        else {
            textArea->obj.alignment = TOP_MIDDLE;
            textArea->obj.alignTo   = NULL;
        }

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    if (info->text2 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = (info->style == NORMAL_INFO) ? DARK_GRAY : BLACK;
        textArea->text          = PIC(info->text2);
        textArea->textAlignment = CENTER;
        textArea->fontId
            = (info->style != LARGE_CASE_BOLD_INFO) ? SMALL_REGULAR_FONT : SMALL_BOLD_FONT;
        textArea->wrapping        = true;
        textArea->obj.area.width  = AVAILABLE_WIDTH;
        textArea->obj.area.height = nbgl_getTextHeightInWidth(
            textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);

        textArea->style = NO_STYLE;
        if (container->nbChildren > 0) {
            textArea->obj.alignment = BOTTOM_MIDDLE;
            textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
            if (info->text1 != NULL) {
#ifdef TARGET_STAX
                // if previous element is text1, only space of 20 px
                textArea->obj.alignmentMarginY = 20;
#else   // TARGET_STAX
                textArea->obj.alignmentMarginY = 16;
#endif  // TARGET_STAX
            }
            else {
#ifdef TARGET_STAX
                // else if icon, space of 28 px
                textArea->obj.alignmentMarginY = 28;
#else   // TARGET_STAX
                textArea->obj.alignmentMarginY = 24;
#endif  // TARGET_STAX
            }
        }
        else {
            textArea->obj.alignment = TOP_MIDDLE;
        }

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    // draw small horizontal line if PLUGIN_INFO
    if (info->style == PLUGIN_INFO) {
        nbgl_line_t *line          = createHorizontalLine(layoutInt->layer);
        line->obj.area.width       = 120;
        line->obj.alignmentMarginY = 32;
        line->obj.alignmentMarginX = 0;
        line->obj.alignment        = BOTTOM_MIDDLE;
        line->obj.alignTo          = (nbgl_obj_t *) container->children[container->nbChildren - 1];
        fullHeight += 32;

        container->children[container->nbChildren] = (nbgl_obj_t *) line;
        container->nbChildren++;
        if (image) {
            // add icon here, under line
            image->obj.alignmentMarginY = 32;
            image->obj.alignment        = BOTTOM_MIDDLE;
            image->obj.alignTo = (nbgl_obj_t *) container->children[container->nbChildren - 1];
            container->children[container->nbChildren] = (nbgl_obj_t *) image;
            container->nbChildren++;
            fullHeight += 32;
        }
    }
    if (info->text3 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = (info->style == LARGE_CASE_GRAY_INFO) ? DARK_GRAY : BLACK;
        textArea->text          = PIC(info->text3);
        textArea->textAlignment = CENTER;
        textArea->fontId        = SMALL_REGULAR_FONT;
        textArea->wrapping      = true;
        textArea->obj.area.width  = AVAILABLE_WIDTH;
        textArea->obj.area.height = nbgl_getTextHeightInWidth(
            textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
        textArea->style = NO_STYLE;
        if (container->nbChildren > 0) {
            textArea->obj.alignment = BOTTOM_MIDDLE;
            textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
#ifdef TARGET_STAX
            textArea->obj.alignmentMarginY
                = (info->style == LARGE_CASE_BOLD_INFO) ? 0 : BORDER_MARGIN;
#else   // TARGET_STAX
            textArea->obj.alignmentMarginY = (info->style == LARGE_CASE_BOLD_INFO) ? 16 : 28;
#endif  // TARGET_STAX
        }
        else {
            textArea->obj.alignment = TOP_MIDDLE;
            textArea->obj.alignTo   = NULL;
        }

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    container->obj.area.height = fullHeight;
    container->layout          = VERTICAL;
    if (info->onTop) {
        container->obj.alignmentMarginX = BORDER_MARGIN;
        container->obj.alignmentMarginY = BORDER_MARGIN + info->offsetY;
        container->obj.alignment        = NO_ALIGNMENT;
    }
    else {
        container->obj.alignmentMarginY = info->offsetY;
        container->obj.alignment        = CENTER;
    }

    container->obj.area.width = AVAILABLE_WIDTH;

    // set this new container as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return 0;
}

#else   // HAVE_SE_TOUCH
/**
 * @brief Creates an area with given text and sub text, using the given style
 *
 * @param layout the current layout
 * @param text main text for the switch
 * @param subText description under main text (NULL terminated, single line, may be null)
 * @param style if @ref REGULAR_INFO, use regular font for text, otherwise use bold font for text
 * @return >= 0 if OK
 */
int nbgl_layoutAddText(nbgl_layout_t                  *layout,
                       const char                     *text,
                       const char                     *subText,
                       nbgl_contentCenteredInfoStyle_t style)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_text_area_t      *textArea;
    uint16_t               fullHeight = 0;

    UNUSED(subText);

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddText():\n");
    if (layout == NULL) {
        return -1;
    }
    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

    // get container children
    container->nbChildren = 1;
    if (subText != NULL) {
        container->nbChildren++;
    }

    container->children       = nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
    container->obj.area.width = AVAILABLE_WIDTH;

    textArea                 = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor      = WHITE;
    textArea->text           = PIC(text);
    textArea->textAlignment  = CENTER;
    textArea->fontId         = (style == REGULAR_INFO) ? BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp
                                                       : BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
    textArea->obj.area.width = AVAILABLE_WIDTH;

    uint16_t nbLines
        = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
    // if more than available lines on screen
    if (nbLines > NB_MAX_LINES) {
        uint16_t len;

        nbLines              = NB_MAX_LINES;
        textArea->nbMaxLines = NB_MAX_LINES;
        nbgl_getTextMaxLenInNbLines(
            textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
        textArea->len = len;
    }
    const nbgl_font_t *font   = nbgl_getFont(textArea->fontId);
    textArea->obj.area.height = nbLines * font->line_height;
    textArea->wrapping        = true;
    textArea->obj.alignment   = TOP_MIDDLE;
    fullHeight += textArea->obj.area.height;
    container->children[0] = (nbgl_obj_t *) textArea;

    if (subText != NULL) {
        textArea            = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor = WHITE;
        textArea->text      = PIC(subText);
        textArea->wrapping  = true;
        textArea->fontId    = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        textArea->obj.area.width = AVAILABLE_WIDTH;
        nbLines
            = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
        // if more than available lines on screen
        if (nbLines > (NB_MAX_LINES - 1)) {
            uint16_t len;
            nbLines              = NB_MAX_LINES - 1;
            textArea->nbMaxLines = nbLines;
            nbgl_getTextMaxLenInNbLines(
                textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
            textArea->len = len;
        }
        textArea->obj.area.height      = nbLines * font->line_height;
        textArea->textAlignment        = CENTER;
        textArea->obj.alignment        = NO_ALIGNMENT;
        textArea->obj.alignmentMarginY = 2;
        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;
        container->children[1] = (nbgl_obj_t *) textArea;
    }
    container->obj.area.height = fullHeight;
    container->layout          = VERTICAL;
    container->obj.alignment   = CENTER;
    // set this new obj as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return 0;
}

/**
 * @brief Creates a menu list (only for nanos) with the given parameters. The navigation (and
 * selection) must be handled by the caller
 *
 * @param layout the current layout
 * @param list structure giving the list of choices and the current selected one
 * @return >= 0 if OK
 */
int nbgl_layoutAddMenuList(nbgl_layout_t *layout, nbgl_layoutMenuList_t *list)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    uint8_t                i;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddMenuList():\n");
    if (layout == NULL) {
        return -1;
    }
    for (i = 0; i < list->nbChoices; i++) {
        nbgl_text_area_t *textArea;

        // check whether this object is visible or not
        // only the two objects above or below the selected one are visible
        if (((list->selectedChoice > 2) && (i < (list->selectedChoice - 2)))
            || (i > (list->selectedChoice + 2))) {
            continue;
        }

        textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);

        // init text area for this choice
        textArea->text                 = list->callback(i);
        textArea->textAlignment        = CENTER;
        textArea->obj.area.width       = AVAILABLE_WIDTH;
        textArea->obj.area.height      = 12;
        textArea->style                = NO_STYLE;
        textArea->obj.alignment        = CENTER;
        textArea->obj.alignmentMarginY = ((i - list->selectedChoice) * 16);
        textArea->textColor            = WHITE;

        // highlight init choice
        if (i == list->selectedChoice) {
            textArea->fontId = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
        }
        else {
            textArea->fontId = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        }

        // set this new obj as child of main container
        addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);
    }

    return 0;
}

/**
 * @brief Creates an area on the center of the main panel, with a possible icon/image,
 * a possible text in black under it, and a possible text in gray under it
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddCenteredInfo(nbgl_layout_t *layout, const nbgl_layoutCenteredInfo_t *info)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_text_area_t      *textArea   = NULL;
    nbgl_image_t          *image      = NULL;
    uint16_t               fullHeight = 0;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddCenteredInfo():\n");
    if (layout == NULL) {
        return -1;
    }

    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

    // 3 children at max
    container->children   = nbgl_containerPoolGet(3, layoutInt->layer);
    container->nbChildren = 0;
    if (info->icon != NULL) {
        image                  = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        image->foregroundColor = WHITE;
        image->buffer          = PIC(info->icon);
        image->obj.area.bpp    = NBGL_BPP_1;
        image->obj.alignment   = TOP_MIDDLE;
        image->obj.alignTo     = NULL;

        fullHeight += image->buffer->height;
        container->children[container->nbChildren] = (nbgl_obj_t *) image;
        container->nbChildren++;
    }
    if (info->text1 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = WHITE;
        textArea->text          = PIC(info->text1);
        textArea->textAlignment = CENTER;
        textArea->fontId = (info->style == REGULAR_INFO) ? BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp
                                                         : BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
        textArea->obj.area.width = AVAILABLE_WIDTH;
        textArea->wrapping       = true;
        uint16_t nbLines
            = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
        // if more than available lines on screen
        if (nbLines > NB_MAX_LINES) {
            uint16_t len;
            nbLines              = NB_MAX_LINES;
            textArea->nbMaxLines = NB_MAX_LINES;
            nbgl_getTextMaxLenInNbLines(
                textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
            textArea->len = len;
        }
        const nbgl_font_t *font   = nbgl_getFont(textArea->fontId);
        textArea->obj.area.height = nbLines * font->line_height;
        textArea->style           = NO_STYLE;
        if (info->icon != NULL) {
            textArea->obj.alignment = BOTTOM_MIDDLE;  // under icon
            textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
            textArea->obj.alignmentMarginY = (nbLines < 3) ? 4 : 0;
        }
        else if (info->text2 == NULL) {
            textArea->obj.alignment = CENTER;
            textArea->obj.alignTo   = NULL;
        }
        else {
            textArea->obj.alignment = TOP_MIDDLE;
            textArea->obj.alignTo   = NULL;
        }

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    if (info->text2 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = WHITE;
        textArea->text          = PIC(info->text2);
        textArea->textAlignment = CENTER;
        textArea->fontId        = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        textArea->obj.area.width = AVAILABLE_WIDTH;
        uint16_t nbLines
            = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
        // if more than available lines on screen
        if (nbLines > (NB_MAX_LINES - 1)) {
            uint16_t len;
            nbLines              = NB_MAX_LINES - 1;
            textArea->nbMaxLines = nbLines;
            nbgl_getTextMaxLenInNbLines(
                textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
            textArea->len = len;
        }
        const nbgl_font_t *font   = nbgl_getFont(textArea->fontId);
        textArea->obj.area.height = nbLines * font->line_height;

        textArea->style         = NO_STYLE;
        textArea->obj.alignment = BOTTOM_MIDDLE;
        textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
        textArea->obj.alignmentMarginY = 2;

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    container->obj.area.height      = fullHeight;
    container->layout               = VERTICAL;
    container->obj.alignmentMarginY = 0;
    if (info->onTop) {
        container->obj.alignment = TOP_MIDDLE;
    }
    else {
        container->obj.alignment = CENTER;
    }

    container->obj.area.width = AVAILABLE_WIDTH;

    // set this new container as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return 0;
}
#endif  // HAVE_SE_TOUCH

#ifdef NBGL_QRCODE
/**
 * @brief Creates an area on the center of the main panel, with a QRCode,
 * a possible text in black (bold) under it, and a possible text in black under it
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddQRCode(nbgl_layout_t *layout, const nbgl_layoutQRCode_t *info)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_text_area_t      *textArea = NULL;
    nbgl_qrcode_t         *qrcode;
    uint16_t               fullHeight = 0;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddQRCode():\n");
    if (layout == NULL) {
        return -1;
    }

    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

    // get container children (max 2 (QRCode + text1 + text2))
    container->children   = nbgl_containerPoolGet(3, layoutInt->layer);
    container->nbChildren = 0;

    qrcode = (nbgl_qrcode_t *) nbgl_objPoolGet(QR_CODE, layoutInt->layer);
    // version is forced to V10 if url is longer than 62 characters
    if (strlen(PIC(info->url)) > 62) {
        qrcode->version = QRCODE_V10;
    }
    else {
        qrcode->version = QRCODE_V4;
    }
    qrcode->foregroundColor = BLACK;
    // in QR V4, we use 8*8 screen pixels for one QR pixel
    // in QR V10, we use 4*4 screen pixels for one QR pixel
    qrcode->obj.area.width
        = (qrcode->version == QRCODE_V4) ? (QR_V4_NB_PIX_SIZE * 8) : (QR_V10_NB_PIX_SIZE * 4);
    qrcode->obj.area.height = qrcode->obj.area.width;
    qrcode->text            = PIC(info->url);
    qrcode->obj.area.bpp    = NBGL_BPP_1;
    qrcode->obj.alignment   = TOP_MIDDLE;

    fullHeight += qrcode->obj.area.height;
    container->children[container->nbChildren] = (nbgl_obj_t *) qrcode;
    container->nbChildren++;

    if (info->text1 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = BLACK;
        textArea->text          = PIC(info->text1);
        textArea->textAlignment = CENTER;
        textArea->fontId   = (info->largeText1 == true) ? LARGE_MEDIUM_FONT : SMALL_REGULAR_FONT;
        textArea->wrapping = true;
        textArea->obj.area.width  = AVAILABLE_WIDTH;
        textArea->obj.area.height = nbgl_getTextHeightInWidth(
            textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
        textArea->obj.alignment = BOTTOM_MIDDLE;
        textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
#ifdef TARGET_STAX
        textArea->obj.alignmentMarginY = 40;
#else   // TARGET_STAX
        textArea->obj.alignmentMarginY = 24;
#endif  // TARGET_STAX

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    if (info->text2 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = DARK_GRAY;
        textArea->text          = PIC(info->text2);
        textArea->textAlignment = CENTER;
        textArea->fontId        = SMALL_REGULAR_FONT;
        textArea->wrapping      = true;
        textArea->obj.area.width  = AVAILABLE_WIDTH;
        textArea->obj.area.height = nbgl_getTextHeightInWidth(
            textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
        textArea->obj.alignment = BOTTOM_MIDDLE;
        textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
#ifdef TARGET_STAX
        textArea->obj.alignmentMarginY = 40;
#else   // TARGET_STAX
        textArea->obj.alignmentMarginY = 28;
#endif  // TARGET_STAX

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    container->obj.area.height = fullHeight;
    container->layout          = VERTICAL;
    if (info->centered) {
        container->obj.alignment = CENTER;
    }
    else {
        container->obj.alignment = BOTTOM_MIDDLE;
        container->obj.alignTo
            = layoutInt->container->children[layoutInt->container->nbChildren - 1];
    }
    container->obj.alignmentMarginY = info->offsetY;

    container->obj.area.width = AVAILABLE_WIDTH;

    // set this new container as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return 0;
}
#endif  // NBGL_QRCODE

#ifdef HAVE_SE_TOUCH
/**
 * @brief Creates two buttons to make a choice. Both buttons are mandatory.
 *        Both buttons are full width, one under the other
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddChoiceButtons(nbgl_layout_t *layout, const nbgl_layoutChoiceButtons_t *info)
{
    nbgl_layoutFooter_t footerDesc;
    footerDesc.type                     = FOOTER_CHOICE_BUTTONS;
    footerDesc.separationLine           = false;
    footerDesc.choiceButtons.bottomText = info->bottomText;
    footerDesc.choiceButtons.token      = info->token;
    footerDesc.choiceButtons.topText    = info->topText;
    footerDesc.choiceButtons.style      = info->style;
    footerDesc.choiceButtons.tuneId     = info->tuneId;
    return nbgl_layoutAddExtendedFooter(layout, &footerDesc);
}

/**
 * @brief Creates two buttons to make a choice. Both buttons are mandatory
 *        The left one contains only an icon and is round, the other contains only
 *        a text
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (text, icon, tokens)
 * @return >= 0 if OK
 */
int nbgl_layoutAddHorizontalButtons(nbgl_layout_t                        *layout,
                                    const nbgl_layoutHorizontalButtons_t *info)
{
    layoutObj_t           *obj;
    nbgl_button_t         *button;
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddHorizontalButtons():\n");
    if (layout == NULL) {
        return -1;
    }

    // icon & text cannot be NULL
    if ((info->leftIcon == NULL) || (info->rightText == NULL)) {
        return -1;
    }

    // create left button (in white) at first
    button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
    obj    = addCallbackObj(layoutInt, (nbgl_obj_t *) button, info->leftToken, info->tuneId);
    if (obj == NULL) {
        return -1;
    }
    // associate with with index 1
    obj->index                   = 1;
    button->obj.alignment        = BOTTOM_LEFT;
    button->obj.alignmentMarginX = BORDER_MARGIN;
    button->obj.alignmentMarginY = 24;  // 24 pixels from screen bottom
    button->borderColor          = LIGHT_GRAY;
    button->innerColor           = WHITE;
    button->foregroundColor      = BLACK;
    button->obj.area.width       = BUTTON_DIAMETER;
    button->obj.area.height      = BUTTON_DIAMETER;
    button->radius               = BUTTON_RADIUS;
    button->icon                 = PIC(info->leftIcon);
    button->fontId               = SMALL_BOLD_FONT;
    button->obj.touchMask        = (1 << TOUCHED);
    button->obj.touchId          = CHOICE_2_ID;
    // set this new button as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) button);

    // then black button, on right
    button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
    obj    = addCallbackObj(layoutInt, (nbgl_obj_t *) button, info->rightToken, info->tuneId);
    if (obj == NULL) {
        return -1;
    }
    // associate with with index 0
    obj->index                   = 0;
    button->obj.alignment        = BOTTOM_RIGHT;
    button->obj.alignmentMarginX = BORDER_MARGIN;
    button->obj.alignmentMarginY = 24;  // 24 pixels from screen bottom
    button->innerColor           = BLACK;
    button->borderColor          = BLACK;
    button->foregroundColor      = WHITE;
    button->obj.area.width       = AVAILABLE_WIDTH - BUTTON_DIAMETER - 16;
    button->obj.area.height      = BUTTON_DIAMETER;
    button->radius               = BUTTON_RADIUS;
    button->text                 = PIC(info->rightText);
    button->fontId               = SMALL_BOLD_FONT;
    button->obj.touchMask        = (1 << TOUCHED);
    button->obj.touchId          = CHOICE_1_ID;
    // set this new button as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) button);

    return 0;
}

/**
 * @brief Creates a list of [tag,value] pairs
 *
 * @param layout the current layout
 * @param list structure giving the list of [tag,value] pairs
 * @return >= 0 if OK
 */
int nbgl_layoutAddTagValueList(nbgl_layout_t *layout, const nbgl_layoutTagValueList_t *list)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_area_t      *itemTextArea;
    nbgl_text_area_t      *valueTextArea;
    nbgl_container_t      *container;
    uint8_t                i;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddTagValueList():\n");
    if (layout == NULL) {
        return -1;
    }

    for (i = 0; i < list->nbPairs; i++) {
        const nbgl_layoutTagValue_t *pair;
        uint16_t                     fullHeight = 0, usableWidth;

        if (list->pairs != NULL) {
            pair = &list->pairs[i];
        }
        else {
            pair = list->callback(list->startIndex + i);
        }
        // width that can be used for item and text
        usableWidth = AVAILABLE_WIDTH;

        container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

        // get container children (max 3 if a valueIcon, otherwise 2)
        container->children
            = nbgl_containerPoolGet((pair->valueIcon != NULL) ? 3 : 2, layoutInt->layer);

        itemTextArea  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        valueTextArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);

        // init text area for this choice
        itemTextArea->textColor       = DARK_GRAY;
        itemTextArea->text            = PIC(pair->item);
        itemTextArea->textAlignment   = MID_LEFT;
        itemTextArea->fontId          = SMALL_REGULAR_FONT;
        itemTextArea->wrapping        = true;
        itemTextArea->obj.area.width  = usableWidth;
        itemTextArea->obj.area.height = nbgl_getTextHeightInWidth(
            itemTextArea->fontId, itemTextArea->text, usableWidth, itemTextArea->wrapping);
        itemTextArea->style                        = NO_STYLE;
        itemTextArea->obj.alignment                = NO_ALIGNMENT;
        itemTextArea->obj.alignmentMarginX         = 0;
        itemTextArea->obj.alignmentMarginY         = 0;
        itemTextArea->obj.alignTo                  = NULL;
        container->children[container->nbChildren] = (nbgl_obj_t *) itemTextArea;
        container->nbChildren++;

        fullHeight += itemTextArea->obj.area.height;

        // init button for this choice
        valueTextArea->textColor     = BLACK;
        valueTextArea->text          = PIC(pair->value);
        valueTextArea->textAlignment = MID_LEFT;
        if (list->smallCaseForValue) {
            valueTextArea->fontId = SMALL_BOLD_FONT;
        }
        else {
            valueTextArea->fontId = LARGE_MEDIUM_FONT;
        }
        if (pair->valueIcon == NULL) {
            valueTextArea->obj.area.width = usableWidth;
        }
        else {
            // we assume that value is single line
            valueTextArea->obj.area.width
                = nbgl_getTextWidth(valueTextArea->fontId, valueTextArea->text);
        }

        // handle the nbMaxLinesForValue parameter, used to automatically keep only
        // nbMaxLinesForValue lines
        uint16_t nbLines = nbgl_getTextNbLinesInWidth(
            valueTextArea->fontId, valueTextArea->text, usableWidth, list->wrapping);
        // use this nbMaxLinesForValue parameter only if >0
        if ((list->nbMaxLinesForValue > 0) && (nbLines > list->nbMaxLinesForValue)) {
            nbLines                   = list->nbMaxLinesForValue;
            valueTextArea->nbMaxLines = list->nbMaxLinesForValue;
        }
        const nbgl_font_t *font                    = nbgl_getFont(valueTextArea->fontId);
        valueTextArea->obj.area.height             = nbLines * font->line_height;
        valueTextArea->style                       = NO_STYLE;
        valueTextArea->obj.alignment               = BOTTOM_LEFT;
        valueTextArea->obj.alignmentMarginY        = 4;
        valueTextArea->obj.alignTo                 = (nbgl_obj_t *) itemTextArea;
        valueTextArea->wrapping                    = list->wrapping;
        container->children[container->nbChildren] = (nbgl_obj_t *) valueTextArea;
        container->nbChildren++;

        fullHeight += valueTextArea->obj.area.height + valueTextArea->obj.alignmentMarginY;
        if (pair->valueIcon != NULL) {
            nbgl_image_t *image = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
            layoutObj_t  *obj
                = addCallbackObj(layoutInt, (nbgl_obj_t *) image, list->token, TUNE_TAP_CASUAL);
            obj->index                  = i;
            image->foregroundColor      = BLACK;
            image->buffer               = PIC(pair->valueIcon);
            image->obj.alignment        = MID_RIGHT;
            image->obj.alignmentMarginX = 4;
            image->obj.alignTo          = (nbgl_obj_t *) valueTextArea;
            image->obj.touchMask        = (1 << TOUCHED);

            container->children[container->nbChildren] = (nbgl_obj_t *) image;
            container->nbChildren++;
        }

        container->obj.area.width       = usableWidth;
        container->obj.area.height      = fullHeight;
        container->layout               = VERTICAL;
        container->obj.alignmentMarginX = BORDER_MARGIN;
#ifdef TARGET_STAX
        container->obj.alignmentMarginY = 12;
#else   // TARGET_STAX
        // On Europa, 24 px between each tag/value pair
        if (i > 0) {
            container->obj.alignmentMarginY = 24;
        }
#endif  // TARGET_STAX
        container->obj.alignment = NO_ALIGNMENT;

        addObjectToLayout(layoutInt, (nbgl_obj_t *) container);
    }

    return 0;
}
#endif  // HAVE_SE_TOUCH

/**
 * @brief Creates an area in main panel to display a progress bar, with a title text and a
 * description under the progress
 *
 * @param layout the current layout
 * @param barLayout structure giving the description of progress bar
 * @return >= 0 if OK
 */
int nbgl_layoutAddProgressBar(nbgl_layout_t *layout, const nbgl_layoutProgressBar_t *barLayout)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_progress_bar_t   *progress;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddProgressBar():\n");
    if (layout == NULL) {
        return -1;
    }
#ifdef HAVE_SE_TOUCH
    if (barLayout->text != NULL) {
        nbgl_text_area_t *textArea;

        textArea                  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA,
                                                        ((nbgl_layoutInternal_t *) layout)->layer);
        textArea->textColor       = BLACK;
        textArea->text            = PIC(barLayout->text);
        textArea->textAlignment   = MID_LEFT;
        textArea->fontId          = SMALL_REGULAR_FONT;
        textArea->wrapping        = true;
        textArea->obj.area.width  = AVAILABLE_WIDTH;
        textArea->obj.area.height = nbgl_getTextHeightInWidth(
            textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
        textArea->style                = NO_STYLE;
        textArea->obj.alignment        = NO_ALIGNMENT;
        textArea->obj.alignmentMarginX = BORDER_MARGIN;
        textArea->obj.alignmentMarginY = BORDER_MARGIN;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);
    }
    progress                       = (nbgl_progress_bar_t *) nbgl_objPoolGet(PROGRESS_BAR,
                                                       ((nbgl_layoutInternal_t *) layout)->layer);
    progress->foregroundColor      = BLACK;
    progress->withBorder           = true;
    progress->state                = barLayout->percentage;
    progress->obj.area.width       = 120;
    progress->obj.area.height      = 12;
    progress->obj.alignment        = NO_ALIGNMENT;
    progress->obj.alignmentMarginX = (AVAILABLE_WIDTH - progress->obj.area.width) / 2;
    progress->obj.alignmentMarginY = BORDER_MARGIN;
    addObjectToLayout(layoutInt, (nbgl_obj_t *) progress);

    if (barLayout->subText != NULL) {
        nbgl_text_area_t *subTextArea;

        subTextArea = (nbgl_text_area_t *) nbgl_objPoolGet(
            TEXT_AREA, ((nbgl_layoutInternal_t *) layout)->layer);
        subTextArea->textColor            = LIGHT_GRAY;
        subTextArea->text                 = PIC(barLayout->subText);
        subTextArea->textAlignment        = MID_LEFT;
        subTextArea->fontId               = SMALL_REGULAR_FONT;
        subTextArea->wrapping             = true;
        subTextArea->obj.area.width       = AVAILABLE_WIDTH;
        subTextArea->obj.area.height      = nbgl_getTextHeightInWidth(subTextArea->fontId,
                                                                 subTextArea->text,
                                                                 subTextArea->obj.area.width,
                                                                 subTextArea->wrapping);
        subTextArea->style                = NO_STYLE;
        subTextArea->obj.alignment        = NO_ALIGNMENT;
        subTextArea->obj.alignmentMarginX = BORDER_MARGIN;
        subTextArea->obj.alignmentMarginY = BORDER_MARGIN;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) subTextArea);
    }
#else   // HAVE_SE_TOUCH
    if (barLayout->text != NULL) {
        nbgl_text_area_t *textArea;

        textArea                       = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA,
                                                        ((nbgl_layoutInternal_t *) layout)->layer);
        textArea->textColor            = WHITE;
        textArea->text                 = PIC(barLayout->text);
        textArea->textAlignment        = CENTER;
        textArea->fontId               = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        textArea->obj.area.width       = AVAILABLE_WIDTH;
        textArea->obj.area.height      = nbgl_getTextHeight(textArea->fontId, textArea->text);
        textArea->obj.alignment        = TOP_MIDDLE;
        textArea->obj.alignmentMarginX = 0;
        textArea->obj.alignmentMarginY = 16;  // 16 px from top
        addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);
    }
    progress                       = (nbgl_progress_bar_t *) nbgl_objPoolGet(PROGRESS_BAR,
                                                       ((nbgl_layoutInternal_t *) layout)->layer);
    progress->foregroundColor      = WHITE;
    progress->withBorder           = true;
    progress->state                = barLayout->percentage;
    progress->obj.area.width       = 102;
    progress->obj.area.height      = 14;
    progress->obj.alignment        = TOP_MIDDLE;
    progress->obj.alignmentMarginX = 0;
    progress->obj.alignmentMarginY = 33;  // 33px from top
    addObjectToLayout(layoutInt, (nbgl_obj_t *) progress);

    if (barLayout->subText != NULL) {
        nbgl_text_area_t *subTextArea;

        subTextArea = (nbgl_text_area_t *) nbgl_objPoolGet(
            TEXT_AREA, ((nbgl_layoutInternal_t *) layout)->layer);
        subTextArea->textColor       = WHITE;
        subTextArea->text            = PIC(barLayout->subText);
        subTextArea->textAlignment   = CENTER;
        subTextArea->fontId          = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        subTextArea->obj.area.width  = AVAILABLE_WIDTH;
        subTextArea->obj.area.height = nbgl_getTextHeight(subTextArea->fontId, subTextArea->text);
        subTextArea->obj.alignment   = BOTTOM_MIDDLE;
        subTextArea->obj.alignTo     = (nbgl_obj_t *) progress;
        subTextArea->obj.alignmentMarginX = 0;
        subTextArea->obj.alignmentMarginY = 4;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) subTextArea);
    }
#endif  // HAVE_SE_TOUCH

    return 0;
}

#ifdef HAVE_SE_TOUCH

/**
 * @brief adds a separation line on bottom of the last added item
 *
 * @param layout the current layout
 * @return >= 0 if OK
 */
int nbgl_layoutAddSeparationLine(nbgl_layout_t *layout)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_line_t           *line;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddSeparationLine():\n");
    line                       = createHorizontalLine(layoutInt->layer);
    line->obj.alignmentMarginY = -4;
    line->offset               = 3;
    addObjectToLayout(layoutInt, (nbgl_obj_t *) line);
    return 0;
}

/**
 * @brief Creates a rounded button in the main container.
 *
 * @param layout the current layout
 * @param buttonInfo structure giving the description of button (text, icon, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddButton(nbgl_layout_t *layout, const nbgl_layoutButton_t *buttonInfo)
{
    layoutObj_t           *obj;
    nbgl_button_t         *button;
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddButton():\n");
    if (layout == NULL) {
        return -1;
    }

    // Add in footer if matching
    if ((buttonInfo->onBottom) && (!buttonInfo->fittingContent)
        && (layoutInt->footerContainer == NULL)) {
        nbgl_layoutFooter_t footerDesc;
        footerDesc.type           = FOOTER_SIMPLE_BUTTON;
        footerDesc.separationLine = false;
        footerDesc.button.text    = buttonInfo->text;
        footerDesc.button.token   = buttonInfo->token;
        footerDesc.button.tuneId  = buttonInfo->tuneId;
        footerDesc.button.icon    = buttonInfo->icon;
        footerDesc.button.style   = buttonInfo->style;
        return nbgl_layoutAddExtendedFooter(layout, &footerDesc);
    }

    button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
    obj = addCallbackObj(layoutInt, (nbgl_obj_t *) button, buttonInfo->token, buttonInfo->tuneId);
    if (obj == NULL) {
        return -1;
    }

#ifdef TARGET_STAX
    button->obj.alignmentMarginY = BORDER_MARGIN;
#else   // TARGET_STAX
    button->obj.alignmentMarginY = BOTTOM_BORDER_MARGIN;
#endif  // TARGET_STAX
    if (buttonInfo->onBottom != true) {
        button->obj.alignmentMarginX = BORDER_MARGIN;
        button->obj.alignment        = NO_ALIGNMENT;
    }
    else {
        button->obj.alignment = BOTTOM_MIDDLE;
    }
    if (buttonInfo->style == BLACK_BACKGROUND) {
        button->innerColor      = BLACK;
        button->foregroundColor = WHITE;
    }
    else {
        button->innerColor      = WHITE;
        button->foregroundColor = BLACK;
    }
    if (buttonInfo->style == NO_BORDER) {
        button->borderColor = WHITE;
    }
    else {
        if (buttonInfo->style == BLACK_BACKGROUND) {
            button->borderColor = BLACK;
        }
        else {
            button->borderColor = LIGHT_GRAY;
        }
    }
    button->text   = PIC(buttonInfo->text);
    button->fontId = SMALL_BOLD_FONT;
    button->icon   = PIC(buttonInfo->icon);
    if (buttonInfo->fittingContent == true) {
        button->obj.area.width = nbgl_getTextWidth(button->fontId, button->text)
                                 + SMALL_BUTTON_HEIGHT
                                 + ((button->icon) ? (button->icon->width + 8) : 0);
        button->obj.area.height = SMALL_BUTTON_HEIGHT;
        button->radius          = RADIUS_32_PIXELS;
        if (buttonInfo->onBottom != true) {
            button->obj.alignmentMarginX
                += (SCREEN_WIDTH - 2 * BORDER_MARGIN - button->obj.area.width) / 2;
        }
    }
    else {
        button->obj.area.width  = AVAILABLE_WIDTH;
        button->obj.area.height = BUTTON_DIAMETER;
        button->radius          = BUTTON_RADIUS;
    }
    button->obj.alignTo   = NULL;
    button->obj.touchMask = (1 << TOUCHED);
    button->obj.touchId   = SINGLE_BUTTON_ID;
    // set this new button as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) button);

    return 0;
}

/**
 * @brief Creates a long press button in the main container.
 *
 * @param layout the current layout
 * @param text text of the button button
 * @param token token attached to actionCallback when long time of press is elapsed
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is long pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddLongPressButton(nbgl_layout_t *layout,
                                  const char    *text,
                                  uint8_t        token,
                                  tune_index_e   tuneId)
{
    layoutObj_t           *obj;
    nbgl_button_t         *button;
    nbgl_text_area_t      *textArea;
    nbgl_progress_bar_t   *progressBar;
    nbgl_container_t      *container;
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_line_t           *line;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddLongPressButton():\n");
    if (layout == NULL) {
        return -1;
    }

    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    obj       = addCallbackObj(layoutInt, (nbgl_obj_t *) container, token, tuneId);
    if (obj == NULL) {
        return -1;
    }

    container->obj.area.width  = SCREEN_WIDTH;
    container->obj.area.height = 128;
    container->layout          = VERTICAL;
    container->nbChildren      = 4;  // progress-bar + text + line + button
    container->children
        = (nbgl_obj_t **) nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
    container->obj.alignment = BOTTOM_MIDDLE;
    container->obj.touchId   = LONG_PRESS_BUTTON_ID;
    container->obj.touchMask = ((1 << TOUCHING) | (1 << TOUCH_RELEASED) | (1 << OUT_OF_TOUCH));

    button                       = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
    button->obj.alignmentMarginX = BORDER_MARGIN;
    button->obj.alignment        = MID_RIGHT;
    button->innerColor           = BLACK;
    button->foregroundColor      = WHITE;
    button->borderColor          = BLACK;
    button->obj.area.width       = BUTTON_DIAMETER;
    button->obj.area.height      = BUTTON_DIAMETER;
    button->radius               = BUTTON_RADIUS;
    button->icon                 = PIC(&VALIDATE_ICON);
    container->children[0]       = (nbgl_obj_t *) button;

    textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor     = BLACK;
    textArea->text          = PIC(text);
    textArea->textAlignment = MID_LEFT;
    textArea->fontId        = LARGE_MEDIUM_FONT;
    textArea->wrapping      = true;
    textArea->obj.area.width
        = container->obj.area.width - 3 * BORDER_MARGIN - button->obj.area.width;
    textArea->obj.area.height = nbgl_getTextHeightInWidth(
        textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
    textArea->style                = NO_STYLE;
    textArea->obj.alignment        = MID_LEFT;
    textArea->obj.alignmentMarginX = BORDER_MARGIN;
    container->children[1]         = (nbgl_obj_t *) textArea;

    line                   = createHorizontalLine(layoutInt->layer);
    line->offset           = 3;
    line->obj.alignment    = TOP_MIDDLE;
    container->children[2] = (nbgl_obj_t *) line;

    progressBar = (nbgl_progress_bar_t *) nbgl_objPoolGet(PROGRESS_BAR, layoutInt->layer);
    progressBar->withBorder           = false;
    progressBar->obj.area.width       = container->obj.area.width;
    progressBar->obj.area.height      = 8;
    progressBar->obj.alignment        = TOP_MIDDLE;
    progressBar->obj.alignmentMarginY = 4;
    progressBar->obj.alignTo          = NULL;
    container->children[3]            = (nbgl_obj_t *) progressBar;

    // set this new container as child of the main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return 0;
}

/**
 * @brief Creates a touchable text at the footer of the screen, separated with a thin line from the
 * rest of the screen.
 *
 * @param layout the current layout
 * @param text text to used in the footer
 * @param token token to use when the footer is touched
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is long pressed
 * @return height of the control if OK
 */
int nbgl_layoutAddFooter(nbgl_layout_t *layout,
                         const char    *text,
                         uint8_t        token,
                         tune_index_e   tuneId)
{
    nbgl_layoutFooter_t footerDesc;
    footerDesc.type              = FOOTER_SIMPLE_TEXT;
    footerDesc.separationLine    = true;
    footerDesc.simpleText.text   = text;
    footerDesc.simpleText.token  = token;
    footerDesc.simpleText.tuneId = tuneId;
    return nbgl_layoutAddExtendedFooter(layout, &footerDesc);
}

/**
 * @brief Creates 2 touchable texts at the footer of the screen, separated with a thin line from the
 * rest of the screen, and from each other.
 *
 * @param layout the current layout
 * @param leftText text to used in the left part of footer
 * @param leftToken token to use when the left part of footer is touched
 * @param rightText text to used in the right part of footer
 * @param rightToken token to use when the right part of footer is touched
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is long pressed
 * @return height of the control if OK
 */
int nbgl_layoutAddSplitFooter(nbgl_layout_t *layout,
                              const char    *leftText,
                              uint8_t        leftToken,
                              const char    *rightText,
                              uint8_t        rightToken,
                              tune_index_e   tuneId)
{
    nbgl_layoutFooter_t footerDesc;
    footerDesc.type                  = FOOTER_DOUBLE_TEXT;
    footerDesc.separationLine        = true;
    footerDesc.doubleText.leftText   = leftText;
    footerDesc.doubleText.leftToken  = leftToken;
    footerDesc.doubleText.rightText  = rightText;
    footerDesc.doubleText.rightToken = rightToken;
    footerDesc.doubleText.tuneId     = tuneId;
    return nbgl_layoutAddExtendedFooter(layout, &footerDesc);
}

/**
 * @brief Creates a touchable (or not) area at the header of the screen, containing various
 * controls, described in the given structure. This header is not part of the main container
 *
 * @param layout the current layout
 * @param headerDesc description of the header to add
 * @return height of the control if OK
 */
int nbgl_layoutAddHeader(nbgl_layout_t *layout, const nbgl_layoutHeader_t *headerDesc)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    layoutObj_t           *obj;
    nbgl_text_area_t      *textArea;
    nbgl_line_t           *line, *separationLine = NULL;
    ;
    nbgl_button_t *button;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddHeader():\n");
    if (layout == NULL) {
        return -1;
    }
    if ((headerDesc == NULL) || (headerDesc->type >= NB_HEADER_TYPES)) {
        return -2;
    }

    layoutInt->headerContainer = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    layoutInt->headerContainer->obj.area.width = SCREEN_WIDTH;
    layoutInt->headerContainer->layout         = VERTICAL;
    layoutInt->headerContainer->children
        = (nbgl_obj_t **) nbgl_containerPoolGet(5, layoutInt->layer);
    layoutInt->headerContainer->obj.alignment = TOP_MIDDLE;

    switch (headerDesc->type) {
        case HEADER_EMPTY: {
            layoutInt->headerContainer->obj.area.height = headerDesc->emptySpace.height;
            break;
        }
        case HEADER_BACK_AND_TEXT: {
            // add back button
            button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
            obj    = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) button,
                                 headerDesc->backAndText.token,
                                 headerDesc->backAndText.tuneId);
            if (obj == NULL) {
                return -1;
            }

            button->obj.alignment   = MID_LEFT;
            button->innerColor      = WHITE;
            button->foregroundColor = BLACK;
            button->borderColor     = WHITE;
            button->obj.area.width  = BACK_KEY_WIDTH;
            button->obj.area.height = TOUCHABLE_HEADER_BAR_HEIGHT;
            button->text            = NULL;
#ifdef TARGET_STAX
            button->icon = PIC(&C_leftArrow32px);
#else   // TARGET_STAX
            button->icon                                = PIC(&C_leftArrow40px);
#endif  // TARGET_STAX
            button->obj.touchMask = (1 << TOUCHED);
            button->obj.touchId   = BACK_BUTTON_ID;
            layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
                = (nbgl_obj_t *) button;
            layoutInt->headerContainer->nbChildren++;

            // add optional text if needed
            if (headerDesc->backAndText.text != NULL) {
                textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
                textArea->obj.alignment = CENTER;
                textArea->textColor     = BLACK;
                textArea->obj.area.width
                    = layoutInt->headerContainer->obj.area.width - 2 * BACK_KEY_WIDTH;
                textArea->text          = PIC(headerDesc->backAndText.text);
                textArea->fontId        = SMALL_BOLD_FONT;
                textArea->textAlignment = CENTER;
                textArea->wrapping      = true;
                layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
                    = (nbgl_obj_t *) textArea;
                layoutInt->headerContainer->nbChildren++;
            }

            layoutInt->headerContainer->obj.area.height = TOUCHABLE_HEADER_BAR_HEIGHT;
            break;
        }
        case HEADER_BACK_AND_PROGRESS: {
#ifdef TARGET_STAX
            // add optional back button
            if (headerDesc->progressAndBack.withBack) {
                button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
                obj    = addCallbackObj(layoutInt,
                                     (nbgl_obj_t *) button,
                                     headerDesc->backAndText.token,
                                     headerDesc->backAndText.tuneId);
                if (obj == NULL) {
                    return -1;
                }

                button->obj.alignment   = MID_LEFT;
                button->innerColor      = WHITE;
                button->foregroundColor = BLACK;
                button->borderColor     = WHITE;
                button->obj.area.width  = BACK_KEY_WIDTH;
                button->obj.area.height = TOUCHABLE_HEADER_BAR_HEIGHT;
                button->text            = NULL;
                button->icon            = PIC(&C_leftArrow32px);
                button->obj.touchMask   = (1 << TOUCHED);
                button->obj.touchId     = BACK_BUTTON_ID;
                // add to container
                layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
                    = (nbgl_obj_t *) button;
                layoutInt->headerContainer->nbChildren++;
            }

            // add progress indicator
            if (headerDesc->progressAndBack.nbPages > 1) {
                nbgl_page_indicator_t *progress;

                progress
                    = (nbgl_page_indicator_t *) nbgl_objPoolGet(PAGE_INDICATOR, layoutInt->layer);
                progress->activePage     = headerDesc->progressAndBack.activePage;
                progress->nbPages        = headerDesc->progressAndBack.nbPages;
                progress->obj.area.width = 224;
                progress->obj.alignment  = CENTER;
                // add to container
                layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
                    = (nbgl_obj_t *) progress;
                layoutInt->headerContainer->nbChildren++;
            }
            layoutInt->activePage                       = headerDesc->progressAndBack.activePage;
            layoutInt->nbPages                          = headerDesc->progressAndBack.nbPages;
            layoutInt->headerContainer->obj.area.height = TOUCHABLE_HEADER_BAR_HEIGHT;
#endif  // TARGET_STAX
            break;
        }
        case HEADER_TITLE: {
            textArea            = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
            textArea->textColor = BLACK;
            textArea->obj.area.width  = AVAILABLE_WIDTH;
            textArea->obj.area.height = TOUCHABLE_HEADER_BAR_HEIGHT;
            textArea->text            = PIC(headerDesc->title.text);
            textArea->fontId          = SMALL_BOLD_FONT;
            textArea->textAlignment   = CENTER;
            textArea->wrapping        = true;
            layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
                = (nbgl_obj_t *) textArea;
            layoutInt->headerContainer->nbChildren++;
            layoutInt->headerContainer->obj.area.height = textArea->obj.area.height;
            break;
        }
        case HEADER_RIGHT_TEXT: {
            textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
            obj      = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) textArea,
                                 headerDesc->rightText.token,
                                 headerDesc->rightText.tuneId);
            if (obj == NULL) {
                return -1;
            }
            textArea->obj.alignment   = MID_RIGHT;
            textArea->textColor       = BLACK;
            textArea->obj.area.width  = 124;
            textArea->obj.area.height = 96;
            textArea->text            = PIC(headerDesc->rightText.text);
            textArea->fontId          = SMALL_BOLD_FONT;
            textArea->textAlignment   = CENTER;
            textArea->obj.touchMask   = (1 << TOUCHED);
            textArea->obj.touchId     = RIGHT_BUTTON_ID;
            // add to bottom container
            layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
                = (nbgl_obj_t *) textArea;
            layoutInt->headerContainer->nbChildren++;
            layoutInt->headerContainer->obj.area.height = textArea->obj.area.height;

            // create vertical line separating texts
            separationLine            = (nbgl_line_t *) nbgl_objPoolGet(LINE, layoutInt->layer);
            separationLine->lineColor = LIGHT_GRAY;
            separationLine->obj.area.width       = 1;
            separationLine->obj.area.height      = layoutInt->headerContainer->obj.area.height;
            separationLine->direction            = VERTICAL;
            separationLine->thickness            = 1;
            separationLine->obj.alignment        = MID_LEFT;
            separationLine->obj.alignTo          = (nbgl_obj_t *) textArea;
            separationLine->obj.alignmentMarginX = -1;
            layoutInt->headerContainer->obj.area.height = textArea->obj.area.height;
            break;
        }
        default:
            return -2;
    }

    if (headerDesc->separationLine) {
        line                = createHorizontalLine(layoutInt->layer);
        line->obj.alignment = BOTTOM_MIDDLE;
        line->offset        = 3;
        layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
            = (nbgl_obj_t *) line;
        layoutInt->headerContainer->nbChildren++;
    }
    if (separationLine != NULL) {
        layoutInt->headerContainer->children[layoutInt->headerContainer->nbChildren]
            = (nbgl_obj_t *) separationLine;
        layoutInt->headerContainer->nbChildren++;
    }
    // header must be the first child
    layoutInt->children[0] = (nbgl_obj_t *) layoutInt->headerContainer;

    // subtract header height from main container height
    layoutInt->container->obj.area.height -= layoutInt->headerContainer->obj.area.height;

    layoutInt->headerType = headerDesc->type;

    return layoutInt->headerContainer->obj.area.height;
}

/**
 * @brief Creates a touchable area at the footer of the screen, containing various controls,
 * described in the given structure. This footer is not part of the main container
 *
 * @param layout the current layout
 * @param footerDesc if not @ref NBGL_NO_TUNE, a tune will be played when button is long pressed
 * @return height of the control if OK
 */
int nbgl_layoutAddExtendedFooter(nbgl_layout_t *layout, const nbgl_layoutFooter_t *footerDesc)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    layoutObj_t           *obj;
    nbgl_text_area_t      *textArea;
    nbgl_line_t           *line, *separationLine = NULL;
    nbgl_button_t         *button;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddExtendedFooter():\n");
    if (layout == NULL) {
        return -1;
    }
    if ((footerDesc == NULL) || (footerDesc->type >= NB_FOOTER_TYPES)) {
        return -2;
    }

    layoutInt->footerContainer = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    layoutInt->footerContainer->obj.area.width = AVAILABLE_WIDTH;
    layoutInt->footerContainer->layout         = VERTICAL;
    layoutInt->footerContainer->children
        = (nbgl_obj_t **) nbgl_containerPoolGet(5, layoutInt->layer);
    layoutInt->footerContainer->obj.alignment = BOTTOM_MIDDLE;

    switch (footerDesc->type) {
        case FOOTER_EMPTY: {
            layoutInt->footerContainer->obj.area.height = footerDesc->emptySpace.height;
            break;
        }
        case FOOTER_SIMPLE_TEXT: {
            textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
            obj      = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) textArea,
                                 footerDesc->simpleText.token,
                                 footerDesc->simpleText.tuneId);
            if (obj == NULL) {
                return -1;
            }

            textArea->obj.alignment  = BOTTOM_MIDDLE;
            textArea->textColor      = BLACK;
            textArea->obj.area.width = AVAILABLE_WIDTH;
#ifdef TARGET_STAX
            textArea->obj.area.height = 88;
#else   // TARGET_STAX
            textArea->obj.area.height                   = 96;
#endif  // TARGET_STAX
            textArea->text          = PIC(footerDesc->simpleText.text);
            textArea->fontId        = SMALL_BOLD_FONT;
            textArea->textAlignment = CENTER;
            textArea->obj.touchMask = (1 << TOUCHED);
            textArea->obj.touchId   = BOTTOM_BUTTON_ID;
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) textArea;
            layoutInt->footerContainer->nbChildren++;
            layoutInt->footerContainer->obj.area.height = textArea->obj.area.height;
            break;
        }
        case FOOTER_DOUBLE_TEXT: {
            textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
            obj      = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) textArea,
                                 footerDesc->doubleText.leftToken,
                                 footerDesc->doubleText.tuneId);
            if (obj == NULL) {
                return -1;
            }
            textArea->obj.alignment  = BOTTOM_LEFT;
            textArea->textColor      = BLACK;
            textArea->obj.area.width = AVAILABLE_WIDTH / 2;
#ifdef TARGET_STAX
            textArea->obj.area.height = 88;
#else   // TARGET_STAX
            textArea->obj.area.height                   = 96;
#endif  // TARGET_STAX
            textArea->text          = PIC(footerDesc->doubleText.leftText);
            textArea->fontId        = SMALL_BOLD_FONT;
            textArea->textAlignment = CENTER;
            textArea->obj.touchMask = (1 << TOUCHED);
            textArea->obj.touchId   = BOTTOM_BUTTON_ID;
            // add to bottom container
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) textArea;
            layoutInt->footerContainer->nbChildren++;

            // create right touchable text
            textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
            obj      = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) textArea,
                                 footerDesc->doubleText.rightToken,
                                 footerDesc->doubleText.tuneId);
            if (obj == NULL) {
                return -1;
            }

            textArea->obj.alignment  = BOTTOM_RIGHT;
            textArea->textColor      = BLACK;
            textArea->obj.area.width = AVAILABLE_WIDTH / 2;
#ifdef TARGET_STAX
            textArea->obj.area.height = 88;
#else   // TARGET_STAX
            textArea->obj.area.height                   = 96;
#endif  // TARGET_STAX
            textArea->text          = PIC(footerDesc->doubleText.rightText);
            textArea->fontId        = SMALL_BOLD_FONT;
            textArea->textAlignment = CENTER;
            textArea->obj.touchMask = (1 << TOUCHED);
            textArea->obj.touchId   = RIGHT_BUTTON_ID;
            // add to bottom container
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) textArea;
            layoutInt->footerContainer->nbChildren++;
            layoutInt->footerContainer->obj.area.height = textArea->obj.area.height;

            // create vertical line separating texts
            separationLine            = (nbgl_line_t *) nbgl_objPoolGet(LINE, layoutInt->layer);
            separationLine->lineColor = LIGHT_GRAY;
            separationLine->obj.area.width       = 1;
            separationLine->obj.area.height      = layoutInt->footerContainer->obj.area.height;
            separationLine->direction            = VERTICAL;
            separationLine->thickness            = 1;
            separationLine->obj.alignment        = MID_LEFT;
            separationLine->obj.alignTo          = (nbgl_obj_t *) textArea;
            separationLine->obj.alignmentMarginY = -1;
            break;
        }
        case FOOTER_TEXT_AND_NAV: {
#ifndef TARGET_STAX
            layoutInt->footerContainer->obj.area.width  = SCREEN_WIDTH;
            layoutInt->footerContainer->obj.area.height = 96;
            // add touchable text on the left
            textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
            obj      = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) textArea,
                                 footerDesc->textAndNav.token,
                                 footerDesc->textAndNav.tuneId);
            if (obj == NULL) {
                return -1;
            }
            textArea->obj.alignment   = BOTTOM_LEFT;
            textArea->textColor       = BLACK;
            textArea->obj.area.width  = 192;
            textArea->obj.area.height = 96;
            textArea->text            = PIC(footerDesc->textAndNav.text);
            textArea->fontId          = SMALL_BOLD_FONT;
            textArea->textAlignment   = CENTER;
            textArea->obj.touchMask   = (1 << TOUCHED);
            textArea->obj.touchId     = BOTTOM_BUTTON_ID;
            // add to bottom container
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) textArea;
            layoutInt->footerContainer->nbChildren++;

            // add navigation on the right
            nbgl_container_t *navContainer
                = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
            navContainer->obj.area.width = AVAILABLE_WIDTH;
            navContainer->layout         = VERTICAL;
            navContainer->nbChildren     = 4;
            navContainer->children
                = (nbgl_obj_t **) nbgl_containerPoolGet(navContainer->nbChildren, layoutInt->layer);
            navContainer->obj.alignment   = BOTTOM_RIGHT;
            navContainer->obj.area.width  = SCREEN_WIDTH - textArea->obj.area.width;
            navContainer->obj.area.height = 96;
            nbgl_navigationPopulate(navContainer,
                                    footerDesc->textAndNav.navigation.nbPages,
                                    footerDesc->textAndNav.navigation.activePage,
                                    footerDesc->textAndNav.navigation.withExitKey,
                                    footerDesc->textAndNav.navigation.withBackKey,
                                    true,
                                    layoutInt->layer);
            obj = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) navContainer,
                                 footerDesc->textAndNav.navigation.token,
                                 footerDesc->textAndNav.navigation.tuneId);
            if (obj == NULL) {
                return -1;
            }

            // create vertical line separating text from nav
            separationLine            = (nbgl_line_t *) nbgl_objPoolGet(LINE, layoutInt->layer);
            separationLine->lineColor = LIGHT_GRAY;
            separationLine->obj.area.width       = 1;
            separationLine->obj.area.height      = layoutInt->footerContainer->obj.area.height;
            separationLine->direction            = VERTICAL;
            separationLine->thickness            = 1;
            separationLine->obj.alignment        = MID_LEFT;
            separationLine->obj.alignTo          = (nbgl_obj_t *) navContainer;
            separationLine->obj.alignmentMarginY = -1;

            layoutInt->activePage = footerDesc->textAndNav.navigation.activePage;
            layoutInt->nbPages    = footerDesc->textAndNav.navigation.nbPages;
            // add to bottom container
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) navContainer;
            layoutInt->footerContainer->nbChildren++;
#endif  // TARGET_STAX
            break;
        }
        case FOOTER_NAV: {
#ifdef TARGET_STAX
            layoutInt->footerContainer->obj.area.height = 128;
#else   // TARGET_STAX
            layoutInt->footerContainer->obj.area.width  = SCREEN_WIDTH;
            layoutInt->footerContainer->obj.area.height = 96;
#endif  // TARGET_STAX
            nbgl_navigationPopulate(layoutInt->footerContainer,
                                    footerDesc->navigation.nbPages,
                                    footerDesc->navigation.activePage,
                                    footerDesc->navigation.withExitKey,
                                    footerDesc->navigation.withBackKey,
                                    false,
                                    layoutInt->layer);
            layoutInt->footerContainer->nbChildren = 4;
            obj                                    = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) layoutInt->footerContainer,
                                 footerDesc->navigation.token,
                                 footerDesc->navigation.tuneId);
            if (obj == NULL) {
                return -1;
            }

            layoutInt->activePage = footerDesc->navigation.activePage;
            layoutInt->nbPages    = footerDesc->navigation.nbPages;
            break;
        }
        case FOOTER_SIMPLE_BUTTON: {
            button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
            obj    = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) button,
                                 footerDesc->button.token,
                                 footerDesc->button.tuneId);
            if (obj == NULL) {
                return -1;
            }

            button->obj.alignment = CENTER;
            if (footerDesc->button.style == BLACK_BACKGROUND) {
                button->innerColor      = BLACK;
                button->foregroundColor = WHITE;
            }
            else {
                button->innerColor      = WHITE;
                button->foregroundColor = BLACK;
            }

            if (footerDesc->button.style == NO_BORDER) {
                button->borderColor = WHITE;
            }
            else {
                if (footerDesc->button.style == BLACK_BACKGROUND) {
                    button->borderColor = BLACK;
                }
                else {
                    button->borderColor = LIGHT_GRAY;
                }
            }
            button->text            = PIC(footerDesc->button.text);
            button->fontId          = SMALL_BOLD_FONT;
            button->icon            = PIC(footerDesc->button.icon);
            button->radius          = BUTTON_RADIUS;
            button->obj.area.height = BUTTON_DIAMETER;
            if (footerDesc->button.text == NULL) {
                button->obj.area.width = BUTTON_DIAMETER;
            }
            else {
                button->obj.area.width = AVAILABLE_WIDTH;
            }
            button->obj.touchMask = (1 << TOUCHED);
            button->obj.touchId   = button->text ? SINGLE_BUTTON_ID : BOTTOM_BUTTON_ID;
            // add to bottom container
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) button;
            layoutInt->footerContainer->nbChildren++;
#ifdef TARGET_STAX
            layoutInt->footerContainer->obj.area.height = 128;
#else   // TARGET_STAX
            layoutInt->footerContainer->obj.area.height = 136;
#endif  // TARGET_STAX
            break;
        }
        case FOOTER_CHOICE_BUTTONS: {
            // texts cannot be NULL
            if ((footerDesc->choiceButtons.bottomText == NULL)
                || (footerDesc->choiceButtons.topText == NULL)) {
                return -1;
            }

            // create bottomButton (in white) at first
            button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
            obj    = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) button,
                                 footerDesc->choiceButtons.token,
                                 footerDesc->choiceButtons.tuneId);
            if (obj == NULL) {
                return -1;
            }
            // associate with with index 1
            obj->index = 1;
            // put at the bottom of the container
            button->obj.alignment = BOTTOM_MIDDLE;
            if (footerDesc->choiceButtons.style == ROUNDED_AND_FOOTER_STYLE) {
                button->obj.alignmentMarginY = 4;  // 4 pixels from screen bottom
                button->borderColor          = WHITE;
            }
            else if (footerDesc->choiceButtons.style == BOTH_ROUNDED_STYLE) {
#ifdef TARGET_STAX
                button->obj.alignmentMarginY
                    = BOTTOM_BORDER_MARGIN;  // 24 pixels from screen bottom
                button->borderColor = LIGHT_GRAY;
#else   // TARGET_STAX
                button->obj.alignmentMarginY = 4;      // 4 pixels from screen bottom
                button->borderColor          = WHITE;  // not a real round button on Europa
#endif  // TARGET_STAX
            }
            button->innerColor      = WHITE;
            button->foregroundColor = BLACK;
            button->obj.area.width  = AVAILABLE_WIDTH;
            button->obj.area.height = BUTTON_DIAMETER;
            button->radius          = BUTTON_RADIUS;
            button->text            = PIC(footerDesc->choiceButtons.bottomText);
            button->fontId          = SMALL_BOLD_FONT;
            button->obj.touchMask   = (1 << TOUCHED);
            button->obj.touchId     = CHOICE_2_ID;
            // add to bottom container
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) button;
            layoutInt->footerContainer->nbChildren++;

#ifndef TARGET_STAX
            // add line if needed
            if (footerDesc->choiceButtons.style == BOTH_ROUNDED_STYLE) {
                line                       = createHorizontalLine(layoutInt->layer);
                line->obj.alignment        = TOP_MIDDLE;
                line->obj.alignmentMarginY = 4;
                line->obj.alignTo          = (nbgl_obj_t *) button;
                layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                    = (nbgl_obj_t *) line;
                layoutInt->footerContainer->nbChildren++;
            }
#endif  // TARGET_STAX

            // then black button, on top of it
            button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
            obj    = addCallbackObj(layoutInt,
                                 (nbgl_obj_t *) button,
                                 footerDesc->choiceButtons.token,
                                 footerDesc->choiceButtons.tuneId);
            if (obj == NULL) {
                return -1;
            }
            // associate with with index 0
            obj->index            = 0;
            button->obj.alignment = TOP_MIDDLE;
#ifdef TARGET_STAX
            if (footerDesc->choiceButtons.style == BOTH_ROUNDED_STYLE) {
                button->obj.alignmentMarginY = 0;  // 0 pixels from top of container
            }
            else {
                button->obj.alignmentMarginY = 4;  // 4 pixels from top of container
            }
            button->innerColor      = BLACK;
            button->borderColor     = BLACK;
            button->foregroundColor = WHITE;
#else   // TARGET_STAX
            button->obj.alignmentMarginY = 24;         // 12 pixels from bottom button
            if (footerDesc->choiceButtons.style == BOTH_ROUNDED_STYLE) {
                button->innerColor      = WHITE;
                button->borderColor     = LIGHT_GRAY;
                button->foregroundColor = BLACK;
            }
            else {
                button->innerColor      = BLACK;
                button->borderColor     = BLACK;
                button->foregroundColor = WHITE;
            }
#endif  // TARGET_STAX
            button->obj.area.width  = AVAILABLE_WIDTH;
            button->obj.area.height = BUTTON_DIAMETER;
            button->radius          = BUTTON_RADIUS;
            button->text            = PIC(footerDesc->choiceButtons.topText);
            button->fontId          = SMALL_BOLD_FONT;
            button->obj.touchMask   = (1 << TOUCHED);
            button->obj.touchId     = CHOICE_1_ID;
            // add to bottom container
            layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
                = (nbgl_obj_t *) button;
            layoutInt->footerContainer->nbChildren++;

#ifdef TARGET_STAX
            layoutInt->footerContainer->obj.area.height = 168;
#else   // TARGET_STAX
            if (footerDesc->choiceButtons.style == BOTH_ROUNDED_STYLE) {
                layoutInt->footerContainer->obj.area.height = 232;
            }
            else {
                layoutInt->footerContainer->obj.area.height = 208;
            }
#endif  // TARGET_STAX

            break;
        }
        default:
            return -2;
    }
#ifndef TARGET_STAX
    // add swipable feature for navigation
    if ((footerDesc->type == FOOTER_NAV) || (footerDesc->type == FOOTER_TEXT_AND_NAV)) {
        addSwipeInternal(layoutInt,
                         ((1 << SWIPED_LEFT) | (1 << SWIPED_RIGHT)),
                         SWIPE_USAGE_NAVIGATION,
                         (footerDesc->type == FOOTER_NAV) ? footerDesc->navigation.token
                                                          : footerDesc->textAndNav.navigation.token,
                         (footerDesc->type == FOOTER_NAV)
                             ? footerDesc->navigation.tuneId
                             : footerDesc->textAndNav.navigation.tuneId);
    }
#endif  // TARGET_STAX

    if (footerDesc->separationLine) {
        line                = createHorizontalLine(layoutInt->layer);
        line->obj.alignment = TOP_MIDDLE;
        layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
            = (nbgl_obj_t *) line;
        layoutInt->footerContainer->nbChildren++;
    }
    if (separationLine != NULL) {
        layoutInt->footerContainer->children[layoutInt->footerContainer->nbChildren]
            = (nbgl_obj_t *) separationLine;
        layoutInt->footerContainer->nbChildren++;
    }

    layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t *) layoutInt->footerContainer;
    layoutInt->nbChildren++;

    // subtract footer height from main container height
    layoutInt->container->obj.area.height -= layoutInt->footerContainer->obj.area.height;

    layoutInt->footerType = footerDesc->type;

    return layoutInt->footerContainer->obj.area.height;
}

/**
 * @brief Creates a kind of navigation bar with an optional <- arrow on the left. This widget is
 * placed on top of the main container
 *
 * @param layout the current layout
 * @param activePage current page [O,(nbPages-1)]
 * @param nbPages number of pages
 * @param withBack if true, the back arrow is drawn
 * @param backToken token used with actionCallback is withBack is true
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when back button is pressed
 * @return the height of the control if OK
 */
int nbgl_layoutAddProgressIndicator(nbgl_layout_t *layout,
                                    uint8_t        activePage,
                                    uint8_t        nbPages,
                                    bool           withBack,
                                    uint8_t        backToken,
                                    tune_index_e   tuneId)
{
    nbgl_layoutHeader_t headerDesc = {.type                       = HEADER_BACK_AND_PROGRESS,
                                      .separationLine             = false,
                                      .progressAndBack.activePage = activePage,
                                      .progressAndBack.nbPages    = nbPages,
                                      .progressAndBack.token      = backToken,
                                      .progressAndBack.tuneId     = tuneId,
                                      .progressAndBack.withBack   = withBack};
    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddProgressIndicator():\n");

    return nbgl_layoutAddHeader(layout, &headerDesc);
}

/**
 * @brief Creates a centered (vertically & horizontally) spinner with a text under it
 *
 * @param layout the current layout
 * @param text text to draw under the spinner
 * @param fixed if set to true, the spinner won't spin and be entirely black
 * @return >= 0 if OK
 */
int nbgl_layoutAddSpinner(nbgl_layout_t *layout, const char *text, bool fixed)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_area_t      *textArea;
    nbgl_spinner_t        *spinner;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddSpinner():\n");
    if (layout == NULL) {
        return -1;
    }

    // create spinner
    spinner                       = (nbgl_spinner_t *) nbgl_objPoolGet(SPINNER, layoutInt->layer);
    spinner->position             = fixed ? 0xFF : 0;
    spinner->obj.alignmentMarginY = -20;
    spinner->obj.alignTo          = NULL;
    spinner->obj.alignment        = CENTER;
    // set this new spinner as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) spinner);

    // create text area
    textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor     = BLACK;
    textArea->text          = PIC(text);
    textArea->textAlignment = CENTER;
    textArea->fontId        = SMALL_REGULAR_FONT;
    textArea->wrapping      = true;
#ifdef TARGET_STAX
    textArea->obj.alignmentMarginY = 20;
#else   // TARGET_STAX
    textArea->obj.alignmentMarginY = 24;
#endif  // TARGET_STAX
    textArea->obj.alignTo     = (nbgl_obj_t *) spinner;
    textArea->obj.alignment   = BOTTOM_MIDDLE;
    textArea->obj.area.width  = AVAILABLE_WIDTH;
    textArea->obj.area.height = nbgl_getTextHeightInWidth(
        textArea->fontId, textArea->text, textArea->obj.area.width, textArea->wrapping);
    textArea->style = NO_STYLE;

    // center spinner + text vertically
    spinner->obj.alignmentMarginY
        = -(textArea->obj.alignmentMarginY + textArea->obj.area.height) / 2;

    // set this new spinner as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);

    if (!fixed) {
        // update ticker to update the spinner periodically
        nbgl_screenTickerConfiguration_t tickerCfg;

        tickerCfg.tickerIntervale = SPINNER_REFRESH_PERIOD;  // ms
        tickerCfg.tickerValue     = SPINNER_REFRESH_PERIOD;  // ms
        tickerCfg.tickerCallback  = &spinnerTickerCallback;
        nbgl_screenUpdateTicker(layoutInt->layer, &tickerCfg);
    }

    return 0;
}
#endif  // HAVE_SE_TOUCH

#ifdef NBGL_KEYBOARD
#ifdef HAVE_SE_TOUCH
/**
 * @brief Creates a keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param kbdInfo configuration of the keyboard to draw (including the callback when touched)
 * @return the index of keyboard, to use in @ref nbgl_layoutUpdateKeyboard()
 */
int nbgl_layoutAddKeyboard(nbgl_layout_t *layout, const nbgl_layoutKbd_t *kbdInfo)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keyboard_t       *keyboard;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddKeyboard():\n");
    if (layout == NULL) {
        return -1;
    }

    // create keyboard
    keyboard = (nbgl_keyboard_t *) nbgl_objPoolGet(KEYBOARD, layoutInt->layer);
#ifdef TARGET_STAX
    keyboard->obj.alignmentMarginY = 64;
#endif  // TARGET_STAX
    keyboard->obj.alignment = BOTTOM_MIDDLE;
    keyboard->borderColor   = LIGHT_GRAY;
    keyboard->callback      = PIC(kbdInfo->callback);
    keyboard->lettersOnly   = kbdInfo->lettersOnly;
    keyboard->mode          = kbdInfo->mode;
    keyboard->keyMask       = kbdInfo->keyMask;
    keyboard->casing        = kbdInfo->casing;
    // set this new keyboard as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) keyboard);

    // return index of keyboard to be modified later on
    return (layoutInt->container->nbChildren - 1);
}

/**
 * @brief Updates an existing keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeyboard()
 * @param keyMask mask of keys to activate/deactivate on keyboard
 * @param updateCasing if true, update keyboard casing with given value
 * @param casing  casing to use
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeyboard(nbgl_layout_t *layout,
                              uint8_t        index,
                              uint32_t       keyMask,
                              bool           updateCasing,
                              keyboardCase_t casing)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keyboard_t       *keyboard;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateKeyboard(): keyMask = 0x%X\n", keyMask);
    if (layout == NULL) {
        return -1;
    }

    // get keyboard at given index
    keyboard = (nbgl_keyboard_t *) layoutInt->container->children[index];
    if ((keyboard == NULL) || (keyboard->obj.type != KEYBOARD)) {
        return -1;
    }
    keyboard->keyMask = keyMask;
    if (updateCasing) {
        keyboard->casing = casing;
    }

    nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);

    return 0;
}

/**
 * @brief function called to know whether the keyboard has been redrawn and needs a refresh
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeyboard()
 * @return true if keyboard needs a refresh
 */
bool nbgl_layoutKeyboardNeedsRefresh(nbgl_layout_t *layout, uint8_t index)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keyboard_t       *keyboard;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutKeyboardNeedsRefresh(): \n");
    if (layout == NULL) {
        return -1;
    }

    // get keyboard at given index
    keyboard = (nbgl_keyboard_t *) layoutInt->container->children[index];
    if ((keyboard == NULL) || (keyboard->obj.type != KEYBOARD)) {
        return -1;
    }
    if (keyboard->needsRefresh) {
        keyboard->needsRefresh = false;
        return true;
    }

    return false;
}

/**
 * @brief Adds up to 4 black suggestion buttons under the previously added object
 *
 * @param layout the current layout
 * @param nbUsedButtons the number of actually used buttons
 * @param buttonTexts array of 4 strings for buttons (last ones can be NULL)
 * @param firstButtonToken first token used for buttons, provided in onActionCallback (the next 3
 * values will be used for other buttons)
 * @param tuneId tune to play when any button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddSuggestionButtons(nbgl_layout_t *layout,
                                    uint8_t        nbUsedButtons,
                                    const char    *buttonTexts[NB_MAX_SUGGESTION_BUTTONS],
                                    int            firstButtonToken,
                                    tune_index_e   tuneId)
{
    layoutObj_t           *obj;
    nbgl_container_t      *container;
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddSuggestionButtons():\n");
    if (layout == NULL) {
        return -1;
    }

    nbActiveButtons           = nbUsedButtons;
    container                 = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    container->layout         = VERTICAL;
    container->obj.area.width = SCREEN_WIDTH;
#ifdef TARGET_STAX
    // 2 rows of buttons with radius=32, and a intervale of 8px
    container->obj.area.height = 2 * SMALL_BUTTON_HEIGHT + INTERNAL_MARGIN;
    container->nbChildren      = nbUsedButtons;
    container->children = (nbgl_obj_t **) nbgl_containerPoolGet(NB_MAX_VISIBLE_SUGGESTION_BUTTONS,
                                                                layoutInt->layer);
#else   // TARGET_STAX
    // 1 row of buttons + 24px + page indicator
    container->obj.area.height = SMALL_BUTTON_HEIGHT + 28;
    // on Europa, the first child is used by the progress indicator, if more that 2 buttons
    container->nbChildren = nbUsedButtons + FIRST_BUTTON_INDEX;
    container->children   = (nbgl_obj_t **) nbgl_containerPoolGet(
        NB_MAX_VISIBLE_SUGGESTION_BUTTONS + 1, layoutInt->layer);

    // the container is swipable on Europa
    container->obj.touchMask = (1 << SWIPED_LEFT) | (1 << SWIPED_RIGHT);
    container->obj.touchId   = CONTROLS_ID;  // TODO: change this value
    obj                      = addCallbackObj(layoutInt, (nbgl_obj_t *) container, 0, NBGL_NO_TUNE);
    if (obj == NULL) {
        return -1;
    }
#endif  // TARGET_STAX
    container->obj.alignmentMarginY = 24;
    // align this control on top of keyboard (that must have been added just before)
    container->obj.alignment = TOP_MIDDLE;
    container->obj.alignTo   = layoutInt->container->children[layoutInt->container->nbChildren - 1];

    // create all possible suggestion buttons, even if not displayed at first
    nbgl_objPoolGetArray(BUTTON, NB_MAX_SUGGESTION_BUTTONS, 0, (nbgl_obj_t **) &choiceButtons);
    for (int i = 0; i < NB_MAX_SUGGESTION_BUTTONS; i++) {
        obj = addCallbackObj(
            layoutInt, (nbgl_obj_t *) choiceButtons[i], firstButtonToken + i, tuneId);
        if (obj == NULL) {
            return -1;
        }

        choiceButtons[i]->innerColor      = BLACK;
        choiceButtons[i]->borderColor     = BLACK;
        choiceButtons[i]->foregroundColor = WHITE;
        choiceButtons[i]->obj.area.width = (SCREEN_WIDTH - 2 * BORDER_MARGIN - INTERNAL_MARGIN) / 2;
        choiceButtons[i]->obj.area.height = SMALL_BUTTON_HEIGHT;
        choiceButtons[i]->radius          = RADIUS_32_PIXELS;
        choiceButtons[i]->fontId          = SMALL_BOLD_1BPP_FONT;
        choiceButtons[i]->icon            = NULL;
        if ((i % 2) == 0) {
#ifdef TARGET_STAX
            choiceButtons[i]->obj.alignmentMarginX = BORDER_MARGIN;
            // second row 8px under the first one
            if (i != 0) {
                choiceButtons[i]->obj.alignmentMarginY = INTERNAL_MARGIN;
            }
            choiceButtons[i]->obj.alignment = NO_ALIGNMENT;
#else   // TARGET_STAX
            choiceButtons[i]->obj.alignmentMarginX = BORDER_MARGIN + INTERNAL_MARGIN;
            if (i == 0) {
                choiceButtons[i]->obj.alignment = TOP_LEFT;
            }
#endif  // TARGET_STAX
        }
        else {
            choiceButtons[i]->obj.alignmentMarginX = INTERNAL_MARGIN;
            choiceButtons[i]->obj.alignment        = MID_RIGHT;
            choiceButtons[i]->obj.alignTo          = (nbgl_obj_t *) choiceButtons[i - 1];
        }
        choiceButtons[i]->text          = buttonTexts[i];
        choiceButtons[i]->obj.touchMask = (1 << TOUCHED);
        choiceButtons[i]->obj.touchId   = CONTROLS_ID + i;
        // some buttons may not be visible
        if (i < MIN(NB_MAX_VISIBLE_SUGGESTION_BUTTONS, nbActiveButtons)) {
            container->children[i + FIRST_BUTTON_INDEX] = (nbgl_obj_t *) choiceButtons[i];
        }
    }
#ifndef TARGET_STAX
    // on Europa, the first child is used by the progress indicator, if more that 2 buttons
    nbgl_page_indicator_t *indicator
        = (nbgl_page_indicator_t *) nbgl_objPoolGet(PAGE_INDICATOR, layoutInt->layer);
    indicator->activePage                     = 0;
    indicator->nbPages                        = (nbUsedButtons + 1) / 2;
    indicator->obj.area.width                 = 184;
    indicator->obj.alignment                  = BOTTOM_MIDDLE;
    indicator->style                          = CURRENT_INDICATOR;
    container->children[PAGE_INDICATOR_INDEX] = (nbgl_obj_t *) indicator;
    // also allocate the semi disc that may be displayed on the left or right of the full buttons
    nbgl_objPoolGetArray(IMAGE, 2, 0, (nbgl_obj_t **) &partialButtonImages);
    partialButtonImages[0]->buffer          = &C_left_half_64px;
    partialButtonImages[0]->obj.alignment   = TOP_LEFT;
    partialButtonImages[0]->foregroundColor = BLACK;
    partialButtonImages[0]->transformation  = VERTICAL_MIRROR;
    partialButtonImages[1]->buffer          = &C_left_half_64px;
    partialButtonImages[1]->obj.alignment   = TOP_RIGHT;
    partialButtonImages[1]->foregroundColor = BLACK;
    partialButtonImages[1]->transformation  = NO_TRANSFORMATION;
    updateSuggestionButtons(container, 0, 0);
#endif  // TARGET_STAX
    // set this new container as child of the main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    // return index of container to be modified later on
    return (layoutInt->container->nbChildren - 1);
}

/**
 * @brief Updates the number and/or the text suggestion buttons created with @ref
 * nbgl_layoutAddSuggestionButtons()
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddSuggestionButtons()
 * @param nbUsedButtons the number of actually used buttons
 * @param buttonTexts array of 4 strings for buttons (last ones can be NULL)
 * @return >= 0 if OK
 */
int nbgl_layoutUpdateSuggestionButtons(nbgl_layout_t *layout,
                                       uint8_t        index,
                                       uint8_t        nbUsedButtons,
                                       const char    *buttonTexts[NB_MAX_SUGGESTION_BUTTONS])
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateSuggestionButtons():\n");
    if (layout == NULL) {
        return -1;
    }

    container = (nbgl_container_t *) layoutInt->container->children[index];
    if ((container == NULL) || (container->obj.type != CONTAINER)) {
        return -1;
    }
    nbActiveButtons       = nbUsedButtons;
    container->nbChildren = nbUsedButtons + FIRST_BUTTON_INDEX;

    // update suggestion buttons
    for (int i = 0; i < NB_MAX_SUGGESTION_BUTTONS; i++) {
        choiceButtons[i]->text = buttonTexts[i];
        // some buttons may not be visible
        if (i < MIN(NB_MAX_VISIBLE_SUGGESTION_BUTTONS, nbUsedButtons)) {
            if ((i % 2) == 0) {
                choiceButtons[i]->obj.alignmentMarginX = BORDER_MARGIN;
#ifdef TARGET_STAX
                // second row 8px under the first one
                if (i != 0) {
                    choiceButtons[i]->obj.alignmentMarginY = INTERNAL_MARGIN;
                }
                choiceButtons[i]->obj.alignment = NO_ALIGNMENT;
#else   // TARGET_STAX
                if (i == 0) {
                    choiceButtons[i]->obj.alignment = TOP_LEFT;
                }
#endif  // TARGET_STAX
            }
            else {
                choiceButtons[i]->obj.alignmentMarginX = INTERNAL_MARGIN;
                choiceButtons[i]->obj.alignment        = MID_RIGHT;
                choiceButtons[i]->obj.alignTo          = (nbgl_obj_t *) choiceButtons[i - 1];
            }
            container->children[i + FIRST_BUTTON_INDEX] = (nbgl_obj_t *) choiceButtons[i];
        }
        else {
            container->children[i + FIRST_BUTTON_INDEX] = NULL;
        }
    }
    container->forceClean = true;
#ifndef TARGET_STAX
    // on Europa, the first child is used by the progress indicator, if more that 2 buttons
    nbgl_page_indicator_t *indicator
        = (nbgl_page_indicator_t *) container->children[PAGE_INDICATOR_INDEX];
    indicator->nbPages    = (nbUsedButtons + 1) / 2;
    indicator->activePage = 0;
    updateSuggestionButtons(container, 0, 0);
#endif  // TARGET_STAX

    nbgl_redrawObject((nbgl_obj_t *) container, NULL, false);

    return 0;
}

/**
 * @brief Adds a "text entry" area under the previously entered object. This area can be preceded
 * (beginning of line) by an index, indicating for example the entered world. A vertical gray line
 * is placed under the text. This text must be vertical placed in the screen with offsetY
 *
 * @note This area is touchable
 *
 * @param layout the current layout
 * @param numbered if true, the "number" param is used as index
 * @param number index of the text
 * @param text string to display in the area
 * @param grayedOut if true, the text is grayed out (but not the potential number)
 * @param offsetY vertical offset from the top of the page
 * @param token token provided in onActionCallback when this area is touched
 * @return >= 0 if OK
 */
int nbgl_layoutAddEnteredText(nbgl_layout_t *layout,
                              bool           numbered,
                              uint8_t        number,
                              const char    *text,
                              bool           grayedOut,
                              int            offsetY,
                              int            token)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_area_t      *textArea;
    nbgl_line_t           *line;
    layoutObj_t           *obj;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddEnteredText():\n");
    if (layout == NULL) {
        return -1;
    }

    // create gray line
    line                       = (nbgl_line_t *) nbgl_objPoolGet(LINE, layoutInt->layer);
    line->lineColor            = LIGHT_GRAY;
    line->obj.alignmentMarginY = offsetY;
    line->obj.alignTo     = layoutInt->container->children[layoutInt->container->nbChildren - 1];
    line->obj.alignment   = TOP_MIDDLE;
    line->obj.area.width  = SCREEN_WIDTH - 2 * 32;
    line->obj.area.height = 4;
    line->direction       = HORIZONTAL;
    line->thickness       = 2;
    line->offset          = 2;
    // set this new line as child of the main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) line);

    if (numbered) {
        // create Word num typed text
        textArea            = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor = BLACK;
        snprintf(numText, sizeof(numText), "%d.", number);
        textArea->text          = numText;
        textArea->textAlignment = CENTER;
        textArea->fontId        = LARGE_MEDIUM_1BPP_FONT;
#ifdef TARGET_STAX
        textArea->obj.alignmentMarginY = 12;
#else   // TARGET_STAX
        textArea->obj.alignmentMarginY = 9;
#endif  // TARGET_STAX
        textArea->obj.alignTo   = (nbgl_obj_t *) line;
        textArea->obj.alignment = TOP_LEFT;
#ifdef TARGET_STAX
        textArea->obj.area.width = 50;
#else   // TARGET_STAX
        textArea->obj.area.width       = 66;
#endif  // TARGET_STAX
        textArea->obj.area.height = nbgl_getFontLineHeight(textArea->fontId);
        // set this new text area as child of the main container
        addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);
    }

    // create text area
    textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor     = grayedOut ? LIGHT_GRAY : BLACK;
    textArea->text          = text;
    textArea->textAlignment = MID_LEFT;
    textArea->fontId        = LARGE_MEDIUM_1BPP_FONT;
#ifdef TARGET_STAX
    textArea->obj.alignmentMarginY = 12;
#else   // TARGET_STAX
    textArea->obj.alignmentMarginY = 9;
#endif  // TARGET_STAX
    textArea->obj.alignTo    = (nbgl_obj_t *) line;
    textArea->obj.alignment  = TOP_LEFT;
    textArea->obj.area.width = line->obj.area.width;
    if (numbered) {
#ifdef TARGET_STAX
        textArea->obj.alignmentMarginX = 50;
#else   // TARGET_STAX
        textArea->obj.alignmentMarginX = 66;
#endif  // TARGET_STAX
        textArea->obj.area.width -= textArea->obj.alignmentMarginX;
    }
    textArea->obj.area.height  = nbgl_getFontLineHeight(textArea->fontId);
    textArea->autoHideLongLine = true;

    obj = addCallbackObj(layoutInt, (nbgl_obj_t *) textArea, token, NBGL_NO_TUNE);
    if (obj == NULL) {
        return -1;
    }
    textArea->token         = token;
    textArea->obj.touchMask = (1 << TOUCHED);
    textArea->obj.touchId   = ENTERED_TEXT_ID;

    // set this new text area as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);

    // return index of text area to be modified later on
    return (layoutInt->container->nbChildren - 1);
}

/**
 * @brief Updates an existing "text entry" area, created with @ref nbgl_layoutAddEnteredText()
 *
 * @param layout the current layout
 * @param index index of the text (return value of @ref nbgl_layoutAddEnteredText())
 * @param numbered if set to true, the text is preceded on the left by 'number.'
 * @param number if numbered is true, number used to build 'number.' text
 * @param text string to display in the area
 * @param grayedOut if true, the text is grayed out (but not the potential number)
 * @return <0 if error, 0 if OK with text fitting the area, 1 of 0K with text
 * not fitting the area
 */
int nbgl_layoutUpdateEnteredText(nbgl_layout_t *layout,
                                 uint8_t        index,
                                 bool           numbered,
                                 uint8_t        number,
                                 const char    *text,
                                 bool           grayedOut)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_area_t      *textArea;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateEnteredText():\n");
    if (layout == NULL) {
        return -1;
    }

    // update main text area
    textArea = (nbgl_text_area_t *) layoutInt->container->children[index];
    if ((textArea == NULL) || (textArea->obj.type != TEXT_AREA)) {
        return -1;
    }
    textArea->text          = text;
    textArea->textColor     = grayedOut ? LIGHT_GRAY : BLACK;
    textArea->textAlignment = MID_LEFT;
    nbgl_redrawObject((nbgl_obj_t *) textArea, NULL, false);

    // update number text area
    if (numbered) {
        // it is the previously created object
        textArea = (nbgl_text_area_t *) layoutInt->container->children[index - 1];
        snprintf(numText, sizeof(numText), "%d.", number);
        textArea->text = numText;
        nbgl_redrawObject((nbgl_obj_t *) textArea, NULL, false);
    }
    // if the text doesn't fit, indicate it by returning 1 instead of 0, for different refresh
    if (nbgl_getSingleLineTextWidth(textArea->fontId, text) > textArea->obj.area.width) {
        return 1;
    }
    return 0;
}

/**
 * @brief Adds a black full width confirmation button on top of the previously added keyboard.
 *
 * @param layout the current layout
 * @param active if true, button is active, otherwise inactive (grayed-out)
 * @param text text of the button
 * @param token token of the button, used in onActionCallback
 * @param tuneId tune to play when button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddConfirmationButton(nbgl_layout_t *layout,
                                     bool           active,
                                     const char    *text,
                                     int            token,
                                     tune_index_e   tuneId)
{
    layoutObj_t           *obj;
    nbgl_button_t         *button;
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddConfirmationButton():\n");
    if (layout == NULL) {
        return -1;
    }

    button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layoutInt->layer);
    obj    = addCallbackObj(layoutInt, (nbgl_obj_t *) button, token, tuneId);
    if (obj == NULL) {
        return -1;
    }

#ifdef TARGET_STAX
    button->obj.alignmentMarginY = BOTTOM_BORDER_MARGIN;
#else   // TARGET_STAX
    button->obj.alignmentMarginY = 12;
#endif  // TARGET_STAX
    button->obj.alignment   = TOP_MIDDLE;
    button->foregroundColor = WHITE;
    if (active) {
        button->innerColor    = BLACK;
        button->borderColor   = BLACK;
        button->obj.touchMask = (1 << TOUCHED);
        button->obj.touchId   = BOTTOM_BUTTON_ID;
    }
    else {
        button->borderColor = LIGHT_GRAY;
        button->innerColor  = LIGHT_GRAY;
    }
    button->text            = PIC(text);
    button->fontId          = SMALL_BOLD_1BPP_FONT;
    button->obj.area.width  = AVAILABLE_WIDTH;
    button->obj.area.height = BUTTON_DIAMETER;
    button->radius          = BUTTON_RADIUS;
    button->obj.alignTo     = layoutInt->container->children[layoutInt->container->nbChildren - 1];
    // set this new button as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) button);

    // return index of button to be modified later on
    return (layoutInt->container->nbChildren - 1);
}

/**
 * @brief Updates an existing black full width confirmation button on top of the previously added
keyboard.
 *
 * @param layout the current layout
 * @param index returned value of @ref nbgl_layoutAddConfirmationButton()
 * @param active if true, button is active
 * @param text text of the button
= * @return >= 0 if OK
 */
int nbgl_layoutUpdateConfirmationButton(nbgl_layout_t *layout,
                                        uint8_t        index,
                                        bool           active,
                                        const char    *text)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_button_t         *button;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateConfirmationButton():\n");
    if (layout == NULL) {
        return -1;
    }

    // update main text area
    button = (nbgl_button_t *) layoutInt->container->children[index];
    if ((button == NULL) || (button->obj.type != BUTTON)) {
        return -1;
    }
    button->text = text;

    if (active) {
        button->innerColor    = BLACK;
        button->borderColor   = BLACK;
        button->obj.touchMask = (1 << TOUCHED);
        button->obj.touchId   = BOTTOM_BUTTON_ID;
    }
    else {
        button->borderColor = LIGHT_GRAY;
        button->innerColor  = LIGHT_GRAY;
    }
    nbgl_redrawObject((nbgl_obj_t *) button, NULL, false);
    return 0;
}
#else   // HAVE_SE_TOUCH
/**
 * @brief Creates a keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param kbdInfo configuration of the keyboard to draw (including the callback when touched)
 * @return the index of keyboard, to use in @ref nbgl_layoutUpdateKeyboard()
 */
int nbgl_layoutAddKeyboard(nbgl_layout_t *layout, const nbgl_layoutKbd_t *kbdInfo)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keyboard_t       *keyboard;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddKeyboard():\n");
    if (layout == NULL) {
        return -1;
    }

    // create keyboard
    keyboard = (nbgl_keyboard_t *) nbgl_objPoolGet(KEYBOARD, layoutInt->layer);
    keyboard->obj.alignmentMarginY = 0;
    keyboard->obj.alignment        = CENTER;
    keyboard->enableBackspace      = kbdInfo->enableBackspace;
    keyboard->enableValidate       = kbdInfo->enableValidate;
    if (kbdInfo->lettersOnly) {
        keyboard->selectedCharIndex = cx_rng_u32() % 26;
        keyboard->mode              = MODE_LOWER_LETTERS;
    }
    else {
        keyboard->mode = MODE_NONE;
    }
    keyboard->callback    = PIC(kbdInfo->callback);
    keyboard->lettersOnly = kbdInfo->lettersOnly;
    keyboard->keyMask     = kbdInfo->keyMask;
    // set this new keyboard as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) keyboard);

    // return index of keyboard to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeyboard()
 * @param keyMask mask of keys to activate/deactivate on keyboard
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeyboard(nbgl_layout_t *layout, uint8_t index, uint32_t keyMask)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keyboard_t       *keyboard;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateKeyboard(): keyMask = 0x%X\n", keyMask);
    if (layout == NULL) {
        return -1;
    }

    // get keyboard at given index
    keyboard = (nbgl_keyboard_t *) layoutInt->children[index];
    if ((keyboard == NULL) || (keyboard->obj.type != KEYBOARD)) {
        return -1;
    }
    keyboard->keyMask = keyMask;
    if (keyboard->lettersOnly) {
        if (keyMask & (1 << 26)) {
            keyboard->selectedCharIndex = cx_rng_u32() % 26;
        }
        else {
            keyboard->selectedCharIndex = 0;
        }
    }

    nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);

    return 0;
}

/**
 * @brief Adds a "text entry" area under the previously entered object.
 *        The max number of really displayable characters is 8, even if there are 9 placeholders (_)
 *        If longer than 8 chars, the first ones are replaced by a '..'
 *        The 9th placeholder is never filled
 *
 * @param layout the current layout
 * @param text string to display in the area
 * @param lettersOnly if true, display 8 chars placeholders, otherwise 9
 * @return >= 0 if OK
 */
int nbgl_layoutAddEnteredText(nbgl_layout_t *layout, const char *text, bool lettersOnly)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_entry_t     *textEntry;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddEnteredText():\n");
    if (layout == NULL) {
        return -1;
    }

    // create text area
    textEntry          = (nbgl_text_entry_t *) nbgl_objPoolGet(TEXT_ENTRY, layoutInt->layer);
    textEntry->text    = text;
    textEntry->fontId  = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
    textEntry->nbChars = lettersOnly ? 8 : 9;
    textEntry->obj.alignmentMarginY = 5;
    textEntry->obj.alignment        = BOTTOM_MIDDLE;
    textEntry->obj.area.width       = 98;
    textEntry->obj.area.height      = 16;

    // set this new text area as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) textEntry);

    // return index of text area to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing "text entry" area, created with @ref nbgl_layoutAddEnteredText()
 *
 * @param layout the current layout
 * @param index index of the text (return value of @ref nbgl_layoutAddEnteredText())
 * @param text string to display in the area
 * @return <0 if error, 0 if OK with text fitting the area, 1 of 0K with text
 * not fitting the area
 */
int nbgl_layoutUpdateEnteredText(nbgl_layout_t *layout, uint8_t index, const char *text)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_entry_t     *textEntry;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateEnteredText():\n");
    if (layout == NULL) {
        return -1;
    }

    // update main text area
    textEntry = (nbgl_text_entry_t *) layoutInt->children[index];
    if ((textEntry == NULL) || (textEntry->obj.type != TEXT_ENTRY)) {
        return -1;
    }
    textEntry->text = text;
    nbgl_redrawObject((nbgl_obj_t *) textEntry, NULL, false);

    return 0;
}
#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYBOARD

#ifdef NBGL_KEYPAD
#ifdef HAVE_SE_TOUCH
/**
 * @brief Adds a keypad on bottom of the screen, with the associated callback
 *
 * @note Validate and Backspace keys are not enabled at start-up
 *
 * @param layout the current layout
 * @param callback function called when any of the key is touched
 * @param shuffled if set to true, digits are shuffled in keypad
 * @return the index of keypad, to use in @ref nbgl_layoutUpdateKeypad()
 */
int nbgl_layoutAddKeypad(nbgl_layout_t *layout, keyboardCallback_t callback, bool shuffled)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keypad_t         *keypad;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddKeypad():\n");
    if (layout == NULL) {
        return -1;
    }

    // create keypad
    keypad                       = (nbgl_keypad_t *) nbgl_objPoolGet(KEYPAD, layoutInt->layer);
    keypad->obj.alignmentMarginY = 0;
    keypad->obj.alignment        = BOTTOM_MIDDLE;
    keypad->obj.alignTo          = NULL;
    keypad->borderColor          = LIGHT_GRAY;
    keypad->callback             = PIC(callback);
    keypad->enableDigits         = true;
    keypad->enableBackspace      = false;
    keypad->enableValidate       = false;
    keypad->shuffled             = shuffled;
    // set this new keypad as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) keypad);

    // return index of keypad to be modified later on
    return (layoutInt->container->nbChildren - 1);
}

/**
 * @brief Updates an existing keypad on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeypad()
 * @param enableValidate if true, enable Validate key
 * @param enableBackspace if true, enable Backspace key
 * @param enableDigits if true, enable all digit keys
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeypad(nbgl_layout_t *layout,
                            uint8_t        index,
                            bool           enableValidate,
                            bool           enableBackspace,
                            bool           enableDigits)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keypad_t         *keypad;

    LOG_DEBUG(LAYOUT_LOGGER,
              "nbgl_layoutUpdateKeypad(): enableValidate = %d, enableBackspace = %d\n",
              enableValidate,
              enableBackspace);
    if (layout == NULL) {
        return -1;
    }

    // get existing keypad
    keypad = (nbgl_keypad_t *) layoutInt->container->children[index];
    if ((keypad == NULL) || (keypad->obj.type != KEYPAD)) {
        return -1;
    }
    keypad->enableValidate  = enableValidate;
    keypad->enableBackspace = enableBackspace;
    keypad->enableDigits    = enableDigits;

    nbgl_redrawObject((nbgl_obj_t *) keypad, NULL, false);

    return 0;
}

/**
 * @brief Adds a placeholder for hidden digits on top of a keypad, to represent the entered digits,
 * as full circles The placeholder is "underligned" with a thin horizontal line of the expected full
 * length
 *
 * @note It must be the last added object, after potential back key, title, and keypad. Vertical
 * positions of title and hidden digits will be computed here
 *
 * @param layout the current layout
 * @param nbDigits number of digits to be displayed
 * @return the index of digits set, to use in @ref nbgl_layoutUpdateHiddenDigits()
 */
int nbgl_layoutAddHiddenDigits(nbgl_layout_t *layout, uint8_t nbDigits)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    uint8_t                space;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddHiddenDigits():\n");
    if (layout == NULL) {
        return -1;
    }
    if (nbDigits > KEYPAD_MAX_DIGITS) {
        return -1;
    }
    if (nbDigits > 8) {
        space = 4;
    }
    else {
        space = 12;
    }

    // create a container, invisible or bordered
    container             = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    container->nbChildren = nbDigits;
#ifdef TARGET_STAX
    container->nbChildren++;  // +1 for the line
#endif                        // TARGET_STAX
    container->children = nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
    // <space> pixels between each icon (knowing that the effective round are 18px large and the
    // icon 24px)
    container->obj.area.width = nbDigits * C_round_24px.width + (nbDigits + 1) * space;
#ifdef TARGET_STAX
    container->obj.area.height = 48;
    // distance from digits to title is fixed to 20 px, except if title is more than 1 line and a
    // back key is present
    if ((layoutInt->container->nbChildren != 3)
        || (layoutInt->container->children[1]->area.height == 32)) {
        container->obj.alignmentMarginY = 20;
    }
    else {
        container->obj.alignmentMarginY = 12;
    }
#else   // TARGET_STAX
    container->obj.area.height   = 64;
#endif  // TARGET_STAX

    // item N-2 is the title
    container->obj.alignTo   = layoutInt->container->children[layoutInt->container->nbChildren - 2];
    container->obj.alignment = BOTTOM_MIDDLE;

    // set this new container as child of the main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    // create children of the container, as images (empty circles)
    nbgl_objPoolGetArray(IMAGE, nbDigits, layoutInt->layer, (nbgl_obj_t **) container->children);
    for (int i = 0; i < nbDigits; i++) {
        nbgl_image_t *image = (nbgl_image_t *) container->children[i];
#ifdef TARGET_STAX
        image->buffer = &C_round_24px;
#else   // TARGET_STAX
        image->buffer = &C_pin_24;
#endif  // TARGET_STAX
        image->foregroundColor      = WHITE;
        image->obj.alignmentMarginX = space;
        if (i > 0) {
            image->obj.alignment = MID_RIGHT;
            image->obj.alignTo   = (nbgl_obj_t *) container->children[i - 1];
        }
        else {
            image->obj.alignment        = NO_ALIGNMENT;
            image->obj.alignmentMarginY = (container->obj.area.height - C_round_24px.width) / 2;
        }
    }
#ifdef TARGET_STAX
    nbgl_line_t *line;
    // create gray line
    line                          = (nbgl_line_t *) nbgl_objPoolGet(LINE, layoutInt->layer);
    line->lineColor               = LIGHT_GRAY;
    line->obj.alignmentMarginY    = 0;
    line->obj.alignTo             = NULL;
    line->obj.alignment           = BOTTOM_MIDDLE;
    line->obj.area.width          = container->obj.area.width;
    line->obj.area.height         = 4;
    line->direction               = HORIZONTAL;
    line->thickness               = 2;
    line->offset                  = 2;
    container->children[nbDigits] = (nbgl_obj_t *) line;
#endif  // TARGET_STAX

    // return index of keypad to be modified later on
    return (layoutInt->container->nbChildren - 1);
}

/**
 * @brief Updates an existing set of hidden digits, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddHiddenDigits()
 * @param nbActive number of "active" digits (represented by discs instead of circles)
 * @return >=0 if OK
 */
int nbgl_layoutUpdateHiddenDigits(nbgl_layout_t *layout, uint8_t index, uint8_t nbActive)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_image_t          *image;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateHiddenDigits(): nbActive = %d\n", nbActive);
    if (layout == NULL) {
        return -1;
    }

    // get container
    container = (nbgl_container_t *) layoutInt->container->children[index];
    // sanity check
    if ((container == NULL) || (container->obj.type != CONTAINER)) {
        return -1;
    }
    if (nbActive > container->nbChildren) {
        return -1;
    }
    if (nbActive == 0) {
        // deactivate the first digit
        image = (nbgl_image_t *) container->children[0];
        if ((image == NULL) || (image->obj.type != IMAGE)) {
            return -1;
        }
        image->foregroundColor = WHITE;
    }
    else {
        image = (nbgl_image_t *) container->children[nbActive - 1];
        if ((image == NULL) || (image->obj.type != IMAGE)) {
            return -1;
        }
        // if the last "active" is already active, it means that we are decreasing the number of
        // active otherwise we are increasing it
        if (image->foregroundColor == BLACK) {
            // all digits are already active
            if (nbActive == container->nbChildren) {
                return 0;
            }
            // deactivate the next digit
            image                  = (nbgl_image_t *) container->children[nbActive];
            image->foregroundColor = WHITE;
        }
        else {
#ifdef TARGET_STAX
            image->buffer = &C_round_24px;
#else   // TARGET_STAX
            image->buffer = &C_pin_24;
#endif  // TARGET_STAX
            image->foregroundColor = BLACK;
        }
    }

    nbgl_redrawObject((nbgl_obj_t *) image, NULL, false);

    return 0;
}
#else   // HAVE_SE_TOUCH
/**
 * @brief Adds a keypad on bottom of the screen, with the associated callback
 *
 * @note Validate and Backspace keys are not enabled at start-up
 *
 * @param layout the current layout
 * @param callback function called when any of the key is touched
 * @param text text to use as title for the keypad
 * @param shuffled if set to true, digits are shuffled in keypad
 * @return the index of keypad in layout, to use in @ref nbgl_layoutUpdateKeypad()
 */
int nbgl_layoutAddKeypad(nbgl_layout_t     *layout,
                         keyboardCallback_t callback,
                         const char        *text,
                         bool               shuffled)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keypad_t         *keypad;
    nbgl_text_area_t      *textArea;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddKeypad():\n");
    if (layout == NULL) {
        return -1;
    }

    textArea                  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor       = WHITE;
    textArea->text            = PIC(text);
    textArea->textAlignment   = CENTER;
    textArea->fontId          = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
    textArea->obj.area.width  = AVAILABLE_WIDTH;
    textArea->obj.area.height = 12;
    textArea->wrapping        = false;
    textArea->obj.alignment   = TOP_MIDDLE;
    textArea->obj.alignmentMarginY = 3;
    // set this new obj as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);

    // create keypad
    keypad                       = (nbgl_keypad_t *) nbgl_objPoolGet(KEYPAD, layoutInt->layer);
    keypad->obj.alignment        = BOTTOM_MIDDLE;
    keypad->obj.alignmentMarginY = 6;
    keypad->obj.alignTo          = NULL;
    keypad->callback             = PIC(callback);
    keypad->enableBackspace      = false;
    keypad->enableValidate       = false;
    keypad->selectedKey          = 0xFF;  // to be picked
    keypad->shuffled             = shuffled;
    // set this new keypad as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) keypad);

    // return index of keypad to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing keypad on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeypad()
 * @param enableValidate if true, enable Validate key
 * @param enableBackspace if true, enable Backspace key
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeypad(nbgl_layout_t *layout,
                            uint8_t        index,
                            bool           enableValidate,
                            bool           enableBackspace)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keypad_t         *keypad;

    LOG_DEBUG(LAYOUT_LOGGER,
              "nbgl_layoutUpdateKeypad(): enableValidate = %d, enableBackspace = %d\n",
              enableValidate,
              enableBackspace);
    if (layout == NULL) {
        return -1;
    }

    // get existing keypad
    keypad = (nbgl_keypad_t *) layoutInt->children[index];
    if ((keypad == NULL) || (keypad->obj.type != KEYPAD)) {
        LOG_WARN(LAYOUT_LOGGER, "nbgl_layoutUpdateKeypad(): keypad not found\n");
        return -1;
    }
    if (enableValidate && !keypad->enableValidate) {
        // if validate key is enabled and was not, select it directly
        keypad->selectedKey = 11;
    }
    else {
        // otherwise let the draw function pick a new selected
        keypad->selectedKey = 0xFF;
    }
    keypad->enableValidate  = enableValidate;
    keypad->enableBackspace = enableBackspace;

    nbgl_redrawObject((nbgl_obj_t *) keypad, NULL, false);

    return 0;
}

/**
 * @brief Adds a placeholder for hidden digits on top of a keypad, to represent the entered digits,
 * as full circles
 *
 * @note It must be the last added object, after keypad. Vertical positions of title and hidden
 * digits will be computed here
 *
 * @param layout the current layout
 * @param nbDigits number of digits to be displayed
 * @return the index of digits set, to use in @ref nbgl_layoutUpdateHiddenDigits()
 */
int nbgl_layoutAddHiddenDigits(nbgl_layout_t *layout, uint8_t nbDigits)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddHiddenDigits():\n");
    if (layout == NULL) {
        return -1;
    }

    // create a container, invisible or bordered
    container             = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    container->nbChildren = nbDigits;
    container->children   = nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
    // 1 pixel between each icon (knowing that the effective bullets are 8px large)
    container->obj.area.width  = nbDigits * C_pin_bullet_empty.width + (nbDigits - 1);
    container->obj.area.height = C_pin_bullet_empty.height;
    // distance from top to digits is fixed to 24 px
    container->obj.alignmentMarginY = 24;
    container->obj.alignTo          = NULL;
    container->obj.alignment        = TOP_MIDDLE;

    // set this new container as child of the main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    // create children of the container, as images (empty circles)
    nbgl_objPoolGetArray(IMAGE, nbDigits, layoutInt->layer, (nbgl_obj_t **) container->children);
    for (int i = 0; i < nbDigits; i++) {
        nbgl_image_t *image    = (nbgl_image_t *) container->children[i];
        image->buffer          = &C_pin_bullet_empty;
        image->foregroundColor = WHITE;
        if (i > 0) {
            image->obj.alignment        = MID_RIGHT;
            image->obj.alignTo          = (nbgl_obj_t *) container->children[i - 1];
            image->obj.alignmentMarginX = 1;
        }
        else {
            image->obj.alignment = NO_ALIGNMENT;
        }
    }
    // return index of container to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing set of hidden digits, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddHiddenDigits()
 * @param nbActive number of "active" digits (represented by discs instead of circles)
 * @return >=0 if OK
 */
int nbgl_layoutUpdateHiddenDigits(nbgl_layout_t *layout, uint8_t index, uint8_t nbActive)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_image_t          *image;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateHiddenDigits(): nbActive = %d\n", nbActive);
    if (layout == NULL) {
        return -1;
    }

    // get container
    container = (nbgl_container_t *) layoutInt->children[index];
    // sanity check
    if ((container == NULL) || (container->obj.type != CONTAINER)) {
        return -1;
    }
    if (nbActive > container->nbChildren) {
        return -1;
    }
    if (nbActive == 0) {
        // deactivate the first digit
        image = (nbgl_image_t *) container->children[0];
        if ((image == NULL) || (image->obj.type != IMAGE)) {
            return -1;
        }
        image->buffer = &C_pin_bullet_empty;
    }
    else {
        image = (nbgl_image_t *) container->children[nbActive - 1];
        if ((image == NULL) || (image->obj.type != IMAGE)) {
            return -1;
        }
        // if the last "active" is already active, it means that we are decreasing the number of
        // active otherwise we are increasing it
        if (image->buffer == &C_pin_bullet_filled) {
            // all digits are already active
            if (nbActive == container->nbChildren) {
                return 0;
            }
            // deactivate the next digit
            image         = (nbgl_image_t *) container->children[nbActive];
            image->buffer = &C_pin_bullet_empty;
        }
        else {
            image->buffer = &C_pin_bullet_filled;
        }
    }

    nbgl_redrawObject((nbgl_obj_t *) image, NULL, false);

    return 0;
}
#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYPAD

/**
 * @brief Applies given layout. The screen will be redrawn
 *
 * @param layoutParam layout to redraw
 * @return a pointer to the corresponding layout
 */
int nbgl_layoutDraw(nbgl_layout_t *layoutParam)
{
    nbgl_layoutInternal_t *layout = (nbgl_layoutInternal_t *) layoutParam;

    if (layout == NULL) {
        return -1;
    }
#ifdef HAVE_SE_TOUCH
    LOG_DEBUG(LAYOUT_LOGGER,
              "nbgl_layoutDraw(): container.nbChildren =%d, layout->nbChildren = %d\n",
              layout->container->nbChildren,
              layout->nbChildren);
    if (layout->tapText) {
        // set this new container as child of main container
        addObjectToLayout(layout, (nbgl_obj_t *) layout->tapText);
    }
    if (layout->withLeftBorder == true) {
        // draw now the line
        nbgl_line_t *line                    = createLeftVerticalLine(layout->layer);
        layout->children[layout->nbChildren] = (nbgl_obj_t *) line;
        layout->nbChildren++;
    }
#else   // HAVE_SE_TOUCH
    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutDraw(): layout->nbChildren = %d\n", layout->nbChildren);
#endif  // HAVE_SE_TOUCH
    nbgl_screenRedraw();

    return 0;
}

/**
 * @brief Release the layout obtained with @ref nbgl_layoutGet()
 *
 * @param layoutParam layout to release
 * @return >= 0 if OK
 */
int nbgl_layoutRelease(nbgl_layout_t *layoutParam)
{
    nbgl_layoutInternal_t *layout = (nbgl_layoutInternal_t *) layoutParam;
    LOG_DEBUG(PAGE_LOGGER, "nbgl_layoutRelease(): \n");
    if (layout == NULL) {
        return -1;
    }
    // if modal
    if (layout->modal) {
        nbgl_screenPop(layout->layer);
    }
    layout->nbChildren = 0;
    return 0;
}
