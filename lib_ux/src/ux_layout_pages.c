
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
#include "os_helpers.h"
#include "os_math.h"
#include "os_pic.h"
#include "os_print.h"
#include "os_utils.h"
#include "ux.h"
#include <string.h>
#include "os.h"

#ifdef HAVE_UX_FLOW
#if defined(HAVE_INDEXED_STRINGS)

#include "ux_layout_common.h"

#ifdef HAVE_BAGL

#if (defined HAVE_BOLOS || defined(BUILD_PNG))

// clang-format off
static const bagl_element_t ux_layout_pages_elements[] = {
#if (BAGL_WIDTH==128 && BAGL_HEIGHT==64)
// Default Layout for LNX/LNS+:
// Coordinates for Layout:
// N  => X=6 Y=35
// P  => X=57 Y=26
// NN => X=6 Y=27,43
// PN => X=57,6 Y=17,44
// NP => X=6,57 Y=26,33
// P/N => X=41 Y=35
// P/NN => X=41 Y=27,43
// NNN => X=6 Y=19,35,51
// PNN => X=57,6,6 Y=10,36,52
// NNP => X=6,6,57 Y=19,34,41
// NNNN => X=6 Y=14,28,42,56
// PNNN => X=57,6,6,6 Y=5,29,43,56
// NNNP => X=6,6,57 Y=14,27,41,45

  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  64, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, .text=NULL},
  // Left & Right arrows, when applicable:
  {{BAGL_ICON                           , 0x01,   2,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_left},
  {{BAGL_ICON                           , 0x02, 122,  28,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_right},
  // 14x14 Icon on the left side, vertically centered
  {{BAGL_ICON                           , 0x0F,  16,   24,  16,  16, 0, 0, 0       , 0xFFFFFF, 0x000000, 0, 0  }, .text=NULL},
  // Up to 4 lines of text (can be mixed with one centered 14x14 Icon):
  {{BAGL_LABELINE                       , 0x10,   6,  15, 116,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL},
  {{BAGL_LABELINE                       , 0x11,   6,  29, 116,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL},
  {{BAGL_LABELINE                       , 0x12,   6,  43, 116,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL},
  {{BAGL_LABELINE                       , 0x13,   6,  57, 116,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL},

#elif (BAGL_WIDTH==128 && BAGL_HEIGHT==32)
// Default Layout for LNS:
// Coordinates for Layout:
// N  => X=6 Y=19
// P  => X=57 Y=9
// NN => X=6 Y=12,26
// PN => X=57,6 Y=2,27
// NP => X=6,57 Y=11,16
// P/N => X=41 Y=19
// P/NN => X=41 Y=12,26

  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, .text=NULL},
  // Left & Right arrows, when applicable:
  {{BAGL_ICON                           , 0x01,   2,  12,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_left},
  {{BAGL_ICON                           , 0x02, 122,  12,   4,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=(const char*)&C_icon_right},
  // 14x14 Icon on the left side, vertically centered
  {{BAGL_ICON                           , 0x0F,  14,   8,  16,  16, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, .text=NULL},
  // Up to 2 lines of text (can be mixed with one centered 14x14 Icon):
  {{BAGL_LABELINE                       , 0x10,   6,  12, 116,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL},
  {{BAGL_LABELINE                       , 0x11,   6,  26, 116,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, .text=NULL},
#endif // (BAGL_WIDTH==128 && BAGL_HEIGHT==64)
};
// clang-format on

// X Coordinate & width of text when there is an icon centered on the side:
#define BAGL_ICON_TEXT_XCOORD 38
#define BAGL_ICON_TEXT_WIDTH  (BAGL_WIDTH - BAGL_ICON_TEXT_XCOORD)

// =============================================================================
// This function will parse text and build arrays of txt ptr & lengths for each
// line, taking in account specific contents like string buffer, side icons etc.
// It returns a ptr to the side icon if there is one on the page (on any line).
const bagl_icon_details_t *update_lines_length(const char    *text,
                                               const char    *lines[],
                                               unsigned short lengths[])
{
    const bagl_icon_details_t *icon_details = NULL;

    // Override text parameter by a string buffer addr, eventually:
    // (this is necessary when a string buffer is displayed over several pages)
    const char *string_buffer = get_string_buffer(G_ux.layout_paging.string_buffer_id);

    // Parse all lines on this page to see if there is something special to do:
    for (int i = 0; i < UX_LAYOUT_PAGES_LINE_COUNT; i++) {
        const char    *txt;
        unsigned short length = G_ux.layout_paging.lengths[i];

        // When using UX_LOC_PAGING macro, first line contain the title
        if (!i && G_ux.layout_paging.paging_title) {
            const char *title = get_ux_loc_string(G_ux.layout_paging.paging_title - 1);
            if (title) {
                SPRINTF(G_ux.string_buffer,
                        (G_ux.layout_paging.count > 1) ? "%s (%d/%d)" : "%s",
                        title,
                        G_ux.layout_paging.current + 1,
                        G_ux.layout_paging.count);
            }
            else {
                SPRINTF(G_ux.string_buffer,
                        "%d/%d",
                        G_ux.layout_paging.current + 1,
                        G_ux.layout_paging.count);
            }
            // Store text pointer & length for this line:
            lines[0]   = G_ux.string_buffer;
            lengths[0] = strlen(G_ux.string_buffer);
            continue;
        }

        // Is there anything to display on this line?
        if (length == 0) {
            lines[i]   = NULL;
            lengths[i] = 0;
            continue;
        }
        if (string_buffer) {
            txt = string_buffer;
        }
        else if (text) {
            txt = STRPIC(text);
        }
        else {
            txt = G_ux.externalText;
        }
        // Take in account offset which is relative to txt ptr:
        txt += G_ux.layout_paging.offsets[i];

        // Check if this line starts with an escape sequence:
        while (txt && txt[0] == '\e') {
            // There is at least one more byte which is readable.
            // (even if it is the end of the string, it contains '\0')
            unsigned short extra_byte = (unsigned char) txt[1];
            // Is it an icon?
            if (extra_byte >= FIRST_GLYPHS_ICON && extra_byte <= LAST_GLYPHS_ICON) {
                // Does this line contain a 'left side icon'?
                // If there is only an icon, then it is displayed on a full line.
                // If there are text after the icon, it is displayed on left side.
                // (=> This is what userid 0x0F is about!!)
                if (length > 2) {
                    // This is a side icon => update icon_details ptr:
                    icon_details = get_glyphs_icon(extra_byte);
                    // Skip the side icon related bytes => update txt ptr & length:
                    txt += 2;
                    length -= 2;
                    // Skip all spaces characters after the side icon related bytes:
                    while (*txt == ' ' && length) {
                        ++txt;
                        --length;
                    }
                    continue;  // Check if there is something else to handle...
                }
                else {
                    // There is just an icon displayed on a full line.
                    break;
                }
            }
            else {
                if (extra_byte >= FIRST_STRING_BUFFER && extra_byte <= LAST_STRING_BUFFER) {
                    // WARNING: if the string buffer contain an escape sequence to a
                    // string buffer, then  we'll have an infinite loop! => Avoid that...
                    if (string_buffer) {
                        // Don't recursively parse string buffers!
                        txt    = NULL;
                        length = 0;
                        break;
                    }

                    // There is a string buffer, here => update txt to point on it:
                    txt = get_string_buffer(extra_byte);
                    // We have just switched from txt to string buffer => offset=0.
                    // (G_ux.layout_paging.offsets[i] is relative to original txt ptr)
                    // (G_ux.layout_paging.lengths[i] is related to string buffer)
                    // (G_ux.layout_paging.offsets[>i] are relative to string buffer)
                    // Use that string buffer for next lines on this page:
                    string_buffer = txt;
                    // Check for an escape sequence at the beginning of the string buffer:
                    continue;
                }
                else {  // This is not an icon nor a screen buffer...ignore it!
                    txt    = NULL;
                    length = 0;
                    break;
                }
            }
        }
        // Store text pointer & length for this line:
        lines[i]   = txt;
        lengths[i] = length;
    }

    // Return a pointer to the side icon, if there is one on this page:
    return icon_details;
}

// =============================================================================

const bagl_element_t *ux_layout_pages_prepro_common(const bagl_element_t *element, const char *text)
{
    const char    *lines[UX_LAYOUT_PAGES_LINE_COUNT];
    unsigned short lengths[UX_LAYOUT_PAGES_LINE_COUNT];

    // copy element before any mod
    memmove(&G_ux.tmp_element, element, sizeof(bagl_element_t));

    switch (element->component.userid) {
        case 0x01:
            // no step before AND no pages before
            if (ux_flow_is_first() && G_ux.layout_paging.current == 0) {
                return NULL;
            }
            break;

        case 0x02:
            if (ux_flow_is_last() && G_ux.layout_paging.current == G_ux.layout_paging.count - 1) {
                return NULL;
            }
            break;

        // Do we have to display 14x14 Icon on the left side, vertically centered?
        case 0x0F:
        case 0x1F: {
            // Check all lines on this page to see if there is an icon on the side:
            const bagl_icon_details_t *icon_side_details
                = update_lines_length(text, lines, lengths);
            if (icon_side_details != NULL) {
                G_ux.tmp_element.text = (const void *) icon_side_details;
                return &G_ux.tmp_element;
            }
            // There is no 14x14 icon to display on the side.
            return NULL;
        }
        // Up to 4 lines of text (can be mixed with one centered 14x14 Icon):
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23: {
            unsigned short lineidx = element->component.userid & 0xF;
            // Check if there is something to display on this line:
            if (lineidx >= UX_LAYOUT_PAGES_LINE_COUNT || G_ux.layout_paging.lengths[lineidx] == 0) {
                return NULL;
            }
            // Check all lines on this page and update lines & lengths information:
            // (if icon_side_details != NULL => there is an icon on left side)
            const bagl_icon_details_t *icon_side_details
                = update_lines_length(text, lines, lengths);
            G_ux.tmp_element.component.font_id = G_ux.layout_paging.fond_ids[lineidx];
            // When the icon is displayed on left side, text is left aligned:
            if (icon_side_details != NULL) {
                G_ux.tmp_element.component.font_id |= BAGL_FONT_ALIGNMENT_LEFT;
            }
            else {
                G_ux.tmp_element.component.font_id |= BAGL_FONT_ALIGNMENT_CENTER;
            }
            // Let's eat a little bit more vertical space on LNX:
#if (BAGL_HEIGHT == 64)
            short y_margin = BAGL_HEIGHT - FONT_HEIGHT;  // Start at 52
#endif                                                   //(BAGL_HEIGHT==64)
            unsigned short nb_lines    = 0;              // Number of lines displayed in this screen
            short          y_coord     = 0;              // Y coordinate for this element
            short          x_coord     = G_ux.tmp_element.component.x;
            unsigned short used_height = 0;  // Number of vertical pixels used

            // Find at which X,Y coordinates this line have to be displayed:
            // (we need to scan all lines...)
            for (int i = 0; i < UX_LAYOUT_PAGES_LINE_COUNT; i++) {
                unsigned short length;
                const char    *txt;

                // Skip that line if it doesn't contain anything:
                if ((length = lengths[i]) == 0 || (txt = lines[i]) == NULL) {
                    continue;
                }
                // By default consider this line will contain text => use font height
                unsigned short height = FONT_HEIGHT;

                // If there is an icon displayed on the left side, adjust xcoord and
                // available width of ALL lines:
                if (icon_side_details != NULL) {
                    // The icon will be displayed when element->component.userid == 0x0F.
                    x_coord = BAGL_ICON_TEXT_XCOORD;  // That's the X coord for PNN on LNS
                    G_ux.tmp_element.component.width = BAGL_ICON_TEXT_WIDTH;
                }
                // Does this line contain an icon?
                // If yes it is the first characters in the string.
                // If there is only the icon, then it is displayed on a full line.
                // If there are text after the icon, it is displayed on left side.
                // (This MUST HAVE BEEN handled before)
                if (txt[0] == '\e') {
                    unsigned short extra_byte = (unsigned char) txt[1];
                    // Is it really an icon?
                    if (extra_byte >= FIRST_GLYPHS_ICON && extra_byte <= LAST_GLYPHS_ICON) {
                        // Be sure this icon have to be displayed here:
                        if (length == 2) {
                            // That line contain an icon => update height with correct value:
                            const bagl_icon_details_t *icon_details = get_glyphs_icon(extra_byte);
                            height                                  = icon_details->height;
                            // If the icon is on current line, display it instead of text:
                            if (lineidx == i) {
                                G_ux.tmp_element.component.type    = BAGL_ICON;
                                G_ux.tmp_element.component.icon_id = 0;
                                G_ux.tmp_element.text              = (const char *) icon_details;
                                // Update x_coord, width & height using icon's width:
                                x_coord
                                    += (G_ux.tmp_element.component.width - icon_details->width) / 2;
                                G_ux.tmp_element.component.height = icon_details->height;
                                G_ux.tmp_element.component.width  = icon_details->width;
                            }
                        }
                    }
                }
                else if (lineidx == i) {
                    // Update G_ux.tmp_element.text value for the line we want to display:
                    SPRINTF(
                        G_ux.layout_paging.line_buffer,
                        "%.*s",
                        // avoid overflow
                        (unsigned int) (MIN(sizeof(G_ux.layout_paging.line_buffer) - 1, length)),
                        txt);
                    G_ux.tmp_element.text = G_ux.layout_paging.line_buffer;
                    // Don't forget to add Baseline for characters:
                    y_coord += FONT_BASELINE;
                }
                if (i < lineidx) {
                    y_coord += height;
                }
                used_height += height;
                ++nb_lines;
#if (BAGL_HEIGHT == 64)
                // Vertical margin depending on the number of lines displayed:
                y_margin /= 2;  // y_margin values will be 26,13,6,3
#endif                          //(BAGL_HEIGHT==64)
            }
            // Y will depend on the number of lines displayed.
#if (BAGL_HEIGHT == 64)
            y_coord += y_margin;
            used_height += 2 * y_margin;
#endif  //(BAGL_HEIGHT==64)
        // Vertical padding (vertical space between each element) is:
        // vertical_padding = (BAGL_HEIGHT - used_height) / (nb_lines + 1);
        // So we can compute Y coordinate which will be:
            y_coord += (((lineidx + 1) * (BAGL_HEIGHT - used_height)) + (nb_lines + 1) / 2)
                       / (nb_lines + 1);
            // NB: previous computations provides almost same values than original
            // ones, but we can replace them with manually fixed values if necessary.
            G_ux.tmp_element.component.x = x_coord;
            G_ux.tmp_element.component.y = y_coord;
            break;
        }
    }
    return &G_ux.tmp_element;
}

// =============================================================================

static bool is_loc_word_delim(unsigned char c)
{
    // return !((c >= 'a' && c <= 'z')
    //       || (c >= 'A' && c <= 'Z')
    //       || (c >= '0' && c <= '9'));
    return c == ' ' || c == '\n' || c == '\b' || c == '\f' || c == '\e' || c == '-' || c == '_';
}

// return the number of pages to be displayed when current page to show is -1
unsigned int ux_layout_pages_compute(const char               *text_to_split,
                                     unsigned int              page_to_display,
                                     ux_layout_paging_state_t *paging_state)
{
    bagl_font_id_e font_id;

    // reset length and offset of lines for this page
    memset(paging_state->offsets, 0, sizeof(paging_state->offsets));
    memset(paging_state->lengths, 0, sizeof(paging_state->lengths));

    // a page has been asked, but no page exists
    if (page_to_display >= paging_state->count && page_to_display != (unsigned int) -1) {
        return 0;
    }

#ifndef HAVE_FONTS
    unsigned char paging_format;
#endif  // HAVE_FONTS
    // Regular font by default at the beginning of the first page:
    unsigned char use_bold_font;
    if (G_ux.layout_paging.format & PAGING_FORMAT_NB) {
        use_bold_font = 1;
    }
    else {
        use_bold_font = 0;
    }
    // compute offset/length of text of each line for the current page
    unsigned int  page   = 0;
    unsigned int  line   = 0;
    const char   *start  = (text_to_split ? STRPIC(text_to_split) : G_ux.externalText);
    const char   *start2 = start;
    const char   *end    = start + strlen(start);
    unsigned int  string_buffer_mark_offset = (unsigned int) -1;
    unsigned int  string_buffer_mark_len    = 0;
    unsigned char string_buffer_id          = 0;  // By default, 0 => no string buffer
    unsigned char icon_on_side              = 0;
    unsigned char extra_byte;
    const char   *page_start               = start;
    unsigned char page_use_bold_font       = use_bold_font;
    unsigned int  string_buffer_start_page = (unsigned int) -1;
    paging_state->string_buffer_id         = string_buffer_id;
    while (start < end) {
        unsigned int  len, icon_on_side_len;
        unsigned int  linew;
        unsigned char c;
    page_start_loop:
        // When using UX_LOC_PAGING macro, first line contain the title on all pages
        if (!line && G_ux.layout_paging.paging_title) {
            paging_state->fond_ids[0] = BAGL_FONT_OPEN_SANS_REGULAR_11px;  // By default
            paging_state->lengths[0]  = 1;  // If 0 the line will be skipped
            line                      = 1;
        }
        len = icon_on_side_len = 0;
        linew                  = 0;
        c                      = 0;

        // Skip all spaces characters at the beginning of the line:
        while (*start == ' ' && (start + 1) < end) {
            ++start;
        }
        // If first character is \b, update bold state and remove it from the string
        if (*start == '\b') {
            use_bold_font ^= 1;
            ++start;
        }
        const char *last_word_delim = start;
        // Initialise font_id at the beginning of each line depending on bold state:
        if (use_bold_font) {
            font_id = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px;
#ifndef HAVE_FONTS
            paging_format = PAGING_FORMAT_NB;
#endif  // HAVE_FONTS
        }
        else {
            font_id = BAGL_FONT_OPEN_SANS_REGULAR_11px;
#ifndef HAVE_FONTS
            paging_format = PAGING_FORMAT_NN;
#endif  // HAVE_FONTS
        }
        // not reached end of content
        while (start + len < end
               // line is not full
               && (icon_on_side == 0 || linew <= BAGL_ICON_TEXT_WIDTH)
               && linew <= PIXEL_PER_LINE
               // avoid display buffer overflow for each line
               && len < (sizeof(G_ux.layout_paging.line_buffer) - 1)) {
            // ========================
            // Handle escape sequences:
            // =========================
            // Check if this line starts with the escape character \e:
            if (start[icon_on_side_len] == '\e') {
                extra_byte = start[icon_on_side_len + 1];
                // ----------------------------
                // Special case: string buffer:
                // ----------------------------
                // If this is a string buffer, then restart parsing the string buffer:
                if (extra_byte >= FIRST_STRING_BUFFER && extra_byte <= LAST_STRING_BUFFER) {
                    // Don't allow recursive string buffer!
                    if (string_buffer_id || !get_string_buffer(extra_byte)) {
                        // Ignore this escape sequence
                        if (!icon_on_side_len) {
                            start += 2;
                        }
                    }
                    else {
                        // Store the string buffer mark offset/len from original string:
                        string_buffer_mark_offset = start - start2;
                        string_buffer_mark_len    = icon_on_side_len;
                        string_buffer_start_page  = page;
                        // If the string just contain \b + the string buffer, switch now!
                        if (string_buffer_mark_offset == 1 && start2[0] == '\b') {
                            string_buffer_mark_offset = 0;
                        }
                        string_buffer_id = extra_byte;
                        // Get String buffer addr:
                        start  = get_string_buffer(extra_byte);
                        start2 = start;
                        end    = start + strlen(start);
                    }
                    // Just continue parsing starting from the same line, but with new text buffer:
                    goto page_start_loop;
                }
                // Currently, escape character use just one additional byte.
                // If next character is '\n' or '\f' or '\0' we'll remove it and store 2 bytes:
                // (this mean we was dealing with a full line icon)
                // -----------------------------
                // Special case: full line icon:
                // -----------------------------
                if (start[icon_on_side_len + 2] == '\n' || start[icon_on_side_len + 2] == '\f'
                    || start[icon_on_side_len + 2] == '\0') {
                    c   = start[icon_on_side_len + 2];
                    len = icon_on_side_len + 3;
                    break;
                }
                // -----------------------------
                // Special case: left side icon:
                // -----------------------------
                // Otherwise, we continue adding following characters to that string.
                // Check if line != 0 to restart scanning at line 0 with icon_on_side set!
                if (!icon_on_side && line) {
                    line         = 0;
                    icon_on_side = 1;
                    start        = page_start;
                    end          = start + strlen(start);
                    // If we switched to a string buffer on this page, reset start2:
                    if (string_buffer_start_page != (unsigned int) -1
                        && string_buffer_start_page == page) {
                        start2 = (text_to_split ? STRPIC(text_to_split) : G_ux.externalText);
                        string_buffer_start_page       = (unsigned int) -1;
                        paging_state->string_buffer_id = string_buffer_id = 0;
                        string_buffer_mark_offset                         = (unsigned int) -1;
                        string_buffer_mark_len                            = 0;
                    }
                    use_bold_font = page_use_bold_font;
                    // reset length and offset of lines for this page
                    if (page_to_display == page) {
                        memset(paging_state->offsets, 0, sizeof(paging_state->offsets));
                        memset(paging_state->lengths, 0, sizeof(paging_state->lengths));
                    }
                    goto page_start_loop;
                }
                icon_on_side = 1;
                len += 2;
                // Skip all spaces characters after the icon on side bytes:
                while (start[len] == ' ') {
                    ++len;
                }
                // Be able to check \e after some icon on the side bytes!
                icon_on_side_len = len;
                // ---------------------------------------------------------
                // Special case: string buffer right after a left side icon:
                // ---------------------------------------------------------
                continue;
                // =================================
                // End of escape sequences handling:
                // =================================
            }
            // compute new line length
#ifdef HAVE_FONTS
            linew = bagl_compute_line_width(font_id, 0, start, len + 1, BAGL_ENCODING_DEFAULT);
#else   // HAVE_FONTS
            linew = se_compute_line_width_light(start, len + 1, paging_format);
#endif  // HAVE_FONTS
        // if (start[len] )
            if ((icon_on_side != 0 && linew > BAGL_ICON_TEXT_WIDTH) || linew > PIXEL_PER_LINE) {
                // we got a full line
                break;
            }
            c = start[len];
            if (is_loc_word_delim(c)) {
                last_word_delim = &start[len];
            }
            len++;
            // New line, don't go further
            if (c == '\n' || c == '\f') {
                break;
            }
        }

        // if not splitting line onto a word delimiter, then cut at the previous word_delim, adjust
        // len accordingly (and a word delim has been found already)
        if (start + len < end && last_word_delim != start && len) {
            // if line split within a word
            if ((!is_loc_word_delim(start[len - 1]) && !is_loc_word_delim(start[len]))) {
                len = last_word_delim - start;
            }
        }

        // Update boldness: parse text and toggle boldness if needed:
        for (unsigned int i = 0; i < len; i++) {
            if (start[i] == '\b') {
                use_bold_font ^= 1;
            }
        }
        // fill up the paging structure
        if (page_to_display != (unsigned int) -1 && page_to_display == page
            && page_to_display < paging_state->count) {
            // If we just switched to a string buffer, keep the offset from original string:
            if (string_buffer_mark_offset != (unsigned int) -1) {
                paging_state->offsets[line] = string_buffer_mark_offset;
            }
            else {
                paging_state->offsets[line] = start - start2;
            }
            // Store string buffer id if we are not in the initial page or if there
            // is only the string buffer to display in that page
            if (string_buffer_start_page != (unsigned int) -1
                && (string_buffer_start_page < page
                    || (string_buffer_start_page == page && string_buffer_mark_offset == 0))) {
                paging_state->string_buffer_id = string_buffer_id;  // Contain 0xB1, 0xB2, etc
            }
            // Remove the '\n' or '\f' from the string (=> no need to handle it anymore!)
            if (c == '\n' || c == '\f' || c == '\0') {
                paging_state->lengths[line] = len - 1 + string_buffer_mark_len;
            }
            else {
                paging_state->lengths[line] = len + string_buffer_mark_len;
            }
            string_buffer_mark_offset = (unsigned int) -1;
            string_buffer_mark_len    = 0;

            paging_state->fond_ids[line] = font_id;

            // won't compute all pages, we reached the one to display
#if UX_LAYOUT_PAGES_LINE_COUNT > 1
            if (line >= UX_LAYOUT_PAGES_LINE_COUNT - 1)
#endif  // UX_LAYOUT_PAGES_LINE_COUNT
            {
                return 1;
            }
        }

        // prepare for next line
        start += len;

        // skip to next line/page
        line++;

        // Do we want to jump to next page?
        if (c == '\f'
            || (
#if UX_LAYOUT_PAGES_LINE_COUNT > 1
                line >= UX_LAYOUT_PAGES_LINE_COUNT &&
#endif  // UX_LAYOUT_PAGES_LINE_COUNT
                start < end)) {
            page++;
            line                      = 0;
            icon_on_side              = 0;
            page_start                = start;
            string_buffer_mark_offset = (unsigned int) -1;
            string_buffer_mark_len    = 0;
            page_use_bold_font        = use_bold_font;
        }
    }

    // return total number of pages detected
    return page + 1;
}

// =============================================================================

void ux_layout_pages_display_init(const char *text)
{
    // Compute all UX_LOC_PAGE stuff into G_ux.layout_paging
    G_ux.layout_paging.current      = 0;
    G_ux.layout_paging.count        = 1;
    G_ux.layout_paging.paging_title = 0;
    G_ux.layout_paging.format       = PAGING_FORMAT_NN;
    // request offsets and lengths of lines for the current page
    ux_layout_pages_compute(text, G_ux.layout_paging.current, &G_ux.layout_paging);
}

const bagl_element_t *ux_layout_pages_display_element(const bagl_element_t *element,
                                                      const char           *text)
{
    return ux_layout_pages_prepro_common(element, text);
}

// =============================================================================

// redisplay current page
void ux_layout_pages_redisplay_common(unsigned int            stack_slot,
                                      const char             *text,
                                      button_push_callback_t  button_callback,
                                      bagl_element_callback_t prepro)
{
    ux_stack_slot_t *slot = &G_ux.stack[stack_slot];

    slot->element_arrays[0].element_array       = ux_layout_pages_elements;
    slot->element_arrays[0].element_array_count = ARRAYLEN(ux_layout_pages_elements);
    slot->element_arrays_count                  = 1;

    // request offsets and lengths of lines for the current page
    ux_layout_pages_compute(text, G_ux.layout_paging.current, &G_ux.layout_paging);

    slot->screen_before_element_display_callback = prepro;
    slot->button_push_callback                   = button_callback;
    ux_stack_display(stack_slot);
}

static const bagl_element_t *ux_layout_pages_prepro_by_addr(const bagl_element_t *element)
{
    // don't display if null
    const void *params = ux_stack_get_current_step_params();
    if (NULL == params) {
        return NULL;
    }
    const char *text;

#if defined(HAVE_INDEXED_STRINGS)
    text = get_ux_loc_string(((const ux_loc_layout_params_t *) params)->index);
#else   // defined(HAVE_INDEXED_STRINGS)
    text = ((const ux_layout_pages_params_t *) params)->text;
#endif  // defined(HAVE_INDEXED_STRINGS)
    return ux_layout_pages_prepro_common(element, text);
}

unsigned int ux_layout_pages_button_callback_by_addr(unsigned int button_mask,
                                                     unsigned int button_mask_counter);
unsigned int ux_loc_layout_paging_button_callback_by_addr(unsigned int button_mask,
                                                          unsigned int button_mask_counter);

void ux_layout_pages_redisplay_by_addr(unsigned int stack_slot)
{
    const char *text;
    const void *params = ux_stack_get_current_step_params();
    if (NULL == params) {
        return;
    }
#if defined(HAVE_INDEXED_STRINGS)
    text = get_ux_loc_string(((const ux_loc_layout_params_t *) params)->index);
#else   // defined(HAVE_INDEXED_STRINGS)
    text = ((const ux_layout_pages_params_t *) params)->text;
#endif  // defined(HAVE_INDEXED_STRINGS)
    ux_layout_pages_redisplay_common(
        stack_slot, text, ux_layout_pages_button_callback_by_addr, ux_layout_pages_prepro_by_addr);
}

static const bagl_element_t *ux_loc_layout_paging_prepro_by_addr(const bagl_element_t *element)
{
    // don't display if null
    const void *params = ux_stack_get_current_step_params();
    if (NULL == params) {
        return NULL;
    }
    const char *text;

    // As 0 mean it's not a UX_LOC_PAGING (due to all the memset layout_paging)
    // Store index value + 1
    G_ux.layout_paging.paging_title = ((const ux_loc_layout_params_t *) params)->index + 1;
    text                            = get_ux_loc_string(G_ux.layout_paging.paging_title - 1 + 1);
    return ux_layout_pages_prepro_common(element, text);
}

void ux_loc_layout_paging_redisplay_by_addr(unsigned int stack_slot)
{
    const char *text;
    const void *params = ux_stack_get_current_step_params();
    if (NULL == params) {
        return;
    }
    // As 0 mean it's not a UX_LOC_PAGING (due to all the memset layout_paging)
    // Store index value + 1
    G_ux.layout_paging.paging_title = ((const ux_loc_layout_params_t *) params)->index + 1;
    text                            = get_ux_loc_string(G_ux.layout_paging.paging_title - 1 + 1);
    ux_layout_pages_redisplay_common(stack_slot,
                                     text,
                                     ux_loc_layout_paging_button_callback_by_addr,
                                     ux_loc_layout_paging_prepro_by_addr);
}

// Function located in ux_layout_paging.c
unsigned int ux_layout_paging_button_callback_common(unsigned int button_mask,
                                                     unsigned int button_mask_counter,
                                                     ux_layout_paging_redisplay_t redisplay);

unsigned int ux_loc_layout_paging_button_callback_by_addr(unsigned int button_mask,
                                                          unsigned int button_mask_counter)
{
    return ux_layout_paging_button_callback_common(
        button_mask, button_mask_counter, ux_loc_layout_paging_redisplay_by_addr);
}

unsigned int ux_layout_pages_button_callback_by_addr(unsigned int button_mask,
                                                     unsigned int button_mask_counter)
{
    return ux_layout_paging_button_callback_common(
        button_mask, button_mask_counter, ux_layout_pages_redisplay_by_addr);
}

void ux_layout_pages_init(unsigned int stack_slot)
{
    const char *text;
    const void *params = ux_stack_get_step_params(stack_slot);

    G_ux.layout_paging.format       = PAGING_FORMAT_NN;  // By default
    G_ux.layout_paging.paging_title = 0;                 // 0 mean it's not a UX_LOC_PAGING
#if defined(HAVE_INDEXED_STRINGS)
    text = get_ux_loc_string(((const ux_loc_layout_params_t *) params)->index);
#else   // defined(HAVE_INDEXED_STRINGS)
    text = ((const ux_layout_pages_params_t *) params)->text;
#endif  // defined(HAVE_INDEXED_STRINGS)
    ux_layout_pages_init_common(stack_slot, text, ux_layout_pages_redisplay_by_addr);
}

void ux_loc_layout_pages_init(unsigned int stack_slot)
{
    ux_layout_pages_init(stack_slot);
}

void ux_loc_layout_paging_init(unsigned int stack_slot)
{
    const char *text;
    const void *params = ux_stack_get_step_params(stack_slot);

    G_ux.layout_paging.format = PAGING_FORMAT_NN;  // By default
    // As 0 mean it's not a UX_LOC_PAGING (due to all the memset layout_paging)
    // Store index value + 1
    G_ux.layout_paging.paging_title = ((const ux_loc_layout_params_t *) params)->index + 1;
    text                            = get_ux_loc_string(G_ux.layout_paging.paging_title - 1 + 1);
    ux_layout_pages_init_common(stack_slot, text, ux_loc_layout_paging_redisplay_by_addr);
}

#endif  // (defined HAVE_BOLOS || defined(BUILD_PNG))

void ux_layout_pages_init_common(unsigned int                 stack_slot,
                                 const char                  *text,
                                 ux_layout_paging_redisplay_t redisplay)
{
    // At this very moment, we don't want to get rid of the format nor paging_title, so
    // keep the one which has just been set (in case of direction backward or forward).
    unsigned int backup_format = G_ux.layout_paging.format;
    unsigned int backup_title  = G_ux.layout_paging.paging_title;

    // depending flow browsing direction, select the correct page to display
    switch (ux_flow_direction()) {
        case FLOW_DIRECTION_BACKWARD:
            ux_layout_paging_reset();
            // ask the paging to start at the last page.
            // This step must be performed after the 'ux_layout_paging_reset' call,
            // thus we cannot mutualize the call with the one in the 'forward' case.
            G_ux.layout_paging.current = (unsigned int) -1;
            break;
        case FLOW_DIRECTION_FORWARD:
            // open the first page
            ux_layout_paging_reset();
            break;
        case FLOW_DIRECTION_START:
            // shall already be at the first page
            break;
    }

    G_ux.layout_paging.format       = backup_format;
    G_ux.layout_paging.paging_title = backup_title;

    // store params
    ux_stack_init(stack_slot);

    // compute number of chars to display from the params complete string
    if ((text == NULL) && (G_ux.externalText == NULL)) {
        text = "";  // empty string to avoid disrupting the ux flow.
    }

    // count total number of pages
    G_ux.layout_paging.count = ux_layout_pages_compute(
        text, (unsigned int) -1, &G_ux.layout_paging);  // at least one page

    // PRINTF("%d pages needed for =>%s<=\n", G_ux.layout_paging.count, text);
    if (G_ux.layout_paging.count == 0) {
        ux_layout_paging_reset();
    }

    // if (start != end) {
    //   ux_layout_paging_reset();
    // }

    // perform displaying the last page as requested (-1UL in prevstep hook does this)
    if (G_ux.layout_paging.count && G_ux.layout_paging.current > (G_ux.layout_paging.count - 1)) {
        G_ux.layout_paging.current = G_ux.layout_paging.count - 1;
    }

    redisplay(stack_slot);
}

#endif  // HAVE_BAGL
#endif  // defined(HAVE_INDEXED_STRINGS)
#endif  // HAVE_UX_FLOW
