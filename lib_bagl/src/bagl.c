
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2022 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#ifdef HAVE_BAGL
#ifdef DISPLAY_FLOWS
#undef HAVE_BOLOS
#define PIC(a) (a)
#endif  // DISPLAY_FLOWS

#include "bagl.h"
#include <string.h>
#include <stdio.h>

#include "os.h"
#if defined(HAVE_LANGUAGE_PACK)
#include "ux.h"
#endif  // defined(HAVE_LANGUAGE_PACK)

/**
 Coordinate system for BAGL:
 ===========================

       0   X axis
      0 +----->
        |
Y axis  |   #####
        v  #######
           ##   ##
           #######
            #####
*/

// --------------------------------------------------------------------------------------
// Checks
// --------------------------------------------------------------------------------------

/*
#ifndef BAGL_COMPONENT_MAXCOUNT
#error BAGL_COMPONENT_MAXCOUNT not set
#endif // !BAGL_COMPONENT_MAXCOUNT
*/

#ifndef BAGL_WIDTH
#error BAGL_WIDTH not set
#endif  // !BAGL_WIDTH

#ifndef BAGL_HEIGHT
#error BAGL_HEIGHT not set
#endif  // !BAGL_HEIGHT

// --------------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------------

#define ICON_WIDTH 0
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef U2BE
#define U2BE(buf, off) ((((buf)[off] & 0xFF) << 8) | ((buf)[off + 1] & 0xFF))
#endif
#ifndef U4BE
#define U4BE(buf, off) ((U2BE(buf, off) << 16) | (U2BE(buf, off + 2) & 0xFFFF))
#endif

// 'Magic' number for x & y to mutualise code from bagl_draw_string
#define MAGIC_XY 12345

// --------------------------------------------------------------------------------------
// Variables
// --------------------------------------------------------------------------------------

#ifdef HAVE_BAGL_GLYPH_ARRAY
const bagl_glyph_array_entry_t *G_glyph_array;
unsigned int                    G_glyph_count;
#endif  // HAVE_BAGL_GLYPH_ARRAY

#if defined(HAVE_UNICODE_SUPPORT)
static const bagl_font_unicode_t *font_unicode;
static unsigned int               unicode_byte_count;
#endif  // defined(HAVE_UNICODE_SUPPORT)

#if defined(HAVE_LANGUAGE_PACK)
static const bagl_font_unicode_character_t *unicode_characters;
static const unsigned char                 *unicode_bitmap;
extern const LANGUAGE_PACK                 *language_pack;
#endif  // defined(HAVE_LANGUAGE_PACK)

// --------------------------------------------------------------------------------------
// API
// --------------------------------------------------------------------------------------

void screen_update(void);

// --------------------------------------------------------------------------------------
void bagl_draw_bg(unsigned int color)
{
    bagl_component_t c;
    memset(&c, 0, sizeof(c));
    c.type    = BAGL_RECTANGLE;
    c.userid  = BAGL_NONE;
    c.fgcolor = color;
    c.x       = 0;
    c.y       = 0;
    c.width   = BAGL_WIDTH;
    c.height  = BAGL_HEIGHT;
    c.fill    = BAGL_FILL;
    // draw the rect
    bagl_draw_with_context(&c, NULL, 0, 0);
}

#ifdef HAVE_BAGL_GLYPH_ARRAY
// --------------------------------------------------------------------------------------
// internal helper, get the glyph entry from the glyph id (sparse glyph array support)
const bagl_glyph_array_entry_t *bagl_get_glyph(unsigned int                    icon_id,
                                               const bagl_glyph_array_entry_t *glyph_array,
                                               unsigned int                    glyph_count)
{
    unsigned int i = glyph_count;

    while (i--) {
        // font id match this entry (non linear)
        if (glyph_array[i].icon_id == icon_id) {
            return &glyph_array[i];
        }
    }

    // id not found
    return NULL;
}
#endif  // HAVE_BAGL_GLYPH_ARRAY

// --------------------------------------------------------------------------------------
// internal helper, get the font entry from the font id (sparse font array support)
const bagl_font_t *bagl_get_font(unsigned int font_id)
{
    unsigned int i = C_bagl_fonts_count;
    font_id &= BAGL_FONT_ID_MASK;

    while (i--) {
        // font id match this entry (non indexed array)
        if (PIC_FONT(C_bagl_fonts[i])->font_id == font_id) {
            return PIC_FONT(C_bagl_fonts[i]);
        }
    }

    // id not found
    return NULL;
}

#if defined(HAVE_UNICODE_SUPPORT)
// ----------------------------------------------------------------------------
// Get the font entry from the font id (sparse font array support)
const bagl_font_unicode_t *bagl_get_font_unicode(unsigned int font_id)
{
    // Be sure we need to change font
    if (font_unicode && PIC_FONTU(font_unicode)->font_id == font_id) {
        return font_unicode;
    }
#if defined(HAVE_LANGUAGE_PACK)
    const uint8_t             *ptr = (const uint8_t *) language_pack;
    const bagl_font_unicode_t *font
        = (const void *) (PIC(ptr + PIC_LANGU(language_pack)->fonts_offset));
    unicode_characters = PIC_CHARU(ptr + PIC_LANGU(language_pack)->characters_offset);
    unicode_bitmap     = (const void *) (PIC(ptr + PIC_LANGU(language_pack)->bitmaps_offset));

    font_id &= BAGL_FONT_ID_MASK;

    for (unsigned int i = 0; i < PIC_LANGU(language_pack)->nb_fonts; i++) {
        if (PIC_FONTU(font)->font_id == font_id) {
            // Update all other global variables
            return PIC_FONTU(font);
        }
        // Compute next Bitmap offset
        unicode_bitmap += PIC_FONTU(font)->bitmap_len;

        // Update all pointers for next font
        unicode_characters += PIC_LANGU(language_pack)->nb_characters;
        ++font;
    }
#else   // defined(HAVE_LANGUAGE_PACK)
    font_id &= BAGL_FONT_ID_MASK;

    for (unsigned int i = 0; i < C_bagl_fonts_count; i++) {
        if (PIC_FONTU(C_bagl_fonts_unicode[i])->font_id == font_id) {
            // Update all other global variables
            return PIC_FONTU(C_bagl_fonts_unicode[i]);
        }
    }
#endif  // defined(HAVE_LANGUAGE_PACK)
    // id not found
    return NULL;
}

// ----------------------------------------------------------------------------
const bagl_font_unicode_character_t *get_unicode_character(unsigned int unicode)
{
#if defined(HAVE_LANGUAGE_PACK)
    const bagl_font_unicode_character_t *characters = PIC(unicode_characters);
    unsigned int                         n          = PIC_LANGU(language_pack)->nb_characters;
#else   // defined(HAVE_LANGUAGE_PACK)
    const bagl_font_unicode_character_t *characters = PIC_CHARU(font_unicode->characters);
    unsigned int                         n          = C_unicode_characters_count;
#endif  // defined(HAVE_LANGUAGE_PACK)
    // For the moment, let just parse the full array, but at the end let use
    // binary search as data are sorted by unicode value !
    for (unsigned i = 0; i < n - 1; i++, characters++) {
        if ((PIC_CHARU(characters))->char_unicode == unicode) {
            // Compute the number of bytes used to display this character
            unicode_byte_count = (PIC_CHARU(characters + 1))->bitmap_offset
                                 - (PIC_CHARU(characters))->bitmap_offset;
            return (PIC_CHARU(characters));
        }
    }
    // By default, let's use the last Unicode character, which should be the
    // 0x00FFFD one, used to replace unrecognized or unrepresentable character.

    // Compute the number of bytes used to display this character
    unicode_byte_count = font_unicode->bitmap_len - (PIC_CHARU(characters))->bitmap_offset;
    return (PIC_CHARU(characters));
}

// ----------------------------------------------------------------------------

bagl_font_t const *toggle_bold(unsigned short *font_id)
{
    bagl_font_t const *font;
    unsigned short     id        = *font_id & BAGL_FONT_ID_MASK;
    unsigned short     alignment = *font_id & ~(BAGL_FONT_ID_MASK);

    switch (id) {
        case BAGL_FONT_OPEN_SANS_REGULAR_11px:
            id = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px;
            break;
        case BAGL_FONT_OPEN_SANS_EXTRABOLD_11px:
        default:
            id = BAGL_FONT_OPEN_SANS_REGULAR_11px;
            break;
    }
    *font_id = id | alignment;

    // Update fonts
    font = bagl_get_font(*font_id);

    font_unicode = bagl_get_font_unicode(*font_id);
    if (font_unicode == NULL) {
        font = NULL;
    }

    return font;
}
#endif  // defined(HAVE_UNICODE_SUPPORT)

// --------------------------------------------------------------------------------------
// return the width of a text (first line only) for alignment processing
unsigned short bagl_compute_line_width(unsigned short font_id,
                                       unsigned short width,
                                       const void    *text,
                                       unsigned char  text_length,
                                       unsigned char  text_encoding)
{
    // We will mutualise code from bagl_draw_string(smaller, easier to maintain):
    return (unsigned short) bagl_draw_string(
        font_id, 0, 0, MAGIC_XY, MAGIC_XY, width, 0, text, text_length, text_encoding);
}

// --------------------------------------------------------------------------------------
// draw char until a char fit before reaching width
// TODO support hyphenation ??
int bagl_draw_string(unsigned short font_id,
                     unsigned int   fgcolor,
                     unsigned int   bgcolor,
                     int            x,
                     int            y,
                     unsigned int   width,
                     unsigned int   height,
                     const void    *text,
                     unsigned int   text_length,
                     unsigned char  text_encoding)
{
    // This function can be called just to compute line width: set a flag.
    unsigned char dont_draw = 0;
    // Is it a 'magic number' telling we just want to compute line width?
    if (x == MAGIC_XY && y == MAGIC_XY) {
        x = y     = 0;
        dont_draw = 1;
    }

    int          xx;
    unsigned int colors[16];
    colors[0]               = bgcolor;
    colors[1]               = fgcolor;
    unsigned int         ch = 0;
    unsigned int         bpp;
    const unsigned char *txt = text;

#if defined(HAVE_UNICODE_SUPPORT)
    // Make sure font_unicode is recomputed using current language pack
    font_unicode = NULL;
#else   // defined(HAVE_UNICODE_SUPPORT)
    UNUSED(text_encoding);
#endif  // defined(HAVE_UNICODE_SUPPORT)

    const bagl_font_t *font = bagl_get_font(font_id);
    if (font == NULL) {
        return 0;
    }
    bpp = font->bpp;
#if defined(HAVE_UNICODE_SUPPORT)
    unsigned int unicode = 0;
    font_unicode         = bagl_get_font_unicode(font_id);
    if (font_unicode == NULL) {
        return 0;
    }
#endif  // defined(HAVE_UNICODE_SUPPORT)

#ifdef BAGL_MULTICHROME
    if (bpp > 1) {
        // fgcolor = 0x7e7ecc
        // bgcolor = 0xeca529
        // $1 = {0xeca529, 0xc6985f, 0xa28b95, 0x7e7ecc}

        unsigned int color_count = 1 << bpp;
        memset(colors, 0, sizeof(colors));
        colors[0]               = bgcolor;
        colors[color_count - 1] = fgcolor;

        // compute for all base colors
        int off;
        for (off = 0; off < 3; off++) {
            int cfg = (fgcolor >> (off * 8)) & 0xFF;
            int cbg = (bgcolor >> (off * 8)) & 0xFF;

            int crange = MAX(cfg, cbg) - MIN(cfg, cbg) + 1;
            int cinc   = crange / (color_count - 1UL);

            if (cfg > cbg) {
                unsigned int i;
                for (i = 1; i < color_count - 1UL; i++) {
                    colors[i] |= MIN(0xFF, cbg + i * cinc) << (off * 8);
                }
            }
            else {
                unsigned int i;

                for (i = 1; i < color_count - 1UL; i++) {
                    colors[i] |= MIN(0xFF, cfg + (color_count - 1UL - i) * cinc) << (off * 8);
                }
            }
        }
    }
#endif  // BAGL_MULTICHROME // for the blue

    // always comparing this way, very optimized etc
    width += x;
    height += y;

    // initialize first index
    xx = x;

    // depending on encoding
    while (text_length--) {
        ch = *txt++;

#if defined(HAVE_UNICODE_SUPPORT)
        if (text_encoding == BAGL_ENCODING_UTF8) {
            // Handle UTF-8 decoding (RFC3629): (https://www.ietf.org/rfc/rfc3629.txt
            // Char. number range  |        UTF-8 octet sequence
            // (hexadecimal)    |              (binary)
            // --------------------+---------------------------------------------
            // 0000 0000-0000 007F | 0xxxxxxx
            // 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
            // 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
            // 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

            // 4 bytes UTF-8, Unicode 0x1000 to 0x1FFFF
            if (ch >= 0xF0 && text_length >= 3) {
                unicode = (ch & 0x07) << 18;
                unicode |= (*txt++ & 0x3F) << 12;
                unicode |= (*txt++ & 0x3F) << 6;
                unicode |= (*txt++ & 0x3F);
                text_length -= 3;

                // 3 bytes UTF-8, Unicode 0x800 to 0xFFFF
            }
            else if (ch >= 0xE0 && text_length >= 2) {
                unicode = (ch & 0x0F) << 12;
                unicode |= (*txt++ & 0x3F) << 6;
                unicode |= (*txt++ & 0x3F);
                text_length -= 2;

                // 2 bytes UTF-8, Unicode 0x80 to 0x7FF
                // (0xC0 & 0xC1 are unused and can be used to store something else)
            }
            else if (ch >= 0xC2 && text_length >= 1) {
                unicode = (ch & 0x1F) << 6;
                unicode |= (*txt++ & 0x3F);
                text_length -= 1;
            }
            else {
                unicode = 0;
            }
        }
#endif  // defined(HAVE_UNICODE_SUPPORT)

        unsigned char        ch_height   = font->height;
        int                  ch_offset_x = 0;
        int                  ch_x_max    = 0;
        int                  ch_offset_y = 0;
        unsigned char        ch_width    = 0;
        uint16_t             ch_bits     = 0;
        const unsigned char *ch_bitmap   = NULL;
        int                  ch_y        = y;

        if (ch < font->first_char || ch > font->last_char) {
#if defined(HAVE_UNICODE_SUPPORT)
            // All escape sequences \n, \f and \e should have been removed before.
            switch (ch) {
                case '\b':  // Bold toggle: turn Bold On/Off
                    if ((font = toggle_bold(&font_id)) == NULL) {
                        return 0;
                    }
                    continue;
                case '\e':                   // Escape character => ignore it and the extra byte.
                    if (text_length >= 1) {  // Take care of \e without additional byte!
                        ++txt;
                        text_length -= 1;
                    }
                    continue;
            }

            if (unicode) {
                const bagl_font_unicode_character_t *character;
                character = get_unicode_character(unicode);
                ch_width  = character->width;
                // Number of bits needed to display this character
                ch_bits = unicode_byte_count * 8;

#if defined(HAVE_LANGUAGE_PACK)
                ch_bitmap = PIC_BMPU(unicode_bitmap);
#else   // defined(HAVE_LANGUAGE_PACK)
                ch_bitmap = PIC_BMPU(font_unicode->bitmap);
#endif  // defined(HAVE_LANGUAGE_PACK)
                ch_bitmap += character->bitmap_offset;

                ch_offset_x = (uint16_t) character->x_min_offset;
                ch_x_max    = (uint16_t) character->x_max_offset;
                ch_offset_y = (unsigned int) character->y_min_offset;
                // Take in account the difference of baseline with reference value
                ch_offset_y -= (int) font_unicode->baseline - (int) FONT_BASELINE;
                ch_width -= character->x_min_offset;
                ch_width -= ch_x_max;
            }
            else
#endif  // defined(HAVE_UNICODE_SUPPORT)
            {
                if (ch >= 0xC0) {
                    ch_width = ch & 0x3F;
                }
                else if (ch >= 0x80) {
                    // open the glyph font
                    const bagl_font_t *font_symbols
                        = bagl_get_font((ch & 0x20) ? BAGL_FONT_SYMBOLS_1 : BAGL_FONT_SYMBOLS_0);
                    if (font_symbols != NULL) {
                        ch_bitmap = &PIC_BMP(
                            font_symbols->bitmap)[PIC_CHAR(font_symbols->characters)[ch & 0x1F]
                                                      .bitmap_offset];
                        ch_width  = PIC_CHAR(font_symbols->characters)[ch & 0x1F].width;
                        ch_height = font_symbols->height;
                        // align baselines
                        ch_y = y + font->baseline - font_symbols->baseline;
                    }
                }
                ch_bits = bpp * ch_width * ch_height;
            }
        }
        else {
            // retrieve the char bitmap
            ch -= font->first_char;
            ch_bitmap = &PIC_BMP(font->bitmap)[PIC_CHAR(font->characters)[ch].bitmap_offset];

            // Number of bits needed to display this character
            if (ch == (unsigned int) (font->last_char - font->first_char)) {
                ch_bits
                    = font->bitmap_len - (uint16_t) (PIC_CHAR(font->characters)[ch].bitmap_offset);
            }
            else {
                ch_bits = PIC_CHAR(font->characters)[ch + 1].bitmap_offset
                          - PIC_CHAR(font->characters)[ch].bitmap_offset;
            }
            ch_bits *= 8;

            ch_width = PIC_CHAR(font->characters)[ch].width;

            ch_offset_x = (uint16_t) PIC_CHAR(font->characters)[ch].x_min_offset;
            ch_x_max    = PIC_CHAR(font->characters)[ch].x_max_offset;
            ch_offset_y = (uint16_t) PIC_CHAR(font->characters)[ch].y_min_offset;
            // Take in account the difference of baseline with reference value
            ch_offset_y -= (int) font->baseline - (int) FONT_BASELINE;
            ch_width -= PIC_CHAR(font->characters)[ch].x_min_offset;
            ch_width -= ch_x_max;
        }

        if (dont_draw) {
            // go to next line if needed
            if ((int) width > 0 && (xx + ch_offset_x + (int) ch_width) > (int) width) {
                return xx;
            }
            // prepare for next char
            xx += ch_offset_x + (int) ch_width + ch_x_max;
        }
        else {
            // go to next line if needed
            if ((xx + ch_offset_x + (int) ch_width) > (int) width) {
                y += FONT_HEIGHT;  // no interleave

                // IGNORED for first line
                if ((y + ch_offset_y + (int) ch_height) > (int) height) {
                    // we're writing half height of the last line ... probably better to put some
                    // dashes
                    return (y << 16) | (xx & 0xFFFF);
                }

                // newline starts back at first x offset
                xx   = x;
                ch_y = y;
            }

            /* IGNORED for first line
               if (y + ch_height > height) {
               // we're writing half height of the last line ... probably better to put some dashes
               return;
               }
            */

            // chars are storred LSB to MSB in each char, packed chars. horizontal scan
            if (ch_bitmap) {
                bolos_err_t ret = bagl_hal_draw_bitmap_within_rect(xx + ch_offset_x,
                                                                   ch_y + ch_offset_y,
                                                                   ch_width,
                                                                   ch_height,
                                                                   (1 << bpp),
                                                                   colors,
                                                                   bpp,
                                                                   ch_bitmap,
                                                                   ch_bits);
                if (SWO_SUCCESS != ret) {
                    // Exiting as early as error is detected can permit to see
                    // on screen that there is an issue drawing some text
                    return xx;
                }
            }
            else {
                bagl_hal_draw_rect(bgcolor, xx, ch_y, ch_width, ch_height);
            }
            // prepare for next char
            xx += ch_offset_x + (int) ch_width + ch_x_max;
        }
    }

    if (dont_draw) {
        return xx;
    }
    else {
        // return newest position, for upcoming printf
        return (y << 16) | (xx & 0xFFFF);
    }
}

// --------------------------------------------------------------------------------------

// draw round or circle. unaliased.
// if radiusint is !=0 then draw a circle of color outline, and colorint inside
void bagl_draw_circle_helper(unsigned int  color,
                             int           x_center,
                             int           y_center,
                             unsigned int  radius,
                             unsigned char octants,
                             unsigned int  radiusint,
                             unsigned int  colorint)
{
    /*
       128 ***** 32
          *     *
      64 *       * 16
        *         *
        *         *
       4 *       * 1
          *     *
         8 ***** 2
    */

    int last_x;
    int x                = radius;
    int y                = 0;
    int decisionOver2    = 1 - x;  // Decision criterion divided by 2 evaluated at x=r, y=0
    int dradius          = radius - radiusint;
    last_x               = x;
    unsigned int drawint = (radiusint > 0 && dradius > 0 /*&& xint <= yint*/);

    while (y <= x) {
        if (octants & 1) {  //
            if (drawint) {
                bagl_hal_draw_rect(colorint, x_center, y + y_center, x - (dradius - 1), 1);
                bagl_hal_draw_rect(color, x_center + x - (dradius - 1), y + y_center, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center, y + y_center - 1, x, 1);
            }
        }
        if (octants & 2) {  //
            if (drawint) {
                if (last_x != x) {
                    bagl_hal_draw_rect(colorint, x_center, x + y_center, y - (dradius - 1), 1);
                }
                bagl_hal_draw_rect(color, x_center + y - (dradius - 1), x + y_center, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center, x + y_center - 1, y, 1);
            }
        }
        if (octants & 4) {  //
            if (drawint) {
                bagl_hal_draw_rect(colorint, x_center - x, y + y_center, x - (dradius - 1), 1);
                bagl_hal_draw_rect(color, x_center - x - (dradius - 1), y + y_center, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center - x, y + y_center - 1, x, 1);
            }
        }
        if (octants & 8) {  //
            if (drawint) {
                if (last_x != x) {
                    bagl_hal_draw_rect(colorint, x_center - y, x + y_center, y - (dradius - 1), 1);
                }
                bagl_hal_draw_rect(color, x_center - y - (dradius - 1), x + y_center, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center - y, x + y_center - 1, y, 1);
            }
        }
        if (octants & 16) {  //
            if (drawint) {
                bagl_hal_draw_rect(colorint, x_center, y_center - y, x - (dradius - 1), 1);
                bagl_hal_draw_rect(color, x_center + x - (dradius - 1), y_center - y, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center, y_center - y, x, 1);
            }
        }
        if (octants & 32) {  //
            if (drawint) {
                if (last_x != x) {
                    bagl_hal_draw_rect(colorint, x_center, y_center - x, y - (dradius - 1), 1);
                }
                bagl_hal_draw_rect(color, x_center + y - (dradius - 1), y_center - x, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center, y_center - x, y, 1);
            }
        }
        if (octants & 64) {  //
            if (drawint) {
                bagl_hal_draw_rect(colorint, x_center - x, y_center - y, x - (dradius - 1), 1);
                bagl_hal_draw_rect(color, x_center - x - (dradius - 1), y_center - y, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center - x, y_center - y, x, 1);
            }
        }
        if (octants & 128) {  //
            if (drawint) {
                if (last_x != x) {
                    bagl_hal_draw_rect(colorint, x_center - y, y_center - x, y - (dradius - 1), 1);
                }
                bagl_hal_draw_rect(color, x_center - y - (dradius - 1), y_center - x, dradius, 1);
            }
            else {
                bagl_hal_draw_rect(color, x_center - y, y_center - x, y, 1);
            }
        }

        last_x = x;
        y++;
        if (decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;  // Change in decision criterion for y -> y+1
        }
        else {
            x--;
            decisionOver2 += 2 * (y - x) + 1;  // Change for y -> y+1, x -> x-1
        }
    }
}

// --------------------------------------------------------------------------------------

#ifdef HAVE_BAGL_GLYPH_ARRAY
void bagl_set_glyph_array(const bagl_glyph_array_entry_t *array, unsigned int count)
{
    G_glyph_array = array;
    G_glyph_count = count;
}
#endif  // HAVE_BAGL_GLYPH_ARRAY

// --------------------------------------------------------------------------------------

void bagl_draw_with_context(const bagl_component_t *component,
                            const void             *context,
                            unsigned short          context_length,
                            unsigned char           context_encoding)
{
    // unsigned char comp_idx;
    int          halignment     = 0;
    int          valignment     = 0;
    int          baseline       = 0;
    unsigned int height_to_draw = 0;
    int          strwidth       = 0;
    unsigned int ellipsis_1_len = 0;
#ifdef HAVE_BAGL_ELLIPSIS
    const char *ellipsis_2_start = NULL;
#endif  // HAVE_BAGL_ELLIPSIS

#ifdef HAVE_BAGL_GLYPH_ARRAY
    const bagl_glyph_array_entry_t *glyph = NULL;
#endif  // HAVE_BAGL_GLYPH_ARRAY

    // DESIGN NOTE: always consider drawing onto a bg color filled image. (done upon undraw)

    /*
    // check if userid already exist, if yes, reuse entry
    for (comp_idx=0; comp_idx < BAGL_COMPONENT_MAXCOUNT; comp_idx++) {
      if (bagl_components[comp_idx].userid == component->userid) {
        goto idx_ok;
      }
    }

    // find the first empty entry
    for (comp_idx=0; comp_idx < BAGL_COMPONENT_MAXCOUNT; comp_idx++) {
      if (bagl_components[comp_idx].userid == BAGL_NONE) {
        goto idx_ok;
      }
    }
    // no more space :(
    //BAGL_THROW(NO_SPACE);
    return;


  idx_ok:
    */

    // strip the flags to match kinds
    unsigned int type = component->type & ~(BAGL_TYPE_FLAGS_MASK);

#if defined(HAVE_INDEXED_STRINGS)
    // BAGL_LABELINE_LOC are BAGL_LABELINE:
    if (type == BAGL_LABELINE_LOC) {
        type = BAGL_LABELINE;
    }
#endif  // defined(HAVE_INDEXED_STRINGS)

    // compute alignment if text provided and requiring special alignment
    if (type != BAGL_ICON) {
        const bagl_font_t *font = bagl_get_font(component->font_id);
        if (font) {
            baseline       = font->baseline;
            height_to_draw = component->height;

            if (context && context_length) {
                // compute with some margin to fit other characters and check if ellipsis algorithm
                // is required
                strwidth       = bagl_compute_line_width(component->font_id,
                                                   component->width + 100,
                                                   context,
                                                   context_length,
                                                   context_encoding);
                ellipsis_1_len = context_length;

#ifdef HAVE_BAGL_ELLIPSIS
                // ellipsis mode (ensure something is to be split!)
                if (strwidth > component->width && context_length > 4) {
                    unsigned int robin = 0;  // remove char by char either on the left or right side
                    unsigned int dots_len = bagl_compute_line_width(
                        component->font_id,
                        100 /*probably larger than ... whatever the font*/,
                        "...",
                        3,
                        context_encoding);
                    ellipsis_1_len   = context_length / 2;
                    ellipsis_2_start = ((const char *) context) + context_length / 2;
                    // split line in 2 halves, strip a char from end of left part, and from start of
                    // right part, reassemble with ... , repeat until it fits. NOTE: algorithm is
                    // wrong if special blank chars are inserted, they should be removed first
                    while (strwidth > component->width && ellipsis_1_len
                           && (context_length
                               - ((const void *) ellipsis_2_start - (const void *) context))) {
                        unsigned int left_part  = bagl_compute_line_width(component->font_id,
                                                                         component->width,
                                                                         context,
                                                                         ellipsis_1_len,
                                                                         context_encoding);
                        unsigned int right_part = bagl_compute_line_width(
                            component->font_id,
                            component->width,
                            ellipsis_2_start,
                            (context_length
                             - ((const void *) ellipsis_2_start - (const void *) context)),
                            context_encoding);
                        // update to check and to compute alignment if needed
                        strwidth = left_part + dots_len + right_part;
                        // only start to split if the middle char if odd context_length removal is
                        // not sufficient
                        if (strwidth > component->width) {
                            // either remove a left char, OR remove a right char
                            switch (robin) {
                                case 0:
                                    // remove a left char
                                    ellipsis_1_len--;
                                    break;
                                case 1:
                                    // remove a right char
                                    ellipsis_2_start++;
                                    break;
                            }
                            robin = (robin + 1) % 2;
                        }
                    }
                    // we've computed split positions
                }
#endif  // HAVE_BAGL_ELLIPSIS

                switch (component->font_id & BAGL_FONT_ALIGNMENT_HORIZONTAL_MASK) {
                    default:
                    case BAGL_FONT_ALIGNMENT_LEFT:
                        halignment = 0;
                        break;
                    case BAGL_FONT_ALIGNMENT_RIGHT:
                        halignment = MAX(0, component->width - strwidth);
                        break;
                    case BAGL_FONT_ALIGNMENT_CENTER:
                        // x   xalign      strwidth width
                        // '     '            '     '
                        //       ^
                        // xalign = x+ (width/2) - (strwidth/2) => align -x
                        halignment = MAX(0, component->width / 2 - strwidth / 2);
                        break;
                }

                switch (component->font_id & BAGL_FONT_ALIGNMENT_VERTICAL_MASK) {
                    default:
                    case BAGL_FONT_ALIGNMENT_TOP:
                        valignment = 0;
                        break;
                    case BAGL_FONT_ALIGNMENT_BOTTOM:
                        valignment = component->height - baseline;
                        break;
                    case BAGL_FONT_ALIGNMENT_MIDDLE:
                        // y                 yalign           charheight        height
                        // '                    '          v  '                 '
                        //                           baseline
                        // yalign = y+ (height/2) - (baseline/2) => align - y
                        valignment = component->height / 2 - baseline / 2 - 1;
                        break;
                }
            }
        }
    }

    unsigned int radius = component->radius;
    radius              = MIN(radius, MIN(component->width / 2, component->height / 2));

    // Check the type only, ignore the touchable flag
    switch (type) {
        /*
            button (B)
            <   |Icon|Space|Textstring|   >
                 I.w   W.w     T.w
            I.x = B.x+B.w/2-(I.w+W.w+T.w)/2
            W.x = I.x+I.w
            T.x = W.x+W.w = I.x+I.w+W.w = B.x+B.w/2-(I.w+W.w+T.w)/2+I.w+W.w =
           B.x+B.w/2-T.w/2+(I.w+W.w)/2
        */

        // Following types are supposed to draw a rectangle or write text or both
        case BAGL_BUTTON:
        case BAGL_LABEL:
        case BAGL_RECTANGLE:
        case BAGL_LINE:
        case BAGL_LABELINE:
            if ((type == BAGL_LINE) && (component->radius == 0)) {
                bagl_hal_draw_rect(component->fgcolor,
                                   component->x,
                                   component->y,
                                   component->width,
                                   component->height);
                break;
            }

            if ((type == BAGL_LABEL) || (type == BAGL_LABELINE)) {
                /*if (component->fill == BAGL_FILL)*/ {
#ifndef DISPLAY_FLOWS
                    bagl_hal_draw_rect(
                        component->bgcolor,
                        component->x,
                        component->y - (type == BAGL_LABELINE ? (baseline) : 0),
                        component->width,
                        (type == BAGL_LABELINE ? height_to_draw : component->height));
#endif  // DISPLAY_FLOWS
                }
            }
            else {
                // Draw the rounded or not, filled or not rectangle
                if (component->fill != BAGL_FILL) {
                    // inner
                    // centered top to bottom
                    bagl_hal_draw_rect(component->bgcolor,
                                       component->x + radius,
                                       component->y,
                                       component->width - 2 * radius,
                                       component->height);
                    // left to center rect
                    bagl_hal_draw_rect(component->bgcolor,
                                       component->x,
                                       component->y + radius,
                                       radius,
                                       component->height - 2 * radius);
                    // center rect to right
                    bagl_hal_draw_rect(component->bgcolor,
                                       component->x + component->width - radius - 1,
                                       component->y + radius,
                                       radius,
                                       component->height - 2 * radius);

                    // outline
                    // 4 rectangles (with last pixel of each corner not set)
                    bagl_hal_draw_rect(component->fgcolor,
                                       component->x + radius,
                                       component->y,
                                       component->width - 2 * radius,
                                       component->stroke);  // top
                    bagl_hal_draw_rect(component->fgcolor,
                                       component->x + radius,
                                       component->y + component->height - 1,
                                       component->width - 2 * radius,
                                       component->stroke);  // bottom
                    bagl_hal_draw_rect(component->fgcolor,
                                       component->x,
                                       component->y + radius,
                                       component->stroke,
                                       component->height - 2 * radius);  // left
                    bagl_hal_draw_rect(component->fgcolor,
                                       component->x + component->width - 1,
                                       component->y + radius,
                                       component->stroke,
                                       component->height - 2 * radius);  // right
                }
                else {
                    // centered top to bottom
                    bagl_hal_draw_rect(component->fgcolor,
                                       component->x + radius,
                                       component->y,
                                       component->width - 2 * radius,
                                       component->height);
                    // left to center rect
                    bagl_hal_draw_rect(component->fgcolor,
                                       component->x,
                                       component->y + radius,
                                       radius,
                                       component->height - 2 * radius);

                    // center rect to right
                    bagl_hal_draw_rect(component->fgcolor,
                                       component->x + component->width - radius,
                                       component->y + radius,
                                       radius,
                                       component->height - 2 * radius);
                }
#ifndef DISPLAY_FLOWS
                // draw corners
                if (radius > 1) {
                    unsigned int radiusint = 0;
                    // carve round when not filling
                    if ((component->fill != BAGL_FILL) && (component->stroke < radius)) {
                        radiusint = radius - component->stroke;
                    }
                    bagl_draw_circle_helper(component->fgcolor,
                                            component->x + radius,
                                            component->y + radius,
                                            radius,
                                            BAGL_FILL_CIRCLE_PI2_PI,
                                            radiusint,
                                            component->bgcolor);
                    bagl_draw_circle_helper(
                        component->fgcolor,
                        component->x + component->width - radius - component->stroke,
                        component->y + radius,
                        radius,
                        BAGL_FILL_CIRCLE_0_PI2,
                        radiusint,
                        component->bgcolor);
                    bagl_draw_circle_helper(
                        component->fgcolor,
                        component->x + radius,
                        component->y + component->height - radius - component->stroke,
                        radius,
                        BAGL_FILL_CIRCLE_PI_3PI2,
                        radiusint,
                        component->bgcolor);
                    bagl_draw_circle_helper(
                        component->fgcolor,
                        component->x + component->width - radius - component->stroke,
                        component->y + component->height - radius - component->stroke,
                        radius,
                        BAGL_FILL_CIRCLE_3PI2_2PI,
                        radiusint,
                        component->bgcolor);
                }
#endif  // DISPLAY_FLOWS
            }

            if (type == BAGL_LINE) {
                // No text for this type
                break;
            }

            // Text display
            if (context && context_length) {
                unsigned int pos     = 0;
                unsigned int fgcolor = component->fgcolor;
                unsigned int bgcolor = component->bgcolor;

                // Invert colors of text when rectangle/button is filled
                if (((type == BAGL_BUTTON) || (type == BAGL_RECTANGLE))
                    && (component->fill == BAGL_FILL)) {
                    fgcolor = component->bgcolor;
                    bgcolor = component->fgcolor;
                }

                pos = bagl_draw_string(
                    component->font_id,
                    fgcolor,
                    bgcolor,
                    component->x + halignment,
                    component->y + ((type == BAGL_LABELINE) ? -(baseline) : valignment),
                    component->width - halignment,
                    component->height - ((type == BAGL_LABELINE) ? 0 : valignment),
                    context,
                    ellipsis_1_len,
                    context_encoding);
#ifdef HAVE_BAGL_ELLIPSIS
                if (ellipsis_2_start) {
                    // draw ellipsis
                    pos = bagl_draw_string(
                        component->font_id,
                        fgcolor,
                        bgcolor,
                        (pos & 0xFFFF),
                        component->y + ((type == BAGL_LABELINE) ? -(baseline) : valignment),
                        component->width - halignment,
                        component->height - (type == BAGL_LABELINE ? 0 : valignment),
                        "...",
                        3,
                        context_encoding);
                    // draw the right part
                    bagl_draw_string(
                        component->font_id,
                        fgcolor,
                        bgcolor,
                        (pos & 0xFFFF),
                        component->y + ((type == BAGL_LABELINE) ? -(baseline) : valignment),
                        component->width - halignment,
                        component->height - ((type == BAGL_LABELINE) ? 0 : valignment),
                        ellipsis_2_start,
                        (context_length
                         - ((const void *) ellipsis_2_start - (const void *) context)),
                        context_encoding);
                }
#else
                (void) pos;
#endif  // HAVE_BAGL_ELLIPSIS
            }
            break;

#ifdef HAVE_BAGL_GLYPH_ARRAY
        case BAGL_ICON:
            // icon data follows are in the context
            if (component->icon_id != 0) {
                // select the default or custom glyph array
                if (context_encoding && G_glyph_array && G_glyph_count > 0) {
                    glyph = bagl_get_glyph(component->icon_id, G_glyph_array, G_glyph_count);
                }
                else {
                    glyph = bagl_get_glyph(component->icon_id, C_glyph_array, C_glyph_count);
                }

                // 404 glyph not found
                if (glyph == NULL) {
                    break;
                }

                // color accounted as bytes in the context length
                if (context_length) {
                    if ((1 << glyph->bits_per_pixel) * 4 != context_length) {
                        // invalid color count
                        break;
                    }
                    context_length /= 4;
                }
                // use default colors
                if (!context_length || !context) {
                    context_length = 1 << (glyph->bits_per_pixel);
                    context        = glyph->default_colors;
                }

                // center glyph in rect
                // draw the glyph from the bitmap using the context for colors
                bolos_err_t ret = bagl_hal_draw_bitmap_within_rect(
                    component->x + (component->width / 2 - glyph->width / 2),
                    component->y + (component->height / 2 - glyph->height / 2),
                    glyph->width,
                    glyph->height,
                    context_length,
                    (unsigned int *) context,  // Endianness remarkably ignored !
                    glyph->bits_per_pixel,
                    glyph->bitmap,
                    glyph->bits_per_pixel * (glyph->width * glyph->height));
                if (SWO_SUCCESS != ret) {
                    return;
                }
            }
            else {
                // context: <bitperpixel> [color_count*4 bytes (LE encoding)] <icon bitmap (raw
                // scan, LE)>

                unsigned int colors[4];
                unsigned int bpp = ((unsigned char *) context)[0];
                // no space to display that
                if (bpp > 2) {
                    break;
                }
                unsigned int i = 1 << bpp;
                while (i--) {
                    colors[i] = U4BE((unsigned char *) context, 1 + i * 4);
                }

                // draw the glyph from the bitmap using the context for colors
                bolos_err_t ret = bagl_hal_draw_bitmap_within_rect(
                    component->x,
                    component->y,
                    component->width,
                    component->height,
                    1 << bpp,
                    colors,
                    bpp,
                    ((unsigned char *) context) + 1 + (1 << bpp) * 4,
                    bpp * (component->width * component->height));
                if (SWO_SUCCESS != ret) {
                    return;
                }
            }
            break;
#endif  // HAVE_BAGL_GLYPH_ARRAY

        case BAGL_CIRCLE:
            // draw the circle (all 8 octants)
            bagl_draw_circle_helper(
                component->fgcolor,
                component->x + component->radius,
                component->y + component->radius,
                component->radius,
                0xFF,
                ((component->fill != BAGL_FILL && component->stroke < component->radius)
                     ? component->radius - component->stroke
                     : 0),
                component->bgcolor);
            break;

        default:
            break;
    }
}

// --------------------------------------------------------------------------------------
void bagl_draw_glyph(const bagl_component_t *component, const bagl_icon_details_t *icon_details)
{
    // no space to display that
    if (icon_details->bpp > 2) {
        return;
    }

    /*
    // take into account the remaining bits not byte aligned
    unsigned int w = ((component->width*component->height*icon_details->bpp)/8);
    if (w%8) {
      w++;
    }
    */

    // draw the glyph from the bitmap using the context for colors
    bolos_err_t ret = bagl_hal_draw_bitmap_within_rect(
        component->x,
        component->y,
        icon_details->width,
        icon_details->height,
        1 << (icon_details->bpp),
#ifdef DISPLAY_FLOWS
        PIC(icon_details->colors),
#else   // DISPLAY_FLOWS
        (unsigned int *) PIC((unsigned int) icon_details->colors),
#endif  // DISPLAY_FLOWS
        icon_details->bpp,
#ifdef DISPLAY_FLOWS
        PIC(icon_details->bitmap),
#else   // DISPLAY_FLOWS
        (unsigned char *) PIC((unsigned int) icon_details->bitmap),
#endif  // DISPLAY_FLOWS
        icon_details->bpp * (icon_details->width * icon_details->height));
    if (SWO_SUCCESS != ret) {
        return;
    }
}

// --------------------------------------------------------------------------------------

void bagl_draw(const bagl_component_t *component)
{
    // component without text
    bagl_draw_with_context(component, NULL, 0, 0);
}

void bagl_init(void)
{
#ifdef HAVE_BAGL_GLYPH_ARRAY
    G_glyph_array = NULL;
    G_glyph_count = 0;
#endif  // HAVE_BAGL_GLYPH_ARRAY

#if defined(HAVE_UNICODE_SUPPORT)
    font_unicode = NULL;
#endif  // defined(HAVE_UNICODE_SUPPORT)
}

#endif  // HAVE_BAGL
