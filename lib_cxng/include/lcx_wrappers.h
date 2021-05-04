#pragma once

/*
 * This file is only required when the macro CX_THROW is used (ie. in cx
 * _no_throw wrappers.
 *
 * This macro can't be defined in cx_errors.h because tests can't build when
 * exceptions.h is included.
 */

#include "exceptions.h"
#include "os_helpers.h" // for UNUSED

/* used by wrappers */
#define CX_THROW(call)	do {                      \
    cx_err_t error = call;                        \
    if (error) {                                  \
      THROW(error);                               \
    }                                             \
  } while (0)
