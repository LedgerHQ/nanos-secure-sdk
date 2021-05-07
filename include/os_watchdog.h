#pragma once

/* ----------------------------------------------------------------------- */
/* -                         PRECISE WATCHDOG                            - */
/* ----------------------------------------------------------------------- */

#ifndef BOLOS_SECURITY_BOOT_DELAY_H
#ifdef BOLOS_RELEASE
// Boot delay before wiping the fault detection counter
#define BOLOS_SECURITY_BOOT_DELAY_H 5
#else // BOLOS_RELEASE
#define BOLOS_SECURITY_BOOT_DELAY_H 1 - (60 * 60 * 100) + 15 * 100
#endif // BOLOS_RELEASE
#endif // BOLOS_SECURITY_BOOT_DELAY_H
#ifndef BOLOS_SECURITY_ONBOARD_DELAY_S
#ifdef BOLOS_RELEASE
// Minimal time for an onboard
#define BOLOS_SECURITY_ONBOARD_DELAY_S (2 * 60)
#else // BOLOS_RELEASE
// small overhead in dev
#define BOLOS_SECURITY_ONBOARD_DELAY_S 5
#endif // BOLOS_RELEASE
#endif // BOLOS_SECURITY_ONBOARD_DELAY_S

#ifndef BOLOS_SECURITY_ATTESTATION_DELAY_S
// Mininal time interval in between two use of the device's private key (SCP
// opening and endorsement)
#define BOLOS_SECURITY_ATTESTATION_DELAY_S 5
#endif // BOLOS_SECURITY_ATTESTATION_DELAY_S

void safe_desynch(void);
#define SAFE_DESYNCH() safe_desynch()

typedef enum {
  /* Watchdog consumption lead to no action being taken, overflowed value is
     accounted and can be retrieved by the application */
  OS_WATCHDOG_NOACTION = 0,
  /* Request a platform reset when the watchdog set value is completely consumed
   */
  OS_WATCHDOG_RESET = 1,
  /* Request a wipe of the user data when the watchdog times out */
  OS_WATCHDOG_WIPE = 2,
} os_watchdog_behavior_t;

/**
 * This function arm a low level watchdog, when the value is consumed, and
 * depending on the requested behavior, an action can be taken.
 * @throw SWO_PAR_VAL_03 when the useconds value overflows the possible value.
 */
void os_watchdog_arm(unsigned int useconds, os_watchdog_behavior_t behavior);

/**
 * This function returns the number of useconds to be still consumed by the
 * watchdog (when > 0), or the overflowed useconds after the watchdog has timed
 * out (when < 0)
 */
int os_watchdog_value(void);
