#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "nbgl_obj.h"
#include "nbgl_screen.h"
#include "nbgl_debug.h"
#include "ux_loc.h"

#define UNUSED(x) (void) x

unsigned long gLogger = 0;

#ifdef HAVE_SE_TOUCH
#define GLYPH_switch_60_40_WIDTH  60
#define GLYPH_switch_60_40_HEIGHT 40
#define GLYPH_switch_60_40_ISFILE false
#define GLYPH_switch_60_40_BPP    1
uint8_t const C_switch_60_40_bitmap[] = {
    0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00,
    0x3f, 0xff, 0xfc, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff,
    0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff,
    0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff,
    0xf8, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc,
    0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f,
    0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff,
    0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff,
    0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0x00, 0xff, 0xfc, 0x3f, 0xf8, 0x00, 0x1f,
    0xfc, 0x3f, 0xf0, 0x00, 0x0f, 0xfc, 0x3f, 0xc0, 0x00, 0x03, 0xfc, 0x3f, 0x80, 0x00, 0x01, 0xfc,
    0x3f, 0x00, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x3e, 0x00, 0x00, 0x00, 0x7c, 0x3c,
    0x00, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x00, 0x3c, 0x38, 0x00,
    0x00, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x00,
    0x00, 0x1c, 0x38, 0x00, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x00,
    0x1c, 0x38, 0x00, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x38, 0x1c, 0x00, 0x00, 0x00, 0x38,
    0x1c, 0x00, 0x00, 0x00, 0x38, 0x0e, 0x00, 0x00, 0x00, 0x70, 0x0f, 0x00, 0x00, 0x00, 0xf0, 0x07,
    0x00, 0x00, 0x00, 0xe0, 0x03, 0x80, 0x00, 0x01, 0xc0, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x01, 0xf0,
    0x00, 0x0f, 0x80, 0x00, 0xf8, 0x00, 0x1f, 0x00, 0x00, 0x3f, 0x00, 0xfc, 0x00, 0x00, 0x1f, 0xff,
    0xf8, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
};
const nbgl_icon_details_t C_switch_60_40 = {GLYPH_switch_60_40_WIDTH,
                                            GLYPH_switch_60_40_HEIGHT,
                                            NBGL_BPP_1,
                                            GLYPH_switch_60_40_ISFILE,
                                            C_switch_60_40_bitmap};

#define GLYPH_radio_inactive_32px_WIDTH  32
#define GLYPH_radio_inactive_32px_HEIGHT 32
#define GLYPH_radio_inactive_32px_ISFILE true
#define GLYPH_radio_inactive_32px_BPP    1
uint8_t const C_radio_inactive_32px_bitmap[] = {
    0x20, 0x00, 0x20, 0x00, 0x02, 0x4a, 0x00, 0x00, 0xc8, 0xf0, 0x6e, 0xf0, 0x2f, 0x01,
    0xef, 0x05, 0xbf, 0x07, 0x99, 0x69, 0x77, 0xc7, 0x66, 0xe6, 0x56, 0xf0, 0x16, 0x36,
    0xf0, 0x36, 0x25, 0xf0, 0x55, 0x25, 0xf0, 0x55, 0x16, 0xf0, 0x5b, 0xf0, 0x7a, 0xf0,
    0x7a, 0xf0, 0x7a, 0xf0, 0x7a, 0xf0, 0x7a, 0xf0, 0x7b, 0xf0, 0x56, 0x15, 0xf0, 0x55,
    0x25, 0xf0, 0x55, 0x26, 0xf0, 0x36, 0x36, 0xf0, 0x16, 0x56, 0xe6, 0x67, 0xc7, 0x79,
    0x69, 0x9f, 0x07, 0xbf, 0x05, 0xef, 0x01, 0xf0, 0x2e, 0xf0, 0x68, 0xc0,
};
const nbgl_icon_details_t C_radio_inactive_32px = {GLYPH_radio_inactive_32px_WIDTH,
                                                   GLYPH_radio_inactive_32px_HEIGHT,
                                                   NBGL_BPP_1,
                                                   GLYPH_radio_inactive_32px_ISFILE,
                                                   C_radio_inactive_32px_bitmap};

#define GLYPH_radio_active_32px_WIDTH  32
#define GLYPH_radio_active_32px_HEIGHT 32
#define GLYPH_radio_active_32px_ISFILE true
#define GLYPH_radio_active_32px_BPP    1
uint8_t const C_radio_active_32px_bitmap[] = {
    0x20, 0x00, 0x20, 0x00, 0x02, 0x4a, 0x00, 0x00, 0xc8, 0xf0, 0x6e, 0xf0, 0x2f, 0x01,
    0xef, 0x05, 0xbf, 0x07, 0x99, 0x69, 0x77, 0xc7, 0x66, 0xe6, 0x56, 0xf0, 0x16, 0x36,
    0xf0, 0x36, 0x25, 0x84, 0x85, 0x25, 0x68, 0x65, 0x16, 0x5a, 0x5b, 0x6a, 0x6a, 0x5c,
    0x5a, 0x5c, 0x5a, 0x5c, 0x5a, 0x5c, 0x5a, 0x6a, 0x6b, 0x5a, 0x56, 0x15, 0x68, 0x65,
    0x25, 0x84, 0x85, 0x26, 0xf0, 0x36, 0x36, 0xf0, 0x16, 0x56, 0xe6, 0x67, 0xc7, 0x79,
    0x69, 0x9f, 0x07, 0xbf, 0x05, 0xef, 0x01, 0xf0, 0x2e, 0xf0, 0x68, 0xc0,
};
const nbgl_icon_details_t C_radio_active_32px = {GLYPH_radio_active_32px_WIDTH,
                                                 GLYPH_radio_active_32px_HEIGHT,
                                                 NBGL_BPP_1,
                                                 GLYPH_radio_active_32px_ISFILE,
                                                 C_radio_active_32px_bitmap};
#else  // HAVE_SE_TOUCH
#define GLYPH_bitcoin_logo_WIDTH  14
#define GLYPH_bitcoin_logo_HEIGHT 14
#define GLYPH_bitcoin_logo_ISFILE false
#define GLYPH_bitcoin_logo_BPP    1
uint8_t const C_bitcoin_logo_bitmap[] = {
    0x07, 0x80, 0x7f, 0x83, 0xff, 0x1f, 0x8e, 0x70, 0x1b, 0x82, 0x7c, 0xdc, 0xfb,
    0x77, 0xc0, 0x0d, 0x80, 0x66, 0xfd, 0x8f, 0xfc, 0x1f, 0xe0, 0x1e, 0x00,
};
const nbgl_icon_details_t C_bitcoin_logo = {GLYPH_bitcoin_logo_WIDTH,
                                            GLYPH_bitcoin_logo_HEIGHT,
                                            NBGL_BPP_1,
                                            GLYPH_bitcoin_logo_ISFILE,
                                            C_bitcoin_logo_bitmap};
#endif  // HAVE_SE_TOUCH

static bool fatal_reached = false;

void fetch_language_packs(void);

void mainExit(int exitCode)
{
    fatal_reached = true;
}

void nbgl_drawIcon(nbgl_area_t               *area __attribute__((unused)),
                   nbgl_color_map_t           color_map __attribute__((unused)),
                   const nbgl_icon_details_t *icon __attribute__((unused)))
{
}
void nbgl_drawRoundedRect(const nbgl_area_t *area __attribute__((unused)),
                          nbgl_radius_t      radius __attribute__((unused)),
                          color_t            innerColor __attribute__((unused)))
{
}
void nbgl_drawRoundedBorderedRect(const nbgl_area_t *area __attribute__((unused)),
                                  nbgl_radius_t      radius __attribute__((unused)),
                                  uint8_t            stroke __attribute__((unused)),
                                  color_t            innerColor __attribute__((unused)),
                                  color_t            borderColor __attribute__((unused)))
{
}
nbgl_font_id_e nbgl_drawText(const nbgl_area_t *area __attribute__((unused)),
                             const char        *text __attribute__((unused)),
                             uint16_t           textLen __attribute__((unused)),
                             nbgl_font_id_e     fontId,
                             color_t            fontColor __attribute__((unused)))
{
    return fontId;
}
void nbgl_drawQrCode(const nbgl_area_t *area __attribute__((unused)),
                     uint8_t            version __attribute__((unused)),
                     const char        *text __attribute__((unused)),
                     color_t            backgroundColor __attribute__((unused)))
{
}

void nbgl_frontDrawRect(const nbgl_area_t *area __attribute__((unused))) {}
void nbgl_frontDrawHorizontalLine(const nbgl_area_t *area __attribute__((unused)),
                                  uint8_t            mask __attribute__((unused)),
                                  color_t            lineColor __attribute__((unused)))
{
}
void nbgl_frontDrawImage(const nbgl_area_t    *area __attribute__((unused)),
                         const uint8_t        *buffer __attribute__((unused)),
                         nbgl_transformation_t transformation __attribute__((unused)),
                         nbgl_color_map_t      colorMap __attribute__((unused)))
{
}
void nbgl_frontDrawImageFile(const nbgl_area_t *area __attribute__((unused)),
                             const uint8_t     *buffer __attribute__((unused)),
                             nbgl_color_map_t   colorMap __attribute__((unused)),
                             const uint8_t     *uzlib_chunk_buffer __attribute__((unused)))
{
}
void nbgl_frontDrawImageRle(const nbgl_area_t *area __attribute__((unused)),
                            const uint8_t     *buffer __attribute__((unused)),
                            uint32_t           buffer_len __attribute__((unused)),
                            color_t            fore_color __attribute__((unused)),
                            uint8_t            nb_skipped_bytes __attribute__((unused)))
{
}
void nbgl_frontRefreshArea(const nbgl_area_t  *area __attribute__((unused)),
                           nbgl_refresh_mode_t mode __attribute__((unused)),
                           nbgl_post_refresh_t post_refresh __attribute__((unused)))
{
}

void nbgl_objDrawKeyboard(nbgl_keyboard_t *kbd __attribute__((unused))) {}
void nbgl_objDrawKeypad(nbgl_keypad_t *kbd __attribute__((unused))) {}

const char *get_ux_loc_string(UX_LOC_STRINGS_INDEX index __attribute__((unused)))
{
    return NULL;
}

uint8_t touch_exclude_borders(uint8_t excluded_borders)
{
    return excluded_borders;
}

void nbgl_screen_reinit(void)
{
    return;
}

#ifdef HAVE_SE_TOUCH
static void test_draw_obj(void **state __attribute__((unused)))
{
    nbgl_image_t          *image;
    nbgl_line_t           *line;
    nbgl_text_area_t      *textArea;
    nbgl_button_t         *button;
    nbgl_switch_t         *switchObj;
    nbgl_page_indicator_t *indicator;
    nbgl_progress_bar_t   *progress;
    nbgl_radio_t          *radio;
    nbgl_spinner_t        *spinner;
    nbgl_obj_t           **screenChildren = NULL;
    int                    layer;

    fetch_language_packs();
    nbgl_objInit();

    layer = nbgl_screenSet(&screenChildren, 9, NULL, NULL);
    assert_int_equal(layer, 0);
    assert_non_null(screenChildren);

    // all objects are drawn one under the other
    textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layer);
    assert_non_null(textArea);
    textArea->textColor       = WHITE;
    textArea->text            = "TRUST YOURSELF";
    textArea->textAlignment   = CENTER;
    textArea->fontId          = BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px;
    textArea->obj.alignTo     = NULL;
    textArea->obj.alignment   = TOP_LEFT;
    textArea->obj.area.width  = 340;
    textArea->obj.area.height = 80;
    screenChildren[0]         = (nbgl_obj_t *) textArea;

    image = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layer);
    assert_non_null(image);
    image->foregroundColor = BLACK;
    image->buffer          = &C_radio_active_32px;
    image->obj.alignTo     = (nbgl_obj_t *) textArea;
    image->obj.alignment   = BOTTOM_LEFT;
    image->obj.area.width  = 340;
    image->obj.area.height = 80;
    screenChildren[1]      = (nbgl_obj_t *) image;

    line = (nbgl_line_t *) nbgl_objPoolGet(LINE, layer);
    assert_non_null(line);
    line->lineColor       = BLACK;
    line->direction       = HORIZONTAL;
    line->offset          = 0;
    line->thickness       = 4;
    line->obj.alignTo     = (nbgl_obj_t *) image;
    line->obj.alignment   = BOTTOM_LEFT;
    line->obj.area.width  = 340;
    line->obj.area.height = 4;
    screenChildren[2]     = (nbgl_obj_t *) line;

    button = (nbgl_button_t *) nbgl_objPoolGet(BUTTON, layer);
    assert_non_null(button);
    button->fontId          = BAGL_FONT_INTER_REGULAR_24px;
    button->borderColor     = BLACK;
    button->foregroundColor = BLACK;
    button->innerColor      = WHITE;
    button->text            = "something";
    button->obj.alignTo     = (nbgl_obj_t *) line;
    button->obj.alignment   = BOTTOM_LEFT;
    button->obj.area.width  = 340;
    button->obj.area.height = 32;
    screenChildren[3]       = (nbgl_obj_t *) button;

    switchObj = (nbgl_switch_t *) nbgl_objPoolGet(SWITCH, layer);
    assert_non_null(switchObj);
    switchObj->offColor      = LIGHT_GRAY;
    switchObj->onColor       = BLACK;
    switchObj->state         = ON_STATE;
    switchObj->obj.alignTo   = (nbgl_obj_t *) button;
    switchObj->obj.alignment = BOTTOM_LEFT;
    screenChildren[4]        = (nbgl_obj_t *) switchObj;

    indicator = (nbgl_page_indicator_t *) nbgl_objPoolGet(PAGE_INDICATOR, layer);
    assert_non_null(indicator);
    indicator->activePage     = 0;
    indicator->nbPages        = 8;
    indicator->obj.alignTo    = (nbgl_obj_t *) switchObj;
    indicator->obj.alignment  = BOTTOM_LEFT;
    indicator->obj.area.width = 340;
    screenChildren[5]         = (nbgl_obj_t *) indicator;

    progress = (nbgl_progress_bar_t *) nbgl_objPoolGet(PROGRESS_BAR, layer);
    assert_non_null(progress);
    progress->state           = 10;
    progress->foregroundColor = BLACK;
    progress->obj.alignTo     = (nbgl_obj_t *) indicator;
    progress->obj.alignment   = BOTTOM_LEFT;
    progress->obj.area.width  = 340;
    progress->obj.area.height = 32;
    screenChildren[6]         = (nbgl_obj_t *) progress;

    radio = (nbgl_radio_t *) nbgl_objPoolGet(RADIO_BUTTON, layer);
    assert_non_null(radio);
    radio->state         = 10;
    radio->activeColor   = BLACK;
    radio->borderColor   = BLACK;
    radio->obj.alignTo   = (nbgl_obj_t *) progress;
    radio->obj.alignment = BOTTOM_LEFT;
    screenChildren[7]    = (nbgl_obj_t *) radio;

    spinner = (nbgl_spinner_t *) nbgl_objPoolGet(SPINNER, layer);
    assert_non_null(spinner);
    spinner->position      = 1;
    spinner->obj.alignTo   = (nbgl_obj_t *) radio;
    spinner->obj.alignment = BOTTOM_LEFT;
    screenChildren[8]      = (nbgl_obj_t *) spinner;
    nbgl_screenRedraw();

    // check positions of every object
    assert_int_equal(textArea->obj.area.x0, 0);
    assert_int_equal(textArea->obj.area.y0, 0);

    assert_int_equal(image->obj.area.x0, 0);
    assert_int_equal(image->obj.area.y0, 80);

    assert_int_equal(line->obj.area.x0, 0);
    assert_int_equal(line->obj.area.y0, 112);

    assert_int_equal(button->obj.area.x0, 0);
    assert_int_equal(button->obj.area.y0, 116);

    assert_int_equal(switchObj->obj.area.x0, 0);
    assert_int_equal(switchObj->obj.area.y0, 148);

    assert_int_equal(indicator->obj.area.x0, 0);
    assert_int_equal(indicator->obj.area.y0, 188);

    assert_int_equal(progress->obj.area.x0, 0);
    assert_int_equal(progress->obj.area.y0, 192);

    assert_int_equal(radio->obj.area.x0, 0);
    assert_int_equal(radio->obj.area.y0, 224);

    assert_int_equal(spinner->obj.area.x0, 0);
    assert_int_equal(spinner->obj.area.y0, 256);
}
#else   // HAVE_SE_TOUCH

static void test_draw_obj(void **state __attribute__((unused)))
{
    nbgl_image_t        *image;
    nbgl_text_area_t    *textArea;
    nbgl_progress_bar_t *progress;
    nbgl_obj_t         **screenChildren = NULL;
    int                  layer;

    fetch_language_packs();
    nbgl_objInit();

    layer = nbgl_screenSet(&screenChildren, 3, NULL, NULL);
    assert_int_equal(layer, 0);
    assert_non_null(screenChildren);

    // all objects are drawn one under the other
    textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layer);
    assert_non_null(textArea);
    textArea->textColor       = BLACK;
    textArea->text            = "TRUST";
    textArea->textAlignment   = CENTER;
    textArea->fontId          = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
    textArea->obj.alignTo     = NULL;
    textArea->obj.alignment   = TOP_LEFT;
    textArea->obj.area.width  = 80;
    textArea->obj.area.height = 16;
    screenChildren[0]         = (nbgl_obj_t *) textArea;

    image = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layer);
    assert_non_null(image);
    image->foregroundColor = BLACK;
    image->buffer          = &C_bitcoin_logo;
    image->obj.alignTo     = (nbgl_obj_t *) textArea;
    image->obj.alignment   = BOTTOM_LEFT;
    image->obj.area.width  = 20;
    image->obj.area.height = 80;
    screenChildren[1]      = (nbgl_obj_t *) image;

    progress = (nbgl_progress_bar_t *) nbgl_objPoolGet(PROGRESS_BAR, layer);
    assert_non_null(progress);
    progress->state           = 10;
    progress->foregroundColor = BLACK;
    progress->obj.alignTo     = (nbgl_obj_t *) image;
    progress->obj.alignment   = BOTTOM_LEFT;
    progress->obj.area.width  = 20;
    progress->obj.area.height = 32;
    screenChildren[2]         = (nbgl_obj_t *) progress;
    nbgl_screenRedraw();

    // check positions of every object
    assert_int_equal(textArea->obj.area.x0, 0);
    assert_int_equal(textArea->obj.area.y0, 0);

    assert_int_equal(image->obj.area.x0, 0);
    assert_int_equal(image->obj.area.y0, 16);

    assert_int_equal(progress->obj.area.x0, 0);
    assert_int_equal(progress->obj.area.y0, 30);
}
#endif  // HAVE_SE_TOUCH

int main(int argc, char **argv)
{
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_draw_obj)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
