#include "bolos_target.h"
#include "os_pic.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
__attribute__((naked)) void *pic_internal(void *link_address)
{
  // compute the delta offset between LinkMemAddr & ExecMemAddr
  __asm volatile ("mov r2, pc\n");
  __asm volatile ("ldr r1, =pic_internal\n");
  __asm volatile ("adds r1, r1, #3\n");
  __asm volatile ("subs r1, r1, r2\n");

  // adjust value of the given parameter
  __asm volatile ("subs r0, r0, r1\n");
  __asm volatile ("bx lr\n");
}
#pragma GCC diagnostic pop

// only apply PIC conversion if link_address is in linked code (over 0xC0D00000 in our example)
// this way, PIC call are armless if the address is not meant to be converted
extern void _nvram;
extern void _envram;

#if defined(ST31)

void *pic(void *link_address) {
  // check if in the LINKED TEXT zone
  if (link_address >= &_nvram && link_address < &_envram) {
    link_address = pic_internal(link_address);
  }

  return link_address;
}

#elif defined(ST33)

extern void _bss;
extern void _estack;

void *pic(void *link_address) {
  void *n, *en;

  // check if in the LINKED TEXT zone
  __asm volatile("ldr %0, =_nvram":"=r"(n));
  __asm volatile("ldr %0, =_envram":"=r"(en));
  if (link_address >= n && link_address <= en) {
    link_address = pic_internal(link_address);
  }

  // check if in the LINKED RAM zone
  __asm volatile("ldr %0, =_bss":"=r"(n));
  __asm volatile("ldr %0, =_estack":"=r"(en));
  if (link_address >= n && link_address <= en) {
    __asm volatile("mov %0, r9":"=r"(en));
    // deref into the RAM therefore add the RAM offset from R9
    link_address = (char *)link_address - (char *)n + (char *)en;
  }

  return link_address;
}

#else

#error "invalid architecture"

#endif
