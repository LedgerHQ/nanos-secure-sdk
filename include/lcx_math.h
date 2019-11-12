
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#ifndef LCX_MATH_H
#define LCX_MATH_H

/**
 * Compare to unsigned long big-endian integer
 * The maximum length supported is 64.
 *
 * @param a    first operand
 * @param b    second operand
 * @param len  byte length of a, b
 *
 * @return 0 if a==b,  negative value if a<b, positive value if a>b
 */
CXCALL int cx_math_cmp(const unsigned char WIDE *a PLENGTH(len),
                       const unsigned char WIDE *b PLENGTH(len),
                       unsigned int len);

/**
 * Compare to unsigned long big-endian integer to zero
 *
 * @param a    value to compare to zero
 * @param len  byte length of a
 *
 * @return 1 if a==0,  0 else
 */
CXCALL int cx_math_is_zero(const unsigned char WIDE *a PLENGTH(len),
                           unsigned int len);

/**
 * Addition of two big integer: r = a+b
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 * @return carry
 */
CXCALL int cx_math_add(unsigned char *r PLENGTH(len),
                       const unsigned char WIDE *a PLENGTH(len),
                       const unsigned char WIDE *b PLENGTH(len),
                       unsigned int len);

/**
 * Subtraction of two big integer: r = a-b
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 * @return borrow
 */
CXCALL int cx_math_sub(unsigned char *r PLENGTH(len),
                       const unsigned char WIDE *a PLENGTH(len),
                       const unsigned char WIDE *b PLENGTH(len),
                       unsigned int len);

/**
 * Subtraction of two big integer: r = a-b
 *
 * @param r    where to put result, len*2 bytes
 * @param a    first operand, len bytes
 * @param b    second operand, len bytes
 * @param len  byte length base of a, b, r
 *
 */
CXCALL void cx_math_mult(unsigned char *r PLENGTH(2 * len),
                         const unsigned char WIDE *a PLENGTH(len),
                         const unsigned char WIDE *b PLENGTH(len),
                         unsigned int len);

/**
 * Modular addition of two big integer: r = a+b mod m
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 */
CXCALL void cx_math_addm(unsigned char *r PLENGTH(len),
                         const unsigned char WIDE *a PLENGTH(len),
                         const unsigned char WIDE *b PLENGTH(len),
                         const unsigned char WIDE *m PLENGTH(len),
                         unsigned int len);
/**
 * Modular subtraction of tow big integer: r = a-b mod m
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 */
CXCALL void cx_math_subm(unsigned char *r PLENGTH(len),
                         const unsigned char WIDE *a PLENGTH(len),
                         const unsigned char WIDE *b PLENGTH(len),
                         const unsigned char WIDE *m PLENGTH(len),
                         unsigned int len);
/**
 * Modular multiplication of tow big integer: r = a*b mod m
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 */
CXCALL void cx_math_multm(unsigned char *r PLENGTH(len),
                          const unsigned char WIDE *a PLENGTH(len),
                          const unsigned char WIDE *b PLENGTH(len),
                          const unsigned char WIDE *m PLENGTH(len),
                          unsigned int len);

/**
 * Modular exponentiation of tow big integer: r = a^^e mod m
 *
 * @param r     where to put result
 * @param a     first operand
 * @param e     second operand
 * @param len_e byte length of e
 * @param m     modulo
 * @param len   byte length of r, a, b, m
 *
 */
CXCALL void
cx_math_powm(unsigned char *r PLENGTH(len), const unsigned char *a PLENGTH(len),
             const unsigned char WIDE *e PLENGTH(len_e), unsigned int len_e,
             const unsigned char WIDE *m PLENGTH(len), unsigned int len);

/**
 * Reduce in place (left zero padded) the given value: v = v mod m
 *
 * @param v        value to reduce
 * @param len_v    shall be >= len_m
 * @param m        modulus
 * @param len_m    length of modulus
 *
 */
CXCALL void cx_math_modm(unsigned char *v PLENGTH(len_v), unsigned int len_v,
                         const unsigned char WIDE *m PLENGTH(len_m),
                         unsigned int len_m);

/**
 * Modular prime inversion: r = (a^-1) mod m, with m prime
 *
 * @param r     where to put result
 * @param a        value to invert
 * @param m        modulus
 * @param len   length of r,a,m
 *
 */
CXCALL void cx_math_invprimem(unsigned char *r PLENGTH(len),
                              const unsigned char *a PLENGTH(len),
                              const unsigned char WIDE *m PLENGTH(len),
                              unsigned int len);

/**
 * Modular integer inversion: r = (a^-1) mod m, with a 32 bits
 *
 * @param r     where to put result
 * @param a     value to invert
 * @param m     modulus
 * @param len   length of r,m
 *
 */
CXCALL void cx_math_invintm(unsigned char *r PLENGTH(len), unsigned long int a,
                            const unsigned char WIDE *m PLENGTH(len),
                            unsigned int len);

/**
 * Test if p is prime
 *
 * @param p     value to test
 * @param len   length p
 */
CXCALL int cx_math_is_prime(const unsigned char *p PLENGTH(len),
                            unsigned int len);

/**
 * Find in place the next prime number follwing n
 *
 * @param n     seed value for next prime
 * @param len   length n
 */
CXCALL void cx_math_next_prime(unsigned char *n PLENGTH(len), unsigned int len);

#endif
