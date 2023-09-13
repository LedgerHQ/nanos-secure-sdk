
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
#pragma once

#include "bolos_target.h"

#include "bagl.h"
#include "bolos_ux_loc_strings.h"

typedef BOLOS_UX_LOC_STRINGS UX_LOC_STRINGS_INDEX;

//********************************************************************************
typedef struct ux_loc_layout_params_s {
    const UX_LOC_STRINGS_INDEX index;
} ux_loc_layout_params_t;

#define ux_loc_layout_nn_paging_params_t ux_loc_layout_params_t
#define ux_loc_layout_nb_paging_params_t ux_loc_layout_params_t
#define ux_loc_layout_bn_paging_params_t ux_loc_layout_params_t
#define ux_loc_layout_bb_paging_params_t ux_loc_layout_params_t
#define ux_loc_layout_nnn_params_t       ux_loc_layout_params_t
#define ux_loc_layout_bnn_params_t       ux_loc_layout_params_t
#define ux_loc_layout_nnnn_params_t      ux_loc_layout_params_t
#define ux_loc_layout_bnnn_params_t      ux_loc_layout_params_t
#define ux_loc_layout_nnbnn_params_t     ux_loc_layout_params_t
#define ux_loc_layout_pages_params_t     ux_loc_layout_params_t

typedef struct ux_loc_layout_icon_params_s {
    const bagl_icon_details_t *icon;
    const UX_LOC_STRINGS_INDEX index;
} ux_loc_layout_icon_params_t;

#define ux_loc_layout_pb_params_t  ux_loc_layout_icon_params_t
#define ux_loc_layout_pn_params_t  ux_loc_layout_icon_params_t
#define ux_loc_layout_pbb_params_t ux_loc_layout_icon_params_t
#define ux_loc_layout_pnn_params_t ux_loc_layout_icon_params_t
#define ux_loc_layout_pbn_params_t ux_loc_layout_icon_params_t

//********************************************************************************
typedef struct ux_layout_pages_params_s {
    const char *text;
} ux_layout_pages_params_t;

#if (BAGL_WIDTH == 128 && BAGL_HEIGHT == 64)
#define UX_LAYOUT_PAGES_LINE_COUNT 4
#elif (BAGL_WIDTH == 128 && BAGL_HEIGHT == 32)
#define UX_LAYOUT_PAGES_LINE_COUNT 2
#else
#error "BAGL_WIDTH/BAGL_HEIGHT not defined"
#endif

void ux_layout_pages_init(unsigned int stack_slot);
void ux_loc_layout_pages_init(unsigned int stack_slot);
void ux_loc_layout_paging_init(unsigned int stack_slot);

// deprecation
#define ux_loc_layout_bnnn_paging_init  ux_loc_layout_bn_paging_init
#define ux_loc_layout_bnnn_paging_reset ux_layout_paging_reset
#define ux_loc_layout_bn_paging_reset   ux_layout_paging_reset
