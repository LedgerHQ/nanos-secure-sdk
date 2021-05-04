#pragma once

#include "appflags.h"
#include "bolos_target.h"
#include "decorators.h"
#include "os_types.h"

typedef unsigned char bolos_task_status_t;

enum task_unsecure_id_e {
  TASK_BOLOS = 0, // can call os
  TASK_SYSCALL,   // can call os
  TASK_USERTASKS_START,
  // disabled for now // TASK_USER_UX, // must call syscalls to reach os, locked
  // in ux ram
  TASK_USER =
      TASK_USERTASKS_START, // must call syscalls to reach os, locked in app ram
  TASK_SUBTASKS_START,
  TASK_SUBTASK_0 = TASK_SUBTASKS_START,
#ifdef TARGET_NANOX
  TASK_SUBTASK_1,
  TASK_SUBTASK_2,
  TASK_SUBTASK_3,
#endif // TARGET_NANOX
  TASK_BOLOS_UX,
  TASK_MAXCOUNT, // must be last in the structure
};

// execute the given application index in the registry, this function kills the
// current app task
SYSCALL TASKSWITCH PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_sched_exec(
    unsigned int app_idx);
// exit the current task
SYSCALL void os_sched_exit(bolos_task_status_t exit_code);

// returns true when the given task is running, false else.
SYSCALL bolos_bool_t os_sched_is_running(unsigned int task_idx);

/**
 * Retrieve the last status issued by a task using either yield or exit.
 */
SUDOCALL bolos_task_status_t os_sched_last_status(unsigned int task_idx);

/**
 * Current task is yielding the process to another task.
 * Meta call for task_switch with 'the enxt' task idx.
 * @param status is the current task status
 */
SUDOCALL TASKSWITCH void os_sched_yield(bolos_task_status_t status);

/**
 * Perform task switching
 * @param task_idx is the task index to switch to
 * @param status of the currently executed task
 * @return the status of the previously running task
 */
SUDOCALL TASKSWITCH void os_sched_switch(unsigned int task_idx,
                                         bolos_task_status_t status);

/**
 * Function that returns the currently running task identifier.
 */
SUDOCALL unsigned int os_sched_current_task(void);

/**
 * Create a new task with the given parameters and return its task identifier.
 * The newly created task is chrooted in the given nvram/ram1/ram2 segments
 * and its task pointer is set at the end of ram1 segment.
 * The task is bound to the currently running application.
 * The task identifiers are not garanteed to be the same after a power cycle.
 * At least valid main, nvram segment, ram0 segment and stack segment must be
 * provided with.
 * @param main The main function address to start the task with.
 * @param nvram The nvram segment address start
 * @param nvram_length The nvram segment length
 * @param ram0 /ram0_length the first RAM segment description
 * @param stack /stack_length the task's stack RAM segment description
 */
SYSCALL unsigned int
os_sched_create(void *main PLENGTH(4), void *nvram PLENGTH(nvram_length),
                unsigned int nvram_length, void *ram0 PLENGTH(ram0_length),
                unsigned int ram0_length, void *stack PLENGTH(stack_length),
                unsigned int stack_length);

// kill a task
SYSCALL void os_sched_kill(unsigned int taskidx);

typedef struct {
  void (*asynchmodal_end_callback)(unsigned int ux_status);
} bolos_ux_asynch_callback_t;

extern bolos_ux_asynch_callback_t G_io_asynch_ux_callback;
