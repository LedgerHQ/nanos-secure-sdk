#pragma once

#include "nbgl_obj.h"

#define NBGL_SERIALIZE_OK    0
#define NBGL_SERIALIZE_ERROR 1

typedef enum nbgl_serialized_event_type_e {
    NBGL_DRAW_OBJ = 0,
    NBGL_REFRESH_AREA
} nbgl_serialized_event_type_e;

uint8_t nbgl_serializeNbglEvent(nbgl_serialized_event_type_e type,
                                nbgl_obj_t                  *obj,
                                uint8_t                     *out,
                                size_t                      *w_cnt,
                                size_t                       max_len);
