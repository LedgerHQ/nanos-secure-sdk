/**
 * @file nbgl_touch.c
 * Implementation of touchscreen management in new BAGL
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "nbgl_obj.h"
#include "nbgl_debug.h"
#include "nbgl_touch.h"
#include "nbgl_screen.h"
#include "os_pic.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t lastPressedTime = 0;
static uint32_t lastCurrentTime = 0;
static nbgl_obj_t *lastPressedObj = NULL;
static nbgl_touchStatePosition_t firstTouchedPosition,lastTouchedPosition;

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * @brief This function applies the touch event of given type on the given graphic object
 *
 * @param obj object on which the event is applied
 * @param eventType type of touchscreen event
 */
static void applytouchStatePosition(nbgl_obj_t *obj, nbgl_touchType_t eventType) {
  LOG_DEBUG(TOUCH_LOGGER,"Apply event %d on object of type %d\n",eventType,obj->type);
  if (!obj) {
    return;
  }
  /* the first action is the one provided by the application */
  if (((obj->touchMask&(1<<eventType)) != 0) && (obj->touchCallback != NULL)) {
    ((nbgl_touchCallback_t)PIC(obj->touchCallback))((void *)obj,eventType);
  }
}

/**
 * @brief if the given obj contains the coordinates of the given event, parse
 * all its children with the same criterion.
 * If no children or none concerned, check whether this object can process the event or not
 *
 * @param obj
 * @param event
 * @return the concerned object or NULL if not found
 */
static nbgl_obj_t * getTouchedObject(nbgl_obj_t *obj, nbgl_touchStatePosition_t *event) {
  if (obj == NULL) {
    return NULL;
  }
  /* check coordinates
     no need to go further if the touched point is not within the object
     And because the children are also within the object, no need to check them either */
  if ((event->x < obj->x0) || (event->x > (obj->x0+obj->width)) ||
      (event->y < obj->y0) || (event->y > (obj->y0+obj->height))) {
    return NULL;
  }
  if ((obj->type == SCREEN) ||
      (obj->type == CONTAINER) ||
      (obj->type == PANEL)) {
    nbgl_container_t *container = (nbgl_container_t *)obj;
    // parse the children, if any
    if (container->children != NULL) {
      uint8_t i;
      for (i=0;i<container->nbChildren;i++) {
        nbgl_obj_t *current = container->children[i];
        if (current != NULL) {
          current = getTouchedObject(current,event);
          if (current != NULL) {
            return current;
          }
        }
      }
    }
  }
  /* now see if the object is interested by touch events (any of them) */
  if (obj->touchMask != 0) {
    //LOG_DEBUG(TOUCH_LOGGER,"%d %d \n",clickableObjectTypes ,(1<<obj->type));
    return obj;
  }
  else {
    return NULL;
  }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Function to be called periodically to check touchscreen state
 * and coordinates
 * @param touchStatePosition state and position read from touch screen
 * @param currentTime current time in ms
 */
void nbgl_touchHandler(nbgl_touchStatePosition_t *touchStatePosition, uint32_t currentTime) {
  static nbgl_touchState_t lastState = RELEASED;
  nbgl_obj_t *foundObj;

  // save last received currentTime
  lastCurrentTime = currentTime;

  if (lastState == RELEASED) {
    // filter out not realistic cases (successive RELEASE events)
    if (RELEASED == touchStatePosition->state) {
      lastState = touchStatePosition->state;
      return;
    }
    // memorize first touched position
    memcpy(&firstTouchedPosition, touchStatePosition, sizeof(nbgl_touchStatePosition_t));
  }
  //LOG_DEBUG(TOUCH_LOGGER,"state = %s, x = %d, y=%d\n",(touchStatePosition->state == RELEASED)?"RELEASED":"PRESSED",touchStatePosition->x,touchStatePosition->y);

  //parse the whole screen to find proper object
  foundObj = getTouchedObject(nbgl_screenGetTop(),touchStatePosition);

  //LOG_DEBUG(TOUCH_LOGGER,"nbgl_touchHandler: found obj %p, type = %d\n",foundObj, foundObj->type);
  if (foundObj == NULL) {
    LOG_DEBUG(TOUCH_LOGGER,"nbgl_touchHandler: no found obj\n");
    if ((touchStatePosition->state == PRESSED) &&
        (lastState == PRESSED) &&
        (lastPressedObj != NULL)) {
      // finger has move out of an objet
      // make sure lastPressedObj still belongs to current screen before warning it
      if (nbgl_screenContainsObj(lastPressedObj)) {
        applytouchStatePosition(lastPressedObj,OUT_OF_TOUCH);
      }
    }
    lastPressedObj = NULL;
    lastState = touchStatePosition->state;
    return;
  }

  // memorize last touched position
  memcpy(&lastTouchedPosition, touchStatePosition, sizeof(nbgl_touchStatePosition_t));

  if (touchStatePosition->state == RELEASED) {
    // very strange if lastPressedObj != foundObj, let's consider that it's a normal release on lastPressedObj
    // make sure lastPressedObj still belongs to current screen before "releasing" it
    if ((lastPressedObj != NULL) &&
      ((foundObj == lastPressedObj) ||
       (nbgl_screenContainsObj(lastPressedObj)))){
      applytouchStatePosition(lastPressedObj,TOUCH_RELEASED);
      if (currentTime >= (lastPressedTime+LONG_TOUCH_DURATION)) {
        applytouchStatePosition(lastPressedObj,LONG_TOUCHED);
      }
      else if (currentTime >= (lastPressedTime+SHORT_TOUCH_DURATION)) {
        applytouchStatePosition(lastPressedObj,TOUCHED);
      }
    }
    // Released event has been handled, forget lastPressedObj
    lastPressedObj = NULL;
  }
  else { // PRESSED
    if ((lastState == PRESSED) && (lastPressedObj != NULL)) {
      if (foundObj != lastPressedObj) {
        // finger has move out of an objet
        // make sure lastPressedObj still belongs to current screen before warning it
        if (nbgl_screenContainsObj(lastPressedObj)) {
          applytouchStatePosition(lastPressedObj,OUT_OF_TOUCH);
        }
        lastPressedObj = NULL;
      }
      else {
        // warn the concerned object that it is still touched
        applytouchStatePosition(foundObj,TOUCHING);
      }
    }
    else if (lastState == RELEASED) {
      // newly touched object
      lastPressedObj = foundObj;
      lastPressedTime = currentTime;
      applytouchStatePosition(foundObj,TOUCH_PRESSED);
      applytouchStatePosition(foundObj,TOUCHING);
    }
  }

  lastState = touchStatePosition->state;
}

bool nbgl_touchGetTouchedPosition(nbgl_obj_t *obj, nbgl_touchStatePosition_t **firstPos, nbgl_touchStatePosition_t **lastPos) {
  LOG_DEBUG(TOUCH_LOGGER,"nbgl_touchGetTouchedPosition: %p %p\n", obj,lastPressedObj);
  if (obj == lastPressedObj) {
    *firstPos = &firstTouchedPosition;
    *lastPos = &lastTouchedPosition;
    return true;
  }
  return false;
}

uint32_t nbgl_touchGetTouchDuration(nbgl_obj_t *obj) {
  if (obj == lastPressedObj) {
    return (lastCurrentTime-lastPressedTime);
  }
  return 0;
}
