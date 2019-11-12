
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#include "bagl.h"
#include <string.h>
#include <stdio.h>

#include "os.h"

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
#endif // !BAGL_WIDTH

#ifndef BAGL_HEIGHT
#error BAGL_HEIGHT not set
#endif // !BAGL_HEIGHT

// --------------------------------------------------------------------------------------
// Definitions
// -------------------------------------------------------------------------------------- 

#define ICON_WIDTH 0
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef U2BE
#define U2BE(buf, off) ((((buf)[off]&0xFF)<<8) | ((buf)[off+1]&0xFF) )
#endif
#ifndef U4BE
#define U4BE(buf, off) ((U2BE(buf, off)<<16) | (U2BE(buf, off+2)&0xFFFF))
#endif

// --------------------------------------------------------------------------------------
// Variables
// -------------------------------------------------------------------------------------- 

#ifdef HAVE_BAGL_GLYPH_ARRAY
const bagl_glyph_array_entry_t* G_glyph_array;
unsigned int                    G_glyph_count; 
#endif // HAVE_BAGL_GLYPH_ARRAY

// --------------------------------------------------------------------------------------
// API
// -------------------------------------------------------------------------------------- 

// --------------------------------------------------------------------------------------
void bagl_draw_bg(unsigned int color) {
  bagl_component_t c;
  memset(&c, 0, sizeof(c));
  c.type = BAGL_RECTANGLE;
  c.userid = BAGL_NONE;
  c.fgcolor = color;
  c.x = 0;
  c.y = 0;
  c.width = BAGL_WIDTH;
  c.height = BAGL_HEIGHT;
  c.fill = BAGL_FILL;
  // draw the rect
  bagl_draw_with_context(&c, NULL, 0, 0);
}

#ifdef HAVE_BAGL_GLYPH_ARRAY
// --------------------------------------------------------------------------------------
// internal helper, get the glyph entry from the glyph id (sparse glyph array support)
const bagl_glyph_array_entry_t* bagl_get_glyph(unsigned int icon_id, const bagl_glyph_array_entry_t* glyph_array, unsigned int glyph_count) {
  unsigned int i=glyph_count; 

  while(i--) {
    // font id match this entry (non linear)
    if (glyph_array[i].icon_id == icon_id) {
      return &glyph_array[i];
    }
  }

  // id not found
  return NULL;
}
#endif // HAVE_BAGL_GLYPH_ARRAY

// --------------------------------------------------------------------------------------
// internal helper, get the font entry from the font id (sparse font array support)
const bagl_font_t* bagl_get_font(unsigned int font_id) {
  unsigned int i=C_bagl_fonts_count; 
  font_id &= BAGL_FONT_ID_MASK;

  while(i--) {
    // font id match this entry (non indexed array)
    if (PIC_FONT(C_bagl_fonts[i])->font_id == font_id) {
      return PIC_FONT(C_bagl_fonts[i]);
    }
  }

  // id not found
  return NULL;
}

// --------------------------------------------------------------------------------------
// return the width of a text (first line only) for alignment processing
unsigned short bagl_compute_line_width(unsigned short font_id, unsigned short width, const void * text, unsigned char text_length, unsigned char text_encoding) {
  unsigned short xx;
  const bagl_font_t *font = bagl_get_font(font_id);
  if (font == NULL) {
    return 0;
  }

  // initialize first index
  xx = 0;

  //printf("display text: %s\n", text);

  // depending on encoding
  while (text_length--) {
    unsigned int ch = 0;
    // TODO support other encoding than ascii ISO8859 Latin
    switch(text_encoding) {
      default:
      case BAGL_ENCODING_LATIN1:
        ch = *((unsigned char*)text);
        text = (void*)(((unsigned char*)text)+1);
        break;
    }

    unsigned char ch_width = 0; 
    if (ch < font->first_char || ch > font->last_char) {
      // only proceed the first line width, not the whole paragraph
      if (ch == '\n' || ch == '\r') {
        return xx;
      }

      // else use the low bits as an extra spacing value
      if (ch >= 0xC0) {
        ch_width = ch&0x3F;
      }
      else if (ch >= 0x80) {
        // open the glyph font
        const bagl_font_t *font_symbols = bagl_get_font((ch&0x20)?BAGL_FONT_SYMBOLS_1:BAGL_FONT_SYMBOLS_0);
        if (font_symbols != NULL) {
          // extract the size of the symbols' font character
          ch_width = PIC_CHAR(font_symbols->characters)[ch & 0x1F].char_width;
        }
      }
    }
    else {
      // compute the index in the bitmap array
      ch -= font->first_char;
      ch_width = PIC_CHAR(font->characters)[ch].char_width;
    }

    // retrieve the char bitmap

    // go to next line if needed, kerning is not used here
    if (width > 0 && xx + ch_width > width) {
      return xx;
    }

    // prepare for next char
    xx += ch_width;
  }
  return xx;
}

// --------------------------------------------------------------------------------------
// draw char until a char fit before reaching width
// TODO support hyphenation ??
int bagl_draw_string(unsigned short font_id, unsigned int fgcolor, unsigned int bgcolor, int x, int y, unsigned int width, unsigned int height, const void* text, unsigned int text_length, unsigned char text_encoding) {
  int xx;
  unsigned int colors[16];
  colors[0] = bgcolor;
  colors[1] = fgcolor;
  unsigned int ch = 0;

  const bagl_font_t *font = bagl_get_font(font_id);
  if (font == NULL) {
    return 0;
  }


#ifdef BAGL_MULTICHROME
  if (font->bpp > 1) {
    // fgcolor = 0x7e7ecc
    // bgcolor = 0xeca529
    // $1 = {0xeca529, 0xc6985f, 0xa28b95, 0x7e7ecc}

    unsigned int color_count = 1<<(font->bpp);
    memset(colors, 0, sizeof(colors));
    colors[0] = bgcolor;
    colors[color_count-1] = fgcolor;

    // compute for all base colors
    int off;
    for (off = 0; off < 3; off++) {

      int cfg = (fgcolor>>(off*8))&0xFF;
      int cbg = (bgcolor>>(off*8))&0xFF;

      int crange = MAX(cfg,cbg)-MIN(cfg,cbg)+1;
      int cinc = crange/(color_count-1UL);

      if (cfg > cbg) {
        unsigned int i;
        for (i=1; i < color_count-1UL; i++) {
          colors[i] |= MIN(0xFF, cbg+i*cinc)<<(off*8);
        }
      }
      else {
        unsigned int i;

        for (i=1; i < color_count-1UL; i++) {
          colors[i] |= MIN(0xFF, cfg+(color_count-1UL-i)*cinc)<<(off*8);
        }
      }
    }
  }
#endif // BAGL_MULTICHROME // for the blue

  // always comparing this way, very optimized etc
  width += x;
  height += y;

  // initialize first index
  xx = x;

  //printf("display text: %s\n", text);


  // depending on encoding
  while (text_length--) {
    // TODO support other encoding than ascii ISO8859 Latin
    switch(text_encoding) {
      default:
      case BAGL_ENCODING_LATIN1:
        // avoid 2 new line on \r and \n for windows familiar users
        if (ch == '\r') {
          ch = *((unsigned char*)text);
          if (ch == '\n') {
            text = (void*)(((unsigned int)text)+1);
            continue;
          }
        }
        else {
          ch = *((unsigned char*)text);
        }
        text = (void*)(((unsigned int)text)+1);
        break;
    }

    unsigned char ch_height = font->char_height;
    unsigned char ch_kerning = 0;
    unsigned char ch_width = 0;
    const unsigned char * ch_bitmap = NULL;
    int ch_y = y;

    if (ch < font->first_char || ch > font->last_char) {
      //printf("invalid char");
      // can't proceed
      if (ch == '\n' || ch == '\r') {
        y += ch_height; // no interleave

        // IGNORED for first line
        if (y + ch_height > (int)height) {
          // we're writing half height of the last line ... probably better to put some dashes
          return (y<<16)|(xx&0xFFFF);
        }

        // newline starts back at first x offset
        xx = x;
        continue;
      }

      if (ch >= 0xC0) {
        ch_width = ch & 0x3F;
      }
      else if (ch >= 0x80) {
        // open the glyph font
        const bagl_font_t *font_symbols = bagl_get_font((ch&0x20)?BAGL_FONT_SYMBOLS_1:BAGL_FONT_SYMBOLS_0);
        if (font_symbols != NULL) {
          ch_bitmap = &PIC_BMP(font_symbols->bitmap)[PIC_CHAR(font_symbols->characters)[ch & 0x1F].bitmap_offset];
          ch_width = PIC_CHAR(font_symbols->characters)[ch & 0x1F].char_width;
          ch_height = font_symbols->char_height;
          // align baselines
          ch_y = y + font->baseline_height - font_symbols->baseline_height;
        }
      }
    }
    else {
      ch -= font->first_char;
      ch_bitmap = &PIC_BMP(font->bitmap)[PIC_CHAR(font->characters)[ch].bitmap_offset];
      ch_width = PIC_CHAR(font->characters)[ch].char_width;
      ch_kerning = font->char_kerning;
    }

    // retrieve the char bitmap

    // go to next line if needed
    if (xx + ch_width > (int)width) {
      y += ch_height; // no interleave

      // IGNORED for first line
      if (y + ch_height > (int)height) {
        // we're writing half height of the last line ... probably better to put some dashes
        return (y<<16)|(xx&0xFFFF);
      }

      // newline starts back at first x offset
      xx = x;
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
      bagl_hal_draw_bitmap_within_rect(xx, ch_y, ch_width, ch_height, (1<<font->bpp), colors, font->bpp, ch_bitmap, font->bpp*ch_width*ch_height); // note, last parameter is computable could be avoided
    }
    else {
      bagl_hal_draw_rect(bgcolor, xx, ch_y, ch_width, ch_height);
    }
    // prepare for next char
    xx += ch_width + ch_kerning;
  }

  // return newest position, for upcoming printf
  return (y<<16)|(xx&0xFFFF);
}

// --------------------------------------------------------------------------------------

// draw round or circle. unaliased.
// if radiusint is !=0 then draw a circle of color outline, and colorint inside
void bagl_draw_circle_helper(unsigned int color, int x_center, int y_center, unsigned int radius, unsigned char octants, unsigned int radiusint, unsigned int colorint) {

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
  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0
  int dradius = radius-radiusint;
  last_x = x;
  unsigned int drawint = (radiusint > 0 && dradius > 0 /*&& xint <= yint*/);

  while( y <= x )
  {
    if (octants & 1) { // 
      if (drawint) {
        bagl_hal_draw_rect(colorint, x_center,   y+y_center, x-(dradius-1), 1);
        bagl_hal_draw_rect(color, x_center+x-(dradius-1), y+y_center, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center,   y+y_center-1, x, 1);
      }
    }
    if (octants & 2) { // 
      if (drawint) {
        if (last_x != x) {
          bagl_hal_draw_rect(colorint, x_center,   x+y_center, y-(dradius-1), 1);
        }
        bagl_hal_draw_rect(color, x_center+y-(dradius-1), x+y_center, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center,   x+y_center-1, y, 1);
      }
    }
    if (octants & 4) { // 
      if (drawint) {
        bagl_hal_draw_rect(colorint, x_center-x, y+y_center, x-(dradius-1), 1);
        bagl_hal_draw_rect(color, x_center-x-(dradius-1), y+y_center, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center-x, y+y_center-1, x, 1);
      }
    }
    if (octants & 8) { // 
      if (drawint) {
        if (last_x != x) {
          bagl_hal_draw_rect(colorint, x_center-y, x+y_center, y-(dradius-1), 1);
        }
        bagl_hal_draw_rect(color, x_center-y-(dradius-1), x+y_center, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center-y, x+y_center-1, y, 1);
      }
    }
    if (octants & 16) { //
      if (drawint) {
        bagl_hal_draw_rect(colorint, x_center,   y_center-y, x-(dradius-1), 1);
        bagl_hal_draw_rect(color, x_center+x-(dradius-1), y_center-y, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center,   y_center-y, x, 1);
      }
    }
    if (octants & 32) { // 
      if (drawint) {
        if (last_x != x) {
          bagl_hal_draw_rect(colorint, x_center,   y_center-x, y-(dradius-1), 1);
        }
        bagl_hal_draw_rect(color, x_center+y-(dradius-1), y_center-x, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center,   y_center-x, y, 1);
      }
    }
    if (octants & 64) { // 
      if (drawint) {
        bagl_hal_draw_rect(colorint, x_center-x, y_center-y, x-(dradius-1), 1);
        bagl_hal_draw_rect(color, x_center-x-(dradius-1), y_center-y, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center-x, y_center-y, x, 1);
      }
    }
    if (octants & 128) { //
      if (drawint) {
        if (last_x != x) {
          bagl_hal_draw_rect(colorint, x_center-y, y_center-x, y-(dradius-1), 1);
        }
        bagl_hal_draw_rect(color, x_center-y-(dradius-1), y_center-x, dradius, 1);
      }
      else {
        bagl_hal_draw_rect(color, x_center-y, y_center-x, y, 1);
      }
    }

    last_x = x;
    y++;
    if (decisionOver2<=0)
    {
      decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
    }
    else
    {
      x--;
      decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
    }
  }
}

// --------------------------------------------------------------------------------------

#ifdef HAVE_BAGL_GLYPH_ARRAY
void bagl_set_glyph_array(const bagl_glyph_array_entry_t* array, unsigned int count) {
  G_glyph_array = array;
  G_glyph_count = count;
}
#endif // HAVE_BAGL_GLYPH_ARRAY

// --------------------------------------------------------------------------------------

void bagl_draw_with_context(const bagl_component_t* component, const void* context, unsigned short context_length, unsigned char context_encoding) {
  //unsigned char comp_idx;
  int halignment=0;
  int valignment=0;
#ifdef HAVE_BAGL_GLYPH_ARRAY
  int x,y;
#endif // HAVE_BAGL_GLYPH_ARRAY
  unsigned int baseline=0;
  unsigned int char_height=0;
  int strwidth = 0;
  unsigned int ellipsis_1_len = 0;
  const char* ellipsis_2_start = NULL;
  unsigned int fgcolor=component->fgcolor; 
  unsigned int bgcolor=component->bgcolor;

#ifdef HAVE_BAGL_GLYPH_ARRAY
  const bagl_glyph_array_entry_t* glyph=NULL;
#endif // HAVE_BAGL_GLYPH_ARRAY

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
  unsigned int type = component->type&~(BAGL_TYPE_FLAGS_MASK);

  // compute alignment if text provided and requiring special alignment
  if (type != BAGL_ICON) {
    const bagl_font_t* font = bagl_get_font(component->font_id);
    if (font) {
      baseline = font->baseline_height;
      char_height = font->char_height;
      if (context && context_length) {
        // compute with some margin to fit other characters and check if ellipsis algorithm is required
        strwidth = bagl_compute_line_width(component->font_id, component->width+100, context, context_length, context_encoding);
        ellipsis_1_len = context_length;

#ifdef HAVE_BAGL_ELLIPSIS
        // ellipsis mode (ensure something is to be splitted!)
        if (strwidth > component->width && context_length>4) {
          unsigned int robin = 0; // remove char by char either on the left or right side
          unsigned int dots_len = bagl_compute_line_width(component->font_id, 100 /*probably larger than ... whatever the font*/, "...", 3, context_encoding);
          ellipsis_1_len = context_length/2;
          ellipsis_2_start = ((char*)context) + context_length/2;
          // split line in 2 halves, strip a char from end of left part, and from start of right part, reassemble with ... , repeat until it fits.
          // NOTE: algorithm is wrong if special blank chars are inserted, they should be removed first
          while (strwidth > component->width && ellipsis_1_len && (context_length - ((unsigned int)ellipsis_2_start-(unsigned int)context) )) {
            unsigned int left_part = bagl_compute_line_width(component->font_id, component->width, context, ellipsis_1_len, context_encoding);
            unsigned int right_part = bagl_compute_line_width(component->font_id, component->width, ellipsis_2_start, (context_length - ((unsigned int)ellipsis_2_start-(unsigned int)context) ), context_encoding);
            // update to check and to compute alignement if needed
            strwidth = left_part + dots_len + right_part;
            // only start to split if the middle char if odd context_length removal is not sufficient
            if (strwidth > component->width) {
              // either remove a left char, OR remove a right char
              switch(robin) {
                case 0:
                  // remove a left char
                  ellipsis_1_len--;
                  break;
                case 1:
                  // remove a right char
                  ellipsis_2_start++;
                  break;
              }
              robin = (robin+1)%2;
            }
          }
          // we've computed split positions
        }
#endif // HAVE_BAGL_ELLIPSIS

        switch (component->font_id & BAGL_FONT_ALIGNMENT_HORIZONTAL_MASK ) {
          default:
          case BAGL_FONT_ALIGNMENT_LEFT:
            halignment = 0;
            break;
          case BAGL_FONT_ALIGNMENT_RIGHT:
            halignment = MAX(0,component->width - strwidth);
            break;
          case BAGL_FONT_ALIGNMENT_CENTER:
            // x   xalign      strwidth width
            // '     '            '     '
            //       ^
            // xalign = x+ (width/2) - (strwidth/2) => align -x
            halignment = MAX(0,component->width/2 - strwidth/2);
            break;
        }

        switch (component->font_id & BAGL_FONT_ALIGNMENT_VERTICAL_MASK ) {
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
            valignment = component->height/2 - baseline/2 - 1;
            break;
        }
      }
    }
  }

  // only check the type only, ignore the touchable flag
  switch(type) {
    /*
        button (B)
        <   |Icon|Space|Textstring|   >
             I.w   W.w     T.w
        I.x = B.x+B.w/2-(I.w+W.w+T.w)/2
        W.x = I.x+I.w
        T.x = W.x+W.w = I.x+I.w+W.w = B.x+B.w/2-(I.w+W.w+T.w)/2+I.w+W.w = B.x+B.w/2-T.w/2+(I.w+W.w)/2
    */
    case BAGL_RECTANGLE:
    case BAGL_BUTTON: // optional icon + textbox + rectangle
    draw_round_rect: {
      unsigned int radius = component->radius;
      

      if (component->fill != BAGL_FILL) {
        // inner
        // centered top to bottom
        bagl_hal_draw_rect(bgcolor, 
                           component->x+radius,                  
                           component->y-(type==BAGL_LABELINE?(baseline):0), 
                           component->width-2*radius, 
                           component->height);
        // left to center rect
        bagl_hal_draw_rect(bgcolor, 
                           component->x,                                    
                           component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                           radius, 
                           component->height-2*radius); 
        // center rect to right
        bagl_hal_draw_rect(bgcolor, 
                           component->x+component->width-radius-1, 
                           component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                           radius, 
                           component->height-2*radius);

        // outline
        // 4 rectangles (with last pixel of each corner not set)
        bagl_hal_draw_rect(fgcolor, 
                           component->x+radius,                  
                           component->y-(type==BAGL_LABELINE?(baseline):0), 
                           component->width-2*radius, 
                           component->stroke); // top
        bagl_hal_draw_rect(fgcolor, 
                           component->x+radius,                  
                           component->y-(type==BAGL_LABELINE?(baseline):0)+component->height-1, 
                           component->width-2*radius, 
                           component->stroke); // bottom
        bagl_hal_draw_rect(fgcolor, 
                           component->x,                                    
                           component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                           component->stroke, 
                           component->height-2*radius); // left
        bagl_hal_draw_rect(fgcolor, 
                           component->x+component->width-1, 
                           component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                           component->stroke, 
                           component->height-2*radius); // right
      }
      else {
        // centered top to bottom
        bagl_hal_draw_rect(fgcolor, 
                           component->x+radius,                  
                           component->y-(type==BAGL_LABELINE?(baseline):0), 
                           component->width-2*radius, 
                           component->height);
        // left to center rect
        bagl_hal_draw_rect(fgcolor, 
                           component->x,                                    
                           component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                           radius, 
                           component->height-2*radius); 

        // center rect to right
        bagl_hal_draw_rect(fgcolor, 
                           component->x+component->width-radius, 
                           component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                           radius, 
                           component->height-2*radius);
      }

      goto rect;
    }

    case BAGL_LABELINE:       
    case BAGL_LABEL: {


      // for strings, colors are inverted.     
      unsigned int radius;
      unsigned int icon_width;
      fgcolor=component->bgcolor; 
      bgcolor=component->fgcolor;
    rect:
      icon_width = 0;
      radius = component->radius;
      //if (radius > component->width/2 ||radius > component->height/2) 
      {
        radius = MIN(radius, MIN(component->width/2, component->height/2));
      }
      // draw the background
      /* shall not be needed
      if (component->fill == BAGL_FILL) {
        bagl_hal_draw_rect(component->bgcolor, 
                           component->x+radius,                  
                           component->y, 
                           component->width-2*radius, 
                           component->stroke); // top
      }
      */
      

      // draw corners
      if (radius > 1) {
        unsigned int radiusint = 0;
        // carve round when not filling
        if (component->fill != BAGL_FILL && component->stroke < radius) {
          radiusint = radius-component->stroke;
        }
        bagl_draw_circle_helper(fgcolor, 
                                component->x+radius, 
                                component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                                radius, 
                                BAGL_FILL_CIRCLE_PI2_PI, 
                                radiusint, 
                                bgcolor);
        bagl_draw_circle_helper(fgcolor, 
                                component->x+component->width-radius-component->stroke, 
                                component->y-(type==BAGL_LABELINE?(baseline):0)+radius, 
                                radius, 
                                BAGL_FILL_CIRCLE_0_PI2, 
                                radiusint, 
                                bgcolor);
        bagl_draw_circle_helper(fgcolor, 
                                component->x+radius, 
                                component->y-(type==BAGL_LABELINE?(baseline):0)+component->height-radius-component->stroke, 
                                radius, 
                                BAGL_FILL_CIRCLE_PI_3PI2, 
                                radiusint, 
                                bgcolor);
        bagl_draw_circle_helper(fgcolor, 
                                component->x+component->width-radius-component->stroke, 
                                component->y-(type==BAGL_LABELINE?(baseline):0)+component->height-radius-component->stroke, 
                                radius, 
                                BAGL_FILL_CIRCLE_3PI2_2PI, 
                                radiusint, 
                                bgcolor);
      }

      // for rectangle/line/... which fallthrough here
      if (type != BAGL_LABELINE && type != BAGL_LABEL) {
        break;
      }

      if (context && context_length) {
        // debug centering
        //bagl_hal_draw_rect(component->fgcolor, component->x, component->y, 2, 2);

        unsigned int pos = bagl_draw_string(component->font_id,
                         component->fgcolor, 
                         component->bgcolor, 
                         component->x + halignment /*+ component->stroke*/, 
                         component->y + (type==BAGL_LABELINE?-(baseline):valignment) /*+ component->stroke*/, 
                         component->width /*- 2*component->stroke*/ - halignment, 
                         component->height /*- 2*component->stroke*/ - (type==BAGL_LABELINE?0/*-char_height*/:valignment),
                         context,
                         ellipsis_1_len,
                         context_encoding);
#ifdef HAVE_BAGL_ELLIPSIS
        if (ellipsis_2_start) {
          // draw ellipsis
          pos = bagl_draw_string(component->font_id,
                                 component->fgcolor, 
                                 component->bgcolor, 
                                 (pos & 0xFFFF) /*+ component->stroke*/, 
                                 component->y + (type==BAGL_LABELINE?-(baseline):valignment) /*+ component->stroke*/, 
                                 component->width /*- 2*component->stroke*/ - halignment, 
                                 component->height /*- 2*component->stroke*/ - (type==BAGL_LABELINE?0/*-char_height*/:valignment),
                                 "...",
                                 3,
                                 context_encoding); 
          // draw the right part
          pos = bagl_draw_string(component->font_id,
                                 component->fgcolor, 
                                 component->bgcolor, 
                                 (pos & 0xFFFF) /*+ component->stroke*/, 
                                 component->y + (type==BAGL_LABELINE?-(baseline):valignment) /*+ component->stroke*/, 
                                 component->width /*- 2*component->stroke*/ - halignment, 
                                 component->height /*- 2*component->stroke*/ - (type==BAGL_LABELINE?0/*-char_height*/:valignment),
                                 ellipsis_2_start,
                                 (context_length - ((unsigned int)ellipsis_2_start-(unsigned int)context) ),
                                 context_encoding);
        }
#endif // HAVE_BAGL_ELLIPSIS
        
        // debug centering
        //bagl_hal_draw_rect(component->fgcolor, component->x+component->width-2, component->y+component->height-2, 2, 2);
      }
      /*
      if (component->stroke > 0) {
        goto outline_rect;
      }
      */
      break;
    }
    case BAGL_LINE: // a line is just a flat rectangle :p
    //case BAGL_RECTANGLE:
      if(/*component->fill == BAGL_FILL &&*/ component->radius == 0) {
        bagl_hal_draw_rect(component->fgcolor, component->x, component->y, component->width, component->height);
      }
      else {
        goto draw_round_rect;
        /*
        // not filled, respect the stroke
        // left
        bagl_hal_draw_rect(component->fgcolor, component->x, component->y, MAX(1,component->stroke), component->height);
        // top
        bagl_hal_draw_rect(component->fgcolor, component->x, component->y, component->width, MAX(1,component->stroke));
        // right
        bagl_hal_draw_rect(component->fgcolor, component->x+component->width-MAX(1,component->stroke), component->y, MAX(1,component->stroke), component->height);
        // bottom
        bagl_hal_draw_rect(component->fgcolor, component->x, component->y+component->height-MAX(1,component->stroke), component->width, MAX(1,component->stroke));
        */
      }
      break;

#ifdef HAVE_BAGL_GLYPH_ARRAY
    case BAGL_ICON: {
      x = component->x;
      y = component->y;

      // icon data follows are in the context
      if (component->icon_id != 0) {

        // select the default or custom glyph array
        if (component->type == BAGL_ICON && context_encoding && G_glyph_array && G_glyph_count > 0) {
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
          if ((1<<glyph->bits_per_pixel)*4 != context_length) {
            // invalid color count
            break;
          }
          context_length /= 4;
        }
        // use default colors
        if (!context_length || !context) {
          context_length = 1<<(glyph->bits_per_pixel);
          context = glyph->default_colors;
        }

        // center glyph in rect
        // draw the glyph from the bitmap using the context for colors
        bagl_hal_draw_bitmap_within_rect(x + (component->width / 2 - glyph->width / 2), 
                                         y + (component->height / 2 - glyph->height / 2), 
                                         glyph->width, 
                                         glyph->height, 
                                         context_length, 
                                         (unsigned int*)context, // Endianness remarkably ignored !
                                         glyph->bits_per_pixel, 
                                         glyph->bitmap, 
                                         glyph->bits_per_pixel*(glyph->width*glyph->height));
      }
      else {
        // context: <bitperpixel> [color_count*4 bytes (LE encoding)] <icon bitmap (raw scan, LE)>

        unsigned int colors[4];
        unsigned int bpp = ((unsigned char*)context)[0];
        // no space to display that
        if (bpp > 2) {
          return;
        }
        unsigned int i=1<<bpp;
        while(i--) {
          colors[i] = U4BE((unsigned char*)context, 1+i*4);
        }
       
        // draw the glyph from the bitmap using the context for colors
        bagl_hal_draw_bitmap_within_rect(x, 
                                         y, 
                                         component->width, 
                                         component->height, 
                                         1<<bpp,
                                         colors,
                                         bpp, 
                                         ((unsigned char*)context)+1+(1<<bpp)*4, 
                                         bpp*(component->width*component->height));
      }

      break;
    }
#endif // HAVE_BAGL_GLYPH_ARRAY
    
    case BAGL_CIRCLE:
      // draw the circle (all 8 octants)
      bagl_draw_circle_helper(component->fgcolor, component->x+component->radius, component->y+component->radius, component->radius, 0xFF, ((component->fill != BAGL_FILL && component->stroke < component->radius)?component->radius-component->stroke:0), component->bgcolor);
      break;

    //case BAGL_NONE:
      // performing, but not registering
      //bagl_hal_draw_rect(component->fgcolor, component->x, component->y, component->width, component->height);
      //return;
    case BAGL_NONE:
    default:
      return;
  }

  /*
  // remember drawn component for user action
  memcpy(&bagl_components[comp_idx], component, sizeof(bagl_component_t));  
  */
}

// --------------------------------------------------------------------------------------
void bagl_draw_glyph(const bagl_component_t* component, const bagl_icon_details_t* icon_details) {
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
  bagl_hal_draw_bitmap_within_rect(component->x, 
                                   component->y, 
                                   icon_details->width, 
                                   icon_details->height, 
                                   1<<(icon_details->bpp),
                                   (unsigned int*)PIC((unsigned int)icon_details->colors),
                                   icon_details->bpp, 
                                   (unsigned char*)PIC((unsigned int)icon_details->bitmap), 
                                   icon_details->bpp*(icon_details->width*icon_details->height));
}

    
// --------------------------------------------------------------------------------------

void bagl_animate(bagl_animated_t* anim, unsigned int timestamp_ms, unsigned int interval_ms) {
  // nothing to be animated right now (or no horizontal scrolling speed defined)
  if ((anim->next_ms != 0 && anim->next_ms > timestamp_ms) || anim->c.width == 0 || anim->c.icon_id == 0 || (anim->current_x & 0xF0000000)==0x40000000) {
    return;
  }

  // when starting the animation, perform a pause on the left of the string
  if (anim->next_ms == 0) {
    anim->next_ms = timestamp_ms + (anim->c.stroke&0x7F)*100;
    anim->current_x = 0x0;
    anim->current_char_idx = 0;
  }

  unsigned int a,b;
  unsigned int valignment=0;
  unsigned int baseline=0;
  //unsigned int char_height=0;
  unsigned int totalwidth = 0;
  unsigned int remwidth = 0;
  unsigned int charwidth=0;
  unsigned int type = anim->c.type&~(BAGL_TYPE_FLAGS_MASK);

  // compute alignment if text provided and requiring special alignment
  if (anim->text && anim->text_length && (type == BAGL_LABELINE || type == BAGL_LABEL)) {
    const bagl_font_t* font = bagl_get_font(anim->c.font_id);
    // invalid font, nothing to animate :(
    if (font == NULL) {
      return;
    }
    unsigned int maxcharwidth = bagl_compute_line_width(anim->c.font_id, 0, "W", 1, anim->text_encoding)+1;
    baseline = font->baseline_height;
    //char_height = font->char_height;
    
    totalwidth = bagl_compute_line_width(anim->c.font_id, 0, anim->text, anim->text_length, anim->text_encoding);

    // nothing to be animated here, text is already fully drawn in its text box
    if (totalwidth <= anim->c.width) {
      return;
    }
    if (anim->current_char_idx > anim->text_length) {
      anim->current_char_idx = 0;
    }

    remwidth = bagl_compute_line_width(anim->c.font_id, 0, anim->text+anim->current_char_idx, anim->text_length-anim->current_char_idx, anim->text_encoding);
    charwidth = bagl_compute_line_width(anim->c.font_id, 0, anim->text+anim->current_char_idx, 1, anim->text_encoding);

    switch (anim->c.font_id & BAGL_FONT_ALIGNMENT_VERTICAL_MASK ) {
      default:
      case BAGL_FONT_ALIGNMENT_TOP:
        valignment = 0;
        break;
      case BAGL_FONT_ALIGNMENT_BOTTOM:
        valignment = anim->c.height - baseline;
        break;
      case BAGL_FONT_ALIGNMENT_MIDDLE:
        // y                 yalign           charheight        height
        // '                    '          v  '                 '
        //                           baseline
        // yalign = y+ (height/2) - (baseline/2) => align - y
        valignment = anim->c.height/2 - baseline/2 - 1;
        break;
    }

    // consider the current char of the string has been displayed on the screen (or at least a part of it)
    // viewport         |    < width >   |
    // totalwidth  | a text that does not fit the viewport |
    // rem width        |xt that does not fit the viewport |           s
    // 
    unsigned int current_char_displayed_width = (anim->current_x & ~(0xF0000000)) - (totalwidth - remwidth);

    // draw a bg rectangle on the area before painting the animated value, to clearup glitches on both sides

    a = anim->c.y + (type==BAGL_LABELINE?-(baseline):valignment);
    b = anim->c.height- (type==BAGL_LABELINE?0/*-char_height*/:valignment);
    bagl_draw_string(anim->c.font_id,
	         anim->c.fgcolor, 
	         anim->c.bgcolor, 
	         anim->c.x - current_char_displayed_width, 
	         a, 
	         anim->c.width + current_char_displayed_width + charwidth /*- 2*component->stroke*/, 
	         b,
	         anim->text+anim->current_char_idx, anim->text_length-anim->current_char_idx, anim->text_encoding);

    // crop the viewport
    bagl_hal_draw_rect(anim->c.bgcolor,
                       anim->c.x-maxcharwidth,
                       a,
                       maxcharwidth,
                       b);
    bagl_hal_draw_rect(anim->c.bgcolor,
                       anim->c.x+anim->c.width,
                       a,
                       maxcharwidth,
                       b);

  // report on screen
    screen_update();
    unsigned int step_ms=interval_ms;
    unsigned int step_x=anim->c.icon_id * step_ms/1000;
    while(step_x == 0) {
      step_ms += interval_ms;
      step_x = anim->c.icon_id * step_ms / 1000;
    }

    switch (anim->current_x & 0xF0000000) {
      // left to right
      case 0:
        anim->next_ms += step_ms;
        if (current_char_displayed_width >= charwidth) {
          anim->current_char_idx++;
          // if text fits, then stop scrolling and wait a bit
          if (remwidth - current_char_displayed_width <= anim->c.width) {
            anim->current_x = (totalwidth-remwidth+current_char_displayed_width) | 0x10000000;
            break;
          }
        }
        anim->current_x += step_x;
        break;
        
      // pause after finished left to right
      case 0x10000000:
        anim->next_ms += (anim->c.stroke&0x7F)*100;
        anim->current_x = (totalwidth-remwidth+current_char_displayed_width) | 0x20000000;
        break;

      // right to left
      case 0x20000000:
        anim->next_ms += step_ms;
        if (current_char_displayed_width >= charwidth) {
          // we're displaying from the start
          if (remwidth >= totalwidth) {
            anim->current_x = 0x30000000;
            anim->current_char_idx = 0;
            break;
          }
          anim->current_char_idx--;
        }
        anim->current_x = ((anim->current_x & ~(0xF0000000)) - step_x) | 0x20000000;
        break;
        
      // pause after finished right to left
      case 0x30000000:
        anim->next_ms += (anim->c.stroke&0x7F)*100;
        anim->current_x = 0;
        // not going for another scroll anim
        if (anim->c.stroke & 0x80) {
          anim->current_x = 0x40000000;
        }
        break;
      case 0x40000000:
        // stalled, nothing to do
        break;
    }
  }
}

// --------------------------------------------------------------------------------------

void bagl_draw(const bagl_component_t* component) {
  // component without text
  bagl_draw_with_context(component, NULL, 0, 0);
}

#endif // HAVE_BAGL
