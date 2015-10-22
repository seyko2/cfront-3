/*ident	"@(#)cls4:lib/task/task/fudge.c.386	1.3" */
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
#include <task.h>
#include "hw_stack.h"

// Intel 386 frame fudger

/* careful -- stack frame not self-describing */
// STACK GROWS DOWN

// We define a mask in which FrameLayout can record which regs are saved.
// We only care about the non-scratch regs edi, esi, ebx.
const short REG_EDI	= 0x1;
const short REG_ESI	= 0x2;
const short REG_EBX	= 0x4;
#define	ALL_REGS_SAVED(m)	((short)(m) == 0x0007)

const int		OPCODE_SIZE	= 1;
const MACHINE_BYTE	CALL_INSTR	= 0xe8;
const int		CALL_SIZE	= 5;
const MACHINE_BYTE	JMP_SHORT	= 0xeb;
const MACHINE_BYTE	JMP_DISPL	= 0xe9;
// assumes we don't need to worry about jump near indirect (0xff)

#define IS_JMP_INSTR(instr)					\
	((MACHINE_BYTE)(instr) == (MACHINE_BYTE)JMP_SHORT ||	\
	 (MACHINE_BYTE)(instr) == (MACHINE_BYTE)JMP_DISPL)
const MACHINE_BYTE	PUSH_EBP_INSTR = 0x55;
const int		PUSH_SIZE = 1;
const MACHINE_BYTE	MOV_INSTR = 0x8b;
const int		MOV_SIZE = 2;
const MACHINE_BYTE	SUB_INSTR = 0x83;
const int		SUB_SIZE = 3;
const MACHINE_BYTE	PUSH_EDI_INSTR = 0x57;
const MACHINE_BYTE	PUSH_ESI_INSTR = 0x56;
const MACHINE_BYTE	PUSH_EBX_INSTR = 0x53;
#define	IS_PUSH_REG_INSTR(instp)		\
	((*((MACHINE_BYTE*)(instp)) & 0xf0) == 0x50)
#define IS_SET_EBP_INSTR(instp)			\
	(((*(instp)) == MOV_INSTR) && (*((MACHINE_BYTE*)(instp) + 1) == 0xec))
#define	IS_SUB_ESP_INSTR(instp)				\
	((*((MACHINE_BYTE*)(instp)) == SUB_INSTR)	\
	&& (*((MACHINE_BYTE*)(instp) + 1) == 0xec))


int*	Skip_pc_p;	// global to hold fudged return pc.
			// See comments in hw_stack.c.

/* Given a pointer to a call instruction, yield a pointer to
 * the called function */
unsigned int
call_dst_ptr(MACHINE_BYTE* callp)
{
	/*
	 * On the Intel 386, the call instruction is: call displ
	 * where the displacement is always relative to the pc.
	 */
	MACHINE_BYTE* dst_op_p = callp + OPCODE_SIZE;
	int offset = 0;
	// bytes are in reverse order, must flip positions
	offset = dst_op_p[0]       |
		 dst_op_p[1] <<  8 |
		 dst_op_p[2] << 16 |
		 dst_op_p[3] << 24;
	return (unsigned int)(callp + CALL_SIZE + offset);
}

FrameLayout::FrameLayout(int* fp)
{
	mask = 0;
	unsigned int*	return_pc = (unsigned int*)OLD_PC(fp);
	/*
	 * find the starting address of the function.  The idea is that the
	 * instruction immediately before the return address must be the
	 * function call.
	 */
	MACHINE_BYTE*	callp = (MACHINE_BYTE*)return_pc - CALL_SIZE;
	if (*callp != CALL_INSTR)
		object::task_error(E_FUNCS, (object*)0);
	MACHINE_BYTE* func_addr = (MACHINE_BYTE*) call_dst_ptr(callp);

	/* If first instruction is jmp (-O not used), add displacement to pc
	 * to get to function prologue.
	 * Function prologue should be:
	 *	pushl	%ebp
	 *	movl	%esp,%ebp
	 *	subl	XXX,%esp	/ XXX is the offset we want
	 *	pushl	%eXX		/ a push instruction for each saved reg
	 *				/ (sometimes first push is of scratch
	 *				/ reg, and then that is the offset)
	 *	/ stop when no more push instructions, or have saved all regs
	 */
	if (IS_JMP_INSTR(*func_addr)) {
		int displ;
		if (*func_addr == JMP_SHORT) {	// 1 byte displacement
			displ = func_addr[1];
			func_addr += 2;		// next instruction
		} else {	// JMP_DISPL, 4 byte displacement
				// bytes reversed
			displ = func_addr[1]       |
				func_addr[2] <<  8 |
				func_addr[3] << 16 |
				func_addr[4] << 24;
			func_addr += 5;		// next instruction
		}
		func_addr += displ;	// jmp destination
	}
	if ((*func_addr == PUSH_EBP_INSTR)
	    && (IS_SET_EBP_INSTR(func_addr + PUSH_SIZE))) {
		func_addr += MOV_SIZE + PUSH_SIZE;
		if (IS_SUB_ESP_INSTR(func_addr)) {
			offset = func_addr[2] / 4;
			func_addr += SUB_SIZE;
		} else {	// assume no automatics to be allocated
			offset = 0;
		}
	} else {
		object::task_error(E_FUNCS, (object*)0);
	}
	while (IS_PUSH_REG_INSTR(func_addr) && (! ALL_REGS_SAVED(mask))) {
		if (*func_addr == PUSH_EDI_INSTR) {
			mask |= REG_EDI;
			func_addr += PUSH_SIZE;
		} else if (*func_addr == PUSH_ESI_INSTR) {
			mask |= REG_ESI;
			func_addr += PUSH_SIZE;
		} else if (*func_addr == PUSH_EBX_INSTR) {
			mask |= REG_EBX;
			func_addr += PUSH_SIZE;
		} else	{
			// Compiler might push a scratch reg before any non-
			// scratch regs to reserve space for automatics.
			if (mask != 0) {
				// if non-scratch regs already saved,
				// then this is not an offset
				break;
			} else {
				offset += 1;
				func_addr += PUSH_SIZE;
			}
		}

	}	
}

/*
 * Fudge frame of function-defined-by-f_fp (called "f" below)
 * so that that function returns to its grandparent,
 * in particular, so a parent task returns to the function that
 * called the derived constructor, skipping the derived constructor;
 * the child will return to the derived constructor, which is its "main."
 * To do this we will overwrite the old fp and pc (those saved by
 * f) with the old-old ones (those saved by f's caller),
 * and we will overwrite the register save area with registers saved by 
 * f's caller (referred to as "skip" below).
 *
 * There are 2 register-save cases to deal with:
 *     1. skip_n_saved <= f_n_saved
 *     3. skip_n_saved >  f_n_saved
 *
 * These are handled as follows:
 *     1. copy the saved skip_regs over the corresponding f_regs,
 *        leaving any additional saved f_regs intact.
 *        f's epilogue instructions will be correct.
 *     2. f's epilogue instructions will restore too few regs,
 *	  must take special care to see that the extras are restored properly.
 *	  -Copy saved skip_regs over any corresponding f_regs,
 *	  -If fudge_return saved more regs than f did, then
 *	   copy saved extra saved skip_regs over any corresponding fudge_regs,
 *	  -If more extra skip_regs (not saved by either f or fudge_return,
 *	   and therefore not used by either) remain, restore them explicitly.
 *	   They will not be disturbed by the return from fudge_return or f.
 */
void
task::fudge_return(int* f_fp)
{
	register int*	fp = f_fp;		// fp of frame-to-be-fudged
	FrameLayout	lo(fp);			// frame to be fudged
	register int*	skip_fp = (int*)OLD_FP(fp); // fp for f's caller (skip)
	FrameLayout	skip_lo(skip_fp);	// frame for skip
	register int*	fr_fp = FP();		// fp for fudge_return
	FrameLayout	fr_lo(fr_fp);		// frame for fudge_return

	OLD_PC(fp) = (int)&fudge_sp;	// task::task will return through
					// fudge_sp, which will reset the sp
					// to point at the return-pc in
					// skip's frame
	// copy old fp
	OLD_FP(fp) = OLD_FP(skip_fp);

	// now copy saved registers
	// copy any saved skip regs over corresponding f regs; if there isn't
	// a corresponding f reg, copy over corresponding fudge_return reg;
	// if there isn't a corresponding fr_reg, restore it explicitly.
	register int*	to = FIRST_SAVED_REG_P(fp, lo.offset);
	register int*	from = FIRST_SAVED_REG_P(skip_fp, skip_lo.offset);
	register int*	fr_to = FIRST_SAVED_REG_P(fr_fp, fr_lo.offset);
	for (register short m = 1; m != 0x08; m <<=1) {
		if (m & lo.mask) {
			if (m & skip_lo.mask) {
				*to-- = *from--;
				if (m & fr_lo.mask) fr_to--;
			} else {  // nothing to copy
				to--;
				if (m & fr_lo.mask) fr_to--;
			}
		} else if (m & skip_lo.mask) {
			// No slot for *from in f's frame
			if (m & fr_lo.mask) {  // copy to fudge_return's frame
				*fr_to-- = *from--;
			} else {
				// Not used in f or fudge_return;
				// restore explicitly
				switch(m)	{
				case REG_EDI:
					set_edi(from--);
					break;
				case REG_ESI:
					set_esi(from--);
					break;
				case REG_EBX:
					set_ebx(from--);
					break;
				default:
					// Oops--don't expect other regs
					// to be saved
					from--;
					task_error(E_REGMASK, this);
					break;
				}
			}
		}
	}
}

