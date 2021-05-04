.syntax unified
.text
.thumb
.thumb_func
.global SVC_cx_call

SVC_cx_call:
    svc 1
    bx lr
