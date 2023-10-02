#pragma once

#include "appflags.h"
#include "decorators.h"

/* ----------------------------------------------------------------------- */
/* -                         SETTINGS FUNCTIONS                          - */
/* ----------------------------------------------------------------------- */

#define IS_NOTIF_ENABLED(setting) ((setting & 0b10)?false:true)
#define IS_TAP_ENABLED(setting) ((setting & 0b01)?false:true)
#define GET_SOUND_SETTING_VALUE(notif_setting, feedback_setting) ((uint32_t) ((!(feedback_setting& 0b01)) | ((!(notif_setting) << 1) & 0b10)))

// Keep a V0 of the settings to make updates easier
// Balenos_v2.0.2
typedef enum os_setting_v0_e {
  OS_SETTING_BRIGHTNESS_V0,
  OS_SETTING_INVERT_V0,
  OS_SETTING_ROTATION_V0,
  OS_SETTING_MCU_UPGRADE_V0=OS_SETTING_ROTATION_V0,
#ifdef HAVE_BOLOS_NOT_SHUFFLED_PIN
  OS_SETTING_NOSHUFFLE_PIN_V0,
#endif // HAVE_BOLOS_NOT_SHUFFLED_PIN
  OS_SETTING_AUTO_LOCK_DELAY_V0,
  OS_SETTING_SCREEN_LOCK_DELAY_V0,
  OS_SETTING_POWER_OFF_DELAY_V0,

  OS_SETTING_PLANEMODE_V0,

  // default off
  OS_SETTING_PRIVACY_MODE_V0,

  // before that value, all settings are only making use of the length value with a null buffer to be set, and are returned through the return value with a maxlength = 0 in the get.
  OS_SETTING_LAST_INT_V0,

  // screen saver string to display
  OS_SETTING_SAVER_STRING_V0=OS_SETTING_LAST_INT_V0,
  OS_SETTING_DEVICENAME_V0,
  OS_SETTING_BLEMACADR_V0,

  OS_SETTING_LAST_V0,
} os_setting_v0_t;

typedef enum os_setting_v1_e {
  OS_SETTING_BRIGHTNESS_V1,
  OS_SETTING_INVERT_V1,
  OS_SETTING_ROTATION_V1,
  OS_SETTING_MCU_UPGRADE_V1=OS_SETTING_ROTATION_V1,
#ifdef HAVE_BOLOS_NOT_SHUFFLED_PIN
  OS_SETTING_NOSHUFFLE_PIN_V1,
#endif // HAVE_BOLOS_NOT_SHUFFLED_PIN
  OS_SETTING_AUTO_LOCK_DELAY_V1,
  OS_SETTING_SCREEN_LOCK_DELAY_V1,
  OS_SETTING_POWER_OFF_DELAY_V1,

  OS_SETTING_PLANEMODE_V1,

  // default off
  OS_SETTING_PRIVACY_MODE_V1,

  OS_SETTING_LANGUAGE_V1,

  // before that value, all settings are only making use of the length value with a null buffer to be set, and are returned through the return value with a maxlength = 0 in the get.
  OS_SETTING_LAST_INT_V1,

  // screen saver string to display
  OS_SETTING_SAVER_STRING_V1=OS_SETTING_LAST_INT_V1,
  OS_SETTING_DEVICENAME_V1,
  OS_SETTING_BLEMACADR_V1,

  OS_SETTING_LAST_V1,
} os_setting_v1_t;

typedef enum os_setting_e {
  OS_SETTING_BRIGHTNESS,
  OS_SETTING_INVERT,
  OS_SETTING_ROTATION,
  OS_SETTING_MCU_UPGRADE=OS_SETTING_ROTATION,
  OS_SETTING_AUTO_LOCK_DELAY,
  OS_SETTING_SCREEN_LOCK_DELAY,
  OS_SETTING_POWER_OFF_DELAY,

  OS_SETTING_PLANEMODE,

  // default off
  OS_SETTING_PRIVACY_MODE,

  OS_SETTING_LANGUAGE,

  OS_SETTING_PIEZO_SOUND,

  // before that value, all settings are only making use of the length value with a null buffer to be set, and are returned through the return value with a maxlength = 0 in the get.
  OS_SETTING_LAST_INT,

  // screen saver string to display
  OS_SETTING_SAVER_STRING=OS_SETTING_LAST_INT,
  OS_SETTING_DEVICENAME,
  OS_SETTING_BLEMACADR,

  OS_SETTING_NFC_TAG_CONTENT,
  OS_SETTING_FEATURES,

  OS_SETTING_LAST,
} os_setting_t;

/**
 * @brief Bitfield for features in OS_SETTING_FEATURES
 *
 */
// if (os_settings[OS_SETTING_FEATURES] & OS_SETTING_FEATURES_NFC_ENABLED) then NFC is enabled
#define OS_SETTING_FEATURES_NFC_ENABLED     0x1
// if (os_settings[OS_SETTING_FEATURES] & OS_SETTING_FEATURES_NO_PIN_SHUFFLE) then PIN shuffle is disable
#define OS_SETTING_FEATURES_NO_PIN_SHUFFLE  0x2
// if (os_settings[OS_SETTING_FEATURES] & OS_SETTING_FEATURES_BATT_IN_SIDE) then Battery percentage in side screen is enabled
#define OS_SETTING_FEATURES_BATT_IN_SIDE    0x4

/**
 * Retrieve the value of a setting in a user specified buffer, with a max length, and return the effective returned length.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_SETTINGS) unsigned int os_setting_get(unsigned int setting_id, unsigned char* value PLENGTH(maxlen), unsigned int maxlen);

/**
 * Define a setting's value from a user buffer and its length. In case of error, a throw is executed.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_SETTINGS) void os_setting_set(unsigned int setting_id, unsigned char* value PLENGTH(length), unsigned int length);

#ifdef HAVE_LANGUAGE_PACK
// Shift preferences settings (could be needed after an update)
void os_setting_update(void);

// Prototypes for language helper functions
unsigned int get_os_language(void);
void set_os_language(unsigned int language);
#endif //HAVE_LANGUAGE_PACK

unsigned int os_setting_get_default_device_name(unsigned char* buffer, unsigned int maxlength);
