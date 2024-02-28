/**
 * @file nbgl_fonts.h
 * Fonts types of the new BOLOS Graphical Library
 *
 */

#ifndef NBGL_FONTS_H
#define NBGL_FONTS_H

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
#define PIC_CHAR(x) ((const nbgl_font_character_t *) PIC(x))
#define PIC_BMP(x)  ((uint8_t const *) PIC(x))

/**
 * @brief fonts nicknames to be used for various wallet size targets (non-Nano)
 *
 */
#ifdef TARGET_STAX
#define SMALL_REGULAR_FONT      BAGL_FONT_INTER_REGULAR_24px
#define SMALL_BOLD_FONT         BAGL_FONT_INTER_SEMIBOLD_24px
#define LARGE_MEDIUM_FONT       BAGL_FONT_INTER_MEDIUM_32px
#define SMALL_REGULAR_1BPP_FONT BAGL_FONT_INTER_REGULAR_24px_1bpp
#define SMALL_BOLD_1BPP_FONT    BAGL_FONT_INTER_SEMIBOLD_24px_1bpp
#define LARGE_MEDIUM_1BPP_FONT  BAGL_FONT_INTER_MEDIUM_32px_1bpp

#else  // TARGET_STAX

#ifdef TARGET_EUROPA
#define SMALL_REGULAR_FONT      BAGL_FONT_INTER_REGULAR_28px
#define SMALL_BOLD_FONT         BAGL_FONT_INTER_SEMIBOLD_28px
#define LARGE_MEDIUM_FONT       BAGL_FONT_INTER_MEDIUM_36px
#define SMALL_REGULAR_1BPP_FONT BAGL_FONT_INTER_REGULAR_28px_1bpp
#define SMALL_BOLD_1BPP_FONT    BAGL_FONT_INTER_SEMIBOLD_28px_1bpp
#define LARGE_MEDIUM_1BPP_FONT  BAGL_FONT_INTER_MEDIUM_36px_1bpp
#endif
#endif  // TARGET_STAX

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief structure defining an ASCII character (except the bitmap)
 *
 */
// WARNING: please DON'T CHANGE the order/values of the fields below!
// (otherwise python tools that generate data will need to be modified too)
typedef struct {
    uint32_t bitmap_offset;     ///< offset of this character in chars buffer
    uint32_t encoding : 1;      ///< method used to encode bitmap data
    uint32_t width : 6;         ///< width of character in pixels
    uint32_t x_min_offset : 4;  ///< x_min = x_min_offset
    uint32_t y_min_offset : 6;  ///< y_min = (y_min + y_min_offset)
    uint32_t x_max_offset : 4;  ///< x_max = width - x_max_offset
    uint32_t y_max_offset : 6;  ///< y_max = (height - y_max_offset)
} nbgl_font_character_t;

/**
 * @brief structure defining an ASCII font
 *
 */
typedef struct {
    uint32_t bitmap_len;    ///< Size in bytes of the associated bitmap
    uint8_t  font_id;       ///< ID of the font, from @ref nbgl_font_id_e
    uint8_t  bpp;           ///< number of bits per pixels
    uint8_t  height;        ///< height of all characters in pixels
    uint8_t  line_height;   ///< height of a line for all characters in pixels
    uint8_t  char_kerning;  ///< kerning for the font
    uint8_t  crop;          ///< If false, x_min_offset+y_min_offset=bytes to skip
    uint8_t  y_min;         ///< Most top Y coordinate of any char in the font
    uint8_t
        first_char;  ///< ASCII code of the first character in \b bitmap and in \b characters fields
    uint8_t
        last_char;  ///< ASCII code of the last character in \b bitmap and in \b characters fields
    const nbgl_font_character_t
        *const     characters;  ///< array containing definitions of all characters
    uint8_t const *bitmap;      ///< array containing bitmaps of all characters
} nbgl_font_t;

#define BAGL_ENCODING_LATIN1  0
#define BAGL_ENCODING_UTF8    1
#define BAGL_ENCODING_DEFAULT BAGL_ENCODING_UTF8

/**
 * @brief structure defining a unicode character (except the bitmap)
 *
 */
// WARNING: please DON'T CHANGE the order of the fields below!
// (otherwise python tools that generate data will need to be modified too)
typedef struct {
    uint32_t char_unicode : 21;   ///< plane value from 0 to 16 then 16-bit code.
    uint32_t encoding : 1;        ///< method used to encode bitmap data
    uint32_t width : 6;           ///< width of character in pixels
    uint32_t x_min_offset : 4;    ///< x_min = x_min_offset
    uint32_t y_min_offset : 6;    ///< y_min = (y_min + y_min_offset)
    uint32_t x_max_offset : 4;    ///< x_max = width - x_max_offset
    uint32_t y_max_offset : 6;    ///< y_max = (height - y_max_offset)
    uint32_t bitmap_offset : 16;  ///< offset of this character in chars buffer
} nbgl_font_unicode_character_t;
/**
 * @brief structure defining a unicode font
 *
 */
typedef struct {
    uint16_t bitmap_len;    ///< Size in bytes of all characters bitmaps
    uint8_t  font_id;       ///< ID of the font, from @ref nbgl_font_id_e
    uint8_t  bpp;           ///< Number of bits per pixels, (interpreted as nbgl_bpp_t)
    uint8_t  height;        ///< height of all characters in pixels
    uint8_t  line_height;   ///< height of a line for all characters in pixels
    uint8_t  char_kerning;  ///< kerning for the font
    uint8_t  crop;          ///< If false, x_min_offset+y_min_offset=bytes to skip
    uint8_t  y_min;         ///< Most top Y coordinate of any char in the font
    uint8_t  unused[3];     ///< for alignment
#if !defined(HAVE_LANGUAGE_PACK)
    // When using language packs, those 2 pointers does not exists
    const nbgl_font_unicode_character_t
        *const     characters;  ///< array containing definitions of all characters
    uint8_t const *bitmap;      ///< array containing bitmaps of all characters
#endif                          //! defined(HAVE_LANGUAGE_PACK)
} nbgl_font_unicode_t;

typedef enum {
    BAGL_FONT_INTER_REGULAR_24px,
    BAGL_FONT_INTER_SEMIBOLD_24px,
    BAGL_FONT_INTER_MEDIUM_32px,
    BAGL_FONT_INTER_REGULAR_24px_1bpp,
    BAGL_FONT_INTER_SEMIBOLD_24px_1bpp,
    BAGL_FONT_INTER_MEDIUM_32px_1bpp,
    BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp = 8u,   // on Nano
    BAGL_FONT_OPEN_SANS_LIGHT_16px_1bpp     = 9u,   // on Nano
    BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp   = 10u,  // on Nano
    BAGL_FONT_INTER_REGULAR_28px            = 11u,
    BAGL_FONT_INTER_SEMIBOLD_28px           = 12u,
    BAGL_FONT_INTER_MEDIUM_36px             = 13u,
    BAGL_FONT_INTER_REGULAR_28px_1bpp       = 14u,
    BAGL_FONT_INTER_SEMIBOLD_28px_1bpp      = 15u,
    BAGL_FONT_INTER_MEDIUM_36px_1bpp        = 16u,
    BAGL_FONT_LAST  // MUST ALWAYS BE THE LAST, FOR AUTOMATED INVALID VALUE CHECKS
} nbgl_font_id_e;

typedef struct nbgl_unicode_ctx_s {
    const nbgl_font_unicode_t           *font;
    const nbgl_font_unicode_character_t *characters;
    const uint8_t                       *bitmap;
    uint32_t                             unicode_character_byte_count;
} nbgl_unicode_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const nbgl_font_t *nbgl_font_getFont(unsigned int fontId);
const nbgl_font_t *nbgl_getFont(nbgl_font_id_e fontId);
uint16_t           nbgl_getSingleLineTextWidth(nbgl_font_id_e fontId, const char *text);
uint16_t nbgl_getSingleLineTextWidthInLen(nbgl_font_id_e fontId, const char *text, uint16_t maxLen);
uint16_t nbgl_getTextWidth(nbgl_font_id_e fontId, const char *text);
uint8_t  nbgl_getCharWidth(nbgl_font_id_e fontId, const char *text);
uint8_t  nbgl_getFontHeight(nbgl_font_id_e fontId);
uint8_t  nbgl_getFontLineHeight(nbgl_font_id_e fontId);
uint16_t nbgl_getTextNbLines(const char *text);
uint16_t nbgl_getTextHeight(nbgl_font_id_e fontId, const char *text);
uint16_t nbgl_getTextLength(const char *text);
void     nbgl_getTextMaxLenAndWidth(nbgl_font_id_e fontId,
                                    const char    *text,
                                    uint16_t       maxWidth,
                                    uint16_t      *len,
                                    uint16_t      *width,
                                    bool           wrapping);
uint16_t nbgl_getTextNbLinesInWidth(nbgl_font_id_e fontId,
                                    const char    *text,
                                    uint16_t       maxWidth,
                                    bool           wrapping);
uint16_t nbgl_getTextHeightInWidth(nbgl_font_id_e fontId,
                                   const char    *text,
                                   uint16_t       maxWidth,
                                   bool           wrapping);
bool     nbgl_getTextMaxLenAndWidthFromEnd(nbgl_font_id_e fontId,
                                           const char    *text,
                                           uint16_t       maxWidth,
                                           uint16_t      *len,
                                           uint16_t      *width);
bool     nbgl_getTextMaxLenInNbLines(nbgl_font_id_e fontId,
                                     const char    *text,
                                     uint16_t       maxWidth,
                                     uint16_t       maxNbLines,
                                     uint16_t      *len,
                                     bool           wrapping);
void nbgl_textWrapOnNbLines(nbgl_font_id_e fontId, char *text, uint16_t maxWidth, uint8_t nbLines);
uint8_t nbgl_getTextNbPagesInWidth(nbgl_font_id_e fontId,
                                   const char    *text,
                                   uint8_t        nbLinesPerPage,
                                   uint16_t       maxWidth);

uint32_t nbgl_popUnicodeChar(const uint8_t **text, uint16_t *text_length, bool *is_unicode);
#ifdef HAVE_UNICODE_SUPPORT
nbgl_unicode_ctx_t                  *nbgl_getUnicodeFont(nbgl_font_id_e font_id);
const nbgl_font_unicode_character_t *nbgl_getUnicodeFontCharacter(uint32_t unicode);
uint32_t                             nbgl_getUnicodeFontCharacterByteCount(void);
#ifdef HAVE_LANGUAGE_PACK
void nbgl_refreshUnicodeFont(void);
#endif
#endif  // HAVE_UNICODE_SUPPORT

/**********************
 *      MACROS
 **********************/
#define IS_UNICODE(__value) ((__value) > 0xF0)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_FONTS_H */
