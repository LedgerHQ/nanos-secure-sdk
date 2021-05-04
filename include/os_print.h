#pragma once

#include <stdio.h>

// avoid typing the size each time
#define SPRINTF(strbuf, ...) snprintf(strbuf, sizeof(strbuf), __VA_ARGS__)
