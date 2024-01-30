
/**
 * @file nbgl_obj_keyboard_nanos.c
 * @brief The construction and touch management of a keyboard object for Nanos
 *
 */

#ifdef NBGL_KEYBOARD
#ifndef HAVE_SE_TOUCH

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "nbgl_debug.h"
#include "nbgl_front.h"
#include "nbgl_draw.h"
#include "nbgl_obj.h"
#include "nbgl_fonts.h"
#include "nbgl_touch.h"
#include "glyphs.h"
#include "os_io.h"

/*********************
 *      DEFINES
 *********************/

#define BACKSPACE_KEY_INDEX      26
#define VALIDATE_INDEX           27
#define SHIFT_KEY_INDEX          28
#define LETTER_TO_DIGITS_OFFSET  (sizeof(keysByMode[DIGITS_AND_SPECIALS]) - 29)
#define GET_CHAR(mode, char_idx) ((const char *) PIC(screen_keyboard_keys_by_mode[mode]))[char_idx]

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LOWER_LETTERS,
    UPPER_LETTERS,
    DIGITS_AND_SPECIALS
} KbdMode_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
const char *const keysByMode[] = {
    // when first letter is already entered
    "abcdefghijklmnopqrstuvwxyz\b\n\r",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ\b\n\r",
    "0123456789 '\"`&/?!:;.,~*$=+-[](){}^<>\\_#@|%\b\n\r",
};

// these icons will be centered
const nbgl_icon_details_t *const keyboardIcons[] = {
    &C_icon_lowercase,
    &C_icon_uppercase,
    &C_icon_digits,
    &C_icon_backspace,
    &C_icon_validate,
    &C_icon_classes,
    &C_icon_lowercase_invert,
    &C_icon_uppercase_invert,
    &C_icon_digits_invert,
    &C_icon_backspace_invert,
    &C_icon_validate_invert,
    &C_icon_classes_invert,
};

static const nbgl_icon_details_t *const modeIcons[] = {
    &C_icon_lowercase,
    &C_icon_uppercase,
    &C_icon_digits,
};

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void keyboardDrawIcon(int16_t x0, int8_t y0, bool inverted, const nbgl_icon_details_t *icon)
{
    nbgl_area_t rectArea;

    rectArea.backgroundColor = inverted ? WHITE : BLACK;
    rectArea.width           = icon->width;
    rectArea.height          = icon->height;
    rectArea.bpp             = NBGL_BPP_1;
    // center
    rectArea.x0 = x0 + (KEYBOARD_KEY_WIDTH - icon->width) / 2;
    rectArea.y0 = y0 + (KEYBOARD_KEY_HEIGHT - icon->height) / 2;
    nbgl_drawIcon(&rectArea, NO_TRANSFORMATION, inverted ? BLACK : WHITE, icon);
}

static void keyboardDrawChar(int16_t x0, int8_t y0, bool inverted, const char *charPtr)
{
    nbgl_area_t rectArea;

    rectArea.backgroundColor = inverted ? WHITE : BLACK;
    rectArea.width           = nbgl_getCharWidth(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, charPtr);
    rectArea.height          = 12;
    rectArea.bpp             = NBGL_BPP_1;
    // center
    rectArea.x0 = x0 + (KEYBOARD_KEY_WIDTH - rectArea.width) / 2;
    rectArea.y0 = y0 + (KEYBOARD_KEY_HEIGHT - rectArea.height) / 2 - 3;
    nbgl_drawText(
        &rectArea, charPtr, 1, BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, inverted ? BLACK : WHITE);
}

static void keyboardDrawCommon(nbgl_keyboard_t *keyboard)
{
    nbgl_area_t rectArea;

    // clean full area
    rectArea.backgroundColor = BLACK;
    rectArea.x0              = keyboard->obj.area.x0;
    rectArea.y0              = keyboard->obj.area.y0;
    rectArea.width           = keyboard->obj.area.width;
    rectArea.height          = keyboard->obj.area.height;
    nbgl_frontDrawRect(&rectArea);

    // draw select 'key' in white
    rectArea.x0     = keyboard->obj.area.x0 + 2 * KEYBOARD_KEY_WIDTH;
    rectArea.y0     = keyboard->obj.area.y0;
    rectArea.width  = KEYBOARD_KEY_WIDTH;
    rectArea.height = keyboard->obj.area.height;
    nbgl_drawRoundedRect(&rectArea, RADIUS_3_PIXELS, WHITE);

    // draw separating '-' in white
    rectArea.backgroundColor = WHITE;
    rectArea.x0              = keyboard->obj.area.x0 + KEYBOARD_KEY_WIDTH + 5;
    rectArea.y0              = keyboard->obj.area.y0 + 6;
    rectArea.width           = 3;
    rectArea.height          = 1;
    nbgl_frontDrawRect(&rectArea);
    rectArea.x0 = keyboard->obj.area.x0 + 3 * KEYBOARD_KEY_WIDTH + 5;
    nbgl_frontDrawRect(&rectArea);
}

// draw letters for letters mode
static void keyboardDrawLettersOnly(nbgl_keyboard_t *keyboard)
{
    uint8_t     i;
    const char *keys = keysByMode[keyboard->mode];

    if (keyboard->keyMask == 0x07FFFFFF) {
        return;
    }

    keyboardDrawCommon(keyboard);

    // if selectedChar is masked, move to the first unmasked
    while (keyboard->keyMask & (1 << keyboard->selectedCharIndex)) {
        if (keyboard->selectedCharIndex < BACKSPACE_KEY_INDEX) {
            keyboard->selectedCharIndex++;
        }
        else {
            keyboard->selectedCharIndex = 0;
        }
    }

    // fill the letters at position middle and right
    uint8_t j = 1;
    i         = keyboard->selectedCharIndex;
    while (j < 3) {
        // use the provided mask to check what letters(+backspace at 27th position) to use
        if ((keyboard->keyMask & (1 << i)) == 0) {
            if (i == BACKSPACE_KEY_INDEX) {  // backspace
                keyboardDrawIcon(keyboard->obj.area.x0 + 2 * j * KEYBOARD_KEY_WIDTH,
                                 keyboard->obj.area.y0,
                                 (j == 1),
                                 &C_icon_backspace);
            }
            else {  // any char
                keyboardDrawChar(keyboard->obj.area.x0 + 2 * j * KEYBOARD_KEY_WIDTH,
                                 keyboard->obj.area.y0,
                                 (j == 1),
                                 &keys[i]);
            }
            j++;
        }
        i++;
        i %= 27;
        // safe check in case mask is full of 1s
        if (i == keyboard->selectedCharIndex) {
            break;
        }
    }
    // fill the letters at left position from end
    i = (keyboard->selectedCharIndex + 26) % 27;
    while (true) {
        if ((keyboard->keyMask & (1 << i)) == 0) {
            if (i == BACKSPACE_KEY_INDEX) {  // backspace
                keyboardDrawIcon(
                    keyboard->obj.area.x0, keyboard->obj.area.y0, false, &C_icon_backspace);
            }
            else {  // any char
                keyboardDrawChar(keyboard->obj.area.x0, keyboard->obj.area.y0, false, &keys[i]);
            }
            break;
        }
        if (i > 0) {
            i--;
        }
        else {
            i = BACKSPACE_KEY_INDEX;
        }
        // safe check in case mask is full of 1s
        if (i == (keyboard->selectedCharIndex + 26) % 27) {
            break;
        }
    }
}

// draw letters for regular mode
static void keyboardDrawRegular(nbgl_keyboard_t *keyboard)
{
    int8_t i;

    keyboardDrawCommon(keyboard);

    // if mode is not already defined, display the 3 icons of keyboard modes to let user choose it:
    // "ab, AB and 0? "
    if (keyboard->mode == MODE_NONE) {
        for (i = 0; i < 3; i++) {
            uint8_t charIndex = (keyboard->selectedCharIndex + 2 + i) % 3;
            keyboardDrawIcon(keyboard->obj.area.x0 + 2 * i * KEYBOARD_KEY_WIDTH,
                             keyboard->obj.area.y0,
                             (i == 1),
                             modeIcons[charIndex]);
        }
        return;
    }

    const char *keys   = keysByMode[keyboard->mode];
    uint8_t     maxLen = strlen(keys);

    // mode is defined, so draw the proper 3 letters (or icons)
    for (i = 0; i < 3; i++) {
        uint8_t charIndex = (keyboard->selectedCharIndex + maxLen - 1 + i) % maxLen;
        if (keys[charIndex] == '\r') {
            keyboardDrawIcon(keyboard->obj.area.x0 + 2 * i * KEYBOARD_KEY_WIDTH,
                             keyboard->obj.area.y0,
                             (i == 1),
                             &C_icon_classes);
        }
        else if (keys[charIndex] == '\n') {
            keyboardDrawIcon(keyboard->obj.area.x0 + 2 * i * KEYBOARD_KEY_WIDTH,
                             keyboard->obj.area.y0,
                             (i == 1),
                             &C_icon_validate_10);
        }
        else if (keys[charIndex] == '\b') {
            keyboardDrawIcon(keyboard->obj.area.x0 + 2 * i * KEYBOARD_KEY_WIDTH,
                             keyboard->obj.area.y0,
                             (i == 1),
                             &C_icon_backspace);
        }
        else {
            keyboardDrawChar(keyboard->obj.area.x0 + 2 * i * KEYBOARD_KEY_WIDTH,
                             keyboard->obj.area.y0,
                             (i == 1),
                             &keys[charIndex]);
        }
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief function to be called when the keyboard object is receiving a button press
 *
 * @param obj touched object (keyboard)
 * @param buttonEvent type of button event
 * @return none
 */
void nbgl_keyboardCallback(nbgl_obj_t *obj, nbgl_buttonEvent_t buttonEvent)
{
    nbgl_keyboard_t *keyboard = (nbgl_keyboard_t *) obj;
    uint8_t          nbMax;

    LOG_DEBUG(MISC_LOGGER, "nbgl_keyboardCallback(): buttonEvent = %d\n", buttonEvent);

    if (keyboard->mode == MODE_NONE) {
        nbMax = 2;
    }
    else {
        if (keyboard->lettersOnly) {
            nbMax = BACKSPACE_KEY_INDEX;
        }
        else {
            nbMax = strlen(keysByMode[keyboard->mode]) - 1;
        }
    }
    if (buttonEvent == BUTTON_BOTH_PRESSED) {
        if (keyboard->mode == MODE_NONE) {
            // apply the selected mode
            keyboard->mode = keyboard->selectedCharIndex;
            // reset the selected index to start with "a" (or first char of selected mode)
            keyboard->selectedCharIndex = 0;
        }
        else {
            if (keyboard->lettersOnly) {
                if ((keyboard->selectedCharIndex < 26)
                    && ((keyboard->keyMask & (1 << keyboard->selectedCharIndex)) == 0)) {
                    const char *keys = keysByMode[keyboard->mode];
                    keyboard->callback(keys[keyboard->selectedCharIndex]);
                }
                else if (keyboard->selectedCharIndex == BACKSPACE_KEY_INDEX) {  // backspace
                    keyboard->callback(BACKSPACE_KEY);
                }
            }
            else {
                char key = keysByMode[keyboard->mode][keyboard->selectedCharIndex];
                if (key == '\r') {
                    // go back to mode choice
                    keyboard->mode = MODE_NONE;
                    // reset the selected index
                    keyboard->selectedCharIndex = 0;
                    nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
                    nbgl_refresh();
                }
                else if (key == '\n') {
                    keyboard->callback(VALIDATE_KEY);
                }
                else if (key == '\b') {
                    keyboard->callback(BACKSPACE_KEY);
                }
                else {
                    keyboard->callback(key);
                }
            }
            return;
        }
    }
    else if (buttonEvent == BUTTON_LEFT_PRESSED) {
        do {
            if (keyboard->selectedCharIndex > 0) {
                keyboard->selectedCharIndex--;
            }
            else {
                keyboard->selectedCharIndex = nbMax;
            }
        } while (keyboard->keyMask & (1 << keyboard->selectedCharIndex));
    }
    else if (buttonEvent == BUTTON_RIGHT_PRESSED) {
        do {
            if (keyboard->selectedCharIndex < nbMax) {
                keyboard->selectedCharIndex++;
            }
            else {
                keyboard->selectedCharIndex = 0;
            }
        } while (keyboard->keyMask & (1 << keyboard->selectedCharIndex));
    }
    else {
        return;
    }
    nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
    nbgl_refresh();
}

/**
 * @brief This function draws a keyboard object
 *
 * @param kbd the object to be drawned
 */
void nbgl_objDrawKeyboard(nbgl_keyboard_t *kbd)
{
    if (kbd->lettersOnly) {
        keyboardDrawLettersOnly(kbd);
    }
    else {
        keyboardDrawRegular(kbd);
    }
}
#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYBOARD
