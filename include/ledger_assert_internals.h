#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "os_print.h"

/*******************************
 * Default configuration level *
 ******************************/
#ifdef LEDGER_ASSERT_CONFIG_FILE_INFO
#define LEDGER_ASSERT_CONFIG_MESSAGE_INFO   1
#define LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO 1
#endif

#ifdef LEDGER_ASSERT_CONFIG_MESSAGE_INFO
#define LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO 1
#endif

/*********************************************************************
 * Compute needed buffer length to display the assert info on screen *
 ********************************************************************/
#ifdef HAVE_LEDGER_ASSERT_DISPLAY

#ifdef LEDGER_ASSERT_CONFIG_MESSAGE_INFO
#if defined(TARGET_NANOS)
// Spare RAM on NANOS
#define MESSAGE_SIZE 20
#else
#define MESSAGE_SIZE 50
#endif
#else
#define MESSAGE_SIZE 0
#endif

#ifdef LEDGER_ASSERT_CONFIG_FILE_INFO
#define FILE_SIZE 50
#else
#define FILE_SIZE 0
#endif

#ifdef LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO
#define LR_AND_PC_SIZE 30
#else
#define LR_AND_PC_SIZE 0
#endif

#define ASSERT_BUFFER_LEN LR_AND_PC_SIZE + MESSAGE_SIZE + FILE_SIZE
extern char assert_buffer[ASSERT_BUFFER_LEN];

#endif  // HAVE_LEDGER_ASSERT_DISPLAY

/************************************************************
 * Define behavior when LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO *
 ***********************************************************/
#ifdef LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO

#ifdef HAVE_PRINTF
void assert_print_lr_and_pc(int lr, int pc);
#define ASSERT_PRINT_LR_AND_PC(lr, pc) assert_print_lr_and_pc(lr, pc)
#else
#define ASSERT_PRINT_LR_AND_PC(lr, pc) \
    do {                               \
    } while (0)
#endif

#ifdef HAVE_LEDGER_ASSERT_DISPLAY
void assert_display_lr_and_pc(int lr, int pc);
#define ASSERT_DISPLAY_LR_AND_PC(lr, pc) assert_display_lr_and_pc(lr, pc)
#else
#define ASSERT_DISPLAY_LR_AND_PC(lr, pc) \
    do {                                 \
    } while (0)
#endif

#define LEDGER_ASSERT_LR_AND_PC_PREAMBLE()            \
    int _lr_address = 0;                              \
    int _pc_address = 0;                              \
                                                      \
    __asm volatile("mov %0, lr" : "=r"(_lr_address)); \
    __asm volatile("mov %0, pc" : "=r"(_pc_address))

#define LEDGER_ASSERT_LR_AND_PC()                           \
    do {                                                    \
        ASSERT_PRINT_LR_AND_PC(_lr_address, _pc_address);   \
        ASSERT_DISPLAY_LR_AND_PC(_lr_address, _pc_address); \
    } while (0)

#else  // LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO

#define LEDGER_ASSERT_LR_AND_PC_PREAMBLE() \
    do {                                   \
    } while (0)

#define LEDGER_ASSERT_LR_AND_PC() \
    do {                          \
    } while (0)

#endif  // LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO

/**********************************************************
 * Define behavior when LEDGER_ASSERT_CONFIG_MESSAGE_INFO *
 *********************************************************/
#ifdef LEDGER_ASSERT_CONFIG_MESSAGE_INFO

#ifdef HAVE_PRINTF
#define ASSERT_PRINT_MESSAGE(format, ...) \
    do {                                  \
        PRINTF(format, ##__VA_ARGS__);    \
        PRINTF("\n");                     \
    } while (0)
#else
#define ASSERT_PRINT_MESSAGE(...) \
    do {                          \
    } while (0)
#endif

#ifdef HAVE_LEDGER_ASSERT_DISPLAY
// Immediately call snprintf here (no function wrapping it cleanly in ledger_assert.c).
// This is because we don't have an vsnprintf implementation which would be needed if
// we were to pass the va_args to an intermediate function.
// See https://stackoverflow.com/a/150578
#define ASSERT_DISPLAY_MESSAGE(format, ...)                           \
    do {                                                              \
        snprintf(assert_buffer, MESSAGE_SIZE, format, ##__VA_ARGS__); \
        strncat(assert_buffer, "\n", MESSAGE_SIZE);                   \
    } while (0)
#else
#define ASSERT_DISPLAY_MESSAGE(format, ...) \
    do {                                    \
    } while (0)
#endif

#define LEDGER_ASSERT_MESSAGE(format, ...)                 \
    do {                                                   \
        if (format != NULL) {                              \
            ASSERT_PRINT_MESSAGE(format, ##__VA_ARGS__);   \
            ASSERT_DISPLAY_MESSAGE(format, ##__VA_ARGS__); \
        }                                                  \
    } while (0)

#else  // LEDGER_ASSERT_CONFIG_MESSAGE_INFO

#define LEDGER_ASSERT_MESSAGE(...) \
    do {                           \
    } while (0)

#endif  // LEDGER_ASSERT_CONFIG_MESSAGE_INFO

/*******************************************************
 * Define behavior when LEDGER_ASSERT_CONFIG_FILE_INFO *
 ******************************************************/
#ifdef LEDGER_ASSERT_CONFIG_FILE_INFO

#ifdef HAVE_PRINTF
void assert_print_file_info(const char *file, int line);
#define ASSERT_PRINT_FILE_INFO(file, line) assert_print_file_info(file, line)
#else
#define ASSERT_PRINT_FILE_INFO(file, line) \
    do {                                   \
    } while (0)
#endif

#ifdef HAVE_LEDGER_ASSERT_DISPLAY
void assert_display_file_info(const char *file, unsigned int line);
#define ASSERT_DISPLAY_FILE_INFO(file, line) assert_display_file_info(file, line)
#else
#define ASSERT_DISPLAY_FILE_INFO(file, line) \
    do {                                     \
    } while (0)
#endif

#define LEDGER_ASSERT_FILE_INFO()                     \
    do {                                              \
        ASSERT_PRINT_FILE_INFO(__FILE__, __LINE__);   \
        ASSERT_DISPLAY_FILE_INFO(__FILE__, __LINE__); \
    } while (0)

#else  // LEDGER_ASSERT_CONFIG_FILE_INFO

#define LEDGER_ASSERT_FILE_INFO() \
    do {                          \
    } while (0)

#endif  // LEDGER_ASSERT_CONFIG_FILE_INFO

/********************************************************
 * Define exit behavior when HAVE_LEDGER_ASSERT_DISPLAY *
 *******************************************************/
#ifdef HAVE_LEDGER_ASSERT_DISPLAY
void __attribute__((noreturn)) assert_display_exit(void);
#define LEDGER_ASSERT_EXIT() assert_display_exit()

#else  // HAVE_LEDGER_ASSERT_DISPLAY

void assert_exit(bool confirm);
#define LEDGER_ASSERT_EXIT() assert_exit(true)

#endif  // HAVE_LEDGER_ASSERT_DISPLAY

/*************************************
 * Specific mechanism to debug THROW *
 ************************************/
#ifdef HAVE_DEBUG_THROWS

#ifdef HAVE_PRINTF
void throw_print_lr(int e, int lr);
#define THROW_PRINT_LR(e, lr_val) throw_print_lr(e, lr_val)
#else
#define THROW_PRINT_LR(e, lr_val) \
    do {                          \
    } while (0)
#endif

void __attribute__((noreturn)) assert_display_exit(void);
void throw_display_lr(int e, int lr);

#define DEBUG_THROW(e)                               \
    do {                                             \
        unsigned int lr_val;                         \
        __asm volatile("mov %0, lr" : "=r"(lr_val)); \
        throw_display_lr(e, lr_val);                 \
        THROW_PRINT_LR(e, lr_val);                   \
    } while (0)

#endif  // HAVE_DEBUG_THROWS
