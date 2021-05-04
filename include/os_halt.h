#pragma once

#include "bolos_target.h"
#include "decorators.h"

// halt the chip, waiting for a physical user interaction
SYSCALL void halt(void);

// deprecated
#define reset halt
