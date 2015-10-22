/	.file "swap.s.386"
/	ident	"@(#)cls4:lib/task/task/swap.s.386	1.4"
/ ##############################################################################
/ 
/ C++ source for the C++ Language System, Release 3.0.  This product
/ is a new release of the original cfront developed in the computer
/ science research center of AT&T Bell Laboratories.
/
/ Copyright (c) 1993  UNIX System Laboratories, Inc.
/ Copyright (c) 1991, 1992   AT&T and UNIX System Laboratories, Inc.
/ Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.
/
/ 
/ THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
/ 	
/ Laboratories, Inc.  The copyright notice above does not evidence
/ any actual or intended publication of such source code.
/
/ 
/ ##############################################################################
/
/	swap of DEDICATED
/ call swap(*running_task, *to_run_task, is_new_child, running_is_terminated)
/ This routine saves the fp in running's t_framep.
/ If running is a SHARED task, we must save its stack size as well,
/ although the stack does not need to be copied out here.
/ It then restores to_run's t_framep to be the current fp.
/ If to_run is a new child, it explicitly restores the registers from
/ New_task_regs, and returns without restoring the regs saved in the stack.
/ If to_run is not a new child, it restores all the registers saved in
/ the frame on returning.
/ If running_task is TERMINATED, then we don't need to do a save.
/ NOTE:  assumes all functions return values in eax.

	.text
	.globl	swap
swap:
	pushl	%ebp			/ save caller's fp 
	movl	%esp,%ebp		/ fp = sp 
	pushl	%edi			/ save all user regs: edi, esi, ebx
	pushl	%esi
	pushl	%ebx
	movl	12(%ebp),%edx		/ edx = to_run
	movl	20(%ebp),%eax		/ eax = running_is_terminated
	cmpl	$1,%eax			/ if eax == 1
	je	.L_RESTORE		/ 	skip the save
	movl	8(%ebp),%eax		/ eax = running

	/ save state of running task
	movl	%ebp,20(%eax)		/ running->t_framep = fp (ebp)
	movl	52(%eax),%ecx		/ %ecx = running->t_mode
	cmpl	$2,%ecx			/ if running-t_mode == SHARED
	jne	.L_RESTORE
	/ the code here to save the t_size is the same as for sswap
	movl	32(%eax),%ecx		/ ecx = running->t_basep
	subl	%esp,%ecx		/ ecx = running->t_basep - sp
	addl	$4,%ecx			/ (size in bytes)
	shrl	$2,%ecx			/ ecx /= 4 (size in ints)
	movl	%ecx,36(%eax)		/ running->t_size = ecx

.L_RESTORE:
	movl	16(%ebp),%ecx		/ ecx = is_new_child

	/ restore state of to_run task
	movl	20(%edx),%ebp		/ ebp (fp) = to_run->t_framep

	/ if is_new_child, restore registers
	jcxz	.L_RET			/ if ecx==0; go to .L_RET
	/ new child task effectively returns from task::task, so we need
	/ to set the return value to "this"
	movl	24(%edx),%eax		/ eax = to_run->th
	leal	New_task_regs,%edx	/ edx = address of New_task_regs
	movl	0(%edx),%edi
	movl	4(%edx),%esi
	movl	8(%edx),%ebx
	/ Note: leave resets sp relative to fp, so don't need to reset sp
	/ for NEW_CHILD case.
	leave
	ret
.L_RET:
	/ The i386 restores registers relative to sp, so we need to reset
	/ to-runs's sp.  We add 3 words (n saved regs) to the fp to get the sp.
	movl	%ebp,%edx
	sub	$12,%edx
	movl	%edx,%esp
	/ restore all user regs: edi, esi, ebx
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret


/	swap of SHARED
/ sswap(*running, *prevOnStack, *to_run, is_new_child, running_is_terminated)
/ This routine saves the fp in running's t_framep and the stack size in t_size.
/ Then it copies out the target stack to prevOnStack's t_savearea.
/ If to_run is not a new child, it then copies the saved stack of to_run 
/ (from t_savearea) to the target stack, and then restores to_run's t_framep 
/ to be the current fp.  We don't need to restore state of a child 
/ to_run object, because it's already in place.
/ If running_task is TERMINATED, then we don't need to do a save,
/ NOTE:  assumes all functions return values in eax.
/ and if running_task is TERMINATED and equals prevOnStack, then we don't
/ have to do the stack copy.
	.text
	.globl	sswap
sswap:
	pushl	%ebp			/ save caller's fp 
	movl	%esp,%ebp		/ fp = sp 
	pushl	%edi			/ save all user regs: edi, esi, ebx
	pushl	%esi
	pushl	%ebx

	movl	8(%ebp),%eax		/ eax = running
	movl	12(%ebp),%edi		/ edi = prevOnStack
	movl	24(%ebp),%ecx		/ ecx = running_is_terminated
	cmpl	$1,%ecx			/ if ecx == 1
	je	.L_SKIP			/ 	skip the save
	/save hw state of running
	movl	%ebp,20(%eax)		/ running->t_framep = fp (ebp)
	movl	32(%eax),%ecx		/ ecx = running->t_basep
	subl	%esp,%ecx		/ ecx = running->t_basep - sp
	addl	$4,%ecx			/ (size in bytes)
	shrl	$2,%ecx			/ ecx /= 4 (size in ints)
	movl	%ecx,36(%eax)		/ running->t_size = ecx
	jmp	.L_SAVE

.L_SKIP:	/if running is TERMINATED and running == prevOnStack,
		/then we can skip the stack copy too
	cmpl	%eax,%edi		/ if running == prevOnStack
	je	.L_REST			/	skip save

.L_SAVE:	/copy out target stack to prevOnStack->t_savearea
	movl	36(%edi),%ecx		/ ecx = prevOnStack->t_size (count)
	pushl	%ecx			/ push count arg on stack
	call	swap_call_new		/ get count bytes of storage
	popl	%ecx			/ pop arg off stack
	sall	$2,%ecx			/ scale ecx to bytes
	addl	%ecx,%eax		/ eax = base of new stack, plus 1 long
	subl	$4,%eax			/ eax = base of new stack (to)
	movl	%eax,40(%edi)		/ prevOnStack->t_savearea = eax (to)
	movl	32(%edi),%esi		/ esi = prevOnStack->t_basep (from)
	movl	%eax,%edi		/ edi = to
	shrl	$2,%ecx			/ ecx /= 4 = size in ints (count)
	pushf				/ save old DF
	std				/ set DF for copy from hi to lo addr
	rep; smovl			/ copy words 
	popf				/ retore DF

.L_REST:
	movl	20(%ebp),%eax		/ eax = is_new_child
	testl	%eax,%eax		/ if is_new_child != 0
	jne	.L6			/	skip the copy-in loop

	/copy into target stack from to_run->t_savearea
	movl	16(%ebp),%ebx		/ ebx = to_run
	movl	32(%ebx),%eax		/ eax = to_run->t_basep (to)
	movl	36(%ebx),%ecx		/ ecx = to_run->t_size (count)
	/ Kick up the sp if new stack will be taller than current.
	sall	$2,%ecx			/ ecx = new stack height in bytes
	subl	%ecx,%eax		/ eax = base - size = target sp (+ 1)
	subl	%esp,%eax		/ eax = eax - sp
	testl	%eax,%eax
	jge	.L3			/ if eax < 0
	addl	%eax,%esp		/	kick up sp
.L3:
	movl	32(%ebx),%edi		/ edi = to_run->t_basep (to)
	movl	36(%ebx),%ecx		/ ecx = to_run->t_size (count)
	movl	40(%ebx),%esi		/ esi = to_run->t_savearea (from)
	pushf				/ save old DF
	std				/ set DF for copy from hi to lo addr
	rep; smovl			/ copy words 
	popf				/ retore DF

	/ restore state of to_run
	movl	20(%ebx),%ebp		/ fp (a6) = to_run->t_framep
	/ to (edi) points to one word beyond new sp
	addl	$4,%edi
	movl	%edi,%esp		/ reset sp, so regs can be restored
	/ finally, delete to_run's t_savearea
	movl	36(%ebx),%ecx		/ ecx = to_run->t_size
	sall	$2,%ecx			/ scale size to bytes
	movl	40(%ebx),%eax		/ eax = to_run->t_savearea
	subl	%ecx,%eax		/ get low address of savearea
	addl	$4,%eax
	pushl	%eax			/ push pointer to savearea on stack
	call	swap_call_delete	/ delete to_run->t_savearea
	popl	%eax			/ pop arg off stack
	movl	$0,40(%ebx)		/ to_run->t_savearea = 0
/ sp should be correct here, for both is and is not new child cases
.L6:	/ restore all user regs: edi, esi, ebx
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
