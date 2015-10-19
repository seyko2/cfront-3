/* @(#) sched.c 1.2 1/27/86 17:48:09 */
/*ident	"@(#)cfront:lib/task/sched.c	1.2"*/
/* @(#) sched.c 1.1 1/17/85 14:49:22 */
#include "task.h"

void setclock(long t)
{
	if (clock) task_error(E_SETCLOCK,0);
	clock = t;
}

int in_error = 0;

int task_error(int n, object* oo)
{
	if (in_error)
		exit(in_error);
	else
		in_error = n;

	if (error_fct) {
		n = (*error_fct)(n,oo);
		if (n) exit(n);
	}
	else {
		print_error(n);
		exit(n);
	}
	in_error = 0;
	return 0;
}

char* error_name[] = {
	"",
	"object.delete(): has chain",
	"object.delete(): on chain",
	"qhead.get(): empty",
	"qhead.putback(): object on other queue",
	"qhead.putback(): full",
	"qtail.put(): object on other queue",
	"qtail.put(): full",
	"set_clock(): clock!=0",
	"schedule(): clock_task not idle",
	"schedule: terminated", 
	"schedule: running",
	"schedule: clock<0",
	"schedule: task or timer on other queue",
	"histogram.new(): bad arguments",
	"task.save(): stack overflow",
	"new: free store exhausted",
	"task.new(): bad mode",
	"task.delete(): not terminated",
	"task.preempt(): not running",
	"timer.delete(): not terminated",
	"schedule: bad time",
	"schedule: bad object",
	"queue.delete(): not empty",
	"thistask->result()",
	"task.wait(thistask)",
};

void print_error(int n)
{

	register i = (n<1 || MAXERR<n) ? 0 : n;

	printf("\n\n***** task_error(%d) %s\n",n,error_name[i]);

	if (thistask) { 
		printf("thistask: ");
		thistask->print(VERBOSE|STACK);
	}
	if (run_chain) {
		printf("run_chain:\n");
		run_chain->print(CHAIN);
	}
} /* task_error */

sched* run_chain = 0;
task* task_chain = 0;
long clock = 0;
task* thistask = 0;
task* clock_task = 0;
PFIO error_fct = 0;
/*PFIO sched_fct = 0;*/
PFV exit_fct = 0;

void sched.cancel(int res)
{
DB(("%x->sched::cancel( %x )\n", this,res));
	if (s_state==RUNNING) remove();
	s_state = TERMINATED;
	s_time = res;
	alert();
}

int sched.result()
/* wait for termination and retrieve result */
{
DB(("%x->sched::result( )\n", this));
DB(("     o_type == x%x\n", o_type));
	if (this == (sched*)thistask) task_error(E_RESULT,0);
	while (s_state != TERMINATED) {
		remember(thistask);
		DB(("     o_type == x%x -- remember(x%x)\n", o_type,thistask));
		thistask->sleep();
		DB(("     o_type == x%x -- x%x->sleep()\n", o_type,thistask));
		forget(thistask);
		DB(("     o_type == x%x -- forget(x%x)\n", o_type,thistask));
	}

	return (int) s_time;
}

void sched.schedule()
/* schedule either clock_task or front of run_chain */
{
DB(("%x->sched::schedule( )\n",this));
	register sched* p;
	register long tt;

lll:
	DB(("	run_chain:  x%x\n", run_chain));
	if (p = run_chain) {
		run_chain = (sched*) p->o_next;
		p->o_next = 0;
	}
	else {
		if (exit_fct) (*exit_fct)();
		exit(0);
	}

	tt = p->s_time;
	DB(("	thistask->s_time, p->s_time, clock == %ld, %ld, %ld\n", thistask->s_time, tt, clock));
	if (tt != clock) {
		if (tt < clock) task_error(E_SCHTIME,this);
		clock = tt;
		if (clock_task) {
			if (clock_task->s_state != IDLE)
				task_error(E_CLOCKIDLE,this);
			/* clock_task preferred */
			p->o_next = (object*) run_chain;
			run_chain = p;
			p = (sched*) clock_task;
		}
	}

	switch (p->o_type) {
	case TIMER: /* time is up; "delete" timer & schedule next task */
		p->s_state = TERMINATED;
		p->alert();
		goto lll;
	case TASK:
		if (p != this) {
			if (thistask && thistask->s_state != TERMINATED)
				thistask->save();
			thistask = (task*) p;
			thistask->restore();
		}
		break;
	default:
		DB(("object(%x) type == %x\n", p, p->o_type));
		task_error(E_SCHOBJ,this);
	}
} /* schedule */

void sched.insert(int d, object* who)
/*
	schedule THIS to run in ``d'' time units
	inserted by who
*/
{
	register sched * p;
	register sched * pp;
	register long tt = s_time = clock + d;

DB(("%x->sched::insert( %x, %x )\n",this,d,who));
	switch (s_state) {
	case TERMINATED:
		task_error(E_RESTERM,this);
		break;
	case IDLE:
		break;
	case RUNNING:
		if (this != (class sched *)thistask) task_error(E_RESRUN,this);
	}

	if (d<0) task_error(E_NEGTIME,this);	

	if (o_next) task_error(E_RESOBJ,this);

	s_state = RUNNING;
	if (o_type == TASK) ((task *) this)->t_alert = who;

	/* run_chain ordered by s_time */
	if (p = run_chain) {
		if (tt < p->s_time) {
			o_next = (object*) run_chain;
			run_chain = this;
		}
		else {
			while (pp = (sched *) p->o_next) {
				if (tt < pp->s_time) {
					o_next = pp;
					p->o_next = this;
					return;
				}
				else p = pp;
			}
			p->o_next = this;
		}
	}
	else 
		run_chain = this;
}

void sched.remove()
/* remove from run_chain and make IDLE */
{
DB(("%x->sched::remove( )\n",this));
	register class sched * p;
	register class sched * pp;

	if (p = run_chain)
		if (p == this)
			run_chain = (sched*) o_next;
		else
			for (; pp = (sched*) p->o_next; p=pp)
				if (pp == this) {
					p->o_next = pp->o_next;
					goto ll;
				}
ll:
	s_state = IDLE;
	o_next = 0;
}

void sched.print(int n)
{
DB(("x%x->print( %d )\n", this,n));
	int m = n & ~CHAIN;

	switch (o_type) {
	case TIMER:
		((timer*)this)->print(m);
		break;
	case TASK:
		((task*)this)->print(m);
		break;
	}

	if (n&CHAIN) {
		if (o_next) ((sched*) o_next)->print(n);
	}
}
