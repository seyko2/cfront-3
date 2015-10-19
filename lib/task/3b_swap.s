	.file	"3b_swap.s"
#	ident	"@(#)cfront:lib/task/3b_swap.s	1.2"

	#	swap of SHARED

	.data
	.globl rr4
	.globl rr3
	.globl rr2

	.text
	.globl sswap
	.align	4
sswap:
	save &1
	movw	0(%ap),%r8	# this
	movw	rr4,%r2	# t_savearea
	movw	rr3,%r1	# sz
	movw	rr2,%r0	# t_basep
.L1:
	cmpw	%r1,&0
	jle	.L2
	subw2	&1,%r1
	movw	0(%r2),0(%r0)
	addw2	&4,%r2
	addw2	&4,%r0
	jmp 	.L1
.L2:
	#  the following constant is the displacement of t_framep in task
	movw	20(%r8),%fp	#	fp = this->t_framep
	movw	24(%r8),%r0	#	fudge return -- this->th
	movw	28(%r8),%ap	#	this->t_ap
	ret &6

	.text
	.globl swap
	.align	4
swap:
	save &0
	movw	0(%ap),%r1	#	r1 = this
	#  the following constant is the displacement of t_framep in task
	movw	20(%r1),%fp	#	fp = this->t_framep
	movw	24(%r1),%r0	#	fudge return -- this->th
	movw	28(%r1),%ap	#	this->t_ap
	ret &6
