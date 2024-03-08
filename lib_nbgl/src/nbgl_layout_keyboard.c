/**
 * @file nbgl_layout_keyboard.c
 * @brief Implementation of predefined keyboard related layouts management
 * @note This file applies only to wallet size products (Stax, Europa...)
 */

#ifdef HAVE_SE_TOUCH
#ifdef NBGL_KEYBOARD
/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdlib.h>
#include "nbgl_debug.h"
#include "nbgl_front.h"
#include "nbgl_layout_internal.h"
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

/**********************
 *      MACROS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

static nbgl_button_t *choiceButtons[NB_MAX_SUGGESTION_BUTTONS];
static char           numText[5];
static uint8_t        nbActiveButtons;
#ifndef TARGET_STAX
static nbgl_image_t *partialButtonImages[2];
#endif  // TARGET_STAX

/**********************
 *  STATIC PROTOTYPES
 **********************/

#ifndef TARGET_STAX
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

/**********************
 *   GLOBAL INTERNAL FUNCTIONS
 **********************/

void keyboardInit(void)
{
    nbActiveButtons = 0;
}

#ifndef TARGET_STAX
bool keyboardSwipeCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType)
{
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

            return true;
        }
    }
    return false;
}
#endif  // TARGET_STAX

/**********************
 *   GLOBAL API FUNCTIONS
 **********************/

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
    layoutAddObject(layoutInt, (nbgl_obj_t *) keyboard);

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
    obj = layoutAddCallbackObj(layoutInt, (nbgl_obj_t *) container, 0, NBGL_NO_TUNE);
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
        obj = layoutAddCallbackObj(
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
    layoutAddObject(layoutInt, (nbgl_obj_t *) container);

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
    layoutAddObject(layoutInt, (nbgl_obj_t *) line);

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
        layoutAddObject(layoutInt, (nbgl_obj_t *) textArea);
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

    obj = layoutAddCallbackObj(layoutInt, (nbgl_obj_t *) textArea, token, NBGL_NO_TUNE);
    if (obj == NULL) {
        return -1;
    }
    textArea->token         = token;
    textArea->obj.touchMask = (1 << TOUCHED);
    textArea->obj.touchId   = ENTERED_TEXT_ID;

    // set this new text area as child of the container
    layoutAddObject(layoutInt, (nbgl_obj_t *) textArea);

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
    obj    = layoutAddCallbackObj(layoutInt, (nbgl_obj_t *) button, token, tuneId);
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
    layoutAddObject(layoutInt, (nbgl_obj_t *) button);

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
#endif  // NBGL_KEYBOARD
#endif  // HAVE_SE_TOUCH
