#include "os_helpers.h"
#include "os_pic.h"
#include "ux.h"
#include "ux_layouts.h"
#include "bagl.h"
#include "string.h"

#ifdef HAVE_UX_FLOW
#ifdef HAVE_BAGL

#if !defined(HAVE_SE_SCREEN)
// We implement a light mecanism in order to be able to retrieve the width of
// nano S characters, in the two possible fonts:
// - BAGL_FONT_OPEN_SANS_EXTRABOLD_11px,
// - BAGL_FONT_OPEN_SANS_REGULAR_11px.
#define NANOS_FIRST_CHAR          0x20
#define NANOS_LAST_CHAR           0x7F

// OPEN_SANS_REGULAR_11PX << 4 | OPEN_SANS_EXTRABOLD_11PX
const char nanos_characters_width[96] = {
   3 << 4 |  3,   /* code 0020 */
   3 << 4 |  3,   /* code 0021 */
   4 << 4 |  6,   /* code 0022 */
   7 << 4 |  7,   /* code 0023 */
   6 << 4 |  6,   /* code 0024 */
   9 << 4 | 10,   /* code 0025 */
   8 << 4 |  9,   /* code 0026 */
   2 << 4 |  3,   /* code 0027 */
   3 << 4 |  4,   /* code 0028 */
   3 << 4 |  4,   /* code 0029 */
   6 << 4 |  6,   /* code 002A */
   6 << 4 |  6,   /* code 002B */
   3 << 4 |  3,   /* code 002C */
   4 << 4 |  4,   /* code 002D */
   3 << 4 |  3,   /* code 002E */
   4 << 4 |  5,   /* code 002F */
   6 << 4 |  8,   /* code 0030 */
   6 << 4 |  6,   /* code 0031 */
   6 << 4 |  7,   /* code 0032 */
   6 << 4 |  7,   /* code 0033 */
   8 << 4 |  8,   /* code 0034 */
   6 << 4 |  6,   /* code 0035 */
   6 << 4 |  8,   /* code 0036 */
   6 << 4 |  7,   /* code 0037 */
   6 << 4 |  8,   /* code 0038 */
   6 << 4 |  8,   /* code 0039 */
   3 << 4 |  3,   /* code 003A */
   3 << 4 |  3,   /* code 003B */
   6 << 4 |  5,   /* code 003C */
   6 << 4 |  6,   /* code 003D */
   6 << 4 |  5,   /* code 003E */
   5 << 4 |  6,   /* code 003F */
  10 << 4 | 10,   /* code 0040 */
   7 << 4 |  8,   /* code 0041 */
   7 << 4 |  7,   /* code 0042 */
   7 << 4 |  7,   /* code 0043 */
   8 << 4 |  8,   /* code 0044 */
   6 << 4 |  6,   /* code 0045 */
   6 << 4 |  6,   /* code 0046 */
   8 << 4 |  8,   /* code 0047 */
   8 << 4 |  8,   /* code 0048 */
   3 << 4 |  4,   /* code 0049 */
   4 << 4 |  5,   /* code 004A */
   7 << 4 |  8,   /* code 004B */
   6 << 4 |  6,   /* code 004C */
  10 << 4 | 11,   /* code 004D */
   8 << 4 |  9,   /* code 004E */
   9 << 4 |  9,   /* code 004F */
   7 << 4 |  7,   /* code 0050 */
   9 << 4 |  9,   /* code 0051 */
   7 << 4 |  8,   /* code 0052 */
   6 << 4 |  6,   /* code 0053 */
   7 << 4 |  6,   /* code 0054 */
   8 << 4 |  8,   /* code 0055 */
   7 << 4 |  6,   /* code 0056 */
  10 << 4 | 11,   /* code 0057 */
   6 << 4 |  8,   /* code 0058 */
   6 << 4 |  7,   /* code 0059 */
   6 << 4 |  7,   /* code 005A */
   4 << 4 |  5,   /* code 005B */
   4 << 4 |  5,   /* code 005C */
   4 << 4 |  5,   /* code 005D */
   6 << 4 |  7,   /* code 005E */
   5 << 4 |  6,   /* code 005F */
   6 << 4 |  7,   /* code 0060 */
   6 << 4 |  7,   /* code 0061 */
   7 << 4 |  7,   /* code 0062 */
   5 << 4 |  6,   /* code 0063 */
   7 << 4 |  7,   /* code 0064 */
   6 << 4 |  7,   /* code 0065 */
   5 << 4 |  6,   /* code 0066 */
   6 << 4 |  7,   /* code 0067 */
   7 << 4 |  7,   /* code 0068 */
   3 << 4 |  4,   /* code 0069 */
   4 << 4 |  5,   /* code 006A */
   6 << 4 |  7,   /* code 006B */
   3 << 4 |  4,   /* code 006C */
  10 << 4 | 10,   /* code 006D */
   7 << 4 |  7,   /* code 006E */
   7 << 4 |  7,   /* code 006F */
   7 << 4 |  7,   /* code 0070 */
   7 << 4 |  7,   /* code 0071 */
   4 << 4 |  5,   /* code 0072 */
   5 << 4 |  6,   /* code 0073 */
   4 << 4 |  5,   /* code 0074 */
   7 << 4 |  7,   /* code 0075 */
   6 << 4 |  7,   /* code 0076 */
   9 << 4 | 10,   /* code 0077 */
   6 << 4 |  7,   /* code 0078 */
   6 << 4 |  7,   /* code 0079 */
   5 << 4 |  6,   /* code 007A */
   4 << 4 |  5,   /* code 007B */
   6 << 4 |  6,   /* code 007C */
   4 << 4 |  5,   /* code 007D */
   6 << 4 |  6,   /* code 007E */
   7 << 4 |  6,   /* code 007F */
};

// This function is used to retrieve the length of a string (expressed in bytes) delimited with a boundary width (expressed in pixels).
uint8_t se_get_cropped_length(const char* text, uint8_t text_length, uint32_t width_limit_in_pixels, uint8_t text_format) {
  char        current_char;
  uint8_t     length;
  uint32_t    current_width_in_pixels = 0;

  for (length = 0; length < text_length; length++) {
    current_char = text[length];

    if ((text_format & PAGING_FORMAT_NB) == PAGING_FORMAT_NB) {
      // Bold.
      current_width_in_pixels += nanos_characters_width[current_char - NANOS_FIRST_CHAR] & 0x0F;
    }
    else {
      // Regular.
      current_width_in_pixels += (nanos_characters_width[current_char - NANOS_FIRST_CHAR] >> 0x04) & 0x0F;
    }

    // We stop the processing when we reached the limit.
    if (current_width_in_pixels > width_limit_in_pixels) {
      break;
    }
  }

  return length;
}

// This function is used to retrieve the width of a line of text.
static uint32_t se_compute_line_width_light(const char* text, uint8_t text_length, uint8_t text_format) {
  char current_char;
  uint32_t line_width = 0;

  // We parse the characters of the input text on all the input length.
  while (text_length--) {
    current_char = *text;

    if (current_char < NANOS_FIRST_CHAR || current_char > NANOS_LAST_CHAR) {
      if (current_char == '\n' || current_char == '\r') {
        break;
      }
    }
    else {
      // We retrieve the character width, and the paging format indicates whether we are
      // processing bold characters or not.
      if ((text_format & PAGING_FORMAT_NB) == PAGING_FORMAT_NB) {
        // Bold.
        line_width += nanos_characters_width[current_char - NANOS_FIRST_CHAR] & 0x0F;
      }
      else {
        // Regular.
        line_width += (nanos_characters_width[current_char - NANOS_FIRST_CHAR] >> 0x04) & 0x0F;
      }
    }
    text++;
  }
  return line_width;
}

#endif // !HAVE_SE_SCREEN

static unsigned int is_word_delim(unsigned char c) {
  // return !((c >= 'a' && c <= 'z') 
  //       || (c >= 'A' && c <= 'Z')
  //       || (c >= '0' && c <= '9'));
  return c == ' ' || c == '\n' || c == '\t' || c == '-' || c == '_';
}

// return the number of pages to be displayed when current page to show is -1
unsigned int ux_layout_paging_compute(const char* text_to_split, 
                                      unsigned int page_to_display,
                                      ux_layout_paging_state_t* paging_state,
                                      bagl_font_id_e font
                                      ) {
#ifndef HAVE_FONTS
  UNUSED(font);
#endif

  // reset length and offset of lines
  memset(paging_state->offsets, 0, sizeof(paging_state->offsets));
  memset(paging_state->lengths, 0, sizeof(paging_state->lengths));

  // a page has been asked, but no page exists
  if (page_to_display >= paging_state->count && page_to_display != -1UL) {
    return 0;
  }

  // compute offset/length of text of each line for the current page
  unsigned int page = 0;
  unsigned int line = 0;
  const char* start = (text_to_split ? STRPIC(text_to_split) : G_ux.externalText);
  const char* start2 = start;
  const char* end = start + strlen(start);
  while (start < end) {
    unsigned int len = 0;
    unsigned int linew = 0; 
    const char* last_word_delim = start;
    // not reached end of content
    while (start + len < end
      // line is not full
      && linew <= PIXEL_PER_LINE
      // avoid display buffer overflow for each line
      // && len < sizeof(G_ux.string_buffer)-1
      ) {
      // compute new line length
#ifdef HAVE_FONTS
      linew = bagl_compute_line_width(font, 0, start, len+1, BAGL_ENCODING_LATIN1);
#else // HAVE_FONTS
      linew = se_compute_line_width_light(start, len + 1, G_ux.layout_paging.format);
#endif //HAVE_FONTS
      //if (start[len] )
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

    // if not splitting line onto a word delimiter, then cut at the previous word_delim, adjust len accordingly (and a wor delim has been found already)
    if (start + len < end && last_word_delim != start && len) {
      // if line split within a word
      if ((!is_word_delim(start[len-1]) && !is_word_delim(start[len]))) {
        len = last_word_delim - start;
      }
    }

    // fill up the paging structure
    if (page_to_display != -1UL && page_to_display == page && page_to_display < paging_state->count) {
      paging_state->offsets[line] = start - start2;
      paging_state->lengths[line] = len;

      // won't compute all pages, we reached the one to display
#if UX_LAYOUT_PAGING_LINE_COUNT > 1
      if (line >= UX_LAYOUT_PAGING_LINE_COUNT-1) 
#endif // UX_LAYOUT_PAGING_LINE_COUNT
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
#endif // UX_LAYOUT_PAGING_LINE_COUNT
      start < end) {
      page++;
      line = 0;
    }
  }

  // return total number of page detected
  return page+1;
}

#endif // HAVE_BAGL
#endif // HAVE_UX_FLOW
