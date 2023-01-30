/**
 * @file nbgl_layout.c
 * @brief Implementation of predefined layouts management for Applications
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdlib.h>
#include "nbgl_debug.h"
#include "nbgl_front.h"
#include "nbgl_layout.h"
#include "nbgl_obj.h"
#include "nbgl_draw.h"
#include "nbgl_screen.h"
#include "nbgl_touch.h"
#include "glyphs.h"
#include "os_pic.h"

/*********************
 *      DEFINES
 *********************/
// half internal margin, between items
#define INTERNAL_SPACE 16
// internal margin, between sub-items
#define INTERNAL_MARGIN 8
// inner margin, between buttons
#define INNER_MARGIN 12

#define NB_MAX_LAYOUTS 3

// used by container
#define NB_MAX_CONTAINER_CHILDREN 20

// used by screen
#define NB_MAX_SCREEN_CHILDREN 6

/**
 * @brief Max number of complex objects with callback retrievable from pool
 *
 */
#define LAYOUT_OBJ_POOL_LEN 10

#define TAG_VALUE_ICON_WIDTH 32

#define TOUCHABLE_BAR_HEIGHT 88
#define FOOTER_HEIGHT 80

// refresh period of the spinner, in ms
#define SPINNER_REFRESH_PERIOD 400

/**********************
 *      MACROS
 **********************/
#define ASSERT_ENOUGH_PLACE_FOR_MAIN_PANEL_CHILD(__layout) { \
  if ((__layout)->panel.nbChildren==(NB_MAX_MAIN_PANEL_CHILDREN-1)) return NO_MORE_OBJ_ERROR; \
}

#define GET_AVAILABLE_WIDTH(__layout) (SCREEN_WIDTH-2*BORDER_MARGIN)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
  nbgl_obj_t* obj;
  uint8_t token; // user token, attached to callback
  uint8_t index; // index within the token
  tune_index_e tuneId; // if not @ref NBGL_NO_TUNE, a tune will be played
} layoutObj_t;

/**
 * @brief Structure containing all information about the current layout.
 * @note It shall not be used externally
 *
 */
typedef struct nbgl_layoutInternal_s {
  bool modal; ///< if true, means the screen is a modal
  bool withLeftBorder; ///< if true, draws a light gray left border on the whole height of the screen
  uint8_t layer; ///< if >0, puts the layout on top of screen stack (modal). Otherwise puts on background (for apps)
  uint8_t nbChildren; ///< number of children in above array
  nbgl_obj_t **children; ///< children for main screen

  nbgl_obj_type_t bottomContainerUsage;
  nbgl_container_t *bottomContainer; // Used for navigation bar
  nbgl_text_area_t *tapText;
  nbgl_layoutTouchCallback_t callback; // user callback for all controls
  // This is the pool of callback objects, usable by any layout
  layoutObj_t callbackObjPool[LAYOUT_OBJ_POOL_LEN];
  // number of callback objects used by the whole layout in callbackObjPool
  uint8_t nbUsedCallbackObjs;

  nbgl_container_t *container;

} nbgl_layoutInternal_t;

/**********************
 *      VARIABLES
 **********************/

/**
 * @brief array of layouts, if used by modal
 *
 */
static nbgl_layoutInternal_t gLayout[NB_MAX_LAYOUTS] = {0};

#ifdef NBGL_KEYBOARD
static nbgl_button_t *choiceButtons[NB_MAX_SUGGESTION_BUTTONS];
#endif // NBGL_KEYBOARD

/**********************
 *  STATIC PROTOTYPES
 **********************/

// Unit step in % of touchable progress bar
#define HOLD_TO_APPROVE_STEP_PERCENT (17)
// Duration in ms the user must hold the progress bar
// to make it progress HOLD_TO_APPROVE_STEP_PERCENT %.
// This duration must be higher than the screen refresh duration.
#define HOLD_TO_APPROVE_STEP_DURATION_MS (400)

static inline uint8_t get_hold_to_approve_percent(uint32_t touch_duration) {
        uint8_t current_step_nb = (touch_duration / HOLD_TO_APPROVE_STEP_DURATION_MS) + 1;
        return (current_step_nb * HOLD_TO_APPROVE_STEP_PERCENT);
}

// function used to retrieve the concerned layout and layout obj matching the given touched obj
static bool getLayoutAndLayoutObj(nbgl_obj_t *obj, nbgl_layoutInternal_t **layout, layoutObj_t **layoutObj) {
  uint8_t i = NB_MAX_LAYOUTS;

  // gLayout[1] is on top of gLayout[0] so if gLayout[1] is active, it must catch the event
  *layout = NULL;
  while (i>0) {
    i--;
    if (gLayout[i].nbChildren > 0) {
      *layout = &gLayout[i];
      break;
    }
  }
  if (*layout == NULL) {
    return false;
  }
  i=0;
  // get index of obj
  while (i<(*layout)->nbUsedCallbackObjs) {
    if (obj == (nbgl_obj_t *)(*layout)->callbackObjPool[i].obj) {
      break;
    }
    i++;
  }
  LOG_DEBUG(LAYOUT_LOGGER,"touchCallback(): i = %d, nbUsedCallbackObjs = %d\n",i, (*layout)->nbUsedCallbackObjs);
  if (i==(*layout)->nbUsedCallbackObjs)
    return false;

  *layoutObj = &((*layout)->callbackObjPool[i]);
  return true;
}

// callback for most touched object
static void touchCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType) {
  nbgl_layoutInternal_t *layout;
  layoutObj_t *layoutObj;

  UNUSED(eventType);
  LOG_DEBUG(LAYOUT_LOGGER,"touchCallback(): obj = %p, gLayout[1].nbChildren = %d\n",obj,gLayout[1].nbChildren);

  if (getLayoutAndLayoutObj(obj, &layout, &layoutObj) == false) {
    LOG_WARN(LAYOUT_LOGGER,"touchCallback(): eventType = %d, obj = %p, no active layout\n",eventType,obj);
    return;
  }

  // case of page indicator
  if ((obj == (nbgl_obj_t *)layout->bottomContainer)&&(layout->bottomContainerUsage = PAGE_INDICATOR)) {
    layoutObj->index = nbgl_navigationGetActivePage();
  }

  // case of switch
  if ((obj->type == CONTAINER) &&
      (((nbgl_container_t*)obj)->nbChildren >= 2) &&
      (((nbgl_container_t*)obj)->children[1] != NULL) &&
      (((nbgl_container_t*)obj)->children[1]->type == SWITCH)) {
    nbgl_switch_t *lSwitch = (nbgl_switch_t*)((nbgl_container_t*)obj)->children[1];
    lSwitch->state = (lSwitch->state == ON_STATE)?OFF_STATE:ON_STATE;
    nbgl_redrawObject((nbgl_obj_t *)lSwitch,false,false);
    nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
    // index is used for state
    layoutObj->index = lSwitch->state;
  }
  LOG_DEBUG(LAYOUT_LOGGER,"touchCallback(): layout->callback = %p\n",layout->callback);
  if (layout->callback != NULL) {
#ifdef HAVE_PIEZO_SOUND
    if (layoutObj->tuneId<NBGL_NO_TUNE) {
      io_seproxyhal_play_tune(layoutObj->tuneId);
    }
#endif // HAVE_PIEZO_SOUND
    layout->callback(layoutObj->token,layoutObj->index);
  }
}

// callback for long press button
static void longTouchCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType) {
  nbgl_layoutInternal_t *layout;
  layoutObj_t *layoutObj;
  nbgl_container_t *container = (nbgl_container_t *)obj;
    // 4th child of container is the progress bar
  nbgl_progress_bar_t *progressBar = (nbgl_progress_bar_t *)container->children[3];

  LOG_DEBUG(LAYOUT_LOGGER,"longTouchCallback(): eventType = %d, obj = %p, gLayout[1].nbChildren = %d\n",eventType,obj,gLayout[1].nbChildren);

  if (getLayoutAndLayoutObj(obj, &layout, &layoutObj) == false) {
    LOG_WARN(LAYOUT_LOGGER,"longTouchCallback(): eventType = %d, obj = %p, no active layout\n",eventType,obj);
    return;
  }
  // case of pressing a long press button
  if (eventType == TOUCHING) {
      uint32_t touchDuration = nbgl_touchGetTouchDuration(obj);

      // Compute the new progress bar state in %
      uint8_t new_state = get_hold_to_approve_percent(touchDuration);

      // Ensure the callback is triggered once,
      // when the progress bar state reaches 100%
      bool trigger_callback = (new_state >= 100) && (progressBar->state < 100);

      // Cap progress bar state at 100%
      if(new_state >= 100) {
        new_state = 100;
      }

      // Update progress bar state
      if(new_state != progressBar->state) {
        progressBar->state = new_state;
        nbgl_redrawObject((nbgl_obj_t *)progressBar,false,false);
        // Ensure progress bar is fully drawn
        // before calling the callback.
        nbgl_refreshSpecial(BLACK_AND_WHITE_FAST_REFRESH);
      }

      if (trigger_callback) {
          // End of progress bar reached: trigger callback
          if (layout->callback != NULL) {
            layout->callback(layoutObj->token,layoutObj->index);
          }
      }
  }
  // case of releasing a long press button (or getting out of it)
  else if ((eventType == TOUCH_RELEASED) || (eventType == OUT_OF_TOUCH)) {
    progressBar->state = 0;
    nbgl_redrawObject((nbgl_obj_t *)progressBar,false,false);
    nbgl_refreshSpecial(BLACK_AND_WHITE_REFRESH);
  }
}

// callback for radio button touch
static void radioTouchCallback(nbgl_obj_t *obj, nbgl_touchType_t eventType) {
  uint8_t i=NB_MAX_LAYOUTS, radioIndex=0, foundRadio = 0xFF, foundRadioIndex;
  nbgl_layoutInternal_t *layout=NULL;

  if (eventType != TOUCHED) {
    return;
  }

  // gLayout[1] is on top of gLayout[0] so if gLayout[1] is active, it must catch the event
  while (i > 0) {
    i--;
    if (gLayout[i].nbChildren > 0) {
      layout = &gLayout[i];
      break;
    }
  }
  if (layout == NULL) {
    return;
  }

  i = 0;
  // parse all objs to find all containers of radio buttons
  while (i<layout->nbUsedCallbackObjs) {
    if ((obj == (nbgl_obj_t *)layout->callbackObjPool[i].obj) && (layout->callbackObjPool[i].obj->type == CONTAINER)) {
      nbgl_radio_t *radio = (nbgl_radio_t *)((nbgl_container_t*)layout->callbackObjPool[i].obj)->children[1];
      nbgl_text_area_t *textArea = (nbgl_text_area_t *)((nbgl_container_t*)layout->callbackObjPool[i].obj)->children[0];
      foundRadio = i;
      foundRadioIndex = radioIndex;
      // set text as active (black and bold)
      textArea->textColor = BLACK;
      textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
      // ensure that radio button is ON
      radio->state = ON_STATE;
      // redraw container
      nbgl_redrawObject((nbgl_obj_t*)obj,NULL,false);
    }
    else if ((layout->callbackObjPool[i].obj->type == CONTAINER) &&
             (((nbgl_container_t*)layout->callbackObjPool[i].obj)->nbChildren == 2) &&
             (((nbgl_container_t*)layout->callbackObjPool[i].obj)->children[1]->type == RADIO_BUTTON)) {
      nbgl_radio_t *radio = (nbgl_radio_t *)((nbgl_container_t*)layout->callbackObjPool[i].obj)->children[1];
      nbgl_text_area_t *textArea = (nbgl_text_area_t *)((nbgl_container_t*)layout->callbackObjPool[i].obj)->children[0];
      radioIndex++;
      // set to OFF the one that was in ON
      if (radio->state == ON_STATE) {
        radio->state = OFF_STATE;
        // set text it as inactive (gray and normal)
        textArea->textColor = DARK_GRAY;
        textArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
        // redraw container
        nbgl_redrawObject((nbgl_obj_t*)layout->callbackObjPool[i].obj,NULL,false);
      }
    }
    i++;
  }
  // call callback after redraw to avoid asynchronicity
  if (foundRadio != 0xFF) {
    if (layout->callback != NULL) {
#ifdef HAVE_PIEZO_SOUND
      if (layout->callbackObjPool[foundRadio].tuneId<NBGL_NO_TUNE) {
        io_seproxyhal_play_tune(layout->callbackObjPool[foundRadio].tuneId);
      }
#endif // HAVE_PIEZO_SOUND
      layout->callback(layout->callbackObjPool[foundRadio].token,foundRadioIndex);
    }
  }
}

// callback for spinner ticker
static void spinnerTickerCallback(void) {
  nbgl_spinner_t *spinner;
  uint8_t i=0;
  nbgl_layoutInternal_t *layout;

  // gLayout[1] is on top of gLayout[0] so if gLayout[1] is active, it must catch the event
  if (gLayout[1].nbChildren > 0) {
    layout = &gLayout[1];
  }
  else {
    layout = &gLayout[0];
  }

  // get index of obj
  while (i<layout->container->nbChildren) {
    if (layout->container->children[i]->type == SPINNER) {
      spinner = (nbgl_spinner_t *)layout->container->children[i];
      spinner->position++;
      spinner->position &= 3; // modulo 4
      nbgl_redrawObject((nbgl_obj_t*)spinner, NULL, false);
      nbgl_refreshSpecial(BLACK_AND_WHITE_FAST_REFRESH);
      return;
    }
    i++;
  }
}

static nbgl_line_t* createHorizontalLine(uint8_t layer) {
  nbgl_line_t *line;

  line = (nbgl_line_t*)nbgl_objPoolGet(LINE,layer);
  line->lineColor = LIGHT_GRAY;
  line->width = SCREEN_WIDTH;
  line->height = 4;
  line->direction = HORIZONTAL;
  line->thickness = 1;
  return line;
}

static nbgl_line_t* createLeftVerticalLine(uint8_t layer) {
  nbgl_line_t *line;

  line = (nbgl_line_t*)nbgl_objPoolGet(LINE,layer);
  line->lineColor = LIGHT_GRAY;
  line->width = 1;
  line->height = SCREEN_HEIGHT;
  line->direction = VERTICAL;
  line->thickness = 1;
  line->alignment = MID_LEFT;
  return line;
}

// function adding a layout object in the callbackObjPool array for the given layout, and configuring it
static layoutObj_t *addCallbackObj(nbgl_layoutInternal_t *layout, nbgl_obj_t *obj, uint8_t token, tune_index_e tuneId) {
  layoutObj_t *layoutObj = NULL;

  if (layout->nbUsedCallbackObjs < (LAYOUT_OBJ_POOL_LEN-1)) {
    layoutObj = &layout->callbackObjPool[layout->nbUsedCallbackObjs];
    layout->nbUsedCallbackObjs++;
    layoutObj->obj = obj;
    layoutObj->token = token;
    layoutObj->tuneId = tuneId;
  }

  return layoutObj;
}

/**
 * @brief adds the given obj to the container
 *
 * @param layout
 * @param obj
 */
static void addObjectToLayout(nbgl_layoutInternal_t *layout, nbgl_obj_t* obj) {
  layout->container->children[layout->container->nbChildren] = obj;
  layout->container->nbChildren++;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief returns a layout of the given type. The layout is reset
 *
 * @param description description of layout
 * @return a pointer to the corresponding layout
 */
nbgl_layout_t *nbgl_layoutGet(nbgl_layoutDescription_t *description) {
  nbgl_layoutInternal_t *layout = NULL;

  // find an empty layout in the proper "layer"
  if (description->modal) {
    if (gLayout[1].nbChildren == 0) {
      layout = &gLayout[1];
    }
    else if (gLayout[2].nbChildren == 0) {
      layout = &gLayout[2];
    }
  }
  else {
    // automatically "release" a potentially opened non-modal layout
    gLayout[0].nbChildren = 0;
    layout = &gLayout[0];
  }
  if (layout == NULL) {
    LOG_WARN(LAYOUT_LOGGER,"nbgl_layoutGet(): impossible to get a layout!\n");
    return NULL;
  }

  // reset globals
  memset(layout,0,sizeof(nbgl_layoutInternal_t));

  layout->callback = (nbgl_layoutTouchCallback_t)PIC(description->onActionCallback);
  layout->modal = description->modal;
  layout->withLeftBorder = description->withLeftBorder;
  if (description->modal) {
    layout->layer = nbgl_screenPush(&layout->children, NB_MAX_SCREEN_CHILDREN, &description->ticker);
  }
  else {
    nbgl_screenSet(&layout->children, NB_MAX_SCREEN_CHILDREN, &description->ticker);
    layout->layer = 0;
  }
  layout->container = (nbgl_container_t*)nbgl_objPoolGet(CONTAINER,layout->layer);
  layout->container->width = SCREEN_WIDTH;
  layout->container->height = SCREEN_HEIGHT;
  layout->container->layout = VERTICAL ;
  layout->container->nbChildren = 0;
  layout->container->alignmentMarginX = 0;
  layout->container->alignmentMarginY = 0;
  layout->container->touchMask = 0;
  layout->container->children = nbgl_containerPoolGet(NB_MAX_CONTAINER_CHILDREN,layout->layer);
  layout->children[layout->nbChildren] = (nbgl_obj_t*)layout->container;
  layout->nbChildren++;

  // if a tap text is defined, make the container tapable and display this text in gray
  if (description->tapActionText != NULL) {
    layoutObj_t *obj;

    obj = &layout->callbackObjPool[layout->nbUsedCallbackObjs];
    layout->nbUsedCallbackObjs++;
    obj->obj = (nbgl_obj_t*)layout->container;
    obj->token = description->tapActionToken;
    obj->tuneId = description->tapTuneId;
    layout->container->touchMask = (1<<TOUCHED);
    layout->container->touchCallback = (nbgl_touchCallback_t)&touchCallback;

    // create 'tap to continue' text area
    layout->tapText = (nbgl_text_area_t*)nbgl_objPoolGet(TEXT_AREA, 0);
    layout->tapText->localized = false;
    layout->tapText->text = PIC(description->tapActionText);
    layout->tapText->textColor = DARK_GRAY;
    layout->tapText->fontId = BAGL_FONT_INTER_REGULAR_24px;
    layout->tapText->width = SCREEN_WIDTH - 2*BORDER_MARGIN;
    layout->tapText->height = nbgl_getFontLineHeight(layout->tapText->fontId);
    layout->tapText->textAlignment = CENTER;
    layout->tapText->alignmentMarginX = 0;
    layout->tapText->alignmentMarginY = BORDER_MARGIN;
    layout->tapText->alignment = BOTTOM_MIDDLE;
    layout->tapText->alignTo = NULL;
    layout->tapText->touchMask = 0;
  }

  return (nbgl_layout_t*)layout;
}

/**
 * @brief Creates a Top-right button in the top right corner of the top panel
 *
 * @param layout the current layout
 * @param icon icon configuration
 * @param token the token that will be used as argument of the callback
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddTopRightButton(nbgl_layout_t *layout, const nbgl_icon_details_t *icon, uint8_t token, tune_index_e tuneId) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;
  nbgl_button_t *button;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddTopRightButton():\n");
  if (layout == NULL)
    return -1;
  button = (nbgl_button_t *)nbgl_objPoolGet(BUTTON,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)button,token,tuneId);
  if (obj == NULL)
    return -1;

  addObjectToLayout(layoutInt,(nbgl_obj_t*)button);
  button->width = BUTTON_DIAMETER;
  button->height = BUTTON_DIAMETER;
  button->radius = BUTTON_RADIUS;
  button->alignmentMarginX = BORDER_MARGIN;
  button->alignmentMarginY = BORDER_MARGIN;
  button->foregroundColor = BLACK;
  button->innerColor = WHITE;
  button->borderColor = LIGHT_GRAY;
  button->touchMask = (1<<TOUCHED);
  button->touchCallback = (nbgl_touchCallback_t)&touchCallback;
  button->icon = PIC(icon);
  button->alignment = TOP_RIGHT;

  return 0;
}

/**
 * @brief Creates a navigation bar on bottom of main container
 *
 * @param layout the current layout
 * @param info structure giving the description of the navigation bar
 * @return >= 0 if OK
 */
int nbgl_layoutAddNavigationBar(nbgl_layout_t *layout, nbgl_layoutNavigationBar_t *info) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddNavigationBar():\n");
  if (layout == NULL)
    return -1;

  layoutInt->bottomContainer = nbgl_navigationPopulate(info->nbPages, info->activePage, info->withExitKey, (nbgl_touchCallback_t)&touchCallback, layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)layoutInt->bottomContainer,info->token,info->tuneId);
  if (obj == NULL)
    return -1;

  layoutInt->bottomContainer->alignmentMarginX = 0;
  layoutInt->bottomContainer->alignmentMarginY = 0;
  layoutInt->bottomContainer->alignTo = NULL;
  layoutInt->bottomContainer->alignment = BOTTOM_MIDDLE;
  layoutInt->bottomContainerUsage = PAGE_INDICATOR; // used for navigation bar
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)layoutInt->bottomContainer;
  layoutInt->nbChildren++;

  layoutInt->container->height -= layoutInt->bottomContainer->height;

  if (info->withSeparationLine) {
    nbgl_line_t *line = createHorizontalLine(layoutInt->layer);
    line->alignTo = (nbgl_obj_t*)layoutInt->bottomContainer;
    line->alignment = TOP_MIDDLE;
    layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)line;
    layoutInt->nbChildren++;

    layoutInt->container->height -= 4;
  }

  return 0;
}

/**
 * @brief Creates a centered button at bottom of main container
 * @brief incompatible with navigation bar
 *
 * @param layout the current layout
 * @param icon icon inside the round button
 * @param token used as parameter of userCallback when button is touched
 * @param separationLine if set to true, adds a light gray separation line on top of the container
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddBottomButton(nbgl_layout_t *layout, const nbgl_icon_details_t *icon, uint8_t token, bool separationLine, tune_index_e tuneId) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddBottomButton():\n");
  if (layout == NULL)
    return -1;

  layoutInt->bottomContainer = nbgl_bottomButtonPopulate(icon,(nbgl_touchCallback_t)&touchCallback,separationLine,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)layoutInt->bottomContainer,token,tuneId);
  if (obj == NULL)
    return -1;
  layoutInt->bottomContainer->alignmentMarginX = 0;
  layoutInt->bottomContainer->alignmentMarginY = 0;
  layoutInt->bottomContainer->alignTo = NULL;
  layoutInt->bottomContainer->alignment = BOTTOM_MIDDLE;
  layoutInt->bottomContainerUsage = BUTTON; // used for a button
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)layoutInt->bottomContainer;
  layoutInt->nbChildren++;

  layoutInt->container->height -= layoutInt->bottomContainer->height;

  return 0;
}

/**
 * @brief Creates a touchable bar in main panel
 *
 * @param layout the current layout
 * @param barLayout the properties of the bar
 * @return >= 0 if OK
 */
int nbgl_layoutAddTouchableBar(nbgl_layout_t *layout, nbgl_layoutBar_t *barLayout) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;
  nbgl_text_area_t *textArea;
  nbgl_image_t *imageLeft=NULL, *imageRight=NULL;
  nbgl_container_t *container;
  color_t color = (barLayout->inactive != true)? BLACK:LIGHT_GRAY;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddTouchableBar():\n");
  if (layout == NULL)
    return -1;

  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)container,barLayout->token,barLayout->tuneId);
  if (obj == NULL)
    return -1;

  // get container children (up to 4)
  container->children = nbgl_containerPoolGet(4,layoutInt->layer);
  container->nbChildren = 0;

  container->width = GET_AVAILABLE_WIDTH(layoutInt);
  container->height = TOUCHABLE_BAR_HEIGHT;
  container->layout = HORIZONTAL;
  container->alignmentMarginX = BORDER_MARGIN;
  container->alignment = NO_ALIGNMENT;
  container->alignTo = NULL;
  // the bar can only be touched if not inactive AND if one of the icon is present
  // otherwise it is seen as a title
  if ((barLayout->inactive != true) && ((barLayout->iconLeft != NULL)||(barLayout->iconRight != NULL))) {
    container->touchMask = (1<<TOUCHED);
    container->touchCallback = (nbgl_touchCallback_t)&touchCallback;
  }

  if (barLayout->iconLeft != NULL) {
    imageLeft = (nbgl_image_t *)nbgl_objPoolGet(IMAGE,layoutInt->layer);
    imageLeft->foregroundColor = color;
    imageLeft->buffer = PIC(barLayout->iconLeft);
    imageLeft->alignment = MID_LEFT;
    imageLeft->alignTo = (nbgl_obj_t*)NULL;
    container->children[container->nbChildren] = (nbgl_obj_t*)imageLeft;
    container->nbChildren++;
  }
  if (barLayout->text != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    textArea->textColor = color;
    textArea->text = PIC(barLayout->text);
    textArea->onDrawCallback = NULL;
    textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
    textArea->width = container->width;
    if (barLayout->iconLeft != NULL)
      textArea->width -= imageLeft->buffer->width+BORDER_MARGIN;
    if (barLayout->iconRight != NULL)
      textArea->width -= ((nbgl_icon_details_t *)PIC(barLayout->iconRight))->width;
    textArea->height = container->height;
    textArea->style = NO_STYLE;
    if ((barLayout->iconLeft != NULL) && (barLayout->centered != true))
      textArea->alignmentMarginX = BORDER_MARGIN;
    if (barLayout->iconLeft != NULL) {
      textArea->alignTo = (nbgl_obj_t*)imageLeft;
      textArea->alignment = MID_RIGHT;
    }
    else {
      textArea->alignTo = (nbgl_obj_t*)NULL;
      textArea->alignment = NO_ALIGNMENT;
    }
    if (barLayout->centered != true) {
      textArea->textAlignment = MID_LEFT;
    }
    else {
      textArea->textAlignment = CENTER;
    }
    container->children[container->nbChildren] = (nbgl_obj_t*)textArea;
    container->nbChildren++;
  }
  if (barLayout->iconRight != NULL) {
    imageRight = (nbgl_image_t *)nbgl_objPoolGet(IMAGE,layoutInt->layer);
    imageRight->foregroundColor = color;
    imageRight->buffer = PIC(barLayout->iconRight);
    imageRight->alignment = MID_RIGHT;
    if (barLayout->text == NULL) {
      imageRight->alignTo = (nbgl_obj_t*)NULL;
    }
    else {
      imageRight->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
    }
    container->children[container->nbChildren] = (nbgl_obj_t*)imageRight;
    container->nbChildren++;
  }
  if (barLayout->subText != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);

    textArea->textColor = BLACK;
    textArea->text = PIC(barLayout->subText);
    textArea->textAlignment = MID_LEFT;
    textArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    textArea->height = nbgl_getTextHeight(textArea->fontId,textArea->text);
    textArea->style = NO_STYLE;
    textArea->alignment = BOTTOM_LEFT;
    textArea->alignmentMarginY = BORDER_MARGIN;
    textArea->width = container->width;
    container->children[container->nbChildren] = (nbgl_obj_t*)textArea;
    container->nbChildren++;
    container->height += textArea->height+16;
  }

  // set this new container as child of main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  return 0;
}

/**
 * @brief Creates a switch with the given text and its state
 *
 * @param layout the current layout
 * @param switchLayout description of the parameters of the switch
 * @return >= 0 if OK
 */
int nbgl_layoutAddSwitch(nbgl_layout_t *layout, nbgl_layoutSwitch_t *switchLayout) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;
  nbgl_text_area_t *textArea;
  nbgl_text_area_t *subTextArea;
  nbgl_switch_t *switchObj;
  nbgl_container_t *container;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddSwitch():\n");
  if (layout == NULL)
    return -1;
  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)container,switchLayout->token,switchLayout->tuneId);
  if (obj == NULL)
    return -1;

  // get container children
  container->children = nbgl_containerPoolGet(3,layoutInt->layer);
  container->width = GET_AVAILABLE_WIDTH(layoutInt);
  container->height = 2*BORDER_MARGIN;
  container->layout = VERTICAL;
  container->alignmentMarginX = BORDER_MARGIN;
  container->alignTo = NULL;
  container->alignment = NO_ALIGNMENT;
  container->touchMask = (1<<TOUCHED);
  container->touchCallback = (nbgl_touchCallback_t)&touchCallback;

  textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
  textArea->textColor = BLACK;
  textArea->text = PIC(switchLayout->text);
  textArea->textAlignment = MID_LEFT;
  textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  textArea->width = container->width-60; // the width icon has 60px width
  textArea->height = nbgl_getTextHeight(BAGL_FONT_INTER_SEMIBOLD_24px,textArea->text);
  container->height += textArea->height;
  textArea->style = NO_STYLE;
  textArea->alignment = TOP_LEFT;
  textArea->alignmentMarginY = BORDER_MARGIN;
  container->children[0] = (nbgl_obj_t*)textArea;

  switchObj = (nbgl_switch_t *)nbgl_objPoolGet(SWITCH,layoutInt->layer);
  switchObj->onColor = BLACK;
  switchObj->offColor = LIGHT_GRAY;
  switchObj->state = switchLayout->initState;
  switchObj->alignment = MID_RIGHT;
  switchObj->alignTo = (nbgl_obj_t*)textArea;
  switchObj->touchCallback = NULL;
  container->children[1] = (nbgl_obj_t*)switchObj;

  if (switchLayout->subText != NULL) {
    subTextArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    subTextArea->textColor = DARK_GRAY;
    subTextArea->text = PIC(switchLayout->subText);
    subTextArea->textAlignment = MID_LEFT;
    subTextArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    subTextArea->width = container->width;
    subTextArea->height = nbgl_getTextHeight(BAGL_FONT_INTER_REGULAR_24px,subTextArea->text);
    container->height += subTextArea->height+INTERNAL_SPACE;
    subTextArea->style = NO_STYLE;
    subTextArea->alignment = NO_ALIGNMENT;
    subTextArea->alignmentMarginY = INTERNAL_SPACE;
    subTextArea->alignTo = NULL;
    container->children[2] = (nbgl_obj_t*)subTextArea;
    container->nbChildren = 3;
  }
  else {
    container->nbChildren = 2;
  }
  // set this new container as child of main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  return 0;
}

/**
 * @brief Creates an area with given text and sub text (in gray)
 *
 * @param layout the current layout
 * @param text main text for the switch
 * @param subText description under main text (NULL terminated, single line, may be null)
 * @return >= 0 if OK
 */
int nbgl_layoutAddText(nbgl_layout_t *layout, char *text, char *subText) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_container_t *container;
  nbgl_text_area_t *textArea;
  nbgl_text_area_t *subTextArea;
  uint16_t fullHeight = 0;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddText():\n");
  if (layout == NULL)
    return -1;
  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER,layoutInt->layer);

  // get container children
  if (text != NULL)
    container->nbChildren++;
  if (subText != NULL)
    container->nbChildren++;

  container->children = nbgl_containerPoolGet(container->nbChildren,layoutInt->layer);
  container->width = GET_AVAILABLE_WIDTH(layoutInt);

  if (text != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);

    textArea->textColor = BLACK;
    textArea->text = PIC(text);
    textArea->textAlignment = MID_LEFT;
    textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
    textArea->height = nbgl_getTextHeight(textArea->fontId,textArea->text);
    textArea->style = NO_STYLE;
    textArea->alignment = NO_ALIGNMENT;
    textArea->alignmentMarginY = BORDER_MARGIN;
    textArea->width = container->width;
    fullHeight += textArea->height;
    container->children[0] = (nbgl_obj_t*)textArea;
  }
  if (subText != NULL) {
    subTextArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    subTextArea->textColor = BLACK;
    subTextArea->text = PIC(subText);
    subTextArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    subTextArea->style = NO_STYLE;
    subTextArea->width = container->width;
    subTextArea->height = nbgl_getTextHeightInWidth(subTextArea->fontId,subTextArea->text,subTextArea->width,false);
    subTextArea->textAlignment = MID_LEFT;
    subTextArea->alignment = NO_ALIGNMENT;
    fullHeight += subTextArea->height;
    if (text != NULL) {
      subTextArea->alignmentMarginY = INTERNAL_MARGIN;
      fullHeight += INTERNAL_MARGIN;
      container->children[1] = (nbgl_obj_t*)subTextArea;
    }
    else {
      subTextArea->alignmentMarginY = BORDER_MARGIN;
      container->children[0] = (nbgl_obj_t*)subTextArea;
    }
  }
  container->height = fullHeight+2*BORDER_MARGIN;
  container->layout = VERTICAL;
  container->alignmentMarginX = BORDER_MARGIN;
  container->alignment = NO_ALIGNMENT;
  // set this new obj as child of main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  return 0;
}

/**
 * @brief Creates an area with given text in 32px font (in Black)
 *
 * @param layout the current layout
 * @param text text to be displayed (auto-wrap)
 * @return >= 0 if OK
 */
int nbgl_layoutAddLargeCaseText(nbgl_layout_t *layout, char *text) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_text_area_t *textArea;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddLargeCaseText():\n");
  if (layout == NULL)
    return -1;
  textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);

  textArea->textColor = BLACK;
  textArea->text = PIC(text);
  textArea->textAlignment = MID_LEFT;
  textArea->fontId = BAGL_FONT_INTER_REGULAR_32px;
  textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
  textArea->height = nbgl_getTextHeightInWidth(textArea->fontId,textArea->text,textArea->width,false);
  textArea->style = NO_STYLE;
  textArea->alignment = NO_ALIGNMENT;
  textArea->alignmentMarginX = BORDER_MARGIN;
  textArea->alignmentMarginY = BORDER_MARGIN;

  // set this new obj as child of main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)textArea);

  return 0;
}

/**
 * @brief Creates a list of radio buttons (on the right)
 *
 * @param layout the current layout
 * @param choices structure giving the list of choices and the current selected one
 * @return >= 0 if OK
 */
int nbgl_layoutAddRadioChoice(nbgl_layout_t *layout, nbgl_layoutRadioChoice_t *choices) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;
  uint8_t i;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddRadioChoice():\n");
  if (layout == NULL)
    return -1;
  for (i=0;i<choices->nbChoices;i++) {
    nbgl_container_t *container;
    nbgl_text_area_t *textArea;
    nbgl_radio_t *button;
    nbgl_line_t *line;

    container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER,layoutInt->layer);
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    button = (nbgl_radio_t *)nbgl_objPoolGet(RADIO_BUTTON,layoutInt->layer);

    obj = addCallbackObj(layoutInt,(nbgl_obj_t*)container,choices->token,choices->tuneId);
    if (obj == NULL)
      return -1;

    // get container children (max 2)
    container->nbChildren = 2;
    container->children = nbgl_containerPoolGet(container->nbChildren,layoutInt->layer);

    // init text area for this choice
    if (choices->localized == true) {
#if defined(HAVE_LANGUAGE_PACK)
      textArea->localized = true;
      textArea->textId = choices->nameIds[i];
#endif // HAVE_LANGUAGE_PACK
    }
    else {
      textArea->text = PIC(choices->names[i]);
    }

    textArea->textAlignment = MID_LEFT;
    textArea->width = 300;
    textArea->height = 24;
    textArea->style = NO_STYLE;
    textArea->alignment = MID_LEFT;
    textArea->alignTo = (nbgl_obj_t*)container;
    container->children[0] = (nbgl_obj_t*)textArea;

    // init button for this choice
    button->activeColor = BLACK;
    button->borderColor = LIGHT_GRAY;
    button->alignmentMarginX = INNER_MARGIN;
    button->alignTo = (nbgl_obj_t*)container;
    button->alignment = MID_RIGHT;
    button->state = OFF_STATE;
    container->children[1] = (nbgl_obj_t*)button;

    container->width = SCREEN_WIDTH-2*BORDER_MARGIN;
    container->height = 32;
    container->alignment = NO_ALIGNMENT;
    container->alignmentMarginX = BORDER_MARGIN;
    container->alignmentMarginY = BORDER_MARGIN;
    container->alignTo = (nbgl_obj_t*)NULL;
    // whole container should be touchable
    container->touchMask = (1<<TOUCHED);
    container->touchCallback = (nbgl_touchCallback_t)&radioTouchCallback;

    // highlight init choice
    if (i == choices->initChoice) {
      button->state = ON_STATE;
      textArea->textColor = BLACK;
      textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
    }
    else {
      button->state = OFF_STATE;
      textArea->textColor = DARK_GRAY;
      textArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    }
    line = createHorizontalLine(layoutInt->layer);
    line->alignmentMarginY = BORDER_MARGIN;

    // set these new objs as child of main container
    addObjectToLayout(layoutInt,(nbgl_obj_t*)container);
    addObjectToLayout(layoutInt,(nbgl_obj_t*)line);
  }

  return 0;
}

/**
 * @brief Creates an area on the center of the main panel, with a possible icon/image,
 * a possible text in black under it, and a possible text in gray under it
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddCenteredInfo(nbgl_layout_t *layout, nbgl_layoutCenteredInfo_t *info) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_container_t *container;
  nbgl_text_area_t *textArea=NULL;
  nbgl_image_t *image=NULL;
  uint16_t fullHeight = 0;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddCenteredInfo():\n");
  if (layout == NULL)
    return -1;

  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER,layoutInt->layer);

  // get container children (max 5 if PLUGIN_INFO)
  container->children = nbgl_containerPoolGet((info->style == PLUGIN_INFO)?5:4,layoutInt->layer);
  container->nbChildren = 0;

  if (info->icon != NULL) {
    image = (nbgl_image_t *)nbgl_objPoolGet(IMAGE,layoutInt->layer);
    image->foregroundColor = BLACK;
    image->buffer = PIC(info->icon);
    image->bpp = NBGL_BPP_1;
    image->alignment = TOP_MIDDLE;
    image->alignTo = NULL;

    fullHeight += image->buffer->height;
    if ((info->style != PLUGIN_INFO)) {
      container->children[container->nbChildren] = (nbgl_obj_t*)image;
      container->nbChildren++;
    }
  }
  if (info->text1 != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    textArea->textColor = BLACK;
    textArea->text = PIC(info->text1);
    textArea->textAlignment = CENTER;
    if (info->style == LEDGER_INFO) {
      textArea->fontId = BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px;
    }
    else if ((info->style == LARGE_CASE_INFO) ||
             (info->style == LARGE_CASE_BOLD_INFO) ||
             (info->style == PLUGIN_INFO)) {
      textArea->fontId = BAGL_FONT_INTER_REGULAR_32px;
    }
    else {
      textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
    }
    textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
    textArea->height = nbgl_getTextHeightInWidth(textArea->fontId,textArea->text,textArea->width,false);

    if (info->style == LEDGER_INFO) {
      textArea->style = LEDGER_BORDER;
      textArea->width = SCREEN_WIDTH - 2*40;
      textArea->height += 2*16;
    }
    else
      textArea->style = NO_STYLE;
    if (container->nbChildren>0) {
      textArea->alignment = BOTTOM_MIDDLE;
      textArea->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
      textArea->alignmentMarginY = BORDER_MARGIN+8;
    }
    else {
      textArea->alignment = TOP_MIDDLE;
      textArea->alignTo = NULL;
    }

    fullHeight += textArea->height+textArea->alignmentMarginY;

    container->children[container->nbChildren] = (nbgl_obj_t*)textArea;
    container->nbChildren++;
  }
  if (info->text2 != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    if ((info->style != LARGE_CASE_INFO) &&
        (info->style != LARGE_CASE_BOLD_INFO) &&
        (info->style != PLUGIN_INFO)) {
      textArea->textColor = DARK_GRAY;
    }
    else {
      textArea->textColor = BLACK;
    }
    textArea->text = PIC(info->text2);
    textArea->textAlignment = CENTER;
    textArea->fontId = (info->style != LARGE_CASE_BOLD_INFO) ? BAGL_FONT_INTER_REGULAR_24px: BAGL_FONT_INTER_SEMIBOLD_24px;
    textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
    textArea->height = nbgl_getTextHeightInWidth(textArea->fontId,textArea->text,textArea->width,false);

    textArea->style = NO_STYLE;
    if (container->nbChildren>0) {
      textArea->alignment = BOTTOM_MIDDLE;
      textArea->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
      textArea->alignmentMarginY = BORDER_MARGIN+8;
    }
    else {
      textArea->alignment = TOP_MIDDLE;
    }

    fullHeight += textArea->height + textArea->alignmentMarginY;

    container->children[container->nbChildren] = (nbgl_obj_t*)textArea;
    container->nbChildren++;
  }
  // draw small horizontal line if PLUGIN_INFO
  if (info->style == PLUGIN_INFO) {
    nbgl_line_t *line = createHorizontalLine(layoutInt->layer);
    line->width = 120;
    line->alignmentMarginY = 32;
    line->alignmentMarginX = 0;
    line->alignment = BOTTOM_MIDDLE;
    line->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
    fullHeight += 32;

    container->children[container->nbChildren] = (nbgl_obj_t*)line;
    container->nbChildren++;
    if (image) {
      // add icon here, under line
      image->alignmentMarginY = 32;
      image->alignment = BOTTOM_MIDDLE;
      image->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
      container->children[container->nbChildren] = (nbgl_obj_t*)image;
      container->nbChildren++;
      fullHeight += 32;
    }
  }
  if (info->text3 != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    textArea->textColor = BLACK;
    textArea->text = PIC(info->text3);
    textArea->textAlignment = CENTER;
    textArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
    textArea->height = nbgl_getTextHeightInWidth(textArea->fontId,textArea->text,textArea->width,false);
    textArea->style = NO_STYLE;
    if (container->nbChildren>0) {
      textArea->alignment = BOTTOM_MIDDLE;
      textArea->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
      textArea->alignmentMarginY = BORDER_MARGIN;
    }
    else {
      textArea->alignment = TOP_MIDDLE;
      textArea->alignTo = NULL;
    }

    fullHeight += textArea->height+textArea->alignmentMarginY;

    container->children[container->nbChildren] = (nbgl_obj_t*)textArea;
    container->nbChildren++;
  }
  container->height = fullHeight;
  container->layout = VERTICAL;
  if (info->onTop) {
    container->alignmentMarginX = BORDER_MARGIN;
    container->alignmentMarginY = BORDER_MARGIN+ info->offsetY;
    container->alignment = NO_ALIGNMENT;
  }
  else {
    container->alignmentMarginY = info->offsetY;
    container->alignment = CENTER;
  }

  container->width = GET_AVAILABLE_WIDTH(layoutInt);

  // set this new container as child of main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  return 0;
}

#ifdef NBGL_QRCODE
/**
 * @brief Creates an area on the center of the main panel, with a QRCode,
 * a possible text in black (bold) under it, and a possible text in black under it
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddQRCode(nbgl_layout_t *layout, nbgl_layoutQRCode_t *info) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_container_t *container;
  nbgl_text_area_t *textArea=NULL;
  nbgl_qrcode_t *qrcode;
  uint16_t fullHeight = 0;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddQRCode():\n");
  if (layout == NULL)
    return -1;

  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER,layoutInt->layer);

  // get container children (max 4)
  container->children = nbgl_containerPoolGet(4,layoutInt->layer);
  container->nbChildren = 0;

  qrcode = (nbgl_qrcode_t *)nbgl_objPoolGet(QR_CODE,layoutInt->layer);
  // version is forced to V10 if url is longer than 62 characters
  if (strlen(PIC(info->url))>62) {
    qrcode->version = QRCODE_V10;
  }
  else {
    qrcode->version = QRCODE_V4;
  }
  qrcode->foregroundColor = BLACK;
  // in QR V4, we use 8*8 screen pixels for one QR pixel
  // in QR V10, we use 4*4 screen pixels for one QR pixel
  qrcode->width = (qrcode->version == QRCODE_V4)?(QR_V4_NB_PIX_SIZE*8):(QR_V10_NB_PIX_SIZE*4);
  qrcode->height = qrcode->width;
  qrcode->text = PIC(info->url);
  qrcode->bpp = NBGL_BPP_1;
  qrcode->alignment = TOP_MIDDLE;

  fullHeight += qrcode->height;
  container->children[container->nbChildren] = (nbgl_obj_t*)qrcode;
  container->nbChildren++;

  if (info->text1 != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    textArea->textColor = BLACK;
    textArea->text = PIC(info->text1);
    textArea->textAlignment = CENTER;
    textArea->fontId = (info->largeText1 == true)? BAGL_FONT_INTER_REGULAR_32px : BAGL_FONT_INTER_REGULAR_24px;
    textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
    textArea->height = nbgl_getTextHeightInWidth(textArea->fontId,textArea->text,textArea->width,false);
    textArea->style = NO_STYLE;
    textArea->alignment = BOTTOM_MIDDLE;
    textArea->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
    textArea->alignmentMarginY = 40;

    fullHeight += textArea->height;

    container->children[container->nbChildren] = (nbgl_obj_t*)textArea;
    container->nbChildren++;
  }
  else if (info->text2 != NULL) {
    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    textArea->textColor = DARK_GRAY;
    textArea->text = PIC(info->text2);
    textArea->textAlignment = CENTER;
    textArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
    textArea->height = nbgl_getTextHeightInWidth(textArea->fontId,textArea->text,textArea->width,false);
    textArea->style = NO_STYLE;
    textArea->alignment = BOTTOM_MIDDLE;
    textArea->alignTo = (nbgl_obj_t*)container->children[container->nbChildren-1];
    textArea->alignmentMarginY = 40;

    fullHeight += textArea->height;

    container->children[container->nbChildren] = (nbgl_obj_t*)textArea;
    container->nbChildren++;
  }
  container->height = fullHeight;
  container->layout = VERTICAL;
  container->alignment = CENTER;

  container->width = GET_AVAILABLE_WIDTH(layoutInt);

  // set this new container as child of main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  return 0;
}
#endif // NBGL_QRCODE

/**
 * @brief Creates two buttons to make a choice. Both buttons are mandatory
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddChoiceButtons(nbgl_layout_t *layout, nbgl_layoutChoiceButtons_t *info) {
  layoutObj_t *obj;
  nbgl_button_t *topButton, *bottomButton;
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddChoiceButtons():\n");
  if (layout == NULL)
    return -1;

  // texts cannot be NULL
  if ((info->bottomText == NULL) || (info->topText == NULL))
    return -1;

  // create bottomButton (in white) at first
  bottomButton = (nbgl_button_t *)nbgl_objPoolGet(BUTTON,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)bottomButton,info->token,info->tuneId);
  if (obj == NULL)
    return -1;
  // associate with with index 1
  obj->index = 1;
  bottomButton->alignment = BOTTOM_MIDDLE;
  if (info->style == ROUNDED_AND_FOOTER_STYLE) {
    bottomButton->alignmentMarginY = 4; // 4 pixels from screen bottom
    bottomButton->borderColor = WHITE;
  }
  else if (info->style == BOTH_ROUNDED_STYLE) {
    bottomButton->alignmentMarginY = BORDER_MARGIN; // 24 pixels from screen bottom
    bottomButton->borderColor = LIGHT_GRAY;
  }
  bottomButton->innerColor = WHITE;
  bottomButton->foregroundColor = BLACK;
  bottomButton->width = GET_AVAILABLE_WIDTH(layoutInt);
  bottomButton->height = BUTTON_DIAMETER;
  bottomButton->radius = BUTTON_RADIUS;
  bottomButton->text = PIC(info->bottomText);
  bottomButton->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  bottomButton->touchMask = (1 << TOUCHED);
  bottomButton->touchCallback = (nbgl_touchCallback_t)touchCallback;
  // set this new button as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)bottomButton);

  // then black button, on top of it
  topButton = (nbgl_button_t *)nbgl_objPoolGet(BUTTON,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)topButton,info->token,info->tuneId);
  if (obj == NULL)
    return -1;
  // associate with with index 0
  obj->index = 0;
  topButton->alignment = TOP_MIDDLE;
  topButton->alignTo = (nbgl_obj_t*)bottomButton;
  if (info->style == BOTH_ROUNDED_STYLE) {
    topButton->alignmentMarginY = INNER_MARGIN; // 12 pixels from bottom button
  }
  else {
    topButton->alignmentMarginY = 4; // 4 pixels from bottom button
  }
  topButton->innerColor = BLACK;
  topButton->borderColor = BLACK;
  topButton->foregroundColor = WHITE;
  topButton->width = bottomButton->width;
  topButton->height = BUTTON_DIAMETER;
  topButton->radius = BUTTON_RADIUS;
  topButton->text = PIC(info->topText);
  topButton->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  topButton->touchMask = (1 << TOUCHED);
  topButton->touchCallback = (nbgl_touchCallback_t)touchCallback;
  // set this new button as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)topButton);

  return 0;
}

/**
 * @brief Creates a list of [tag,value] pairs
 *
 * @param layout the current layout
 * @param list structure giving the list of [tag,value] pairs
 * @return >= 0 if OK
 */
int nbgl_layoutAddTagValueList(nbgl_layout_t *layout, nbgl_layoutTagValueList_t *list) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_text_area_t *itemTextArea;
  nbgl_text_area_t *valueTextArea;
  nbgl_container_t *container;
  uint8_t i;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddTagValueList():\n");
  if (layout == NULL)
    return -1;

  for (i=0;i<list->nbPairs;i++) {
    nbgl_layoutTagValue_t *pair;
    uint16_t fullHeight = 0, usableWidth;

    if (list->pairs != NULL) {
      pair = &list->pairs[i];
    }
    else {
      pair = list->callback(list->startIndex+i);
    }
    // width that can be used for item and text
    usableWidth = GET_AVAILABLE_WIDTH(layoutInt);

    container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER,layoutInt->layer);

    // get container children
    container->children = nbgl_containerPoolGet(2,layoutInt->layer);

    itemTextArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
    valueTextArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);

    // init text area for this choice
    itemTextArea->textColor = DARK_GRAY;
    itemTextArea->text = PIC(pair->item);
    itemTextArea->textAlignment = MID_LEFT;
    itemTextArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    itemTextArea->width = usableWidth;
    itemTextArea->height = nbgl_getTextHeightInWidth(itemTextArea->fontId,itemTextArea->text,usableWidth,false);
    itemTextArea->style = NO_STYLE;
    itemTextArea->alignment = NO_ALIGNMENT;
    itemTextArea->alignmentMarginX = 0;
    itemTextArea->alignmentMarginY = 0;
    itemTextArea->alignTo = NULL;
    container->children[container->nbChildren] = (nbgl_obj_t*)itemTextArea;
    container->nbChildren++;

    fullHeight += itemTextArea->height;

    // init button for this choice
    valueTextArea->textColor = BLACK;
    valueTextArea->text = PIC(pair->value);
    valueTextArea->textAlignment = MID_LEFT;
    if (list->smallCaseForValue) {
      valueTextArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    } else {
      valueTextArea->fontId = BAGL_FONT_INTER_REGULAR_32px;
    }
    valueTextArea->width = usableWidth;
    // handle the nbMaxLinesForValue parameter, used to automatically keep only
    // nbMaxLinesForValue lines
    uint16_t nbLines = nbgl_getTextNbLinesInWidth(valueTextArea->fontId,valueTextArea->text,usableWidth,list->wrapping);
    // use this nbMaxLinesForValue parameter only if >0
    if ((list->nbMaxLinesForValue > 0) && (nbLines > list->nbMaxLinesForValue)) {
      nbLines = list->nbMaxLinesForValue;
      valueTextArea->nbMaxLines = list->nbMaxLinesForValue;
    }
    const nbgl_font_t *font = nbgl_getFont(valueTextArea->fontId);
    valueTextArea->height = nbLines*font->line_height;
    valueTextArea->style = NO_STYLE;
    valueTextArea->alignment = BOTTOM_LEFT;
    valueTextArea->alignmentMarginY = 4;
    valueTextArea->alignTo = (nbgl_obj_t*)itemTextArea;
    valueTextArea->wrapping = list->wrapping;
    container->children[container->nbChildren] = (nbgl_obj_t*)valueTextArea;
    container->nbChildren++;

    fullHeight += valueTextArea->height;

    container->width = GET_AVAILABLE_WIDTH(layoutInt);
    container->height = fullHeight;
    container->layout = VERTICAL;
    container->alignmentMarginX = BORDER_MARGIN;
    container->alignmentMarginY = 12;
    container->alignment = NO_ALIGNMENT;

    addObjectToLayout(layoutInt,(nbgl_obj_t*)container);
  }

  return 0;
}

/**
 * @brief Creates an area in main panel to display a progress bar, with a title text and a description under the progress
 *
 * @param layout the current layout
 * @param barLayout structure giving the description of progress bar
 * @return >= 0 if OK
 */
int nbgl_layoutAddProgressBar(nbgl_layout_t *layout, nbgl_layoutProgressBar_t *barLayout) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_progress_bar_t *progress;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddProgressBar():\n");
  if (layout == NULL)
    return -1;
  if (barLayout->text != NULL) {
    nbgl_text_area_t *textArea;

    textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,((nbgl_layoutInternal_t *)layout)->layer);
    textArea->textColor=BLACK;
    textArea->text = PIC(barLayout->text);
    textArea->textAlignment = MID_LEFT;
    textArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    textArea->width = GET_AVAILABLE_WIDTH(((nbgl_layoutInternal_t *)layout));
    textArea->height = nbgl_getTextHeight(textArea->fontId,textArea->text);
    textArea->style = NO_STYLE;
    textArea->alignment = NO_ALIGNMENT;
    textArea->alignmentMarginX = BORDER_MARGIN;
    textArea->alignmentMarginY = BORDER_MARGIN;
    addObjectToLayout(layoutInt,(nbgl_obj_t*)textArea);
  }
  progress = (nbgl_progress_bar_t *)nbgl_objPoolGet(PROGRESS_BAR,((nbgl_layoutInternal_t *)layout)->layer);
  progress->foregroundColor = BLACK;
  progress->withBorder = true;
  progress->state = barLayout->percentage;
  progress->width = 120;
  progress->height = 12;
  progress->alignment = NO_ALIGNMENT;
  progress->alignmentMarginX = (GET_AVAILABLE_WIDTH(((nbgl_layoutInternal_t *)layout))-progress->width)/2;
  progress->alignmentMarginY = BORDER_MARGIN;
  addObjectToLayout(layoutInt,(nbgl_obj_t*)progress);

  if (barLayout->subText != NULL) {
    nbgl_text_area_t *subTextArea;

    subTextArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,((nbgl_layoutInternal_t *)layout)->layer);
    subTextArea->textColor=LIGHT_GRAY;
    subTextArea->text = PIC(barLayout->subText);
    subTextArea->textAlignment = MID_LEFT;
    subTextArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
    subTextArea->width = GET_AVAILABLE_WIDTH(((nbgl_layoutInternal_t *)layout));
    subTextArea->height = nbgl_getTextHeight(subTextArea->fontId,subTextArea->text);
    subTextArea->style = NO_STYLE;
    subTextArea->alignment = NO_ALIGNMENT;
    subTextArea->alignmentMarginX = BORDER_MARGIN;
    subTextArea->alignmentMarginY = BORDER_MARGIN;
    addObjectToLayout(layoutInt,(nbgl_obj_t*)subTextArea);
  }

  return 0;

}

/**
 * @brief adds a separation line on bottom of the last added item
 *
 * @param layout the current layout
 * @return >= 0 if OK
 */
int nbgl_layoutAddSeparationLine(nbgl_layout_t *layout) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_line_t *line;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddSeparationLine():\n");
  line = createHorizontalLine(layoutInt->layer);
  line->alignmentMarginY = -4;
  addObjectToLayout(layoutInt,(nbgl_obj_t*)line);
  return 0;
}

/**
 * @brief Creates a rounded button in the main container.
 *
 * @param layout the current layout
 * @param buttonInfo structure giving the description of button (text, icon, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddButton(nbgl_layout_t *layout, nbgl_layoutButton_t *buttonInfo) {
  layoutObj_t *obj;
  nbgl_button_t *button;
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddButton():\n");
  if (layout == NULL)
    return -1;

  button = (nbgl_button_t *)nbgl_objPoolGet(BUTTON,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)button,buttonInfo->token,buttonInfo->tuneId);
  if (obj == NULL)
    return -1;

  button->alignmentMarginY = BORDER_MARGIN;
  if (buttonInfo->onBottom != true) {
    button->alignmentMarginX = BORDER_MARGIN;
    button->alignment = NO_ALIGNMENT;
  }
  else {
    button->alignment = BOTTOM_MIDDLE;
  }
  if (buttonInfo->style == BLACK_BACKGROUND) {
    button->innerColor = BLACK;
    button->foregroundColor = WHITE;
  }
  else {
    button->innerColor = WHITE;
    button->foregroundColor = BLACK;
  }
  if (buttonInfo->style == NO_BORDER) {
    button->borderColor = WHITE;
  }
  else {
    if (buttonInfo->style == BLACK_BACKGROUND) {
      button->borderColor = BLACK;
    }
    else {
      button->borderColor = LIGHT_GRAY;
    }
  }
  button->text = PIC(buttonInfo->text);
  button->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  button->icon = PIC(buttonInfo->icon);
  if (buttonInfo->fittingContent == true) {
    button->width = nbgl_getTextWidth(button->fontId,button->text)+64+((button->icon)?(button->icon->width+8):0);
    button->height = 64;
    button->radius = RADIUS_32_PIXELS;
    if (buttonInfo->onBottom != true)
      button->alignmentMarginX += (SCREEN_WIDTH-2*BORDER_MARGIN-button->width)/2;
  }
  else {
    button->width = GET_AVAILABLE_WIDTH(layoutInt);
    button->height = BUTTON_DIAMETER;
    button->radius = BUTTON_RADIUS;
  }
  button->alignTo = NULL;
  button->touchMask = (1 << TOUCHED);
  button->touchCallback = (nbgl_touchCallback_t)touchCallback;
  // set this new button as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)button);

  return 0;
}

/**
 * @brief Creates a long press button in the main container.
 *
 * @param layout the current layout
 * @param text text of the button button
 * @param token token attached to actionCallback when long time of press is elapsed
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is long pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddLongPressButton(nbgl_layout_t *layout, char *text, uint8_t token, tune_index_e tuneId) {
  layoutObj_t *obj;
  nbgl_button_t *button;
  nbgl_text_area_t *textArea;
  nbgl_progress_bar_t *progressBar;
  nbgl_container_t *container;
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_line_t *line;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddLongPressButton():\n");
  if (layout == NULL)
    return -1;

  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER, layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)container,token,tuneId);
  if (obj == NULL)
    return -1;

  container->width = SCREEN_WIDTH;
  container->height = 128;
  container->layout = VERTICAL ;
  container->nbChildren = 4; // progress-bar + text + line + button
  container->children = (nbgl_obj_t**)nbgl_containerPoolGet(container->nbChildren,layoutInt->layer);
  container->alignment = BOTTOM_MIDDLE;
  container->touchMask = ((1<<TOUCHING)|(1<<TOUCH_RELEASED)|(1<<OUT_OF_TOUCH));
  container->touchCallback = (nbgl_touchCallback_t)longTouchCallback;

  button = (nbgl_button_t *)nbgl_objPoolGet(BUTTON,layoutInt->layer);
  button->alignmentMarginX = BORDER_MARGIN;
  button->alignment = MID_RIGHT;
  button->innerColor = BLACK;
  button->foregroundColor = WHITE;
  button->borderColor = BLACK;
  button->width = BUTTON_DIAMETER;
  button->height = BUTTON_DIAMETER;
  button->radius = BUTTON_RADIUS;
  button->icon = PIC(&C_check32px);
  container->children[0] = (nbgl_obj_t*)button;

  textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
  textArea->textColor = BLACK;
  textArea->text = PIC(text);
  textArea->textAlignment = MID_LEFT;
  textArea->fontId = BAGL_FONT_INTER_REGULAR_32px;
  textArea->width = container->width - 3 * BORDER_MARGIN - button->width;
  textArea->height = nbgl_getTextHeight(textArea->fontId,textArea->text);
  textArea->style = NO_STYLE;
  textArea->alignment = MID_LEFT;
  textArea->alignmentMarginX = BORDER_MARGIN;
  container->children[1] = (nbgl_obj_t*)textArea;

  line = createHorizontalLine(layoutInt->layer);
  line->offset = 3;
  line->alignment = TOP_MIDDLE;
  container->children[2] = (nbgl_obj_t*)line;

  progressBar = (nbgl_progress_bar_t *)nbgl_objPoolGet(PROGRESS_BAR,layoutInt->layer);
  progressBar->withBorder = false;
  progressBar->width = container->width;
  progressBar->height = 8;
  progressBar->alignment = TOP_MIDDLE;
  progressBar->alignmentMarginY = 4;
  progressBar->alignTo = NULL;
  container->children[3] = (nbgl_obj_t*)progressBar;

  // set this new container as child of the main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  return 0;
}

/**
 * @brief Creates a touchable text at the footer of the screen, separated with a thin line from the rest of the screen.
 *
 * @param layout the current layout
 * @param text text to used in the footer
 * @param token token to use when the footer is touched
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is long pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddFooter(nbgl_layout_t *layout, char *text, uint8_t token, tune_index_e tuneId) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;
  nbgl_text_area_t *textArea;
  nbgl_line_t *line;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddFooter():\n");
  if (layout == NULL)
    return -1;

  textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)textArea,token,tuneId);
  if (obj == NULL)
    return -1;

  textArea->alignment = BOTTOM_MIDDLE;
  textArea->textColor = BLACK;
  textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
  textArea->height = BUTTON_DIAMETER;
  textArea->text = PIC(text);
  textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  textArea->textAlignment = CENTER;
  textArea->touchMask = (1 << TOUCHED);
  textArea->touchCallback = (nbgl_touchCallback_t)touchCallback;
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)textArea;
  layoutInt->nbChildren++;

  line = createHorizontalLine(layoutInt->layer);
  line->alignTo = (nbgl_obj_t*)textArea;
  line->alignment = TOP_MIDDLE;
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)line;
  layoutInt->nbChildren++;

  layoutInt->container->height -= textArea->height+line->height;

  return 0;
}

/**
 * @brief Creates 2 touchable texts at the footer of the screen, separated with a thin line from the rest of the screen, and from each other.
 *
 * @param layout the current layout
 * @param leftText text to used in the left part of footer
 * @param leftToken token to use when the left part of footer is touched
 * @param rightText text to used in the right part of footer
 * @param rightToken token to use when the right part of footer is touched
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when button is long pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddSplitFooter(nbgl_layout_t *layout, char *leftText, uint8_t leftToken, char *rightText, uint8_t rightToken, tune_index_e tuneId) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  layoutObj_t *obj;
  nbgl_text_area_t *textArea;
  nbgl_line_t *line;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddSplitFooter():\n");
  if (layout == NULL)
    return -1;

  // create left touchable text
  textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)textArea,leftToken,tuneId);
  if (obj == NULL)
    return -1;

  textArea->alignment = BOTTOM_LEFT;
  textArea->textColor = BLACK;
  textArea->width = GET_AVAILABLE_WIDTH(layoutInt)/2;
  textArea->height = BUTTON_DIAMETER;
  textArea->text = PIC(leftText);
  textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  textArea->textAlignment = CENTER;
  textArea->touchMask = (1 << TOUCHED);
  textArea->touchCallback = (nbgl_touchCallback_t)touchCallback;
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)textArea;
  layoutInt->nbChildren++;

  // create right touchable text
  textArea = (nbgl_text_area_t *)nbgl_objPoolGet(TEXT_AREA,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)textArea,rightToken,tuneId);
  if (obj == NULL)
    return -1;

  textArea->alignment = BOTTOM_RIGHT;
  textArea->textColor = BLACK;
  textArea->width = GET_AVAILABLE_WIDTH(layoutInt)/2;
  textArea->height = BUTTON_DIAMETER;
  textArea->text = PIC(rightText);
  textArea->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  textArea->textAlignment = CENTER;
  textArea->touchMask = (1 << TOUCHED);
  textArea->touchCallback = (nbgl_touchCallback_t)touchCallback;
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)textArea;
  layoutInt->nbChildren++;

  // create horizontal line separating footer from main container
  line = createHorizontalLine(layoutInt->layer);
  line->alignTo = layoutInt->children[layoutInt->nbChildren-2];
  line->alignment = TOP_LEFT;
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)line;
  layoutInt->nbChildren++;

  // create vertical line separating both text areas
  line = (nbgl_line_t*)nbgl_objPoolGet(LINE,layoutInt->layer);
  line->lineColor = LIGHT_GRAY;
  line->width = 1;
  line->height = textArea->height+4;
  line->direction = VERTICAL;
  line->thickness = 1;
  line->alignment = BOTTOM_MIDDLE;
  layoutInt->children[layoutInt->nbChildren] = (nbgl_obj_t*)line;
  layoutInt->nbChildren++;

  layoutInt->container->height -= textArea->height+4;

  return 0;
}

/**
 * @brief Creates a kind of navigation bar with an optional <- arrow on the left. This widget is placed on top of the
 * main container
 *
 * @param layout the current layout
 * @param activePage current page [O,(nbPages-1)]
 * @param nbPages number of pages
 * @param withBack if true, the back arrow is drawn
 * @param backToken token used with actionCallback is withBack is true
 * @param tuneId if not @ref NBGL_NO_TUNE, a tune will be played when back button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddProgressIndicator(nbgl_layout_t *layout, uint8_t activePage, uint8_t nbPages, bool withBack, uint8_t backToken, tune_index_e tuneId) {
  layoutObj_t *obj;
  nbgl_container_t *container;
  nbgl_button_t *button;
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddProgressIndicator():\n");
  if (layout == NULL)
    return -1;

  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER, layoutInt->layer);
  container->width = SCREEN_WIDTH;
  container->height = BUTTON_DIAMETER+8;
  container->layout = VERTICAL;
  container->nbChildren = 2;
  container->children = (nbgl_obj_t**)nbgl_containerPoolGet(container->nbChildren,layoutInt->layer);
  container->alignment = TOP_MIDDLE;
  container->alignTo = NULL;

  if (nbPages > 1) {
    nbgl_page_indicator_t *navigationBar;

    navigationBar = (nbgl_page_indicator_t *)nbgl_objPoolGet(PAGE_INDICATOR,layoutInt->layer);
    navigationBar->activePage = activePage;
    navigationBar->nbPages = nbPages;
    navigationBar->width = SCREEN_WIDTH-2*100;
    navigationBar->alignment = CENTER;
    container->children[0] = (nbgl_obj_t*)navigationBar;
  }

  if ((withBack== true) && ((nbPages < 2)||(activePage > 0))) {
    button = (nbgl_button_t *)nbgl_objPoolGet(BUTTON,layoutInt->layer);
    obj = addCallbackObj(layoutInt,(nbgl_obj_t*)button,backToken,tuneId);
    if (obj == NULL)
      return -1;

    button->alignment = MID_LEFT;
    button->innerColor = WHITE;
    button->foregroundColor = BLACK;
    button->borderColor = WHITE;
    button->width = BUTTON_DIAMETER;
    button->height = BUTTON_DIAMETER;
    button->radius = BUTTON_RADIUS;
    button->text = NULL;
    button->icon = PIC(&C_leftArrow32px);
    button->touchMask = (1<<TOUCHED);
    button->touchCallback = (nbgl_touchCallback_t)touchCallback;
    container->children[1] = (nbgl_obj_t*)button;
  }

  // set this new container as child of the main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  return 0;
}

/**
 * @brief Creates a centered (vertically & horizontally) spinner with a text under it
 *
 * @param layout the current layout
 * @param text text to draw under the spinner
 * @param fixed if set to true, the spinner won't spin and be entirely black
 * @return >= 0 if OK
 */
int nbgl_layoutAddSpinner(nbgl_layout_t *layout, char *text, bool fixed) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_text_area_t *textArea;
  nbgl_spinner_t *spinner;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddSpinner():\n");
  if (layout == NULL)
    return -1;

  // create spinner
  spinner = (nbgl_spinner_t*)nbgl_objPoolGet(SPINNER, layoutInt->layer);
  spinner->position = fixed? 0xFF : 0;
  spinner->alignmentMarginY = -20;
  spinner->alignTo = NULL;
  spinner->alignment = CENTER;
  // set this new spinner as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)spinner);

  // create text area
  textArea = (nbgl_text_area_t*)nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
  textArea->textColor = BLACK;
  textArea->text = (char*)PIC(text);
  textArea->textAlignment = CENTER;
  textArea->fontId = BAGL_FONT_INTER_REGULAR_24px;
  textArea->alignmentMarginY = 36;
  textArea->alignTo = (nbgl_obj_t*)spinner;
  textArea->alignment = BOTTOM_MIDDLE;
  textArea->width = GET_AVAILABLE_WIDTH(layoutInt);
  textArea->height = nbgl_getFontLineHeight(textArea->fontId);
  textArea->style = NO_STYLE;

  // set this new spinner as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)textArea);

  if (!fixed) {
    // update ticker to update the spinner periodically
    nbgl_screenTickerConfiguration_t tickerCfg;

    tickerCfg.tickerIntervale = SPINNER_REFRESH_PERIOD; //ms
    tickerCfg.tickerValue = SPINNER_REFRESH_PERIOD; //ms
    tickerCfg.tickerCallback = &spinnerTickerCallback;
    nbgl_screenUpdateTicker(layoutInt->layer,&tickerCfg);
  }

  return 0;
}

#ifdef NBGL_KEYBOARD
/**
 * @brief Creates a keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param kbdInfo configuration of the keyboard to draw (including the callback when touched)
 * @return the index of keyboard, to use in @ref nbgl_layoutUpdateKeyboard()
 */
int nbgl_layoutAddKeyboard(nbgl_layout_t *layout, nbgl_layoutKbd_t *kbdInfo) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_keyboard_t *keyboard;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddKeyboard():\n");
  if (layout == NULL)
    return -1;

  // create keyboard
  keyboard = (nbgl_keyboard_t*)nbgl_objPoolGet(KEYBOARD, layoutInt->layer);
  keyboard->alignmentMarginY = 60;
  keyboard->alignment = BOTTOM_MIDDLE;
  keyboard->borderColor = LIGHT_GRAY;
  keyboard->callback = PIC(kbdInfo->callback);
  keyboard->lettersOnly = kbdInfo->lettersOnly;
  keyboard->mode = kbdInfo->mode;
  keyboard->keyMask = kbdInfo->keyMask;
  keyboard->casing = kbdInfo->casing;
  // set this new keyboard as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)keyboard);

  // return index of keyboard to be modified later on
  return (layoutInt->container->nbChildren-1);
}

/**
 * @brief Updates an existing keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeyboard()
 * @param keyMask mask of keys to activate/deactivate on keyboard
 * @param updateCasing if true, update keyboard casing with given value
 * @param casing  casing to use
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeyboard(nbgl_layout_t *layout, uint8_t index, uint32_t keyMask, bool updateCasing, keyboardCase_t casing) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_keyboard_t *keyboard;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutUpdateKeyboard(): keyMask = 0x%X\n",keyMask);
  if (layout == NULL)
    return -1;

  // get keyboard at given index
  keyboard = (nbgl_keyboard_t*)layoutInt->container->children[index];
  if ((keyboard == NULL) || (keyboard->type != KEYBOARD)) {
    return -1;
  }
  keyboard->keyMask = keyMask;
  if (updateCasing) {
    keyboard->casing  = casing;
  }

  nbgl_redrawObject((nbgl_obj_t*)keyboard,NULL,false);

  return 0;
}

/**
 * @brief Adds up to 4 black suggestion buttons under the previously added object
 *
 * @param layout the current layout
 * @param nbUsedButtons the number of actually used buttons
 * @param buttonTexts array of 4 strings for buttons (last ones can be NULL)
 * @param firstButtonToken first token used for buttons, provided in onActionCallback (the next 3 values will be used for other buttons)
 * @param tuneId tune to play when any button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddSuggestionButtons(nbgl_layout_t *layout, uint8_t nbUsedButtons,
                                    char *buttonTexts[NB_MAX_SUGGESTION_BUTTONS],
                                    int firstButtonToken, tune_index_e tuneId) {
  layoutObj_t *obj;
  nbgl_container_t *container;
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddSuggestionButtons():\n");
  if (layout == NULL)
    return -1;

  container = (nbgl_container_t *)nbgl_objPoolGet(CONTAINER, layoutInt->layer);
  container->width = SCREEN_WIDTH;
  container->height = 2*64+8;
  container->layout = VERTICAL;
  container->nbChildren = NB_MAX_SUGGESTION_BUTTONS;
  container->children = (nbgl_obj_t**)nbgl_containerPoolGet(container->nbChildren,layoutInt->layer);
  container->alignmentMarginY = BORDER_MARGIN;
  container->alignment = TOP_MIDDLE;
  container->alignTo = layoutInt->container->children[layoutInt->container->nbChildren-1];

  // create suggestion buttons, even if not displayed at first
  nbgl_objPoolGetArray(BUTTON,NB_MAX_SUGGESTION_BUTTONS,0,(nbgl_obj_t**)&choiceButtons);
  for (int i=0;i<NB_MAX_SUGGESTION_BUTTONS;i++) {
    obj = addCallbackObj(layoutInt,(nbgl_obj_t*)choiceButtons[i],firstButtonToken+i,tuneId);
    if (obj == NULL)
      return -1;

    choiceButtons[i]->innerColor = BLACK;
    choiceButtons[i]->borderColor = BLACK;
    choiceButtons[i]->foregroundColor = WHITE;
    choiceButtons[i]->width = (SCREEN_WIDTH-2*BORDER_MARGIN-8)/2;
    choiceButtons[i]->height = 64;
    choiceButtons[i]->radius = RADIUS_32_PIXELS;
    choiceButtons[i]->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
    choiceButtons[i]->icon = NULL;
    if ((i%2) == 0) {
      choiceButtons[i]->alignmentMarginX = BORDER_MARGIN;
      if (i != 0)
        choiceButtons[i]->alignmentMarginY = 8;
      choiceButtons[i]->alignment = NO_ALIGNMENT;
    }
    else {
      choiceButtons[i]->alignmentMarginX = 8;
      choiceButtons[i]->alignment = MID_RIGHT;
      choiceButtons[i]->alignTo = (nbgl_obj_t*)choiceButtons[i-1];
    }
    choiceButtons[i]->text = buttonTexts[i];
    choiceButtons[i]->touchMask = (1<<TOUCHED);
    choiceButtons[i]->touchCallback = (nbgl_touchCallback_t)&touchCallback;
    // some buttons may not be visible
    if (i<nbUsedButtons)
      container->children[i] = (nbgl_obj_t*)choiceButtons[i];
    else
      container->children[i] = NULL;
  }
  // set this new container as child of the main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)container);

  // return index of container to be modified later on
  return (layoutInt->container->nbChildren-1);
}

/**
 * @brief Updates the number and/or the text suggestion buttons created with @ref nbgl_layoutAddSuggestionButtons()
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddSuggestionButtons()
 * @param nbUsedButtons the number of actually used buttons
 * @param buttonTexts array of 4 strings for buttons (last ones can be NULL)
 * @return >= 0 if OK
 */
int nbgl_layoutUpdateSuggestionButtons(nbgl_layout_t *layout, uint8_t index, uint8_t nbUsedButtons,
                                    char *buttonTexts[NB_MAX_SUGGESTION_BUTTONS]) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_container_t *container;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutUpdateSuggestionButtons():\n");
  if (layout == NULL)
    return -1;

  container = (nbgl_container_t *)layoutInt->container->children[index];
  if ((container == NULL) || (container->type != CONTAINER)) {
    return -1;
  }

  // update suggestion buttons
  for (int i=0;i<NB_MAX_SUGGESTION_BUTTONS;i++) {
    choiceButtons[i]->text = buttonTexts[i];
    // some buttons may not be visible
    if (i<nbUsedButtons)
      container->children[i] = (nbgl_obj_t*)choiceButtons[i];
    else
      container->children[i] = NULL;
  }
  container->forceClean = true;

  nbgl_redrawObject((nbgl_obj_t*)container,NULL,false);

  return 0;
}

/**
 * @brief Adds a "text entry" area under the previously entered object. This area can be preceded (beginning of line) by
 *        an index, indicating for example the entered world.
 *        A vertical gray line is placed under the text.
 *        This text must be vertical placed in the screen with offsetY
 *
 * @param layout the current layout
 * @param numbered if true, the "number" param is used as index
 * @param number index of the text
 * @param text string to display in the area
 * @param grayedOut if true, the text is grayed out (but not the potential number)
 * @param offsetY vertical offset from the top of the page
 * @return >= 0 if OK
 */
int nbgl_layoutAddEnteredText(nbgl_layout_t *layout, bool numbered, uint8_t number, char *text, bool grayedOut, int offsetY) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_text_area_t *textArea;
  nbgl_line_t *line;
  static char numText[5];

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddEnteredText():\n");
  if (layout == NULL)
    return -1;

  // create gray line
  line = (nbgl_line_t*)nbgl_objPoolGet(LINE, layoutInt->layer);
  line->lineColor = LIGHT_GRAY;
  line->alignmentMarginY = offsetY;
  line->alignTo = layoutInt->container->children[layoutInt->container->nbChildren-1];
  line->alignment = TOP_MIDDLE;
  line->width = SCREEN_WIDTH-2*44;
  line->height = 4;
  line->direction = HORIZONTAL;
  line->thickness = 2;
  line->offset = 2;
  // set this new line as child of the main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)line);

  if (numbered) {
    // create Word num typed text
    textArea = (nbgl_text_area_t*)nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor = BLACK;
    snprintf(numText,sizeof(numText),"%d.",number);
    textArea->text = numText;
    textArea->textAlignment = MID_LEFT;
    textArea->fontId = BAGL_FONT_INTER_REGULAR_32px;
    textArea->alignmentMarginY = 12;
    textArea->alignTo = (nbgl_obj_t*)line;
    textArea->alignment = TOP_LEFT;
    textArea->width = 50;
    textArea->height = nbgl_getFontLineHeight(textArea->fontId);
    // set this new text area as child of the main container
    addObjectToLayout(layoutInt,(nbgl_obj_t*)textArea);
  }

  // create text area
  textArea = (nbgl_text_area_t*)nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
  textArea->textColor = grayedOut ? LIGHT_GRAY:BLACK;
  textArea->text = text;
  textArea->textAlignment = CENTER;
  textArea->fontId = BAGL_FONT_INTER_REGULAR_32px;
  textArea->alignmentMarginY = 12;
  textArea->alignTo = (nbgl_obj_t*)line;
  textArea->alignment = TOP_MIDDLE;
  if (numbered) {
    textArea->width = line->width - 2*50;
  }
  else {
    textArea->width = SCREEN_WIDTH-2*BORDER_MARGIN;
  }
  textArea->height = nbgl_getFontLineHeight(textArea->fontId);
  textArea->autoHideLongLine = true;
  // set this new text area as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)textArea);

  // return index of text area to be modified later on
  return (layoutInt->container->nbChildren-1);

}

/**
 * @brief Updates an existing "text entry" area, created with @ref nbgl_layoutAddEnteredText()
 *
 * @param layout the current layout
 * @param index index of the text (return value of @ref nbgl_layoutAddEnteredText())
 * @param numbered if set to true, the text is preceded on the left by 'number.'
 * @param number if numbered is true, number used to build 'number.' text
 * @param text string to display in the area
 * @param grayedOut if true, the text is grayed out (but not the potential number)
 * @return >= 0 if OK
 */
int nbgl_layoutUpdateEnteredText(nbgl_layout_t *layout, uint8_t index, bool numbered, uint8_t number, char *text, bool grayedOut) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_text_area_t *textArea;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutUpdateEnteredText():\n");
  if (layout == NULL)
    return -1;

  // update main text area
  textArea = (nbgl_text_area_t*)layoutInt->container->children[index];
  if ((textArea == NULL) || (textArea->type != TEXT_AREA)) {
    return -1;
  }
  textArea->text = text;
  textArea->textColor = grayedOut ? LIGHT_GRAY:BLACK;
  nbgl_redrawObject((nbgl_obj_t*)textArea,NULL,false);

  // update number text area
  if (numbered) {
    // it is the previously created object
    textArea = (nbgl_text_area_t*)layoutInt->container->children[index-1];
    snprintf(textArea->text,5,"%d.",number);
    nbgl_redrawObject((nbgl_obj_t*)textArea,NULL,false);
  }

  return 0;
}

/**
 * @brief Adds a black full width confirmation button on top of the previously added keyboard.
 *
 * @param layout the current layout
 * @param active if true, button is active, otherwise inactive (grayed-out)
 * @param text text of the button
 * @param token token of the button, used in onActionCallback
 * @param tuneId tune to play when button is pressed
 * @return >= 0 if OK
 */
int nbgl_layoutAddConfirmationButton(nbgl_layout_t *layout, bool active, char *text, int token, tune_index_e tuneId) {
  layoutObj_t *obj;
  nbgl_button_t *button;
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddConfirmationButton():\n");
  if (layout == NULL)
    return -1;

  button = (nbgl_button_t *)nbgl_objPoolGet(BUTTON,layoutInt->layer);
  obj = addCallbackObj(layoutInt,(nbgl_obj_t*)button,token,tuneId);
  if (obj == NULL)
    return -1;

  button->alignmentMarginY = BORDER_MARGIN+4;
  button->alignment = TOP_MIDDLE;
  button->foregroundColor = WHITE;
  if (active) {
    button->innerColor = BLACK;
    button->borderColor = BLACK;
    button->touchMask = (1 << TOUCHED);
  }
  else {
    button->borderColor = DARK_GRAY;
    button->innerColor = DARK_GRAY;
  }
  button->text = PIC(text);
  button->fontId = BAGL_FONT_INTER_SEMIBOLD_24px;
  button->width = GET_AVAILABLE_WIDTH(layoutInt);
  button->height = BUTTON_DIAMETER;
  button->radius = BUTTON_RADIUS;
  button->alignTo = layoutInt->container->children[layoutInt->container->nbChildren-1];
  button->touchCallback = (nbgl_touchCallback_t)touchCallback;
  // set this new button as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)button);

  // return index of button to be modified later on
  return (layoutInt->container->nbChildren-1);
}

/**
 * @brief Updates an existing black full width confirmation button on top of the previously added keyboard.
 *
 * @param layout the current layout
 * @param index returned value of @ref nbgl_layoutAddConfirmationButton()
 * @param active if true, button is active
 * @param text text of the button
= * @return >= 0 if OK
 */
int nbgl_layoutUpdateConfirmationButton(nbgl_layout_t *layout, uint8_t index, bool active, char *text) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_button_t *button;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutUpdateConfirmationButton():\n");
  if (layout == NULL)
    return -1;

  // update main text area
  button = (nbgl_button_t*)layoutInt->container->children[index];
  if ((button == NULL) || (button->type != BUTTON)) {
    return -1;
  }
  button->text = text;

  if (active) {
    button->innerColor = BLACK;
    button->borderColor = BLACK;
    button->touchMask = (1 << TOUCHED);
  }
  else {
    button->borderColor = DARK_GRAY;
    button->innerColor = DARK_GRAY;
  }
  nbgl_redrawObject((nbgl_obj_t*)button,NULL,false);
  return 0;
}
#endif // NBGL_KEYBOARD

#ifdef NBGL_KEYPAD
/**
 * @brief Adds a keypad on bottom of the screen, with the associated callback
 *
 * @note Validate and Backspace keys are not enabled at start-up
 *
 * @param layout the current layout
 * @param callback function called when any of the key is touched
 * @param shuffled if set to true, digits are shuffled in keypad
 * @return the index of keypad, to use in @ref nbgl_layoutUpdateKeypad()
 */
int nbgl_layoutAddKeypad(nbgl_layout_t *layout, keyboardCallback_t callback, bool shuffled) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_keypad_t *keypad;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddKeypad():\n");
  if (layout == NULL)
    return -1;

  // create keypad
  keypad = (nbgl_keypad_t*)nbgl_objPoolGet(KEYPAD, layoutInt->layer);
  keypad->alignmentMarginY = 0;
  keypad->alignment = BOTTOM_MIDDLE;
  keypad->alignTo = NULL;
  keypad->borderColor = LIGHT_GRAY;
  keypad->callback = PIC(callback);
  keypad->enableDigits = true;
  keypad->enableBackspace = false;
  keypad->enableValidate = false;
  keypad->shuffled = shuffled;
  // set this new keypad as child of the container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)keypad);

  // return index of keypad to be modified later on
  return (layoutInt->container->nbChildren-1);
}

/**
 * @brief Updates an existing keypad on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeypad()
 * @param enableValidate if true, enable Validate key
 * @param enableBackspace if true, enable Backspace key
 * @param enableDigits if true, enable all digit keys
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeypad(nbgl_layout_t *layout, uint8_t index, bool enableValidate, bool enableBackspace, bool enableDigits) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_keypad_t *keypad;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutUpdateKeypad(): enableValidate = %d, enableBackspace = %d\n",enableValidate,enableBackspace);
  if (layout == NULL)
    return -1;

  // get existing keypad
  keypad = (nbgl_keypad_t*)layoutInt->container->children[index];
  if ((keypad == NULL) || (keypad->type != KEYPAD)) {
    return -1;
  }
  keypad->enableValidate = enableValidate;
  keypad->enableBackspace = enableBackspace;
  keypad->enableDigits = enableDigits;

  nbgl_redrawObject((nbgl_obj_t*)keypad,NULL,false);

  return 0;
}

/**
 * @brief Adds a set of hidden digits on top of a keypad, to represent the entered digits, as full/empty circles
 *        The set can be bordered with a gray rounded corners rectangle
 *
 * @note It must be the last added object, after potential back key, title, and keypad. Vertical positions of title and hidden digits will be computed here
 *
 * @param layout the current layout
 * @param nbDigits number of digits to be displayed
 * @param bordered if true, the set is bordered with a gray rounded corners rectangle
 * @return the index of digits set, to use in @ref nbgl_layoutUpdateHiddenDigits()
 */
int nbgl_layoutAddHiddenDigits(nbgl_layout_t *layout, uint8_t nbDigits, bool bordered) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_panel_t *panel;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutAddHiddenDigits():\n");
  if (layout == NULL)
    return -1;

  // create a panel, invisible or bordered
  panel = (nbgl_panel_t*)nbgl_objPoolGet(PANEL,layoutInt->layer);
  panel->nbChildren = nbDigits;
  panel->children = nbgl_containerPoolGet(panel->nbChildren, layoutInt->layer);
  panel->borderColor = bordered ? LIGHT_GRAY : WHITE;
  panel->width = nbDigits*C_circle_24px.width + (nbDigits+1)*8;
  panel->height = 48;
  // distance from digits to title is fixed to 24 px
  panel->alignmentMarginY = 24;
  // item N-2 is the title
  panel->alignTo = layoutInt->container->children[layoutInt->container->nbChildren-2];
  panel->alignment = BOTTOM_MIDDLE;

  // set this new container as child of the main container
  addObjectToLayout(layoutInt,(nbgl_obj_t*)panel);

  // create children of the panel, as images (empty circles)
  nbgl_objPoolGetArray(IMAGE,nbDigits,layoutInt->layer,(nbgl_obj_t**)panel->children);
  for (int i=0;i<nbDigits;i++) {
    nbgl_image_t* image = (nbgl_image_t*)panel->children[i];
    image->buffer = &C_circle_24px;
    image->foregroundColor = BLACK;
    image->alignmentMarginX = 8;
    if (i>0) {
      image->alignment = MID_RIGHT;
      image->alignTo = (nbgl_obj_t*)panel->children[i-1];
    }
    else {
      image->alignment = NO_ALIGNMENT;
      image->alignmentMarginY = (panel->height - C_circle_24px.width)/2;
    }
  }

  // return index of keypad to be modified later on
  return (layoutInt->container->nbChildren-1);
}

/**
 * @brief Updates an existing set of hidden digits, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddHiddenDigits()
 * @param nbActive number of "active" digits (represented by discs instead of circles)
 * @return >=0 if OK
 */
int nbgl_layoutUpdateHiddenDigits(nbgl_layout_t *layout, uint8_t index, uint8_t nbActive) {
  nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *)layout;
  nbgl_panel_t *panel;
  nbgl_image_t* image;

  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutUpdateHiddenDigits(): nbActive = %d\n",nbActive);
  if (layout == NULL)
    return -1;

  // get panel
  panel = (nbgl_panel_t*)layoutInt->container->children[index];
  // sanity check
  if ((panel == NULL) || (panel->type != PANEL)) {
    return -1;
  }
  if (nbActive > panel->nbChildren) {
    return -1;
  }
  if (nbActive == 0) {
    // deactivate the first digit
    image = (nbgl_image_t*)panel->children[0];
    if ((image == NULL) || (image->type != IMAGE)) {
      return -1;
    }
    image->buffer = &C_circle_24px;
  }
  else {
    image = (nbgl_image_t*)panel->children[nbActive-1];
    if ((image == NULL) || (image->type != IMAGE)) {
      return -1;
    }
    // if the last "active" is already active, it means that we are decreasing the number of active
    // otherwise we are increasing it
    if (image->buffer == &C_round_24px) {
      // all digits are already active
      if (nbActive == panel->nbChildren) {
        return 0;
      }
      // deactivate the next digit
      image = (nbgl_image_t*)panel->children[nbActive];
      image->buffer = &C_circle_24px;
    }
    else {
      image->buffer = &C_round_24px;
    }
  }

  nbgl_redrawObject((nbgl_obj_t*)image,NULL,false);

  return 0;
}
#endif // NBGL_KEYPAD

/**
 * @brief Applies given layout. The screen will be redrawn
 *
 * @param layoutParam layout to redraw
 * @return a pointer to the corresponding layout
 */
int nbgl_layoutDraw(nbgl_layout_t *layoutParam) {
  nbgl_layoutInternal_t *layout = (nbgl_layoutInternal_t *)layoutParam;

  if (layout == NULL)
    return -1;
  LOG_DEBUG(LAYOUT_LOGGER,"nbgl_layoutDraw(): container.nbChildren =%d, layout->nbChildren = %d\n",layout->container->nbChildren,layout->nbChildren);
  if (layout->tapText) {
    // set this new container as child of main container
    addObjectToLayout(layout,(nbgl_obj_t*)layout->tapText);
  }
  if (layout->withLeftBorder == true) {
    // draw now the line
    nbgl_line_t *line = createLeftVerticalLine(layout->layer);
    layout->children[layout->nbChildren] = (nbgl_obj_t*)line;
    layout->nbChildren++;
  }
  nbgl_screenRedraw();

  return 0;
}

/**
 * @brief Release the layout obtained with @ref nbgl_layoutGet()
 *
 * @param layoutParam layout to release
 * @return >= 0 if OK
 */
int nbgl_layoutRelease(nbgl_layout_t *layoutParam) {
  nbgl_layoutInternal_t *layout = (nbgl_layoutInternal_t *)layoutParam;
  LOG_DEBUG(PAGE_LOGGER,"nbgl_layoutRelease(): \n");
  if (layout == NULL)
    return -1;
  // if modal
  if (layout->modal) {
    nbgl_screenPop(layout->layer);
  }
  layout->nbChildren = 0;
  return 0;
}
