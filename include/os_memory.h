#pragma once

#include "appflags.h"
#include "decorators.h"

typedef struct meminfo_s {
  unsigned int free_nvram_size;
  unsigned int appMemory;
  unsigned int systemSize;
  unsigned int slots;
} meminfo_t;

SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_get_memory_info(
    meminfo_t *meminfo PLENGTH(sizeof(meminfo_t)));
