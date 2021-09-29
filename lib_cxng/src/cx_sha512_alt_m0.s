@ vim:ft=armv6

#if defined(HAVE_SHA512) || defined(HAVE_SHA384)
        
#ifdef HAVE_SHA512_WITH_BLOCK_ALT_METHOD_M0
        .syntax unified
        .thumb
        .text


        
@ Convention ri,ri+1 are 64bits, with ri is LSB: 64bits full little-endian
@ TODO use new rotr64 macro instead of function

@ ============================================================
@ WARN: big MSB,LSB   MSB,LSB
_primeSqrt:
.word 0x428a2f98,0xd728ae22, 0x71374491,0x23ef65cd 
.word 0xb5c0fbcf,0xec4d3b2f, 0xe9b5dba5,0x8189dbbc 
.word 0x3956c25b,0xf348b538, 0x59f111f1,0xb605d019
.word 0x923f82a4,0xaf194f9b, 0xab1c5ed5,0xda6d8118 
.word 0xd807aa98,0xa3030242, 0x12835b01,0x45706fbe  
.word 0x243185be,0x4ee4b28c, 0x550c7dc3,0xd5ffb4e2
.word 0x72be5d74,0xf27b896f, 0x80deb1fe,0x3b1696b1
.word 0x9bdc06a7,0x25c71235, 0xc19bf174,0xcf692694
.word 0xe49b69c1,0x9ef14ad2, 0xefbe4786,0x384f25e3
.word 0x0fc19dc6,0x8b8cd5b5, 0x240ca1cc,0x77ac9c65
.word 0x2de92c6f,0x592b0275, 0x4a7484aa,0x6ea6e483
.word 0x5cb0a9dc,0xbd41fbd4, 0x76f988da,0x831153b5
.word 0x983e5152,0xee66dfab, 0xa831c66d,0x2db43210
.word 0xb00327c8,0x98fb213f, 0xbf597fc7,0xbeef0ee4
.word 0xc6e00bf3,0x3da88fc2, 0xd5a79147,0x930aa725 
.word 0x06ca6351,0xe003826f, 0x14292967,0x0a0e6e70
.word 0x27b70a85,0x46d22ffc, 0x2e1b2138,0x5c26c926
.word 0x4d2c6dfc,0x5ac42aed, 0x53380d13,0x9d95b3df
.word 0x650a7354,0x8baf63de, 0x766a0abb,0x3c77b2a8
.word 0x81c2c92e,0x47edaee6, 0x92722c85,0x1482353b
.word 0xa2bfe8a1,0x4cf10364, 0xa81a664b,0xbc423001
.word 0xc24b8b70,0xd0f89791, 0xc76c51a3,0x0654be30
.word 0xd192e819,0xd6ef5218, 0xd6990624,0x5565a910
.word 0xf40e3585,0x5771202a, 0x106aa070,0x32bbd1b8
.word 0x19a4c116,0xb8d2d0c8, 0x1e376c08,0x5141ab53 
.word 0x2748774c,0xdf8eeb99, 0x34b0bcb5,0xe19b48a8
.word 0x391c0cb3,0xc5c95a63, 0x4ed8aa4a,0xe3418acb
.word 0x5b9cca4f,0x7763e373, 0x682e6ff3,0xd6b2b8a3
.word 0x748f82ee,0x5defb2fc, 0x78a5636f,0x43172f60
.word 0x84c87814,0xa1f0ab72, 0x8cc70208,0x1a6439ec
.word 0x90befffa,0x23631e28, 0xa4506ceb,0xde82bde9
.word 0xbef9a3f7,0xb2c67915, 0xc67178f2,0xe372532b
.word 0xca273ece,0xea26619c, 0xd186b8c7,0x21c0c207
.word 0xeada7dd6,0xcde0eb1e, 0xf57d4f7f,0xee6ed178
.word 0x06f067aa,0x72176fba, 0x0a637dc5,0xa2c898a6 
.word 0x113f9804,0xbef90dae, 0x1b710b35,0x131c471b 
.word 0x28db77f5,0x23047d84, 0x32caab7b,0x40c72493
.word 0x3c9ebe0a,0x15c9bebc, 0x431d67c4,0x9c100d4c
.word 0x4cc5d4be,0xcb3e42b6, 0x597f299c,0xfc657e2a
.word 0x5fcb6fab,0x3ad6faec, 0x6c44198c,0x4a475817



@ ============================================================

@ cx_sha512_t field offset
@ struct cx_sha512_s {
@         cx_md_t               algo;
@         unsigned int           counter;
@         unsigned int     blen;
@         unsigned char  block[128];
@         unsigned char  acc[8*8];
@ }
.equ OFSalgo,           0
.equ OFScounter,        4
.equ OFSblen,           8
.equ OFSblock,          12
.equ OFSacc,            140

@ locals stack
.equ _H ,       0
.equ _G ,       8
.equ _F ,       16 
.equ _E ,       24
.equ _D ,       32
.equ _C ,       40
.equ _B ,       48
.equ _A ,       56
.equ _hash,     64

.equ _cx_sha512_block_locals, 68

        
;@ rh,rl = A|B|...|H
.macro peekAH rl,rh, _AH
        LDR     \rl, [SP,\_AH]
        LDR     \rh, [SP,\_AH+4]
.endm


@ convert loop j to index j; j%16
@ then convert index to offset: j*8
@ do that in place
.macro offsetJ  rj 
        LSLS    \rj, #32-4
        LSRS    \rj, #32-7
.endm


@ orl,orh=rotr64(r0,r1, n)
@ pre: orl,orh != r0,r1
.macro _mcx_rotr64_32 n,orl,orh
                MOV     \orl, r0
                MOV     \orh, r1
                LSRS    \orl, #\n
                LSRS    \orh, #\n
                LSLS    r0, #32-(\n)
                LSLS    r1, #32-(\n)
                ORRS    \orh, r0
                ORRS    \orl, r1
.endm

@ perform a long addition correctly
@ rc = ra + rc, rd = rd + rb + carry(ra+rc)
.macro add_doubleword  ra, rb, rc, rd
        ADDS    \rc, \ra
        ADCS    \rd, \rb
.endm

@ ============================================================
                        .thumb_func
                        .global cx_sha512_block

                        @     void cx_sha512_block(cx_sha512_t  *hash)
                        @     v: RO  <-- hash

@ r8 hold X block pointer
@ Stack is:
@   |_H .... _A|*hash|
@    ^SP               ^SP+_cx_sha512_block_locals
@
@ r6:     j
@ r7:     X, aka hash->block
@ SP[0]:  acc
@ r0-r5:  compute 64bits
@ r8-r12:  64bits tmp backup
@ r10-11:  64bits tmp backup
@ 
cx_sha512_block:
                PUSH    {r4,r5,r6,r7,lr}
                MOV     r4, r8
                MOV     r5, r9
                MOV     r6, r10
                MOV     r7, r11
                PUSH    {r4, r5, r6, r7}
                SUB     SP, #_cx_sha512_block_locals
                @set locals
                STR     r0, [SP, #_hash]                                
                @ _AH:loopACC
                ADDS     r0, #OFSacc
                MOV     r1, SP
                ADDS     r1, #_A
                LDM     r0!, {r2-r7} @A,B,C
                STM     r1!, {r2-r3}
                SUBS     r1, #16
                STM     r1!, {r4-r5}
                SUBS     r1, #16
                STM     r1!, {r6-r7}
                SUBS     r1, #16
                LDM     r0!, {r2-r7} @D,E,F
                STM     r1!, {r2-r3}
                SUBS     r1, #16
                STM     r1!, {r4-r5}
                SUBS     r1, #16
                STM     r1!, {r6-r7}
                SUBS     r1, #16
                LDM     r0!, {r2-r5} @G,H
                STM     r1!, {r2-r3}
                SUBS     r1, #16
                STM     r1!, {r4-r5}
                @ X: block
                LDR     r7, [SP, #_hash]                                
                ADDS     r7, #OFSblock
 

                @swap 8 64bits words of block
                MOV     r0,  r7
                MOV     r1,  r7
                MOV     r8,  r7
                .rept 8
                LDM     r0!, {r4-r7}
                REV     r3,  r4
                REV     r2,  r5
                REV     r5,  r6
                REV     r4,  r7
                STM     r1!, {r2-r5}
                .endr
      
                @80 loop
                MOV     r7, r8
                MOVS    r6, #0
        _loop_80: @TODO use CMP
                CMP     r6, #80
                BNE     _cont_loop_80
                B       _end_loop_80
                
        _cont_loop_80: @TODO use CMP     
                @ loop >= 16
                CMP     r6, #16
                BCS     _cont_16
                B       _end_16
        _cont_16:
                @ X[j-2]
                SUBS    r0, r6, #2
                offsetJ r0                  
                ADDS    r0, r7
                LDM     r0, {r0, r1}
                PUSH    {r0,r1}
                @ sigma1 X[j-2]:  r19,r61,s6
                _mcx_rotr64_32 19,r4,r5
                MOV     r0, r5
                MOV     r1, r4
                _mcx_rotr64_32 10,r2,r3
                EORS    r4, r4, r2
                EORS    r5, r5, r3
                POP     {r0,r1}
                MOV     r2, r1              
                LSRS    r0, #6
                LSRS    r1, #6
                LSLS    r2, #32-6
                ORRS    r0, r0, r2
                EORS    r4, r0
                EORS    r5, r1
                MOV     r10,r4
                MOV     r11,r5

                @ X[j-15]
                MOV     r0, r6
                SUBS     r0, #15
                offsetJ r0
                ADD     r0, r7
                LDM     r0, {r0, r1}
                PUSH    {r0,r1}
                @ + sigma0 X[j-15]: r1,r8,s7
                _mcx_rotr64_32 1,r4,r5     
                MOV     r0, r4
                MOV     r1, r5
                _mcx_rotr64_32 7,r2,r3
                EORS    r4, r4, r2
                EORS    r5, r5, r3
                POP     {r0,r1}
                MOV     r2, r1              
                LSRS    r0, #7
                LSRS    r1, #7
                LSLS    r2, #32-7
                ORRS    r0, r0, r2
                EORS    r4, r0
                EORS    r5, r1
                MOV     r0, r10
                MOV     r1, r11
                add_doubleword r0,r1,r4,r5
 
                @ + X[j-7  %F]
                SUBS    r0, r6, #7
                offsetJ r0
                ADD     r0, r7
                LDM     r0, {r0, r1}
                add_doubleword r0,r1,r4,r5

                @ + X[j-16  %F]
                MOV     r0, r6
                SUBS    r0, #16                  
                offsetJ r0                
                ADD     r0, r7
                LDM     r0, {r0, r1}
                add_doubleword r0,r1,r4,r5
                
                MOV     r0, r6
                offsetJ r0                  
                ADD     r0, r7
                STM     r0!, {r4,r5}  
       _end_16:        

        @compute t1        
                @ sum1(E): r14,r18,r41
                peekAH  r0, r1,#_E
                _mcx_rotr64_32 14,r4,r5
                MOV     r0,r4
                MOV     r1,r5
                _mcx_rotr64_32 4,r2,r3
                MOV     r0, r2
                MOV     r1, r3
                EORS    r4, r2
                EORS    r5, r3
                 _mcx_rotr64_32 23,r2,r3
                EORS    r4, r2
                EORS    r5, r3
                @ + H
                peekAH  r0, r1,#_H
                add_doubleword r0,r1,r4,r5
                MOV     r10,r4
                MOV     r11,r5  
                @  + ch(E,F,G)  : (x & y) ^ (~x & z)
                peekAH  r0, r1, #_E
                peekAH  r2, r3, #_F
                ANDS    r2, r2, r0     @ y = y & x
                ANDS    r3, r3, r1
                MVNS    r0, r0         @ x = ~x
                MVNS    r1, r1
                peekAH  r4, r5, #_G
                ANDS    r4, r4, r0     @ z =  ~x& z
                ANDS    r5, r5, r1
                EORS    r4, r2         @ ^
                EORS    r5, r3 
                MOV     r0, r10
                MOV     r1, r11
                add_doubleword r0,r1,r4,r5
                @ + primeSqrt[j] (warn, MSB|LSB array)
                LDR     r0, =_primeSqrt
                MOV     r1, r6
                LSLS    r1, #3
                ADD     r0, r1
                LDM     r0, {r0,r1} 
                add_doubleword r1,r0,r4,r5
                @ +  X[j&0xF]
                MOV     r0, r6
                offsetJ r0                
                ADD     r0, r7
                LDM     r0, {r0, r1}
                add_doubleword r0,r1,r4,r5
                @ t1                
                MOV     r10,r4
                MOV     r11,r5

        @ compute t2        
                @ sum0(A): r28,r34,r39
                peekAH  r0, r1,#_A
                _mcx_rotr64_32 28,r4,r5
                MOV     r0, r4
                MOV     r1, r5
                _mcx_rotr64_32 6,r2,r3
                MOV     r0, r2
                MOV     r1, r3
                EORS    r4, r2
                EORS    r5, r3
                _mcx_rotr64_32 5,r2,r3
                EORS    r4, r2
                EORS    r5, r3
                MOV     r8, r4
                MOV     r12,r5

                @ maj(A,B,C);  (x & y) ^ (x & z) ^ (y & z)
                peekAH  r0, r1, #_A
                peekAH  r2, r3, #_B
                ANDS    r2, r0, r2     
                ANDS    r3, r1, r3      @ x&y
                peekAH  r4, r5, #_C
                ANDS    r0, r0, r4
                ANDS    r1, r1, r5      @ x&z
                EORS    r0, r2
                EORS    r1, r3          @ ^
                peekAH  r2, r3, #_B
                ANDS    r4, r4, r2      
                ANDS    r5, r5, r3      @ y&z
                EORS    r0, r4
                EORS    r1, r5          @ ^
                @ t2
                MOV     r4, r8
                MOV     r5, r12
                add_doubleword r0,r1,r4,r5
                MOV     r8, r4
                MOV     r12, r5

                @ t1: r10r11
                @ t2: r8r12


        @ rotate state
                MOV     r0, SP
                ADDS    r0, #8
                MOV     r1, SP
                LDM     r0!, {r2,r3,r4,r5} @ load G,F
                STM     r1!, {r2,r3,r4,r5} @ H=G, G=F

                LDM     r0!, {r2,r3,r4,r5} @ load E,D
                STM     r1!, {r2,r3}       @ F=F


                MOV     r2, r10
                MOV     r3, r11
                add_doubleword r2,r3,r4,r5
                STM     r1!, {r4,r5} @ E=D+t1

                LDM     r0!, {r2,r3,r4,r5} @ load C,B
                STM     r1!, {r2,r3,r4,r5} @ D=C, C=B

                LDM     r0!, {r2,r3}       @ load A
                STM     r1!, {r2,r3}       @ B = A

                MOV     r2,r10
                MOV     r3,r11
                MOV     r4,r8
                MOV     r5,r12
                add_doubleword r4,r5,r2,r3
                STM     r1!, {r2,r3}     @A = t1+t2
        
                @ more rounds?
                ADDS    r6,#1
                B       _loop_80
        _end_loop_80:

        @ final chaining

                LDR     r0, [SP, #_hash]                
                ADDS    r0, r0, #OFSacc
                MOV     r1, r0
.macro updateACC _AH
                LDM     r0!, {r2, r3}
                peekAH  r4, r5, \_AH
                add_doubleword r4,r5,r2,r3
                STM     r1!, {r2,r3}
.endm                                        
                updateACC #_A                
                updateACC #_B                
                updateACC #_C                
                updateACC #_D                
                updateACC #_E                
                updateACC #_F                
                updateACC #_G                
                updateACC #_H                
                 
                ADD     SP,#_cx_sha512_block_locals
                POP     {r4, r5, r6, r7}
                MOV     r8, r4
                MOV     r9, r5
                MOV     r10, r6
                MOV     r11, r7
                POP     {r4, r5, r6, r7, pc}

#endif

#endif
