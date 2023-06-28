/**
 * @file nbgl_screen.c
 * @brief Implementation of screens management API
 */

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_screen.h"
#include "nbgl_debug.h"
#include "os_pic.h"
#include "os_io.h"

/*********************
 *      DEFINES
 *********************/

/**
 * @brief Max number of stackable screens
 * @note Only the screen at the top of the stack is visible
 *
 * Only index 0 is always bottom of stack, for the others the index can change
 *
 * The worst case is (the order is not relevant)
 *
 * Validate pin
 * ----------------
 * BLE Pairing
 * -------------
 * Control Center
 * ---------
 * Dashboard
 *
 */
#define SCREEN_STACK_SIZE 4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/
static nbgl_screen_t screenStack[SCREEN_STACK_SIZE];
// number of screens in the stack
static uint8_t nbScreensOnStack = 0;
// this is a pointer of the current top of stack screen
static nbgl_screen_t *topOfStack;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief This function redraws the whole screen on top of stack and its children
 *
 */
void nbgl_screenRedraw(void) {
  if (nbScreensOnStack == 0) {
    LOG_WARN(SCREEN_LOGGER,"nbgl_screenRedraw(): no screen to redraw\n");
    return;
  }
  LOG_DEBUG(SCREEN_LOGGER,"nbgl_screenRedraw(): nbScreensOnStack = %d\n",nbScreensOnStack);
#ifdef HAVE_SE_TOUCH
  // by default, exclude left & top borders from touch
  // if any sub-object is a keyboard, this will be modified when drawing it
  touch_exclude_borders(TOP_BORDER | LEFT_BORDER);
#endif // HAVE_SE_TOUCH

  nbgl_screen_reinit();
  nbgl_redrawObject((nbgl_obj_t *)topOfStack, NULL, true);
}

/**
 * @brief Returns the screen on top layer, as a generic object
 * @return the screen on top layer, or NULL if no screen in stack
 */
nbgl_obj_t *nbgl_screenGetTop(void) {
  if (nbScreensOnStack > 0)
    return (nbgl_obj_t *)topOfStack;
  else
    return NULL;
}

/**
 * @brief Returns the number of used screens on stack
 * @return the number of used screens on stack
 */
uint8_t nbgl_screenGetCurrentStackSize(void) {
  if ((nbScreensOnStack == 1) && (screenStack[0].container.nbChildren == 0))
    return 0;
  return nbScreensOnStack;
}

/**
 * @brief Set the children of the screen with the given array of nbgl_obj_t*
 * It will replace the current children array.
 * A @ref nbgl_screenRedraw() can be called after that to draw all the given objects (and their children)
 *
 * @param screenIndex index of the screen in the stack. 0 is the usual value, except for modal windows
 * @param children an array of nbgl_obj_t*
 * @param nbChildren number of elements in children array
 * @param ticker configuration of ticker
 * @param callback callback called when any touchable object of the screen is touched (except keyboard/keypad)
 *
 * @return >= 0 if OK
 */
static int nbgl_screenSetAt(uint8_t screenIndex, nbgl_obj_t*** children, uint8_t nbChildren,
                            const nbgl_screenTickerConfiguration_t *ticker,
                            nbgl_touchCallback_t callback) {
  if (screenIndex >= SCREEN_STACK_SIZE) {
    LOG_WARN(SCREEN_LOGGER,"nbgl_screenSetAt(): forbidden screenIndex (%d)\n",screenIndex);
    return -1;
  }
  *children = nbgl_containerPoolGet(nbChildren,screenIndex);
  screenStack[screenIndex].container.obj.type = SCREEN;
  screenStack[screenIndex].container.obj.area.backgroundColor = WHITE;
  screenStack[screenIndex].container.obj.area.height = SCREEN_HEIGHT;
  screenStack[screenIndex].container.obj.area.width = SCREEN_WIDTH;
  screenStack[screenIndex].container.obj.area.x0 = 0;
  screenStack[screenIndex].container.obj.area.y0 = 0;
  screenStack[screenIndex].container.obj.rel_x0 = 0;
  screenStack[screenIndex].container.obj.rel_y0 = 0;
  screenStack[screenIndex].container.layout = VERTICAL;
  screenStack[screenIndex].container.children = *children;
  screenStack[screenIndex].container.nbChildren = nbChildren;
  screenStack[screenIndex].touchCallback = callback;
  if (ticker != NULL) {
    screenStack[screenIndex].ticker.tickerCallback = (nbgl_tickerCallback_t)PIC(ticker->tickerCallback);
    screenStack[screenIndex].ticker.tickerIntervale = ticker->tickerIntervale;
    screenStack[screenIndex].ticker.tickerValue = ticker->tickerValue;
  }
  else {
    screenStack[screenIndex].ticker.tickerCallback = NULL;
  }
  return 0;
}

/**
 * @brief Configures the lowest layer screen. To be used by applications
 * A @ref nbgl_screenRedraw() can be called after that to draw all the given objects (and their children)
 *
 * @param elements an pointer on an array of nbgl_obj_t* to get as children of the screen, it will be allocated by the function
 * @param nbElements number of elements in elements array
 * @param ticker if not NULL, configures the potential ticker to be used as a periodic timer
 * @param callback callback called when any touchable object of the screen is touched (except keyboard/keypad) (can be NULL)
 *
 * @return >= 0 if OK
 */
int nbgl_screenSet(nbgl_obj_t*** elements, uint8_t nbElements,
                   const nbgl_screenTickerConfiguration_t *ticker,
                   nbgl_touchCallback_t callback) {
  // if no screen, consider it as a first fake push
  if (nbScreensOnStack == 0) {
    nbScreensOnStack++;
    topOfStack = &screenStack[0];
  }
  // release used objects and containers
  nbgl_objPoolRelease(0);
  nbgl_containerPoolRelease(0);
  // always use the first layer (background) for user application
  return nbgl_screenSetAt(0, elements,  nbElements, ticker, callback);
}

/**
 * @brief Updates the number of children on given layer. can only be smaller than the number given in @ref nbgl_screenSet()
 *
 * @param screenIndex index of the screen in the stack. 0 is the usual value, except for modal windows
 * @param nbElements number of elements in elements array
 *
 * @return >= 0 if OK
 */
int nbgl_screenUpdateNbElements(uint8_t screenIndex, uint8_t nbElements) {
  screenStack[screenIndex].container.nbChildren = nbElements;
  return 0;
}

/**
 * @brief Updates the background color of the screen at the given screenIndex, always set at @ref WHITE in
 *
 * @param screenIndex index of the screen in the stack. 0 is the usual value, except for modal windows
 * @param color color to set as background color for tge screen
 *
 * @return >= 0 if OK
 */
int nbgl_screenUpdateBackgroundColor(uint8_t screenIndex, color_t color) {
  screenStack[screenIndex].container.obj.area.backgroundColor = color;
  return 0;
}

/**
 * @brief Updates the ticker configuration of the screen at the given screenIndex, always set at @ref WHITE in
 *
 * @param screenIndex index of the screen in the stack. 0 is the usual value, except for modal windows
 * @param ticker if not NULL, configures the potential ticker to be used as a periodic timer
 *
 * @return >= 0 if OK
 */
int nbgl_screenUpdateTicker(uint8_t screenIndex, const nbgl_screenTickerConfiguration_t *ticker) {
  if (ticker != NULL) {
    screenStack[screenIndex].ticker.tickerCallback = (nbgl_tickerCallback_t)PIC(ticker->tickerCallback);
    screenStack[screenIndex].ticker.tickerIntervale = ticker->tickerIntervale;
    screenStack[screenIndex].ticker.tickerValue = ticker->tickerValue;
  }
  else {
    screenStack[screenIndex].ticker.tickerCallback = NULL;
  }
  return 0;
}

/**
 * @brief Returns the array of elements (children) of the screen at the given index (return value of @ref nbgl_screenPush() or 0
 * for a screen created with @ref nbgl_screenSet())
 *
 * @param screenIndex index of the screen in the stack. 0 is the usual value, except for modal windows
  *
 * @return array of elements (children) of the screen
 */
nbgl_obj_t** nbgl_screenGetElements(uint8_t screenIndex) {
  return screenStack[screenIndex].container.children;
}

/**
 * @brief Pushes a screen on top of the stack, with the given number of elements, if possible. The array of children for
 * this screen is set in given <b>elements</b> parameter
 * A @ref nbgl_screenRedraw() can be called after that to draw all the given objects (and their children)
 *
 * @param elements (output) an array of nbgl_obj_t** to get
 * @param nbElements number of elements to get in <b>elements</b> array
 * @param ticker if not NULL, configures the potential ticker to be used as a periodic timer
 * @param callback callback called when any touchable object of the screen is touched (except keyboard/keypad) (can be NULL)
 *
 * @return current screen index, or < 0 if error
 */
int nbgl_screenPush(nbgl_obj_t*** elements, uint8_t nbElements,
                    const nbgl_screenTickerConfiguration_t *ticker,
                    nbgl_touchCallback_t callback) {
  uint8_t screenIndex;
  if (nbScreensOnStack >= SCREEN_STACK_SIZE) {
    LOG_WARN(SCREEN_LOGGER,"nbgl_screenPush(): already in highest index in the stack(%d)\n",nbScreensOnStack-1);
    return -1;
  }
  // if no screen, consider it as a first fake push
  if (nbScreensOnStack == 0) {
    screenIndex = 1; // push at position 1 because 0 is reserved for background
    topOfStack = &screenStack[screenIndex];
    topOfStack->next = NULL;
    // link top of stack to background (even if empty)
    topOfStack->previous = &screenStack[0];
    screenStack[0].next = topOfStack;
    screenStack[0].container.nbChildren = 0;
    // count empty background as an active screen
    nbScreensOnStack++;
  }
  else {
    // find a non used screen in the array
    for (screenIndex=1;screenIndex<SCREEN_STACK_SIZE;screenIndex++)
    {
      if (screenStack[screenIndex].previous == NULL) {
        // if no previous, means unused, so take it
        // update previous topOfStack
        topOfStack->next = &screenStack[screenIndex];
        screenStack[screenIndex].previous = topOfStack;
        // new top of stack
        topOfStack = &screenStack[screenIndex];
        topOfStack->next = NULL;
        break;
      }
    }
    if (screenIndex == SCREEN_STACK_SIZE) {
      // should never happen
      LOG_WARN(SCREEN_LOGGER,"nbgl_screenPush(): corruption in stack\n");
    }
  }
  if (nbgl_screenSetAt(screenIndex, elements,  nbElements, ticker, callback) >= 0) {
    nbScreensOnStack++;
    LOG_DEBUG(SCREEN_LOGGER,"nbgl_screenPush(): screen %d is now top of stack\n",screenIndex);
    return screenIndex;
  }
  else {
    return -1;
  }
}

/**
 * @brief Release the screen at the given index in screen array (index returned by @ref nbgl_screenPush()).
 * A @ref nbgl_screenRedraw() can be called after that to draw all objects in the new top of the stack
 *
 * @param screenIndex index of the screen to pop in the stack. 0 is the usual value, except for modal windows
 * @return the index of screen on top of stack (if -1, the stack is empty)
 */
int nbgl_screenPop(uint8_t screenIndex) {
  if (nbScreensOnStack == 0) {
    LOG_WARN(SCREEN_LOGGER,"nbgl_screenPop(): already in lowest index in the stack\n");
    return -1;
  }
  LOG_DEBUG(SCREEN_LOGGER,"nbgl_screenPop(): at index %d\n",screenIndex);
  nbScreensOnStack--;
  // move top of stack if needed
  if (&screenStack[screenIndex] == topOfStack) {
    if (nbScreensOnStack == 0) {
      topOfStack = NULL;
    }
    else {
      topOfStack = topOfStack->previous;
    }
  }
  else {
    // connect previous to next
    if (screenStack[screenIndex].previous != NULL) {
      screenStack[screenIndex].previous->next = screenStack[screenIndex].next;
    }
    if (screenStack[screenIndex].next != NULL) {
      screenStack[screenIndex].next->previous = screenStack[screenIndex].previous;
    }
  }
  // free slot
  screenStack[screenIndex].previous = NULL;
  screenStack[screenIndex].next = NULL;
  screenStack[screenIndex].container.nbChildren = 0;
  screenStack[screenIndex].container.children = NULL;
  // release used objects and containers
  nbgl_objPoolRelease(screenIndex);
  nbgl_containerPoolRelease(screenIndex);
  return 0;
}

/**
 * @brief Releases all screens and objects and resets the screen stack.
 * It is supposed to be called before running an app
 *
 * @return >= 0 if OK
 */
int nbgl_screenReset(void) {
  uint8_t screenIndex;
  for (screenIndex=0;screenIndex<SCREEN_STACK_SIZE;screenIndex++) {
    if ((screenStack[screenIndex].previous != NULL)||(screenStack[screenIndex].next != NULL)) {
      // release used objects and containers
      nbgl_objPoolRelease(screenIndex);
      nbgl_containerPoolRelease(screenIndex);
    }
  }
  nbScreensOnStack = 0;
  topOfStack = NULL;
  return 1;
}

/**
 * @brief Function to be called periodically by system to enable using ticker
 *
 * @param intervaleMs intervale or time since the last call, in ms
 */
void nbgl_screenHandler(uint32_t intervaleMs) {
  // ensure a screen exists
  if (nbScreensOnStack == 0)
    return;
  // call ticker callback of top of stack if active and not expired yet (for a non periodic)
  if ((topOfStack->ticker.tickerCallback != NULL) &&
      (topOfStack->ticker.tickerValue != 0)) {
    topOfStack->ticker.tickerValue -= MIN(topOfStack->ticker.tickerValue, intervaleMs);
    if (topOfStack->ticker.tickerValue == 0) {
      // rearm if intervale is not null, and call the registered function
      topOfStack->ticker.tickerValue = topOfStack->ticker.tickerIntervale;
      topOfStack->ticker.tickerCallback();
    }
  }
}

/**
 * @brief return true if the given obj can be found in refObj or any of its children
 *
 * @param refObj the object to search obj into
 * @param obj the object to search
 * @return true if belongs, false otherwise
 */
static bool objIsIn(nbgl_obj_t *refObj, nbgl_obj_t *obj) {
  uint8_t i;

  if (obj == NULL) {
    return false;
  }
  if ((nbgl_obj_t *)refObj == obj) {
    LOG_DEBUG(SCREEN_LOGGER,"nbgl_screenContainsObj(): yes\n");
    return true;
  }

  if ((refObj->type == SCREEN) ||
      (refObj->type == CONTAINER)) {
    nbgl_container_t *container = (nbgl_container_t *)refObj;
    // draw the children, if any
    if (container->children != NULL) {
      for (i=0;i<container->nbChildren;i++) {
        nbgl_obj_t *current = container->children[i];
        if (current != NULL) {
          if (objIsIn(current,obj) == true) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

/**
 * @brief return true if the given obj can be found in refObj or any of its children
 *
 * @param obj the object to search
 * @return true if belongs, false otherwise
 */
bool nbgl_screenContainsObj(nbgl_obj_t *obj) {
  if (nbScreensOnStack == 0)
    return false;
  return objIsIn((nbgl_obj_t *)topOfStack,obj);
}
