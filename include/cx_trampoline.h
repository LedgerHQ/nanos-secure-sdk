#pragma once

#include "bolos_target.h"

#if defined(TARGET_NANOS)
#define CX_TRAMPOLINE_ADDR 0x00120001
#elif defined(TARGET_BLUE)
#define CX_TRAMPOLINE_ADDR 0x00120001
#elif defined(TARGET_NANOX)
#define CX_TRAMPOLINE_ADDR 0x00210001
#endif
