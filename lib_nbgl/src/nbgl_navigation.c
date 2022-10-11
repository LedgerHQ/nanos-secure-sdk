
/**
 * @file nbgl_navigation.c
 * @brief The construction of a navigation bar with buttons super-object
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
#define BUTTON_RADIUS RADIUS_40_PIXELS
#define BUTTON_DIAMETER 80
#define INTERNAL_SMALL_MARGIN 8


/**********************
 *      TYPEDEFS
 **********************/
enum {
  EXIT_BUTTON_INDEX = 0,
  PREVIOUS_PAGE_INDEX,
  NEXT_PAGE_INDEX,
  NB_MAX_CHILDREN
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
// Be careful, no nav is allowed in Modals
static nbgl_touchCallback_t navCallback = NULL;
static nbgl_container_t *navContainer;
static uint8_t navNbPages; ///< number of pages.
static uint8_t navActivePage; ///< index of active page (from 0 to nbPages-1).

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void configButtons(void) {
  nbgl_button_t *buttonPrevious = (nbgl_button_t *)navContainer->children[PREVIOUS_PAGE_INDEX];
  nbgl_button_t *buttonNext = (nbgl_button_t *)navContainer->children[NEXT_PAGE_INDEX];

  if (navActivePage == 0) {
    // only modify left button if not used for exit
    buttonPrevious->foregroundColor = LIGHT_GRAY;
  }
  else {
    buttonPrevious->foregroundColor = BLACK;
  }
  if (navActivePage == (navNbPages-1)) {
    buttonNext->foregroundColor = LIGHT_GRAY;
  }
  else {
    buttonNext->foregroundColor = BLACK;
  }
}

static void navTouchCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType) {
  if (eventType != TOUCHED) {
    return;
  }
  if (obj == navContainer->children[EXIT_BUTTON_INDEX]) {
    // fake page when Quit button is touched
    navActivePage = EXIT_PAGE;
    navCallback((nbgl_obj_t *)navContainer,VALUE_CHANGED);
  }
  else if (obj == navContainer->children[PREVIOUS_PAGE_INDEX]) {
    if (navActivePage > 0) {
      navActivePage--;
      configButtons();
      navCallback((nbgl_obj_t *)navContainer,VALUE_CHANGED);
    }
  }
  else if (obj == navContainer->children[NEXT_PAGE_INDEX]) {
    if (navActivePage < (navNbPages-1)) {
      navActivePage++;
      configButtons();
      navCallback((nbgl_obj_t *)navContainer,VALUE_CHANGED);
    }
  }
}

/**
 * @brief This function creates a full navigation bar "object", with button and returns it as a container
 *
 * @note Be careful, no navigation is allowed in modal if already used under it
 *
 * @param nbPages max number of pages for navigation (if < 2, no navigation keys)
 * @param activePage active page at start-up in [0-(nbPages-1)]
 * @param withExitKey if set to true, on page 0 the left key is replaced by a exit key
 * @param callback the callback called when an navigation button is pressed
 * @param layer layer (screen) to create the navigation bar in
 * @return the created navigation bar container object
 */
nbgl_container_t *nbgl_navigationPopulate(uint8_t nbPages, uint8_t activePage, bool withExitKey, nbgl_touchCallback_t callback, uint8_t layer) {
  nbgl_button_t *button;

  navNbPages = nbPages;
  navActivePage = activePage;

  navContainer = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER, layer);
  navContainer->width = SCREEN_WIDTH - 2*BORDER_MARGIN;
  navContainer->height = BUTTON_DIAMETER+2*BORDER_MARGIN;
  navContainer->layout = HORIZONTAL ;
  navContainer->nbChildren = NB_MAX_CHILDREN;
  navContainer->children = (nbgl_obj_t**)nbgl_containerPoolGet(navContainer->nbChildren, layer);
  navContainer->alignmentMarginX = 0;
  navContainer->alignmentMarginY = 0;
  navContainer->alignment = NO_ALIGNMENT;

  if (withExitKey) {
    button = (nbgl_button_t*)nbgl_objPoolGet(BUTTON,layer);
    button->innerColor = WHITE;
    button->borderColor = LIGHT_GRAY;
    button->width = BUTTON_DIAMETER;
    button->height = BUTTON_DIAMETER;
    button->radius = BUTTON_RADIUS;
    button->text = NULL;
    button->icon = &C_cross32px;
    button->alignmentMarginX = 0;
    button->alignmentMarginY = 0;

    button->alignment = (nbPages > 1) ? MID_LEFT:CENTER;
    button->alignTo = NULL;
    button->touchMask = (1<<TOUCHED);
    button->touchCallback = (nbgl_touchCallback_t)&navTouchCallback;
    navContainer->children[EXIT_BUTTON_INDEX] = (nbgl_obj_t*)button;
  }
  if (nbPages > 1) {
    button = (nbgl_button_t*)nbgl_objPoolGet(BUTTON,layer);
    button->innerColor = WHITE;
    button->borderColor = LIGHT_GRAY;
    button->width = (SCREEN_WIDTH - (2*BORDER_MARGIN+2*INTERNAL_SMALL_MARGIN+BUTTON_DIAMETER))/2;
    if (!withExitKey)
      button->width += BUTTON_DIAMETER/2;
    button->height = BUTTON_DIAMETER;
    button->radius = BUTTON_RADIUS;
    button->text = NULL;
    button->icon = &C_leftArrow32px;
    button->alignmentMarginY = 0;
    if (withExitKey) {
      button->alignmentMarginX = INTERNAL_SMALL_MARGIN;
      button->alignment = MID_RIGHT;
      button->alignTo = navContainer->children[EXIT_BUTTON_INDEX];
    }
    else {
      button->alignmentMarginX = 0;
      button->alignment = MID_LEFT;
      button->alignTo = NULL;

    }
    button->touchMask = (1<<TOUCHED);
    button->touchCallback = (nbgl_touchCallback_t)&navTouchCallback;
    navContainer->children[PREVIOUS_PAGE_INDEX] = (nbgl_obj_t*)button;

    // create next page button
    button = (nbgl_button_t*)nbgl_objPoolGet(BUTTON,layer);
    button->innerColor = WHITE;
    button->borderColor = LIGHT_GRAY;
    button->foregroundColor = BLACK;
    button->width = (SCREEN_WIDTH - (2*BORDER_MARGIN+2*INTERNAL_SMALL_MARGIN+BUTTON_DIAMETER))/2;
    if (!withExitKey)
      button->width += BUTTON_DIAMETER/2;
    button->height = BUTTON_DIAMETER;
    button->radius = BUTTON_RADIUS;
    button->text = NULL;
    button->icon = &C_rightArrow32px;
    button->alignmentMarginX = INTERNAL_SMALL_MARGIN;
    button->alignmentMarginY = 0;
    button->alignment = MID_RIGHT;
    button->alignTo = navContainer->children[PREVIOUS_PAGE_INDEX];
    button->touchMask = (1<<TOUCHED);
    button->touchCallback = (nbgl_touchCallback_t)&navTouchCallback;
    navContainer->children[NEXT_PAGE_INDEX] = (nbgl_obj_t*)button;

    configButtons();
  }

  navCallback = callback;
  return navContainer;
}

/**
 * @brief get the active page of the given navigation container
 *
 * @return active page
 */
uint8_t nbgl_navigationGetActivePage(void) {
  return navActivePage;
}
