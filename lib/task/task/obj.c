/*ident	"@(#)cls4:lib/task/task/obj.c	1.5" */
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
#include <stdlib.h>

task* object::thxstxsk = 0;
PFIO object::error_fct = 0;

object::~object()
{
	if (o_next) task_error(E_ONEXT, this);
	register olink* ol = o_link;
	// Don't give error when this is remembering a TERMINATED task
	while (ol) {
		if (ol->l_task->s_state == sched::TERMINATED) {
			ol = ol->l_next;
		} else {
			task_error(E_OLINK, this);
			break;
		}
	}
} /* delete */

/*	note that a task can be on a chain in several places
*/

void
object::remember(task* t)
{
	if (t->s_state != sched::TERMINATED) {
		o_link = new olink(t, o_link);
	}
}

void
object::forget(register task* p)
/* remove all occurrences of task* p from this object's task list */
{
	register olink* ll;
	register olink* l;

	if (o_link == 0) return;

	while (o_link->l_task == p) {
		ll = o_link;
		o_link = ll->l_next;
		delete ll;
		if (o_link == 0) return;
	};

	l = o_link;
	while (ll = l->l_next) {
		if (ll->l_task == p) {
			l->l_next = ll->l_next;
			delete ll;
		}
		else l = ll;
	};
}


/*
 * prepare IDLE tasks on this object for scheduling
 * also flush the remember chain
 */
void
object::alert()
{
	register olink* l;
	register olink* ll;

	for (l=o_link; ll = l; l=l->l_next, delete ll) {
		register task* p = l->l_task;
		if (p->s_state == sched::IDLE) p->insert(0,this);
	}
	o_link = 0;
}

/*
 * virtual int object::pending() returns 1 if the object should be waited for.
 * By default say yes and assume that alert() will be called somehow.
 */
int
object::pending()
{
	return 1;
}

/*
 * object::print() is a virtual function.  In each derived class,
 * derived::print() will print the derived information,
 * then call its immediate base::print(), with second argument non-zero. 
 * The original virtual call will have the second argument 0.
 * The first arg is meant to be CHAIN, VERBOSE, or STACK, or several combined
 * with | (e.g., CHAIN|STACK)
 */
void
object::print(int n, int baseClass)
{
	if (!baseClass)
		printf("Unidentified object ");

	if (n&VERBOSE) {
		olink* l;
		printf("\tobject:  this=%x\n", this);
		if (!o_link)
			printf("\tNo tasks remembered\n");
		for (l=o_link; l; l=l->l_next) {
			printf("\tNext task on this=%x remember chain is:\n",
				this);
			l->l_task->print(n & ~CHAIN);
		}
	}

	if (n&CHAIN) {
		if (o_next) o_next->print(n);
	}
	printf("\n");
}

#define macro_start static char* error_name[] = {
#define macro(num,name,string) string,
#define macro_end(last_name) };
task_error_messages
#undef macro_start
#undef macro
#undef macro_end

static int in_error = 0;

void
_print_error(int n)
{

	register int i = (n<1 || MAXERR<n) ? 0 : n;

	printf("\n\n***** task_error(%d) %s\n",n,error_name[i]);

	if (object::this_task()) { 
		printf("thistask: ");
		thistask->print(VERBOSE|STACK);
	}
	if (sched::runchain) {
		printf("runchain:\n");
		sched::runchain->print(CHAIN);
	}
} /* _print_error */

// Obsolete version, remains for compatibility, use 2-argument static version
// instead
int
object::task_error(int n)
{
	if (in_error)
		exit(in_error);
	else
		in_error = n;

	if (error_fct) {
		n = (*error_fct)(n,this);
		if (n) exit(n);
	}
	else {
		_print_error(n);
		exit(n);
	}
	in_error = 0;
	return 0;
}
//The second, static, version of the task_error function is usable by
//both member functions and non-member functions.
int
object::task_error(int n, object* th)
{
	if (in_error)
		exit(in_error);
	else
		in_error = n;

	if (error_fct) {
		n = (*error_fct)(n, th);
					//th is really a "this" pointer for real
					//objects, and is 0 for non-members
		if (n) exit(n);
	}
	else {
		_print_error(n);
		exit(n);
	}
	in_error = 0;
	return 0;
}
