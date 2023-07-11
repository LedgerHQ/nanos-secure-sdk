/*******************************************************************************
*   Ledger - Secure firmware
*   (c) 2016, 2017, 2018, 2019, 2020, 2021, 2022, 2023 Ledger
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

/* Reuse the main linker script */
INCLUDE script.ld

/* No global variable allowed in plugins */
ASSERT( (_ebss - _bss) <= 4, ".bss section must be empty for plugins" )
