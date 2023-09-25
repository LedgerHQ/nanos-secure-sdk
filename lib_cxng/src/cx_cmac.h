/* @BANNER@ */

#ifndef CX_CMAC_H
#define CX_CMAC_H

#ifdef HAVE_CMAC
#include "cx_errors.h"
#include "decorators.h"
#include <string.h>
#include <stdint.h>

WARN_UNUSED_RESULT cx_err_t cx_cmac_shift_and_xor(uint8_t *output,
                                                  uint8_t *input,
                                                  size_t   block_size);
#endif  // HAVE_CMAC

#endif /* CX_CMAC_H */
