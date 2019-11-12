
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

#ifdef HAVE_TINY_COROUTINE

#include "os.h"

#ifndef TCR_MAX_COUNT
#define TCR_MAX_COUNT 2 /*seph and app*/
#endif // TCR_MAX_COUNT

#define TCR_NO_COROUTINE (-1UL)

typedef struct tcr_s {
#ifdef HAVE_TINY_COROUTINE_CRC
	unsigned int crc;
	#define TCR_MAGIC 0x54430052UL
	unsigned int magic;
#endif // HAVE_TINY_COROUTINE_CRC
	unsigned int coroutine_round_robin;
	unsigned int coroutine_count;
	unsigned int current_coroutine;
	unsigned int prev_coroutine;
	struct {
		#define TCR_FLAG_CRC_COMPUTED 0x10
		#define TCR_FLAG_PRIORITY_MASK 0xF
		unsigned int flags;
		unsigned int stack_adr;
		unsigned int stack_len;
#ifdef HAVE_TINY_COROUTINE_CRC
		unsigned int stack_crc;
#endif // HAVE_TINY_COROUTINE_CRC
		jmp_buf jmpctx;
	} coroutines [TCR_MAX_COUNT];
} tcr_t;

/**
 * The main coroutine context.
 */
tcr_t G_tcr;

/**
 * ARM Cortex M0/3/4 setjmp content.
 */
struct arm_setjmp_s {
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int sl;
	unsigned int fp;
	unsigned int sp;
	unsigned int lr;
};


/**
 * Define it weak to make it possible to do the same on other platforms. 
 * TCR is originally designed for ARM platforms (Cortex M0/3/4).
 */
void tcr_init_stack(unsigned int coroutine_idx, tcr_entry_function_t coroutine_entry)  __attribute__ ((weak));
void tcr_init_stack(unsigned int coroutine_idx, tcr_entry_function_t coroutine_entry) {
	// initialize LR (coroutine entry point) and SP (coroutine stack pointer)
	struct arm_setjmp_s* ctx = (struct arm_setjmp_s*)G_tcr.coroutines[coroutine_idx].jmpctx;
	// start at end of coroutine stack (align on 8 to be real compliant with ARM EABI conventions)
	ctx->sp = G_tcr.coroutines[coroutine_idx].stack_adr + G_tcr.coroutines[coroutine_idx].stack_len - G_tcr.coroutines[coroutine_idx].stack_len%8;
	// set the entry function pointer
	ctx->lr = (unsigned int) coroutine_entry | 1 /* force Thumb mode */;

	// from now, when first longjmp in the coroutine context, the coroutine will run in its stack with the inited entry function
}

#ifdef HAVE_TINY_COROUTINE_CRC
static void tcr_integrity_check(void) {
	if (cx_crc16(&G_tcr.magic, sizeof(G_tcr)-offsetof(tcr_t, magic)) != G_tcr.crc) {
		PRINTF("TCR context altered\n");
		THROW(EXCEPTION);
	}
}

static void tcr_integrity_seal(void) {
	G_tcr.crc = cx_crc16(&G_tcr.magic, sizeof(G_tcr)-offsetof(tcr_t, magic));
}
#endif // HAVE_TINY_COROUTINE_CRC

void tcr_init(void) {
	// wipe the whole context
	os_memset(&G_tcr, 0, sizeof(G_tcr));
#ifdef HAVE_TINY_COROUTINE_CRC
	// set the magic
	G_tcr.magic = TCR_MAGIC;
	// seal the structure content
	tcr_integrity_seal();
#endif // HAVE_TINY_COROUTINE_CRC
}


/**
 * Create a new co routine for the given ram interval and given coroutine init function.
 * Throw and exception when no more co routine slot available
 */
void tcr_add(unsigned int priority, unsigned int stack_ptr, unsigned int stack_size, tcr_entry_function_t coroutine_entry) {
#ifdef HAVE_TINY_COROUTINE_CRC
	tcr_integrity_check();
#endif // HAVE_TINY_COROUTINE_CRC
	if (G_tcr.coroutine_count >= TCR_MAX_COUNT) {
		PRINTF("Too much coroutine, check TCR_MAX_COUNT\n");
		THROW(EXCEPTION);
	}
	// support coroutine delete
	os_memset(&G_tcr.coroutines[G_tcr.coroutine_count], 0, sizeof(G_tcr.coroutines[G_tcr.coroutine_count]));
	// initialize coroutine context
	G_tcr.coroutines[G_tcr.coroutine_count].stack_adr = stack_ptr;
	G_tcr.coroutines[G_tcr.coroutine_count].stack_len = stack_size;
	G_tcr.coroutines[G_tcr.coroutine_count].flags = 0; // CRC NOT computed
	G_tcr.coroutines[G_tcr.coroutine_count].flags = 0 | (priority & 0xF); // CRC NOT computed
  tcr_init_stack(G_tcr.coroutine_count, coroutine_entry);
	G_tcr.coroutine_count++;
#ifdef HAVE_TINY_COROUTINE_CRC
	tcr_integrity_seal();
#endif // HAVE_TINY_COROUTINE_CRC
}

/**
 * Exit the currently running task
 */
void tcr_harakiri(void) {
#ifdef HAVE_TINY_COROUTINE_CRC
	tcr_integrity_check();
#endif // HAVE_TINY_COROUTINE_CRC

	// move the array of coroutine contexts if any to move
	if(G_tcr.current_coroutine < TCR_MAX_COUNT-1) {
		os_memmove(&G_tcr.coroutines[G_tcr.current_coroutine], 
			         &G_tcr.coroutines[G_tcr.current_coroutine+1], 
			         // remaining coroutine slots after the killed one
			         sizeof(G_tcr.coroutines[0]) * (TCR_MAX_COUNT-1-G_tcr.current_coroutine) );	
	}
	/* not necessary
	// else: coroutine_count-1 == current_coroutine, slot is wiped
	// wipe the last slot (moved)
	os_memset(&G_tcr.coroutines[G_tcr.coroutine_count-1], 0, sizeof(G_tcr.coroutines[0]));
	*/
	// decrement number of tasks
	G_tcr.coroutine_count--;
	// prepare next running coroutine
	G_tcr.coroutine_round_robin = (G_tcr.coroutine_round_robin + 1) % G_tcr.coroutine_count;
#ifdef HAVE_TINY_COROUTINE_CRC
	tcr_integrity_seal();
#endif // HAVE_TINY_COROUTINE_CRC

	// start another registered coroutine
	tcr_start();
}

#ifdef HAVE_TINY_COROUTINE_CRC
// internal function to compute stack crc
static void tcr_stack_crc_compute(unsigned int coroutine_idx) {
	// compute CRC
	G_tcr.coroutines[coroutine_idx].stack_crc = cx_crc16((const unsigned char*)G_tcr.coroutines[coroutine_idx].stack_adr, 
		                                         G_tcr.coroutines[coroutine_idx].stack_len);
}

// internal function to check stack crc and reject it when invalid
static void tcr_stack_crc_check(unsigned int coroutine_idx) {
	unsigned int crc = cx_crc16((const unsigned char*)G_tcr.coroutines[coroutine_idx].stack_adr, 
                              G_tcr.coroutines[coroutine_idx].stack_len);
	if (crc != G_tcr.coroutines[coroutine_idx].stack_crc) {
		PRINTF("coroutine stack has been tampered with\n");
		THROW(EXCEPTION);
	}
}
#endif  // HAVE_TINY_COROUTINE_CRC

// internal function to get the next coroutine to run
static unsigned int tcr_next_coroutine_idx(void) {
	// perform a modulo in case a coroutine exited since last call
	unsigned int cr = G_tcr.coroutine_round_robin;
	if (G_tcr.coroutine_count == 0) {
		PRINTF("No coroutine to run\n");
		THROW(EXCEPTION);
	}
	// todo, code the priority enforcement

	// prepare next coroutine to run
	G_tcr.coroutine_round_robin = (G_tcr.coroutine_round_robin + 1) % G_tcr.coroutine_count;
	// this is the coroutine to run
	return cr;
}

// effectively switch to the task
static void tcr_switch_to_coroutine(unsigned int next_coroutine) {
#ifdef HAVE_TINY_COROUTINE_CRC
  // verify next stack, if already yielded once (don't compute crc for uninitialized crc)
  if (G_tcr.coroutines[next_coroutine].flags & TCR_FLAG_CRC_COMPUTED) {
    tcr_stack_crc_check(next_coroutine);
  }
#endif // HAVE_TINY_COROUTINE_CRC
  // switch to next coroutine
  G_tcr.current_coroutine = next_coroutine;
#ifdef HAVE_TINY_COROUTINE_CRC
  // seal the context
  tcr_integrity_seal();
#endif // HAVE_TINY_COROUTINE_CRC
  // note: longjmp value could be a magic, but is not
  longjmp(G_tcr.coroutines[G_tcr.current_coroutine].jmpctx, 1);
}

// this function must have the smallest stack. or be split into subcalls
void tcr_yield(void) {
#ifdef HAVE_TINY_COROUTINE_CRC
	tcr_integrity_check();
#endif // HAVE_TINY_COROUTINE_CRC
  // find next coroutine index depending on coroutine usage and next scheduled (forced coroutine delegation)
  unsigned int next_coroutine = tcr_next_coroutine_idx();
  // save current call stack
  if (0 == setjmp(G_tcr.coroutines[G_tcr.current_coroutine].jmpctx)) {
    // hodl current stack
    G_tcr.prev_coroutine = G_tcr.current_coroutine;
    tcr_switch_to_coroutine(next_coroutine);
    // NO RETURN HERE
  }

#ifdef HAVE_TINY_COROUTINE_CRC
  // check when yielding in this coroutine
  tcr_integrity_check();

  // previous task is not tcr_start?
  if (G_tcr.prev_coroutine != TCR_NO_COROUTINE) { 
	  // we've been yielded, save previous caller's stack
	  tcr_stack_crc_compute(G_tcr.prev_coroutine);
	  // mark crc as computed (to be checked next time)
	  G_tcr.coroutines[G_tcr.prev_coroutine].flags |= TCR_FLAG_CRC_COMPUTED;
		tcr_integrity_seal();
	}
#endif // HAVE_TINY_COROUTINE_CRC

	// give back the hand to the user coroutine
}

/*
 * Run the first registered coroutine to bootstrap the co routine mechanism
 */
void tcr_start(void) {
	// TODO addtionnal test: we must not be call from within an enabled coroutine 
	//                       (this is not the right way) => works ok from harakiri 
	//                       as the coroutine has been disabled.
#ifdef HAVE_TINY_COROUTINE_CRC
	tcr_integrity_check();
#endif // HAVE_TINY_COROUTINE_CRC
  G_tcr.prev_coroutine = TCR_NO_COROUTINE;

  // switch to next coroutine
	tcr_switch_to_coroutine(tcr_next_coroutine_idx());
}

#endif // HAVE_TINY_COROUTINE
