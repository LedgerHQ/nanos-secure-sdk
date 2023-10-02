#include "os_helpers.h"
#include "os_pic.h"
#include "ux.h"
#include "ux_layouts.h"
#include "bagl.h"
#include "string.h"
#include "os.h"

#ifdef HAVE_UX_FLOW
#ifdef HAVE_BAGL

#if !defined(HAVE_SE_SCREEN)

// We implement a light mechanism in order to be able to retrieve the width of
// nano S characters, in the two possible fonts:
// - BAGL_FONT_OPEN_SANS_EXTRABOLD_11px,
// - BAGL_FONT_OPEN_SANS_REGULAR_11px.
#define NANOS_FIRST_CHAR 0x20
#define NANOS_LAST_CHAR  0x7F

// OPEN_SANS_REGULAR_11PX << 4 | OPEN_SANS_EXTRABOLD_11PX
// Check with files (in the bagl_font_character_t array):
// - common/bagl/src/bagl_font_open_sans_extrabold_11px.inc
// - common/bagl/src/bagl_font_open_sans_regular_11px.inc
const char nanos_characters_width[96] = {
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

#if defined(HAVE_UNICODE_SUPPORT)
// That first array will helps find the index of the unicode char:
const unsigned int nanos_unicode_index[] = {
    0x0000A1, 0x0000BF, 0x0000C9, 0x0000E0, 0x0000E1, 0x0000E2, 0x0000E7,
    0x0000E8, 0x0000E9, 0x0000EA, 0x0000EB, 0x0000ED, 0x0000EE, 0x0000EF,
    0x0000F1, 0x0000F3, 0x0000F4, 0x0000F9, 0x0000FA, 0x0000FB
    // No need to enter FFFD, because it will be the one we'll use by default
    // 0x00FFFD
};

// OPEN_SANS_REGULAR_11PX_UNICODE << 4 | OPEN_SANS_EXTRABOLD_11PX_UNICODE
// Check with files (in the bagl_font_character_t array):
// - common/bagl/src/bagl_font_open_sans_extrabold_11px_unicode.inc
// - common/bagl/src/bagl_font_open_sans_regular_11px_unicode.inc
const unsigned char nanos_unicode_width[] = {
    3 << 4 | 3,  // unicode 0x0000A1
    5 << 4 | 6,  // unicode 0x0000BF
    6 << 4 | 6,  // unicode 0x0000C9
    6 << 4 | 7,  // unicode 0x0000E0
    6 << 4 | 7,  // unicode 0x0000E1
    6 << 4 | 7,  // unicode 0x0000E2
    5 << 4 | 6,  // unicode 0x0000E7
    6 << 4 | 7,  // unicode 0x0000E8
    6 << 4 | 7,  // unicode 0x0000E9
    6 << 4 | 7,  // unicode 0x0000EA
    6 << 4 | 7,  // unicode 0x0000EB
    4 << 4 | 5,  // unicode 0x0000ED
    5 << 4 | 6,  // unicode 0x0000EE
    4 << 4 | 6,  // unicode 0x0000EF
    7 << 4 | 7,  // unicode 0x0000F1
    7 << 4 | 7,  // unicode 0x0000F3
    7 << 4 | 7,  // unicode 0x0000F4
    7 << 4 | 7,  // unicode 0x0000F9
    7 << 4 | 7,  // unicode 0x0000FA
    7 << 4 | 7   // unicode 0x0000FB
    // No need to enter FFFD, because it will be the one we'll use by default
    // 11 << 4 |11    //unicode 0x00FFFD
};

#define DEFAULT_NANOS_UNICODE_WIDTH ((unsigned char) (11 << 4 | 11))  // For unicode FFFD

#define NB_NANOS_UNICODE_CHARS (sizeof(nanos_unicode_width) / sizeof(nanos_unicode_width[0]))
#endif  // defined(HAVE_UNICODE_SUPPORT)

// This function is used to retrieve the length of a string (expressed in bytes) delimited with a
// boundary width (expressed in pixels).
uint8_t se_get_cropped_length(const char *text,
                              uint8_t     text_length,
                              uint32_t    width_limit_in_pixels,
                              uint8_t     text_format)
{
    char     current_char;
    uint8_t  length;
    uint32_t current_width_in_pixels = 0;

    for (length = 0; length < text_length; length++) {
        current_char = text[length];

        if ((text_format & PAGING_FORMAT_NB) == PAGING_FORMAT_NB) {
            // Bold.
            current_width_in_pixels
                += nanos_characters_width[current_char - NANOS_FIRST_CHAR] & 0x0F;
        }
        else {
            // Regular.
            current_width_in_pixels
                += (nanos_characters_width[current_char - NANOS_FIRST_CHAR] >> 0x04) & 0x0F;
        }

        // We stop the processing when we reached the limit.
        if (current_width_in_pixels > width_limit_in_pixels) {
            break;
        }
    }

    return length;
}

// This function is used to retrieve the width of a line of text.
STATIC_IF_NOT_INDEXED unsigned int se_compute_line_width_light(const char *text,
                                                               uint8_t     text_length,
                                                               uint8_t     text_format)
{
    unsigned char ch;
    unsigned int  line_width = 0;
#if defined(HAVE_INDEXED_STRINGS)
    unsigned int  width       = 0;
    unsigned char bold_toggle = 0;

    // Bold state at the beginning of the line:
    if ((text_format & PAGING_FORMAT_NB) == PAGING_FORMAT_NB) {
        bold_toggle = 1;
    }
#endif  // defined(HAVE_INDEXED_STRINGS)

    // We parse the characters of the input text on all the input length.
    while (text_length--) {
        ch = *(const unsigned char *) text;
#if defined(HAVE_INDEXED_STRINGS)
        ++text;
        width = 0;
#endif  // defined(HAVE_INDEXED_STRINGS)

#if defined(HAVE_UNICODE_SUPPORT)
        unsigned int unicode;

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
            unicode |= (*((const unsigned char *) text + 0) & 0x3F) << 12;
            unicode |= (*((const unsigned char *) text + 1) & 0x3F) << 6;
            unicode |= (*((const unsigned char *) text + 2) & 0x3F);
            text += 3;
            text_length -= 3;

            // 3 bytes, from 0x800 to 0xFFFF
        }
        else if (ch >= 0xE0 && text_length >= 2) {
            unicode = (ch & 0x0F) << 12;
            unicode |= (*((const unsigned char *) text + 0) & 0x3F) << 6;
            unicode |= (*((const unsigned char *) text + 1) & 0x3F);
            text += 2;
            text_length -= 2;

            // 2 bytes UTF-8, Unicode 0x80 to 0x7FF
            // (0xC0 & 0xC1 are unused and can be used to store something else)
        }
        else if (ch >= 0xC2 && text_length >= 1) {
            unicode = (ch & 0x1F) << 6;
            unicode |= (*((const unsigned char *) text + 0) & 0x3F);
            ++text;
            text_length -= 1;
        }
        else {
            unicode = 0;
        }
#endif  // defined(HAVE_UNICODE_SUPPORT)

        if (ch < NANOS_FIRST_CHAR || ch > NANOS_LAST_CHAR) {
#if defined(HAVE_INDEXED_STRINGS)
            // Only proceed the first line width, not the whole paragraph
            switch (ch) {
                case '\n':
                case '\f':
                    return line_width;
                case '\b':  // Bold toggle: turn Bold On/Off
                    bold_toggle ^= 1;
                    continue;
                case '\e':                   // Escape character => ignore it and the extra byte.
                    if (text_length >= 1) {  // Take care of \e without additional byte!
                        ++text;
                        text_length -= 1;
                    }
                    continue;
            }
#else   // defined(HAVE_INDEXED_STRINGS)
            if (ch == '\n' || ch == '\r') {
                break;
            }
#endif  // defined(HAVE_INDEXED_STRINGS)

#if defined(HAVE_UNICODE_SUPPORT)
            if (unicode) {
                unsigned int i;
                // Find the index of the unicode character we are dealing with.
                // For the moment, let just parse the full array, but at the end let
                // use binary search as data are sorted by unicode value !
                for (i = 0; i < NB_NANOS_UNICODE_CHARS; i++) {
                    if (nanos_unicode_index[i] == unicode) {
                        break;
                    }
                }
                // Did we find a corresponding unicode entry?
                if (i < NB_NANOS_UNICODE_CHARS) {
                    width = nanos_unicode_width[i];
                }
                else {
                    // No, use FFFD character width:
                    width = DEFAULT_NANOS_UNICODE_WIDTH;
                }
            }
#endif  // defined(HAVE_UNICODE_SUPPORT)
        }
        else {
#if defined(HAVE_INDEXED_STRINGS)
            ch -= NANOS_FIRST_CHAR;
            width = nanos_characters_width[ch];  // 4 MSB = regular, 4 LSB = extrabold
#else                                            // defined(HAVE_INDEXED_STRINGS)
            //  We retrieve the character width, and the paging format indicates whether we are
            //  processing bold characters or not.
            if ((text_format & PAGING_FORMAT_NB) == PAGING_FORMAT_NB) {
                // Bold.
                line_width += nanos_characters_width[ch - NANOS_FIRST_CHAR] & 0x0F;
            }
            else {
                // Regular.
                line_width += (nanos_characters_width[ch - NANOS_FIRST_CHAR] >> 0x04) & 0x0F;
            }
#endif                                           // defined(HAVE_INDEXED_STRINGS)
        }
#if defined(HAVE_INDEXED_STRINGS)
        if (width) {
            if (!bold_toggle) {
                width >>= 4;  // 4 LSB = regular, now
            }
            width &= 0x0F;  // Keep only the 4 LSB
            line_width += width;
        }
#else   // defined(HAVE_INDEXED_STRINGS)
        text++;
#endif  // defined(HAVE_INDEXED_STRINGS)
    }
    return line_width;
}

#endif  // !HAVE_SE_SCREEN

static bool is_word_delim(unsigned char c)
{
    // return !((c >= 'a' && c <= 'z')
    //       || (c >= 'A' && c <= 'Z')
    //       || (c >= '0' && c <= '9'));
    return c == ' ' || c == '\n' || c == '-' || c == '_';
}

// return the number of pages to be displayed when current page to show is -1
unsigned int ux_layout_paging_compute(const char               *text_to_split,
                                      unsigned int              page_to_display,
                                      ux_layout_paging_state_t *paging_state,
                                      bagl_font_id_e            font)
{
#ifndef HAVE_FONTS
    UNUSED(font);
#endif

    // reset length and offset of lines
    memset(paging_state->offsets, 0, sizeof(paging_state->offsets));
    memset(paging_state->lengths, 0, sizeof(paging_state->lengths));

    // a page has been asked, but no page exists
    if (page_to_display >= paging_state->count && page_to_display != (unsigned int) -1) {
        return 0;
    }

    // compute offset/length of text of each line for the current page
    unsigned int page   = 0;
    unsigned int line   = 0;
    const char  *start  = (text_to_split ? STRPIC(text_to_split) : G_ux.externalText);
    const char  *start2 = start;
    const char  *end    = start + strlen(start);
    while (start < end) {
        unsigned int len             = 0;
        unsigned int linew           = 0;
        const char  *last_word_delim = start;
        // not reached end of content
        while (start + len < end
               // line is not full
               && linew <= PIXEL_PER_LINE
               // avoid display buffer overflow for each line
               // && len < sizeof(G_ux.string_buffer)-1
        ) {
            // compute new line length
#ifdef HAVE_FONTS
            linew = bagl_compute_line_width(font, 0, start, len + 1, BAGL_ENCODING_DEFAULT);
#else   // HAVE_FONTS
            linew = se_compute_line_width_light(start, len + 1, G_ux.layout_paging.format);
#endif  // HAVE_FONTS
        // if (start[len] )
            if (linew > PIXEL_PER_LINE) {
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
        // len accordingly (and a wor delim has been found already)
        if (start + len < end && last_word_delim != start && len) {
            // if line split within a word
            if ((!is_word_delim(start[len - 1]) && !is_word_delim(start[len]))) {
                len = last_word_delim - start;
            }
        }

        // fill up the paging structure
        if (page_to_display != (unsigned int) -1 && page_to_display == page
            && page_to_display < paging_state->count) {
            paging_state->offsets[line] = start - start2;
            paging_state->lengths[line] = len;

            // won't compute all pages, we reached the one to display
#if UX_LAYOUT_PAGING_LINE_COUNT > 1
            if (line >= UX_LAYOUT_PAGING_LINE_COUNT - 1)
#endif  // UX_LAYOUT_PAGING_LINE_COUNT
            {
                // a page has been computed
                return 1;
            }
        }

        // prepare for next line
        start += len;

        // skip to next line/page
        line++;
        if (
#if UX_LAYOUT_PAGING_LINE_COUNT > 1
            line >= UX_LAYOUT_PAGING_LINE_COUNT &&
#endif  // UX_LAYOUT_PAGING_LINE_COUNT
            start < end) {
            page++;
            line = 0;
        }
    }

    // return total number of page detected
    return page + 1;
}

#endif  // HAVE_BAGL
#endif  // HAVE_UX_FLOW
