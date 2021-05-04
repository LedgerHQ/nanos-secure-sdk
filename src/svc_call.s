.syntax unified
.text
.thumb
.thumb_func
.global SVC_Call

SVC_Call:
    svc 1
    cmp r1, #0
    bne exception
    bx lr
exception:
    // THROW(ex);
    mov r0, r1
    bl os_longjmp
