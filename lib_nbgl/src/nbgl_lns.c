/**
 * @file nbgl_lns.c
 * @brief Implementation of screen management
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdlib.h>

#include "glyphs.h"
#include "os_pic.h"
#include "ux.h"
#include "io.h"
#include "nbgl_lns.h"
#include "nbgl_fonts.h"

/*********************
 *      DEFINES
 *********************/
// Maximum typical string length that fit in a screen line
// Technically we can fit more "'" but let's not consider this.
#define SCREEN_LINE_MAX_STRING_LEN 35

#define COLOR_EMPTY  0x000000
#define COLOR_FILLED 0xFFFFFF

/* Values for positioning elements, most comes from lib_ux/src/ux_layout_*.c */
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32

#define ICON_WIDTH    16
#define ICON_HEIGHT   16
#define ICON_X_BORDER 16
#define ICON_Y_BORDER 2

#define TEXT_X_BORDER             6
#define TEXT_X_OFFSET             TEXT_X_BORDER
#define TEXT_X_OFFSET_BESIDE_ICON 41

#define TEXT_Y_OFFSET_FIRST_LINE  12
#define TEXT_Y_OFFSET_SECOND_LINE 26
#define TEXT_Y_OFFSET_BELOW_ICON  28
#define TEXT_Y_OFFSET_MIDDLE_LINE 19

#define TEXT_WIDTH_BESIDE_ICON (SCREEN_WIDTH - TEXT_X_OFFSET_BESIDE_ICON - TEXT_X_BORDER)

#define TEXT_WIDTH 114

typedef enum {
    STATE_START_DISPLAY,
    STATE_CLEAR_SCREEN,
    STATE_DRAW_ICON,
    STATE_PROCESS_TEXT_AND_DRAW_LINE_1,
    STATE_DRAW_LINE_2,
    STATE_DRAW_LEFT_NAV,
    STATE_DRAW_RIGHT_NAV,
    STATE_DONE,
} DisplayState_e;

typedef struct {
    const char    *line2;
    uint8_t        line2_bold : 1;
    uint8_t        line2_len;
    uint8_t        pagination_pages;
    uint8_t        pagination_current_page;
    DisplayState_e display_state;
} ScreenCxt_t;

// boolean used to enable/forbid drawing/refresh
static bool objDrawingDisabled;

static ScreenCxt_t                      screen_ctx;
static nbgl_lnsScreenContent_t          screen_content;
static nbgl_lnsButtonCallback_t         callback_ctx;
static nbgl_screenTickerConfiguration_t ticker_ctx;

static bool clear_screen(void)
{
    bagl_element_t element    = {0};
    element.component.type    = BAGL_RECTANGLE;
    element.component.width   = SCREEN_WIDTH;
    element.component.height  = SCREEN_HEIGHT;
    element.component.fill    = BAGL_FILL;
    element.component.fgcolor = COLOR_EMPTY;
    element.component.bgcolor = COLOR_FILLED;

    io_seproxyhal_display_default(&element);
    return true;
}

static void display_glyph(const bagl_icon_details_t *icon_det,
                          uint8_t                    x,
                          uint8_t                    y,
                          uint8_t                    width,
                          uint8_t                    height)
{
    bagl_component_t component = {0};
    component.type             = BAGL_ICON;
    component.fill             = BAGL_FILL;
    component.x                = x;
    component.y                = y;
    component.width            = width;
    component.height           = height;
    component.fgcolor          = COLOR_FILLED;
    component.bgcolor          = COLOR_EMPTY;

    io_seproxyhal_display_icon(&component, icon_det);
}

static void display_string(const char *text, bool bold, bool centered, uint8_t x, uint8_t y)
{
    bagl_element_t element    = {0};
    element.component.type    = BAGL_LABELINE;
    element.component.x       = x;
    element.component.y       = y;
    element.component.height  = 32;
    element.component.fgcolor = COLOR_FILLED;
    element.component.bgcolor = COLOR_EMPTY;
    element.text              = text;

    if (bold) {
        element.component.font_id = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px;
    }
    else {
        element.component.font_id = BAGL_FONT_OPEN_SANS_REGULAR_11px;
    }

    if (centered) {
        element.component.width = SCREEN_WIDTH - 2 * x;
        element.component.font_id |= BAGL_FONT_ALIGNMENT_CENTER;
    }
    else {
        element.component.width = SCREEN_WIDTH - x - TEXT_X_OFFSET;
    }

    io_seproxyhal_display_default(&element);
}

static void display_string_with_len(const char *text,
                                    uint8_t     len,
                                    bool        bold,
                                    bool        centered,
                                    uint8_t     x,
                                    uint8_t     y)
{
    if (len == 0) {
        display_string(text, bold, centered, x, y);
    }
    else {
        char buffer[SCREEN_LINE_MAX_STRING_LEN + 1];

        if (len > SCREEN_LINE_MAX_STRING_LEN) {
            len = SCREEN_LINE_MAX_STRING_LEN;
        }

        memcpy(buffer, text, len);
        buffer[len] = '\0';

        display_string(buffer, bold, centered, x, y);
    }
}

static bool draw_icon(void)
{
    uint8_t x, y;

    if (screen_content.icon == NULL) {
        return false;
    }
    if (screen_content.centered) {
        x = (SCREEN_WIDTH - ICON_WIDTH) / 2;
        y = ICON_Y_BORDER;
    }
    else {
        x = ICON_X_BORDER;
        y = (SCREEN_HEIGHT - ICON_HEIGHT) / 2;
    }

    display_glyph(screen_content.icon, x, y, ICON_WIDTH, ICON_HEIGHT);

    return true;
}

static void u8toa(uint8_t i, char buff[static 4])
{
    uint8_t div;
    uint8_t offset = 0;

    div = i / 100;
    if (div) {
        buff[offset++] = '0' + div;
        i              = i % 100;
    }

    div = i / 10;
    if (offset != 0 || div) {
        buff[offset++] = '0' + div;
        i              = i % 10;
    }

    buff[offset++] = '0' + i;
    buff[offset++] = '\0';
}

static bool process_text_and_draw_line_1(void)
{
    const char *line1       = NULL;
    uint8_t     line1_len   = 0;
    uint8_t     line1_x     = TEXT_X_OFFSET;
    uint8_t     line1_y     = TEXT_Y_OFFSET_FIRST_LINE;
    uint8_t     lines_width = TEXT_WIDTH;

    screen_ctx.line2     = NULL;
    screen_ctx.line2_len = 0;

    if (screen_content.text == NULL) {
        return false;
    }

    if (screen_content.icon != NULL) {
        if (screen_content.centered) {
            // Consider subtext is NULL
            // Consider text fit in a single line
            display_string(screen_content.text,
                           screen_content.bold,
                           screen_content.centered,
                           TEXT_X_OFFSET,
                           TEXT_Y_OFFSET_BELOW_ICON);
            return true;
        }
        else {
            // Override default line positioning to support icon on the left of the screen
            lines_width = TEXT_WIDTH_BESIDE_ICON;
            line1_x     = TEXT_X_OFFSET_BESIDE_ICON;
            line1_y     = TEXT_Y_OFFSET_FIRST_LINE;
        }
    }

    if (screen_content.subtext == NULL) {
        uint8_t lines_nb
            = nbgl_font_compute_nb_page(screen_content.text, lines_width, screen_content.bold);
        if (lines_nb == 1) {
            // Center the text on screen
            line1_y = TEXT_Y_OFFSET_MIDDLE_LINE;
        }
        else if (lines_nb > 2) {
            // No pagination supported in this mode
            lines_nb = 2;
        }

        for (int line = 1; line <= lines_nb; line++) {
            const char *start;
            uint8_t     len;

            nbgl_font_compute_paging(
                screen_content.text, line, lines_width, screen_content.bold, &start, &len);
            if (line == 1) {
                line1     = screen_content.text;
                line1_len = len;
            }
            else {
                // line == 2
                screen_ctx.line2      = start;
                screen_ctx.line2_len  = len;
                screen_ctx.line2_bold = screen_content.bold;
            }
        }
    }
    else {
        // Consider text is single line
        // subtext is forced to regular font
        screen_ctx.line2_bold = false;

        // Compute pagination parameters
        if (screen_ctx.pagination_pages == 0) {
            screen_ctx.pagination_pages
                = nbgl_font_compute_nb_page(screen_content.subtext, lines_width, false);
            if (screen_content.pos & BACKWARD_DIRECTION) {
                screen_ctx.pagination_current_page = screen_ctx.pagination_pages;
            }
            else {
                screen_ctx.pagination_current_page = 1;
            }
        }

        if (screen_ctx.pagination_pages < 2) {
            // No paging needed
            line1 = screen_content.text;
            if (screen_ctx.pagination_pages == 1) {
                screen_ctx.line2 = screen_content.subtext;
            }
        }
        else {
            // Generate title with pagination
            // Consider the resulting text fit in screen
            char buffer[SCREEN_LINE_MAX_STRING_LEN];

#if 1
            // Spare flash by not using snprintf here
            char page[4];

            strlcpy(buffer, screen_content.text, sizeof(buffer));
            strlcat(buffer, " (", sizeof(buffer));
            u8toa(screen_ctx.pagination_current_page, page);
            strlcat(buffer, page, sizeof(buffer));
            strlcat(buffer, "/", sizeof(buffer));
            u8toa(screen_ctx.pagination_pages, page);
            strlcat(buffer, page, sizeof(buffer));
            strlcat(buffer, ")", sizeof(buffer));
#else
            snprintf(buffer,
                     sizeof(buffer),
                     "%s (%d/%d)",
                     screen_content.text,
                     screen_ctx.pagination_current_page,
                     screen_ctx.pagination_pages);
#endif

            line1 = buffer;

            const char *start;
            uint8_t     len;

            nbgl_font_compute_paging(screen_content.subtext,
                                     screen_ctx.pagination_current_page,
                                     lines_width,
                                     false,
                                     &start,
                                     &len);
            screen_ctx.line2     = start;
            screen_ctx.line2_len = len;
        }
    }

    if (line1 != NULL) {
        display_string_with_len(
            line1, line1_len, screen_content.bold, screen_content.centered, line1_x, line1_y);
    }
    return true;
}

static bool draw_line_2(void)
{
    if (screen_ctx.line2 == NULL) {
        return false;
    }

    uint8_t text_x = TEXT_X_OFFSET;
    uint8_t text_y = TEXT_Y_OFFSET_SECOND_LINE;
    if (screen_content.icon != NULL) {
        // icon present, text is shifted to right to give space to the icon
        text_x = TEXT_X_OFFSET_BESIDE_ICON;
    }
    display_string_with_len(screen_ctx.line2,
                            screen_ctx.line2_len,
                            screen_ctx.line2_bold,
                            screen_content.centered,
                            text_x,
                            text_y);
    return true;
}

static bool draw_left_nav(void)
{
    bool                left = false;
    nbgl_stepPosition_t pos  = screen_content.pos & STEP_POSITION_MASK;

    if ((screen_ctx.pagination_pages > 1) && (screen_ctx.pagination_current_page > 1)) {
        left = true;
    }

    if ((pos != SINGLE_STEP) && (pos != FIRST_STEP)) {
        left = true;
    }

    if (left) {
        if (screen_content.vertical_nav) {
            display_glyph(&C_icon_up, 2, 15, 7, 4);
        }
        else {
            display_glyph(&C_icon_left, 2, 12, 4, 7);
        }
        return true;
    }

    return false;
}

static bool draw_right_nav(void)
{
    bool                right = false;
    nbgl_stepPosition_t pos   = screen_content.pos & STEP_POSITION_MASK;

    if ((screen_ctx.pagination_pages > 1)
        && (screen_ctx.pagination_current_page < screen_ctx.pagination_pages)) {
        right = true;
    }
    if ((pos != SINGLE_STEP) && (pos != LAST_STEP)) {
        right = true;
    }

    if (right) {
        if (screen_content.vertical_nav) {
            display_glyph(&C_icon_down, 119, 15, 7, 4);
        }
        else {
            display_glyph(&C_icon_right, 122, 12, 4, 7);
        }
        return true;
    }

    return false;
}

static void display_screen_ctx(void)
{
    bool should_wait = false;

    if (io_seproxyhal_spi_is_status_sent()) {
        // Can't send a SEPROXYHAL STATUS, wait for later
        return;
    }

    if (objDrawingDisabled) {
        // Screen is requested by the OS
        return;
    }

    if (screen_ctx.display_state == STATE_DONE) {
        // Nothing to do anymore
        return;
    }

    while (should_wait == false) {
        screen_ctx.display_state++;

        switch (screen_ctx.display_state) {
            case STATE_CLEAR_SCREEN:
                should_wait = clear_screen();
                break;
            case STATE_DRAW_ICON:
                should_wait = draw_icon();
                break;
            case STATE_PROCESS_TEXT_AND_DRAW_LINE_1:
                should_wait = process_text_and_draw_line_1();
                break;
            case STATE_DRAW_LINE_2:
                should_wait = draw_line_2();
                break;
            case STATE_DRAW_LEFT_NAV:
                should_wait = draw_left_nav();
                break;
            case STATE_DRAW_RIGHT_NAV:
                should_wait = draw_right_nav();
                break;
            case STATE_DONE:
                return;
            default:
                PRINTF("Error invalid state %d\n", screen_ctx.display_state);
                return;
        }
    }
}

void nbgl_screenRedraw(void)
{
    screen_ctx.display_state = STATE_START_DISPLAY;
    display_screen_ctx();
}

void nbgl_refresh(void)
{
    display_screen_ctx();
}

void nbgl_objAllowDrawing(bool enable)
{
    objDrawingDisabled = !enable;
}

void nbgl_processUxDisplayedEvent(void)
{
    display_screen_ctx();
}

void nbgl_lns_buttonCallback(nbgl_buttonEvent_t buttonEvent)
{
    if (screen_ctx.pagination_pages > 1) {
        if ((buttonEvent == BUTTON_LEFT_PRESSED) && (screen_ctx.pagination_current_page > 1)) {
            screen_ctx.pagination_current_page--;
            nbgl_screenRedraw();
            return;
        }
        else if ((buttonEvent == BUTTON_RIGHT_PRESSED)
                 && (screen_ctx.pagination_current_page < screen_ctx.pagination_pages)) {
            screen_ctx.pagination_current_page++;
            nbgl_screenRedraw();
            return;
        }
    }

    if (callback_ctx != NULL) {
        callback_ctx(buttonEvent);
    }
}

void nbgl_screenHandler(uint32_t intervaleMs)
{
    // call ticker callback of top of stack if active and not expired yet (for a non periodic)
    if ((ticker_ctx.tickerCallback != NULL) && (ticker_ctx.tickerValue != 0)) {
        ticker_ctx.tickerValue -= MIN(ticker_ctx.tickerValue, intervaleMs);
        if (ticker_ctx.tickerValue == 0) {
            // rearm if intervale is not null, and call the registered function
            ticker_ctx.tickerValue = ticker_ctx.tickerIntervale;
            ticker_ctx.tickerCallback();
        }
    }
}

void nbgl_screenDraw(nbgl_lnsScreenContent_t          *content,
                     nbgl_lnsButtonCallback_t          onActionCallback,
                     nbgl_screenTickerConfiguration_t *ticker)
{
    memcpy(&screen_content, content, sizeof(screen_content));
    screen_content.text    = PIC(screen_content.text);
    screen_content.subtext = PIC(screen_content.subtext);

    screen_ctx.pagination_pages = 0;
    screen_ctx.display_state    = STATE_START_DISPLAY;

    callback_ctx = onActionCallback;
    if (ticker != NULL) {
        memcpy(&ticker_ctx, ticker, sizeof(ticker_ctx));
    }
    else {
        memset(&ticker_ctx, 0, sizeof(ticker_ctx));
    }

    display_screen_ctx();
}
