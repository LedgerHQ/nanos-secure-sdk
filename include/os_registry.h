#pragma once

#include "appflags.h"
#include "decorators.h"
#include "os_app.h"

/* ----------------------------------------------------------------------- */
/* -                          REGISTRY-RELATED                           - */
/* ----------------------------------------------------------------------- */
// read from the application's install parameter TLV (if present), else 0 is
// returned (no exception thrown). takes into account the currently loaded
// application
#define OS_REGISTRY_GET_TAG_OFFSET_COMPARE_WITH_BUFFER (0x80000000UL)
#define OS_REGISTRY_GET_TAG_OFFSET_GET_LENGTH (0x40000000UL)

/**
 * @param appidx The application entry index in the registry (raw, not filtering
 * ux or whatever). If the entry index correspond to the application being
 * installed then RAM structure content is used instead of the NVRAM registry.
 * @param tlvoffset The offset within the install parameters memory area, in
 * bytes. Useful if tag is present multiple times. Can be null. The tlv offset
 * is the offset of the tag in the install parameters area when a tag is
 * matched. This way long tag can be read in multiple time without the need to
 * play with the tlvoffset. Add +1 to skip to the next one when seraching for
 * multiple tag occurences.
 * @param tag The tag to be searched for
 * @param value_offset The offset within the value for this occurence of the
 * tag. The OS_REGISTRY_GET_TAG_OFFSET_COMPARE_WITH_BUFFER or
 * OS_REGISTRY_GET_TAG_OFFSET_GET_LENGTH can be ORed to perform meta operation
 * on the TLV occurence.
 * @param buffer The user buffer for comparison or to retrieve the value of the
 * tag at the given offset.
 * @param maxlength Size of the buffer to be compared OR to be retrieved
 * (trimmed depending the TLV effective length).
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) unsigned int os_registry_get_tag(
    unsigned int app_idx, unsigned int *tlvoffset, unsigned int tag,
    unsigned int value_offset, void *buffer PLENGTH(maxlength),
    unsigned int maxlength);

// Copy the currently running application tag from its install parameters to the
// given user buffer. Only APPNAME/APPVERSION/DERIVEPATH/ICON tags are
// retrievable to avoid install parameters private information. Warning: this
// function returns tag content of the application lastly scheduled to run, not
// the tag content of the currently executed piece of code (libraries subcalls)
SYSCALL unsigned int
os_registry_get_current_app_tag(unsigned int tag,
                                unsigned char *buffer PLENGTH(maxlen),
                                unsigned int maxlen);

/**
 * This function deletes the application mentioned with its index, as well as
 * all its dependees (the applications which depend on it).
 * @param index The index of the the application to suppress.
 */
SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_registry_delete_app_and_dependees(
        unsigned int app_idx);

/**
 * This function deletes all the installed applications.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_registry_delete_all_apps(
    void);

SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) unsigned int os_registry_count(void);
// return any entry, activated or not, enabled or not, empty or not. to enable
// full control
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_registry_get(
    unsigned int app_idx,
    application_t *out_application_entry PLENGTH(sizeof(application_t)));
