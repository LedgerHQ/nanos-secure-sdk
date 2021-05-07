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
