

// gloomy fake definition to avoid problem with O3 and llvm with ignored return value in the caller
unsigned int pic_internal(unsigned int link_address);

// only apply PIC conversion if link_address is in linked code (over 0xC0D00000 in our example)
// this way, PIC call are armless if the address is not meant to be converted
extern unsigned int _nvram;
extern unsigned int _envram;
extern unsigned int _bss;
extern unsigned int _estack;

unsigned int pic(unsigned int link_address) {

	// check if in the LINKED TEXT zone
	unsigned int n,en;
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
		link_address = link_address - n + en;
  }
	return link_address;
}

