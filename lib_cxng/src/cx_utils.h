
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2021 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#ifndef CX_UTILS_H
#define CX_UTILS_H

#include "lcx_common.h"
#include "errors.h"
#include "exceptions.h"
#include <stddef.h>
#include <stdint.h>

/* ======================================================================= */
/*                          32 BITS manipulation                           */
/* ======================================================================= */
uint32_t cx_swap_uint32(uint32_t v);
void     cx_swap_buffer32(uint32_t *v, size_t len);

#define _CX_INLINE_U32
#define cx_rotl(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define cx_rotr(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define cx_shr(x, n) ((x) >> (n))

/* ======================================================================= */
/*                          64 BITS manipulation                           */
/* ======================================================================= */

#ifndef NATIVE_64BITS // NO 64BITS

#ifdef ARCH_LITTLE_ENDIAN
#define _64BITS(h, l)                                                                                                  \
  { l, h }
#else
#define _64BITS(h, l)                                                                                                  \
  { h, l }
#endif

#define CLR64(x)                                                                                                       \
  (x).l = 0;                                                                                                           \
  (x).h = 0
#define ADD64(x, y) cx_add_64(&(x), &(y))
#define ASSIGN64(r, x)                                                                                                 \
  (r).l = (x).l;                                                                                                       \
  (r).h = (x).h

void cx_rotr64(uint64bits_t *x, unsigned char n);

void cx_shr64(uint64bits_t *x, unsigned char n);

void cx_add_64(uint64bits_t *a, uint64bits_t *b);
void cx_swap_uint64(uint64bits_t *v);
void cx_swap_buffer64(uint64bits_t *v, int len);

#else

#define _64BITS(h, l) (h##ULL << 32) | (l##ULL)

#define _CX_INLINE_U64

#define cx_rotr64(x, n) (((x) >> (n)) | ((x) << ((64) - (n))))

#define cx_rotl64(x, n) (((x) << (n)) | ((x) >> ((64) - (n))))

#define cx_shr64(x, n) ((x) >> (n))

uint64bits_t cx_swap_uint64(uint64bits_t v);
void         cx_swap_buffer64(uint64bits_t *v, int len);

#endif

void cx_memxor(uint8_t *buf1, const uint8_t *buf2, size_t len);

#endif
