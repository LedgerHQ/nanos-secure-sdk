.syntax unified
.text
.thumb

.section .text._cx_trampoline
.thumb_func
.global _cx_trampoline
_cx_trampoline:
  // r12 = cx_exported_functions[i]
  lsls r0, #2
  ldr  r1, =cx_exported_functions
  ldr  r1, [r1, r0]
  mov  r12, r1
  // jump to function
  pop  {r0-r1}
  bx   r12
