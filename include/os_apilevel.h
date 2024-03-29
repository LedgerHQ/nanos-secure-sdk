
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

#ifndef OS_APILEVEL_H
#define OS_APILEVEL_H

#ifndef HAVE_BOLOS
// Obsolete defines - shall not be used
// Replaced by API_LEVEL mechanism enforced directly at app sideloading.
// Hence the app can consider it runs on a OS which match the SDK the app has been built for.
#define CX_APILEVEL        _Pragma("GCC warning \"Deprecated constant!\"") 12
#define CX_COMPAT_APILEVEL _Pragma("GCC warning \"Deprecated constant!\"") 12
#else
#define CX_APILEVEL        12
#define CX_COMPAT_APILEVEL 12
#endif

#endif  // OS_APILEVEL_H
