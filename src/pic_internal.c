

unsigned int pic_internal(unsigned int link_address) __attribute__((naked));
unsigned int pic_internal(unsigned int link_address) 
{
  // compute the delta offset between LinkMemAddr & ExecMemAddr
  __asm volatile ("mov r2, pc\n");          // r2 = 0x109004
  __asm volatile ("ldr r1, =pic_internal\n");        // r1 = 0xC0D00001
  __asm volatile ("adds r1, r1, #3\n");     // r1 = 0xC0D00004
  __asm volatile ("subs r1, r1, r2\n");     // r1 = 0xC0BF7000 (delta between load and exec address)

  // adjust value of the given parameter
  __asm volatile ("subs r0, r0, r1\n");     // r0 = 0xC0D0C244 => r0 = 0x115244
  __asm volatile ("bx lr\n");
}
