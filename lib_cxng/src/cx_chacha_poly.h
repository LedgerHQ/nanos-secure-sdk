
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2022 Ledger
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
#if defined(HAVE_CHACHA_POLY)
#if defined(HAVE_POLY1305) && defined(HAVE_CHACHA)

#ifndef CX_CHACHA_POLY_H
#define CX_CHACHA_POLY_H

#if defined(HAVE_AEAD)
#include "lcx_aead.h"
#endif  // HAVE_AEAD
#include "lcx_chacha.h"
#include "lcx_chacha_poly.h"
#include "cx_poly1305.h"
#include "ox.h"
#include <stddef.h>

#if defined(HAVE_AEAD)
extern const cx_aead_info_t cx_chacha20_poly1305_info;
#endif  // HAVE_AEAD

#endif  /* CX_CHACHA_POLY_H */
#endif  // HAVE_POLY1305 && HAVE_CHACHA
#endif  // HAVE_CHACHA_POLY
