/**
 * @file nbgl_layout_keyboard_nanos.c
 * @brief Implementation of predefined layouts management for Nano target, for Keyboard
 */

#ifndef HAVE_SE_TOUCH
#ifdef NBGL_KEYBOARD
/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdlib.h>
#include "nbgl_debug.h"
#include "nbgl_layout_internal_nanos.h"
#include "glyphs.h"
#include "os_pic.h"
#include "os_helpers.h"
#include "lcx_rng.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
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
    layoutAddObject(layoutInt, (nbgl_obj_t *) keyboard);

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
    layoutAddObject(layoutInt, (nbgl_obj_t *) textEntry);

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

#endif  // NBGL_KEYBOARD
#endif  // HAVE_SE_TOUCH
