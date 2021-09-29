#pragma once

#ifndef SYSCALL
// #define SYSCALL syscall
#define SYSCALL
#endif

#ifndef TASKSWITCH
// #define TASKSWITCH taskswitch
#define TASKSWITCH
#endif

#ifndef SUDOCALL
// #define SUDOCALL sudocall
#define SUDOCALL
#endif

#ifndef LIBCALL
// #define LIBCALL libcall
#define LIBCALL
#endif

#ifndef SHARED
// #define SHARED shared
#define SHARED
#endif

#ifndef PERMISSION
#define PERMISSION(...)
#endif

#ifndef PLENGTH
#define PLENGTH(...)
#endif

#ifndef CXPORT
#define CXPORT(...)
#endif

#ifndef TASKLEVEL
#define TASKLEVEL(...)
#endif
