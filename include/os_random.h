#pragma once

#include "cx_errors.h"
#include "decorators.h"
#include <stddef.h>

/* ----------------------------------------------------------------------- */
/*   -                          RANDOMNESS                               - */
/* ----------------------------------------------------------------------- */
SYSCALL cx_err_t cx_get_random_bytes(void *buffer, size_t len);
