#pragma once

#include <stdbool.h>

#ifdef HAVE_PRINTF
#include "os.h"
#endif

#ifdef LEDGER_ASSERT_CONFIG_FILE_INFO
#define LEDGER_ASSERT_CONFIG_MESSAGE_INFO   1
#define LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO 1
#endif

#ifdef LEDGER_ASSERT_CONFIG_MESSAGE_INFO
#define LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO 1
#endif

#if defined(LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO) && defined(HAVE_LEDGER_ASSERT_DISPLAY)
#define LR_AND_PC_SIZE 30
void assert_display_lr_and_pc(int lr, int pc);
#define ASSERT_DISPLAY_LR_AND_PC(lr, pc) assert_display_lr_and_pc(lr, pc)
#else
#define LR_AND_PC_SIZE 0
#define ASSERT_DISPLAY_LR_AND_PC(lr, pc) \
    do {                                 \
    } while (0)
#endif

#if defined(LEDGER_ASSERT_CONFIG_MESSAGE_INFO) && defined(HAVE_LEDGER_ASSERT_DISPLAY)
#define MESSAGE_SIZE 20
void assert_display_message(const char *message);
#define ASSERT_DISPLAY_MESSAGE(message) assert_display_message(message)
#else
#define MESSAGE_SIZE 0
#define ASSERT_DISPLAY_MESSAGE(message) \
    do {                                \
    } while (0)
#endif

#if defined(LEDGER_ASSERT_CONFIG_FILE_INFO) && defined(HAVE_LEDGER_ASSERT_DISPLAY)
#define FILE_SIZE 50
void assert_display_file_info(const char *file, unsigned int line);
#define ASSERT_DISPLAY_FILE_INFO(file, line) assert_display_file_info(file, line)
#else
#define FILE_SIZE 0
#define ASSERT_DISPLAY_FILE_INFO(file, line) \
    do {                                     \
    } while (0)
#endif

#ifdef HAVE_LEDGER_ASSERT_DISPLAY
#define ASSERT_BUFFER_LEN LR_AND_PC_SIZE + MESSAGE_SIZE + FILE_SIZE
void __attribute__((noreturn)) assert_display_exit(void);

#define LEDGER_ASSERT_EXIT() assert_display_exit()
#else
void assert_exit(bool confirm);
#define LEDGER_ASSERT_EXIT() assert_exit(true)
#endif

#if defined(LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO) && defined(HAVE_PRINTF)
void assert_print_lr_and_pc(int lr, int pc);
#define ASSERT_PRINT_LR_AND_PC(lr, pc) assert_print_lr_and_pc(lr, pc)
#else
#define ASSERT_PRINT_LR_AND_PC(lr, pc) \
    do {                               \
    } while (0)
#endif

#if defined(LEDGER_ASSERT_CONFIG_MESSAGE_INFO) && defined(HAVE_PRINTF)
void assert_print_message(const char *message);
#define ASSERT_PRINT_MESSAGE(message) assert_print_message(message)
#else
#define ASSERT_PRINT_MESSAGE(message) \
    do {                              \
    } while (0)
#endif

#if defined(LEDGER_ASSERT_CONFIG_FILE_INFO) && defined(HAVE_PRINTF)
void assert_print_file_info(const char *file, int line);
#define ASSERT_PRINT_FILE_INFO(file, line) assert_print_file_info(file, line)
#else
#define ASSERT_PRINT_FILE_INFO(file, line) \
    do {                                   \
    } while (0)
#endif

#ifdef LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO
#define LEDGER_ASSERT_LR_AND_PC()                           \
    do {                                                    \
        int _lr_address = 0;                                \
        int _pc_address = 0;                                \
                                                            \
        __asm volatile("mov %0, lr" : "=r"(_lr_address));   \
        __asm volatile("mov %0, pc" : "=r"(_pc_address));   \
        ASSERT_PRINT_LR_AND_PC(_lr_address, _pc_address);   \
        ASSERT_DISPLAY_LR_AND_PC(_lr_address, _pc_address); \
    } while (0)
#elif defined(HAVE_PRINTF)
#define LEDGER_ASSERT_LR_AND_PC() PRINTF("LEDGER_ASSERT FAILED\n")
#else
#define LEDGER_ASSERT_LR_AND_PC() \
    do {                          \
    } while (0)
#endif

#ifdef LEDGER_ASSERT_CONFIG_MESSAGE_INFO
#define LEDGER_ASSERT_MESSAGE(message)   \
    do {                                 \
        ASSERT_PRINT_MESSAGE(message);   \
        ASSERT_DISPLAY_MESSAGE(message); \
    } while (0)
#else
#define LEDGER_ASSERT_MESSAGE(message) \
    do {                               \
    } while (0)
#endif

#ifdef LEDGER_ASSERT_CONFIG_FILE_INFO
#define LEDGER_ASSERT_FILE_INFO()                     \
    do {                                              \
        ASSERT_PRINT_FILE_INFO(__FILE__, __LINE__);   \
        ASSERT_DISPLAY_FILE_INFO(__FILE__, __LINE__); \
    } while (0)
#else
#define LEDGER_ASSERT_FILE_INFO() \
    do {                          \
    } while (0)
#endif

#define LEDGER_ASSERT(test, message)        \
    do {                                    \
        if (!(test)) {                      \
            LEDGER_ASSERT_LR_AND_PC();      \
            LEDGER_ASSERT_MESSAGE(message); \
            LEDGER_ASSERT_FILE_INFO();      \
            LEDGER_ASSERT_EXIT();           \
        }                                   \
    } while (0)

#if defined(HAVE_DEBUG_THROWS) && defined(HAVE_PRINTF)
void throw_print_lr(int e, int lr);
#define THROW_PRINT_LR(e, lr_val) throw_print_lr(e, lr_val)
#else
#define THROW_PRINT_LR(e, lr_val) \
    do {                          \
    } while (0)
#endif

#if defined(HAVE_DEBUG_THROWS)
void __attribute__((noreturn)) assert_display_exit(void);
void throw_display_lr(int e, int lr);
#define DEBUG_THROW(e)                               \
    do {                                             \
        unsigned int lr_val;                         \
        __asm volatile("mov %0, lr" : "=r"(lr_val)); \
        throw_display_lr(e, lr_val);                 \
        THROW_PRINT_LR(e, lr_val);                   \
    } while (0)
#endif
