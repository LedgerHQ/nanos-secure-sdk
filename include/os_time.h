#pragma once

#include "decorators.h"

// UX specific syscall to share time with dashboard task
SYSCALL void os_set_ux_time_ms(unsigned int ux_ms);
