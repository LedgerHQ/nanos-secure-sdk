#pragma once

#include "appflags.h"
#include "bolos_target.h"
#include "decorators.h"
#include "os_types.h"

/* ----------------------------------------------------------------------- */
/* -                             PIN FEATURE                             - */
/* ----------------------------------------------------------------------- */

// Global PIN
#define DEFAULT_PIN_RETRIES 3

/* set_pin can update the pin if the perso is onboarded (tearing leads to perso
 * wipe though) */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_pin(
    unsigned int identity, unsigned char *pin PLENGTH(length),
    unsigned int length);
// set the currently unlocked identity pin. (change pin feature)
SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_current_identity_pin(
        unsigned char *pin PLENGTH(length), unsigned int length);

/*
 * @return BOLOS_UX_OK if pin validated
 */
SYSCALL bolos_bool_t os_global_pin_is_validated(void);
/**
 * Validating the pin also setup the identity linked with this pin (normal or
 * alternate)
 * @return BOLOS_UX_OK if pin validated
 */
SYSCALL PERMISSION(APPLICATION_FLAG_GLOBAL_PIN) bolos_bool_t
    os_global_pin_check(unsigned char *pin_buffer PLENGTH(pin_length),
                        unsigned char pin_length);
SYSCALL
    PERMISSION(APPLICATION_FLAG_GLOBAL_PIN) void os_global_pin_invalidate(void);
SYSCALL PERMISSION(
    APPLICATION_FLAG_GLOBAL_PIN) unsigned int os_global_pin_retries(void);

#if defined(HAVE_AEM_PIN)
/* ----------------------------------------------------------------------- */
/* -                  ANTI EVIL MAID (AEM) PIN FEATURE                   - */
/* ----------------------------------------------------------------------- */

/**
 * This feature involves two data elements:
 * - The AEM PIN,
 * - The AEM response.
 */

/**
 * The minimum length of an AEM PIN, in bytes.
 */
#define AEM_MIN_PIN_LENGTH 0x04

/**
 * The maximum length of a AEM PIN associated response, in bytes, including
 * the trailing end-of-string character (which is automatically added by
 * the OS at the end of an APDU buffer).
 */
#define AEM_RESPONSE_MAX_LENGTH 0x80

/**
 * The different supported formats for the AEM response, and the value of the
 * last one.
 */
#define AEM_RESPONSE_FORMAT_TEXT 0x01
#define AEM_RESPONSE_FORMAT_BMP_14x14 0x02
#define AEM_RESPONSE_FORMAT_LAST AEM_RESPONSE_FORMAT_BMP_14x14

/**
 * This function allows one to set a value to the AEM PIN, and activate the
 * associated feature.
 * The associated feature is activated when both the AEM PIN and the boot
 * PIN response have been set and are active.
 * This fonction can only be properly processed if the device is onboarded
 * and the classic PIN has been checked beforehand.
 * @param aem_pin The AEM PIN to be stored in persistent memory.
 * @param aem_pin_length The length of the to-be-set AEM PIN.
 * @throw INVALID_PARAMETER, when the submitted length is less than the
 * minimum accepted lenth for a PIN,
 * @throw SWO_PAR_LEN_13 if aem_pin_length < AEM_MIN_PIN_LENGTH,
 * @throw SWO_SEC_PIN_0D when either the device is not onboarded or the
 * user PIN has not been checked beforehand.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_aem_set_pin(
    unsigned char *aem_pin PLENGTH(aem_pin_length),
    unsigned int aem_pin_length);

/**
 * This function allows one to unset the AEM PIN value (in such a case, it
 * is erased), and deactivate the associated feature.
 * This fonction can only be properly processed if the device is onboarded
 * and the classic PIN has been checked beforehand.
 * @throw SWO_SEC_PIN_0E, when either the device is not onboarded or the
 * user PIN has not been checked beforehand.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_aem_unset_pin(void);

/**
 * This function allows one to know whether the AEM PIN has been set.
 * This fonction can only be properly processed if the device is onboarded
 * and the classic PIN has been checked beforehand.
 * @return BOLOS_TRUE if the AEM PIN has been set, any other value otherwise
 * (depending on the failed internal check).
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) bolos_bool_t
    os_aem_is_pin_set(void);

/**
 * This function allows one to set a value to the AEM response.
 * The associated feature is activated when both the AEM PIN and the boot
 * PIN response have been set and are active.
 * This fonction can only be properly processed if the device is onboarded
 * and the classic PIN has been checked beforehand.
 * It needs to be followed by a call to 'os_aem_pin_activate_response' in
 * order to be used afterwards.
 * @param aem_response The response to be stored in persistent memory.
 * @param aem_response_length The length of the to-be-set response.
 * @param aem_response_format The format of the to-be-set response.
 * @throw SWO_PAR_LEN_14, when the submitted length is more than the
 * maximum accepted lenth (AEM_RESPONSE_MAX_LENGTH bytes), or when the
 * submitted format is not valid,
 * @throw SWO_SEC_PIN_0F, when either the device is not onboarded or the
 * user PIN has not been checked beforehand.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_aem_set_response(
    unsigned char *aem_response PLENGTH(aem_response_length),
    unsigned int aem_response_length, unsigned char aem_response_format);

/**
 * This function allows one to activate the AEM response.
 * This fonction can only be properly processed if the device is onboarded,
 * the classic PIN has been checked beforehand and the length of the response
 * is not zero.
 * @throw SWO_SEC_PIN_10, when either the device is not onboarded, the
 * user PIN has not been checked beforehand or the length of the response is
 * zero.
 */
SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_aem_activate_response(void);

/**
 * This function allows one to deactivate the AEM response, and
 * deactivate the associated feature.
 * This fonction can only be properly processed if the device is onboarded
 * and the classic PIN has been checked beforehand.
 * @throw SWO_SEC_PIN_11, when either the device is not onboarded or the
 * user PIN has not been checked beforehand.
 */
SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_aem_deactivate_response(void);

/**
 * This function allows one to know whether the AEM response
 * has been set and active.
 * This fonction can only be properly processed if the device is onboarded
 * and the classic PIN has been checked beforehand.
 * @return BOLOS_TRUE if the response has been set and activated, any other
 * value otherwise (depending on the failed internal check).
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) bolos_bool_t
    os_aem_is_response_active(void);

/**
 * This function allows one to know whether the AEM PIN feature is active
 * (i.e. if the AEM PIN is set, and the response has been set and activated).
 * This fonction can only be properly processed if the device is onboarded
 * and the classic PIN has been checked beforehand.
 * @return BOLOS_TRUE if the feature is active, any other value otherwise
 * (depending on the failed internal check).
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) bolos_bool_t
    os_aem_is_feature_active(void);

/**
 * This function allows one to retrieve the length of the response.
 * It is used to know the maximum length which can be retrieved with the
 * 'os_aem_get_response' function.
 * @return The length of the currently stored response.
 */
SYSCALL PERMISSION(
    APPLICATION_FLAG_BOLOS_UX) unsigned char os_aem_get_response_length(void);

/**
 * This function allows one to retrieve the format of the response.
 * @return The format of the currently stored response.
 */
SYSCALL PERMISSION(
    APPLICATION_FLAG_BOLOS_UX) unsigned char os_aem_get_response_format(void);

/**
 * This function allows one to retrieve the response.
 * One can retrieve at most the whole response, but the first bytes can also
 * be retrieved, by specifying the number of bytes to retrieve.
 * The 'os_aem_get_response' function must be used before calling this
 * function to know the maximum retrievable length.
 * The caller is responsible for owning a RAM buffer large enough to store
 * the requested amount of bytes.
 * @param output_response_buffer The buffer in which the required response
 * bytes will be copied,
 * @param input_required_length The required amount of response bytes.
 * @throw SWO_PAR_LEN_15 if the required length is greater than the response
 * length.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_aem_get_response(
    char *output_response_buffer PLENGTH(input_required_length),
    unsigned char input_required_length);

/**
 * This function allows one to check a presented PIN against the internally
 * stored AEM PIN.
 * This function can only be properly processed if the device is onboarded
 * and if the AEM PIN has been set beforehand.
 * If the submitted PIN equals the AEM PIN, then the AEM PIN is flagged as
 * validated.
 * @param aem_pin_buffer The presented PIN to check against the AEM PIN,
 * @param aem_pin_length The presented PIN length.
 * @return BOLOS_TRUE if the submitted PIN equls the AEM PIN, any other
 * value otherwise (depending on the failed internal check).
 * @throw SWO_SEC_STA_0A when eiher the device is not onboarded or the
 * AEM PIN is not set.
 * @throw SWO_SEC_CHK_18 when the PIN verification detects an error in
 * the processing.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_GLOBAL_PIN) bolos_bool_t
    os_aem_check_pin(unsigned char *aem_pin_buffer PLENGTH(aem_pin_length),
                     unsigned char aem_pin_length);

/**
 * This function allows one to invalidate the AEM PIN, regardless of its
 * validation state.
 */
SYSCALL
    PERMISSION(APPLICATION_FLAG_GLOBAL_PIN) void os_aem_invalidate_pin(void);

/**
 * This function allows one to retrieve the remaining attempts at verifying
 * a PIN against the AEM PIN.
 * This funtion can only be properly processed if the device is onboarded
 * and if the AEM PIN has been set beforehand.
 * @return The AEM PIN try counter.
 * @throw SWO_SEC_PIN_12 when eiher the device is not onboarded or the
 * AEM PIN is not set.
 */
SYSCALL
    PERMISSION(APPLICATION_FLAG_GLOBAL_PIN) unsigned int os_aem_get_ptc(void);

/**
 * This function allows one to check whether the AEM PIN is currently
 * considered validated.
 * @return BOLOS_TRUE if the AEM PIN has been activated and is validated,
 * any other value otherwise (depending on the internal failed check).
 */
SYSCALL
bolos_bool_t os_aem_is_pin_validated(void);

#endif // HAVE_AEM_PIN
