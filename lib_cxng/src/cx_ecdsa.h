
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
#ifdef HAVE_ECDSA

#ifndef CX_ECDSA_H
#define CX_ECDSA_H

/* In the supported elliptic curves, the order has at most 521 bits (with NIST P-521 a.k.a. secp521r1).
 * The next multiple of 8 is 528 = 66*8.
 */
#define CX_ECDSA_MAX_ORDER_LEN 66

#endif
#endif
