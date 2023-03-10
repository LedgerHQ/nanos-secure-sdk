
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
#include "qrcodegen.h"
#include "glyphs.h"
#include "os_pic.h"
#include "os_utils.h"

/*********************
 *      DEFINES
 *********************/
#define BAGL_FILL_CIRCLE_3PI2_2PI 1
#define BAGL_FILL_CIRCLE_PI_3PI2  2
#define BAGL_FILL_CIRCLE_0_PI2    4
#define BAGL_FILL_CIRCLE_PI2_PI   8

#define QR_PIXEL_WIDTH_HEIGHT 4

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
  uint8_t QrDrawBuffer[QR_PIXEL_WIDTH_HEIGHT * QR_PIXEL_WIDTH_HEIGHT * QR_MAX_PIX_SIZE / 8];
} QrCodeBuffer_t;

#define qrcode ((QrCodeBuffer_t*)ramBuffer)->qrcode
#define tempBuffer ((QrCodeBuffer_t*)ramBuffer)->tempBuffer
#define QrDrawBuffer ((QrCodeBuffer_t*)ramBuffer)->QrDrawBuffer

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const uint8_t quarter_disc_4px_1bpp[] = {
  0x13, 0xFF
};
static const nbgl_icon_details_t C_quarter_disc_4px_1bpp= { 4, 4, NBGL_BPP_1, false, quarter_disc_4px_1bpp};

static const uint8_t quarter_circle_4px_1bpp[] = {
  0x13, 0xFF
};
static const nbgl_icon_details_t C_quarter_circle_4px_1bpp= { 4, 4, NBGL_BPP_1, false, quarter_circle_4px_1bpp};

// indexed by nbgl_radius_t (except RADIUS_0_PIXELS)
static const uint8_t radiusValues[] = {
  4, 8, 16, 20, 24, 32, 40, 48
};

// indexed by nbgl_radius_t (except RADIUS_0_PIXELS)
static const nbgl_icon_details_t* quarterDiscs[] = {
  &C_quarter_disc_4px_1bpp,
  &C_quarter_round_8px_1bpp,
  &C_quarter_round_16px_1bpp,
  &C_quarter_round_20px_1bpp,
  &C_quarter_round_24px_1bpp,
  &C_quarter_round_32px_1bpp,
  &C_quarter_round_40px_1bpp,
  &C_quarter_round_48px_1bpp
};

// indexed by nbgl_radius_t (except RADIUS_0_PIXELS)
static const nbgl_icon_details_t* quarterCircles[] = {
  &C_quarter_circle_4px_1bpp,
  &C_quarter_circle_8px_1bpp,
  &C_quarter_circle_16px_1bpp,
  &C_quarter_circle_20px_1bpp,
  &C_quarter_circle_24px_1bpp,
  &C_quarter_circle_32px_1bpp,
  &C_quarter_circle_40px_1bpp,
  &C_quarter_circle_48px_1bpp
};

#ifdef NBGL_QRCODE
// ensure that the ramBuffer also used for image file decompression is big enough for QR code
CCASSERT(qr_code_buffer, sizeof(QrCodeBuffer_t) <= GZLIB_UNCOMPRESSED_CHUNK);
#endif // NBGL_QRCODE

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_circle_helper(int x_center, int y_center, nbgl_radius_t radiusIndex, uint8_t quarter,
                               color_t borderColor, color_t innerColor, color_t backgroundColor) {
  uint8_t *quarter_buffer = NULL;
  nbgl_area_t area = {
    .bpp = NBGL_BPP_1,
    .backgroundColor = backgroundColor
  };

  // radius is not supported
  if (radiusIndex > RADIUS_48_PIXELS) {
    return;
  }
  if (borderColor == innerColor) {
    quarter_buffer = (uint8_t*)((nbgl_icon_details_t*)PIC(quarterDiscs[radiusIndex]))->bitmap;
  } else  {
    quarter_buffer = (uint8_t*)((nbgl_icon_details_t*)PIC(quarterCircles[radiusIndex]))->bitmap;
  }
  area.width = area.height = radiusValues[radiusIndex];
  area.backgroundColor = backgroundColor;
  if (quarter & BAGL_FILL_CIRCLE_3PI2_2PI) { //
    area.x0 = x_center;
    area.y0 = y_center;
    nbgl_frontDrawImage(&area,quarter_buffer,BOTH_MIRRORS,borderColor);
  }
  if (quarter & BAGL_FILL_CIRCLE_PI_3PI2) { //
    area.x0 = x_center-area.width;
    area.y0 = y_center;
    nbgl_frontDrawImage(&area,quarter_buffer,HORIZONTAL_MIRROR,borderColor);
  }
  if (quarter & BAGL_FILL_CIRCLE_0_PI2) { //
    area.x0 = x_center;
    area.y0 = y_center-area.width;
    nbgl_frontDrawImage(&area,quarter_buffer,VERTICAL_MIRROR,borderColor);
  }
  if (quarter & BAGL_FILL_CIRCLE_PI2_PI) { //
    area.x0 = x_center-area.width;
    area.y0 = y_center-area.width;
    nbgl_frontDrawImage(&area,quarter_buffer,NO_TRANSFORMATION,borderColor);
  }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief This functions draws a rounded corners rectangle (without border), with the given parameters.
 *
 * @param area position, size and background color (outside of the rectangle) to use for the rectangle
 * @param radiusIndex radius size
 * @param innerColor color to use for inside the rectangle
 */
void nbgl_drawRoundedRect(nbgl_area_t *area, nbgl_radius_t radiusIndex, color_t innerColor) {
  nbgl_area_t rectArea;
  uint8_t radius;

  LOG_DEBUG(DRAW_LOGGER,"nbgl_drawRoundedRect x0 = %d, y0 = %d, width =%d, height =%d\n",area->x0,area->y0,area->width,area->height);

  if (radiusIndex <= RADIUS_48_PIXELS) {
    radius = radiusValues[radiusIndex];
  }
  else if (radiusIndex == RADIUS_0_PIXELS) {
    radius = 0;
  }
  else {
    // radius not supported
    LOG_WARN(DRAW_LOGGER,"nbgl_drawRoundedRect forbidden radius index =%d\n",radiusIndex);
    return;
  }

  // Draw main inner rectangle
  rectArea.x0 = area->x0+radius;
  rectArea.y0 = area->y0;
  rectArea.width = area->width-(2*radius);
  rectArea.height = area->height;
  rectArea.backgroundColor = innerColor;
  nbgl_frontDrawRect(&rectArea);
  // special case when radius is null, just draw a rectangle
  if (radiusIndex == RADIUS_0_PIXELS) {
    return;
  }
  // Draw left inner rectangle
  rectArea.x0 = area->x0;
  rectArea.y0 = area->y0+radius;
  rectArea.width = radius;
  rectArea.height = area->height-(2*radius);
  nbgl_frontDrawRect(&rectArea);
  // Draw right inner rectangle
  rectArea.x0 = area->x0+area->width-radius;
  rectArea.y0 = area->y0+radius;
  rectArea.width = radius;
  rectArea.height = area->height-(2*radius);
  nbgl_frontDrawRect(&rectArea);

  // Draw 4 quarters of disc
  draw_circle_helper(     area->x0+radius,
                          area->y0+radius,
                          radiusIndex, BAGL_FILL_CIRCLE_PI2_PI,
                          innerColor,//unused
                          innerColor,
                          area->backgroundColor);
  draw_circle_helper(
                          area->x0+area->width-radius,
                          area->y0+radius,
                          radiusIndex, BAGL_FILL_CIRCLE_0_PI2,
                          innerColor,//unused
                          innerColor,
                          area->backgroundColor);
  draw_circle_helper(
                          area->x0+radius,
                          area->y0+area->height-radius,
                          radiusIndex, BAGL_FILL_CIRCLE_PI_3PI2,
                          innerColor,//unused
                          innerColor,
                          area->backgroundColor);
  draw_circle_helper(
                          area->x0+area->width-radius,
                          area->y0+area->height-radius,
                          radiusIndex, BAGL_FILL_CIRCLE_3PI2_2PI,
                          innerColor,//unused
                          innerColor,
                          area->backgroundColor);
}

/**
 * @brief This functions draws a rounded corners rectangle with a border, with the given parameters.
 *
 * @param area position, size and background color (outside of the rectangle) to use for the rectangle
 * @param radiusIndex radius size
 * @param stroke thickness of border (fixed to 2)
 * @param innerColor color to use for inside the rectangle
 * @param borderColor color to use for the border
 */
void nbgl_drawRoundedBorderedRect(nbgl_area_t *area, nbgl_radius_t radiusIndex, uint8_t stroke,
                                  color_t innerColor, color_t borderColor) {
  uint8_t maskTop, maskBottom;
  uint8_t radius;
  nbgl_area_t rectArea;

  LOG_DEBUG(DRAW_LOGGER,"nbgl_drawRoundedBorderedRect: innerColor = %d, borderColor = %d, backgroundColor=%d\n",innerColor,borderColor,area->backgroundColor);

  if (radiusIndex <= RADIUS_48_PIXELS) {
    radius = radiusValues[radiusIndex];
  }
  else if (radiusIndex == RADIUS_0_PIXELS) {
    radius = 0;
  }
  else {
    // radius not supported
    LOG_WARN(DRAW_LOGGER,"nbgl_drawRoundedBorderedRect forbidden radius index =%d\n",radiusIndex);
    return;
  }
  rectArea.backgroundColor = innerColor;

  // special case, when border_color == inner_color == background_color, just draw a rectangle
  if ((innerColor == borderColor) && (borderColor == area->backgroundColor )) {
    rectArea.x0 = area->x0;
    rectArea.y0 = area->y0;
    rectArea.width = area->width;
    rectArea.height = area->height;
    nbgl_frontDrawRect(&rectArea);
    return;
  }
  // Draw 3 rectangles
  if ((2*radius)<area->width) {
    rectArea.x0 = area->x0+radius;
    rectArea.y0 = area->y0;
    rectArea.width = area->width-(2*radius);
    rectArea.height = area->height;
    nbgl_frontDrawRect(&rectArea);
  }
  // special case when radius is null, left and right rectangles are not necessary
  if (radiusIndex <= RADIUS_48_PIXELS) {
    if ((2*radius)<area->height) {
      rectArea.x0 = area->x0;
      rectArea.y0 = area->y0+radius;
      rectArea.width = radius;
      rectArea.height = area->height-(2*radius);
      nbgl_frontDrawRect(&rectArea);
      rectArea.x0 = area->x0+area->width-radius;
      rectArea.y0 = area->y0+radius;
      nbgl_frontDrawRect(&rectArea);
    }
  }
  // border
  // 4 rectangles (with last pixel of each corner not set)
  if (stroke == 1) {
    maskTop = 0x1;
    maskBottom = 0x8;
  }
  else if (stroke == 2) {
    maskTop = 0x3;
    maskBottom = 0xC;
  }
  else if (stroke == 3) {
    maskTop = 0x7;
    maskBottom = 0xE;
  }
  else if (stroke == 4) {
    maskTop = 0xF;
    maskBottom = 0xF;
  }
  else {
    LOG_WARN(DRAW_LOGGER,"nbgl_drawRoundedBorderedRect forbidden stroke=%d\n",stroke);
    return;
  }
  rectArea.x0 = area->x0+radius;
  rectArea.y0 = area->y0;
  rectArea.width = area->width-2*radius;
  rectArea.height = 4;
  nbgl_frontDrawHorizontalLine(&rectArea, maskTop,borderColor); // bottom
  rectArea.x0 = area->x0+radius;
  rectArea.y0 = area->y0+area->height-4;
  nbgl_frontDrawHorizontalLine(&rectArea, maskBottom,borderColor); // bottom
  if ((2*radius)<area->height) {
    rectArea.x0 = area->x0;
    rectArea.y0 = area->y0+radius;
    rectArea.width = stroke;
    rectArea.height = area->height-2*radius;
    rectArea.backgroundColor = borderColor;
    nbgl_frontDrawRect(&rectArea); // left
    rectArea.x0 = area->x0+area->width-stroke;
    nbgl_frontDrawRect(&rectArea); // right
  }

  if (radiusIndex <= RADIUS_48_PIXELS) {
    // Draw 4 quarters of circles
    draw_circle_helper(
                          area->x0+radius,
                          area->y0+radius,
                          radiusIndex, BAGL_FILL_CIRCLE_PI2_PI,
                          borderColor,
                          innerColor,
                          area->backgroundColor);
    draw_circle_helper(
                          area->x0+area->width-radius,
                          area->y0+radius,
                          radiusIndex, BAGL_FILL_CIRCLE_0_PI2,
                          borderColor,
                          innerColor,
                          area->backgroundColor);
    draw_circle_helper(
                          area->x0+radius,
                          area->y0+area->height-radius,
                          radiusIndex, BAGL_FILL_CIRCLE_PI_3PI2,
                          borderColor,
                          innerColor,
                          area->backgroundColor);
    draw_circle_helper(
                          area->x0+area->width-radius,
                          area->y0+area->height-radius,
                          radiusIndex, BAGL_FILL_CIRCLE_3PI2_2PI,
                          borderColor,
                          innerColor,
                          area->backgroundColor);
  }
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
void nbgl_drawText(nbgl_area_t *area, const char* text, uint16_t textLen, nbgl_font_id_e fontId, color_t fontColor) {
  // text is a series of characters, each character being a bitmap
  // we need to align bitmaps on width multiple of 4 limitation.
  int16_t x = area->x0;
  nbgl_area_t rectArea;
  const nbgl_font_t *font = nbgl_getFont(fontId);

  LOG_DEBUG(DRAW_LOGGER,"nbgl_drawText: x0 = %d, y0 = %d, w = %d, h = %d, fontColor = %d, backgroundColor=%d, text = %s\n",area->x0, area->y0, area->width,area->height,fontColor,area->backgroundColor, text);

#ifdef HAVE_UNICODE_SUPPORT
  nbgl_unicode_ctx_t *unicode_ctx = nbgl_getUnicodeFont(fontId);
#endif // HAVE_UNICODE_SUPPORT

  rectArea.height = font->char_height;
  rectArea.backgroundColor = area->backgroundColor;
  rectArea.y0 = area->y0;
  rectArea.bpp = NBGL_BPP_1;
  while (textLen>0) {
    nbgl_font_character_t *character;
    uint8_t *char_buffer=NULL;
    uint8_t char_width;
    uint32_t unicode;
    bool is_unicode;

    unicode = nbgl_popUnicodeChar((uint8_t **)&text, &textLen, &is_unicode);

    if (is_unicode) {
#ifdef HAVE_UNICODE_SUPPORT
      const nbgl_font_unicode_character_t *unicodeCharacter = nbgl_getUnicodeFontCharacter(unicode);
      // if not supported char, go to next one
      if (unicodeCharacter == NULL) {
        continue;
      }
      char_width = unicodeCharacter->char_width;
#if defined(HAVE_LANGUAGE_PACK)
      char_buffer = (uint8_t*)unicode_ctx->bitmap;
      char_buffer+= unicodeCharacter->bitmap_offset;
#endif //defined(HAVE_LANGUAGE_PACK)
#else // HAVE_UNICODE_SUPPORT
      continue;
#endif // HAVE_UNICODE_SUPPORT
    }
    else {
      // if not supported char, go to next one
      if ((unicode < font->first_char) || (unicode > font->last_char)) {
        continue;
      }
      character = (nbgl_font_character_t *)PIC(&font->characters[unicode-font->first_char]);
      char_buffer = (uint8_t *)&font->bitmap[character->bitmap_offset];
      char_width = character->char_width;
    }
    rectArea.x0 = x;
    rectArea.width = char_width;
    nbgl_frontDrawImage(&rectArea, char_buffer,NO_TRANSFORMATION, fontColor);
    x+=char_width;
  }
}

#ifdef NBGL_QRCODE
static void nbgl_frontDrawQrInternal(nbgl_area_t *area, color_t foregroundColor)
{
  int size = qrcodegen_getSize(qrcode);
  uint16_t idx = 0;

  nbgl_area_t qrArea =
  {
      .x0 = area->x0,
      .y0 = area->y0,
      .backgroundColor = area->backgroundColor,
      // QR codes are 1 BPP only
      .bpp = NBGL_BPP_1
  };
  if (size == QR_V4_NB_PIX_SIZE) {
    // for each point of the V4 QR code, paint 64 pixels in image (8 in width, 8 in height)
    qrArea.width = 2;
    qrArea.height = QR_PIXEL_WIDTH_HEIGHT*2*size;
    // paint a column of 2*size pixels in width by 8 pixels in height
    for (int x = 0; x < size; x++) {
      idx = 0;
      for (int y = 0; y < size; y++) {
        // draw 2 columns at once
        QrDrawBuffer[idx] = qrcodegen_getModule(qrcode, x, y)?0xFF:0x00;
        QrDrawBuffer[idx+QR_V4_NB_PIX_SIZE] = QrDrawBuffer[idx];
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
    qrArea.width = QR_PIXEL_WIDTH_HEIGHT*size;
    qrArea.height = QR_PIXEL_WIDTH_HEIGHT;
    // paint a line of 4*size pixels in width by 4 pixels in height
    for (int y = 0; y < size; y++) {
    idx = 0;
      for (int x = 0; x < size; x++) {
        memset(&QrDrawBuffer[idx], qrcodegen_getModule(qrcode, x, y)?0xFF:0x00, 2);
        idx += 2;
      }
      nbgl_frontDrawImage(&qrArea, QrDrawBuffer, NO_TRANSFORMATION, foregroundColor);
      qrArea.y0 += QR_PIXEL_WIDTH_HEIGHT;
    }
  }
}

/**
 * @brief Draws the given text into a V10 QR code (QR code version is fixed using qrcodegen_VERSION_MIN/qrcodegen_VERSION_MAX in qrcodegen.h)
 *
 * @note y0 and height must be multiple 4 pixels, and background color is applied to 0's in 1BPP bitmap.
 *
 * @param area position, size and color of the QR code to draw
 * @param version version of QR Code (4, 10, ...)
 * @param text text to encode
 * @param foregroundColor color to be applied to the 1's in QR code
 */
void nbgl_drawQrCode(nbgl_area_t *area, uint8_t version, const char *text, color_t foregroundColor)
{

  bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode, qrcodegen_Ecc_LOW,
                                 version, version, qrcodegen_Mask_AUTO, true);

  if (ok) {
    nbgl_frontDrawQrInternal(area, foregroundColor);
  }
  else {
    LOG_WARN(DRAW_LOGGER,"Impossible to draw QRCode text %s with version %d\n",text, version);
  }
}
#endif // NBGL_QRCODE
