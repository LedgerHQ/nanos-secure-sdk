
/**
 * @file nbgl_obj_keypad_nanos.c
 * @brief The construction and key management of a keypad object for nanos
 *
 */

#ifdef NBGL_KEYPAD
#ifndef HAVE_SE_TOUCH

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_debug.h"
#include "nbgl_front.h"
#include "nbgl_draw.h"
#include "nbgl_obj.h"
#include "nbgl_fonts.h"
#include "nbgl_screen.h"
#include "glyphs.h"
#include "os_io_seproxyhal.h"
#include "lcx_rng.h"

/*********************
 *      DEFINES
 *********************/
#define KEY_WIDTH        9
#define DIGIT_HEIGHT     12
#define DIGIT_OFFSET_X   13
#define DIGIT_OFFSET_Y   ((KEYPAD_HEIGHT - DIGIT_HEIGHT) / 2)
#define INIT_DIGIT_VALUE 5

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const nbgl_icon_details_t *digits_icons[] = {&C_digit_0,
                                                    &C_digit_1,
                                                    &C_digit_2,
                                                    &C_digit_3,
                                                    &C_digit_4,
                                                    &C_digit_5,
                                                    &C_digit_6,
                                                    &C_digit_7,
                                                    &C_digit_8,
                                                    &C_digit_9};
/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC FUNCTIONS
 **********************/
static char positionToChar(uint8_t pos)
{
    if (pos == 0) {
        return BACKSPACE_KEY;
    }
    else if (pos == 11) {
        return VALIDATE_KEY;
    }
    else {
        return 0x30 + (pos - 1);
    }
}

static void keypadDrawDigits(nbgl_keypad_t *keypad)
{
    uint8_t     i;
    nbgl_area_t rectArea;

    // clean full area
    rectArea.backgroundColor = BLACK;
    rectArea.x0              = keypad->obj.area.x0;
    rectArea.y0              = keypad->obj.area.y0;
    rectArea.width           = keypad->obj.area.width;
    rectArea.height          = keypad->obj.area.height;
    nbgl_frontDrawRect(&rectArea);

    rectArea.backgroundColor = BLACK;
    rectArea.y0              = keypad->obj.area.y0 + DIGIT_OFFSET_Y;
    rectArea.bpp             = NBGL_BPP_1;
    // row of digits: 0 1 2 3... 9
    for (i = 0; i < 10; i++) {
        rectArea.width  = digits_icons[i]->width;
        rectArea.height = digits_icons[i]->height;

        rectArea.x0 = keypad->obj.area.x0 + DIGIT_OFFSET_X + i * KEY_WIDTH;
        nbgl_frontDrawImage(&rectArea, digits_icons[i]->bitmap, NO_TRANSFORMATION, WHITE);
    }
    // draw backspace
    if (keypad->enableBackspace) {
        rectArea.width  = C_icon_backspace.width;
        rectArea.height = C_icon_backspace.height;
        rectArea.x0     = keypad->obj.area.x0;
        rectArea.y0     = keypad->obj.area.y0 + ((KEYPAD_HEIGHT - C_icon_backspace.height) / 2);
        nbgl_frontDrawImage(&rectArea, C_icon_backspace.bitmap, NO_TRANSFORMATION, WHITE);
    }

    // draw validate
    if (keypad->enableValidate) {
        rectArea.width  = C_digit_validate_bold.width;
        rectArea.height = C_digit_validate_bold.height;
        rectArea.x0     = keypad->obj.area.x0 + KEYPAD_WIDTH - C_digit_validate_bold.width;
        rectArea.y0 = keypad->obj.area.y0 + ((KEYPAD_HEIGHT - C_digit_validate_bold.height) / 2);
        nbgl_frontDrawImage(&rectArea, C_digit_validate_bold.bitmap, NO_TRANSFORMATION, WHITE);
    }
}

static void keypadDrawSelected(nbgl_keypad_t *keypad)
{
    nbgl_area_t rectArea;
    /// then draw 1 horizontal line
    rectArea.backgroundColor = WHITE;

    if (keypad->selectedKey == 0) {
        rectArea.x0 = keypad->obj.area.x0;
    }
    else if (keypad->selectedKey < 11) {  // if it's a digit
        rectArea.x0 = keypad->obj.area.x0 + DIGIT_OFFSET_X + (keypad->selectedKey - 1) * KEY_WIDTH;
    }
    else if (keypad->selectedKey == 11) {
        rectArea.x0 = keypad->obj.area.x0 + KEYPAD_WIDTH - C_digit_validate_bold.width;
    }
    else {
        return;
    }
    rectArea.y0     = keypad->obj.area.y0 + keypad->obj.area.height - 2;
    rectArea.width  = 8;
    rectArea.height = 2;
    nbgl_frontDrawRect(&rectArea);
}

static void keypadDrawSelectedTouched(nbgl_keypad_t *keypad)
{
    nbgl_area_t rectArea;
    /// draw 1 horizontal line upper selected key
    rectArea.backgroundColor = WHITE;

    if (keypad->selectedKey == 0) {
        rectArea.x0 = keypad->obj.area.x0;
    }
    else if (keypad->selectedKey < 11) {  // if it's a digit
        rectArea.x0 = keypad->obj.area.x0 + DIGIT_OFFSET_X + (keypad->selectedKey - 1) * KEY_WIDTH;
    }
    else if (keypad->selectedKey == 11) {
        rectArea.x0 = keypad->obj.area.x0 + KEYPAD_WIDTH - C_digit_validate_bold.width;
    }
    else {
        return;
    }
    rectArea.y0     = keypad->obj.area.y0;
    rectArea.width  = 8;
    rectArea.height = 2;
    nbgl_frontDrawRect(&rectArea);
    nbgl_frontRefreshArea(&rectArea, FULL_COLOR_CLEAN_REFRESH, POST_REFRESH_KEEP_POWER_STATE);
}

static void keypadInitSelected(nbgl_keypad_t *keypad)
{
    if (!keypad->shuffled) {
        keypad->selectedKey = 1 + INIT_DIGIT_VALUE;
    }
    else {
        uint8_t nbChoices = 10;
        uint8_t random;
        if (keypad->enableBackspace) {
            nbChoices++;
        }
        if (keypad->enableValidate) {
            nbChoices++;
        }
        random = cx_rng_u32_range(0, nbChoices);
        if (random < 10) {
            keypad->selectedKey = 1 + random;
        }
        else if (random == 10) {
            keypad->selectedKey = 0;
        }
        else if (random == 11) {
            keypad->selectedKey = 11;
        }
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief function to be called when the keypad object is touched
 *
 * @param obj touched object (keypad)
 * @param buttonEvent type of key event
 * @return none
 */
void nbgl_keypadCallback(nbgl_obj_t *obj, nbgl_buttonEvent_t buttonEvent)
{
    nbgl_keypad_t *keypad = (nbgl_keypad_t *) obj;

    LOG_DEBUG(OBJ_LOGGER, "nbgl_keypadCallback(): buttonEvent = %d\n", buttonEvent);

    if (buttonEvent == BUTTON_BOTH_TOUCHED) {
        // draw bar upper selected key
        keypadDrawSelectedTouched(keypad);
    }
    else if (buttonEvent == BUTTON_BOTH_PRESSED) {
        keypad->callback(positionToChar(keypad->selectedKey));
    }
    else if ((buttonEvent == BUTTON_LEFT_PRESSED)
             || (buttonEvent == BUTTON_LEFT_CONTINUOUS_PRESSED)) {
        switch (keypad->selectedKey) {
            case 1:
                if (keypad->enableBackspace) {
                    keypad->selectedKey = 0;
                }
                else {
                    keypad->selectedKey = 10;
                }
                break;
            case 0:  // backspace
                if (keypad->enableValidate) {
                    keypad->selectedKey = 11;
                }
                else {
                    keypad->selectedKey = 10;
                }
                break;
            default:
                keypad->selectedKey--;
                break;
        }
        nbgl_screenRedraw();
    }
    else if ((buttonEvent == BUTTON_RIGHT_PRESSED)
             || (buttonEvent == BUTTON_RIGHT_CONTINUOUS_PRESSED)) {
        switch (keypad->selectedKey) {
            case 10:  // '9'
                if (keypad->enableValidate) {
                    keypad->selectedKey = 11;
                }
                else if (keypad->enableBackspace) {
                    keypad->selectedKey = 0;
                }
                else {
                    keypad->selectedKey = 1;
                }
                break;
            case 11:  // validate
                keypad->selectedKey = 0;
                break;
            default:
                keypad->selectedKey++;
                break;
        }
        nbgl_screenRedraw();
    }
}

/**
 * @brief This function draws a keypad object
 *
 * @param keypad keypad object to draw
 * @return the keypad object
 */
void nbgl_objDrawKeypad(nbgl_keypad_t *keypad)
{
    LOG_DEBUG(OBJ_LOGGER, "nbgl_objDrawKeypad keypad->shuffled= %d\n", keypad->shuffled);
    // draw digits content
    keypadDrawDigits(keypad);
    if (keypad->selectedKey == 0xFF) {
        keypadInitSelected(keypad);
    }
    keypadDrawSelected(keypad);
}

#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYPAD
