
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

#include "ux.h"
#include "os_helpers.h"
#include "os_pic.h"
#include "ux_flow_engine.h"

#include <string.h>

#ifdef HAVE_UX_FLOW

// check if flow is valid (has valid length and in correct stack slot)
static unsigned int ux_flow_check_valid(void) {
	unsigned int top_stack_slot = G_ux.stack_count - 1;

	if (G_ux.stack_count > UX_STACK_SLOT_COUNT
		|| G_ux.flow_stack[top_stack_slot].length == 0) {
		return 0;
	}
	return 1;
}

// to hide the left tick or not
unsigned int ux_flow_is_first(void) {
  // no previous ?
  unsigned int top_stack_slot = G_ux.stack_count - 1;
  if (!ux_flow_check_valid() || G_ux.flow_stack[top_stack_slot].steps == NULL ||
      (G_ux.flow_stack[top_stack_slot].index == 0 &&
       G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].length - 1] != FLOW_LOOP)) {
    return 1;
  }

  // previous is a flow barrier ?
  if (G_ux.flow_stack[top_stack_slot].length > 0 &&
      G_ux.flow_stack[top_stack_slot].index < G_ux.flow_stack[top_stack_slot].length &&
      G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index - 1] == FLOW_BARRIER) {
    return 1;
  }

  // not the first, for sure
  return 0;
}

unsigned int ux_flow_is_last(void){
	// last ?
  	unsigned int top_stack_slot = G_ux.stack_count - 1;

	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[top_stack_slot].steps == NULL
		|| G_ux.flow_stack[top_stack_slot].length == 0
		|| G_ux.flow_stack[top_stack_slot].index >= G_ux.flow_stack[top_stack_slot].length -1) {
		return 1;
	}

	// followed by a flow barrier ?
	if (G_ux.flow_stack[top_stack_slot].length > 0
		&& G_ux.flow_stack[top_stack_slot].index < G_ux.flow_stack[top_stack_slot].length - 2
		&& G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index+1] == FLOW_BARRIER) {
		return 1;
	}

	// is not last
	return 0;
}

ux_flow_direction_t ux_flow_direction(void) {
  	unsigned int top_stack_slot = G_ux.stack_count - 1;

	if (G_ux.stack_count) {
		if (G_ux.flow_stack[top_stack_slot].index > G_ux.flow_stack[top_stack_slot].prev_index) {
		return FLOW_DIRECTION_FORWARD;
		}
		else if (G_ux.flow_stack[top_stack_slot].index < G_ux.flow_stack[top_stack_slot].prev_index) {
			return FLOW_DIRECTION_BACKWARD;
		}
	}
  return FLOW_DIRECTION_START;
}

const ux_flow_step_t* ux_flow_get_current(void) {
  	unsigned int top_stack_slot = G_ux.stack_count - 1;

	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[top_stack_slot].steps == NULL
		|| G_ux.flow_stack[top_stack_slot].index >= G_ux.flow_stack[top_stack_slot].length
		|| G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index] == FLOW_BARRIER
		|| G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index] == FLOW_LOOP
		|| G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index] == FLOW_END_STEP) {
		return NULL;
	}
	return STEPPIC(G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index]);
}

static void ux_flow_engine_init_step(unsigned int stack_slot) {
	// invalid ux_flow_length ??? (previous check shall have exited earlier)
	if (G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index] == FLOW_END_STEP) {
		return;
	}
	// this shall not have occured due to previous checks
	if (G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index] == FLOW_BARRIER) {
		return;
	}
	// this shall not have occured due to previous checks
	if (G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index] == FLOW_LOOP) {
		return;
	}
	// if init function is set, call it
	if (STEPPIC(G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index])->init) {
		INITPIC(STEPPIC(G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index])->init)(stack_slot);
	}
	else {
		// if init method is not set, jump to referenced flow and step
		ux_flow_init(stack_slot,
			           STEPSPIC(STEPPIC(G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index])->validate_flow),
			           (const ux_flow_step_t*) PIC(STEPPIC(G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index])->params));
	}
}

static void ux_flow_next_internal(unsigned int display_step) {
  	unsigned int top_stack_slot = G_ux.stack_count - 1;

	// last reached already (need validation, not next)
	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[top_stack_slot].steps == NULL
		|| G_ux.flow_stack[top_stack_slot].length <= 1
		|| G_ux.flow_stack[top_stack_slot].index >= G_ux.flow_stack[top_stack_slot].length -1) {
		return;
	}

	// followed by a flow barrier ? => need validation instead of next
	if (G_ux.flow_stack[top_stack_slot].index <= G_ux.flow_stack[top_stack_slot].length - 2) {
		if (G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index+1] == FLOW_BARRIER) {
			return;
		}

		// followed by a flow barrier ? => need validation instead of next
		if (G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index+1] == FLOW_LOOP) {
			// display first step, fake direction as forward
			G_ux.flow_stack[top_stack_slot].prev_index = G_ux.flow_stack[top_stack_slot].index = 0;
			ux_flow_engine_init_step(top_stack_slot);
			return;
		}
	}

	// advance flow pointer and display it (skip META STEPS)
	G_ux.flow_stack[top_stack_slot].prev_index = G_ux.flow_stack[top_stack_slot].index;
	G_ux.flow_stack[top_stack_slot].index++;
	if (display_step) {
		ux_flow_engine_init_step(top_stack_slot);
	}
}

void ux_flow_next_no_display(void) {
	ux_flow_next_internal(0);
}

void ux_flow_next(void) {
	ux_flow_next_internal(1);
}

void ux_flow_prev(void) {
	unsigned int top_stack_slot = G_ux.stack_count - 1;

	// first reached already
	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[top_stack_slot].steps == NULL
		|| G_ux.flow_stack[top_stack_slot].length <= 1
		|| (G_ux.flow_stack[top_stack_slot].index == 0
			  && G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].length-1] != FLOW_LOOP)) {
		return;
	}

	// loop in flow (before checking barrier as there is no prestep when looping)
	if (G_ux.flow_stack[top_stack_slot].index == 0
		&& G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].length-1] == FLOW_LOOP) {
		// display last step (shall skip BARRIER if any, but a flow finishing with a BARRIER is cryptic)
		G_ux.flow_stack[top_stack_slot].index = G_ux.flow_stack[top_stack_slot].length-2;
		// fact direction as backward
		G_ux.flow_stack[top_stack_slot].prev_index = G_ux.flow_stack[top_stack_slot].index+1;
		ux_flow_engine_init_step(top_stack_slot);
		return;
	}

	// previous item is a flow barrier ?
	if (G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index-1] == FLOW_BARRIER) {
		return;
	}

	// advance flow pointer and display it (skip META STEPS)
	G_ux.flow_stack[top_stack_slot].prev_index = G_ux.flow_stack[top_stack_slot].index;
	G_ux.flow_stack[top_stack_slot].index--;

	ux_flow_engine_init_step(top_stack_slot);
}

void ux_flow_validate(void) {
  	unsigned int top_stack_slot = G_ux.stack_count - 1;

	// no flow ?
	if (!ux_flow_check_valid()
	  || G_ux.flow_stack[top_stack_slot].steps == NULL
		|| G_ux.flow_stack[top_stack_slot].length == 0
		|| G_ux.flow_stack[top_stack_slot].index >= G_ux.flow_stack[top_stack_slot].length) {
		return;
	}

	// no validation flow ?
	if (STEPPIC(G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index])->validate_flow != NULL) {
		// execute validation flow
		ux_flow_init(top_stack_slot, STEPSPIC(STEPPIC(G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index])->validate_flow), NULL);
	}
	else {
		// if next is a barrier, then proceed to the item after the barrier
		// if NOT followed by a barrier, then validation is only performed through
		// a validate_flow specified in the step, else ignored
		if (G_ux.flow_stack[top_stack_slot].length > 0
			&& G_ux.flow_stack[top_stack_slot].index <= G_ux.flow_stack[top_stack_slot].length - 2) {

			if (G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index+1] == FLOW_BARRIER) {

				// take into account multi barrier at once, kthx poor code review
				while (G_ux.flow_stack[top_stack_slot].length > 0
					&& G_ux.flow_stack[top_stack_slot].index <= G_ux.flow_stack[top_stack_slot].length - 2
					&& G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index+1] == FLOW_BARRIER) {
					G_ux.flow_stack[top_stack_slot].index++;
				}
				// skip to next step
				G_ux.flow_stack[top_stack_slot].prev_index = G_ux.flow_stack[top_stack_slot].index;
				G_ux.flow_stack[top_stack_slot].index++;

				// execute reached step
				ux_flow_engine_init_step(top_stack_slot);
			}
			// reached the last step, but step if FLOW_LOOP
			else if (G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index+1] == FLOW_LOOP) {
				// we go the forward direction
				G_ux.flow_stack[top_stack_slot].prev_index = G_ux.flow_stack[top_stack_slot].index = 0;
				// execute reached step
				ux_flow_engine_init_step(top_stack_slot);
			}
		}
	}
}

void ux_flow_error(unsigned int error) {
	UNUSED(error);

  	unsigned int top_stack_slot = G_ux.stack_count - 1;

	if (G_ux.flow_stack[top_stack_slot].steps == NULL
		|| G_ux.flow_stack[top_stack_slot].length == 0
		|| G_ux.flow_stack[top_stack_slot].index >= G_ux.flow_stack[top_stack_slot].length) {
		return;
	}

	if (STEPPIC(G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index])->error_flow != NULL) {
		ux_flow_init(top_stack_slot, STEPSPIC(STEPPIC(G_ux.flow_stack[top_stack_slot].steps[G_ux.flow_stack[top_stack_slot].index])->error_flow), NULL);
	}
}

/**
 * Last step is marked with a FLOW_END_STEP value
 */
void ux_flow_init(unsigned int stack_slot, const ux_flow_step_t* const * steps, const ux_flow_step_t* const start_step) {
	if (stack_slot >= UX_STACK_SLOT_COUNT) {
		return;
	}

	G_ux.flow_stack[stack_slot].length = G_ux.flow_stack[stack_slot].prev_index = G_ux.flow_stack[stack_slot].index = 0;
	G_ux.flow_stack[stack_slot].steps = NULL;

	// reset paging to avoid troubles if first step is a paginated step
	memset(&G_ux.layout_paging, 0, sizeof(G_ux.layout_paging));

	if (steps) {
		G_ux.flow_stack[stack_slot].steps = STEPSPIC(steps);
		while(G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].length] != FLOW_END_STEP) {
			G_ux.flow_stack[stack_slot].length++;
		}
		if (start_step != NULL) {
			const ux_flow_step_t* const start_step2  = STEPPIC(start_step);
			while(G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index] != FLOW_END_STEP
				 && STEPPIC(G_ux.flow_stack[stack_slot].steps[G_ux.flow_stack[stack_slot].index]) != start_step2) {
				G_ux.flow_stack[stack_slot].prev_index = G_ux.flow_stack[stack_slot].index;
				G_ux.flow_stack[stack_slot].index++;
			}
		}

		// init step
		ux_flow_engine_init_step(stack_slot);
	}
}

void ux_flow_uninit(unsigned int stack_slot) {
  if (stack_slot < UX_STACK_SLOT_COUNT) {
    memset(&G_ux.flow_stack[stack_slot], 0, sizeof(G_ux.flow_stack[stack_slot]));
  }
}

unsigned int ux_flow_button_callback(unsigned int button_mask, unsigned int button_mask_counter) {
  UNUSED(button_mask_counter);
  switch(button_mask) {
    case BUTTON_EVT_RELEASED|BUTTON_LEFT:
      ux_flow_prev();
      break;
    case BUTTON_EVT_RELEASED|BUTTON_RIGHT:
      ux_flow_next();
      break;
    case BUTTON_EVT_RELEASED|BUTTON_LEFT|BUTTON_RIGHT:
      ux_flow_validate();
      break;
  }
  return 0;
}

const void* ux_stack_get_step_params(unsigned int stack_slot) {
	if (stack_slot >= UX_STACK_SLOT_COUNT) {
		return NULL;
	}

	if (G_ux.flow_stack[stack_slot].length == 0) {
		return NULL;
	}

	if (G_ux.flow_stack[stack_slot].index >= G_ux.flow_stack[stack_slot].length) {
		return NULL;
	}

	return PIC(STEPPIC(STEPSPIC(G_ux.flow_stack[stack_slot].steps)[G_ux.flow_stack[stack_slot].index])->params);
}

const void* ux_stack_get_current_step_params(void) {
	unsigned int top_stack_slot = G_ux.stack_count - 1;

	return ux_stack_get_step_params(top_stack_slot);
}

unsigned int ux_flow_relayout(void) {
	// if a flow is defined and valid
	if (ux_flow_get_current() != NULL) {
		unsigned int top_stack_slot = G_ux.stack_count - 1;

		ux_flow_engine_init_step(top_stack_slot);
		return 1;
	}
	return 0;
}

#endif // HAVE_UX_FLOW
