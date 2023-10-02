
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

#ifndef HAVE_SELFTESTS_H
#define HAVE_SELFTESTS_H

#ifndef HAVE_SELFTESTS_START_LOOP
#define HAVE_SELFTESTS_START_LOOP 0
#endif

#ifdef NATIVE_PRINT
#define cx_printf(a) printf a
#else
#define cx_printf(a)
#endif

void cx_printa(char *prefix, const unsigned char *r, unsigned short len);

#ifdef HAVE_SELFTESTS

void cx_check_result(int r);

int cx_benchmark(void);

#endif // HAVE_SELFTESTS

#endif // HAVE_SELFTESTS_H
