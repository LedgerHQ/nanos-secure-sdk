#pragma once

/**
 * Macro for the size of a specific structure field.
 */
#define MEMBER_SIZE(type, member) (sizeof(((type *) 0)->member))

#define WEAK __attribute((weak))

#define ARRAY_LENGTH(array) (sizeof((array)) / sizeof((array)[0]))
