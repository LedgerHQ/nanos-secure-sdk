
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

#pragma once

#include "bolos_target.h"

#include "bagl.h"

#ifdef TARGET_NANOX
/*********************************************************************************
 * 1 bold text line
 * 3 text lines
 */

typedef struct ux_layout_bnnn_params_s {
	const char* line1;
	const char* line2;
	const char* line3;
	const char* line4;
} ux_layout_bnnn_params_t;

void ux_layout_bnnn_init(unsigned int stack_slot);

/*********************************************************************************
 * 4 text lines
 */

typedef struct ux_layout_nnnn_params_s {
	const char* line1;
	const char* line2;
	const char* line3;
	const char* line4;
} ux_layout_nnnn_params_t;

void ux_layout_nnnn_init(unsigned int stack_slot);

/*********************************************************************************
 * 3 normal text lines
 */

typedef struct ux_layout_nnn_params_s {
	const char* line1;
	const char* line2;
	const char* line3;
} ux_layout_nnn_params_t;

void ux_layout_nnn_init(unsigned int stack_slot);

/*********************************************************************************
 * 1 bold text lines
 * 2 normal text lines
 */

typedef struct ux_layout_bnn_params_s {
	const char* line1;
	const char* line2;
	const char* line3;
} ux_layout_bnn_params_t;

void ux_layout_bnn_init(unsigned int stack_slot);
#endif // TARGET_NANOX

/*********************************************************************************
 * 1 bold text line with the title
 * 1-3 text lines [nano s/nano x]
 */
#define UX_LAYOUT_PAGING_LINE 1

typedef struct ux_layout_paging_params_s {
	const char* title;
	const char* text;
} ux_layout_paging_params_t;

#define ux_layout_bn_paging_init ux_layout_paging_init
void ux_layout_paging_init(unsigned int stack_slot);
// Call to reset the paging component to the first page
void ux_layout_paging_reset(void);

// deprecation
#define ux_layout_bnnn_paging_params_t ux_layout_paging_params_t
#define ux_layout_bnnn_paging_init ux_layout_paging_init
#define ux_layout_bnnn_paging_reset ux_layout_paging_reset
#define ux_layout_bn_paging_params_t ux_layout_paging_params_t
#define ux_layout_bn_paging_init ux_layout_paging_init
#define ux_layout_bn_paging_reset ux_layout_paging_reset

/*********************************************************************************
 * 2 text lines
 * 1 bold text lines
 * 2 text lines
 */

typedef struct ux_layout_nnbnn_params_s {
	const char* line1;
	const char* line2;
	const char* line3;
	const char* line4;
	const char* line5;
} ux_layout_nnbnn_params_t;

void ux_layout_nnbnn_init(unsigned int stack_slot);

/*********************************************************************************
 * ICON
 * 1 bold text line
 */

typedef struct ux_layout_pb_params_s {
	const bagl_icon_details_t* icon;
	const char* line1;
} ux_layout_pb_params_t;

void ux_layout_pb_init(unsigned int stack_slot);

/*********************************************************************************
 * 2 bold text line
 */

typedef struct ux_layout_bb_params_s {
	const char* line1;
	const char* line2;
} ux_layout_bb_params_t;

void ux_layout_bb_init(unsigned int stack_slot);

/*********************************************************************************
 * 1 text line
 * 1 bold text line
 */

typedef struct ux_layout_bn_params_s {
	const char* line1;
	const char* line2;
} ux_layout_bn_params_t;

void ux_layout_bn_init(unsigned int stack_slot);

/*********************************************************************************
 * ICON
 * 2 bold text lines
 */

typedef struct ux_layout_pbb_params_s {
	const bagl_icon_details_t* icon;
	const char* line1;
	const char* line2;
} ux_layout_pbb_params_t;

void ux_layout_pbb_init(unsigned int stack_slot);

/*********************************************************************************
 * ICON
 * 2 normal text lines
 */

typedef struct ux_layout_pnn_params_s {
	const bagl_icon_details_t* icon;
	const char* line1;
	const char* line2;
} ux_layout_pnn_params_t;

void ux_layout_pnn_init(unsigned int stack_slot);

/*********************************************************************************
 * ICON
 * 1 normal text lines
 */

typedef struct ux_layout_pn_params_s {
	const bagl_icon_details_t* icon;
	const char* line1;
} ux_layout_pn_params_t;

void ux_layout_pn_init(unsigned int stack_slot);


/*********************************************************************************
 * 2 normal text lines
 */

typedef struct ux_layout_nn_params_s {
	const char* line1;
	const char* line2;
} ux_layout_nn_params_t;

void ux_layout_nn_init(unsigned int stack_slot);


/*********************************************************************************
 * Browsable component
 */

// activate an item by index, do nothing when item_idx is invalid
typedef void (*list_item_select_t) (unsigned int item_idx);

// return NULL when item_idx is invalid
typedef const char * (*list_item_value_t) (unsigned int item_idx);

void ux_menulist_init(unsigned int stack_slot, 
	                    list_item_value_t getter, 
	                    list_item_select_t selector);
void ux_menulist_init_select(unsigned int stack_slot, 
	                           list_item_value_t getter, 
	                           list_item_select_t selector, 
	                           unsigned int selected_item_idx);

#define UX_STEP_MENULIST(stepname, getter, selector) \
  void stepname ## _init (unsigned int stack_slot) { ux_menulist_init(stack_slot, getter, selector); } \
  const ux_flow_step_t stepname = { \
    stepname ## _init, \
    &G_ux.menulist_params, \
    NULL, \
    NULL, \
  }

#define UX_STEP_MENULIST_INIT(stepname, initfunc) \
  const ux_flow_step_t stepname = { \
    initfunc, \
    &G_ux.menulist_params, \
    NULL, \
    NULL, \
  }


/*********************************************************************************
 * Utilities
 */

/**
 * Set the timeout before auto validation of the step currently displayed 
 * (must be called after layout init function).
 */
void ux_layout_set_timeout(unsigned int stack_slot, unsigned int ms);

