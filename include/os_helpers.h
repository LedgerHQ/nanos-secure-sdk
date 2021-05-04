#pragma once

/* ----------------------------------------------------------------------- */
/*   -                            HELPERS                                - */
/* ----------------------------------------------------------------------- */

#define OS_PARSE_BERTLV_OFFSET_COMPARE_WITH_BUFFER 0x80000000UL
#define OS_PARSE_BERTLV_OFFSET_GET_LENGTH 0x40000000UL

unsigned int os_parse_bertlv(unsigned char *mem, unsigned int mem_len,
                             unsigned int *tlv_instance_offset,
                             unsigned int tag, unsigned int offset,
                             void **buffer, unsigned int maxlength);

#define UNUSED(x) (void)x
