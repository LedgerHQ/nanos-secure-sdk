#pragma once

#include "appflags.h"
#include "bolos_target.h"
#include "decorators.h"
#include "os_task.h"

/* ----------------------------------------------------------------------- */
/* -                            ID FUNCTIONS                             - */
/* ----------------------------------------------------------------------- */
#define OS_FLAG_RECOVERY 1
#define OS_FLAG_SIGNED_MCU_CODE 2
#define OS_FLAG_ONBOARDED 4
#define OS_FLAG_PIN_VALIDATED 128
//#define OS_FLAG_CUSTOM_UX       4
/* Enable application to retrieve OS current running options */
SYSCALL unsigned int os_flags(void);
SYSCALL unsigned int os_version(unsigned char *version PLENGTH(maxlength),
                                unsigned int maxlength);
/* Grab the SE serial number */
SYSCALL unsigned int os_serial(unsigned char *serial PLENGTH(maxlength),
                               unsigned int maxlength);
#ifdef HAVE_MCU_SERIAL_STORAGE
/* Grab the SEPROXYHAL's MCU serial number */
SYSCALL unsigned int os_seph_serial(unsigned char *serial PLENGTH(maxlength),
                                    unsigned int maxlength);
#endif // HAVE_MCU_SERIAL_STORAGE
       /* Grab the SEPROXYHAL's feature set */
SYSCALL unsigned int os_seph_features(void);
/* Grab the SEPROXYHAL's version */
SYSCALL unsigned int os_seph_version(unsigned char *version PLENGTH(maxlength),
                                     unsigned int maxlength);
/* Grab the MCU bootloader's version */
SYSCALL unsigned int
os_bootloader_version(unsigned char *version PLENGTH(maxlength),
                      unsigned int maxlength);

/*
 * Copy the serial number in the given buffer and return its length
 */
unsigned int os_get_sn(unsigned char *buffer);

// get API level
SYSCALL unsigned int get_api_level(void);

#ifndef HAVE_BOLOS
static inline void check_api_level(unsigned int apiLevel) {
  if (apiLevel < get_api_level()) {
    os_sched_exit(-1);
  }
}
#endif
