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
 * Horizontal mirroring when rendering bitmap
 *
 */
#define HORIZONTAL_MIRROR 0x1
/**
 * Vertical mirroring when rendering bitmap
 *
 */
#define VERTICAL_MIRROR   0x2

/**
 * Both directions mirroring when rendering bitmap
 *
 */
#define BOTH_MIRRORS (HORIZONTAL_MIRROR | VERTICAL_MIRROR)

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
 * @brief Available post-refresh power modes
 *
 * - Power off after a refresh allows to save power
 * - Keep the screen powered on after a refresh allows to
 *   achieve a faster following refresh.
 */
typedef enum nbgl_post_refresh_t {
    POST_REFRESH_FORCE_POWER_OFF,   ///< Force screen power off after refresh
    POST_REFRESH_FORCE_POWER_ON,    ///< Force screen power on after refresh
    POST_REFRESH_KEEP_POWER_STATE,  ///< Keep screen power state after refresh
} nbgl_post_refresh_t;

/**
 * @brief possible radius for objects
 *
 */
typedef enum {
    RADIUS_3_PIXELS = 0,     ///< 3 pixels (not on Stax)
    RADIUS_4_PIXELS,         ///< 4 pixels
    RADIUS_8_PIXELS,         ///< 8 pixels
    RADIUS_16_PIXELS,        ///< 16 pixels
    RADIUS_20_PIXELS,        ///< 20 pixels
    RADIUS_24_PIXELS,        ///< 24 pixels
    RADIUS_32_PIXELS,        ///< 32 pixels
    RADIUS_40_PIXELS,        ///< 40 pixels
    RADIUS_48_PIXELS,        ///< 48 pixels
    RADIUS_1_PIXEL,          ///< 1 pixel (not on Stax)
    RADIUS_0_PIXELS = 0xFF,  ///< no radius (square angle)
} nbgl_radius_t;

/**
 * @brief Represents the transformation to be applied on the bitmap before rendering
 * This is a bitfield using masks as @ref HORIZONTAL_MIRROR
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
