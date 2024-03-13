/**
 * @file nbgl_types.h
 * @brief common types for Graphical Library
 *
 */

#ifndef NBGL_TYPES_H
#define NBGL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "bolos_target.h"

/*********************
 *      DEFINES
 *********************/
/**
 * Width of the front screen in pixels
 */
#ifdef SCREEN_SIZE_WALLET
#ifdef TARGET_STAX
#define SCREEN_WIDTH 400
#else  // TARGET_STAX
#define SCREEN_WIDTH 480
#endif  // TARGET_STAX
#else   // SCREEN_SIZE_WALLET
#define SCREEN_WIDTH 128
#endif  // SCREEN_SIZE_WALLET

/**
 * Height of the front screen in pixels
 */
#ifdef SCREEN_SIZE_WALLET
#ifdef TARGET_STAX
#define SCREEN_HEIGHT 672
#else  // TARGET_STAX
#define SCREEN_HEIGHT 600
#endif  // TARGET_STAX
#else   // SCREEN_SIZE_WALLET
#define SCREEN_HEIGHT 64
#endif  // SCREEN_SIZE_WALLET

/**
 * No transformation
 *
 */
#define NO_TRANSFORMATION 0

/**
 * Vertical mirroring when rendering bitmap
 *
 */
#define VERTICAL_MIRROR 0x2

/**
 * Rotation 90 degrees clockwise when rendering bitmap
 *
 */
#define ROTATE_90_CLOCKWISE 0x4

/**
 * Code to be used for color map when not used
 *
 */
#define INVALID_COLOR_MAP 0x0

/**********************
 *      TYPEDEFS
 **********************/
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifdef LINUX_SIMU
#define PACKED__
#else  // LINUX_SIMU
#define PACKED__ __attribute__((packed))
#endif  // LINUX_SIMU
#ifdef BICOLOR_MODE

typedef enum {
    BLACK      = 0,
    DARK_GRAY  = 0,
    LIGHT_GRAY = 0,
    WHITE      = 3
} color_t;

#else

typedef enum {
    BLACK = 0,
    DARK_GRAY,
    LIGHT_GRAY,
    WHITE,
    NB_NBGL_COLOR
} color_t;

#endif

/**
 * @brief All types of graphical objects.
 *
 */
typedef enum {
    SCREEN,          ///< Main screen
    CONTAINER,       ///< Empty container
    IMAGE,           ///< Bitmap (y and height must be multiple of 4 on Stax)
    LINE,            ///< Vertical or Horizontal line
    TEXT_AREA,       ///< Area to contain text line(s)
    BUTTON,          ///< Rounded rectangle button with icon and/or text
    SWITCH,          ///< Switch to turn on/off something
    PAGE_INDICATOR,  ///< horizontal bar to indicate position within pages
    PROGRESS_BAR,    ///< horizontal bar to indicate progression of something (between 0% and 100%)
    RADIO_BUTTON,    ///< Indicator to inform whether something is on or off
    QR_CODE,         ///< QR Code
    KEYBOARD,        ///< Keyboard
    KEYPAD,          ///< Keypad
    SPINNER,         ///< Spinner
    IMAGE_FILE,      ///< Image file (with Ledger compression)
    TEXT_ENTRY       ///< area for entered text, only for Nanos
} nbgl_obj_type_t;

/**
 * @brief All types of alignments.
 *
 */
typedef enum {
    NO_ALIGNMENT,  ///< used when parent container layout is used
    TOP_LEFT,
    TOP_MIDDLE,
    TOP_RIGHT,
    MID_LEFT,
    CENTER,
    MID_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_MIDDLE,
    BOTTOM_RIGHT,
    LEFT_TOP,      ///< on outside left
    LEFT_BOTTOM,   ///< on outside left
    RIGHT_TOP,     ///< on outside right
    RIGHT_BOTTOM,  ///< on outside right
} nbgl_aligment_t;

/**
 * @brief to represent a boolean state.
 */
typedef enum {
    OFF_STATE,
    ON_STATE
} nbgl_state_t;

/**
 * @brief Directions for layout or lines
 *
 */
typedef enum {
    VERTICAL,   ///< from top to bottom
    HORIZONTAL  ///< from left to right
} nbgl_direction_t;

/**
 * @brief possible styles for text area border
 *
 */
typedef enum {
    NO_STYLE,  ///< no border
#ifdef SCREEN_SIZE_NANO
    INVERTED_COLORS  ///< Inverted background and rounded corners, only for @ref TEXT_AREA
#endif               // SCREEN_SIZE_NANO
} nbgl_style_t;

/**
 * @brief possible modes for QR Code
 * @note if text len <= 114 chars, V4 can be used, otherwise use V10
 *
 */
typedef enum {
    QRCODE_V4 = 0,  ///< QRCode V4, can encode text len up to 114 chars
    QRCODE_V10      ///< QRCode V10, can encode text len up to 1500 chars
} nbgl_qrcode_version_t;

/**
 * @brief the 2 possible states of a finger on the Touchscreen
 *
 */
typedef enum {
    RELEASED,  ///< the finger has been released from the screen
    PRESSED,   ///< the finger is currently pressing the screen
} nbgl_touchState_t;

#ifdef HAVE_HW_TOUCH_SWIPE
/**
 * @brief Hardware powered detected swipe states
 */
typedef enum {
    HARDWARE_SWIPE_UP,
    HARDWARE_SWIPE_DOWN,
    HARDWARE_SWIPE_RIGHT,
    HARDWARE_SWIPE_LEFT,
    NO_HARDWARE_SWIPE,
} nbgl_hardwareSwipe_t;
#endif  // HAVE_HW_TOUCH_SWIPE

/**
 * @brief The different types of Touchscreen events
 *
 */
typedef enum {
    TOUCHED,  ///< corresponding to an object touched and released at least SHORT_TOUCH_DURATION ms
              ///< later but less than LONG_TOUCH_DURATION ms
    LONG_TOUCHED,  ///< corresponding to an object touched and released at least LONG_TOUCH_DURATION
                   ///< ms later.
    TOUCHING,      ///< corresponding to an object that is currently touched
    OUT_OF_TOUCH,  ///< corresponding to an object that was touched but that has lost the focus (the
                   ///< finger has moved)
    TOUCH_PRESSED,   ///< corresponding to an object that was not touched and where the finger has
                     ///< been pressed.
    TOUCH_RELEASED,  ///< corresponding to an object that was touched and where the finger has been
                     ///< released.
    VALUE_CHANGED,   ///< corresponding to a change of state of the object (indirect event)
    SWIPED_UP,
    SWIPED_DOWN,
    SWIPED_RIGHT,
    SWIPED_LEFT,
    NB_TOUCH_TYPES
} nbgl_touchType_t;

/**
 * @brief Enum to represent the number of bits per pixel (BPP)
 *
 */
typedef enum {
    NBGL_BPP_1 = 0,  ///< 1 bit per pixel
    NBGL_BPP_2,      ///< 2 bits per pixel
    NBGL_BPP_4,      ///< 4 bits per pixel
    NB_NBGL_BPP,     ///< Number of NBGL_BPP enums
} nbgl_bpp_t;

/**
 * @brief Enum to represent the compression
 *
 */
typedef enum {
    NBGL_NO_COMPRESSION = 0,  ///< no compression, raw data
    NBGL_GZLIB_COMPRESSION,   ///< gzlib compression
    NBGL_RLE_COMPRESSION,     ///< RLE compression
    NB_NBGL_COMPRESSION       ///< Number of NBGL_COMPRESSION enums
} nbgl_compression_t;

/**
 * @brief size of gzlib uncompression buffer in bytes
 *
 */
#define GZLIB_UNCOMPRESSED_CHUNK 2049

/**
 * @brief Represents a rectangle area of the screen
 *
 */
typedef struct PACKED__ nbgl_area_s {
    int16_t x0;  ///< horizontal position of the upper left point of the area (signed int allow for
                 ///< out of screen rendering)
    int16_t y0;  ///< vertical position of the upper left point of the area (signed int allow for
                 ///< out of screen rendering)
    uint16_t   width;            ///< width of the area, in pixels
    uint16_t   height;           ///< height of the area, in pixels
    color_t    backgroundColor;  ///< color (usually background) to be applied
    nbgl_bpp_t bpp;              ///< bits per pixel for this area
} nbgl_area_t;

/**
 * @brief different modes of refresh for @ref nbgl_refreshSpecial()
 *
 */
typedef enum {
    FULL_COLOR_REFRESH,          ///< to be used for normal refresh
    FULL_COLOR_PARTIAL_REFRESH,  ///< to be used for small partial refresh (radio buttons, switches)
    FULL_COLOR_CLEAN_REFRESH,  ///< to be used for lock screen display (cleaner but longer refresh)
    BLACK_AND_WHITE_REFRESH,   ///< to be used for pure B&W area, when contrast is important
    BLACK_AND_WHITE_FAST_REFRESH,  ///< to be used for pure B&W area, when contrast is not priority
    NB_REFRESH_MODES
} nbgl_refresh_mode_t;

/**
 * @brief Post refresh modes
 *
 * - POST_REFRESH_FORCE_POWER_OFF
 *  Screen is powered off after refresh.
 *  Following refreshes will be slower, but less energy is consumed.
 *
 * - POST_REFRESH_FORCE_POWER_ON
 *   Screen is let powered on after refresh.
 *   Following refreshes will be faster, at the cost of more energy consumed.
 *
 * - POST_REFRESH_FORCE_POWER_ON_WITH_PIPELINE
 *   Same as POST_REFRESH_FORCE_POWER_ON, but with pipeline enabled.
 *   When using pipeline, refreshes are faster, but some constraints must be respected: successive
 *   draws & refreshes areas must not overlap.
 *   If overlapping is needed, the function @ref nbgl_driver_waitPipeline() must be called, in order
 *   to wait for pipelined operations to finish.
 */
typedef enum nbgl_post_refresh_t {
    POST_REFRESH_FORCE_POWER_OFF,               ///< Force screen power off after refresh
    POST_REFRESH_FORCE_POWER_ON,                ///< Force screen power on after refresh
    POST_REFRESH_FORCE_POWER_ON_WITH_PIPELINE,  ///< Force screen power on and enable pipeline
    POST_REFRESH_KEEP_POWER_STATE,              ///< Keep state after refresh
} nbgl_post_refresh_t;

/**
 * @brief possible radius for objects
 *
 */
typedef enum {
#ifdef SCREEN_SIZE_WALLET
    RADIUS_32_PIXELS = 0,  ///< 32 pixels
#ifdef TARGET_STAX
    RADIUS_40_PIXELS,  ///< 40 pixels
    RADIUS_MAX = RADIUS_40_PIXELS,
#else                        // TARGET_STAX
    RADIUS_44_PIXELS,  ///< 44 pixels
    RADIUS_MAX = RADIUS_44_PIXELS,
#endif                       // TARGET_STAX
#else                        // SCREEN_SIZE_WALLET
    RADIUS_1_PIXEL = 0,  ///< 1 pixel
    RADIUS_3_PIXELS,     ///< 3 pixels
    RADIUS_MAX = RADIUS_3_PIXELS,
#endif                       // SCREEN_SIZE_WALLET
    RADIUS_0_PIXELS = 0xFF,  ///< no radius (square angle)
} nbgl_radius_t;

/**
 * @brief Represents the transformation to be applied on the bitmap before rendering
 * This is a bitfield using masks as @ref VERTICAL_MIRROR
 */
typedef uint8_t nbgl_transformation_t;

/**
 * @brief Represents the color_map to be used for 2BPP image, or the foreground color for 1BPP image
 * @note colors are encoded on 2 bits, so a 8 bits stores the 4 colors for a 2BPP image
 * @note not used for 4BPP image
 */
typedef uint8_t nbgl_color_map_t;

/**
 * @brief Represents all information about an icon
 *
 */
typedef struct PACKED__ nbgl_icon_details_s {
    uint16_t       width;   ///< width of the icon, in pixels
    uint16_t       height;  ///< height of the icon, in pixels
    nbgl_bpp_t     bpp;     ///< bits per pixel for this area
    bool           isFile;  ///< if true, the bitmap buffer contains an image file
    const uint8_t *bitmap;  ///< buffer containing pixel values
} nbgl_icon_details_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_TYPES_H */
