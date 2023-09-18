#ifndef BAGL_ANIMATE_H_
#define BAGL_ANIMATE_H_

#include <stdlib.h>
#include <stdint.h>

#include "bagl.h"

typedef enum anim_type_e {
    /**
     * [  label  ] <<
     * [ label   ] <<
     * [label    ] <<
     * [abel     ] <<
     * [bel      ] pause
     * [abel     ] >>
     * [label    ] >>
     * [ label   ] >>
     * [  label  ] >>
     * [   label ] >>
     * [    label] >>
     * [     labe] >>
     * [      abe] pause
     * [     labe] <<
     * ...
     */
    ANIMATION_TYPE_CLASSIC,

    /**
     * [  label  ] <<
     * [ label   ] <<
     * [label    ] <<
     * [abel    l] <<
     * [bel    la] <<
     * [el    lab] <<
     * [l    labe] <<
     * [    label] <<
     * ...
     */
    ANIMATION_TYPE_CIRCLE_RL,

    /**
     * [  label  ] >>
     * [   label ] >>
     * [    label] >>
     * [l    labe] >>
     * [el    lab] >>
     * [bel    la] >>
     * [abel    l] >>
     * [label    ] >>
     * ...
     */
    ANIMATION_TYPE_CIRCLE_LR,

    ANIMATION_TYPE_END,
} anim_type_t;

typedef enum anim_speed_e {
    ANIMATION_SPEED_FAST,    // 10ms between two refreshes
    ANIMATION_SPEED_NORMAL,  // 30ms between two refreshes
    ANIMATION_SPEED_SLOW,    // 60ms between two refreshes
    ANIMATION_SPEED_END
} anim_speed_t;

// Maximum size of the label can be animated
#define MAX_LABEL_SIZE 128

/**
 * Structured used to animate a label.
 */
typedef struct bagl_anim_s {
    anim_type_t type;

    /* Label to animate */
    char   label[MAX_LABEL_SIZE];
    size_t len;

    /* Has to be set once at initialization */
    uint16_t tot_w;      /* total width of the label in pixel */
    uint16_t max_char_w; /* maximum char width based on "W" in pixel */
    uint16_t reset_cur_x;
    uint16_t reset_cur_char_idx;

    /* Current properties of the label to display */
    uint16_t rem_w;                /* remaining width to display in pixel */
    uint16_t cur_char_idx;         /* index of the current char of the label */
    uint16_t cur_char_w;           /* size in pixel of the current char */
    uint16_t cur_char_displayed_w; /* size in pixel displayed of the current char */
    uint16_t cur_x;                /* current x position on the screen */

    union {
        struct {
            uint8_t state;
            uint8_t nr_ticker_pause;
        } classic;
        struct {
            uint16_t char_idx_limit;
        } circle;
    };
} bagl_anim_t;

#ifdef HAVE_SE_SCREEN

/**
 * Initialize a label animation.
 */
void bagl_animation_init(bagl_anim_t         *anim,
                         anim_type_t          type,
                         const unsigned char *label,
                         size_t               len);

/**
 * Has to be called for each step of the animation.
 */
void bagl_animation_step(bagl_anim_t *anim);

#endif /* HAVE_SE_SCREEN */

#endif /* !BAGL_ANIMATE_H_ */
