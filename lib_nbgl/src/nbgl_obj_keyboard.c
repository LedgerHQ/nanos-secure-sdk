
/**
 * @file nbgl_obj_keyboard.c
 * @brief The construction and touch management of a keyboard object
 *
 */

#ifdef NBGL_KEYBOARD
#ifdef HAVE_SE_TOUCH

/*********************
 *      INCLUDES
 *********************/
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

#define FIRST_LINE_CHAR_COUNT  10
#define SECOND_LINE_CHAR_COUNT 9

#define NORMAL_KEY_WIDTH                 40
#define LETTER_OFFSET_Y                  (((KEYBOARD_KEY_HEIGHT - 32) / 2) & 0xFFC)
#define SHIFT_KEY_WIDTH                  (NORMAL_KEY_WIDTH + SECOND_LINE_OFFSET)
#define SECOND_LINE_OFFSET               ((SCREEN_WIDTH - (SECOND_LINE_CHAR_COUNT * NORMAL_KEY_WIDTH)) / 2)
#define SPACE_KEY_WIDTH                  276
#define SWITCH_KEY_WIDTH                 (SCREEN_WIDTH - SPACE_KEY_WIDTH)
#define SPECIAL_CHARS_KEY_WIDTH          (NORMAL_KEY_WIDTH * 2 + SECOND_LINE_OFFSET)
#define BACKSPACE_KEY_WIDTH_FULL         SHIFT_KEY_WIDTH
#define BACKSPACE_KEY_WIDTH_DIGITS       SPECIAL_CHARS_KEY_WIDTH
#define BACKSPACE_KEY_WIDTH_LETTERS_ONLY (SCREEN_WIDTH - 7 * NORMAL_KEY_WIDTH)

#define SHIFT_KEY_INDEX         26
#define DIGITS_SWITCH_KEY_INDEX 27
#define BACKSPACE_KEY_INDEX     28
#define SPACE_KEY_INDEX         29
#define SPECIAL_KEYS_INDEX      30

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const char kbd_chars[]       = "qwertyuiopasdfghjklzxcvbnm";
static const char kbd_chars_upper[] = "QWERTYUIOPASDFGHJKLZXCVBNM";
const char        kbd_digits[]      = "1234567890-/:;()&@\".,?!\'";
static const char kbd_specials[]    = "[]{}#%^*+=_\\|~<>$`\".,?!\'";

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC FUNCTIONS
 **********************/
static uint8_t getKeyboardIndex(nbgl_keyboard_t *keyboard, nbgl_touchStatePosition_t *position)
{
    uint8_t i = 0;
    // get index of key pressed
    if (position->y < KEYBOARD_KEY_HEIGHT) {
        // 1st row:
        i = position->x / NORMAL_KEY_WIDTH;
    }
    else if (position->y < (2 * KEYBOARD_KEY_HEIGHT)) {
        // 2nd row:
        i = FIRST_LINE_CHAR_COUNT + (position->x - SECOND_LINE_OFFSET) / NORMAL_KEY_WIDTH;
        if (i >= FIRST_LINE_CHAR_COUNT + SECOND_LINE_CHAR_COUNT) {
            i = FIRST_LINE_CHAR_COUNT + SECOND_LINE_CHAR_COUNT - 1;
        }
    }
    else if (position->y < (3 * KEYBOARD_KEY_HEIGHT)) {
        // 3rd row:
        if (keyboard->mode == MODE_LETTERS) {
            // shift does not exist in letters only mode
            if (!keyboard->lettersOnly) {
                if (position->x < SHIFT_KEY_WIDTH) {
                    i = SHIFT_KEY_INDEX;
                }
                else {
                    i = FIRST_LINE_CHAR_COUNT + SECOND_LINE_CHAR_COUNT
                        + (position->x - SHIFT_KEY_WIDTH) / NORMAL_KEY_WIDTH;
                    // Backspace key is a bit larger...
                    if (i >= 26) {
                        i = BACKSPACE_KEY_INDEX;
                    }
                }
            }
            else {
                i = FIRST_LINE_CHAR_COUNT + SECOND_LINE_CHAR_COUNT + position->x / NORMAL_KEY_WIDTH;
                // Backspace key is larger...
                if (i >= 26) {
                    i = BACKSPACE_KEY_INDEX;
                }
            }
        }
        else {
            if (position->x < SPECIAL_CHARS_KEY_WIDTH) {
                i = SPECIAL_KEYS_INDEX;
            }
            else {
                i = FIRST_LINE_CHAR_COUNT + SECOND_LINE_CHAR_COUNT
                    + (position->x - SPECIAL_CHARS_KEY_WIDTH) / NORMAL_KEY_WIDTH;
                // Backspace key is a bit larger...
                if (i >= 24) {
                    i = BACKSPACE_KEY_INDEX;
                }
            }
        }
    }
    else if (!keyboard->lettersOnly && (position->y < (4 * KEYBOARD_KEY_HEIGHT))) {
        // 4th row
        if (position->x < SWITCH_KEY_WIDTH) {
            i = DIGITS_SWITCH_KEY_INDEX;
        }
        else {
            i = SPACE_KEY_INDEX;
        }
    }
    return i;
}

// draw parts common to all modes of keyboard
static void keyboardDrawCommonLines(nbgl_keyboard_t *keyboard)
{
    nbgl_area_t rectArea;

    // clean full area
    rectArea.backgroundColor = keyboard->obj.area.backgroundColor;
    rectArea.x0              = keyboard->obj.area.x0;
    rectArea.y0              = keyboard->obj.area.y0;
    rectArea.width           = keyboard->obj.area.width;
    rectArea.height          = keyboard->obj.area.height;
    nbgl_frontDrawRect(&rectArea);

    /// draw horizontal lines
    rectArea.backgroundColor = keyboard->obj.area.backgroundColor;
    rectArea.x0              = keyboard->obj.area.x0;
    rectArea.y0              = keyboard->obj.area.y0;
    rectArea.width           = keyboard->obj.area.width;
    rectArea.height          = 4;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keyboard->borderColor);  // 1st line (top)
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keyboard->borderColor);  // 2nd line
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keyboard->borderColor);  // 3rd line
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keyboard->borderColor);  // 4th line
    if (!keyboard->lettersOnly) {
        rectArea.y0 += KEYBOARD_KEY_HEIGHT;
        nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keyboard->borderColor);  // 5th line
    }
    /// then draw vertical lines
    rectArea.backgroundColor = keyboard->borderColor;
    rectArea.x0              = keyboard->obj.area.x0;
    rectArea.y0              = keyboard->obj.area.y0;
    rectArea.width           = 1;
    rectArea.height          = KEYBOARD_KEY_HEIGHT * 3;
    if (!keyboard->lettersOnly) {
        rectArea.height += KEYBOARD_KEY_HEIGHT;
    }
    nbgl_frontDrawRect(&rectArea);  // 1st full line, on the left
}

// draw full grid for letters mode
static void keyboardDrawLetterGrid(nbgl_keyboard_t *keyboard)
{
    nbgl_area_t rectArea;
    uint8_t     i;

    /// draw common lines
    keyboardDrawCommonLines(keyboard);

    // then all vertical lines separating keys
    rectArea.backgroundColor = keyboard->borderColor;
    rectArea.x0              = keyboard->obj.area.x0;
    rectArea.y0              = keyboard->obj.area.y0;
    rectArea.width           = 1;
    rectArea.height          = KEYBOARD_KEY_HEIGHT;
    // First row of keys: 10 letters (qwertyuiop)
    for (i = 0; i < 9; i++) {
        rectArea.x0 += NORMAL_KEY_WIDTH;
        nbgl_frontDrawRect(&rectArea);
    }

    // Second row: 9 letters (asdfghjkl)
    rectArea.x0 = keyboard->obj.area.x0 + SECOND_LINE_OFFSET;
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    nbgl_frontDrawRect(&rectArea);
    for (i = 10; i < 19; i++) {
        rectArea.x0 += NORMAL_KEY_WIDTH;
        nbgl_frontDrawRect(&rectArea);
    }
    // Third row: Shift key, 7 letters (zxcvbnm) and backspace in normal mode
    // Third row: 7 letters (zxcvbnm) and backspace in letters only mode
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    if (!keyboard->lettersOnly) {
        rectArea.x0 = keyboard->obj.area.x0 + SHIFT_KEY_WIDTH;
    }
    else {
        rectArea.x0 = NORMAL_KEY_WIDTH;
    }
    for (i = 0; i < 6; i++) {
        nbgl_frontDrawRect(&rectArea);
        rectArea.x0 += NORMAL_KEY_WIDTH;
    }
    if (!keyboard->lettersOnly) {
        nbgl_frontDrawRect(&rectArea);
        rectArea.x0 += NORMAL_KEY_WIDTH;
    }
    nbgl_frontDrawRect(&rectArea);

    // 4th row, only in Full mode
    if (!keyboard->lettersOnly) {
        rectArea.y0 += KEYBOARD_KEY_HEIGHT;
        rectArea.x0 = keyboard->obj.area.x0 + SWITCH_KEY_WIDTH;
        nbgl_frontDrawRect(&rectArea);
    }
}

// draw full grid for digits/special mode
static void keyboardDrawDigitsGrid(nbgl_keyboard_t *keyboard)
{
    nbgl_area_t rectArea;
    uint8_t     i;

    /// draw common lines
    keyboardDrawCommonLines(keyboard);

    // then all vertical lines separating keys
    rectArea.backgroundColor = keyboard->borderColor;
    rectArea.x0              = keyboard->obj.area.x0;
    rectArea.y0              = keyboard->obj.area.y0;
    rectArea.width           = 1;
    rectArea.height          = KEYBOARD_KEY_HEIGHT;
    // First row of keys: 10 keys so 9 separations
    for (i = 0; i < 9; i++) {
        rectArea.x0 += NORMAL_KEY_WIDTH;
        nbgl_frontDrawRect(&rectArea);
    }

    // Second row: 9 keys
    rectArea.x0 = keyboard->obj.area.x0 + SECOND_LINE_OFFSET;
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    nbgl_frontDrawRect(&rectArea);
    for (i = 10; i < 19; i++) {
        rectArea.x0 += NORMAL_KEY_WIDTH;
        nbgl_frontDrawRect(&rectArea);
    }
    // Third row: Special char key, 5 keys and backspace
    rectArea.x0 = keyboard->obj.area.x0 + SPECIAL_CHARS_KEY_WIDTH;
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    nbgl_frontDrawRect(&rectArea);
    for (i = 0; i < 5; i++) {
        rectArea.x0 += NORMAL_KEY_WIDTH;
        nbgl_frontDrawRect(&rectArea);
    }

    // 4th row, switch to letters and space
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    rectArea.x0 = keyboard->obj.area.x0 + SWITCH_KEY_WIDTH;
    nbgl_frontDrawRect(&rectArea);
}

// draw letters for letters mode
static void keyboardDrawLetters(nbgl_keyboard_t *keyboard)
{
    uint8_t     i;
    nbgl_area_t rectArea;
    const char *keys;

    if (keyboard->casing != LOWER_CASE) {
        keys = kbd_chars_upper;
    }
    else {
        keys = kbd_chars;
    }

    rectArea.backgroundColor = keyboard->obj.area.backgroundColor;
    rectArea.y0              = keyboard->obj.area.y0 + LETTER_OFFSET_Y;
    rectArea.width           = 1;
    rectArea.height          = KEYBOARD_KEY_HEIGHT * 3;
    rectArea.x0              = keyboard->obj.area.x0;

    // First row of keys: 10 letters (qwertyuiop)
    for (i = 0; i < 10; i++) {
        rectArea.x0 = keyboard->obj.area.x0 + i * NORMAL_KEY_WIDTH;

        rectArea.x0
            += (NORMAL_KEY_WIDTH - nbgl_getCharWidth(SMALL_REGULAR_1BPP_FONT, &keys[i])) / 2;
        nbgl_drawText(&rectArea,
                      &keys[i],
                      1,
                      SMALL_REGULAR_1BPP_FONT,
                      (keyboard->keyMask & (1 << i)) ? WHITE : BLACK);
    }
    // Second row: 9 letters (asdfghjkl)
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    for (i = 10; i < 19; i++) {
        rectArea.x0 = keyboard->obj.area.x0 + SECOND_LINE_OFFSET + (i - 10) * NORMAL_KEY_WIDTH;
        rectArea.x0
            += (NORMAL_KEY_WIDTH - nbgl_getCharWidth(SMALL_REGULAR_1BPP_FONT, &keys[i])) / 2;
        nbgl_drawText(&rectArea,
                      &keys[i],
                      1,
                      SMALL_REGULAR_1BPP_FONT,
                      (keyboard->keyMask & (1 << i)) ? WHITE : BLACK);
    }
    // Third row: Shift key, 7 letters (zxcvbnm) and backspace
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    uint16_t offsetX;
    if (!keyboard->lettersOnly) {
        // draw background rectangle
        rectArea.width           = SHIFT_KEY_WIDTH - 1;
        rectArea.height          = KEYBOARD_KEY_HEIGHT;
        rectArea.bpp             = NBGL_BPP_1;
        rectArea.y0              = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2;
        rectArea.x0              = 1;
        rectArea.backgroundColor = (keyboard->casing != LOWER_CASE) ? BLACK : WHITE;
        nbgl_frontDrawRect(&rectArea);
        // draw horizontal line
        rectArea.width           = SHIFT_KEY_WIDTH - 1;
        rectArea.height          = 4;
        rectArea.x0              = 1;
        rectArea.y0              = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2;
        rectArea.backgroundColor = (keyboard->casing != LOWER_CASE) ? BLACK : WHITE;
        nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keyboard->borderColor);
        // draw Shift key
        rectArea.width  = C_shift32px.width;
        rectArea.height = C_shift32px.height;
        rectArea.bpp    = NBGL_BPP_1;
        rectArea.y0     = (keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2
                       + (KEYBOARD_KEY_HEIGHT - rectArea.height) / 2)
                      & 0xFFC;
        rectArea.x0              = (SHIFT_KEY_WIDTH - rectArea.width) / 2;
        rectArea.backgroundColor = (keyboard->casing != LOWER_CASE) ? BLACK : WHITE;
        nbgl_drawIcon(
            &rectArea,
            (keyboard->casing != LOWER_CASE) ? WHITE : BLACK,
            (keyboard->casing != LOCKED_UPPER_CASE) ? (&C_shift_lock32px) : (&C_shift32px));
        rectArea.backgroundColor = WHITE;
        offsetX                  = keyboard->obj.area.x0 + SHIFT_KEY_WIDTH;
    }
    else {
        offsetX = 0;
    }
    rectArea.y0 = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2 + LETTER_OFFSET_Y;
    for (i = 19; i < 26; i++) {
        rectArea.x0 = offsetX + (i - 19) * NORMAL_KEY_WIDTH;
        rectArea.x0
            += (NORMAL_KEY_WIDTH - nbgl_getCharWidth(SMALL_REGULAR_1BPP_FONT, &keys[i])) / 2;
        nbgl_drawText(&rectArea,
                      &keys[i],
                      1,
                      SMALL_REGULAR_1BPP_FONT,
                      (keyboard->keyMask & (1 << i)) ? WHITE : BLACK);
    }
    // draw backspace
    rectArea.width  = C_backspace32px.width;
    rectArea.height = C_backspace32px.height;
    rectArea.bpp    = NBGL_BPP_1;
    rectArea.x0     = offsetX + 7 * NORMAL_KEY_WIDTH;
    rectArea.y0     = (keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2
                   + (KEYBOARD_KEY_HEIGHT - rectArea.height) / 2)
                  & 0xFFC;
    if (!keyboard->lettersOnly) {
        rectArea.x0 += (BACKSPACE_KEY_WIDTH_FULL - rectArea.width) / 2;
    }
    else {
        rectArea.x0 += (BACKSPACE_KEY_WIDTH_LETTERS_ONLY - rectArea.width) / 2;
    }
    nbgl_drawIcon(&rectArea, (keyboard->keyMask & (1 << 26)) ? WHITE : BLACK, &C_backspace32px);

    // 4th row, only in Full mode
    if (!keyboard->lettersOnly) {
        rectArea.x0 = (SWITCH_KEY_WIDTH - nbgl_getTextWidth(SMALL_REGULAR_1BPP_FONT, ".?123")) / 2;
        rectArea.y0 = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 3 + LETTER_OFFSET_Y;
        nbgl_drawText(&rectArea, ".?123", 5, SMALL_REGULAR_1BPP_FONT, BLACK);

        rectArea.x0 = SWITCH_KEY_WIDTH + (SPACE_KEY_WIDTH - C_space32px.width) / 2;
        nbgl_drawIcon(
            &rectArea, (keyboard->keyMask & (1 << SPACE_KEY_INDEX)) ? WHITE : BLACK, &C_space32px);
    }
}

// draw digits/special chars for digits/special mode
static void keyboardDrawDigits(nbgl_keyboard_t *keyboard)
{
    uint8_t     i;
    nbgl_area_t rectArea;
    const char *keys;

    if (keyboard->mode == MODE_DIGITS) {
        keys = kbd_digits;
    }
    else {
        keys = kbd_specials;
    }

    rectArea.backgroundColor = keyboard->obj.area.backgroundColor;
    rectArea.y0              = keyboard->obj.area.y0 + LETTER_OFFSET_Y;
    rectArea.width           = 1;
    rectArea.height          = KEYBOARD_KEY_HEIGHT * 3;
    rectArea.x0              = keyboard->obj.area.x0;

    // First row of keys: 10 digits (1234567890)
    for (i = 0; i < 10; i++) {
        rectArea.x0 = keyboard->obj.area.x0 + i * NORMAL_KEY_WIDTH;
        rectArea.x0
            += (NORMAL_KEY_WIDTH - nbgl_getCharWidth(SMALL_REGULAR_1BPP_FONT, &keys[i])) / 2;
        nbgl_drawText(&rectArea,
                      &keys[i],
                      1,
                      SMALL_REGULAR_1BPP_FONT,
                      (keyboard->keyMask & (1 << i)) ? WHITE : BLACK);
    }
    // Second row: 9 keys ()
    rectArea.y0 += KEYBOARD_KEY_HEIGHT;
    for (i = 10; i < 19; i++) {
        rectArea.x0 = keyboard->obj.area.x0 + (i - 10) * NORMAL_KEY_WIDTH + SECOND_LINE_OFFSET;
        rectArea.x0
            += (NORMAL_KEY_WIDTH - nbgl_getCharWidth(SMALL_REGULAR_1BPP_FONT, &keys[i])) / 2;
        nbgl_drawText(&rectArea,
                      &keys[i],
                      1,
                      SMALL_REGULAR_1BPP_FONT,
                      (keyboard->keyMask & (1 << i)) ? WHITE : BLACK);
    }
    // Third row: special key, 5 keys and backspace

    // draw "#+=" key
    if (keyboard->mode == MODE_DIGITS) {
        rectArea.x0
            = (SPECIAL_CHARS_KEY_WIDTH - nbgl_getTextWidth(SMALL_REGULAR_1BPP_FONT, "#+=")) / 2;
        rectArea.y0 = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2 + LETTER_OFFSET_Y;
        nbgl_drawText(&rectArea, "#+=", 3, SMALL_REGULAR_1BPP_FONT, BLACK);
    }
    else {
        rectArea.x0
            = (SPECIAL_CHARS_KEY_WIDTH - nbgl_getTextWidth(SMALL_REGULAR_1BPP_FONT, "123")) / 2;
        rectArea.y0 = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2 + LETTER_OFFSET_Y;
        nbgl_drawText(&rectArea, "123", 3, SMALL_REGULAR_1BPP_FONT, BLACK);
    }

    for (i = 19; i < 24; i++) {
        rectArea.x0 = SPECIAL_CHARS_KEY_WIDTH + (i - 19) * NORMAL_KEY_WIDTH;
        rectArea.x0
            += (NORMAL_KEY_WIDTH - nbgl_getCharWidth(SMALL_REGULAR_1BPP_FONT, &keys[i])) / 2;
        nbgl_drawText(&rectArea,
                      &keys[i],
                      1,
                      SMALL_REGULAR_1BPP_FONT,
                      (keyboard->keyMask & (1 << i)) ? WHITE : BLACK);
    }
    // draw backspace
    rectArea.width  = C_backspace32px.width;
    rectArea.height = C_backspace32px.height;
    rectArea.bpp    = NBGL_BPP_1;
    rectArea.x0     = SPECIAL_CHARS_KEY_WIDTH + 5 * NORMAL_KEY_WIDTH;
    rectArea.y0     = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 2
                  + (((KEYBOARD_KEY_HEIGHT - rectArea.height) / 2) & 0xFFC);
    rectArea.x0 += (BACKSPACE_KEY_WIDTH_DIGITS - rectArea.width) / 2;
    nbgl_drawIcon(&rectArea, BLACK, &C_backspace32px);

    // 4th row
    rectArea.x0 = (SWITCH_KEY_WIDTH - nbgl_getTextWidth(SMALL_REGULAR_1BPP_FONT, "ABC")) / 2;
    rectArea.y0 = keyboard->obj.area.y0 + KEYBOARD_KEY_HEIGHT * 3 + LETTER_OFFSET_Y;
    nbgl_drawText(&rectArea, "ABC", 3, SMALL_REGULAR_1BPP_FONT, BLACK);

    rectArea.x0 = SWITCH_KEY_WIDTH + (SPACE_KEY_WIDTH - C_space32px.width) / 2;
    nbgl_drawIcon(
        &rectArea, (keyboard->keyMask & (1 << SPACE_KEY_INDEX)) ? WHITE : BLACK, &C_space32px);
}

static void keyboardDraw(nbgl_keyboard_t *keyboard)
{
    if (keyboard->mode == MODE_LETTERS) {
        // At first, draw grid
        keyboardDrawLetterGrid(keyboard);

        // then draw key content
        keyboardDrawLetters(keyboard);
    }
    else {
        ////// At first, draw grid //////
        keyboardDrawDigitsGrid(keyboard);

        ////// then draw key content //////
        keyboardDrawDigits(keyboard);
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief function to be called when the keyboard object is touched
 *
 * @param obj touched object (keyboard)
 * @param eventType type of touch (only TOUCHED is accepted)
 * @return none
 */
void nbgl_keyboardTouchCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType)
{
    uint8_t                    firstIndex, lastIndex;
    nbgl_touchStatePosition_t *firstPosition, *lastPosition;
    nbgl_keyboard_t           *keyboard = (nbgl_keyboard_t *) obj;

    LOG_DEBUG(MISC_LOGGER, "keyboardTouchCallback(): eventType = %d\n", eventType);
    if (eventType != TOUCHED) {
        return;
    }
    if (nbgl_touchGetTouchedPosition(obj, &firstPosition, &lastPosition) == false) {
        return;
    }
    // modify positions with keyboard position
    firstPosition->x -= obj->area.x0;
    firstPosition->y -= obj->area.y0;
    lastPosition->x -= obj->area.x0;
    lastPosition->y -= obj->area.y0;

    firstIndex = getKeyboardIndex(keyboard, firstPosition);
    if (firstIndex > SPECIAL_KEYS_INDEX) {
        return;
    }
    lastIndex = getKeyboardIndex(keyboard, lastPosition);
    if (lastIndex > SPECIAL_KEYS_INDEX) {
        return;
    }
    // if position of finger has moved durinng press to another "key", drop it
    if (lastIndex != firstIndex) {
        return;
    }

    if (keyboard->mode == MODE_LETTERS) {
        keyboardCase_t cur_casing = keyboard->casing;
        // if the casing mode was upper (not-locked), go back to lower case
        if ((keyboard->casing == UPPER_CASE) && (firstIndex != SHIFT_KEY_INDEX)) {
            keyboard->casing = LOWER_CASE;
            // just redraw, refresh will be done by client (user of keyboard)
            nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
            keyboard->needsRefresh = true;
        }
        if ((firstIndex < 26) && ((keyboard->keyMask & (1 << firstIndex)) == 0)) {
            keyboard->callback((cur_casing != LOWER_CASE) ? kbd_chars_upper[firstIndex]
                                                          : kbd_chars[firstIndex]);
        }
        else if (firstIndex == SHIFT_KEY_INDEX) {
            switch (keyboard->casing) {
                case LOWER_CASE:
                    keyboard->casing = UPPER_CASE;
                    break;
                case UPPER_CASE:
                    keyboard->casing = LOCKED_UPPER_CASE;
                    break;
                case LOCKED_UPPER_CASE:
                    keyboard->casing = LOWER_CASE;
                    break;
            }
            nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
            nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_REFRESH,
                                               POST_REFRESH_FORCE_POWER_ON);
        }
        else if (firstIndex == DIGITS_SWITCH_KEY_INDEX) {  // switch to digits
            keyboard->mode = MODE_DIGITS;
            nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
            nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_REFRESH,
                                               POST_REFRESH_FORCE_POWER_ON);
        }
    }
    else if (keyboard->mode == MODE_DIGITS) {
        if (firstIndex < 26) {
            keyboard->callback(kbd_digits[firstIndex]);
        }
        else if (firstIndex == SPECIAL_KEYS_INDEX) {
            keyboard->mode = MODE_SPECIAL;
            nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
            nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_REFRESH,
                                               POST_REFRESH_FORCE_POWER_ON);
        }
        else if (firstIndex == DIGITS_SWITCH_KEY_INDEX) {  // switch to letters
            keyboard->mode = MODE_LETTERS;
            nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
            nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_REFRESH,
                                               POST_REFRESH_FORCE_POWER_ON);
        }
    }
    else if (keyboard->mode == MODE_SPECIAL) {
        if (firstIndex < 26) {
            keyboard->callback(kbd_specials[firstIndex]);
        }
        else if (firstIndex == SPECIAL_KEYS_INDEX) {
            keyboard->mode = MODE_DIGITS;
            nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
            nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_REFRESH,
                                               POST_REFRESH_FORCE_POWER_ON);
        }
        else if (firstIndex == DIGITS_SWITCH_KEY_INDEX) {  // switch to letters
            keyboard->mode = MODE_LETTERS;
            nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);
            nbgl_refreshSpecialWithPostRefresh(BLACK_AND_WHITE_REFRESH,
                                               POST_REFRESH_FORCE_POWER_ON);
        }
    }
    if (firstIndex == BACKSPACE_KEY_INDEX) {  // backspace
        keyboard->callback(BACKSPACE_KEY);
    }
    else if ((firstIndex == SPACE_KEY_INDEX)
             && ((keyboard->keyMask & (1 << SPACE_KEY_INDEX)) == 0)) {  // space
        keyboard->callback(' ');
    }
}

/**
 * @brief This function gets the position (top-left corner) of the key at the
 * given index. (to be used for Testing purpose)
 *
 * @param kbd the object to be drawned
 * @param index ascii character (in lower-case)
 * @param x [out] the top-left position
 * @param y [out] the top-left position
 * @return true if found, false otherwise
 */
bool nbgl_keyboardGetPosition(nbgl_keyboard_t *kbd, char index, uint16_t *x, uint16_t *y)
{
    uint8_t charIndex = 0;

    while (charIndex < 26) {
        if (index == kbd_chars[charIndex]) {
            break;
        }
        charIndex++;
    }

    // if in first line
    if (charIndex < FIRST_LINE_CHAR_COUNT) {
        *x = kbd->obj.area.x0 + charIndex * NORMAL_KEY_WIDTH;
        *y = kbd->obj.area.y0;
    }
    else if (charIndex < (FIRST_LINE_CHAR_COUNT + SECOND_LINE_CHAR_COUNT)) {
        *x = kbd->obj.area.x0 + (charIndex - FIRST_LINE_CHAR_COUNT) * NORMAL_KEY_WIDTH
             + SECOND_LINE_OFFSET;
        *y = kbd->obj.area.y0 + KEYBOARD_KEY_HEIGHT;
    }
    else if (charIndex < sizeof(kbd_chars)) {
        if (kbd->mode == MODE_LETTERS) {
            *x = kbd->obj.area.x0
                 + (charIndex - FIRST_LINE_CHAR_COUNT - SECOND_LINE_CHAR_COUNT) * NORMAL_KEY_WIDTH;
            // shift does not exist in letters only mode
            if (!kbd->lettersOnly) {
                *x = *x + SHIFT_KEY_WIDTH;
            }
        }
        else {
            *x = kbd->obj.area.x0
                 + (charIndex - FIRST_LINE_CHAR_COUNT - SECOND_LINE_CHAR_COUNT) * NORMAL_KEY_WIDTH
                 + SPECIAL_CHARS_KEY_WIDTH;
        }
        *y = kbd->obj.area.y0 + 2 * KEYBOARD_KEY_HEIGHT;
    }
    else {
        return false;
    }
    return true;
}

/**
 * @brief This function draws a keyboard object
 *
 * @param kbd the object to be drawned
 */
void nbgl_objDrawKeyboard(nbgl_keyboard_t *kbd)
{
    kbd->obj.touchMask = (1 << TOUCHED);
    kbd->obj.touchId   = KEYBOARD_ID;
    kbd->needsRefresh  = false;

    keyboardDraw(kbd);

    // If a keyboard in the screen, exclude only top border from touch, to avoid missing touch on
    // left keys
    touch_exclude_borders(TOP_BORDER);
}
#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYBOARD
