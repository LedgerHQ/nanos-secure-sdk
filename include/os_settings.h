#pragma once

#include "appflags.h"
#include "decorators.h"

/* ----------------------------------------------------------------------- */
/* -                         SETTINGS FUNCTIONS                          - */
/* ----------------------------------------------------------------------- */
typedef enum os_setting_e {
  OS_SETTING_BRIGHTNESS,
  OS_SETTING_INVERT,
  OS_SETTING_ROTATION,
#ifdef HAVE_BOLOS_NOT_SHUFFLED_PIN
  OS_SETTING_NOSHUFFLE_PIN,
#endif // HAVE_BOLOS_NOT_SHUFFLED_PIN
  OS_SETTING_AUTO_LOCK_DELAY,
  OS_SETTING_SCREEN_LOCK_DELAY,
  OS_SETTING_POWER_OFF_DELAY,

  OS_SETTING_PLANEMODE,

  // default off
  OS_SETTING_PRIVACY_MODE,

#ifdef HAVE_MCU_UPGRADE
  OS_SETTING_MCU_UPGRADE,
#endif // HAVE_MCU_UPGRADE

  // before that value, all settings are only making use of the length value
  // with a null buffer to be set, and are returned through the return value
  // with a maxlength = 0 in the get.
  OS_SETTING_LAST_INT,

  // screen saver string to display
  OS_SETTING_SAVER_STRING = OS_SETTING_LAST_INT,
  OS_SETTING_DEVICENAME,
  OS_SETTING_BLEMACADR,

  OS_SETTING_LAST,
} os_setting_t;

/**
 * Retrieve the value of a setting in a user specified buffer, with a max
 * length, and return the effective returned length.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_SETTINGS) unsigned int os_setting_get(
    unsigned int setting_id, unsigned char *value PLENGTH(maxlen),
    unsigned int maxlen);

/**
 * Define a setting's value from a user buffer and its length. In case of error,
 * a throw is executed.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_SETTINGS) void os_setting_set(
    unsigned int setting_id, unsigned char *value PLENGTH(length),
    unsigned int length);
