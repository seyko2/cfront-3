/*ident	"@(#)cls4:lib/task/task/task.c	1.8" */
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
#include <assert.h>

task* task::txsk_chxin = 0;
team* task::team_to_delete = 0;

#define ABSV(x) ( (x) > 0 ? (x) : -(x) )

const int NEW_CHILD = 1;

int _hwm;

HW_REGS New_task_regs;   /* hardware regs for activating new child
			  * in task::task frame
			  */

// a team is a collection of tasks that share one stack
class team
{
friend class task;
	int	no_of_tasks;
	task*	got_stack;
	int*	stack;
	int	size;	// of the stack
	team(task*, int =0);	// stacksize == zero ==> using the main stack
	~team() { delete stack; }
};

team::team(task* t, int stacksize) {
	no_of_tasks = 1;
	got_stack = t;
	if (size = stacksize) {
		stack = new int[stacksize];
		while (stack == 0) object::task_error(E_STORE, (object*)0);
		if (_hwm) {
			for (int x = 0; x < stacksize; x++) {
				stack[x] = UNTOUCHED;
			}
		}
	}
}


static void
usemainstack()
/* fudge to allow simple stack overflow check */
{
	int* sp = TOP();

	if (_hwm) {
		/* WARNING:  This function used to declare an array of
		 * SIZE+100 and set each element to UNTOUCHED instead
		 * of using the following method.  But then when compilers
		 * inlined this function it caused each new task's stack
		 * to overflow on initialization, because the task::task
		 * stack frame was SIZE+100 larger.
		 */

		// start with next word after "sp"
		for (int i = 2; i < SIZE+100; i++) {
			*(STACK_LAST_WORD_P(sp, i)) = UNTOUCHED;
		}
	} else {
#ifndef i386
		*(STACK_LAST_WORD_P(sp, 0)) = 0;
#endif
	}
}

void 
copy_stack(register int* f, register int c, register int* t)
/*
	copy c words down from f to t
	do NOT attempt to copy "copy_stack"'s own stackframe
*/
{
	COPY_STACK(f,c,t);
}

/* swap_stack copies contents of parent stack (starting at pa_fp)
 * to child stack.  Adjusts pointers in saved ta_fp frame of child stack 
 * (to point to places in child stack).
 * Returns distance from parent stack to child stack.
 */
int
task::swap_stack(int* ta_fp, int* pa_fp)
{
	int size = ACTIVE_STK_SZ(pa_fp, TOP());
	if (size > t_stacksize) {
		task_error(E_STACK, this);
	}
	copy_stack(pa_fp, size, t_basep);
	/* distance from old stack to new */
	register int distance = pa_fp - t_basep;
	/* now doctor the new frame */
#if defined(vax) || defined(PROC_3B)
	OLD_AP(ta_fp - distance) = int((int*)OLD_AP(ta_fp) - distance);
#endif
	OLD_FP(ta_fp - distance) = int((int*)OLD_FP(ta_fp) - distance);
	return distance;
}
inline
void
task::settrap()
{
	if (t_team->size) {	// Don't set trap for main task
		t_trap = *(STACK_LAST_WORD_P(
				STACK_BASE(t_team->stack, t_stacksize),
				t_team->size));
	}
}

inline
void
task::checktrap()
{
	if (t_team->size	// Don't test for main task
	    && t_trap != *(STACK_LAST_WORD_P(
				STACK_BASE(t_team->stack, t_stacksize),
				t_team->size))) {
		task_error(E_STACK, this);
	}

}

extern "C" {
	/* swap and sswap are assembly language functions */
extern task* swap(task*, task*, int, int);
extern task* sswap(task*, task*, task*, int, int);
}

inline
void
task::restore(task* running, int is_new_child)
/*
 *	Call assembly function swap or sswap to do a task switch.
 *	Swap suspends "running" task by saving current hardware state (fp, etc)
 *	in running->t_framep, etc.,
 *	and makes "this" task run after suspension by returning from the frame
 *	denoted by "t_framep."
 *	
 *	swap does a normal return--must be the last thing called here.
 *	swap for a new child task will not return through restore,
 *	but will return from task::task().
 *
 *	sswap saves running's hw state, copies out the target stack,
 *	copies in to_run's stack from the save area before "swap"ing
 *	It is equivalent to two "copystack"s in the middle of "swap".
 *	sswap for a new child copies out the parent stack, but does not need
 *	to restore hw_state of child--it's already in place, and 
 *	WILL return through restore, unlike swap.
 */
{
	task* prevOnStack;
	// running might have been TERMINATED
	int is_terminated = 0;
	if ((running == 0) || (running->s_state == TERMINATED)) {
		is_terminated = 1;
	} else {
		running->checktrap();
	}

	if ((t_mode == SHARED) && this != (prevOnStack = t_team->got_stack)){
		t_team->got_stack = this;    // assuming sswap will get space.
		sswap(running, prevOnStack, this, is_new_child, is_terminated);
	}
	else  {	// DEDICATED or (SHARED && this already on target stack)
		swap(running, this, is_new_child, is_terminated);
	}
	//sswap and swap for old task return here.
	// This code is not needed for a new child, because a terminated
	// parent cannot create a new child.
	if (team_to_delete) {
		delete team_to_delete;
		team_to_delete = 0;
	}
	// WARNING!  No new code here.
}

task::task(char* name, modetype mode, int stacksize)
/*
	executed in the task creating a new task - thistask.
	1:	put thistask at head of scheduler queue,
	2:	create new task
	3:	transfer execution to new task
	derived::derived can never return - its return link is destroyed

	if thistask==0 then we are executing on main()'s stack and
	should turn it into the "main" task
*/
{
	register task* running;

	register int* ta_fp = (int*)FP();	    // fp for task::task()
	register int* ta_ap = (int*)AP();	    // ap for task::task()
	register int* de_fp = (int*)OLD_FP(ta_fp);  // fp for ctor of class 
						    // derived from class task
	register int* de_ap = (int*)OLD_AP(ta_fp);
	register int* pa_fp = (int*)OLD_FP(de_fp);  // parent fp
						    // (caller of derived ctor)
	t_name = name;
	t_mode = (mode) ? mode : (modetype) DEFAULT_MODE;
	t_stacksize = (stacksize) ? stacksize : SIZE;
	t_alert = 0;
	s_state = RUNNING;
	t_next = txsk_chxin;
	txsk_chxin = this;
	th = this;	/* fudged return value -- "returned" from swap */

	switch ((int)thxstxsk) {
	case 0:
		/* initialize task system by creating "main" task */
		thxstxsk = (task*) 1;
		thxstxsk = new task("main");
		break;
	case 1:
		/*	create "main" task	*/
		usemainstack();		/* ensure that store is allocated */
		//set base pointer assuming a static task (Interrupt_alerter)
		//at this point stack has 6 frames for:
		//main, _main, <static ctor>, Int::Int, task::task, task::task
		//NOTE:  This sets basep to be the fp saved in _main's stack
		//frame (fp for main).  Depending on layout, may not include
		//main's save area.  This shouldn't matter.
#ifdef i386
		t_basep = (int*)OLD_FP((int*)OLD_FP(pa_fp));
#else
		t_basep = (int*)OLD_FP((int*)OLD_FP((int*)OLD_FP(pa_fp)));
#endif
		t_team = new team(this);	/* don't allocate stack */
		t_team->no_of_tasks = 2;   	/* never deallocate */
		return;
	}
	// thxstxsk is parent task
	/* return pointer to "child" */
	thxstxsk->th = this;
	thxstxsk->insert(0,this);

	switch (t_mode) {
	case DEDICATED:
		{
		t_team = new team(this,t_stacksize);
		t_basep = STACK_BASE(t_team->stack, t_stacksize);

		// initialize child's stack
		int distance = swap_stack(ta_fp, pa_fp);
		// save hardware state of this frame in "this" (child)
		t_framep = ta_fp - distance;
		t_ap = ta_ap - distance;
		settrap();

		// save all current and saved hw regs in New_task_regs,
		// so when child begins execution, it will have hw regs
		// as the derived constructor had them set.
		SAVE_CHILD_REGS(&New_task_regs);

		thxstxsk->fudge_return(ta_fp);
		running = thxstxsk;	// running = parent
		thxstxsk = this;
		restore(running, NEW_CHILD);	// no return for child; 
						// parent will return
		// Needed on some machines to reset sp on fudged stack
		FUDGE_SP(de_ap, de_fp);  // Can't access arguments on 3B now
					 // (except arg1, implicit "this")
		return;		// On 68k, return through fudge_sp()
		}
	case SHARED:
		thxstxsk->t_mode = SHARED; /* you cannot share on your own */
		t_basep = pa_fp;
		t_team = thxstxsk->t_team;
		t_team->no_of_tasks++;
		t_framep = ta_fp;
		t_ap = ta_ap;
		settrap();
		running = thxstxsk;	// running == parent
		thxstxsk = this;
		restore(running, NEW_CHILD);	// both parent & child will
						// return
		if (running == thxstxsk) {	// parent
			running->fudge_return(ta_fp);
			// Needed on some machines to reset sp on fudged stack
			FUDGE_SP(de_ap, de_fp);  // Can't access arguments
						 // on 3B now (except arg1,
					 	 // implicit "this")
			// On 68k, parent returns through fudge_sp()
		}
		return;
	default:
		task_error(E_TASKMODE, this);
	}
}

void
task::resume()
{
	task* running = thxstxsk;
	thxstxsk = this;
	restore(running);
}

void
task::cancel(int val)
/*
	TERMINATE and free stack space
*/
{
	if (this->s_state != TERMINATED) {
		sched::cancel(val);
		if (_hwm) t_size = curr_hwm();
		if (t_team && (t_team->no_of_tasks-- == 1)) {
			if (this != thxstxsk) {
				delete t_team;
			} else {	// don't delete current stack!
				// delete will be called from task::restore
				// immediately after task switch
				assert(team_to_delete == 0);
				team_to_delete = t_team;
			}
			t_team = 0;	// no further access to deleted team
		}
	}
	}

task::~task()
/*
	free stack space and remove task from task chain
*/
{
	if (s_state != TERMINATED) task_error(E_TASKDEL, this);
	if (this == txsk_chxin)
		txsk_chxin = t_next;
	else {
		register task* t;
		register task* tt;

		for (t=txsk_chxin; tt=t->t_next; t=tt)  
			if (tt == this) {
				t->t_next = t_next;
				break;
			}
	}

	if (this == thxstxsk) {
		delete (int*) thxstxsk;	/* fudge: free(_that) */
		thxstxsk = 0;
		schedule();
	}
}

void
task::resultis(int val)
{
	cancel(val);
	if (this == thxstxsk) schedule();
}

void
task::sleep(object* t)
{
	if (t) t->remember(this);
	if (s_state == RUNNING) remove();
	if (this == thxstxsk) schedule();
}

void
task::delay(long d)
{
	insert(d,this);
	if (thxstxsk == this) schedule();
}

long
task::preempt()
{
	if (s_state == RUNNING) {
		remove();
		return s_time - get_clock();
	}
	else {
		task_error(E_TASKPRE, this);
		return 0;
	}
}

char*
state_string(sched::statetype s)
{
	switch (s) {
	case sched::IDLE:		return "IDLE";
	case sched::TERMINATED:	return "TERMINATED";
	case sched::RUNNING:		return "RUNNING";
	default:		return 0;
	}
}

char*
mode_string(task::modetype m)
{
	switch(m) {
	case task::SHARED:	return "SHARED";
	case task::DEDICATED:	return "DEDICATED";
	default:		return 0;
	}
}

void
task::print(int n, int baseClass)
/*
	"n" values:	CHAIN, VERBOSE, STACK
*/
{
	if (!baseClass)
		printf("task\n");

	char* ss = state_string(s_state);
	char* ns = (t_name) ? t_name : "";
	
	printf("task %s ",ns);
	if (this == thxstxsk)
		printf("(is thistask, %s) ", ss);
	else if (ss)
		printf("(%s) ",ss);
	else
		printf("(state==%d CORRUPTED) ",s_state);
	printf("\tthis = %x:\n", this);

	if (n&VERBOSE) {
		char* ms = mode_string(t_mode);
		if (ms == 0) ms = "CORRUPTED";
		printf("\tmode=%s t_alert=%x t_next=%x",
			ms, t_alert, t_next);
		printf((s_state==TERMINATED) ? " result=%d\n" : " s_time=%d\n", s_time);
	}

	if (n&STACK) {
		printf("\tstack: ");
		if (s_state == TERMINATED) {
			printf("deleted.  ");
			if (_hwm) {
				printf("hwm size=%d, ", t_size);
				printf("hwm address=%x",STACK_LAST_WORD_P(t_basep,t_size));
			}
			printf("\n");
		}
		else {
			register int* b = t_basep;
			printf("\tsizes:\t");
			register int sz;
			if (this==thxstxsk) { // figure out real current size
				sz = ACTIVE_STK_SZ(b,TOP());
			} else {	// approximate at last switch
				sz = t_mode==DEDICATED ?
					ACTIVE_STK_SZ(b,t_framep) : t_size;
			}
			printf("max=%d, current=%d",t_stacksize, sz);
			if (_hwm) printf(", hwm=%d",curr_hwm());
			printf("\n\t\taddresses:\t");
			printf("t_basep=%x, t_framep=%x\n",b,t_framep);
			printf("\t\t\t\tmax=%x",
				STACK_LAST_WORD_P(b,t_stacksize));
			printf(", current=%x", STACK_LAST_WORD_P(b,sz));
			if (_hwm) printf(", hwm=%x",
				STACK_LAST_WORD_P(b, curr_hwm()));
			printf("\n");
		}
	}

	if (n&CHAIN) {
		sched::print(n, 1);	// call sched::print here to keep
					// output for same object together
		// Start at beginning of task chain, and print all tasks
		task *tp = get_task_chain();
		if (tp == this) {
			tp = tp->t_next;	// just printed, skip it
		} else {
			printf("\nChain of all tasks:\n");
		}
		for (; tp; tp = tp->t_next) {
			printf("Next task on chain of all tasks is:\n");
			tp->print(n & ~CHAIN);
		}
	} else {
		sched::print(n, 1);
	}
}

int
task::curr_hwm()
{
	int* b = t_basep;
	int i;
	for (i=t_team->size; 0<i && *(STACK_LAST_WORD_P(b,i))==UNTOUCHED; i--) ;
	return i;
}

void
task::wait(object* ob)
{
	if (ob == (object*)this) task_error(E_WAIT, this);
	t_alert = ob;
	while (ob->pending())
		sleep(ob);
}

int
task::waitlist(object* a ...)
{
	return waitvec(&a);
}

int
task::waitvec(object** v)
/*
	first determine if it is necessary to sleep(),
	return hint: who caused return
*/
{
	int i;
	int j;
	register object* ob;

	for(;;) {
		for (i = 0; ob = v[i]; i++) {
			if (!ob->pending()) goto ex;
			ob->remember(this);
		}
		if (i==1 && v[0]==(object*)this) task_error(E_WAIT, this);
		sleep();
	}
ex:
	t_alert = ob;
	for (j = 0; ob = v[j]; j++)
		ob->forget(this);
	return i;
} 

Interrupt_alerter	interrupt_alerter;
