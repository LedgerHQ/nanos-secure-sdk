#pragma once

#include "bolos_target.h"
#include "decorators.h"

#ifdef DEBUG_OS_STACK_CONSUMPTION
#define MODE_INITIALIZATION 0x00
#define MODE_RETRIEVAL 0x01
/**
 * Debug syscall allowing one to monitor the OS stack usage of other syscalls.
 * This syscall cannot be embedded within the release version of the OS.
 * @param mode The mode of operation:
 *   - MODE_INITIALIZATION, which initializes the available stack to 0xFF,
 *   - MODE_RETRIEVAL, which indicates the stack maximum consumption since the
 *     last initialization.
 * @return An integer from which:
 *   - The most significant bit [bit 31], when set, indicates that the os stack
 *     has been entirely smashed,
 *   - The two least significant bytes [bits 15:0] encode the size of the used
 *     stack.
 */
SYSCALL int os_stack_operations(unsigned char mode);
#endif // DEBUG_OS_STACK_CONSUMPTION
