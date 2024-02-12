
/**
 * @file nbgl_obj_keypad.c
 * @brief The construction and touch management of a keypad object
 *
 */

#ifdef NBGL_KEYPAD
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
#include "os_io_seproxyhal.h"
#include "lcx_rng.h"

/*********************
 *      DEFINES
 *********************/
#define KEY_WIDTH      (SCREEN_WIDTH / 3)
#define DIGIT_OFFSET_Y (((KEYPAD_KEY_HEIGHT - 48) / 2) & 0xFFC)

#define BACKSPACE_KEY_INDEX 10
#define VALIDATE_KEY_INDEX  11

// to save RAM we use 5 uint8, and 4 bits per digit (MSBs for odd digits, LSBs for even ones)
#define GET_DIGIT_INDEX(_keypad, _digit)                      \
    ((_digit & 1) ? (_keypad->digitIndexes[_digit >> 1] >> 4) \
                  : (_keypad->digitIndexes[_digit >> 1] & 0xF))
#define SET_DIGIT_INDEX(_keypad, _digit, _index) \
    (_keypad->digitIndexes[_digit >> 1] |= (_digit & 1) ? (_index << 4) : _index)

extern uint8_t ramBuffer[GZLIB_UNCOMPRESSED_CHUNK];

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC FUNCTIONS
 **********************/

static uint8_t getKeypadIndex(uint16_t x, uint16_t y)
{
    uint8_t i = 0;
    // get index of key pressed
    if (y < KEYPAD_KEY_HEIGHT) {
        // 1st line:
        i = 1 + x / KEY_WIDTH;
    }
    else if (y < (2 * KEYPAD_KEY_HEIGHT)) {
        // 2nd line:
        i = 4 + x / KEY_WIDTH;
    }
    else if (y < (3 * KEYPAD_KEY_HEIGHT)) {
        // 3rd line:
        i = 7 + x / KEY_WIDTH;
    }
    else if (y < (4 * KEYPAD_KEY_HEIGHT)) {
        // 4th line
        if (x < KEY_WIDTH) {
            i = BACKSPACE_KEY_INDEX;
        }
        else if (x < (2 * KEY_WIDTH)) {
            i = 0;
        }
        else {
            i = VALIDATE_KEY_INDEX;
        }
    }
    return i;
}

static void keypadDrawGrid(nbgl_keypad_t *keypad)
{
    nbgl_area_t rectArea;

    // clean full area
    rectArea.backgroundColor = keypad->obj.area.backgroundColor;
    rectArea.x0              = keypad->obj.area.x0;
    rectArea.y0              = keypad->obj.area.y0;
    rectArea.width           = keypad->obj.area.width;
    rectArea.height          = keypad->obj.area.height;
    nbgl_frontDrawRect(&rectArea);

    /// draw horizontal lines
    rectArea.backgroundColor = keypad->obj.area.backgroundColor;
    rectArea.x0              = keypad->obj.area.x0;
    rectArea.y0              = keypad->obj.area.y0;
    rectArea.width           = keypad->obj.area.width;
    rectArea.height          = 4;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keypad->borderColor);  // 1st line (top)
    rectArea.y0 += KEYPAD_KEY_HEIGHT;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keypad->borderColor);  // 2nd line
    rectArea.y0 += KEYPAD_KEY_HEIGHT;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keypad->borderColor);  // 3rd line
    rectArea.y0 += KEYPAD_KEY_HEIGHT;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x1, keypad->borderColor);  // 4th line

    /// then draw 3 vertical lines
    rectArea.backgroundColor = keypad->borderColor;
    rectArea.x0              = keypad->obj.area.x0;
    rectArea.y0              = keypad->obj.area.y0;
    rectArea.width           = 1;
    rectArea.height          = KEYPAD_KEY_HEIGHT * 4;
    nbgl_frontDrawRect(&rectArea);  // 1st full line, on the left
    rectArea.x0 += KEY_WIDTH;
    nbgl_frontDrawRect(&rectArea);  // 2nd line
    rectArea.x0 += KEY_WIDTH;
    nbgl_frontDrawRect(&rectArea);  // 3rd line
}

static void keypadDrawDigits(nbgl_keypad_t *keypad)
{
    uint8_t     i;
    nbgl_area_t rectArea;
    char        key_value;

    rectArea.backgroundColor = keypad->obj.area.backgroundColor;
    rectArea.y0              = keypad->obj.area.y0 + DIGIT_OFFSET_Y;

    // First row of keys: 1 2 3
    for (i = 0; i < 3; i++) {
        key_value = GET_DIGIT_INDEX(keypad, (i + 1)) + 0x30;

        rectArea.x0 = keypad->obj.area.x0 + i * KEY_WIDTH;
        rectArea.x0 += (KEY_WIDTH - nbgl_getCharWidth(LARGE_MEDIUM_FONT, &key_value)) / 2;
        nbgl_drawText(
            &rectArea, &key_value, 1, LARGE_MEDIUM_FONT, keypad->enableDigits ? BLACK : WHITE);
    }
    // Second row: 4 5 6
    rectArea.y0 += KEYPAD_KEY_HEIGHT;
    for (; i < 6; i++) {
        key_value   = GET_DIGIT_INDEX(keypad, (i + 1)) + 0x30;
        rectArea.x0 = keypad->obj.area.x0 + (i - 3) * KEY_WIDTH;
        rectArea.x0 += (KEY_WIDTH - nbgl_getCharWidth(LARGE_MEDIUM_FONT, &key_value)) / 2;
        nbgl_drawText(
            &rectArea, &key_value, 1, LARGE_MEDIUM_FONT, keypad->enableDigits ? BLACK : WHITE);
    }
    // Third row: 7 8 9
    rectArea.y0 += KEYPAD_KEY_HEIGHT;
    for (; i < 9; i++) {
        key_value   = GET_DIGIT_INDEX(keypad, (i + 1)) + 0x30;
        rectArea.x0 = keypad->obj.area.x0 + (i - 6) * KEY_WIDTH;
        rectArea.x0 += (KEY_WIDTH - nbgl_getCharWidth(LARGE_MEDIUM_FONT, &key_value)) / 2;
        nbgl_drawText(
            &rectArea, &key_value, 1, LARGE_MEDIUM_FONT, keypad->enableDigits ? BLACK : WHITE);
    }
    // 4th raw, Backspace, 0 and Validate
    // draw backspace
    rectArea.width  = BACKSPACE_ICON.width;
    rectArea.height = BACKSPACE_ICON.height;
    rectArea.bpp    = NBGL_BPP_1;
    rectArea.x0     = keypad->obj.area.x0 + (KEY_WIDTH - rectArea.width) / 2;
    rectArea.y0
        = keypad->obj.area.y0 + KEYPAD_KEY_HEIGHT * 3 + (KEYPAD_KEY_HEIGHT - rectArea.height) / 2;
#if GLYPH_backspace32px_ISFILE
    nbgl_frontDrawImageFile(&rectArea,
                            (uint8_t *) BACKSPACE_ICON.bitmap,
                            keypad->enableBackspace ? BLACK : WHITE,
                            ramBuffer);
#else
    nbgl_frontDrawImage(&rectArea,
                        (uint8_t *) BACKSPACE_ICON.bitmap,
                        NO_TRANSFORMATION,
                        keypad->enableBackspace ? BLACK : WHITE);
#endif

    // draw 0
    key_value   = GET_DIGIT_INDEX(keypad, 0) + 0x30;
    rectArea.x0 = keypad->obj.area.x0 + KEY_WIDTH;
    rectArea.x0 += (KEY_WIDTH - nbgl_getCharWidth(LARGE_MEDIUM_FONT, &key_value)) / 2;
    rectArea.y0 = keypad->obj.area.y0 + KEYPAD_KEY_HEIGHT * 3 + DIGIT_OFFSET_Y;
    nbgl_drawText(
        &rectArea, &key_value, 1, LARGE_MEDIUM_FONT, keypad->enableDigits ? BLACK : WHITE);

    // draw validate on gray with white background if not enabled
    if (!keypad->enableValidate) {
        rectArea.width  = VALIDATE_ICON.width;
        rectArea.height = VALIDATE_ICON.height;
        rectArea.bpp    = NBGL_BPP_1;
        rectArea.x0     = keypad->obj.area.x0 + 2 * KEY_WIDTH + (KEY_WIDTH - rectArea.width) / 2;
        rectArea.y0     = keypad->obj.area.y0 + KEYPAD_KEY_HEIGHT * 3
                      + (KEYPAD_KEY_HEIGHT - rectArea.height) / 2;
        rectArea.backgroundColor = WHITE;
        nbgl_frontDrawRect(&rectArea);
    }
    else {
        // if enabled, draw icon in white on a black background
        rectArea.backgroundColor = BLACK;
        rectArea.x0              = keypad->obj.area.x0 + 2 * KEY_WIDTH;
        rectArea.y0              = keypad->obj.area.y0 + KEYPAD_KEY_HEIGHT * 3;
        rectArea.width           = KEY_WIDTH;
        rectArea.height          = KEYPAD_KEY_HEIGHT;
        nbgl_frontDrawRect(&rectArea);
        rectArea.width  = VALIDATE_ICON.width;
        rectArea.height = VALIDATE_ICON.height;
        rectArea.bpp    = NBGL_BPP_1;
        rectArea.x0     = keypad->obj.area.x0 + 2 * KEY_WIDTH + (KEY_WIDTH - rectArea.width) / 2;
        rectArea.y0     = keypad->obj.area.y0 + KEYPAD_KEY_HEIGHT * 3
                      + (KEYPAD_KEY_HEIGHT - rectArea.height) / 2;
#if GLYPH_check32px_ISFILE
        nbgl_frontDrawImageFile(&rectArea, (uint8_t *) VALIDATE_ICON.bitmap, WHITE, ramBuffer);
#else
        nbgl_frontDrawImage(&rectArea, (uint8_t *) VALIDATE_ICON.bitmap, NO_TRANSFORMATION, WHITE);
#endif
    }
}

static void keypadDraw(nbgl_keypad_t *keypad)
{
    // At first, draw grid
    keypadDrawGrid(keypad);

    // then draw key content
    keypadDrawDigits(keypad);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief function to be called when the keypad object is touched
 *
 * @param obj touched object (keypad)
 * @param eventType type of touch (only TOUCHED is accepted)
 * @return none
 */
void nbgl_keypadTouchCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType)
{
    uint8_t                    firstIndex, lastIndex;
    nbgl_touchStatePosition_t *firstPosition, *lastPosition;
    nbgl_keypad_t             *keypad = (nbgl_keypad_t *) obj;

    LOG_DEBUG(MISC_LOGGER, "nbgl_keypadTouchCallback(): eventType = %d\n", eventType);
    if ((eventType != TOUCHED) && (eventType != TOUCH_PRESSED)) {
        return;
    }
    if (nbgl_touchGetTouchedPosition(obj, &firstPosition, &lastPosition) == false) {
        return;
    }

    // use positions relative to keypad position
    firstIndex = getKeypadIndex(firstPosition->x - obj->area.x0, firstPosition->y - obj->area.y0);
    if (firstIndex > VALIDATE_KEY_INDEX) {
        return;
    }
    lastIndex = getKeypadIndex(lastPosition->x - obj->area.x0, lastPosition->y - obj->area.y0);
    if (lastIndex > VALIDATE_KEY_INDEX) {
        return;
    }

    // if position of finger has moved during press to another "key", drop it
    if (lastIndex != firstIndex) {
        return;
    }

    if ((firstIndex < 10) && (keypad->enableDigits)) {
        // only call callback if event is TOUCHED, otherwise play tune on touch event (and not on
        // release)
        if (eventType == TOUCHED) {
            keypad->callback(GET_DIGIT_INDEX(keypad, firstIndex) + 0x30);
        }
        else {
            io_seproxyhal_play_tune(TUNE_TAP_CASUAL);
        }
    }
    if ((firstIndex == BACKSPACE_KEY_INDEX) && (keypad->enableBackspace)) {  // backspace
        // only call callback if event is TOUCHED, otherwise play tune on touch event (and not on
        // release)
        if (eventType == TOUCHED) {
            keypad->callback(BACKSPACE_KEY);
        }
        else {
            io_seproxyhal_play_tune(TUNE_TAP_CASUAL);
        }
    }
    else if ((firstIndex == VALIDATE_KEY_INDEX) && (keypad->enableValidate)) {  // validate
        // only call callback if event is TOUCHED
        if (eventType == TOUCHED) {
            keypad->callback(VALIDATE_KEY);
        }
    }
}

/**
 * @brief This function gets the position (top-left corner) of the key at the
 * given index. (to be used for Testing purpose). Only works without shuffling
 *
 * @param kpd the object to be drawned
 * @param index the char of the key
 * @param x [out] the top-left position
 * @param y [out] the top-left position
 * @return true if found, false otherwise
 */
bool nbgl_keypadGetPosition(nbgl_keypad_t *kpd, char index, uint16_t *x, uint16_t *y)
{
    // if in first line
    if ((index >= '1') && (index <= '3')) {
        *x = kpd->obj.area.x0 + (index - '1') * KEY_WIDTH;
        *y = kpd->obj.area.y0;
    }
    else if ((index >= '4') && (index <= '6')) {
        *x = kpd->obj.area.x0 + (index - '4') * KEY_WIDTH;
        *y = kpd->obj.area.y0 + KEYPAD_KEY_HEIGHT;
    }
    else if ((index >= '7') && (index <= '9')) {
        *x = kpd->obj.area.x0 + (index - '7') * KEY_WIDTH;
        *y = kpd->obj.area.y0 + (2 * KEYPAD_KEY_HEIGHT);
    }
    else if (index == BACKSPACE_KEY) {  // backspace
        *x = kpd->obj.area.x0;
        *y = kpd->obj.area.y0 + (3 * KEYPAD_KEY_HEIGHT);
    }
    else if (index == '0') {
        *x = kpd->obj.area.x0 + KEY_WIDTH;
        *y = kpd->obj.area.y0 + (3 * KEYPAD_KEY_HEIGHT);
    }
    else if (index == VALIDATE_KEY) {  // validate
        *x = kpd->obj.area.x0 + (2 * KEY_WIDTH);
        *y = kpd->obj.area.y0 + (3 * KEYPAD_KEY_HEIGHT);
    }
    else {
        return false;
    }
    return true;
}

/**
 * @brief This function draws a keypad object
 *
 * @param kpd keypad object to draw
 * @return the keypad keypad object
 */
void nbgl_objDrawKeypad(nbgl_keypad_t *kpd)
{
    kpd->obj.touchMask = (1 << TOUCHED) | (1 << TOUCH_PRESSED);
    kpd->obj.touchId   = KEYPAD_ID;

    // if the object has not been already used, prepare indexes of digits
    if (kpd->digitIndexes[0] == 0) {
        uint32_t i;
        if (kpd->shuffled) {
            uint8_t shuffledDigits[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

            // modern version of the Fisher-Yates shuffle
            for (i = 0; i < 9; i++) {
                // pick a random number k in [i:9] intervale
                uint32_t j   = cx_rng_u32_range(i, 10);
                uint8_t  tmp = shuffledDigits[j];

                // exchange shuffledDigits[i] and shuffledDigits[j]
                shuffledDigits[j] = shuffledDigits[i];
                shuffledDigits[i] = tmp;
            }
            for (i = 0; i < 10; i++) {
                // apply the permuted value to digit i
                SET_DIGIT_INDEX(kpd, i, shuffledDigits[i]);
            }
        }
        else {
            // no shuffling
            for (i = 0; i < 10; i++) {
                SET_DIGIT_INDEX(kpd, i, i);
            }
        }
    }
    keypadDraw(kpd);
}

#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYPAD
