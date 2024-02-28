
/**
 * @file nbgl_draw.c
 * @brief Implementation of middle-level API to draw rich forms like
 * rounded rectangle
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "nbgl_front.h"
#include "nbgl_draw.h"
#include "nbgl_fonts.h"
#include "nbgl_debug.h"
#include "nbgl_side.h"
#ifdef NBGL_QRCODE
#include "qrcodegen.h"
#endif  // NBGL_QRCODE
#include "glyphs.h"
#include "os_pic.h"
#include "os_utils.h"

/*********************
 *      DEFINES
 *********************/
typedef enum {
    BAGL_FILL_CIRCLE_0_PI2,
    BAGL_FILL_CIRCLE_PI2_PI,
    BAGL_FILL_CIRCLE_PI_3PI2,
    BAGL_FILL_CIRCLE_3PI2_2PI
} quarter_t;

#define QR_PIXEL_WIDTH_HEIGHT 4

/**********************
 *      TYPEDEFS
 **********************/
#ifdef NBGL_QRCODE
typedef struct {
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    uint8_t QrDrawBuffer[QR_PIXEL_WIDTH_HEIGHT * QR_PIXEL_WIDTH_HEIGHT * QR_MAX_PIX_SIZE / 8];
} QrCodeBuffer_t;

#define qrcode       ((QrCodeBuffer_t *) ramBuffer)->qrcode
#define tempBuffer   ((QrCodeBuffer_t *) ramBuffer)->tempBuffer
#define QrDrawBuffer ((QrCodeBuffer_t *) ramBuffer)->QrDrawBuffer
#endif  // NBGL_QRCODE

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
#ifndef SCREEN_SIZE_WALLET
static const uint8_t quarter_disc_3px_1bpp[]     = {0xEC, 0xFF};
static const uint8_t quarter_disc_3px_90_1bpp[]  = {0x2F, 0xFF};
static const uint8_t quarter_disc_3px_180_1bpp[] = {0x9B, 0xFF};
static const uint8_t quarter_disc_3px_270_1bpp[] = {0xFA, 0x00};

static const uint8_t quarter_circle_3px_1bpp[]     = {0x4C, 0x00};
static const uint8_t quarter_circle_3px_90_1bpp[]  = {0x0D, 0x00};
static const uint8_t quarter_circle_3px_180_1bpp[] = {0x19, 0x00};
static const uint8_t quarter_circle_3px_270_1bpp[] = {0x58, 0x00};
#endif  // SCREEN_SIZE_WALLET

// indexed by nbgl_radius_t (except RADIUS_0_PIXELS)
static const uint8_t radiusValues[] = {
#ifdef SCREEN_SIZE_WALLET
    32,
#ifdef TARGET_STAX
    40,
#else   // TARGET_STAX
    44
#endif  // TARGET_STAX
#else   // SCREEN_SIZE_WALLET
    1,
    3
#endif  // SCREEN_SIZE_WALLET
};

#ifdef SCREEN_SIZE_WALLET
// indexed by nbgl_radius_t (except RADIUS_0_PIXELS)
static const uint8_t *topQuarterDiscs[] = {
    C_quarter_disc_top_left_32px_1bpp_bitmap,
#ifdef TARGET_STAX
    C_quarter_disc_top_left_40px_1bpp_bitmap,
#else   // TARGET_STAX
    C_quarter_disc_top_left_44px_1bpp_bitmap
#endif  // TARGET_STAX
};

static const uint8_t *bottomQuarterDiscs[] = {
    C_quarter_disc_bottom_left_32px_1bpp_bitmap,
#ifdef TARGET_STAX
    C_quarter_disc_bottom_left_40px_1bpp_bitmap,
#else   // TARGET_STAX
    C_quarter_disc_bottom_left_44px_1bpp_bitmap
#endif  // TARGET_STAX
};

// indexed by nbgl_radius_t (except RADIUS_0_PIXELS)
static const uint8_t *topQuarterCircles[] = {
    C_quarter_circle_top_left_32px_1bpp_bitmap,
#ifdef TARGET_STAX
    C_quarter_circle_top_left_40px_1bpp_bitmap,
#else   // TARGET_STAX
    C_quarter_circle_top_left_44px_1bpp_bitmap
#endif  // TARGET_STAX
};

static const uint8_t *bottomQuarterCircles[] = {
    C_quarter_circle_bottom_left_32px_1bpp_bitmap,
#ifdef TARGET_STAX
    C_quarter_circle_bottom_left_40px_1bpp_bitmap,
#else   // TARGET_STAX
    C_quarter_circle_bottom_left_44px_1bpp_bitmap
#endif  // TARGET_STAX
};
#endif  // SCREEN_SIZE_WALLET

#ifdef NBGL_QRCODE
// ensure that the ramBuffer also used for image file decompression is big enough for QR code
CCASSERT(qr_code_buffer, sizeof(QrCodeBuffer_t) <= GZLIB_UNCOMPRESSED_CHUNK);
#endif  // NBGL_QRCODE

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_circle_helper(int           x_center,
                               int           y_center,
                               nbgl_radius_t radiusIndex,
                               quarter_t     quarter,
                               color_t       borderColor,
                               color_t       innerColor,
                               color_t       backgroundColor)
{
    const uint8_t *quarter_buffer = NULL;
    nbgl_area_t    area           = {.bpp = NBGL_BPP_1, .backgroundColor = backgroundColor};

    // radius is not supported
    if (radiusIndex > RADIUS_MAX) {
        return;
    }
    area.width = area.height = radiusValues[radiusIndex];
#ifdef SCREEN_SIZE_WALLET
    if (borderColor == innerColor) {
        if (quarter < BAGL_FILL_CIRCLE_PI_3PI2) {
            quarter_buffer = (const uint8_t *) PIC(topQuarterDiscs[radiusIndex]);
        }
        else {
            quarter_buffer = (const uint8_t *) PIC(bottomQuarterDiscs[radiusIndex]);
        }
    }
    else {
        if (quarter < BAGL_FILL_CIRCLE_PI_3PI2) {
            quarter_buffer = (const uint8_t *) PIC(topQuarterCircles[radiusIndex]);
        }
        else {
            quarter_buffer = (const uint8_t *) PIC(bottomQuarterCircles[radiusIndex]);
        }
    }
    switch (quarter) {
        case BAGL_FILL_CIRCLE_3PI2_2PI:  // bottom right
            area.x0 = x_center;
            area.y0 = y_center;
            nbgl_frontDrawImage(&area, quarter_buffer, VERTICAL_MIRROR, borderColor);
            break;
        case BAGL_FILL_CIRCLE_PI_3PI2:  // bottom left
            area.x0 = x_center - area.width;
            area.y0 = y_center;
            nbgl_frontDrawImage(&area, quarter_buffer, NO_TRANSFORMATION, borderColor);
            break;
        case BAGL_FILL_CIRCLE_0_PI2:  // top right
            area.x0 = x_center;
            area.y0 = y_center - area.width;
            nbgl_frontDrawImage(&area, quarter_buffer, VERTICAL_MIRROR, borderColor);
            break;
        case BAGL_FILL_CIRCLE_PI2_PI:  // top left
            area.x0 = x_center - area.width;
            area.y0 = y_center - area.width;
            nbgl_frontDrawImage(&area, quarter_buffer, NO_TRANSFORMATION, borderColor);
            break;
    }
#else   // SCREEN_SIZE_WALLET
    switch (quarter) {
        case BAGL_FILL_CIRCLE_3PI2_2PI:  // bottom right
            area.x0        = x_center;
            area.y0        = y_center;
            quarter_buffer = (borderColor == innerColor) ? quarter_disc_3px_180_1bpp
                                                         : quarter_circle_3px_180_1bpp;
            break;
        case BAGL_FILL_CIRCLE_PI_3PI2:  // bottom left
            area.x0        = x_center - area.width;
            area.y0        = y_center;
            quarter_buffer = (borderColor == innerColor) ? quarter_disc_3px_270_1bpp
                                                         : quarter_circle_3px_270_1bpp;
            break;
        case BAGL_FILL_CIRCLE_0_PI2:  // top right
            area.x0        = x_center;
            area.y0        = y_center - area.width;
            quarter_buffer = (borderColor == innerColor) ? quarter_disc_3px_90_1bpp
                                                         : quarter_circle_3px_90_1bpp;
            break;
        case BAGL_FILL_CIRCLE_PI2_PI:  // top left
            area.x0 = x_center - area.width;
            area.y0 = y_center - area.width;
            quarter_buffer
                = (borderColor == innerColor) ? quarter_disc_3px_1bpp : quarter_circle_3px_1bpp;
            break;
    }
    nbgl_frontDrawImage(&area, quarter_buffer, NO_TRANSFORMATION, borderColor);
#endif  // SCREEN_SIZE_WALLET
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief This functions draws a rounded corners rectangle (without border), with the given
 * parameters.
 *
 * @param area position, size and background color (outside of the rectangle) to use for the
 * rectangle
 * @param radiusIndex radius size
 * @param innerColor color to use for inside the rectangle
 */
void nbgl_drawRoundedRect(const nbgl_area_t *area, nbgl_radius_t radiusIndex, color_t innerColor)
{
    nbgl_area_t rectArea;
    uint8_t     radius;

    LOG_DEBUG(DRAW_LOGGER,
              "nbgl_drawRoundedRect x0 = %d, y0 = %d, width =%d, height =%d\n",
              area->x0,
              area->y0,
              area->width,
              area->height);

    if (radiusIndex <= RADIUS_MAX) {
        radius = radiusValues[radiusIndex];
    }
    else if (radiusIndex == RADIUS_0_PIXELS) {
        radius = 0;
    }
    else {
        // radius not supported
        LOG_WARN(DRAW_LOGGER, "nbgl_drawRoundedRect forbidden radius index =%d\n", radiusIndex);
        return;
    }

    // Draw main inner rectangle
    rectArea.x0              = area->x0 + radius;
    rectArea.y0              = area->y0;
    rectArea.width           = area->width - (2 * radius);
    rectArea.height          = area->height;
    rectArea.backgroundColor = innerColor;
    nbgl_frontDrawRect(&rectArea);
    // special case when radius is null, just draw a rectangle
    if (radiusIndex == RADIUS_0_PIXELS) {
        return;
    }
    // Draw left inner rectangle
    rectArea.x0     = area->x0;
    rectArea.y0     = area->y0 + radius;
    rectArea.width  = radius;
    rectArea.height = area->height - (2 * radius);
    nbgl_frontDrawRect(&rectArea);
    // Draw right inner rectangle
    rectArea.x0     = area->x0 + area->width - radius;
    rectArea.y0     = area->y0 + radius;
    rectArea.width  = radius;
    rectArea.height = area->height - (2 * radius);
    nbgl_frontDrawRect(&rectArea);

#ifdef SCREEN_SIZE_NANO
    if (radiusIndex == RADIUS_1_PIXEL) {
        return;
    }
#endif  // SCREEN_SIZE_NANO
    // Draw 4 quarters of disc
    draw_circle_helper(area->x0 + radius,
                       area->y0 + radius,
                       radiusIndex,
                       BAGL_FILL_CIRCLE_PI2_PI,
                       innerColor,  // unused
                       innerColor,
                       area->backgroundColor);
    draw_circle_helper(area->x0 + area->width - radius,
                       area->y0 + radius,
                       radiusIndex,
                       BAGL_FILL_CIRCLE_0_PI2,
                       innerColor,  // unused
                       innerColor,
                       area->backgroundColor);
    draw_circle_helper(area->x0 + radius,
                       area->y0 + area->height - radius,
                       radiusIndex,
                       BAGL_FILL_CIRCLE_PI_3PI2,
                       innerColor,  // unused
                       innerColor,
                       area->backgroundColor);
    draw_circle_helper(area->x0 + area->width - radius,
                       area->y0 + area->height - radius,
                       radiusIndex,
                       BAGL_FILL_CIRCLE_3PI2_2PI,
                       innerColor,  // unused
                       innerColor,
                       area->backgroundColor);
}

/**
 * @brief This functions draws a rounded corners rectangle with a border, with the given parameters.
 *
 * @param area position, size and background color (outside of the rectangle) to use for the
 * rectangle
 * @param radiusIndex radius size
 * @param stroke thickness of border (fixed to 2)
 * @param innerColor color to use for inside the rectangle
 * @param borderColor color to use for the border
 */
void nbgl_drawRoundedBorderedRect(const nbgl_area_t *area,
                                  nbgl_radius_t      radiusIndex,
                                  uint8_t            stroke,
                                  color_t            innerColor,
                                  color_t            borderColor)
{
    uint8_t     radius;
    nbgl_area_t rectArea;

    LOG_DEBUG(
        DRAW_LOGGER,
        "nbgl_drawRoundedBorderedRect: innerColor = %d, borderColor = %d, backgroundColor=%d\n",
        innerColor,
        borderColor,
        area->backgroundColor);

    if (radiusIndex <= RADIUS_MAX) {
        radius = radiusValues[radiusIndex];
    }
    else if (radiusIndex == RADIUS_0_PIXELS) {
        radius = 0;
    }
    else {
        // radius not supported
        LOG_WARN(
            DRAW_LOGGER, "nbgl_drawRoundedBorderedRect forbidden radius index =%d\n", radiusIndex);
        return;
    }
    rectArea.backgroundColor = innerColor;

    // special case, when border_color == inner_color == background_color, just draw a rectangle
    if ((innerColor == borderColor) && (borderColor == area->backgroundColor)) {
        rectArea.x0     = area->x0;
        rectArea.y0     = area->y0;
        rectArea.width  = area->width;
        rectArea.height = area->height;
        nbgl_frontDrawRect(&rectArea);
        return;
    }
    // Draw 3 rectangles
    if ((2 * radius) < area->width) {
        rectArea.x0     = area->x0 + radius;
        rectArea.y0     = area->y0;
        rectArea.width  = area->width - (2 * radius);
        rectArea.height = area->height;
        nbgl_frontDrawRect(&rectArea);
    }
    // special case when radius is null, left and right rectangles are not necessary
    if (radiusIndex <= RADIUS_MAX) {
        if ((2 * radius) < area->height) {
            rectArea.x0     = area->x0;
            rectArea.y0     = area->y0 + radius;
            rectArea.width  = radius;
            rectArea.height = area->height - (2 * radius);
            nbgl_frontDrawRect(&rectArea);
            rectArea.x0 = area->x0 + area->width - radius;
            rectArea.y0 = area->y0 + radius;
            nbgl_frontDrawRect(&rectArea);
        }
    }
    // border
    // 4 rectangles (with last pixel of each corner not set)
#ifdef SCREEN_SIZE_WALLET
    uint8_t maskTop, maskBottom;
    if (stroke == 1) {
        maskTop    = 0x1;
        maskBottom = 0x8;
    }
    else if (stroke == 2) {
        maskTop    = 0x3;
        maskBottom = 0xC;
    }
    else if (stroke == 3) {
        maskTop    = 0x7;
        maskBottom = 0xE;
    }
    else if (stroke == 4) {
        maskTop    = 0xF;
        maskBottom = 0xF;
    }
    else {
        LOG_WARN(DRAW_LOGGER, "nbgl_drawRoundedBorderedRect forbidden stroke=%d\n", stroke);
        return;
    }
    rectArea.x0     = area->x0 + radius;
    rectArea.y0     = area->y0;
    rectArea.width  = area->width - 2 * radius;
    rectArea.height = 4;
    nbgl_frontDrawHorizontalLine(&rectArea, maskTop, borderColor);  // top
    rectArea.x0 = area->x0 + radius;
    rectArea.y0 = area->y0 + area->height - 4;
    nbgl_frontDrawHorizontalLine(&rectArea, maskBottom, borderColor);  // bottom
#else                                                                  // SCREEN_SIZE_WALLET
    rectArea.x0              = area->x0 + radius;
    rectArea.y0              = area->y0;
    rectArea.width           = area->width - 2 * radius;
    rectArea.height          = stroke;
    rectArea.backgroundColor = borderColor;
    nbgl_frontDrawRect(&rectArea);  // top
    rectArea.y0 = area->y0 + area->height - stroke;
    nbgl_frontDrawRect(&rectArea);  // bottom
#endif                                                                 // SCREEN_SIZE_WALLET
    if ((2 * radius) < area->height) {
        rectArea.x0              = area->x0;
        rectArea.y0              = area->y0 + radius;
        rectArea.width           = stroke;
        rectArea.height          = area->height - 2 * radius;
        rectArea.backgroundColor = borderColor;
        nbgl_frontDrawRect(&rectArea);  // left
        rectArea.x0 = area->x0 + area->width - stroke;
        nbgl_frontDrawRect(&rectArea);  // right
    }

    if (radiusIndex <= RADIUS_MAX) {
        // Draw 4 quarters of circles
        draw_circle_helper(area->x0 + radius,
                           area->y0 + radius,
                           radiusIndex,
                           BAGL_FILL_CIRCLE_PI2_PI,
                           borderColor,
                           innerColor,
                           area->backgroundColor);
        draw_circle_helper(area->x0 + area->width - radius,
                           area->y0 + radius,
                           radiusIndex,
                           BAGL_FILL_CIRCLE_0_PI2,
                           borderColor,
                           innerColor,
                           area->backgroundColor);
        draw_circle_helper(area->x0 + radius,
                           area->y0 + area->height - radius,
                           radiusIndex,
                           BAGL_FILL_CIRCLE_PI_3PI2,
                           borderColor,
                           innerColor,
                           area->backgroundColor);
        draw_circle_helper(area->x0 + area->width - radius,
                           area->y0 + area->height - radius,
                           radiusIndex,
                           BAGL_FILL_CIRCLE_3PI2_2PI,
                           borderColor,
                           innerColor,
                           area->backgroundColor);
    }
}

/**
 * @brief Helper function to render an icon directly from its `nbgl_icon_details_t` structure.
 *
 * The icon is rendered whether it's an image file or not.
 * No transformation is applied to the icon.
 *
 * @param area Area of drawing
 * @param transformation Transformation to apply to this icon (only available for raw image, not
 * image file)
 * @param color_map Color map applied to icon
 * @param icon Icon details structure to draw
 */
void nbgl_drawIcon(nbgl_area_t               *area,
                   nbgl_transformation_t      transformation,
                   nbgl_color_map_t           color_map,
                   const nbgl_icon_details_t *icon)
{
    if (icon->isFile) {
        nbgl_frontDrawImageFile(area, icon->bitmap, color_map, ramBuffer);
    }
    else {
        nbgl_frontDrawImage(area, icon->bitmap, transformation, color_map);
    }
}

/**
 * @brief Return the size of the bitmap associated to the input font and character
 *
 * @param font pointer to the font infos
 * @param charId id of the character
 */

static uint16_t get_bitmap_byte_cnt(const nbgl_font_t *font, uint8_t charId)
{
    if ((charId < font->first_char) || (charId > font->last_char)) {
        return 0;
    }

    uint16_t baseId = charId - font->first_char;
    if (charId < font->last_char) {
        const nbgl_font_character_t *character
            = (const nbgl_font_character_t *) PIC(&font->characters[baseId]);
        const nbgl_font_character_t *nextCharacter
            = (const nbgl_font_character_t *) PIC(&font->characters[baseId + 1]);
        return (nextCharacter->bitmap_offset - character->bitmap_offset);
    }
    else if (charId == font->last_char) {
        return (font->bitmap_len - font->characters[baseId].bitmap_offset);
    }
    return 0;
}

/**
 * @brief This function draws the given single-line text, with the given parameters.
 *
 * @param area position, size and background color to use for text
 * @param text array of characters (UTF-8)
 * @param textLen number of chars to draw
 * @param fontId font to be used
 * @param fontColor color to use for font
 */
nbgl_font_id_e nbgl_drawText(const nbgl_area_t *area,
                             const char        *text,
                             uint16_t           textLen,
                             nbgl_font_id_e     fontId,
                             color_t            fontColor)
{
    // text is a series of characters, each character being a bitmap
    // we need to align bitmaps on width multiple of 4 limitation.
    int16_t            x = area->x0;
    nbgl_area_t        rectArea;
    const nbgl_font_t *font = nbgl_getFont(fontId);

    LOG_DEBUG(DRAW_LOGGER,
              "nbgl_drawText: x0 = %d, y0 = %d, w = %d, h = %d, fontColor = %d, "
              "backgroundColor=%d, text = %s\n",
              area->x0,
              area->y0,
              area->width,
              area->height,
              fontColor,
              area->backgroundColor,
              text);

#ifdef HAVE_UNICODE_SUPPORT
    nbgl_unicode_ctx_t *unicode_ctx = nbgl_getUnicodeFont(fontId);
#endif  // HAVE_UNICODE_SUPPORT

    rectArea.backgroundColor = area->backgroundColor;
    rectArea.bpp             = (nbgl_bpp_t) font->bpp;

    while (textLen > 0) {
        const nbgl_font_character_t *character;
        uint8_t                      char_width;
        uint32_t                     unicode;
        bool                         is_unicode;
        const uint8_t               *char_buffer;
        int16_t                      char_x_min;
        int16_t                      char_y_min;
        int16_t                      char_x_max;
        int16_t                      char_y_max;
        uint16_t                     char_byte_cnt;
        uint8_t                      encoding;
        uint8_t                      nb_skipped_bytes;

        unicode = nbgl_popUnicodeChar((const uint8_t **) &text, &textLen, &is_unicode);

        if (is_unicode) {
#ifdef HAVE_UNICODE_SUPPORT
            const nbgl_font_unicode_character_t *unicodeCharacter
                = nbgl_getUnicodeFontCharacter(unicode);
            // if not supported char, go to next one
            if (unicodeCharacter == NULL) {
                continue;
            }
            char_width = unicodeCharacter->width;
#if defined(HAVE_LANGUAGE_PACK)
            char_buffer = unicode_ctx->bitmap;
            char_buffer += unicodeCharacter->bitmap_offset;

            char_x_max = char_width;
            char_y_max = unicode_ctx->font->height;

            if (!unicode_ctx->font->crop) {
                // Take in account the skipped bytes, if any
                nb_skipped_bytes = (unicodeCharacter->x_min_offset & 7) << 3;
                nb_skipped_bytes |= unicodeCharacter->y_min_offset & 7;
                char_x_min = 0;
                char_y_min = 0;
            }
            else {
                nb_skipped_bytes = 0;
                char_x_min       = (uint16_t) unicodeCharacter->x_min_offset;
                char_y_min       = unicode_ctx->font->y_min;
                char_y_min += (uint16_t) unicodeCharacter->y_min_offset;
                char_x_max -= (uint16_t) unicodeCharacter->x_max_offset;
                char_y_max -= (uint16_t) unicodeCharacter->y_max_offset;
            }

            char_byte_cnt = nbgl_getUnicodeFontCharacterByteCount();
            encoding      = unicodeCharacter->encoding;
#endif  // defined(HAVE_LANGUAGE_PACK)
#else   // HAVE_UNICODE_SUPPORT
            continue;
#endif  // HAVE_UNICODE_SUPPORT
        }
        else {
            if (unicode == '\f') {
                break;
            }
            // if \b, switch fontId
            else if (unicode == '\b') {
                if (fontId == BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp) {  // switch to bold
                    fontId = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
#ifdef HAVE_UNICODE_SUPPORT
                    unicode_ctx = nbgl_getUnicodeFont(fontId);
#endif  // HAVE_UNICODE_SUPPORT
                    font = (const nbgl_font_t *) nbgl_getFont(fontId);
                }
                else if (fontId == BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp) {  // switch to regular
                    fontId = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
#ifdef HAVE_UNICODE_SUPPORT
                    unicode_ctx = nbgl_getUnicodeFont(fontId);
#endif  // HAVE_UNICODE_SUPPORT
                    font = (const nbgl_font_t *) nbgl_getFont(fontId);
                }
                continue;
            }
            // if not supported char, go to next one
            if ((unicode < font->first_char) || (unicode > font->last_char)) {
                continue;
            }
            character = (const nbgl_font_character_t *) PIC(
                &font->characters[unicode - font->first_char]);
            char_buffer = (const uint8_t *) PIC(&font->bitmap[character->bitmap_offset]);
            char_width  = character->width;
            encoding    = character->encoding;

            char_x_max = char_width;
            char_y_max = font->height;

            if (!font->crop) {
                // Take in account the skipped bytes, if any
                nb_skipped_bytes = (character->x_min_offset & 7) << 3;
                nb_skipped_bytes |= character->y_min_offset & 7;
                char_x_min = 0;
                char_y_min = 0;
            }
            else {
                nb_skipped_bytes = 0;
                char_x_min       = (uint16_t) character->x_min_offset;
                char_y_min       = font->y_min;
                char_y_min += (uint16_t) character->y_min_offset;
                char_x_max -= (uint16_t) character->x_max_offset;
                char_y_max -= (uint16_t) character->y_max_offset;
            }

            char_byte_cnt = get_bitmap_byte_cnt(font, unicode);
        }

        // Render character
        rectArea.x0     = x + char_x_min;
        rectArea.y0     = area->y0 + char_y_min;
        rectArea.height = (char_y_max - char_y_min);
        rectArea.width  = (char_x_max - char_x_min);

        // If char_byte_cnt = 0, call nbgl_frontDrawImageRle to let speculos notice
        // a space character was 'displayed'
        if (!char_byte_cnt || encoding == 1) {
            nbgl_frontDrawImageRle(
                &rectArea, char_buffer, char_byte_cnt, fontColor, nb_skipped_bytes);
        }
        else {
            nbgl_frontDrawImage(&rectArea, char_buffer, NO_TRANSFORMATION, fontColor);
        }
        x += char_width - font->char_kerning;
    }
    return fontId;
}

#ifdef NBGL_QRCODE
static void nbgl_frontDrawQrInternal(const nbgl_area_t *area, color_t foregroundColor)
{
    int      size = qrcodegen_getSize(qrcode);
    uint16_t idx  = 0;

    nbgl_area_t qrArea = {.x0              = area->x0,
                          .y0              = area->y0,
                          .backgroundColor = area->backgroundColor,
                          // QR codes are 1 BPP only
                          .bpp = NBGL_BPP_1};
    if (size == QR_V4_NB_PIX_SIZE) {
        // for each point of the V4 QR code, paint 64 pixels in image (8 in width, 8 in height)
        qrArea.width  = 2;
        qrArea.height = QR_PIXEL_WIDTH_HEIGHT * 2 * size;
        // paint a column of 2*size pixels in width by 8 pixels in height
        for (int x = 0; x < size; x++) {
            idx = 0;
            for (int y = 0; y < size; y++) {
                // draw 2 columns at once
                QrDrawBuffer[idx] = qrcodegen_getModule(qrcode, x, y) ? 0xFF : 0x00;
                QrDrawBuffer[idx + QR_V4_NB_PIX_SIZE] = QrDrawBuffer[idx];
                idx += 1;
            }
            nbgl_frontDrawImage(&qrArea, QrDrawBuffer, NO_TRANSFORMATION, foregroundColor);
            qrArea.x0 += 2;
            nbgl_frontDrawImage(&qrArea, QrDrawBuffer, NO_TRANSFORMATION, foregroundColor);
            qrArea.x0 += 2;
            nbgl_frontDrawImage(&qrArea, QrDrawBuffer, NO_TRANSFORMATION, foregroundColor);
            qrArea.x0 += 2;
            nbgl_frontDrawImage(&qrArea, QrDrawBuffer, NO_TRANSFORMATION, foregroundColor);
            qrArea.x0 += 2;
        }
    }
    else {
        // for each point of the V10 QR code, paint 16 pixels in image (4 in width, 4 in height)
        qrArea.width  = QR_PIXEL_WIDTH_HEIGHT * size;
        qrArea.height = QR_PIXEL_WIDTH_HEIGHT;
        // paint a line of 4*size pixels in width by 4 pixels in height
        for (int y = 0; y < size; y++) {
            idx = 0;
            for (int x = 0; x < size; x++) {
                memset(&QrDrawBuffer[idx], qrcodegen_getModule(qrcode, x, y) ? 0xFF : 0x00, 2);
                idx += 2;
            }
            nbgl_frontDrawImage(&qrArea, QrDrawBuffer, NO_TRANSFORMATION, foregroundColor);
            qrArea.y0 += QR_PIXEL_WIDTH_HEIGHT;
        }
    }
}

/**
 * @brief Draws the given text into a V10 QR code (QR code version is fixed using
 * qrcodegen_VERSION_MIN/qrcodegen_VERSION_MAX in qrcodegen.h)
 *
 * @note y0 and height must be multiple 4 pixels, and background color is applied to 0's in 1BPP
 * bitmap.
 *
 * @param area position, size and color of the QR code to draw
 * @param version version of QR Code (4, 10, ...)
 * @param text text to encode
 * @param foregroundColor color to be applied to the 1's in QR code
 */
void nbgl_drawQrCode(const nbgl_area_t *area,
                     uint8_t            version,
                     const char        *text,
                     color_t            foregroundColor)
{
    bool ok = qrcodegen_encodeText(
        text, tempBuffer, qrcode, qrcodegen_Ecc_LOW, version, version, qrcodegen_Mask_AUTO, true);

    if (ok) {
        nbgl_frontDrawQrInternal(area, foregroundColor);
    }
    else {
        LOG_WARN(DRAW_LOGGER, "Impossible to draw QRCode text %s with version %d\n", text, version);
    }
}
#endif  // NBGL_QRCODE
