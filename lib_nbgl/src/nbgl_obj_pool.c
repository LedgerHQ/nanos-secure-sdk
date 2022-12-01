
/**
 * @file nbgl_obj_pool.c
 * @brief Implementation of a graphical objects pool, to be used by any page, or by
 * predefined layout management
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "nbgl_obj.h"
#include "nbgl_debug.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
/**
 * @brief Max number of objects retrievable from pool
 *
 */
#define OBJ_POOL_LEN 80
/**
 * @brief Max number of objects pointers usable for container pool
 *
 */
#define OBJ_CONTAINER_POOL_LEN 128

#define INVALID_LAYER 0xFF

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
  union {
    nbgl_obj_t obj;
    nbgl_radio_t radioObj;
    nbgl_switch_t switchObj;
    nbgl_text_area_t textAreaObj;
    nbgl_button_t buttonObj;
    nbgl_progress_bar_t progressBarObj;
    nbgl_page_indicator_t navBarObj;
    nbgl_container_t containerObj;
    nbgl_image_t imageObj;
    nbgl_line_t lineObj;
    nbgl_panel_t panelObj;
    nbgl_keyboard_t keyboardObj;
    nbgl_keypad_t keypadObj;
    nbgl_spinner_t spinnerObj;
  };
} genericObj_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

// This is the pool of graphic objects
static genericObj_t objPool[OBJ_POOL_LEN];
// This is the array of layer per object in pool
static uint8_t objPoolLayers[OBJ_POOL_LEN];
// This is the number of graphic objects currently used from the objPool
static uint8_t nbUsedObjsInPool = 0;

// This is the pool of graphic objects pointers for containers
static nbgl_obj_t* objContainerPool[OBJ_CONTAINER_POOL_LEN];
// This is the array of layer per object pointers in pool
static uint8_t objPointersPoolLayers[OBJ_CONTAINER_POOL_LEN];
// This is the number of graphic objects currently used from the objContainerPool
static uint8_t nbUsedObjsInContainerPool = 0;

static bool initialized;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Release the objects from the pool for the given layer
 * @param layer layer to release object from
 */
void nbgl_objPoolRelease(uint8_t layer) {
  uint8_t i;
  LOG_DEBUG(OBJ_POOL_LOGGER,"nbgl_objPoolRelease(): used objs in pool %d, for layer (%d)\n",nbUsedObjsInPool,layer);
  if (nbUsedObjsInPool == 0) {
    return;
  }
  for (i=0;i<OBJ_POOL_LEN;i++) {
    if (layer == objPoolLayers[i]) {
      objPoolLayers[i] = INVALID_LAYER;
      nbUsedObjsInPool--;
    }
  }
}

/**
 * @brief returns the number of objects currently used in the pool
 */
uint8_t nbgl_objPoolGetNbUsed(uint8_t layer) {
  uint8_t i,nbFound=0;
  for (i=0;i<OBJ_POOL_LEN;i++) {
    if (layer == objPoolLayers[i]) {
      nbFound++;
    }
  }
  return nbFound;
}

/**
 * @brief Gets a new graphic object from the pool, with the given type. The type field of the object is set.
 * @param type type of object to get from the pool
 * @param layer layer to get object for
 */
nbgl_obj_t* nbgl_objPoolGet(nbgl_obj_type_t type, uint8_t layer) {
  uint8_t i;
  nbgl_obj_t* obj;

  if (initialized == false) {
    memset(objPoolLayers,INVALID_LAYER,OBJ_POOL_LEN);
    memset(objPointersPoolLayers,INVALID_LAYER,OBJ_CONTAINER_POOL_LEN);
    initialized = true;
  }
  if (nbUsedObjsInPool==(OBJ_POOL_LEN-1)) {
    LOG_FATAL(OBJ_POOL_LOGGER,"nbgl_objPoolGet(): NO ENOUGH OBJ in POOL. max = %d\n",OBJ_POOL_LEN);
    // TODO: crash here?
    return NULL;
  }
  for (i=0;i<OBJ_POOL_LEN;i++) {
    if (INVALID_LAYER == objPoolLayers[i]) {
      obj = &objPool[i].obj;
      memset(obj,0,sizeof(genericObj_t));
      obj->type = type;
      objPoolLayers[i] = layer;
      nbUsedObjsInPool++;
      return obj;
    }
  }
  //should never happen
  return NULL;
}

/**
 * @brief Gets a link to the previous object in the pool, for the given layer. (to be used with care)
 * @param obj object to get previous from
 * @param layer layer (screen) in which to retrieve the object
 * @return a pointer to the previous object in the pool
 */
nbgl_obj_t* nbgl_objPoolGetPrevious(nbgl_obj_t* obj, uint8_t layer) {
  uint8_t index;

  // retrieve object index
  index = (genericObj_t*)obj - objPool;

  while (index>0) {
    index--;
    if (objPoolLayers[index] == layer) {
      return &objPool[index].obj;
    }
  }

  return NULL;
}

/**
 * @brief Gets nbObjects new graphic object from the pool, with the given type, for the given layer (screen). The type field of the object is set.
 * @param type type of objects to get from the pool
 * @param nbObjs number of objects to get from the pool
 * @param layer layer (screen) to get the object to
 * @param objArray array of objs pointers
 * @return >= 0 if OK
 */
int nbgl_objPoolGetArray(nbgl_obj_type_t type, uint8_t nbObjs, uint8_t layer, nbgl_obj_t** objArray) {
  uint8_t i;

  if ((nbUsedObjsInPool+nbObjs)>=OBJ_POOL_LEN) {
    LOG_FATAL(OBJ_POOL_LOGGER,"nbgl_objPoolGetArray(): NO ENOUGH OBJ in POOL. Asked: %d, max = %d\n",(nbUsedObjsInPool+nbObjs),OBJ_POOL_LEN);
    // TODO: crash here?
    return -1;
  }
  for (i=0;i<nbObjs;i++) {
    objArray[i] = nbgl_objPoolGet(type, layer);
  }

  return 0;
}

/**
 * @brief Release the objects pointers from the pool for the given layer
 * @param layer layer to release objects pointers from
 */
void nbgl_containerPoolRelease(uint8_t layer) {
  uint8_t i;
  LOG_DEBUG(OBJ_POOL_LOGGER,"nbgl_containerPoolRelease(): %d containers in pool\n",nbUsedObjsInContainerPool);
  if (nbUsedObjsInContainerPool == 0) {
    return;
  }
  for (i=0;i<OBJ_CONTAINER_POOL_LEN;i++) {
    if (layer == objPointersPoolLayers[i]) {
      objContainerPool[i] = NULL;
      objPointersPoolLayers[i] = INVALID_LAYER;
      nbUsedObjsInContainerPool--;
    }
  }
}

/**
 * @brief Gets a new container from the pool, with the given number of obj pointers
 * @param nbObjs number of objects pointers for the container
 * @param layer layer (screen) to get the container to
 */
nbgl_obj_t** nbgl_containerPoolGet(uint8_t nbObjs, uint8_t layer) {
  uint8_t i=0,nbContiguousFree=0;
  nbgl_obj_t** container;
  if (initialized == false) {
    memset(objPoolLayers,INVALID_LAYER,OBJ_POOL_LEN);
    memset(objPointersPoolLayers,INVALID_LAYER,OBJ_CONTAINER_POOL_LEN);
    initialized = true;
  }
  if ((nbUsedObjsInContainerPool+nbObjs)>=(OBJ_CONTAINER_POOL_LEN-1)) {
    LOG_FATAL(OBJ_POOL_LOGGER,"nbgl_containerPoolGet(): NO ENOUGH OBJ in POOL. Asked: %d, max = %d\n",(nbUsedObjsInContainerPool+nbObjs),OBJ_CONTAINER_POOL_LEN);
    // TODO: crash here?
    return NULL;
  }
  while(i<OBJ_CONTAINER_POOL_LEN) {
    if (INVALID_LAYER == objPointersPoolLayers[i]) {
      nbContiguousFree++;
    }
    else {
      nbContiguousFree = 0;
    }
    i++;
    if (nbContiguousFree == nbObjs) {
      container = &objContainerPool[i-nbObjs];
      memset(container,0,nbObjs*sizeof(nbgl_obj_t*));
      // mark slots as used
      memset(&objPointersPoolLayers[i-nbObjs],layer,nbObjs);
      nbUsedObjsInContainerPool+=nbObjs;
      return container;
    }
  }
  return NULL;
}

/**
 * @brief returns the number of containers currently used in the pool
 */
uint8_t nbgl_containerPoolGetNbUsed(uint8_t layer) {
  uint8_t i,nbFound=0;
  for (i=0;i<OBJ_CONTAINER_POOL_LEN;i++) {
    if (layer == objPointersPoolLayers[i]) {
      nbFound++;
    }
  }
  return nbFound;
}
