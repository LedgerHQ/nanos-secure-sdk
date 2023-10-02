/**
 * @file nbgl_image_utils.h
 * @brief Utilities for Ledger image files processing
 *
 */

#ifndef NBGL_IMAGE_UTILS_H
#define NBGL_IMAGE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_types.h"

/*********************
 *      DEFINES
 *********************/
/**
 * Width offset in image file
 */
#define WIDTH_OFFSET 0

/**
 * Height offset in image file
 */
#define HEIGHT_OFFSET 2

/**
 * BPP & Compression offset in image file
 */
#define BPP_COMPRESS_OFFSET 4

/**
 * Buffer length offset in image file
 *
 */
#define BUFFER_LEN_OFFSET 5

/**
 * Buffer offset in image file
 *
 */
#define BUFFER_OFFSET 8

/**
 * Size in bytes of an image file header
 */
#define IMAGE_FILE_HEADER_SIZE 8

/**********************
 *      MACROS
 **********************/

/**
 * @brief macro to get the width of the image file
 * @param image_file uint8_t* buffer representing the image file
 */
#define GET_IMAGE_FILE_WIDTH(image_file) \
    (image_file[WIDTH_OFFSET] | (image_file[WIDTH_OFFSET + 1] << 8))

/**
 * @brief macro to get the height of the image file
 * @param image_file uint8_t* buffer representing the image file
 */
#define GET_IMAGE_FILE_HEIGHT(image_file) \
    (image_file[HEIGHT_OFFSET] | (image_file[HEIGHT_OFFSET + 1] << 8))

/**
 * @brief macro to get the bpp  of the image file
 * @param image_file uint8_t* buffer representing the image file
 */
#define GET_IMAGE_FILE_BPP(image_file) (image_file[BPP_COMPRESS_OFFSET] >> 4)

/**
 * @brief macro to get the compression of the image file
 * @param image_file uint8_t* buffer representing the image file
 */
#define GET_IMAGE_FILE_COMPRESSION(image_file) (image_file[BPP_COMPRESS_OFFSET] & 0xF)

/**
 * @brief macro to get the buffer len of the image file
 * @param image_file uint8_t* buffer representing the image file
 */
#define GET_IMAGE_FILE_BUFFER_LEN(image_file)                                 \
    (image_file[BUFFER_LEN_OFFSET] | (image_file[BUFFER_LEN_OFFSET + 1] << 8) \
     | (image_file[BUFFER_LEN_OFFSET + 2] << 16))

/**
 * @brief macro to get the buffer of the image file
 * @param image_file uint8_t* buffer representing the image file
 */
#define GET_IMAGE_FILE_BUFFER(image_file) (&image_file[BUFFER_OFFSET])

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_IMAGE_UTILS_H */
