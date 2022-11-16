/**
 * @file nbgl_obj.c
 * @brief Implementation of API to draw all basic graphic objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_obj.h"
#include "nbgl_draw.h"
#include "nbgl_front.h"
#include "nbgl_debug.h"
#include "nbgl_serialize.h"
#include "os_print.h"
#include "glyphs.h"
#ifdef HAVE_SERIALIZED_NBGL
#include "os_io_seproxyhal.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define NB_MAX_PAGES_WITH_DASHES 10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void extendRefreshArea(nbgl_obj_t *obj);

/**********************
 *  STATIC VARIABLES
 **********************/
// area to resfresh
static nbgl_area_t refreshArea;

/**********************
 *      VARIABLES
 **********************/
// buffer used either for image file uncompression and side screen string drawing
uint8_t ramBuffer[GZLIB_UNCOMPRESSED_CHUNK];

/**********************
 *  STATIC PROTOTYPES
 **********************/
extern const char *get_ux_loc_string(uint32_t index);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void compute_relativePosition(nbgl_obj_t* obj, nbgl_obj_t *prevObj) {
  nbgl_container_t *parent = (nbgl_container_t*)(obj->parent);
  // LOG_DEBUG(OBJ_LOGGER,"compute_relativePosition()\n");
  // compute object absolute position thanks to layout/alignment
  if (obj->alignment == NO_ALIGNMENT) {
    LOG_DEBUG(OBJ_LOGGER,"compute_relativePosition() without align to, parent->layout = %d, prevObj = %p\n",parent->layout,prevObj);
    // align to parent, depending of layout
    if (parent->layout == VERTICAL) {
      obj->rel_x0 = obj->alignmentMarginX;
      if (prevObj!=NULL) {
        obj->rel_y0 = prevObj->rel_y0 + prevObj->height + obj->alignmentMarginY;
      }
      else {
        obj->rel_y0 = obj->alignmentMarginY;
      }
    }
    else { // HORIZONTAL
      if (prevObj!=NULL) {
        obj->rel_x0 = prevObj->rel_x0 + prevObj->width + obj->alignmentMarginX;
      }
      else {
        obj->rel_x0 = obj->alignmentMarginX;
      }
      obj->rel_y0 = obj->alignmentMarginY;
    }
  }
  else {
    nbgl_obj_t *alignToObj = obj->alignTo;
    if (alignToObj == NULL) {
      alignToObj = obj->parent;
      LOG_DEBUG(OBJ_LOGGER,"compute_relativePosition() with align to parent\n");
    }
    else {
      LOG_DEBUG(OBJ_LOGGER,"compute_relativePosition() with align to = %p\n",alignToObj);
    }
    // align to the given object, with the given constraints
    if (alignToObj == obj->parent) {
      // align inside if the reference is the parent
      switch (obj->alignment) {
        case TOP_LEFT:
          obj->rel_x0 = obj->alignmentMarginX;
          obj->rel_y0 = obj->alignmentMarginY;
          break;
        case TOP_MIDDLE:
          obj->rel_x0 = (parent->width - obj->width)/2 + obj->alignmentMarginX;
          obj->rel_y0 = obj->alignmentMarginY;
          break;
        case TOP_RIGHT:
          obj->rel_x0 = (parent->width - obj->width) - obj->alignmentMarginX;
          obj->rel_y0 = obj->alignmentMarginY;
          break;
        case MID_LEFT:
          obj->rel_x0 = obj->alignmentMarginX;
          obj->rel_y0 = (parent->height - obj->height)/2 + obj->alignmentMarginY;
          break;
        case CENTER:
          obj->rel_x0 = (parent->width - obj->width)/2 + obj->alignmentMarginX;
          obj->rel_y0 = (parent->height - obj->height)/2 + obj->alignmentMarginY;
          break;
        case MID_RIGHT:
          obj->rel_x0 = (parent->width - obj->width) - obj->alignmentMarginX;
          obj->rel_y0 = (parent->height - obj->height)/2 + obj->alignmentMarginY;
          break;
        case BOTTOM_LEFT:
          obj->rel_x0 = obj->alignmentMarginX;
          obj->rel_y0 = (parent->height - obj->height) - obj->alignmentMarginY;
          break;
        case BOTTOM_MIDDLE:
          obj->rel_x0 = (parent->width - obj->width)/2 + obj->alignmentMarginX;
          obj->rel_y0 = (parent->height - obj->height) - obj->alignmentMarginY;
          break;
        case BOTTOM_RIGHT:
          obj->rel_x0 = (parent->width - obj->width) - obj->alignmentMarginX;
          obj->rel_y0 = (parent->height - obj->height) - obj->alignmentMarginY;
          break;
        default:
          //not supported
          break;
      }
    }
    else {
      // align outside if the reference is a "brother"
      switch (obj->alignment) {
        case TOP_LEFT:
          obj->rel_x0 = alignToObj->rel_x0 + obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 - obj->height - obj->alignmentMarginY;
          break;
        case TOP_MIDDLE:
          obj->rel_x0 = alignToObj->rel_x0 + (alignToObj->width - obj->width)/2 + obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 - obj->height - obj->alignmentMarginY;
          break;
        case TOP_RIGHT:
          obj->rel_x0 = alignToObj->rel_x0 + (alignToObj->width - obj->width) - obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 - obj->height - obj->alignmentMarginY;
          break;

        case LEFT_TOP:
          obj->rel_x0 = alignToObj->rel_x0 - obj->width - obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + obj->alignmentMarginY;
          break;
        case MID_LEFT:
          obj->rel_x0 = alignToObj->rel_x0 - obj->width - obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + (alignToObj->height - obj->height)/2 + obj->alignmentMarginY;
          break;
        case LEFT_BOTTOM:
          obj->rel_x0 = alignToObj->rel_x0 - obj->width - obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + (alignToObj->height - obj->height) + obj->alignmentMarginY;
          break;

        case RIGHT_TOP:
          obj->rel_x0 = alignToObj->rel_x0 + alignToObj->width + obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + obj->alignmentMarginY;
          break;
        case MID_RIGHT:
          obj->rel_x0 = alignToObj->rel_x0 + alignToObj->width + obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + (alignToObj->height - obj->height)/2 + obj->alignmentMarginY;
          break;
        case RIGHT_BOTTOM:
          obj->rel_x0 = alignToObj->rel_x0 + alignToObj->width + obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + (alignToObj->height - obj->height) + obj->alignmentMarginY;
          break;

        case BOTTOM_LEFT:
          obj->rel_x0 = alignToObj->rel_x0 + obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + alignToObj->height + obj->alignmentMarginY;
          break;
        case BOTTOM_MIDDLE:
          obj->rel_x0 = alignToObj->rel_x0 + (alignToObj->width - obj->width)/2 + obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + alignToObj->height + obj->alignmentMarginY;
          break;
        case BOTTOM_RIGHT:
          obj->rel_x0 = alignToObj->rel_x0 + (alignToObj->width - obj->width) - obj->alignmentMarginX;
          obj->rel_y0 = alignToObj->rel_y0 + alignToObj->height + obj->alignmentMarginY;
          break;
        default:
          //not supported
          break;
      }
    }
  }
  // align on multiples of 4
  obj->rel_y0 &= ~0x3;
}

static void compute_position(nbgl_obj_t* obj, nbgl_obj_t *prevObj) {
  nbgl_container_t *parent = (nbgl_container_t*)(obj->parent);

  compute_relativePosition(obj,prevObj);

  if (parent == NULL) {
    // HUGE issue
    return;
  }
  //LOG_DEBUG(OBJ_LOGGER,"compute_position(), parent.type = %d, parent->x0 = %d, obj->rel_x0=%d\n",parent->type,parent->x0,obj->rel_x0);
  //LOG_DEBUG(OBJ_LOGGER,"compute_position(), parent->y0 = %d, obj->rel_y0=%d\n",parent->y0,obj->rel_y0);

  obj->x0 = parent->x0+obj->rel_x0;
  obj->y0 = parent->y0+obj->rel_y0;

  if ((obj->x0+obj->width)>SCREEN_WIDTH) {
    LOG_FATAL(OBJ_LOGGER,"compute_position(), forbidden width, obj->type = %d, x0=%d, width=%d\n",obj->type,obj->x0,obj->width);
  }
  if ((obj->y0+obj->height)>SCREEN_HEIGHT) {
    LOG_FATAL(OBJ_LOGGER,"compute_position(), forbidden height, obj->type = %d, y0=%d, height=%d\n",obj->type,obj->y0,obj->height);
  }
}

static void draw_screen(nbgl_container_t *obj) {
  nbgl_area_t rectArea;

  rectArea.backgroundColor = obj->backgroundColor;
  rectArea.x0 = obj->x0;
  rectArea.y0 = obj->y0;
  rectArea.width = obj->width;
  rectArea.height = obj->height;
  nbgl_frontDrawRect(&rectArea);
}

static void draw_container(nbgl_container_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_container(), x0 = %d, y0 = %d, width = %d\n", obj->x0, obj->y0, obj->width);
  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;
  if (obj->forceClean) {
    nbgl_frontDrawRect((nbgl_area_t*)obj);
  }
}

static void draw_panel(nbgl_panel_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  // panel inherits from container
  draw_container((nbgl_container_t*)obj, prevObj, computePosition);
  LOG_DEBUG(OBJ_LOGGER,"draw_panel(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);
  nbgl_drawRoundedBorderedRect((nbgl_area_t*)obj,
                            RADIUS_8_PIXELS, 2,
                            WHITE, obj->borderColor);
}

/**
 * @brief internal function used to draw a button
 * @note The button contains an icon AND/OR a single-line UTF-8 text
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if true, force to compute the object position
 */
static void draw_button(nbgl_button_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  uint16_t textWidth = 0;
  char *text = NULL;

  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_button(), x0 = %d, y0 = %d, width = %d, height = %d\n", obj->x0, obj->y0, obj->width, obj->height);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;
  // draw the rounded corner rectangle
  if (obj->innerColor == obj->borderColor) {
    nbgl_drawRoundedRect((nbgl_area_t*)obj,
                                obj->radius,
                                obj->innerColor);
  }
  else {
    nbgl_drawRoundedBorderedRect((nbgl_area_t*)obj,
                                obj->radius, 2,
                                obj->innerColor, obj->borderColor);
  }
  // get the text of the button from the callback if not NULL
  if (obj->onDrawCallback != NULL) {
    obj->text = obj->onDrawCallback(obj->token);
  }
  else {
    if (obj->localized == true) {
#if defined(HAVE_LANGUAGE_PACK)
      obj->text = (char*)get_ux_loc_string(obj->textId);
#endif // HAVE_LANGUAGE_PACK
    }
  }
  text = obj->text;
  // draw the text (right of the icon, with 8 pixels between them)
  if (text != NULL) {
    nbgl_area_t rectArea;
    textWidth = nbgl_getTextWidth(obj->fontId, text);
    if (obj->icon != NULL) {
      rectArea.x0 = obj->x0 + obj->width/2  - (textWidth+obj->icon->width+8)/2 + obj->icon->width + 8;
    }
    else {
      rectArea.x0 = obj->x0 + (obj->width - textWidth)/2;
    }
    LOG_DEBUG(OBJ_LOGGER,"draw_button(), text = %s\n", text);
    rectArea.y0 = obj->y0 + (obj->height - nbgl_getFontHeight(obj->fontId))/2;
    rectArea.width = textWidth;
    rectArea.height = nbgl_getFontHeight(obj->fontId);
    rectArea.backgroundColor = obj->innerColor;
    nbgl_drawText(&rectArea, text, nbgl_getTextLength(text), obj->fontId, obj->foregroundColor);
  }
  // draw the icon, if any
  if (obj->icon != NULL) {
    uint16_t iconX0, iconY0;
    nbgl_area_t rectArea;

    if (text != NULL) {
      iconX0 = obj->x0 + (obj->width - (textWidth+obj->icon->width+8))/2;
    }
    else {
      iconX0 = obj->x0 + (obj->width- obj->icon->width)/2;
    }
    LOG_DEBUG(OBJ_LOGGER,"draw_button(), obj->height = %d, obj->iconHeight = %d\n", obj->height , obj->icon->height);
    iconY0 = obj->y0 + (obj->height - obj->icon->height)/2;

    rectArea.backgroundColor = obj->innerColor;
    rectArea.x0 = iconX0;
    rectArea.y0 = iconY0;
    rectArea.width = obj->icon->width;
    rectArea.height = obj->icon->height;
    rectArea.bpp = NBGL_BPP_1;
    nbgl_frontDrawImage(&rectArea,(uint8_t*)obj->icon->bitmap,NO_TRANSFORMATION, obj->foregroundColor);
  }
}

/**
 * @brief internal function used to draw a vertical or horizontal line, up to 4 pixels thick (@ref LINE type)
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if true, force to compute the object position
 */
static void draw_line(nbgl_line_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  nbgl_area_t rectArea;
  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_line(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);
  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;
  LOG_DEBUG(OBJ_LOGGER,"draw_line(), backgroundColor = %d, lineColor = %d\n", obj->backgroundColor,obj->lineColor);
  rectArea.x0 = obj->x0;
  rectArea.y0 = obj->y0;
  if (obj->direction == VERTICAL) {
    rectArea.width = obj->width = obj->thickness;
    rectArea.backgroundColor = obj->lineColor;
    rectArea.height = obj->height;
    nbgl_frontDrawRect(&rectArea);
  }
  else {
    uint8_t mask;
    if (obj->thickness == 1) {
      mask = 0x1 << (obj->offset&0x3);
    }
    else if (obj->thickness == 2) {
      mask = 0x3 << ((obj->offset<3)?obj->offset:2);
    }
    else if (obj->thickness == 3) {
      mask = 0x7 << (obj->offset&0x1);
    }
    else if (obj->thickness == 4) {
      mask = 0xF;
    }
    else {
      LOG_WARN(OBJ_LOGGER,"draw_line(), forbidden thickness = %d\n", obj->thickness);
      return;
    }
    rectArea.width = obj->width;
    rectArea.height = obj->height = 4;
    rectArea.backgroundColor = obj->backgroundColor;
    nbgl_frontDrawHorizontalLine(&rectArea,mask,obj->lineColor);
  }
}

static void draw_image(nbgl_image_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  const nbgl_icon_details_t *iconDetails;

  // if buffer is NULL, let's try to call onDrawCallback, if not NULL, to get it
  if (obj->buffer == NULL) {
    if (obj->onDrawCallback != NULL) {
      iconDetails = obj->onDrawCallback(obj->token);
    }
    else {
      return;
    }
  }
  else {
    iconDetails = obj->buffer;
  }

  // use dimension and bpp from the icon details
  obj->width = iconDetails->width;
  obj->height = iconDetails->height;
  obj->bpp = iconDetails->bpp;
  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_image(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);
  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;
  if (obj->bpp == NBGL_BPP_1) {
    nbgl_frontDrawImage((nbgl_area_t*)obj, (uint8_t*)iconDetails->bitmap,NO_TRANSFORMATION, obj->foregroundColor);
  }
  else if (obj->bpp == NBGL_BPP_2) {
    nbgl_frontDrawImage((nbgl_area_t*)obj, (uint8_t*)iconDetails->bitmap,NO_TRANSFORMATION, ((WHITE<<6)|(LIGHT_GRAY<<4)|(DARK_GRAY<<2)|BLACK));
  }
  else {
    nbgl_frontDrawImage((nbgl_area_t*)obj, (uint8_t*)iconDetails->bitmap,NO_TRANSFORMATION, INVALID_COLOR_MAP);
  }
}

static void draw_switch(nbgl_switch_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  nbgl_area_t rectArea;

  // force dimensions
  obj->width = C_switch_60_40.width;
  obj->height = C_switch_60_40.height;
  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_switch(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

  rectArea.x0 = obj->x0;
  rectArea.y0 = obj->y0;
  rectArea.width = obj->width;
  rectArea.height = obj->height;
  rectArea.backgroundColor = obj->backgroundColor;
  rectArea.bpp = NBGL_BPP_1;
  if (obj->state == OFF_STATE) {
    nbgl_frontDrawImage(&rectArea,(uint8_t*)C_switch_60_40.bitmap,NO_TRANSFORMATION,obj->offColor);
  }
  else {
    nbgl_frontDrawImage(&rectArea,
                          (uint8_t*)C_switch_60_40.bitmap,VERTICAL_MIRROR,obj->onColor);
  }
}

static void draw_radioButton(nbgl_radio_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  nbgl_area_t rectArea;

  // force dimensions
  obj->width = 32;
  obj->height = 32;
  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_radioButton(), x0 = %d, y0 = %d, state = %d\n", obj->x0, obj->y0, obj->state);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

  rectArea.x0 = obj->x0;
  rectArea.y0 = obj->y0;
  rectArea.width = obj->width;
  rectArea.height = obj->height;
  rectArea.backgroundColor = obj->backgroundColor;
  rectArea.bpp = NBGL_BPP_1;
  if (obj->state == OFF_STATE) {
    nbgl_frontDrawImage(&rectArea,(uint8_t*)C_radio_inactive_32px.bitmap,NO_TRANSFORMATION,LIGHT_GRAY);
  }
  else {
    nbgl_frontDrawImage(&rectArea,(uint8_t*)C_radio_active_32px.bitmap,NO_TRANSFORMATION,BLACK);
  }
}

/**
 * @brief internal function used to draw a progress bar object (@ref PROGRESS_BAR type)
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_progressBar(nbgl_progress_bar_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  uint8_t stroke = 2; // 2 pixels for border
  uint16_t levelWidth;

  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_progressBar(), x0 = %d, y0 = %d, level = %d %%\n", obj->x0, obj->y0, obj->state);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

  // draw external part if necessary
  if (obj->withBorder) {
    nbgl_drawRoundedBorderedRect((nbgl_area_t*)obj,
                                RADIUS_0_PIXELS, stroke, obj->backgroundColor, obj->foregroundColor);
  } else {
    nbgl_drawRoundedRect((nbgl_area_t*)obj,
                              RADIUS_0_PIXELS, obj->backgroundColor);
  }
  // draw level
  levelWidth = MIN(obj->width * obj->state / 100, obj->width);
  if (levelWidth > 0) {
    uint16_t tmp_width = obj->width;
    obj->width = levelWidth;
    nbgl_drawRoundedRect((nbgl_area_t*)obj,
                               RADIUS_0_PIXELS, obj->foregroundColor);
    obj->width = tmp_width;
  }
}

/**
 * @brief internal function used to draw a navigation indicator object (@ref PAGE_INDICATOR type)
 * @note It is represented as a dashed line with as many dashes as pages
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_pageIndicator(nbgl_page_indicator_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  nbgl_area_t rectArea;
  uint16_t dashWidth;

  if (obj->nbPages <= NB_MAX_PAGES_WITH_DASHES) {
    uint8_t i;
#define INTER_DASHES 4 // pixels
    // force height
    obj->height = 4;

    if (computePosition) {
      compute_position((nbgl_obj_t *)obj,prevObj);
    }
    LOG_DEBUG(OBJ_LOGGER,"draw_pageIndicator(), x0 = %d, y0 = %d, page = %d/%d\n", obj->x0, obj->y0, obj->activePage, obj->nbPages);

    // inherit background from parent
    obj->backgroundColor = obj->parent->backgroundColor;

    dashWidth = (obj->width - ((obj->nbPages-1) * INTER_DASHES))/obj->nbPages;
    rectArea.x0 = obj->x0;
    rectArea.y0 = obj->y0;
    rectArea.width = dashWidth;
    rectArea.height = obj->height;
    rectArea.backgroundColor = obj->backgroundColor;
    rectArea.bpp = NBGL_BPP_1;
    // draw dashes
    for (i=0;i<=obj->activePage;i++) {
      nbgl_frontDrawHorizontalLine(&rectArea, 0xF, BLACK);
      rectArea.x0 += dashWidth+INTER_DASHES;
    }
    for (;i<obj->nbPages;i++) {
      nbgl_frontDrawHorizontalLine(&rectArea, 0xF, LIGHT_GRAY);
      rectArea.x0 += dashWidth+INTER_DASHES;
    }
  }
  else {
    char navText[11]; // worst case is "ccc of nnn"

    SPRINTF(navText,"%d of %d", obj->activePage+1, obj->nbPages);
    // force height
    obj->height = nbgl_getFontHeight(BAGL_FONT_INTER_REGULAR_24px);
    // the width must be at least 80
    obj->width = nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px,navText);

    if (computePosition) {
      compute_position((nbgl_obj_t *)obj,prevObj);
    }
    LOG_DEBUG(OBJ_LOGGER,"draw_navigationBar(), x0 = %d, y0 = %d, page = %d/%d\n", obj->x0, obj->y0, obj->activePage, obj->nbPages);

    // inherit background from parent
    obj->backgroundColor = obj->parent->backgroundColor;

    // draw active page
    rectArea.x0 = obj->x0;
    rectArea.y0 = obj->y0;
    rectArea.width = obj->width;
    rectArea.height = obj->height;
    rectArea.backgroundColor = obj->backgroundColor;
    rectArea.bpp = NBGL_BPP_1;
    nbgl_drawText(&rectArea, navText, 9, BAGL_FONT_INTER_REGULAR_24px, DARK_GRAY);
  }
}

/**
 * @brief internal function used to draw a text area
 * @note the text itself is placed in the given area depending of textAlignment member of obj
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_textArea(nbgl_text_area_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  nbgl_area_t rectArea;
  uint16_t textWidth,fontHeight,lineHeight, textHeight;
  uint8_t line,nbLines;
  const char *text;

  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  // get the text of the button from the callback if not NULL
  if (obj->onDrawCallback != NULL) {
    obj->text = obj->onDrawCallback(obj->token);
  }
  else {
    if (obj->localized == true) {
#if defined(HAVE_LANGUAGE_PACK)
      obj->text = (char*)get_ux_loc_string(obj->textId);
#endif // HAVE_LANGUAGE_PACK
    }
  }
  text = obj->text;
  if (text == NULL) {
    return;
  }

  LOG_DEBUG(OBJ_LOGGER,"draw_textArea(), wrapping = %d, x0 = %d, y0 = %d, width = %d, height = %d, text = %s\n", obj->wrapping, obj->x0, obj->y0, obj->width, obj->height, text);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

  // draw background to make sure it's clean
  rectArea.x0 = obj->x0;
  rectArea.y0 = obj->y0;
  rectArea.width = obj->width;
  rectArea.height = obj->height;
  rectArea.backgroundColor = obj->backgroundColor;
  nbgl_frontDrawRect(&rectArea);
  // draw border with given style
  if (obj->style == LEDGER_BORDER) {
    // draw horizontal segments (4 pixels stroke)
    rectArea.x0 = obj->x0;
    rectArea.y0 = obj->y0;
    rectArea.width = 24;
    rectArea.height = 4;
    nbgl_frontDrawHorizontalLine(&rectArea, 0xF, obj->textColor); // top left
    rectArea.x0 = obj->x0+obj->width-rectArea.width;
    nbgl_frontDrawHorizontalLine(&rectArea,0xF, obj->textColor); // top right
    rectArea.y0 = obj->y0+obj->height-4;
    nbgl_frontDrawHorizontalLine(&rectArea,0xF, obj->textColor); //bottom right
    rectArea.x0 = obj->x0;
    nbgl_frontDrawHorizontalLine(&rectArea,0xF, obj->textColor); // bottom left
    // draw vertical segments (4 pixels stroke)
    rectArea.x0 = obj->x0;
    rectArea.y0 = obj->y0+4;
    rectArea.width = 4;
    rectArea.height = 12;
    rectArea.backgroundColor = obj->textColor;
    nbgl_frontDrawRect(&rectArea); // top left
    rectArea.x0 = obj->x0+obj->width-rectArea.width;
    nbgl_frontDrawRect(&rectArea); // top right
    rectArea.y0 = obj->y0+obj->height-rectArea.height-4;
    nbgl_frontDrawRect(&rectArea); //bottom right
    rectArea.x0 = obj->x0;
    nbgl_frontDrawRect(&rectArea); // bottom left
  }

  fontHeight = nbgl_getFontHeight(obj->fontId);
  lineHeight = nbgl_getFontLineHeight(obj->fontId);
  // special case of autoHideLongLine, when the text is too long for a line, draw '...' at the beginning
  if (obj->autoHideLongLine == true) {
    textWidth = nbgl_getSingleLineTextWidth(obj->fontId,text);
    if (textWidth > obj->width) {
      uint16_t lineWidth,lineLen;
      uint16_t dotsWidth;

      // at first draw "..." at beginning
      dotsWidth = nbgl_getTextWidth(obj->fontId,"...");
      rectArea.x0 = obj->x0;
      rectArea.y0 = obj->y0 + (obj->height - fontHeight)/2;
      rectArea.width = dotsWidth;
      nbgl_drawText(&rectArea, "...", 3, obj->fontId, obj->textColor);
      // then draw the end of text
      nbgl_getTextMaxLenAndWidthFromEnd(obj->fontId,text,obj->width-dotsWidth,&lineLen,&lineWidth);
      rectArea.x0 += dotsWidth;
      rectArea.width = lineWidth;
      nbgl_drawText(&rectArea, &text[nbgl_getTextLength(text)-lineLen], lineLen, obj->fontId, obj->textColor);
      return;
    }
  }

  // get nb lines in the given width (depending of wrapping)
  nbLines = nbgl_getTextNbLinesInWidth(obj->fontId,text,obj->width,obj->wrapping);
  // saturate nb lines if nbMaxLines is greater than 0
  if ((obj->nbMaxLines > 0) && (obj->nbMaxLines < nbLines)) {
    nbLines = obj->nbMaxLines;
  }

  textHeight = (nbLines-1)*lineHeight+fontHeight;

  rectArea.backgroundColor = obj->backgroundColor;
  rectArea.height = fontHeight;
  // draw each line
  for (line=0;line<nbLines;line++) {
    uint16_t lineWidth,lineLen;

    nbgl_getTextMaxLenAndWidth(obj->fontId,text,obj->width,&lineLen,&lineWidth,obj->wrapping);
    if (obj->textAlignment == MID_LEFT) {
      rectArea.x0 = obj->x0;
    }
    else if (obj->textAlignment == CENTER) {
      rectArea.x0 = obj->x0 + (obj->width - lineWidth)/2;
    }
    else if (obj->textAlignment == MID_RIGHT) {
      rectArea.x0 = obj->x0 + obj->width - lineWidth;
    }
    else {
      LOG_FATAL(OBJ_LOGGER,"Forbidden obj->textAlignment = %d\n",obj->textAlignment);
    }
    rectArea.y0 = obj->y0 + (obj->height - textHeight)/2 + line*lineHeight;
    rectArea.width = lineWidth;

    LOG_DEBUG(OBJ_LOGGER,"draw_textArea(), %s line %d, lineLen %d lineWidth = %d, obj->height = %d, textHeight = %d, obj->nbMaxLines = %d\n",text,  line,lineLen, lineWidth,obj->height, textHeight, obj->nbMaxLines);
    if ((obj->nbMaxLines == 0) || (line < (obj->nbMaxLines-1))) {
      nbgl_drawText(&rectArea, text, lineLen, obj->fontId, obj->textColor);
    }
    else {
      // for last chunk, if nbMaxLines is used, replace the 3 last chars by "..."
      // draw line except 3 last chars
      nbgl_drawText(&rectArea, text, lineLen-3, obj->fontId, obj->textColor);
      // draw "..." after the other chars
      rectArea.x0 += nbgl_getSingleLineTextWidthInLen(obj->fontId, text, lineLen-3);
      rectArea.width = nbgl_getSingleLineTextWidth(obj->fontId,"...");
      nbgl_drawText(&rectArea, "...", 3, obj->fontId, obj->textColor);
      return;
    }
    text += lineLen;
    /* skip trailing \n */
    if (*text == '\n') {
      text++;
    }
  }
}

/**
 * @brief internal function used to draw a QR Code
 * @note the QRCode is centered in the given object area
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_qrCode(nbgl_qrcode_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  nbgl_area_t rectArea;

  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_qrCode(), x0 = %d, y0 = %d, width = %d, height = %d, text = %s\n", obj->x0, obj->y0, obj->width, obj->height, obj->text);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

  rectArea.x0 = obj->x0;
  rectArea.y0 = obj->y0;
  rectArea.width = obj->width;
  rectArea.height = obj->height;
  rectArea.backgroundColor = obj->backgroundColor;
#ifdef NBGL_QRCODE
  nbgl_drawQrCode(&rectArea,(obj->version == QRCODE_V4) ? 4:10,obj->text,obj->foregroundColor);
#endif // NBGL_QRCODE

}

/**
 * @brief internal function used to draw a Keyboard object
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_keyboard(nbgl_keyboard_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  obj->width = SCREEN_WIDTH;
  obj->height = 3*KEYBOARD_KEY_HEIGHT;
  if (!obj->lettersOnly) {
    obj->height += KEYBOARD_KEY_HEIGHT;
  }

  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_keyboard(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

#ifdef NBGL_KEYBOARD
  nbgl_objDrawKeyboard(obj);
#endif // NBGL_KEYBOARD

}

/**
 * @brief internal function used to draw a Keypad object
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_keypad(nbgl_keypad_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  obj->width = SCREEN_WIDTH;
  obj->height = 4*KEYPAD_KEY_HEIGHT;

  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_keypad(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

#ifdef NBGL_KEYPAD
  nbgl_objDrawKeypad(obj);
#endif // NBGL_KEYPAD

}

/**
 * @brief internal function used to draw a Spinner object
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_spinner(nbgl_spinner_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  nbgl_area_t rectArea;

  obj->width = 60;
  obj->height = 44;

  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }
  LOG_DEBUG(OBJ_LOGGER,"draw_spinner(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);

  // inherit background from parent
  obj->backgroundColor = obj->parent->backgroundColor;

  rectArea.bpp = NBGL_BPP_1;
  rectArea.backgroundColor = obj->backgroundColor;
  // if position is OxFF, it means "fixed" so draw 4 corners
  if (obj->position == 0xFF) {
    // draw horizontal segments
    rectArea.x0 = obj->x0;
    rectArea.y0 = obj->y0;
    rectArea.width = 20;
    rectArea.height = 4;
    nbgl_frontDrawHorizontalLine(&rectArea, 0x7, BLACK); // top left
    rectArea.x0 = obj->x0+obj->width-rectArea.width;
    nbgl_frontDrawHorizontalLine(&rectArea,0x7, BLACK); // top right
    rectArea.y0 = obj->y0+obj->height-4;
    nbgl_frontDrawHorizontalLine(&rectArea,0xE, BLACK); //bottom right
    rectArea.x0 = obj->x0;
    nbgl_frontDrawHorizontalLine(&rectArea,0xE, BLACK); // bottom left
    // draw vertical segments
    rectArea.x0 = obj->x0;
    rectArea.y0 = obj->y0;
    rectArea.width = 3;
    rectArea.height = 12;
    rectArea.backgroundColor = BLACK;
    nbgl_frontDrawRect(&rectArea); // top left
    rectArea.x0 = obj->x0+obj->width-rectArea.width;
    nbgl_frontDrawRect(&rectArea); // top right
    rectArea.y0 = obj->y0+obj->height-rectArea.height;
    nbgl_frontDrawRect(&rectArea); //bottom right
    rectArea.x0 = obj->x0;
    nbgl_frontDrawRect(&rectArea); // bottom left
  }
  else {
    uint8_t mask;
    // clean up full rectangle
    rectArea.x0 = obj->x0;
    rectArea.y0 = obj->y0;
    rectArea.width = obj->width;
    rectArea.height = obj->height;
    rectArea.backgroundColor = WHITE;
    nbgl_frontDrawRect(&rectArea); // top left

    // draw horizontal segment in BLACK
    rectArea.width = 20;
    rectArea.height = 4;
    switch (obj->position) {
    case 0: // top left corner
      rectArea.x0 = obj->x0;
      rectArea.y0 = obj->y0;
      mask = 0x7;
      break;
    case 1: // top right
      rectArea.x0 = obj->x0+obj->width-rectArea.width;
      rectArea.y0 = obj->y0;
      mask = 0x7;
      break;
    case 2: //bottom right
      rectArea.x0 = obj->x0+obj->width-rectArea.width;
      rectArea.y0 = obj->y0+obj->height-4;
      mask = 0xE;
      break;
    case 3: // bottom left
      rectArea.x0 = obj->x0;
      rectArea.y0 = obj->y0+obj->height-4;
      mask = 0xE;
      break;
    default:
      return;
    }
    nbgl_frontDrawHorizontalLine(&rectArea, mask, BLACK);

    // draw vertical segment in BLACK
    rectArea.width = 3;
    rectArea.height = 12;
    rectArea.backgroundColor = BLACK;
    switch (obj->position) {
    case 0:// top left corner
      rectArea.x0 = obj->x0;
      rectArea.y0 = obj->y0;
      break;
    case 1:// top right corner
      rectArea.x0 = obj->x0+obj->width-rectArea.width;
      rectArea.y0 = obj->y0;
      break;
    case 2:// bottom right corner
      rectArea.x0 = obj->x0+obj->width-rectArea.width;
      rectArea.y0 = obj->y0+obj->height-rectArea.height;
      break;
    case 3:// bottom left corner
      rectArea.x0 = obj->x0;
      rectArea.y0 = obj->y0+obj->height-rectArea.height;
      break;
    default:
      return;
    }
    nbgl_frontDrawRect(&rectArea);
  }
}

/**
 * @brief internal function used to draw an image file
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
static void draw_image_file(nbgl_image_file_t* obj, nbgl_obj_t *prevObj, bool computePosition) {
  if (obj->buffer == NULL) {
    return;
  }
  if (computePosition) {
    compute_position((nbgl_obj_t *)obj,prevObj);
  }

  LOG_DEBUG(OBJ_LOGGER,"draw_image_file(), x0 = %d, y0 = %d\n", obj->x0, obj->y0);
  nbgl_frontDrawImageFile((nbgl_area_t *)obj, (uint8_t*)obj->buffer, 0, ramBuffer);
}

/**
 * @brief internal function used to draw an object of any type
 *
 * @param obj the object to draw
 * @param prevObj the previous object drawned in the same container, with the default layout
 * @param computePosition if TRUE, force to compute the object position
 */
#ifdef __GNUC__
#ifndef __clang__
__attribute__((optimize("O0")))
#endif
#endif
static void draw_object(nbgl_obj_t *obj, nbgl_obj_t *prevObj, bool computePosition) {
  LOG_DEBUG(OBJ_LOGGER,"draw_object() obj->type = %d, prevObj = %p\n",obj->type,prevObj);
  switch (obj->type) {
    case SCREEN:
      draw_screen((nbgl_container_t*)obj);
      break;
    case CONTAINER:
      draw_container((nbgl_container_t*)obj, prevObj, computePosition);
      break;
    case PANEL:
      draw_panel((nbgl_panel_t*)obj, prevObj, computePosition);
      break;
    case BUTTON:
      draw_button((nbgl_button_t*)obj, prevObj, computePosition);
      break;
    case LINE:
      draw_line((nbgl_line_t*)obj, prevObj, computePosition);
      break;
    case IMAGE:
      draw_image((nbgl_image_t*)obj, prevObj, computePosition);
      break;
    case SWITCH:
      draw_switch((nbgl_switch_t*)obj, prevObj, computePosition);
      break;
    case RADIO_BUTTON:
      draw_radioButton((nbgl_radio_t*)obj, prevObj, computePosition);
      break;
    case PROGRESS_BAR:
      draw_progressBar((nbgl_progress_bar_t*)obj, prevObj, computePosition);
      break;
    case PAGE_INDICATOR:
      draw_pageIndicator((nbgl_page_indicator_t*)obj, prevObj, computePosition);
      break;
    case TEXT_AREA:
      draw_textArea((nbgl_text_area_t*)obj, prevObj, computePosition);
      break;
    case QR_CODE:
      draw_qrCode((nbgl_qrcode_t*)obj, prevObj, computePosition);
      break;
    case KEYBOARD:
      draw_keyboard((nbgl_keyboard_t*)obj, prevObj, computePosition);
      break;
    case KEYPAD:
      draw_keypad((nbgl_keypad_t*)obj, prevObj, computePosition);
      break;
    case SPINNER:
      draw_spinner((nbgl_spinner_t*)obj, prevObj, computePosition);
      break;
    case IMAGE_FILE:
      draw_image_file((nbgl_image_file_t*)obj, prevObj, computePosition);
      break;
    default:
      LOG_DEBUG(OBJ_LOGGER,"Not existing object type\n");
      break;
  }

  #ifdef HAVE_SERIALIZED_NBGL
  io_seproxyhal_send_nbgl_serialized(NBGL_DRAW_OBJ, obj);
  #endif
  extendRefreshArea(obj);
}

/**
 * @brief internal function used when an object is drawn, to extend the area to be refresh in display
 *
 * @param obj the object drawn
 */
void extendRefreshArea(nbgl_obj_t *obj) {
  uint16_t x1,y1; // bottom right corner
  x1 = refreshArea.x0+refreshArea.width;
  y1 = refreshArea.y0+refreshArea.height;

  // if obj top-left is on left of current top-left corner, move top-left corner
  if (obj->x0 < refreshArea.x0) {
    refreshArea.x0 = obj->x0;
  }
  // if obj bottom-right is on right of current bottom-right corner, move bottom-right corner
  if (((obj->x0+obj->width) > x1) || (refreshArea.width == 0)) {
    x1 = obj->x0+obj->width;
  }
  // if obj top-left is on top of current top-left corner, move top-left corner
  if (obj->y0 < refreshArea.y0) {
    refreshArea.y0 = obj->y0;
  }
  // if obj bottom-right is on bottom of current bottom-right corner, move bottom-right corner
  if (((obj->y0+obj->height) > y1) || (refreshArea.height == 0)) {
    y1 = obj->y0+obj->height;
  }

  // sanity check
  if (x1 > SCREEN_WIDTH) {
    LOG_FATAL(OBJ_LOGGER,"extendRefreshArea: Impossible area x0 = %d width %d\n",refreshArea.x0,refreshArea.width);
  }
  if (y1 > SCREEN_HEIGHT) {
    LOG_FATAL(OBJ_LOGGER,"extendRefreshArea: Impossible area y0 = %d height %d\n",refreshArea.y0,refreshArea.height);
  }
  // recompute width and height
  refreshArea.width = x1 - refreshArea.x0;
  refreshArea.height = y1 - refreshArea.y0;

  // revaluate area bpp
  if (obj->bpp > refreshArea.bpp) {
    refreshArea.bpp = obj->bpp;
  }
}

/**
 * @brief This function redraws the given object and its children (recursive version)
 *
 * @param obj the object to redraw
 * @param prevObj the previous child of this object in the parent's children array (may be NULL for first element of the array)
 * @param computePosition if set to true, means that the position of the object will be recomputed (needs prevObj if alignment on this object)
 */
void nbgl_redrawObject(nbgl_obj_t *obj, nbgl_obj_t *prevObj, bool computePosition) {
  uint8_t i=0;
  LOG_DEBUG(OBJ_LOGGER,"nbgl_redrawObject(): obj = %p\n",obj);
  // draw the object itself
  draw_object(obj,prevObj,computePosition);

  if ((obj->type == SCREEN) ||
      (obj->type == CONTAINER) ||
      (obj->type == PANEL)) {
    nbgl_container_t *container = (nbgl_container_t *)obj;
    nbgl_obj_t *prev = NULL;
    LOG_DEBUG(OBJ_LOGGER,"nbgl_redrawObject(): container->children = %p\n",container->children);
    // draw the children, if any
    if (container->children != NULL) {
      for (i=0;i<container->nbChildren;i++) {
        nbgl_obj_t *current = container->children[i];
        if (current!= NULL) {
          current->parent = (nbgl_obj_t *)container;
          nbgl_redrawObject(current,prev,true);
          if (current->alignTo == NULL)
            prev = current;
        }
      }
    }
  }
}

/**
 * @brief This functions refreshes the actual screen on display with what has changed since the last refresh
 *
 */
void nbgl_refresh(void) {
  nbgl_refreshSpecial(FULL_COLOR_REFRESH);
}

/**
 * @brief This functions refreshes the actual screen on display with what has changed since the last refresh,
 *        according to the given mode (Black&White and other)
 * @param mode mode of refresh
 */
void nbgl_refreshSpecial(nbgl_refresh_mode_t mode) {
  if ((refreshArea.width == 0) || (refreshArea.height == 0))
    return;

  #ifdef HAVE_SERIALIZED_NBGL
  io_seproxyhal_send_nbgl_serialized(NBGL_REFRESH_AREA, (nbgl_obj_t *) &refreshArea);
  #endif
  nbgl_frontRefreshArea(&refreshArea, mode);
  LOG_DEBUG(OBJ_LOGGER,"nbgl_refreshSpecial(), x0,y0 = [%d, %d], w,h = [%d, %d]\n", refreshArea.x0, refreshArea.y0, refreshArea.width, refreshArea.height);
  refreshArea.x0 = SCREEN_WIDTH-1;
  refreshArea.width = 0;
  refreshArea.y0 = SCREEN_HEIGHT-1;
  refreshArea.height = 0;
  refreshArea.bpp = NBGL_BPP_2;
}

/**
 * @brief This functions inits all internal of nbgl objects layer
 *
 */
void nbgl_objInit(void) {
  // init area to the smallest size
  refreshArea.bpp = NBGL_BPP_2;
  refreshArea.x0 = SCREEN_WIDTH-1;
  refreshArea.width = 0;
  refreshArea.y0 = SCREEN_HEIGHT-1;
  refreshArea.height = 0;
}
