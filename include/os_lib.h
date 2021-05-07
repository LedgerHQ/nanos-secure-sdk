#pragma once

#include "decorators.h"

/* ----------------------------------------------------------------------- */
/* -                            LIB FUNCTIONS                            - */
/* ----------------------------------------------------------------------- */
/**
 * Library call function.
 * call_parameters[0] = library name string pointer (const)
 * call_parameters[1] = library call identifier (0 = init, ...)
 * call_parameters[2+] = called function parameters
 */
SYSCALL void
os_lib_call(unsigned int *call_parameters PLENGTH(3 * sizeof(unsigned int)));
SYSCALL void os_lib_end(void);
