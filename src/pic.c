#include "os_pic.h"

// only apply PIC conversion if link_address is in linked code (over 0xC0D00000 in our example)
// this way, PIC call are armless if the address is not meant to be converted
extern void _nvram;
extern void _envram;

void *pic(void *link_address) {
  if (link_address >= &_nvram && link_address < &_envram) {
    link_address = pic_internal(link_address);
  }
  return link_address;
}
