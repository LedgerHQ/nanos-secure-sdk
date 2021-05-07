#pragma once

#include "appflags.h"
#include "bolos_target.h"
#include "decorators.h"
#include "errors.h"

// error type definition
typedef unsigned short exception_t;

typedef struct try_context_s try_context_t;

#if (defined(ST31) || defined(ST33) || defined(STM32)) && defined(__arm__)
//#include <setjmp.h>
// GCC/LLVM declare way too big jmp context, reduce them to what is used on CM0+

// jmp context to backup (in increasing order address: r4, r5, r6, r7, r8, r9,
// r10, r11, SP, setjmpcallPC)
typedef unsigned int jmp_buf[10];

// borrowed from setjmp.h

#ifdef __GNUC__
void longjmp(jmp_buf __jmpb, int __retval) __attribute__((__noreturn__));
#else
void longjmp(jmp_buf __jmpb, int __retval);
#endif
int setjmp(jmp_buf __jmpb);
#else
#include <setjmp.h>
#endif

struct try_context_s {
  jmp_buf jmp_buf;

  // link to the previous jmp_buf context
  try_context_t *previous;

  // current exception
  exception_t ex;
};

// workaround to make sure defines are replaced by their value for example
#define CPP_CONCAT(x, y) CPP_CONCAT_x(x, y)
#define CPP_CONCAT_x(x, y) x##y

SUDOCALL try_context_t *try_context_get(void);
// set the new try context and retrieve the previous one
// SECURITY NOTE: no PLENGTH(sizeof(try_context_t)) set because the value is
// never dereferenced within the SUDOCALL.
//                and is checked before being used in all SYSCALL that would use
//                it.
SUDOCALL try_context_t *try_context_set(try_context_t *context);

// -----------------------------------------------------------------------
// - BEGIN TRY
// -----------------------------------------------------------------------

#define BEGIN_TRY_L(L)                                                         \
  {                                                                            \
    try_context_t __try##L;

// -----------------------------------------------------------------------
// - TRY
// -----------------------------------------------------------------------
#define TRY_L(L)                                                               \
  /* previous exception context chain is saved within the setjmp r9 save */    \
  __try                                                                        \
    ##L.ex = setjmp(__try##L.jmp_buf);                                         \
  if (__try##L.ex == 0) {                                                      \
    __try                                                                      \
      ##L.previous = try_context_set(&__try##L);

// -----------------------------------------------------------------------
// - EXCEPTION CATCH
// -----------------------------------------------------------------------
#define CATCH_L(L, x)                                                          \
  goto CPP_CONCAT(__FINALLY, L);                                               \
  }                                                                            \
  else if (__try##L.ex == x) {                                                 \
    __try                                                                      \
      ##L.ex = 0;                                                              \
    CLOSE_TRY_L(L);

// -----------------------------------------------------------------------
// - EXCEPTION CATCH OTHER
// -----------------------------------------------------------------------
#define CATCH_OTHER_L(L, e)                                                    \
  goto CPP_CONCAT(__FINALLY, L);                                               \
  }                                                                            \
  else {                                                                       \
    exception_t e;                                                             \
    e = __try##L.ex;                                                           \
    __try                                                                      \
      ##L.ex = 0;                                                              \
    CLOSE_TRY_L(L);

// -----------------------------------------------------------------------
// - EXCEPTION CATCH ALL
// -----------------------------------------------------------------------
#define CATCH_ALL_L(L)                                                         \
  goto CPP_CONCAT(__FINALLY, L);                                               \
  }                                                                            \
  else {                                                                       \
    __try                                                                      \
      ##L.ex = 0;                                                              \
    CLOSE_TRY_L(L);

// -----------------------------------------------------------------------
// - FINALLY
// -----------------------------------------------------------------------
#define FINALLY_L(L)                                                           \
  goto CPP_CONCAT(__FINALLY, L);                                               \
  }                                                                            \
  CPP_CONCAT(__FINALLY, L)                                                     \
      : /* has TRY clause ended without nested throw ? */                      \
        if (try_context_get() == &__try##L) {                                  \
    /* restore previous context manually (as a throw would have when caught)   \
     */                                                                        \
    CLOSE_TRY_L(L);                                                            \
  }
// -----------------------------------------------------------------------
// - CLOSE TRY
// -----------------------------------------------------------------------
/**
 * Forced finally like clause.
 */
#define CLOSE_TRY_L(L) try_context_set(__try##L.previous)

// -----------------------------------------------------------------------
// - END TRY
// -----------------------------------------------------------------------
#define END_TRY_L(L)                                                           \
  /* nested throw not consumed ? (by CATCH* clause) */                         \
  if (__try##L.ex != 0) {                                                      \
    /* rethrow */                                                              \
    THROW_L(L, __try##L.ex);                                                   \
  }                                                                            \
  }

// -----------------------------------------------------------------------
// - EXCEPTION THROW
// -----------------------------------------------------------------------

/**
 Remember that using break/return/goto/continue keywords that disrupt the
 execution flow may introduce silent errors which can pop afterwards in a
 very sneaky and hard to debug way. Those keywords are malicious ONLY when
 jumping out of the current block (TRY/CATCH/CATCH_OTHER/CATCH_ALL) else
 they are perfectly fine.
 When those keywords use are unavoidable, then remember to CLOSE_TRY your
 opened BEGIN/END block.
 To detect those potential problems, here is a basic sed based script to
 narrow down the search to poentially suspicious cases.
 for i in `find . -name "*.c"`; do echo $i ; sed -n '/BEGIN_TRY/,/END_TRY/{
 /goto/{=;H;g;p} ;/return/{=;H;g;p} ; /continue/{=;H;g;p} ; /break/{=;H;g;p} ; h
 }' $i ; done Run it on your source code if unsure. The rule of thumb to respect
 to decide whether or not to use the CLOSE_TRY statement is the following:
 Jumping out of a TRY/CATCH/CATCH_ALL/CATCH_OTHER clause is not closing the
 BEGIN/TRY block if the FINALLY is not executed wholy (jumping to a label
 at the beginning of the FINALLY is not solving the above stated problem).

 Faulty example:
 ===============
 BEGIN_TRY {
   TRY {
    ...
   }
   CATCH {
     ...
     goto noway;
   }
   FINALLY {

   }
 }
 END_TRY;
 noway:
 return;

 Faulty example 2:
 ===============
 BEGIN_TRY {
   TRY {
    ...
     goto noway;
   }
   CATCH {
     ...
   }
   FINALLY {

   }
 }
 END_TRY;
 noway:
 return;

 Faulty example 3:
 ===============
 BEGIN_TRY {
   TRY {
    ...
   }
   CATCH {
     ...
     goto end;
     ...
   }
   FINALLY {
     end:
   }
 }
 END_TRY;
 noway:
 return;

 Faulty example 4:
 ===============
 for(;;) {
   BEGIN_TRY {
     TRY {
      ...
       continue;
       ...
     }
     CATCH {
       ...
     }
     FINALLY {

     }
   }
   END_TRY;
 }

 Ok example (but very suspicious algorithmly speaking):
 ===============
 BEGIN_TRY {
   TRY {
    ...
    yo:
    ...
   }
   CATCH {
     ...
     goto yo;
   }
   FINALLY {

   }
 }
 END_TRY;
 noway:
 return;

 Ok example 2:
 ===============
 BEGIN_TRY {
   TRY {
    ...
    CLOSE_TRY;
    goto noway;
    ...
   }
   CATCH {
     ...
   }
   FINALLY {

   }
 }
 END_TRY;
 noway:
 return;

 Ok example 3:
 ===============
 BEGIN_TRY {
   TRY {
    ...
    goto baz;
    ...
    baz:
   }
   CATCH {
     ...
   }
   FINALLY {

   }
 }
 END_TRY;

 Faulty example 5:
 ===============
 BEGIN_TRY {
   TRY {
    ...
   }
   CATCH {
     return val;
   }
   FINALLY {
     ...
   }
 }
 END_TRY;

 Faulty example 6:
 ===============
 BEGIN_TRY {
   TRY {
     ...
     return val;
   }
   CATCH {
     ...
   }
   FINALLY {
     ...
   }
 }
 END_TRY;

 Ok example 4:
 ===============
 BEGIN_TRY {
   TRY {
    ...
   }
   CATCH {
     ...
   }
   FINALLY {
     return val;
   }
 }
 END_TRY;
 */

// longjmp is marked as no return to avoid too much generated code
#ifdef __clang_analyzer__
void os_longjmp(unsigned int exception) __attribute__((analyzer_noreturn));
#else
void os_longjmp(unsigned int exception) __attribute__((noreturn));
#endif
#define THROW_L(L, x) os_longjmp(x)

// Default macros when nesting is not used.
#define THROW(x) THROW_L(EX, x)
#define BEGIN_TRY BEGIN_TRY_L(EX)
#define TRY TRY_L(EX)
#define CATCH(x) CATCH_L(EX, x)
#define CATCH_OTHER(e) CATCH_OTHER_L(EX, e)
#define CATCH_ALL CATCH_ALL_L(EX)
#define FINALLY FINALLY_L(EX)
#define CLOSE_TRY CLOSE_TRY_L(EX)
#define END_TRY END_TRY_L(EX)
