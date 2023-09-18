#pragma once

#include "bolos_target.h"
#include "decorators.h"
#include <stdbool.h>

#define NVM_ERASED_WORD_VALUE 0xFFFFFFFF

// write in persistent memory, to make things easy keep a layout of the memory in a structure and
// update fields upon needs The function throws exception when the requesting application buffer
// being written in its declared data segment. The later is declared during the application slot
// allocation (using --dataSize parameter in the python scripts) NOTE: accept copy from far memory
// to another far memory.
// @param src_adr NULL to fill with 00's
SYSCALL void nvm_write(void *dst_adr PLENGTH(src_len),
                       void *src_adr PLENGTH(src_len),
                       unsigned int  src_len);

SYSCALL void nvm_erase(void *dst_adr PLENGTH(len), unsigned int len);

// program a page with the content of the nvm_page_buffer, only callable by the privileged APIs
// HAL for the high level NVM management functions
SUDOCALL void nvm_write_page(unsigned int page_adr, bool force);

// erase a nvm page at given address, only callable by the privileged APIs
SUDOCALL void nvm_erase_page(unsigned int page_adr);

// any application can wipe the global pin, global seed, user's keys
// disabled for security reasons // SYSCALL                                       void
// os_perso_wipe(void); erase seed, settings AND applications
SYSCALL void os_perso_erase_all(void);

#ifndef BOLOS_OS_UPGRADER_APP
// Wipe all data from N_application_registry.applications,
// except language packs / background img
void erase_all_except_user_config_apps(void);
#endif  //(defined(HAVE_LANGUAGE_PACK) && !defined(BOLOS_OS_UPGRADER_APP))
