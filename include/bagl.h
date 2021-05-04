
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2021 Ledger
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

#ifndef BAGL_H_
#define BAGL_H_

#define BAGL_NOFILL 0
#define BAGL_FILL 1
#define BAGL_OUTLINE 2
#define BAGL_NOICON 3

#define BAGL_FILL_CIRCLE_1_OCTANT 1
#define BAGL_FILL_CIRCLE_2_OCTANT 2
#define BAGL_FILL_CIRCLE_3_OCTANT 4
#define BAGL_FILL_CIRCLE_4_OCTANT 8
#define BAGL_FILL_CIRCLE_5_OCTANT 16
#define BAGL_FILL_CIRCLE_6_OCTANT 32
#define BAGL_FILL_CIRCLE_7_OCTANT 64
#define BAGL_FILL_CIRCLE_8_OCTANT 128
#define BAGL_FILL_CIRCLE 0xFF
#define BAGL_FILL_CIRCLE_3PI2_2PI                                              \
  (BAGL_FILL_CIRCLE_1_OCTANT | BAGL_FILL_CIRCLE_2_OCTANT)
#define BAGL_FILL_CIRCLE_PI_3PI2                                               \
  (BAGL_FILL_CIRCLE_3_OCTANT | BAGL_FILL_CIRCLE_4_OCTANT)
#define BAGL_FILL_CIRCLE_0_PI2                                                 \
  (BAGL_FILL_CIRCLE_5_OCTANT | BAGL_FILL_CIRCLE_6_OCTANT)
#define BAGL_FILL_CIRCLE_PI2_PI                                                \
  (BAGL_FILL_CIRCLE_7_OCTANT | BAGL_FILL_CIRCLE_8_OCTANT)

// --------------------------------------------------------------------------------------

typedef enum bagl_components_type_e_ {
  BAGL_NONE = 0, // for fast memset clearing all components
  BAGL_BUTTON = 1,
  // BAGL_TEXTBOX, // NOT NECESSARY
  // BAGL_SENSEZONE, // sense only
  BAGL_LABEL,
  BAGL_RECTANGLE,
  BAGL_LINE,
  BAGL_ICON,
  BAGL_CIRCLE,
  BAGL_LABELINE, // label for which y coordinate is the baseline of the text,
                 // does not support vertical alignment
  BAGL_FLAG_TOUCHABLE = 0x80,
} bagl_components_type_e;
#define BAGL_TYPE_FLAGS_MASK 0x80

// --------------------------------------------------------------------------------------

typedef struct {
  bagl_components_type_e type;
  unsigned char userid;
  short x; // allow for out of screen rendering
  short y; // allow for out of screen rendering
  unsigned short width;
  unsigned short height;
  unsigned char stroke;
  unsigned char radius;
  unsigned char fill;
  unsigned int fgcolor;
  unsigned int bgcolor;
  unsigned short font_id;
  unsigned char icon_id;
} bagl_component_t;

#define BAGL_FONT_ID_MASK 0x0FFF
#define BAGL_FONT_ALIGNMENT_HORIZONTAL_MASK 0xC000
#define BAGL_FONT_ALIGNMENT_LEFT 0x0000
#define BAGL_FONT_ALIGNMENT_RIGHT 0x4000
#define BAGL_FONT_ALIGNMENT_CENTER 0x8000
#define BAGL_FONT_ALIGNMENT_VERTICAL_MASK 0x3000
#define BAGL_FONT_ALIGNMENT_TOP 0x0000
#define BAGL_FONT_ALIGNMENT_BOTTOM 0x1000
#define BAGL_FONT_ALIGNMENT_MIDDLE 0x2000

#define BAGL_STROKE_FLAG_ONESHOT 0x80

// avgcharwidth: the average character width in pixel
// stroke: contains the hold time at start and at end (flag oneshot + in x
// 100ms) iconid: contains the horizontal scroll speed (in pixel/second)
#define BAGL_LONGLABEL_TIME_MS(textlength, avgcharwidth, stroke, iconid)       \
  (2 * (textlength * avgcharwidth) * 1000 / (iconid) +                         \
   2 * (stroke & ~(BAGL_STROKE_FLAG_ONESHOT)) * 100)

// --------------------------------------------------------------------------------------

/**
 * helper structure to help handling icons
 */
typedef struct bagl_icon_details_s {
  unsigned int width;
  unsigned int height;
  // bit per pixel
  unsigned int bpp;
  const unsigned int *colors;
  const unsigned char *bitmap;
} bagl_icon_details_t;

// --------------------------------------------------------------------------------------

#ifdef HAVE_BAGL_GLYPH_ARRAY
typedef struct {
  unsigned int icon_id;
  unsigned int width;
  unsigned int height;
  unsigned int bits_per_pixel;
  const unsigned int
      *default_colors; // color array entry count is (1<<bits_per_pixel)
  const unsigned char *bitmap;
} bagl_glyph_array_entry_t;

extern bagl_glyph_array_entry_t const C_glyph_array[];
extern unsigned int const C_glyph_count;
#endif // HAVE_BAGL_GLYPH_ARRAY

// --------------------------------------------------------------------------------------

typedef struct {
  unsigned char char_width;
  unsigned char bitmap_byte_count;
  // unsigned char const * bitmap; // save space by only keeping the offset (2
  // bytes) instead of the char address (4 bytes)
  unsigned short bitmap_offset;
} bagl_font_character_t;

typedef struct {
  unsigned int font_id; // to allow for sparse font embedding with a linear enum
  unsigned char bpp;    // for antialiased fonts
  unsigned char char_height;
  unsigned char baseline_height;
  unsigned char char_kerning; // specific to the font
  unsigned short first_char;
  unsigned short last_char;
  const bagl_font_character_t *const characters;
#define PIC_CHAR(x) ((const bagl_font_character_t *)PIC(x))
  unsigned char const *bitmap; // single bitmap for all chars of a font
#define PIC_BMP(x) ((unsigned char const *)PIC(x))
} bagl_font_t;

extern const bagl_font_t *const C_bagl_fonts[];
#define PIC_FONT(x) ((bagl_font_t const *)PIC(x))
extern const unsigned int C_bagl_fonts_count;

#define BAGL_ENCODING_LATIN1 0

#define BAGL_FONT_OPEN_SANS_LIGHT_14px BAGL_FONT_OPEN_SANS_REGULAR_11_14PX
typedef enum {
  BAGL_FONT_LUCIDA_CONSOLE_8PX,
  BAGL_FONT_OPEN_SANS_LIGHT_16_22PX,
  BAGL_FONT_OPEN_SANS_REGULAR_8_11PX,
  BAGL_FONT_OPEN_SANS_REGULAR_10_13PX,
  BAGL_FONT_OPEN_SANS_REGULAR_11_14PX,
  BAGL_FONT_OPEN_SANS_REGULAR_13_18PX,
  BAGL_FONT_OPEN_SANS_REGULAR_22_30PX,
  BAGL_FONT_OPEN_SANS_SEMIBOLD_8_11PX,
  BAGL_FONT_OPEN_SANS_EXTRABOLD_11px = 8u, // validated on nano s
  BAGL_FONT_OPEN_SANS_LIGHT_16px = 9u,     // validated on nano s
  BAGL_FONT_OPEN_SANS_REGULAR_11px = 10u,  // validated on nano s
  BAGL_FONT_OPEN_SANS_SEMIBOLD_10_13PX,
  BAGL_FONT_OPEN_SANS_SEMIBOLD_11_16PX,
  BAGL_FONT_OPEN_SANS_SEMIBOLD_13_18PX,
  BAGL_FONT_SYMBOLS_0,
  BAGL_FONT_SYMBOLS_1,
  BAGL_FONT_LAST // MUST ALWAYS BE THE LAST, FOR AUTOMATED INVALID VALUE CHECKS
} bagl_font_id_e;

#define BAGL_FONT_SYMBOLS_0_CLEAR "\x80"
#define BAGL_FONT_SYMBOLS_0_DOT "\x81"
#define BAGL_FONT_SYMBOLS_0_LEFT "\x82"
#define BAGL_FONT_SYMBOLS_0_LIGHTNING "\x83"
#define BAGL_FONT_SYMBOLS_0_MINIRIGHT "\x84"
#define BAGL_FONT_SYMBOLS_0_DASHBOARD "\x85"
#define BAGL_FONT_SYMBOLS_0_SETTINGS "\x86"
#define BAGL_FONT_SYMBOLS_0_EXIT "\x87"
#define BAGL_FONT_SYMBOLS_1_CARET "\xA0"

// --------------------------------------------------------------------------------------

// Glyphs definitions
enum bagl_glyph_e {
  BAGL_GLYPH_NOGLYPH = 0,
  BAGL_GLYPH_LOGO_LEDGER_100,
  BAGL_GLYPH_LOGO_LEDGER_BLUE_120,
  BAGL_GLYPH_ICON_GEARS_50,
  BAGL_GLYPH_ICON_CLEAR_16,
  BAGL_GLYPH_ICON_BACKSPACE_20,
  BAGL_GLYPH_ICON_CHECK,
  BAGL_GLYPH_ICON_CROSS,
  BAGL_GLYPH_ICON_CHECK_BADGE,
  BAGL_GLYPH_ICON_LEFT,
  BAGL_GLYPH_ICON_RIGHT,
  BAGL_GLYPH_ICON_UP,
  BAGL_GLYPH_ICON_DOWN,
  BAGL_GLYPH_LOGO_LEDGER_MINI,
  BAGL_GLYPH_ICON_CROSS_BADGE,
  BAGL_GLYPH_ICON_DASHBOARD_BADGE,
  BAGL_GLYPH_ICON_PLUS,
  BAGL_GLYPH_ICON_LESS,
  BAGL_GLYPH_ICON_TOGGLE_ON,
  BAGL_GLYPH_ICON_TOGGLE_OFF,
  BAGL_GLYPH_ICON_LOADING_BADGE,
  BAGL_GLYPH_ICON_COG_BADGE,
  BAGL_GLYPH_ICON_WARNING_BADGE,
  BAGL_GLYPH_ICON_DOWNLOAD_BADGE,
  BAGL_GLYPH_ICON_TRANSACTION_BADGE,
  BAGL_GLYPH_ICON_BITCOIN_BADGE,
  BAGL_GLYPH_ICON_ETHEREUM_BADGE,
  BAGL_GLYPH_ICON_EYE_BADGE,
  BAGL_GLYPH_ICON_PEOPLE_BADGE,
  BAGL_GLYPH_ICON_LOCK_BADGE,
  BAGL_GLYPH_ICON_BLUE_CABLE,
  BAGL_GLYPH_TEXT_WELCOME,
  BAGL_GLYPH_LOGO_LEDGER_BOOT,
  BAGL_GLYPH_BATT_LEFT,
  BAGL_GLYPH_BATT_RIGHT,
  BAGL_GLYPH_ICON_LIGHTNING,
  BAGL_GLYPH_ICON_PLUG,
  BAGL_GLYPH_BADGE_DOWNLOAD_BLUE,
  BAGL_GLYPH_BADGE_WARNING_BLUE,
  BAGL_GLYPH_ICON_LOADER_BLUE,
  BAGL_GLYPH_BADGE_CHECKMARK_BLUE,
  BAGL_GLYPH_BADGE_WRENCH_BLUE,
  BAGL_GLYPH_BADGE_ERROR_BLUE,
  BAGL_GLYPH_BADGE_POWER_BLUE,
  BAGL_GLYPH_BADGE_CRITICAL_BLUE,
  BAGL_GLYPH_BADGE_ASSISTANCE_BLUE,
  BAGL_GLYPH_BADGE_LOCK_BLUE,
  BAGL_GLYPH_ICON_CHECKMARK_BLUE,
  BAGL_GLYPH_APP_FIRMWARE_BLUE,
  BAGL_GLYPH_BADGE_BLUE,
  BAGL_GLYPH_ICON_BRIGHTNESS_LOW_BLUE,
  BAGL_GLYPH_ICON_BRIGHTNESS_HIGH_BLUE,
};

// --------------------------------------------------------------------------------------
// return y<<16+x after string have been printed
unsigned short bagl_compute_line_width(unsigned short font_id,
                                       unsigned short width, const void *text,
                                       unsigned char text_length,
                                       unsigned char text_encoding);
int bagl_draw_string(unsigned short font_id, unsigned int color1,
                     unsigned int color0, int x, int y, unsigned int width,
                     unsigned int height, const void *text,
                     unsigned int text_length, unsigned char text_encoding);
void bagl_draw_bg(unsigned int color);
void bagl_draw_with_context(const bagl_component_t *component, const void *text,
                            unsigned short text_length,
                            unsigned char text_encoding);
void bagl_draw(const bagl_component_t *const component);
void bagl_draw_glyph(const bagl_component_t *component,
                     const bagl_icon_details_t *icon_details);
// void bagl_undraw_all(void);
// void bagl_undraw_id(unsigned short id);
// void bagl_user_input(unsigned short x, unsigned short y, unsigned char
// event_kind);

#ifdef HAVE_BAGL_GLYPH_ARRAY
// for user to setup the glyph matrix to be used
void bagl_set_glyph_array(const bagl_glyph_array_entry_t *array,
                          unsigned int count);
#endif // HAVE_BAGL_GLYPH_ARRAY

typedef struct bagl_animated_s {
  // the component to be animated
  // NOTE: icon_id holds the horizontal scroll speed
  // NOTE: stroke holds the pause time (unit is 100ms) when reaching each ends
  // of the content to scroll.
  bagl_component_t c;

  // the component context to be animated
  void *text;
  unsigned short text_length;
  unsigned char text_encoding;
  unsigned int current_char_idx;
  unsigned int current_x;
  unsigned int next_ms; // the next time checkpoint to perform an animation
} bagl_animated_t;

// perform step animation
void bagl_animate(bagl_animated_t *anim, unsigned int timestamp_ms,
                  unsigned int interval_ms);

// --------------------------------------------------------------------------------------
// start drawing a bitmap in the given area
void bagl_hal_draw_bitmap_within_rect(int x, int y, unsigned int width,
                                      unsigned int height,
                                      unsigned int color_count,
                                      const unsigned int *colors,
                                      unsigned int bit_per_pixel,
                                      const unsigned char *bitmap,
                                      unsigned int bitmap_length_bits);
// continue drawing the bitmap in the previously setup area, take care to use
// the same bpp
void bagl_hal_draw_bitmap_continue(unsigned int bit_per_pixel,
                                   const unsigned char *bitmap,
                                   unsigned int bitmap_length_bits);

void bagl_hal_draw_rect(unsigned int color, int x, int y, unsigned int width,
                        unsigned int height);
void bagl_action(const bagl_component_t *component, unsigned char event_kind);

// --------------------------------------------------------------------------------------
// internal exposed for smoother coding of printf functions

#endif // BAGL_H_
