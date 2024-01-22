#pragma once

#include <stdio.h>

// avoid typing the size each time
#define SPRINTF(strbuf, ...) snprintf(strbuf, sizeof(strbuf), __VA_ARGS__)

#ifdef HAVE_PRINTF
void screen_printf(const char *format, ...);
void mcu_usb_printf(const char *format, ...);
#else  // !HAVE_PRINTF
#define PRINTF(...)
#endif  // !HAVE_PRINTF
