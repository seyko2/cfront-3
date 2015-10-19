	.file	"vax_swap.s"
#	ident	"@(#)cfront:lib/task/vax_swap.s	1.2"

        #       swap of SHARED

        .globl _rr4
        .globl _rr3
        .globl _rr2

        .globl _sswap
        .align  1
_sswap:
        .word   0x0000
        movl    4(ap),r1        # this
        movl    _rr4,r4
        movl    _rr3,r3
        movl    _rr2,r2

L1:
        tstl    r3
        jeql    L2
        decl    r3
        movl    (r4),(r2)
        cmpl    -(r4),-(r2)
        jbr     L1
L2:
        #  the following constant is the displacement of t_framep in task
        movl    20(r1),fp       #       fp = this->t_framep
        movl    24(r1),r0       #       fudge return -- this->th
        ret

        .globl _swap
        .align  1
_swap:
        .word   0x0000
        movl    4(ap),r1        #       r1 = this
        #  the following constant is the displacement of t_framep in task
        movl    20(r1),fp       #       fp = this->t_framep
        movl    24(r1),r0       #       fudge return -- this->th
        ret


        .globl  _top
        .align  1
_top:
        .word   0x0000
        addl3   $1,(ap),r0
        ashl    $2,r0,r0
        addl2   ap,r0
        ret
