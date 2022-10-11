// These macros are defined by clang and gcc when -fstack-protector arguments
// are passed to the compiler.
#if defined(__SSP__) || defined(__SSP_ALL__) || defined(__SSP_STRONG__) || defined(__SSP_EXPLICIT__)

#include "os_task.h"

__attribute__((noreturn)) void __wrap___stack_chk_fail(void)
{
    // this magic value is an hint for developers
    os_sched_exit(37);
}

#endif
