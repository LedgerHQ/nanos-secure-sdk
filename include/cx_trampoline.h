#pragma once

#include "bolos_target.h"

#if defined(TARGET_NANOS)
#define CX_TRAMPOLINE_ADDR 0x00120001
#elif defined(TARGET_NANOX)
#define CX_TRAMPOLINE_ADDR 0x00210001
#elif defined(TARGET_NANOS2)
#define CX_TRAMPOLINE_ADDR 0x00810001
#elif defined(TARGET_STAX)
#define CX_TRAMPOLINE_ADDR 0x00818001
#elif defined(TARGET_EUROPA)
#define CX_TRAMPOLINE_ADDR 0x00818001
#endif
