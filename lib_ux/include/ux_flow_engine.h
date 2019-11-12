
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

#define STEPSPIC(x) ((const ux_flow_step_t* const * )PIC(x))
#define STEPPIC(x) ((const ux_flow_step_t* )PIC(x))
#define INITPIC(x) ((ux_flow_step_init_t)PIC(x))
#define STRPIC(x) ((const char*)PIC(x))

// forward definition
typedef struct ux_flow_step_s ux_flow_step_t;

/**
 * Relatively static UX flow description framework
 */

typedef struct {
  const ux_flow_step_t* const * steps;
  unsigned short index;
  unsigned short prev_index; // to know the direction the user is browsing the flow
  unsigned short length;

} ux_flow_state_t;

typedef void (*ux_flow_step_init_t) (unsigned int stack_slot);

struct ux_flow_step_s {
	// when NULL, cast flow_step structure as a ux_flow_step_jump_t and switch to the referenced flow
	ux_flow_step_init_t init;

	// this field is the one used by layouts to retrieve data to display.
	const void* params;

	// when NULL and more step in the flow, ux_flow_validate act as ux_flow_next
	const ux_flow_step_t* const * validate_flow; // for both button actions to jump onto a new flow, use init of first step of the flow to trigger a custom function upon validate. that is quite compact.
	// when NULL, error are ignored
	const ux_flow_step_t* const * error_flow; // for both button actions to jump onto a new flow, use init of first step of the flow to trigger a custom function upon validate. that is quite compact.
};

unsigned int ux_flow_is_first(void); // to hide the left tick or not
unsigned int ux_flow_is_last(void); // to hide the right tick or not
typedef enum {
	FLOW_DIRECTION_BACKWARD=-1,
	FLOW_DIRECTION_START=0,
	FLOW_DIRECTION_FORWARD=1,
} ux_flow_direction_t;
ux_flow_direction_t ux_flow_direction(void);
const ux_flow_step_t* ux_flow_step(void); // return the current step pointer
void ux_flow_next_no_display(void); // prepare displaying next step when flow is relayout
void ux_flow_next(void); // skip to next step
void ux_flow_prev(void); // go back to previous step
void ux_flow_validate(void); // called by layout's validation
void ux_flow_error(unsigned int error); // called by layout's error
unsigned int ux_flow_button_callback(unsigned int button_mask, unsigned int button_mask_counter);
// retrieve the parameters of the currently displayed step
void* ux_stack_get_current_step_params(void);
void* ux_stack_get_step_params(unsigned int stack_slot);
/**
 * Return 0 when no relayout occured.
 */
unsigned int ux_flow_relayout(void); // ask for a redisplay of the current displayed step (calling step init function again (recomputing layout if required))

/** 
 * Last step is marked with a FLOW_END_STEP value
 */
#define FLOW_END_STEP ((void*)0xFFFFFFFFUL)
/**
 * Fake step implying a double press validation to go to the next step (if any)
 */
#define FLOW_BARRIER  ((void*)0xFFFFFFFEUL)
/**
 * Fake step to be used as the LAST item of the flow (before the FLOW_END_STEP) to notify
 * that the flow is circular with no end/start for prev/next browsing
 */
#define FLOW_LOOP     ((void*)0xFFFFFFFDUL)
void ux_flow_init(unsigned int stack_slot, const ux_flow_step_t* const * steps, const ux_flow_step_t* const start_step);

/**
 * Wipe a flow definition from a flow stack slot
 */
void ux_flow_uninit(unsigned int stack_slot);

/**
 * Define a flow step given both its error and validation flow.
 */
#define UX_STEP_FLOWS(stepname, layoutkind, preinit, timeout_ms, validate_flow, error_flow, ...) \
	void stepname ##_init (unsigned int stack_slot) { \
		preinit; \
		ux_layout_ ## layoutkind ## _init(stack_slot); \
	  ux_layout_set_timeout(stack_slot, timeout_ms); \
	} \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  stepname ##  _init, \
	  & stepname ## _val, \
	  validate_flow, \
	  error_flow, \
	}

#define UX_STEP(stepname, layoutkind, preinit, timeout_ms, validate_cb, error_flow, ...) \
	UX_FLOW_CALL(stepname ## _validate, { validate_cb; }) \
	void stepname ##_init (unsigned int stack_slot) { \
		preinit; \
		ux_layout_ ## layoutkind ## _init(stack_slot); \
	  ux_layout_set_timeout(stack_slot, timeout_ms); \
	} \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  stepname ##  _init, \
	  & stepname ## _val, \
	  stepname ## _validate, \
	  error_flow, \
	}


/** 
 * Define a flow step with a specific step init function
 */
#define UX_STEP_INIT(stepname, validate_flow, error_flow, ...) \
	void stepname ##_init (unsigned int stack_slot) { UNUSED(stack_slot); __VA_ARGS__; } \
	const ux_flow_step_t stepname = { \
	  stepname ##  _init, \
	  NULL, \
	  validate_flow, \
	  error_flow, \
	}
// deprecated
#define UX_FLOW_DEF_INIT UX_STEP_INIT

/**
 * Define a flow step with a validation callback
 */
#define UX_STEP_CB(stepname, layoutkind, validate_cb, ...) \
	UX_FLOW_CALL(stepname ## _validate, { validate_cb; }) \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  ux_layout_ ## layoutkind ## _init, \
	  & stepname ## _val, \
	  stepname ## _validate, \
	  NULL, \
	}
// deprecated
#define UX_FLOW_DEF_VALID UX_STEP_VALID
// deprecated
#define UX_STEP_VALID UX_STEP_CB

/**
 * Define a flow step with a validation callback and a preinit function to
 * render data into shared variable before screen is displayed
 */
#define UX_STEP_CB_INIT(stepname, layoutkind, preinit, validate_cb, ...) \
	UX_FLOW_CALL(stepname ## _validate, { validate_cb; }) \
	void stepname ##_init (unsigned int stack_slot) { \
		preinit; \
		ux_layout_ ## layoutkind ## _init(stack_slot); \
	} \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  stepname ##  _init, \
	  & stepname ## _val, \
	  stepname ## _validate, \
	  NULL, \
	}

/** 
 * Define a flow step with autovalidation after a given timeout (in ms)
 */
#define UX_STEP_TIMEOUT(stepname, layoutkind, timeout_ms, validate_flow, ...) \
	void stepname ##_init (unsigned int stack_slot) { \
		ux_layout_ ## layoutkind ## _init(stack_slot); \
		ux_layout_set_timeout(stack_slot, timeout_ms); \
	} \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  stepname ##  _init, \
	  & stepname ## _val, \
	  validate_flow, \
	  NULL, \
	}

/**
 * Define a simple flow step, given its name, layout and content.
 */
#define UX_STEP_NOCB(stepname, layoutkind, ...) \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  ux_layout_ ## layoutkind ## _init, \
	  & stepname ## _val, \
	  NULL, \
	  NULL, \
	}
// deprecated
#define UX_FLOW_DEF_NOCB UX_STEP_NOCB

/**
 * A Step with variant data (which could be overwritten by a flow idplsayed 
 * in an higher flow and therefore need to be recomputed when step is redisplayed)
 */
#define UX_STEP_NOCB_INIT(stepname, layoutkind, preinit, ...) \
	void stepname ##_init (unsigned int stack_slot) { \
		preinit; \
		ux_layout_ ## layoutkind ## _init(stack_slot); \
	} \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  stepname ##  _init, \
	  & stepname ## _val, \
	  NULL, \
	  NULL, \
	}

/**
 * A Step with variant data (which could be overwritten by a flow idplsayed 
 * in an higher flow and therefore need to be recomputed when step is redisplayed)
 */
#define UX_STEP_NOCB_POSTINIT(stepname, layoutkind, postinit, ...) \
	void stepname ##_init (unsigned int stack_slot) { \
		ux_layout_ ## layoutkind ## _init(stack_slot); \
		postinit; \
	} \
	const ux_layout_ ## layoutkind ## _params_t stepname ##_val = __VA_ARGS__; \
	const ux_flow_step_t stepname = { \
	  stepname ##  _init, \
	  & stepname ## _val, \
	  NULL, \
	  NULL, \
	}


/**
 * Macro that defines a fake flow of a single step to perform code execution upon validate/error next etc
 */
#define UX_FLOW_CALL(flow_name, code) \
void flow_name ## init (unsigned int stack_slot) { UNUSED(stack_slot);code; } \
const ux_flow_step_t flow_name ## _step = { flow_name ## init, NULL, NULL, NULL}; \
const ux_flow_step_t* const flow_name []= { &flow_name ## _step, FLOW_END_STEP, };

/**
 * Define a flow as a sequence of given steps.
 */
#define UX_FLOW(flow_name, ...) \
	const ux_flow_step_t* const flow_name[] = { \
		__VA_ARGS__, \
		FLOW_END_STEP \
	}
// deprecated
#define UX_DEF UX_FLOW


#define UX_STEP_AFTER_PIN(stepname, stackslot, callback) \
  /*  step ask pin */ \
  UX_STEP_INIT( \
    stepname ## __askpin, \
    NULL, \
    NULL, \
    { \
      if (os_perso_isonboarded() == BOLOS_UX_OK) { \
        /* prepare skipping to current step */ \
        ux_flow_next_no_display(); \
        /* invalidate pin and display pin lock */ \
        screen_modal_validate_pin_init(); \
     } else { \
       callback(0); \
     } \
    }); \
  /* step callback */ \
  UX_STEP_INIT( \
    stepname ## __pincallback, \
    NULL, \
    NULL, \
    { \
      callback(0); \
    }); \
  /* flow new 2 steps */ \
  UX_FLOW( stepname ## __pinflow, \
    &stepname ## __askpin, \
    &stepname ## __pincallback \
    ); \
  /* run new flow */ \
  UX_STEP_INIT( \
  stepname, \
  NULL, \
  NULL, \
  { \
    ux_flow_init(stackslot, stepname ## __pinflow, NULL); \
  });

#include "ux.h"
