
/**
 * @file nbgl_fonts.c
 * Implementation of fonts array
 */
#include <stdint.h>
#include <stdbool.h>

#include "os.h"

// Heavily based on lib_ux/src/ux_layout_paging_compute.c

// We implement a light mechanism in order to be able to retrieve the width of
// nano S characters, in the two possible fonts:
// - BAGL_FONT_OPEN_SANS_EXTRABOLD_11px,
// - BAGL_FONT_OPEN_SANS_REGULAR_11px.
#define NANOS_FIRST_CHAR 0x20
#define NANOS_LAST_CHAR  0x7F

// OPEN_SANS_REGULAR_11PX << 4 | OPEN_SANS_EXTRABOLD_11PX
const uint8_t nanos_characters_width[NANOS_LAST_CHAR - NANOS_FIRST_CHAR + 1] = {
    3 << 4 | 3,   /* code 0020 */
    3 << 4 | 3,   /* code 0021 */
    4 << 4 | 6,   /* code 0022 */
    7 << 4 | 7,   /* code 0023 */
    6 << 4 | 6,   /* code 0024 */
    9 << 4 | 10,  /* code 0025 */
    8 << 4 | 9,   /* code 0026 */
    2 << 4 | 3,   /* code 0027 */
    3 << 4 | 4,   /* code 0028 */
    3 << 4 | 4,   /* code 0029 */
    6 << 4 | 6,   /* code 002A */
    6 << 4 | 6,   /* code 002B */
    3 << 4 | 3,   /* code 002C */
    4 << 4 | 4,   /* code 002D */
    3 << 4 | 3,   /* code 002E */
    4 << 4 | 5,   /* code 002F */
    6 << 4 | 8,   /* code 0030 */
    6 << 4 | 6,   /* code 0031 */
    6 << 4 | 7,   /* code 0032 */
    6 << 4 | 7,   /* code 0033 */
    8 << 4 | 8,   /* code 0034 */
    6 << 4 | 6,   /* code 0035 */
    6 << 4 | 8,   /* code 0036 */
    6 << 4 | 7,   /* code 0037 */
    6 << 4 | 8,   /* code 0038 */
    6 << 4 | 8,   /* code 0039 */
    3 << 4 | 3,   /* code 003A */
    3 << 4 | 3,   /* code 003B */
    6 << 4 | 5,   /* code 003C */
    6 << 4 | 6,   /* code 003D */
    6 << 4 | 5,   /* code 003E */
    5 << 4 | 6,   /* code 003F */
    10 << 4 | 10, /* code 0040 */
    7 << 4 | 8,   /* code 0041 */
    7 << 4 | 7,   /* code 0042 */
    7 << 4 | 7,   /* code 0043 */
    8 << 4 | 8,   /* code 0044 */
    6 << 4 | 6,   /* code 0045 */
    6 << 4 | 6,   /* code 0046 */
    8 << 4 | 8,   /* code 0047 */
    8 << 4 | 8,   /* code 0048 */
    3 << 4 | 4,   /* code 0049 */
    4 << 4 | 5,   /* code 004A */
    7 << 4 | 8,   /* code 004B */
    6 << 4 | 6,   /* code 004C */
    10 << 4 | 11, /* code 004D */
    8 << 4 | 9,   /* code 004E */
    9 << 4 | 9,   /* code 004F */
    7 << 4 | 7,   /* code 0050 */
    9 << 4 | 9,   /* code 0051 */
    7 << 4 | 8,   /* code 0052 */
    6 << 4 | 6,   /* code 0053 */
    7 << 4 | 6,   /* code 0054 */
    8 << 4 | 8,   /* code 0055 */
    7 << 4 | 6,   /* code 0056 */
    10 << 4 | 11, /* code 0057 */
    6 << 4 | 8,   /* code 0058 */
    6 << 4 | 7,   /* code 0059 */
    6 << 4 | 7,   /* code 005A */
    4 << 4 | 5,   /* code 005B */
    4 << 4 | 5,   /* code 005C */
    4 << 4 | 5,   /* code 005D */
    6 << 4 | 7,   /* code 005E */
    5 << 4 | 6,   /* code 005F */
    6 << 4 | 7,   /* code 0060 */
    6 << 4 | 7,   /* code 0061 */
    7 << 4 | 7,   /* code 0062 */
    5 << 4 | 6,   /* code 0063 */
    7 << 4 | 7,   /* code 0064 */
    6 << 4 | 7,   /* code 0065 */
    5 << 4 | 6,   /* code 0066 */
    6 << 4 | 7,   /* code 0067 */
    7 << 4 | 7,   /* code 0068 */
    3 << 4 | 4,   /* code 0069 */
    4 << 4 | 5,   /* code 006A */
    6 << 4 | 7,   /* code 006B */
    3 << 4 | 4,   /* code 006C */
    10 << 4 | 10, /* code 006D */
    7 << 4 | 7,   /* code 006E */
    7 << 4 | 7,   /* code 006F */
    7 << 4 | 7,   /* code 0070 */
    7 << 4 | 7,   /* code 0071 */
    4 << 4 | 5,   /* code 0072 */
    5 << 4 | 6,   /* code 0073 */
    4 << 4 | 5,   /* code 0074 */
    7 << 4 | 7,   /* code 0075 */
    6 << 4 | 7,   /* code 0076 */
    9 << 4 | 10,  /* code 0077 */
    6 << 4 | 7,   /* code 0078 */
    6 << 4 | 7,   /* code 0079 */
    5 << 4 | 6,   /* code 007A */
    4 << 4 | 5,   /* code 007B */
    6 << 4 | 6,   /* code 007C */
    4 << 4 | 5,   /* code 007D */
    6 << 4 | 6,   /* code 007E */
    7 << 4 | 6,   /* code 007F */
};

static uint8_t get_character_width(char character, bool bold)
{
    if (bold) {
        // Bold.
        return nanos_characters_width[character - NANOS_FIRST_CHAR] & 0x0F;
    }
    else {
        // Regular.
        return (nanos_characters_width[character - NANOS_FIRST_CHAR] >> 0x04) & 0x0F;
    }
}

static unsigned int is_word_delim(unsigned char c)
{
    // return !((c >= 'a' && c <= 'z')
    //       || (c >= 'A' && c <= 'Z')
    //       || (c >= '0' && c <= '9'));
    return c == ' ' || c == '\n' || c == '\t' || c == '-' || c == '_';
}

// return the number of pages to be displayed when current page to show is 0
unsigned int nbgl_font_compute_paging(const char  *text_to_split,
                                      uint32_t     line_to_display,
                                      uint32_t     width_limit_in_pixels,
                                      bool         bold,
                                      const char **line_start,
                                      uint8_t     *line_len)
{
    // compute start/length of text for the current line
    uint32_t    line  = 1;
    const char *start = text_to_split;
    const char *end   = start + strlen(start);
    while (start < end) {
        if (line != 0 && ((start[0] == ' ') || (start[0] == '\n'))) {
            // Skip ' ' and '\n' if at a start of a new line
            start++;
        }
        char         current_char;
        unsigned int len             = 0;
        unsigned int linew           = 0;
        const char  *last_word_delim = start;
        // not reached end of content
        while (start + len < end
               // line is not full
               && linew <= width_limit_in_pixels) {
            // compute new line length
            current_char = start[len + 1];
            if (current_char >= NANOS_FIRST_CHAR || current_char <= NANOS_LAST_CHAR) {
                linew += get_character_width(current_char, bold);
            }
            if (linew > width_limit_in_pixels) {
                // we got a full line
                break;
            }
            unsigned char c = start[len];
            if (is_word_delim(c)) {
                last_word_delim = &start[len];
            }
            len++;
            // new line, don't go further
            if (c == '\n') {
                break;
            }
        }

        // if not splitting line onto a word delimiter, then cut at the previous word_delim, adjust
        // len accordingly (and a word delim has been found already)
        if (start + len < end && last_word_delim != start && len) {
            // if line split within a word
            if ((!is_word_delim(start[len - 1]) && !is_word_delim(start[len]))) {
                len = last_word_delim - start;
            }
        }

        // fill up the paging structure
        if (line_to_display != 0 && line_to_display == line) {
            *line_start = start;
            *line_len   = len;

            // won't compute all pages, we reached the one to display
            return 1;
        }

        // prepare for next line
        start += len;

        // skip to next line
        line++;
    }

    // return total number of line detected
    return line - 1;
}
