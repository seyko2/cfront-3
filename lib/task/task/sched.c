/*ident	"@(#)cls4:lib/task/task/sched.c	1.6" */
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
/* Most environments have wait(2) declared in osfcn.h, but BSD has it
 * in sys/wait.h. */
#include <osfcn.h>
#ifdef BSD
#include <sys/wait.h>
#endif

int sched::keep_waiting_count = 0;
sched* sched::runchain = 0;
sched* sched::priority_sched = 0;
long sched::clxck = 0;
int sched::exit_status = 0;
task* sched::clock_task = 0;
PFV sched::exit_fct = 0;

void
sched::setclock(long t)
{
	if (clxck) object::task_error(E_SETCLOCK, (object*)0);
	clxck = t;
}

void
sched::cancel(int res)
{
	if (s_state==RUNNING) remove();
	s_state = TERMINATED;
	s_time = res;
	alert();
}

int
sched::result()
/* wait for termination and retrieve result */
{
	if (this == (sched*)this_task()) task_error(E_RESULT, this);
	while (s_state != TERMINATED) {
		this_task()->sleep(this);
	}

	return (int) s_time;
}

extern void sig_hold(int*);
extern void sig_relse(int*);

void
sched::schedule()
/* schedule either clock_task or front of runchain */
{
	register sched* p;
	register long tt;

keep_waiting:
	if (p = priority_sched) {
		priority_sched = 0;  // no chain here
		p->remove();  // in case it's also on the runchain
		p->s_state = RUNNING;  // since remove sets it IDLE
	}
	else if (p = runchain) {
		runchain = (sched*) p->o_next;
		p->o_next = 0;
	}
	else {
		if (keep_waiting_count) {
			::pause();
			goto keep_waiting;
		}
		if (exit_fct) (*exit_fct)();
		for (p = task::txsk_chxin; p; p = ((task*)p)->t_next) {
			if (p->s_state == RUNNING) p->remove();
			p->s_state = TERMINATED;
			p->s_time = 0;
		}
		exit(exit_status);
	}

	int mask;
	sig_hold(&mask);
	tt = p->s_time;
	if (tt != clxck) {
		if (tt < clxck) task_error(E_SCHTIME, this);
		clxck = tt;
		sig_relse(&mask);
		if (clock_task) {
			if (clock_task->s_state != IDLE)
				task_error(E_CLOCKIDLE, this);
			/* clock_task preferred -- put p back onto runchain */
			p->o_next = (object*) runchain;
			runchain = p;
			p = (sched*) clock_task;
		}
	} else
		sig_relse(&mask);
	if (p != this)
		p->resume();
} /* schedule */

void
sched::insert(long d, object* who)
/*
	schedule THIS to run in ``d'' time units
	inserted by who
*/
{
	register sched * p;
	register sched * pp;
	register long tt = s_time = clxck + d;

	switch (s_state) {
	case TERMINATED:
		task_error(E_RESTERM, this);
		break;
	case IDLE:
		break;
	case RUNNING:
		if (this != (class sched *)this_task()) task_error(E_RESRUN, this);
	}

	if (d<0) task_error(E_NEGTIME, this);	

	if (o_next) task_error(E_RESOBJ, this);

	s_state = RUNNING;
	setwho(who);

	/* runchain ordered by s_time */
	if (p = runchain) {
		if (tt < p->s_time) {
			o_next = (object*) runchain;
			runchain = this;
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
		runchain = this;
}

void
sched::remove()
/* remove from runchain and make IDLE */
{
	register class sched * p;
	register class sched * pp;

	if (p = runchain)
		if (p == this)
			runchain = (sched*) o_next;
		else
			for (; pp = (sched*) p->o_next; p=pp)
				if (pp == this) {
					p->o_next = pp->o_next;
					break;
				}
	s_state = IDLE;
	o_next = 0;
}

void
sched::set_priority_sched(sched* p)
{
	p->s_time = clxck;
	priority_sched = p;
}

void
sched::print(int n, int baseClass)
{
	if (!baseClass)
		printf("naked sched (object not part of class derived from sched)");
	if (n&VERBOSE) {
		printf("\tsched:  this=%x\n", this);
	}
	if (n&CHAIN) {
		object::print(n, 1);	// call object::print here to keep
					// output for same object together
		// If this is a task, all task objects are already being
		// printed under CHAIN.  Therefore, don't call print
		// recursively for tasks.  If current task is thistask,
		// print entire run_chain with 0 arg, to keep it short.
		if (o_type() == TASK) {
			if (this == thxstxsk) {
				sched *sp = get_run_chain();
				if (sp == 0) {
					printf("run chain is empty\n");
				} else {
					printf("run chain is:\n");
					for (; sp; sp = (sched*)sp->o_next) {
						sp->print(0);
					}
					printf("end of run chain.\n");
				}
			}
		} else if (o_next)  {
			printf("Next sched object on run chain is:\n");
			((sched*) o_next)->print(n);
		} 
	} else {
		object::print(n, 1);
	}

	if (!baseClass)
		task_error(E_SCHOBJ, this);  // only derived class instances allowed

}

/* sched::resume() is a virtual function.  Because sched is not intended
 * to be used directly, but only as a base class, this should never be called.
 * Must define resume for each class derived from sched.
 */
void
sched::resume()
{
	task_error(E_SCHOBJ, this);
}

/* sched::setwho() is a virtual function.  Because sched is not intended
 * to be used directly, but only as a base class, this should never be called.
 * Must define setwho for each class derived from sched.
 */
void
sched::setwho(object*)
{
	task_error(E_SCHOBJ, this);
}
