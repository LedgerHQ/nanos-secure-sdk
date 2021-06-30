/* @BANNER@ */

#include "os.h"

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
uint32_t cx_rng_u32_range_func(uint32_t a, uint32_t b, cx_rng_u32_range_randfunc_t randfunc) {
  uint32_t range = b - a;
  uint32_t r;

  if ((range & (range - 1)) == 0) {  // special case: range is a power of 2
    r = randfunc();
    return a + r % range;
  }

  uint32_t chunk_size = UINT32_MAX / range;
  uint32_t last_chunk_value = chunk_size * range;
  r = randfunc();
  while (r >= last_chunk_value) {
    r = randfunc();
  }
  return a + r / chunk_size;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
uint32_t cx_rng_u32_range(uint32_t a, uint32_t b) {
  return cx_rng_u32_range_func(a, b, (cx_rng_u32_range_randfunc_t*)cx_rng_u32);
}

