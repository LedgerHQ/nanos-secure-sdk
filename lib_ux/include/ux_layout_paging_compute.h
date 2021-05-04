
#include "ux_layouts.h"

#ifdef HAVE_UX_FLOW

#define PIXEL_PER_LINE 114

# define PAGING_FORMAT_NN         0x00
# define PAGING_FORMAT_BN         0xF0
# define PAGING_FORMAT_NB         0x0F
# define PAGING_FORMAT_BB         (PAGING_FORMAT_BN | PAGING_FORMAT_NB)

typedef struct {
  unsigned int current;
  unsigned int count;
	unsigned char format;
  unsigned short offsets[UX_LAYOUT_PAGING_LINE_COUNT];
  unsigned short lengths[UX_LAYOUT_PAGING_LINE_COUNT];
} ux_layout_paging_state_t;

unsigned int ux_layout_paging_compute(const char* text_to_split, 
                                      unsigned int page_to_display,
                                      ux_layout_paging_state_t* paging_state,
                                      bagl_font_id_e font
                                      );

#endif // HAVE_UX_FLOW
