/*******************************************************************************
 *   Ledger - Secure firmware
 *   (c) 2023 Ledger
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

#if !defined(HAVE_BOLOS)

#include <stdint.h>

#include "bolos_target.h"

#define STR_IMPL_(x) #x
#define STRINGIFY(x) STR_IMPL_(x)

#define CREATE_METADATA_STRING_ITEM(ITEM_NAME, section_name) \
    __attribute__((section("ledger." #section_name)))        \
    const char section_name[sizeof(ITEM_NAME) - 1]           \
        = ITEM_NAME;

#define CREATE_METADATA_STRING_ITEM_FROM_INT(ITEM_NAME, section_name) \
    CREATE_METADATA_STRING_ITEM(STRINGIFY(ITEM_NAME), section_name)

#if defined(TARGET)
CREATE_METADATA_STRING_ITEM(TARGET, target)
#endif

#if defined(TARGET_NAME)
CREATE_METADATA_STRING_ITEM(TARGET_NAME, target_name)
#endif

#if defined(TARGET_ID)
CREATE_METADATA_STRING_ITEM_FROM_INT(TARGET_ID, target_id)
#endif

#if defined(TARGET_VERSION)
CREATE_METADATA_STRING_ITEM_FROM_INT(TARGET_VERSION, target_version)
#endif

#if defined(APPNAME)
CREATE_METADATA_STRING_ITEM(APPNAME, app_name)
#endif

#if defined(APPVERSION)
CREATE_METADATA_STRING_ITEM(APPVERSION, app_version)
#endif

#if defined(API_LEVEL)
CREATE_METADATA_STRING_ITEM_FROM_INT(API_LEVEL, api_level)
#endif

#if defined(SDK_NAME)
CREATE_METADATA_STRING_ITEM(SDK_NAME, sdk_name)
#endif

#if defined(SDK_VERSION)
CREATE_METADATA_STRING_ITEM(SDK_VERSION, sdk_version)
#endif

#if defined(SDK_HASH)
CREATE_METADATA_STRING_ITEM(SDK_HASH, sdk_hash)
#endif

#if defined(HAVE_BAGL)
CREATE_METADATA_STRING_ITEM("bagl", sdk_graphics)
#endif

#if defined(HAVE_NBGL)
CREATE_METADATA_STRING_ITEM("nbgl", sdk_graphics)
#endif

#ifdef APP_INSTALL_PARAMS_DATA
__attribute__((section(".install_parameters"))) const uint8_t install_parameters[]
    = {APP_INSTALL_PARAMS_DATA};
#endif

#endif
