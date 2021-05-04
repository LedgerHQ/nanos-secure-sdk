#pragma once

#include "bolos_target.h"
#include "os_hal.h"

// Arch definitions
#if defined(SLE78)
#define macro_offsetof
#define WIDE
#include "setjmp.h"
#include <SLE78CLUFX500xPH.h>

// allocate all hconst into rewritable zones. this is easier than patching each
// file independantly
#pragma userclass(HCONST = MASK_NVM__RW)

#endif
#if defined(ST23)
#define WIDE @far

// define a stack based setjmp, when in modml, else reentrancy detected
typedef unsigned char jmp_buf[5]; // sp and @far
@far @stack void longjmp(jmp_buf buf, unsigned short error);
@far @stack unsigned short setjmp(jmp_buf buf);

#include <product.h>

#endif
#if defined(ST31)

#define WIDE // const // don't !!
#include "stddef.h"
#include "stdint.h"
#include <core_sc000.h>
#endif
#if defined(STM32)

#define WIDE // const // don't !!
#define __MPU_PRESENT 1
#include "stddef.h"
#include "stdint.h"
#include <stm32l0xx.h>
#endif
#if defined(ST33)

#define WIDE // const // don't !!
#include "stddef.h"
#include "stdint.h"
#include <core_sc300.h>
#endif
#if defined(SLE97)

#define macro_offsetof
#define WIDE const
#include "SLE97CSFX1M00PE.h"
#include <setjmp.h>

#endif
#if defined(X86)

#define NATIVE_PRINT
#define WIDE // on x86 no short address mode
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#endif
#if defined(TRUSTLET)

#define macro_offsetof
#define NATIVE_PRINT
#define WIDE const // on x86 no short address mode

#include "TlApi/TlApi.h"
#include "setjmp.h"
#include "stdio.h"
#include "tlStd.h"

#define printf tlDbgPrintf
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef WIDE_NULL
#define WIDE_NULL ((void WIDE *)0)
#endif
