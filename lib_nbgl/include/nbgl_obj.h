/**
 * @file nbgl_obj.h
 * @brief API to draw all basic graphic objects
 *
 */

#ifndef NBGL_OBJ_H
#define NBGL_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nbgl_types.h"
#include "nbgl_fonts.h"
#include "ux_loc.h"

/*********************
 *      INCLUDES
 *********************/


/*********************
 *      DEFINES
 *********************/
// Keypad special key values
#define BACKSPACE_KEY 8
#define VALIDATE_KEY '\r'

// for Keyboard
#define KEYBOARD_KEY_HEIGHT         56

// for Keypad
#define KEYPAD_KEY_HEIGHT           104

///< special code used by given callback of @ref nbgl_navigationPopulate to inform when Exit key is pressed
#define EXIT_PAGE 0xFF

// external margin in pixels
#define BORDER_MARGIN 24

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief All types of graphical objects.
 *
 */
typedef enum  {
    SCREEN,         ///< Main screen
    CONTAINER,      ///< Empty container
    PANEL,          ///< Container with special border
    IMAGE,          ///< Bitmap (x and width must be multiple of 4)
    LINE,           ///< Vertical or Horizontal line
    TEXT_AREA,      ///< Area to contain text line(s)
    BUTTON,         ///< Rounded rectangle button with icon and/or text
    SWITCH,         ///< Switch to turn on/off something
    PAGE_INDICATOR, ///< horizontal bar to indicate position within pages
    PROGRESS_BAR,   ///< horizontal bar to indicate progression of something (between 0% and 100%)
    RADIO_BUTTON,   ///< Indicator to inform whether something is on or off
    QR_CODE,        ///< QR Code
    KEYBOARD,       ///< Keyboard
    KEYPAD,         ///< Keypad
    SPINNER,        ///< Spinner
    IMAGE_FILE      ///< Image file (with Ledger compression)
} nbgl_obj_type_t;

/**
 * @brief All types of alignments.
 *
 */
typedef enum  {
    NO_ALIGNMENT, ///< used when parent container layout is used
    TOP_LEFT,
    TOP_MIDDLE,
    TOP_RIGHT,
    MID_LEFT,
    CENTER,
    MID_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_MIDDLE,
    BOTTOM_RIGHT,
    LEFT_TOP, ///< on outside left
    LEFT_BOTTOM, ///< on outside left
    RIGHT_TOP, ///< on outside right
    RIGHT_BOTTOM, ///< on outside right
} nbgl_aligment_t;

/**
 * @brief to represent a boolean state.
 */
typedef enum  {
    OFF_STATE,
    ON_STATE
} nbgl_state_t;

/**
 * @brief Directions for layout or lines
 *
 */
typedef enum  {
    VERTICAL, ///< from top to bottom
    HORIZONTAL ///< from left to right
} nbgl_direction_t;

/**
 * @brief possible styles for text area border
 *
 */
typedef enum  {
    NO_STYLE, ///< no border
    LEDGER_BORDER ///< Ledger style border, only for @ref TEXT_AREA
} nbgl_style_t;

/**
 * @brief possible modes for QR Code
 * @note if text len <= 114 chars, V4 can be used, otherwise use V10
 *
 */
typedef enum  {
    QRCODE_V4=0, ///< QRCode V4, can encode text len up to 114 chars
    QRCODE_V10 ///< QRCode V10, can encode text len up to 1500 chars
} nbgl_qrcode_version_t;

/**
 * @brief the 2 possible states of a finger on the Touchscreen
 *
 */
typedef enum {
    RELEASED, ///< the finger has been released from the screen
    PRESSED, ///< the finger is currently pressing the screen
} nbgl_touchState_t;

/**
 * @brief The different types of Touchscreen events
 *
 */
typedef enum {
    TOUCHED, ///< corresponding to an object touched and released at least SHORT_TOUCH_DURATION ms later but less than LONG_TOUCH_DURATION ms
    LONG_TOUCHED, ///< corresponding to an object touched and released at least LONG_TOUCH_DURATION ms later.
    TOUCHING, ///< corresponding to an object that is currently touched
    OUT_OF_TOUCH, ///< corresponding to an object that was touched but that has lost the focus (the finger has moved)
    TOUCH_PRESSED, ///< corresponding to an object that was not touched and where the finger has been pressed.
    TOUCH_RELEASED, ///< corresponding to an object that was touched and where the finger has been released.
    VALUE_CHANGED ///< corresponding to a change of state of the object (indirect event)
} nbgl_touchType_t;

/**
 * @brief The different styles of panels (temporary)
 *
 */
typedef enum {
    PLAIN_COLOR_PANEL = 0,
    ROUNDED_BORDER_PANEL ///< with a thin border in borderColor, with rounded corners
} nbgl_panelStyle_t;

/**
 * @brief The low level Touchscreen event, coming from driver
 *
 */
typedef struct {
  nbgl_touchState_t state; ///< state of the touch event, e.g @ref PRESSED or @ref RELEASED
  int16_t x; ///< horizontal position of the touch (or for a @ref RELEASED the last touched point)
  int16_t y; ///< vertical position of the touch (or for a @ref RELEASED the last touched point)
} nbgl_touchStatePosition_t;

/**
 * @brief prototype of function to be called when a touch event is received by an object
 * @param obj the concerned object
 * @param eventType type of touch event
 */
typedef void (*nbgl_touchCallback_t)(void *obj, nbgl_touchType_t eventType) ;

#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmicrosoft-anon-tag"
#endif // __clang__
/**
 * @brief Common structure for all graphical objects
 *
 * @note this type must never be instantiated
 */
typedef struct PACKED__ nbgl_obj_s {
    struct nbgl_area_s ; ///< absolute position, backGround color and size of the object. DO NOT MOVE THIS FIELD
    nbgl_obj_type_t type; ///< type of the graphical object, must be explicitly set
    int16_t rel_x0; ///< horizontal position of top-left corner relative to parent's top-left corner
    int16_t rel_y0; ///< vertical position of top-left corner relative to parent's top-left corner, must be multiple of 4
    struct nbgl_obj_s *parent; ///< parent of this object
    struct nbgl_obj_s *alignTo; ///< object to align to (parent by default)
    nbgl_aligment_t alignment; ///< type of alignment
    int16_t alignmentMarginX; ///< horizontal margin when aligning
    int16_t alignmentMarginY; ///< vertical margin when aligning
    uint8_t touchMask; ///< bit mask to tell engine which touch events are handled by this object
    nbgl_touchCallback_t touchCallback; ///< function to be called on events defined in @ref touchMask
} nbgl_obj_t;

/**
 * @brief struct to represent a container (@ref CONTAINER type)
 *
 * @note the main screen is a kind of container
 *
 */
typedef struct PACKED__ nbgl_container_s {
    struct nbgl_obj_s; ///< common part
    nbgl_direction_t layout; ///< layout of children inside this object
    uint8_t nbChildren;
    bool forceClean; ///< if set to true, a paint will be done with background color
    struct nbgl_obj_s **children; ///< children of this object (nbChildren size)
} nbgl_container_t;

/**
 * @brief struct to represent a panel (@ref PANEL type)
 * @note inherits from container
 *
 */
typedef struct PACKED__ nbgl_panel_s {
    struct nbgl_container_s; ///<  common part
    color_t borderColor; ///<  color of the border of the panel
} nbgl_panel_t;

/**
 * @brief struct to represent a vertical or horizontal line
 *
 */
typedef struct PACKED__ nbgl_line_s {
    struct nbgl_obj_s; ///<  common part
    nbgl_direction_t direction; ///< direction of the line, e.g @ref VERTICAL or @ref HORIZONTAL
    color_t lineColor; ///< color of the line
    uint8_t thickness; ///< thickness of the line in pixel, maybe different from height for horizontal line
    uint8_t offset; ///< the object height being always 4, with a y0 multiple of 4, this offset is use to move the line within these 4 pixels
} nbgl_line_t;

/**
 * @brief prototype of function to be called when a @ref IMAGE object is drawned, and no buffer was provided
 * @param token provided token in @ref IMAGE object
 * @return the icn details to be drawned in image object
 */
typedef nbgl_icon_details_t* (*onImageDrawCallback_t)(uint8_t token);

/**
 * @brief  struct to represent an image (@ref IMAGE type)
 *
 */
typedef struct PACKED__ nbgl_image_s {
    struct nbgl_obj_s; // common part
    color_t foregroundColor; ///< color set to '1' bits, for 1PBB images. '0' are set to background color.
    const nbgl_icon_details_t *buffer; ///< buffer containing bitmap, with exact same size as object (width*height*bpp/8 bytes)
    onImageDrawCallback_t onDrawCallback; ///< function called if buffer is NULL, with above token as parameter. Can be NULL
    uint8_t token; ///< token to use as param of onDrawCallback
} nbgl_image_t;

/**
 * @brief  struct to represent an image file object (@ref IMAGE_FILE type)
 * The source of the data is an image file with header. width and height are given in this header
 *
 */
typedef struct PACKED__ nbgl_image_file_s {
    struct nbgl_obj_s; // common part
    const uint8_t *buffer; ///< buffer containing image file
} nbgl_image_file_t;

/**
 * @brief  struct to represent a QR code (@ref QR_CODE type), whose size is fixed
 *
 */
typedef struct PACKED__ nbgl_qrcode_s {
    struct nbgl_obj_s; // common part
    color_t foregroundColor; ///< color set to '1' bits, for 1PBB images. '0' are set to background color.
    nbgl_qrcode_version_t version; ///< requested version, if V10, size will be fixed to 228*228, if V4, size will be fixed to 132*132
    char *text; ///< text single line (NULL terminated)
} nbgl_qrcode_t;

/**
 * @brief struct to represent a radio button (@ref RADIO_BUTTON type)
 *
 * @note size is fixed
 *
 */
typedef struct PACKED__ nbgl_radio_s {
    struct nbgl_obj_s; // common part
    color_t activeColor; ///< color set to to inner circle, when active.
    color_t borderColor; ///< color set to border.
    nbgl_state_t state; ///< state of the radio button. Active is when state == @ref ON_STATE
} nbgl_radio_t;

/**
 * @brief struct to represent a switch (size is fixed) (@ref SWITCH type)
 *
 */
typedef struct PACKED__ nbgl_switch_s {
    struct nbgl_obj_s; // common part
    color_t onColor; ///< color set to border and knob, when ON (knob on the right).
    color_t offColor; ///< color set to border and knob, when OFF (knob on the left).
    nbgl_state_t state; ///< state of the switch.
} nbgl_switch_t;

/**
 * @brief  struct to represent a progress bar (@ref PROGRESS_BAR type)
 * @note if withBorder, the stroke of the border is fixed (3 pixels)
 */
typedef struct PACKED__ nbgl_progress_bar_s {
    struct nbgl_obj_s; // common part
    bool withBorder; ///< if set to true, a border in black surround the whole object
    uint8_t state; ///< state of the progress, in % (from 0 to 100).
    color_t foregroundColor; ///< color of the inner progress bar and border (if applicable)
} nbgl_progress_bar_t;

/**
 * @brief  struct to represent a navigation bar (@ref PAGE_INDICATOR type)
 * There can be up to 5 page indicators, whose shape is fixed.
 * If there are more than 5 pages, the middle indicator will be "..."
 *
 * @note height is fixed
 */
typedef struct PACKED__ nbgl_navigation_bar_s {
    struct nbgl_obj_s; ///< common part
    uint8_t nbPages; ///< number of pages.
    uint8_t activePage; ///< index of active page (from 0 to nbPages-1).
} nbgl_page_indicator_t;

/**
 * @brief prototype of function to be called when a @ref TEXT_AREA object is drawned, and no text was provided
 * @param token provided token in @ref TEXT_AREA object
 * @return an ASCII string (null terminated) to be drawned in text area
 */
typedef char* (*onTextDrawCallback_t)(uint8_t token);

/**
 * @brief struct to represent a button (@ref BUTTON type)
 * that can contain a text and/or an icon
 * @note border width is fixed (2 pixels)
 *
 */
typedef struct PACKED__ nbgl_button_s {
    struct nbgl_obj_s; ///< common part
    color_t innerColor; ///< color set inside of the button
    color_t borderColor; ///< color set to button's border
    color_t foregroundColor; ///< color set to '1' bits in icon, and text. '0' are set to innerColor color.
    nbgl_radius_t radius; ///< radius of the corners, must be a multiple of 4.
    nbgl_font_id_e fontId; ///< id of the font to use, if any
    bool localized; ///< if set to true, means the following 'text' field is considered as a
    char *text; ///< single line UTF-8 text (NULL terminated)
#if defined(HAVE_LANGUAGE_PACK)
    UX_LOC_STRINGS_INDEX textId; ///< id of the text single line UTF-8 text
#endif // HAVE_LANGUAGE_PACK
    onTextDrawCallback_t onDrawCallback; ///< function called if not NULL, with above token as parameter to get the text of the button
    uint8_t token; ///< token to use as param of onDrawCallback
    const nbgl_icon_details_t *icon; ///< buffer containing icons bitmap. Set to NULL when no icon
} nbgl_button_t;

/**
 * @brief struct to represent a text area (@ref TEXT_AREA type)
 *
 */
typedef struct PACKED__ nbgl_text_area_s {
    struct nbgl_obj_s; ///< common part
    color_t textColor; ///< color set to '1' bits in text. '0' are set to backgroundColor color.
    nbgl_aligment_t textAlignment; ///< alignment of text within the area
    nbgl_style_t style; ///< to define the style of border
    nbgl_font_id_e fontId; ///< id of the font to use
    bool localized; ///< if set to true, use textId instead of text
    bool autoHideLongLine; ///< if set to true, replace beginning of line by ... to keep it single line
    bool wrapping; ///< if set to true, break lines on ' ' when possible
    uint8_t nbMaxLines; ///< if >0, replace end (3 last chars) of line (nbMaxLines-1) by "..." and stop display here
    char *text; ///< ASCII text to draw (NULL terminated). Can be NULL.
#if defined(HAVE_LANGUAGE_PACK)
    UX_LOC_STRINGS_INDEX textId; ///< id of the  UTF-8 text
#endif // HAVE_LANGUAGE_PACK
    onTextDrawCallback_t onDrawCallback; ///< function called if not NULL to get the text of the text area
    uint8_t token; ///< token to use as param of onDrawCallback
} nbgl_text_area_t;

/**
 * @brief struct to represent a "spinner", represented by the Ledger corners, in gray, with one of the corners in black (@ref SPINNER type)
 *
 */
typedef struct PACKED__ nbgl_spinner_s {
    struct nbgl_obj_s; ///< common part
    uint8_t position; ///< position of the spinner (from 0 to 3). If set to 0xFF, the spinner is entirely black
} nbgl_spinner_t;

/**
 * @brief prototype of function to be called when a valid key is pressed on keyboard
 * Backspace is equal to 0x8 (ASCII code), Validate (for Keypad) is equal to 15 ('\\r')
 * @param touchedKey char typed on keyboard
 */
typedef void (*keyboardCallback_t)(char touchedKey);

typedef enum {
  MODE_LETTERS=0,
  MODE_DIGITS,
  MODE_SPECIAL
} keyboardMode_t;

/**
 * @brief struct to represent a keyboard (@ref KEYBOARD type)
 *
 */
typedef struct PACKED__ nbgl_keyboard_s {
    struct nbgl_obj_s; ///< common part
    color_t textColor; ///< color set to letters.
    color_t borderColor; ///< color set to key borders
    bool lettersOnly; ///< if true, only display letter keys and Backspace
    bool upperCase; ///< if true, display letter keys in upper case
    keyboardMode_t mode; ///< keyboard mode to start with
    uint32_t keyMask; ///< mask used to disable some keys in letters only mod. The 26 LSB bits of mask are used, for the 26 letters of a QWERTY keyboard. Bit[0] for Q, Bit[1] for W and so on
    keyboardCallback_t callback; ///< function called when an active key is pressed
} nbgl_keyboard_t;


/**
 * @brief struct to represent a keypad (@ref KEYPAD type)
 *
 */
typedef struct PACKED__ nbgl_keypad_s {
    struct nbgl_obj_s; ///< common part
    color_t textColor; ///< color set to digits.
    color_t borderColor; ///< color set to key borders
    bool enableBackspace; ///< if true, Backspace key is enabled
    bool enableValidate; ///< if true, Validate key is enabled
    bool enableDigits; ///< if true, Digit keys are enabled
    bool shuffled; ///< if true, Digit keys are shuffled
    uint8_t digitIndexes[5]; ///< array of digits indexes, 4 bits per digit
    keyboardCallback_t callback; ///< function called when an active key is pressed
} nbgl_keypad_t;
#ifdef __clang__
#pragma GCC diagnostic pop
#endif // __clang__

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_redrawObject(nbgl_obj_t* obj, nbgl_obj_t *prevObj, bool computePosition);

void nbgl_refresh(void);
void nbgl_refreshSpecial(nbgl_refresh_mode_t mode);
void nbgl_refreshReset(void);

void nbgl_objInit(void);

void nbgl_objPoolRelease(uint8_t layer);
nbgl_obj_t* nbgl_objPoolGet(nbgl_obj_type_t type, uint8_t layer);
nbgl_obj_t* nbgl_objPoolGetPrevious(nbgl_obj_t* obj, uint8_t layer);
int nbgl_objPoolGetArray(nbgl_obj_type_t type, uint8_t nbObjs, uint8_t layer, nbgl_obj_t** objArray);
uint8_t nbgl_objPoolGetNbUsed(uint8_t layer);
void nbgl_containerPoolRelease(uint8_t layer);
nbgl_obj_t** nbgl_containerPoolGet(uint8_t nbObjs, uint8_t layer);
uint8_t nbgl_containerPoolGetNbUsed(uint8_t layer);

nbgl_container_t *nbgl_navigationPopulate(uint8_t nbPages, uint8_t activePage, bool withExitKey, nbgl_touchCallback_t callback, uint8_t layer);
uint8_t nbgl_navigationGetActivePage(void);
nbgl_container_t *nbgl_bottomButtonPopulate(const nbgl_icon_details_t *icon, nbgl_touchCallback_t callback, bool separationLine, uint8_t layer);

// for internal use
void nbgl_objDrawKeyboard(nbgl_keyboard_t *kbd);
void nbgl_objDrawKeypad(nbgl_keypad_t *kbd);

/**********************
 *      MACROS
 **********************/
// a text cannot be in a address lower than 2000, so it should be an idea
#ifdef __x86_64__
#define IS_A_STRING(__text) ((uint64_t)(__text) > 2000)
#else //__x86_64__
#define IS_A_STRING(__text) ((uint32_t)obj->text > 2000)
#endif //__x86_64__

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_OBJ_H */
