/*ident	"@(#)cls4:lib/task/task/hw_stack.c	1.4" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
#include "hw_stack.h"

/* Machine-dependent asms and functions for task library.
 * Porting these requires intimate knowledge of the stack frame layout,
 * call and return sequences for the machine.
 */

#ifdef	PROC_3B

asm("	.text");
asm("	.globl	SAVE_REGS");
asm("	.align	4");
asm("SAVE_REGS:");
asm("	save &6");
asm("	movw	0(%ap),%r1 ");		/* ptr to HW_REGS */
asm("	movw	-24(%fp),0(%r1) ");	/* move caller's r3 to arg.r3 */
asm("	movw	-20(%fp),4(%r1) ");	/* move caller's r4 to arg.r4 */
asm("	movw	-16(%fp),8(%r1) ");	/* move caller's r5 to arg.r5 */
asm("	movw	-12(%fp),12(%r1) ");	/* move caller's r6 to arg.r6 */
asm("	movw	-8(%fp),16(%r1) ");	/* move caller's r7 to arg.r7 */
asm("	movw	-4(%fp),20(%r1) ");	/* move caller's r8 to arg.r8 */
asm("	ret &6");

/* save_saved_regs copies n_saved regs saved in stack frame defined by fp
 * to a HW_REGS structure.
 */
void
save_saved_regs(HW_REGS *regsp, int* fp)
{
	FrameLayout layout(fp);
	register int* p = FIRST_SAVED_REG_P(fp, layout.n_saved);
	switch(layout.n_saved)  {	
		case 6:
			regsp->r3 = *p++;
			/* FALLTHROUGH */
		case 5:
			regsp->r4 = *p++;
			/* FALLTHROUGH */
		case 4:
			regsp->r5 = *p++;
			/* FALLTHROUGH */
		case 3:
			regsp->r6 = *p++;
			/* FALLTHROUGH */
		case 2:
			regsp->r7 = *p++;
			/* FALLTHROUGH */
		case 1:
			regsp->r8 = *p++;
			/* FALLTHROUGH */
		default:
			break;
	}
}

/* On the 3Bs, the current ap is the caller's sp, but we want caller's sp - 1
 * to point to last used word in stack, to correspond with sp's on 386, vax,
 * sun
 */
asm("	.text");
asm("	.globl	TOP");
asm("	.align	4");
asm("TOP:");
asm("	save &0");
asm("	movw	%ap,%r0");  /* caller's sp */
asm("	addw2	&4,%r0");   /* caller's sp - 1 */
asm("	ret &0");

/* These assembly language functions should be replaced by new-style
 * asms, when (if) they are implemented for C++.
 * for example:  
 * asm void
 * set_r3(p)
 * {
 *     % mem p;
 *     movw *p,%r3
 * }
 */

asm("	.text");
asm("	.globl	set_r3");
asm("	.align	4");
asm("set_r3:");
asm("	save &0");
asm("	movw	0(%ap),%r1 ");		/* ptr to register saved in stack */
asm("	movw 0(%r1),%r3 ");
asm("	ret &0");

asm("	.text");
asm("	.globl	set_r4");
asm("	.align	4");
asm("set_r4:");
asm("	save &0");
asm("	movw	0(%ap),%r1 ");		/* ptr to register saved in stack */
asm("	movw 0(%r1),%r4 ");
asm("	ret &0");

asm("	.text");
asm("	.globl	set_r5");
asm("	.align	4");
asm("set_r5:");
asm("	save &0");
asm("	movw	0(%ap),%r1 ");		/* ptr to register saved in stack */
asm("	movw 0(%r1),%r5 ");
asm("	ret &0");

asm("	.text");
asm("	.globl	set_r6");
asm("	.align	4");
asm("set_r6:");
asm("	save &0");
asm("	movw	0(%ap),%r1 ");		/* ptr to register saved in stack */
asm("	movw 0(%r1),%r6 ");
asm("	ret &0");

asm("	.text");
asm("	.globl	set_r7");
asm("	.align	4");
asm("set_r7:");
asm("	save &0");
asm("	movw	0(%ap),%r1 ");		/* ptr to register saved in stack */
asm("	movw 0(%r1),%r7 ");
asm("	ret &0");

asm("	.text");
asm("	.globl	set_r8");
asm("	.align	4");
asm("set_r8:");
asm("	save &0");
asm("	movw	0(%ap),%r1 ");		/* ptr to register saved in stack */
asm("	movw 0(%r1),%r8 ");
asm("	ret &0");

#ifdef	u3b

asm("	.text");
asm("	.globl	AP");
asm("	.align	4");
asm("AP:");
asm("	save &0");
asm("	movw	-48(%fp),%r0");  /* r0 = caller's ap */
asm("	ret &0");

asm("	.text");
asm("	.globl	FP");
asm("	.align	4");
asm("FP:");
asm("	save &0");
asm("	movw	-44(%fp),%r0");  /* r0 = caller's fp */
asm("	ret &0");

/* On the 3B20 the sp is restored from the current ap on a return
 * FUDGE_SP overwrites the saved ap with a new value (the 1st arg), causing
 * the caller's ap to be changed to the new value.
 * As a precaution, the 1st arg of caller is copied down to where new ap
 * will point.  (In case caller references 1st arg (implicit "this" arg)
 * to set return value.)
 * 2nd arg unnecessary on 3B20. 
 */
asm("	.text");
asm("	.globl	FUDGE_SP");
asm("	.align	4");
asm("FUDGE_SP:");
asm("	save &0");
asm("	movw	-48(%fp),%r1");		/* r1 = caller's ap */
asm("	movw	0(%ap),%r0");		/* r0 = 1st arg (de_ap) */
asm("	movw	0(%r1),0(%r0)");	/* *r0 = caller's 1st arg */
asm("	movw	0(%ap),-48(%fp)");	/* overwrite saved ap */
asm("	ret &0");

#else	/* BELLMAC_CHIP */


asm("	.text");
asm("	.globl	AP");
asm("	.align	4");
asm("AP:");
asm("	save &0");
asm("	movw	-32(%fp),%r0");  /* r0 = caller's ap */ 
asm("	ret &0");

asm("	.text");
asm("	.globl	FP");
asm("	.align	4");
asm("FP:");
asm("	save &0");
asm("	movw	-28(%fp),%r0");  /* r0 = caller's fp */ 
asm("	ret &0");

/* On BELLMAC_CHIP machines, sp is restored from current ap on return.
 * FUDGE_SP overwrites the saved ap with a new value (the 1st arg), causing
 * the caller's ap to be changed to the new value.
 * As a precaution, the 1st arg of caller is copied down to where new ap
 * will point.  (In case caller references 1st arg (implicit "this" arg)
 * to set return value.)
 * 2nd arg unnecessary on BELLMAC_CHIP machines.
 */ 
asm("	.text");
asm("	.globl	FUDGE_SP");
asm("	.align	4");
asm("FUDGE_SP:");
asm("	save &0");
asm("	movw	-32(%fp),%r1");		/* r1 = caller's ap */
asm("	movw	0(%ap),%r0");		/* r0 = 1st arg (de_ap) */
asm("	movw	0(%r1),0(%r0)");	/* *r0 = caller's 1st arg */
asm("	movw	0(%ap),-32(%fp)");	/* overwrite saved  ap */
asm("	ret &0");
#endif	/* BELLMAC_CHIP */
#endif	/* PROC_3B */


#ifdef	vax
/* save all caller regs in HW_REGS structure */
asm("	.globl _SAVE_REGS");
asm("	.align	1");
asm("_SAVE_REGS:");
asm("	.word	0xfc0 ");	/* entry mask; save all user regs (r11-r6) */
asm("	movl	4(ap),r1 ");		/* ptr to HW_REGS */
asm("	movl	20(fp),0(r1) ");	/* move caller's r6 to arg.r6 */
asm("	movl	24(fp),4(r1) ");	/* move caller's r7 to arg.r7 */
asm("	movl	28(fp),8(r1) ");	/* move caller's r8 to arg.r8 */
asm("	movl	32(fp),12(r1) ");	/* move caller's r9 to arg.r9 */
asm("	movl	36(fp),16(r1) ");	/* move caller's r10 to arg.r10 */
asm("	movl	40(fp),20(r1) ");	/* move caller's r11 to arg.r11 */
asm("	ret");

asm("	.globl _FP");
asm("	.align	1");
asm("_FP:");
asm("	.word	0x0000");	/* entry mask; save no regs */
asm("	movl 12(fp),r0");	/* r0 = caller's fp */
asm("	ret");

/* On the vax, when there are no args, the current ap is the caller's sp */
asm("	.globl _TOP");
asm("	.align	1");
asm("_TOP:");
asm("	.word	0x0000");	/* entry mask; save no regs */
asm("	movl ap,r0");		/* r0 = caller's sp */
asm("	ret");
/* More general (independent of #args) would be:  addl3 $1,(ap),r0 # r0=nargs + 1
 *	ashl $2,r0,r0 # r0 = nargs * 4 (scale to bytes) addl2 ap,r0 #caller's sp
 */

asm("	.globl _set_r6");
asm("	.align	1");
asm("_set_r6:");
asm("	.word	0x0 ");			/*  save no regs  */
asm("	movl	4(ap),r1 ");		/* ptr to register saved in stack */
asm("	movl	(r1),r6 ");
asm("	ret");

asm("	.globl _set_r7");
asm("	.align	1");
asm("_set_r7:");
asm("	.word	0x0 ");			/*  save no regs  */
asm("	movl	4(ap),r1 ");		/* ptr to register saved in stack */
asm("	movl	(r1),r7 ");
asm("	ret");

asm("	.globl _set_r8");
asm("	.align	1");
asm("_set_r8:");
asm("	.word	0x0 ");			/*  save no regs  */
asm("	movl	4(ap),r1 ");		/* ptr to register saved in stack */
asm("	movl	(r1),r8 ");
asm("	ret");

asm("	.globl _set_r9");
asm("	.align	1");
asm("_set_r9:");
asm("	.word	0x0 ");			/*  save no regs  */
asm("	movl	4(ap),r1 ");		/* ptr to register saved in stack */
asm("	movl	(r1),r9 ");
asm("	ret");

asm("	.globl _set_r10");
asm("	.align	1");
asm("_set_r10:");
asm("	.word	0x0 ");			/*  save no regs  */
asm("	movl	4(ap),r1 ");		/* ptr to register saved in stack */
asm("	movl	(r1),r10 ");
asm("	ret");

asm("	.globl _set_r11");
asm("	.align	1");
asm("_set_r11:");
asm("	.word	0x0 ");			/*  save no regs  */
asm("	movl	4(ap),r1 ");		/* ptr to register saved in stack */
asm("	movl	(r1),r11 ");
asm("	ret");

#endif

#ifdef mc68000		/* Really, ifdef sun--stack frame layout
			 * is different on a UNIX PC, for example
			 */
asm("	.text");
asm("	.globl	_SAVE_REGS");
asm("_SAVE_REGS:");
asm("	link	a6,#-40");
asm("	moveml	#0x3cfc,sp@");		/* save d2-d7, a2-a5	*/
asm("	movl	a6@(8),a0");		/* a0 = arg--ptr to HW_REGS */
asm("	movl	a6@(-40),a0@");		/* move caller's d2 to arg.d2 */
asm("	movl	a6@(-36),a0@(4)");	/* move caller's d3 to arg.d3 */
asm("	movl	a6@(-32),a0@(8)");	/* move caller's d4 to arg.d4 */
asm("	movl	a6@(-28),a0@(12)");	/* move caller's d5 to arg.d5 */
asm("	movl	a6@(-24),a0@(16)");	/* move caller's d6 to arg.d6 */
asm("	movl	a6@(-20),a0@(20)");	/* move caller's d7 to arg.d7 */
asm("	movl	a6@(-16),a0@(32)");	/* move caller's a2 to arg.a2 */
asm("	movl	a6@(-12),a0@(36)");	/* move caller's a3 to arg.a3 */
asm("	movl	a6@(-8),a0@(40)");	/* move caller's a4 to arg.a4 */
asm("	movl	a6@(-4),a0@(44)");	/* move caller's a5 to arg.a5 */
asm("	moveml	a6@(-40),#0x3cfc");		/* restore d2-d7, a2-a5	*/
asm("	unlk	a6");
asm("	rts");

/* save_saved_regs copies regs saved in stack frame defined by fp
 * to a HW_REGS structure.  Uses FrameLayout() to find which regs are saved.
 */
void
save_saved_regs(HW_REGS *regsp, int* fp)
{
	FrameLayout layout(fp);
	register int* p = FIRST_SAVED_REG_P(fp,layout.offset);
	register int* rp = (int*)regsp;

	register unsigned short	mask = layout.mask;
	// start with d2, end with a5
	for (register int m = 0x4; m != 0x2000; m <<=1) {
		if (m & mask) {
			*rp++ = *p++;
		} else {
			rp++;
		}
	}
}

/* On the 68k, without a link inst, fp (a6) doesn't move on a call */
asm("	.text");
asm("	.globl	_FP");
asm("_FP:");
asm("	lea	a6@,a0");
asm("	movl	a0,d0");	/* for compilers that return everything in d0*/
asm("	rts");

/* On the 68k, a call (via jsr or bsr) pushes the pc on the stack;
   the caller's sp is the current sp + 1 */
asm("	.text");
asm("	.globl	_TOP");
asm("_TOP:");
asm("	lea	sp@(4),a0");
asm("	movl	a0,d0");	/* for compilers that return everything in d0*/
asm("	rts");

/* On the 68k, immediately before the rts, the sp is pointing at the
 * return pc on the stack.  The rts pops the pc off the stack.
 * To reset the sp correctly after fudging the stack, we'll save the
 * pointer to "skip's" return-pc in Skip_pc_p, and fudge_return() will
 * overwrite task::task's return pc with the address of fudge_sp,
 * a function that will reset the sp to Skip_pc_p, and then do the rts.
 * Ugly, but it should work.
 */
// NOTE:  The following definition of Skip_pc_p appears in fudge.c.68k,
// because of an apparent bug in the Sun-3 compiler.  It seems to get
// confused by intermingling of definitions and asms.
//  int*	Skip_pc_p;	// global to hold fudged return pc.
			// Set in FUDGE_SP, used by fudge_sp a few instructions
			// later.  Be careful not to add intervening insts.
//Note:  1st arg unnecessary on Sun-2/3
void
FUDGE_SP(int*, int* de_fp)
{
	Skip_pc_p = OLD_PC_P(de_fp);
}

asm("	.text");
asm("	.globl	_fudge_sp");
asm("_fudge_sp:");
asm("	movl	_Skip_pc_p,a7");
asm("	rts");

asm("	.text");
asm("	.globl	_set_d2");
asm("_set_d2:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,d2");
asm("	rts");

asm("	.text");
asm("	.globl	_set_d3");
asm("_set_d3:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,d3");
asm("	rts");

asm("	.text");
asm("	.globl	_set_d4");
asm("_set_d4:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,d4");
asm("	rts");

asm("	.text");
asm("	.globl	_set_d5");
asm("_set_d5:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,d5");
asm("	rts");

asm("	.text");
asm("	.globl	_set_d6");
asm("_set_d6:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,d6");
asm("	rts");

asm("	.text");
asm("	.globl	_set_d7");
asm("_set_d7:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,d7");
asm("	rts");

asm("	.text");
asm("	.globl	_set_a2");
asm("_set_a2:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,a2");
asm("	rts");

asm("	.text");
asm("	.globl	_set_a3");
asm("_set_a3:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,a3");
asm("	rts");

asm("	.text");
asm("	.globl	_set_a4");
asm("_set_a4:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,a4");
asm("	rts");

asm("	.text");
asm("	.globl	_set_a5");
asm("_set_a5:");
asm("	movl	sp@(4),a0");	/* arg is ptr to register saved in stack */
asm("	movl	a0@,a5");
asm("	rts");

#endif	/* mc68000 */

#ifdef	i386

asm("	.text");
asm("	.globl SAVE_REGS");
asm("SAVE_REGS:");
asm("	pushl	%ebp");			/* save caller's fp */
asm("	movl	%esp,%ebp");		/* fp = sp */
asm("	pushl	%edi");			/* save edi */
asm("	pushl	%esi");			/* save esi */
asm("	pushl	%ebx");			/* save ebx */
asm("	movl	8(%ebp),%eax");		/* eax = arg--ptr to HW_REGS */
asm("	movl	-4(%ebp),%ecx");	/* ecx = caller's edi */
asm("	movl	%ecx,0(%eax)");		/* arg.edi = ecx */
asm("	movl	-8(%ebp),%ecx");	/* ecx = caller's esi */
asm("	movl	%ecx,4(%eax)");		/* arg.esi = ecx */
asm("	movl	-12(%ebp),%ecx");	/* ecx = caller's ebx */
asm("	movl	%ecx,8(%eax)");		/* arg.ebx = ecx */
asm("	popl	%ebx");
asm("	popl	%esi");
asm("	popl	%edi");
asm("	leave");
asm("	ret");

/* save_saved_regs copies regs saved in stack frame defined by fp
 * to a HW_REGS structure.  Uses FrameLayout() to find which regs are saved.
 */
void
save_saved_regs(HW_REGS *regsp, int* fp)
{
	FrameLayout layout(fp);
	register int* p = FIRST_SAVED_REG_P(fp,layout.offset);
	register int* rp = (int*)regsp;

	register unsigned short	mask = layout.mask;
	for (register short m = 1; m != 0x08; m <<=1) {
		if (m & mask) {
			*rp++ = *p--;
		} else {
			rp++;
		}
	}
}

/* On the i386, without a specific inst, fp (ebp) doesn't move on a call */
asm(".text");
asm(".globl	FP");
asm("FP:");
asm("	movl	%ebp,%eax");
asm("	ret");

/* On the i386, a call pushes the pc on the stack;
   the caller's sp is the current sp + 1 */
asm(".text");
asm(".globl	TOP");
asm("TOP:");
asm("	leal	4(%esp),%eax");
asm("	ret");


/* On the i386, immediately before the ret, the sp is pointing at the
 * return pc on the stack.  The ret pops the pc off the stack.
 * To reset the sp correctly after fudging the stack, we'll save the
 * pointer to "skip's" return-pc in Skip_pc_p, and fudge_return() will
 * overwrite task::task's return pc with the address of fudge_sp,
 * a function that will reset the sp to Skip_pc_p, and then do the ret.
 * Ugly, but it should work.
 */
// NOTE:  The following definition of Skip_pc_p appears in fudge.c.386,
// because of an apparent bug in the Sun-3 compiler.  It seems to get
// confused by intermingling of definitions and asms.
// int*	Skip_pc_p;	// global to hold fudged return pc.
			// Set in FUDGE_SP, used by fudge_sp a few instructions
			// later.  Be careful not to add intervening insts.
//Note:  1st arg unnecessary on i386
void
FUDGE_SP(int*, int* de_fp)
{
	Skip_pc_p = OLD_PC_P(de_fp);
}

asm("	.text");
asm("	.globl	fudge_sp");
asm("fudge_sp:");
asm("	movl	Skip_pc_p,%esp");
asm("	ret");

asm(".text");
asm(".globl	set_edi");
asm("set_edi:");
asm("	movl	4(%esp),%eax");
asm("	movl	0(%eax),%edi");
asm("	ret");

asm(".text");
asm(".globl	set_esi");
asm("set_esi:");
asm("	movl	4(%esp),%eax");
asm("	movl	0(%eax),%esi");
asm("	ret");

asm(".text");
asm(".globl	set_ebx");
asm("set_ebx:");
asm("	movl	4(%esp),%eax");
asm("	movl	0(%eax),%ebx");
asm("	ret");

#endif	/* i386 */

#ifdef uts


/*
 * FP() and AP() return the frame/arg pointer of the caller. In both
 * cases, this is the old value of r12.
 *
 * uts allows asm only inside functions.
 * Ordinarily we could simply load r0 and return, but because we're
 * in a C function, we have to make the compiler happy. A value is
 * returned through a redundant auto, which is assigned a dummy value 
 * to make the compiler shut up about "used but not set."
 * (Assignment is used instead  of initialization, due to a cfront quirk
 * which will delay initialization until right before use, overwriting
 * the asm work with 0).
 *
 */

extern "C" {

int *
FP()
{
	int	*fp;
	fp = 0;

	/*
	 * return the saved r12
	 */
	asm("	l	0, 48(12)");
	asm("	st	0, -4+$FP_len(13)");
	return fp;
}

int *
AP()
{
	int	*ap;
	ap = 0;

	/*
	 * return the saved r12
	 */
	asm("	l	0, 48(12)");
	asm("	st	0, -4+$AP_len(13)");
	return ap;
}

int *
TOP()
{
	int	*sp;
	sp = 0;

	/*
	 * return the saved r13
	 */

	asm("	l	0, 52(12)");
	asm("	st	0, -4+$TOP_len(13)");
	return sp;
}

void
set_r2(int*)
{
	asm("	l	2, 64(12)");		// get loc
	asm("	l	2, 0(2)");		// load r2 with contents
	asm("	st	2, 8(12)");		// overwrite save area
};

void
set_r3(int*)
{
	asm("	l	3, 64(12)");
	asm("	l	3, 0(3)");
	asm("	st	3, 12(12)");
}

void
set_r4(int*)
{
	asm("	l	4, 64(12)");
	asm("	l	4, 0(4)");
	asm("	st	4, 16(12)");
}

void
set_r5(int*)
{
	asm("	l	5, 64(12)");
	asm("	l	5, 0(5)");
	asm("	st	5, 20(12)");
}
void
set_r6(int*)
{
	asm("	l	6, 64(12)");
	asm("	l	6, 0(6)");
	asm("	st	6, 24(12)");
}

void
set_r7(int*)
{
	asm("	l	7, 64(12)");
	asm("	l	7, 0(7)");
	asm("	st	7, 28(12)");
};

void
set_r8(int*)
{
	asm("	l	8, 64(12)");
	asm("	l	8, 0(8)");
	asm("	st	8, 32(12)");
}

void
set_r9(int*)
{
	asm("	l	9, 64(12)");
	asm("	l	9, 0(9)");
	asm("	st	9, 36(12)");
}

void
set_r10(int*)
{
	asm("	l	10, 64(12)");
	asm("	l	10, 0(10)");
	asm("	st	10, 40(12)");
}

};

/*
 * saved_saved_regs copies regs saved in stack frame defined by fp
 * to a HW_REGS structure. Uses FrameLayout() to find which regs are saved.
 */
void
save_saved_regs(HW_REGS *regsp, int* fp)
{
	FrameLayout layout(fp);
	register int* p = FIRST_SAVED_REG_P(fp);
	register int* rp = (int*)regsp;
	register int r;

	for (r = 0; r < 16; r++, rp++, p++) {
		if (layout.begreg <= r && r <= layout.endreg) {
			*rp = *p;
		}
	}
}



#endif
