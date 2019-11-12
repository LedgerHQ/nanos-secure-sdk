
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

#include "ux.h"

#include "string.h"

#ifdef HAVE_UX_FLOW

static unsigned int ux_flow_check_valid(void) {
	if (G_ux.stack_count > UX_STACK_SLOT_COUNT
		|| G_ux.flow_stack[G_ux.stack_count-1].length == 0) {
		return 0;
	}
	return 1;
}

// to hide the left tick or not
unsigned int ux_flow_is_first(void) {
	// no previous ?
	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[G_ux.stack_count-1].steps == NULL
		|| (G_ux.flow_stack[G_ux.stack_count-1].index == 0 
			  && G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].length-1] != FLOW_LOOP)) {
		return 1;
	}

	// previous is a flow barrier ?
	if (G_ux.flow_stack[G_ux.stack_count-1].length > 0 
		&& G_ux.flow_stack[G_ux.stack_count-1].index < G_ux.flow_stack[G_ux.stack_count-1].length
		&& G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index-1] == FLOW_BARRIER) {
		return 1;
	}

	// not the first, for sure
	return 0;
}

unsigned int ux_flow_is_last(void){
	// last ?
	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[G_ux.stack_count-1].steps == NULL
		|| G_ux.flow_stack[G_ux.stack_count-1].length == 0
		|| G_ux.flow_stack[G_ux.stack_count-1].index >= G_ux.flow_stack[G_ux.stack_count-1].length -1) {
		return 1;
	}

	// followed by a flow barrier ?
	if (G_ux.flow_stack[G_ux.stack_count-1].length > 0 
		&& G_ux.flow_stack[G_ux.stack_count-1].index < G_ux.flow_stack[G_ux.stack_count-1].length - 2
		&& G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index+1] == FLOW_BARRIER) {
		return 1;
	}

	// is not last
	return 0;
}

ux_flow_direction_t ux_flow_direction(void) {
	if (G_ux.stack_count) {
		if (G_ux.flow_stack[G_ux.stack_count-1].index > G_ux.flow_stack[G_ux.stack_count-1].prev_index) {
		return FLOW_DIRECTION_FORWARD;
		}
		else if (G_ux.flow_stack[G_ux.stack_count-1].index < G_ux.flow_stack[G_ux.stack_count-1].prev_index) {
			return FLOW_DIRECTION_BACKWARD;
		}
	}
  return FLOW_DIRECTION_START;
}

const ux_flow_step_t* ux_flow_get_current(void) {
	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[G_ux.stack_count-1].steps == NULL
		|| G_ux.flow_stack[G_ux.stack_count-1].index >= G_ux.flow_stack[G_ux.stack_count-1].length
		|| G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index] == FLOW_BARRIER
		|| G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index] == FLOW_LOOP
		|| G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index] == FLOW_END_STEP) {
		return NULL;
	}
	return STEPPIC(G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index]);
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
	// last reached already (need validation, not next)
	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[G_ux.stack_count-1].steps == NULL
		|| G_ux.flow_stack[G_ux.stack_count-1].length <= 1
		|| G_ux.flow_stack[G_ux.stack_count-1].index >= G_ux.flow_stack[G_ux.stack_count-1].length -1) {
		return;
	}

	// followed by a flow barrier ? => need validation instead of next
	if (G_ux.flow_stack[G_ux.stack_count-1].index <= G_ux.flow_stack[G_ux.stack_count-1].length - 2) {
		if (G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index+1] == FLOW_BARRIER) {
			return;
		}

		// followed by a flow barrier ? => need validation instead of next
		if (G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index+1] == FLOW_LOOP) {
			// display first step, fake direction as forward
			G_ux.flow_stack[G_ux.stack_count-1].prev_index = G_ux.flow_stack[G_ux.stack_count-1].index = 0;
			ux_flow_engine_init_step(G_ux.stack_count-1);
			return;
		}
	}

	// advance flow pointer and display it (skip META STEPS)
	G_ux.flow_stack[G_ux.stack_count-1].prev_index = G_ux.flow_stack[G_ux.stack_count-1].index;
	G_ux.flow_stack[G_ux.stack_count-1].index++;
	if (display_step) {
		ux_flow_engine_init_step(G_ux.stack_count-1);
	}
}

void ux_flow_next_no_display(void) {
	ux_flow_next_internal(0);
}

void ux_flow_next(void) {
	ux_flow_next_internal(1);
}

void ux_flow_prev(void) {
	// first reached already
	if (!ux_flow_check_valid()
		|| G_ux.flow_stack[G_ux.stack_count-1].steps == NULL
		|| G_ux.flow_stack[G_ux.stack_count-1].length <= 1
		|| (G_ux.flow_stack[G_ux.stack_count-1].index == 0 
			  && G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].length-1] != FLOW_LOOP)) {
		return;
	}

	// loop in flow (before checking barrier as there is no prestep when looping)
	if (G_ux.flow_stack[G_ux.stack_count-1].index == 0
		&& G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].length-1] == FLOW_LOOP) {
		// display last step (shall skip BARRIER if any, but a flow finishing with a BARRIER is cryptic)
		G_ux.flow_stack[G_ux.stack_count-1].index = G_ux.flow_stack[G_ux.stack_count-1].length-2;
		// fact direction as backward
		G_ux.flow_stack[G_ux.stack_count-1].prev_index = G_ux.flow_stack[G_ux.stack_count-1].index+1;
		ux_flow_engine_init_step(G_ux.stack_count-1);
		return;
	}

	// previous item is a flow barrier ?
	if (G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index-1] == FLOW_BARRIER) {
		return;
	}

	// advance flow pointer and display it (skip META STEPS)
	G_ux.flow_stack[G_ux.stack_count-1].prev_index = G_ux.flow_stack[G_ux.stack_count-1].index;
	G_ux.flow_stack[G_ux.stack_count-1].index--;

	ux_flow_engine_init_step(G_ux.stack_count-1);
}

void ux_flow_validate(void) {
	// no flow ?
	if (!ux_flow_check_valid()
	  || G_ux.flow_stack[G_ux.stack_count-1].steps == NULL
		|| G_ux.flow_stack[G_ux.stack_count-1].length == 0
		|| G_ux.flow_stack[G_ux.stack_count-1].index >= G_ux.flow_stack[G_ux.stack_count-1].length) {
		return;
	}

	// no validation flow ?
	if (STEPPIC(G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index])->validate_flow != NULL) {
		// execute validation flow
		ux_flow_init(G_ux.stack_count-1, STEPSPIC(STEPPIC(G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index])->validate_flow), NULL);
	}
	else {
		// if next is a barrier, then proceed to the item after the barrier
		// if NOT followed by a barrier, then validation is only performed through 
		// a validate_flow specified in the step, else ignored
		if (G_ux.flow_stack[G_ux.stack_count-1].length > 0 
			&& G_ux.flow_stack[G_ux.stack_count-1].index <= G_ux.flow_stack[G_ux.stack_count-1].length - 2) {

			if (G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index+1] == FLOW_BARRIER) {

				// take into account multi barrier at once, kthx poor code review
				while (G_ux.flow_stack[G_ux.stack_count-1].length > 0 
					&& G_ux.flow_stack[G_ux.stack_count-1].index <= G_ux.flow_stack[G_ux.stack_count-1].length - 2
					&& G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index+1] == FLOW_BARRIER) {
					G_ux.flow_stack[G_ux.stack_count-1].index++;
				}
				// skip to next step
				G_ux.flow_stack[G_ux.stack_count-1].prev_index = G_ux.flow_stack[G_ux.stack_count-1].index;
				G_ux.flow_stack[G_ux.stack_count-1].index++;

				// execute reached step
				ux_flow_engine_init_step(G_ux.stack_count-1);
			}
			// reached the last step, but step if FLOW_LOOP
			else if (G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index+1] == FLOW_LOOP) {
				// we go the forward direction
				G_ux.flow_stack[G_ux.stack_count-1].prev_index = G_ux.flow_stack[G_ux.stack_count-1].index = 0;
				// execute reached step
				ux_flow_engine_init_step(G_ux.stack_count-1);
			}
		}
	}
}

void ux_flow_error(unsigned int error) {
	UNUSED(error);
	if (G_ux.flow_stack[G_ux.stack_count-1].steps == NULL
		|| G_ux.flow_stack[G_ux.stack_count-1].length == 0
		|| G_ux.flow_stack[G_ux.stack_count-1].index >= G_ux.flow_stack[G_ux.stack_count-1].length) {
		return;
	}

	if (STEPPIC(G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index])->error_flow != NULL) {
		ux_flow_init(G_ux.stack_count-1, STEPSPIC(STEPPIC(G_ux.flow_stack[G_ux.stack_count-1].steps[G_ux.flow_stack[G_ux.stack_count-1].index])->error_flow), NULL);
	}
}

/** 
 * Last step is marked with a FLOW_END_STEP value
 */
#define FLOW_END_STEP ((void*)0xFFFFFFFFUL)
#define FLOW_BARRIER  ((void*)0xFFFFFFFEUL)
#define FLOW_START    ((void*)0xFFFFFFFDUL)
void ux_flow_init(unsigned int stack_slot, const ux_flow_step_t* const * steps, const ux_flow_step_t* const start_step) {
	G_ux.flow_stack[stack_slot].length = G_ux.flow_stack[stack_slot].prev_index = G_ux.flow_stack[stack_slot].index = 0;
	G_ux.flow_stack[stack_slot].steps = NULL;
	
	// reset paging to avoid troubles if first step is a paginated step
	os_memset(&G_ux.layout_paging, 0, sizeof(G_ux.layout_paging));

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
	memset(&G_ux.flow_stack[stack_slot], 0, sizeof(G_ux.flow_stack[stack_slot]));
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

void* ux_stack_get_step_params(unsigned int stack_slot) {
	if (stack_slot >= UX_STACK_SLOT_COUNT) {
		return NULL;
	}

	if (G_ux.flow_stack[stack_slot].length == 0) {
		return NULL;
	}

	if (G_ux.flow_stack[stack_slot].index >= G_ux.flow_stack[stack_slot].length) {
		return NULL;
	}

	return (void*)PIC(STEPPIC(STEPSPIC(G_ux.flow_stack[stack_slot].steps)[G_ux.flow_stack[stack_slot].index])->params);
}

void* ux_stack_get_current_step_params(void) {
	return ux_stack_get_step_params(G_ux.stack_count-1);
}

unsigned int ux_flow_relayout(void) {
	// if a flow is defined and valid
	if (ux_flow_get_current() != NULL) {
		ux_flow_engine_init_step(G_ux.stack_count-1);
		return 1;
	}
	return 0;
}

#endif // HAVE_UX_FLOW
