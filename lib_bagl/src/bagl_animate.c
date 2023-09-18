#ifdef HAVE_SE_SCREEN

#include <string.h>
#include <stdio.h>

#include "os_math.h"
#include "os_screen.h"
#include "bagl_animate.h"

#define LABEL_ANIMATE_ID 0x01

// fg and bg colors
#define C_BLACK 0x000000
#define C_WHITE 0xFFFFFF

// Font properties
#define FONT_REGULAR      BAGL_FONT_OPEN_SANS_REGULAR_11px
#define FONT_HEIGHT_PIXEL 11
#define ENCODING          BAGL_ENCODING_DEFAULT

// Margin on left and width of displayed text
#define SCREEN_MARGIN_PIXEL 8
#define LEFT_MARGIN         0
#define BOX_WIDTH           BAGL_WIDTH

// Margin on top and height of the displayed text
#define TOP_MARGIN ((BAGL_HEIGHT - FONT_HEIGHT_PIXEL) / 2)
#define BOX_HEIGHT FONT_HEIGHT_PIXEL

// Ppause ticker duration
#define TICKER_PAUSE 4

// Only used for the circle animation
#define DELIMITER "       "

// Only used for the classic animation
// Based on the number of spaces needed to fill the entire screen
#define NR_MAX_SPACE_PADDING 38

// Component used to display the label with the best properties
const bagl_component_t component = {
    BAGL_LABEL,
    LABEL_ANIMATE_ID,
    LEFT_MARGIN,  // x
    TOP_MARGIN,   // y
    BOX_WIDTH,    // width
    BOX_HEIGHT,   // height
    0,
    0,
    0,        // stroke, radius, fill
    C_WHITE,  // fg color
    C_BLACK,  // bg color
    FONT_REGULAR,
    0,
};

/* Reset values are depending on the animation type
   and compute at the context initialization */
static void animation_reset(bagl_anim_t *anim)
{
    anim->cur_x                = anim->reset_cur_x;
    anim->cur_char_idx         = anim->reset_cur_char_idx;
    anim->cur_char_w           = 0;
    anim->cur_char_displayed_w = 0;
}

/* Process the different width properties of the label to animate. */
static void animation_get_width_properties(bagl_anim_t *anim)
{
    const unsigned     font_id = component.font_id;
    const unsigned int idx     = anim->cur_char_idx;

    // compute the remaining length in pixel to display
    anim->rem_w = bagl_compute_line_width(font_id, 0, anim->label + idx, anim->len - idx, ENCODING);

    // compute the current char width in pixel
    anim->cur_char_w = bagl_compute_line_width(font_id, 0, anim->label + idx, 1, ENCODING);

    // comput the current char width already displayed in pixel
    anim->cur_char_displayed_w = anim->cur_x - (anim->tot_w - anim->rem_w);
}

/* Print the label and refresh the screen */
static void animation_draw(bagl_anim_t *anim)
{
    const unsigned font_id     = component.font_id;
    const char    *start_label = anim->label + anim->cur_char_idx;
    const uint16_t len         = anim->len - anim->cur_char_idx;

    bagl_draw_bg(component.bgcolor);

    bagl_draw_string(font_id,
                     component.fgcolor,
                     component.bgcolor,
                     component.x - anim->cur_char_displayed_w,
                     component.y,
                     component.width,
                     component.height,
                     start_label,
                     len,
                     ENCODING);

    bagl_hal_draw_rect(component.bgcolor, 0, 0, SCREEN_MARGIN_PIXEL, BAGL_HEIGHT);
    bagl_hal_draw_rect(
        component.bgcolor, BAGL_WIDTH - SCREEN_MARGIN_PIXEL, 0, SCREEN_MARGIN_PIXEL, BAGL_HEIGHT);
}

/**
 * Classic animation.
 */

typedef enum classic_state_e {
    STATE_FROM_RIGHT_TO_LEFT,
    STATE_FROM_LEFT_TO_RIGHT,
    STATE_LEFT_PAUSE,
    STATE_RIGHT_PAUSE,
    STATE_END,
} classic_state_t;

static void classic_right_to_left(bagl_anim_t *anim)
{
    /* Char has been entirely displayed ? */
    if (anim->cur_char_displayed_w < anim->cur_char_w) {
        ++anim->cur_x;
        return;
    }

    /* Start to display the next char of the text */
    ++anim->cur_char_idx;
    ++anim->cur_x;

    /* Left border has been reached ? [<------    ] */
    if (anim->rem_w - anim->cur_char_displayed_w <= component.width) {
        anim->cur_x         = anim->tot_w - anim->rem_w + anim->cur_char_displayed_w;
        anim->classic.state = STATE_LEFT_PAUSE;
    }
}

static void classic_left_to_right(bagl_anim_t *anim)
{
    /* Char has been entirely displayed ? */
    if (anim->cur_char_displayed_w >= anim->cur_char_w) {
        --anim->cur_char_idx;
    }

    /* Right border has been reached ? [     ------>] */
    if (anim->cur_x == 0 && anim->rem_w == anim->tot_w) {
        anim->cur_char_idx  = 0;
        anim->cur_x         = 0;
        anim->classic.state = STATE_FROM_RIGHT_TO_LEFT;
        return;
    }

    --anim->cur_x;
}

// A pause in classic animation is based on a number of ticker calls.
static void classic_pause(bagl_anim_t *anim, classic_state_t next_state)
{
    ++anim->classic.nr_ticker_pause;
    if (anim->classic.nr_ticker_pause == TICKER_PAUSE) {
        anim->classic.state           = next_state;
        anim->classic.nr_ticker_pause = 0;
    }
}

// Statemachine for the classic animation
static void classic_step(bagl_anim_t *anim)
{
    switch (anim->classic.state) {
        case STATE_FROM_RIGHT_TO_LEFT:
            classic_right_to_left(anim);
            break;
        case STATE_FROM_LEFT_TO_RIGHT:
            classic_left_to_right(anim);
            break;
        case STATE_LEFT_PAUSE:
            classic_pause(anim, STATE_FROM_LEFT_TO_RIGHT);
            break;
        case STATE_RIGHT_PAUSE:
            classic_pause(anim, STATE_FROM_RIGHT_TO_LEFT);
            break;
        case STATE_END:
            break;
    }
}

static void fill_label_with_space(char *label, uint16_t from, uint16_t n)
{
    for (uint16_t i = 0; i < n; ++i) {
        label[i + from] = ' ';
    }
}

static void classic_init(bagl_anim_t *anim, const unsigned char *label, size_t len)
{
    // Add auto padding spaces
    uint16_t nr_space = (MAX_LABEL_SIZE - len) / 2;
    if (nr_space > NR_MAX_SPACE_PADDING) {
        nr_space = NR_MAX_SPACE_PADDING;
    }

    /* <-- nr_space --><--   len   --><-- nr_space -->
       [              ][    label    ][              ] */
    anim->len = len + nr_space * 2;
    fill_label_with_space(anim->label, 0, nr_space);
    memcpy(anim->label + nr_space, label, len);
    fill_label_with_space(anim->label, len + nr_space, nr_space);

    // compute once the total width in pixel of the label
    anim->tot_w = bagl_compute_line_width(component.font_id, 0, anim->label, anim->len, ENCODING);
    anim->classic.state      = STATE_FROM_RIGHT_TO_LEFT;
    anim->reset_cur_x        = 0;
    anim->reset_cur_char_idx = 0;
    animation_reset(anim);
}

/**
 * Circle common
 */

static void circle_common_init(bagl_anim_t *anim, const unsigned char *label, size_t len)
{
    anim->circle.char_idx_limit = len - 1 + sizeof(DELIMITER) - 1;
    anim->len                   = 0;

    /* <--    len     --><-- len(DELIMITER) --><--    len    -->
       [     label      ][                    ][     label     ] */

    while ((anim->len + len + sizeof(DELIMITER)) < MAX_LABEL_SIZE) {
        snprintf((char *) (anim->label + anim->len),
                 sizeof(anim->label) - anim->len,
                 "%s%s",
                 label,
                 DELIMITER);
        anim->len += len + sizeof(DELIMITER);
    }
    anim->len -= sizeof(DELIMITER);

    // compute once the total width in pixel of the label
    anim->tot_w = bagl_compute_line_width(component.font_id, 0, anim->label, anim->len, ENCODING);
}

/**
 * Circle right to left animation.
 */

static void circle_rl_step(bagl_anim_t *anim)
{
    /* Char has been entirely displayed ? */
    if (anim->cur_char_displayed_w < anim->cur_char_w) {
        ++anim->cur_x;
        return;
    }

    /* Reset when needed */
    if (anim->cur_char_idx == anim->circle.char_idx_limit) {
        animation_reset(anim);
        ++anim->cur_x;
        return;
    }

    /* Start to display the next char of the text */
    ++anim->cur_char_idx;
    ++anim->cur_x;
}

static void circle_rl_init(bagl_anim_t *anim, const unsigned char *label, size_t len)
{
    circle_common_init(anim, label, len);
    anim->reset_cur_char_idx = 0;
    anim->reset_cur_x        = 0;
    animation_reset(anim);
}

/**
 * Circle left to right animation.
 */

static void circle_lr_step(bagl_anim_t *anim)
{
    /* Char has been entirely displayed ? */
    if (anim->cur_char_displayed_w >= anim->cur_char_w) {
        if (anim->cur_char_idx == 0) {
            animation_reset(anim);
            ++anim->cur_x;
            return;
        }
        --anim->cur_char_idx;
    }

    /* Display next pixel */
    --anim->cur_x;
}

static void circle_lr_init(bagl_anim_t *anim, const unsigned char *label, size_t len)
{
    circle_common_init(anim, label, len);
    // reset values are processed to have a transparent animation for the user
    anim->reset_cur_x = bagl_compute_line_width(
        component.font_id, 0, anim->label, anim->circle.char_idx_limit, ENCODING);
    anim->reset_cur_char_idx = anim->circle.char_idx_limit;
    animation_reset(anim);
}

void bagl_animation_init(bagl_anim_t         *anim,
                         anim_type_t          type,
                         const unsigned char *label,
                         size_t               len)
{
    anim->type = type;
    memset(anim->label, 0, MAX_LABEL_SIZE);
    anim->max_char_w = bagl_compute_line_width(component.font_id, 0, "W", 1, ENCODING) + 1;

    switch (anim->type) {
        case ANIMATION_TYPE_CLASSIC:
            classic_init(anim, label, len);
            break;
        case ANIMATION_TYPE_CIRCLE_RL:
            circle_rl_init(anim, label, len);
            break;
        case ANIMATION_TYPE_CIRCLE_LR:
            circle_lr_init(anim, label, len);
            break;
        case ANIMATION_TYPE_END:
            break;
    };
}

void bagl_animation_step(bagl_anim_t *anim)
{
    animation_get_width_properties(anim);
    animation_draw(anim);
    screen_update();

    switch (anim->type) {
        case ANIMATION_TYPE_CLASSIC:
            classic_step(anim);
            break;
        case ANIMATION_TYPE_CIRCLE_RL:
            circle_rl_step(anim);
            break;
        case ANIMATION_TYPE_CIRCLE_LR:
            circle_lr_step(anim);
            break;
        case ANIMATION_TYPE_END:
            break;
    };
}

#endif /* HAVE_SE_SCREEN */
