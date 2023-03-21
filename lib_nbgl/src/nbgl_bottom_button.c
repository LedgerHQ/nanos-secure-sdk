
/**
 * @file nbgl_bottom_button.c
 * @brief The construction of a bottom area with a centered button
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_debug.h"
#include "nbgl_draw.h"
#include "nbgl_obj.h"
#include "glyphs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
enum {
  QUIT_BUTTON_INDEX = 0,
  LINE_INDEX,
  NB_MAX_CHILDREN
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      VARIABLES
 **********************/


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief This function creates a bottom area with a centered button and a top line. Returns it as a container
 *
 * @param icon icon to place in centered button
 * @param separationLine if set to true, adds a light gray separation line on top of the container
 * @param layer screen layer to use
 * @return the created container object
 */
nbgl_container_t *nbgl_bottomButtonPopulate(const nbgl_icon_details_t *icon, bool separationLine, uint8_t layer) {
  nbgl_button_t *button;
  nbgl_container_t *container;

  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER, layer);
  container->width = SCREEN_WIDTH;
  container->height = BUTTON_DIAMETER+2*BORDER_MARGIN;
  container->layout = HORIZONTAL ;
  container->nbChildren = NB_MAX_CHILDREN;
  container->children = (nbgl_obj_t**)nbgl_containerPoolGet(container->nbChildren, layer);
  container->alignment = NO_ALIGNMENT;

  button = (nbgl_button_t*)nbgl_objPoolGet(BUTTON,layer);
  button->innerColor = WHITE;
  button->borderColor = LIGHT_GRAY;
  button->width = BUTTON_DIAMETER;
  button->height = BUTTON_DIAMETER;
  button->radius = BUTTON_RADIUS;
  button->icon = icon;
  button->alignment = CENTER;
  button->touchMask = (1<<TOUCHED);
  container->children[QUIT_BUTTON_INDEX] = (nbgl_obj_t*)button;

  if (separationLine) {
    nbgl_line_t *line;
    // create horizontal line
    line = (nbgl_line_t*)nbgl_objPoolGet(LINE,0);
    line->lineColor = LIGHT_GRAY;
    line->width = SCREEN_WIDTH;
    line->height = 4;
    line->direction = HORIZONTAL;
    line->thickness = 1;
    line->alignmentMarginY = BORDER_MARGIN-4;
    line->alignTo = (nbgl_obj_t*)button;
    line->alignment = TOP_MIDDLE;
    container->children[LINE_INDEX] = (nbgl_obj_t*)line;
  }

  return container;
}
