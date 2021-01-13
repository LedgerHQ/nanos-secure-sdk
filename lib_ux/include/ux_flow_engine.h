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
  // to be setup before displaying the layout
  unsigned short stack_slot;
  
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
const ux_flow_step_t* ux_flow_step(void); // return the current step pointer
void ux_flow_next(void); // skip to next step
void ux_flow_prev(void); // go back to previous step
void ux_flow_validate(void); // called by layouts validation
void ux_flow_error(unsigned int error); // called by layouts error
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
		ux_layout_set_timeout(stack_slot, timeout_ms);\
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
	UX_STEP(stepname, layoutkind, NULL, 0, validate_cb, NULL, __VA_ARGS__)
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
	UX_STEP_FLOWS(stepname, layoutkind, NULL, 0, NULL, NULL, __VA_ARGS__)
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

#include "ux.h"
