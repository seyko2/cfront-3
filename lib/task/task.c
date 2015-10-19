/* @(#) task.c 1.2 1/27/86 17:48:10 */
/*ident	"@(#)cfront:lib/task/task.c	1.2"*/
/* @(#) task.c 1.3 1/17/85 14:41:14 */

#include "task.h"

/*	macros giving the addresses of the stack frame pointer
	and the program counter of the caller of the current function
	given the first local variable

	TOP points to the top of the current stack frame
	given the last local variable
*/

#ifdef pdp11

// STACK GROWS DOWN
#define FP(p)		( (int*)(&p+4) )
#define AP()		0	/* unnecessary on pdp11 */
#define OLD_FP(fp)	(*fp)
#define TOP(var9)	(&var9)

#define COPY_STACK(f,c,t)   while (c--) *t-- = *f--
#define ACTIVE_STK_SZ(b,t)  b - t + 1	/* size of active stack */
#define STACK_BASE(b,s)     b + s - 1
#define SAVED_AREA(b,s)     b - s + 1
#define FIX_FRAME(x)\
	OLD_FP(t_framep) = int(de_fp - x)

#define SETTRAP()	t_trap = *(t_basep-t_stacksize+1)
#define CHECKTRAP()	if (t_trap != *(t_basep-t_stacksize+1)) task_error(E_STACK,0)

#endif


#ifdef vax

// STACK GROWS DOWN
#define FP(p)		( (int*)(&p+1) )
#define AP()		0	/* unnecessary on vax */
#define OLD_AP(fp)	(*(fp+2))
#define OLD_FP(fp)	(*(fp+3))
#define TOP(p)		top(&p)
extern int * top(...);

#define COPY_STACK(f,c,t)   while (c--) *t-- = *f--
#define ACTIVE_STK_SZ(b,t)  b - t + 1	/* size of active stack */
#define STACK_BASE(b,s)     b + s - 1
#define SAVED_AREA(b,s)     b - s + 1
#define FIX_FRAME(x)\
	OLD_AP(t_framep) = int(de_ap - x);\
	OLD_FP(t_framep) = int(de_fp - x)

#define SETTRAP()	t_trap = *(t_basep-t_stacksize+1)
#define CHECKTRAP()	if (t_trap != *(t_basep-t_stacksize+1)) task_error(E_STACK,0)

#endif


#if defined(u3b) || defined(u3b5) || defined(u3b2)

// STACK GROWS UP
#define FP(p)		( (int*)&p )
extern int * AP();	/* AP of current frame */
#define TOP(x)		(int*)( (int*)&x + 1 )

#ifdef u3b
asm("	.text");
asm("	.globl	AP");
asm("	.align	4");
asm("AP:");
asm("	save &0");
asm("	movw	-48(%fp),%r0  # caller's ap");
asm("	ret &0");
#define OLD_FP(fp)	( *( (int*)fp - 11 ) )
#define OLD_AP(fp)	( *( (int*)fp - 12 ) )
#else
asm("	.text");
asm("	.globl	AP");
asm("	.align	4");
asm("AP:");
asm("	save &0");
asm("	movw	-32(%fp),%r0  # caller's ap");
asm("	ret &0");
#define OLD_FP(fp)	( *( (int*)fp - 7 ) )
#define OLD_AP(fp)	( *( (int*)fp - 8 ) )
#endif

#define COPY_STACK(f,c,t)   while (c--)  *t++ = *f++
#define ACTIVE_STK_SZ(b,t)  t - b + 1	/* size of active stack */
#define STACK_BASE(b,s)     b
#define SAVED_AREA(b,s)     b
#define FIX_FRAME(x)\
	t_ap = AP() - x;\
	OLD_AP(t_framep) = int(de_ap - x);\
	OLD_FP(t_framep) = int(de_fp - x)

#define SETTRAP()	t_trap = *(t_basep + t_stacksize - 1)
#define CHECKTRAP()	if (t_trap != *(t_basep+t_stacksize-1)) task_error(E_STACK,0)

#endif


// ---- END OF MACROS ----

int _hwm;

class team
{
friend task;
	int	no_of_tasks;
	task*	got_stack;
	int*	stack;
	team(task*, int = 0);
};

team.team(task* t, int stacksize) {
	stacksize += sizeof(team);	// alloc team and stack together
	this = (team*)new int[stacksize];
	while (this == 0) task_error(E_STORE,0);
	stack = (int*)this + sizeof(team);
	no_of_tasks = 1;
	got_stack = t;
}


void usemainstack()
/* fudge to allow simple stack overflow check */
{
	register v[SIZE+100];

	if (_hwm)
		for (register i=0;i<SIZE+100;i++) v[i] = UNTOUCHED;
	else
		v[0] = 0;
}

void copy_stack(register* f, register c, register* t)
/*
	copy c words down from f to t
	do NOT attempt to copy "copy_stack"'s own stackframe
*/
{
	DB(("   copying( %x, %x, %x )\n", f, c, t));
	COPY_STACK(f,c,t);
	DB(("   copied\n"));
}


#define ABSV(x) ( (x) > 0 ? (x) : -(x) )
task.task(char* name, int mode, int stacksize) : (TASK)
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
	int* p;
	int* ta_fp = (int*)FP(p);
	int* de_fp = (int*)OLD_FP(ta_fp);
	int* de_ap;
	de_ap = (int*)OLD_AP(ta_fp);
	int* pa_fp = (int*)OLD_FP(de_fp);
	int x;

	DB(("%x->task::task( %s, %x, %x )\n", this,name,mode,stacksize));
	t_name = name;
	t_mode = (mode) ? mode : DEDICATED;
	t_stacksize = (stacksize) ? stacksize : SIZE;
	t_size = 0;		/* avoid stack copy at initial restore */
	t_alert = 0;
	s_state = RUNNING;
	t_next = task_chain;
	task_chain = this;
	th = this;	/* fudged return value -- "returned" from swap */

	switch ((int)thistask) {
	case 0:
		/* initialize task system by creating "main" task */
		thistask = (task*) 1;
		DB(("   thistask = new task(\"main\")\n"));
		thistask = new task("main");
		break;
	case 1:
		/*	create "main" task	*/
		usemainstack();		/* ensure that store is allocated */
		t_basep = (int*)OLD_FP(pa_fp);	/* fudge, what if main
					   	   is already deeply nested
						*/
		t_team = new team(this);	/* don't allocate stack */
		t_team->no_of_tasks = 2;   	/* never deallocate */
		return;
	}
	thistask->th = this;	/* return pointer to "child" */
	thistask->t_framep = de_fp;  /* return to caller of derived ctor */
	thistask->t_ap = de_ap;
	thistask->insert(0,this);

	switch (t_mode) {
	case DEDICATED:
		t_team = new team(this,t_stacksize);
		t_basep = STACK_BASE(t_team->stack, t_stacksize);
		if (_hwm) for (x=0; x<t_stacksize; x++)
			t_team->stack[x] = UNTOUCHED;
		thistask = this;
		DB(("%x->task::task -- ta_fp = %x\n",this,ta_fp));
		DB(("%x->task::task -- ta_ap = %x\n",this,AP()));
		x = ACTIVE_STK_SZ(pa_fp, TOP(x));
		copy_stack(pa_fp, x, t_basep);
		x = pa_fp - t_basep;	/* distance from old stack to new */
		t_framep = ta_fp - x;	/* fp on new frame */
					/* now doctor the new frame */
		DB(("  OFP(t_framep),de_fp == %x,%x\n",OLD_FP(t_framep),de_fp));
		DB(("  OAP(t_framep),de_ap == %x,%x\n",OLD_AP(t_framep),de_ap));
		FIX_FRAME(x);
		restore();
	case SHARED:
		thistask->t_mode = SHARED; /* you cannot share on your own */
		t_basep = pa_fp;
		t_team = thistask->t_team;
		t_team->no_of_tasks++;
		t_framep = ta_fp;
		t_ap = AP();
		if (mode==0 && stacksize==0)
			t_stacksize = thistask->t_stacksize
					- ABSV(thistask->t_basep - t_basep);
		thistask = this;
		SETTRAP();	// normally done in restore()
		return;
	default:
		task_error(E_TASKMODE,this);
	}
}

void task.save()
/*
	save task's state so that ``restore'' can resume it later
	by returning from the function which called "save"
		- typically the scheduler
*/
{
	int* p = (int*)FP(p);

	DB(("%x->task::save()\n",this));
	t_framep = (int*)OLD_FP(p);
	t_ap = (int*)OLD_AP(p);

	CHECKTRAP();

	if (t_mode == SHARED) {
		register int sz;
		t_size = sz = ACTIVE_STK_SZ(t_basep, TOP(p));
		p = new int[sz];
		while (p == 0) task_error(E_STORE,0);
		t_savearea = STACK_BASE(p, sz);
		copy_stack(t_basep,sz,t_savearea);
	}
}

extern int rr2,rr3,rr4;
int rr2,rr3,rr4;

swap(task*);
sswap(task*);

void task.restore()
/*
	make "this" task run after suspension by returning from the frame
	denoted by "t_framep"

	the key function "swap" is written in assembly code,
	it returns from the function which "save"d the task
		- typically the scheduler

	"sswap" copies the stack back from the save area before "swap"ing
	arguments to "sswap" are passed in rr2,rr3,rr4 to avoid overwriting them
	it is equivallent to "copystack" followed by "swap".
*/
{
	register sz;

	SETTRAP();

	DB(("%x->task::restore()\n",this));
	if ((t_mode == SHARED) && (sz=t_size)){
		register* p = SAVED_AREA(t_savearea, sz); // get ptr for delete
		register x = (this != t_team->got_stack);
		t_team->got_stack = this;
		if (x) {
			DB(("	p		= x%x\n", p));
			DB(("	t_savearea	= x%x\n", t_savearea));
			DB(("	sz		= x%x\n", sz));
			DB(("	t_basep		= x%x\n", t_basep));
			delete p;
			rr4 = (int) t_savearea;
			rr3 = sz;
			rr2 = (int) t_basep;
			sswap(this);
		}
		else
			swap(this);
	}
	else
		swap(this);
}

void task.cancel(int val)
/*
	TERMINATE and free stack space
*/
{
	DB(("%x->task::cancel( %x )\n",this,val));
	sched::cancel(val);
	if (_hwm) t_size = curr_hwm();
	if (t_team->no_of_tasks-- == 1) delete t_team;
}

task.~task()
/*
	free stack space and remove task from task chain
*/
{
	DB(("%x->task::~task()\n",this));
	if (s_state != TERMINATED) task_error(E_TASKDEL,this);
	if (this == task_chain)
		task_chain = t_next;
	else {
		register task* t;
		register task* tt;

		for (t=task_chain; tt=t->t_next; t=tt)  
			if (tt == this) {
				t->t_next = t_next;
				break;
			}
	}

	if (this == thistask) {
		delete (int*) thistask;	/* fudge: free(_that) */
		thistask = 0;
		schedule();
	}
}

void task.resultis(int val)
{
	DB(("%x->task::resultis( %x )\n",this,val));
	cancel(val);
	if (this == thistask) schedule();
}

void task.sleep()
{
	DB(("%x->task::sleep()\n",this));
	if (s_state == RUNNING) remove();
	if (this == thistask) schedule();
}

void task.delay(int d)
{
	DB(("%x->task::delay( %x )\n",this,d));
	insert(d,this);
	if (thistask == this) schedule();
}

int task.preempt()
{
	DB(("%x->task::preempt()\n",this));
	if (s_state == RUNNING) {
		remove();
		return s_time-clock;
	}
	else {
		task_error(E_TASKPRE,this);
		return 0;
	}
}

char* state_string(int s)
{
	switch (s) {
	case IDLE:		return "IDLE";
	case TERMINATED:	return "TERMINATED";
	case RUNNING:		return "RUNNING";
	default:		return 0;
	}
}

char* mode_string(int m)
{
	switch(m) {
	case SHARED:		return "SHARED";
	case DEDICATED:		return "DEDICATED";
	default:		return 0;
	}
}

void task.print(int n)
/*
	``n'' values:	CHAIN,VERBOSE,STACK
*/
{
	char* ss = state_string(s_state);
	char* ns = (t_name) ? t_name : "";
	
	printf("task %s ",ns);
	if (this == thistask)
		printf("(is thistask):\n");
	else if (ss)
		printf("(%s):\n",ss);
	else
		printf("(state==%d CORRUPTED):\n",s_state);

	if (n&VERBOSE) {
		int res = (s_state==TERMINATED) ? (int) s_time : 0;
		char* ms = mode_string(t_mode);
		if (ms == 0) ms = "CORRUPTED";
		printf("\tthis==x%x mode=%s alert=x%x next=x%x result=%d\n",
			this,ms,t_alert,t_next,res);
	}

	if (n&STACK) {
		printf("\tstack: ");
		if (s_state == TERMINATED) {
			if (_hwm) printf("hwm=x%x",t_size);
			printf(" deleted\n");
		}
		else {
			int b = (int) t_basep;
			int x = ((this==thistask) || t_mode==DEDICATED)
				? ACTIVE_STK_SZ( b, (int)t_framep ) : t_size;
			printf("max=%d current=%d",t_stacksize,x);
			if (_hwm) printf(" hwm=x%x",curr_hwm());
			printf(" t_basep=x%x, t_framep=x%x, t_size=x%x\n",
				b,t_framep,t_size);
		}
	}

	if (n&CHAIN) {
		if (t_next) t_next->print(n);
	}
}

int task.curr_hwm()
{
	int* b = t_basep;
	int i;
	for (i=t_stacksize-1; 0<=i && *(b-i)==UNTOUCHED; i--) ;
	return i;
}

int task.waitlist(object* a)
{
	return waitvec(&a);
}

int task.waitvec(object* * v)
/*
	first determine if it is necessary to sleep(),
	return hint: who caused return
*/
{
	int i = 0;
	int r;
	object* ob;

	while (ob = v[i++]) {
		t_alert = ob;
		switch (ob->o_type) {
		case TASK:
		case TIMER:
			if (((sched*)ob)->s_state == TERMINATED) goto ex;
			break;
		case QHEAD:
			if (((qhead*)ob)->rdcount()) goto ex;
			break;
		case QTAIL:
			if (((qtail*)ob)->rdspace()) goto ex;
			break;
		}
		ob->remember(this);
	}
	if (i==2 && v[0]==(object*)thistask) task_error(E_WAIT,0);
	sleep();
ex:
	i = 0;
	while (ob = v[i++]) {
		ob->forget(this);
		if (ob == t_alert) r = i-1;
	}
	return r;
} 
