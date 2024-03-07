/**
 * @file nbgl_layout_keyboard_nanos.c
 * @brief Implementation of predefined layouts management for Nano target, for Keypad
 */

#ifndef HAVE_SE_TOUCH
#ifdef NBGL_KEYPAD
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
    layoutAddObject(layoutInt, (nbgl_obj_t *) textArea);

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
    layoutAddObject(layoutInt, (nbgl_obj_t *) keypad);

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
    layoutAddObject(layoutInt, (nbgl_obj_t *) container);

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
#endif  // NBGL_KEYPAD
#endif  // HAVE_SE_TOUCH
