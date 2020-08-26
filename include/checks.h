
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2020 Ledger
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

#if !defined(CHECKS_H)
#define CHECKS_H

#if !defined(HAVE_BOLOS)

#define CHECK_NOT_AUDITED_TLV_TAG 0x9F
#define CHECK_NOT_AUDITED_TLV_VAL 0x01
#define CHECK_NOT_AUDITED_MAX_LEN 0x40

void check_audited_app(void);
#endif // !defined(HAVE_BOLOS)

#endif
