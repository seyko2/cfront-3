/*ident	"@(#)cls4:lib/task/task/timer.c	1.3" */
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

timer::timer(long d)
{
	s_state = IDLE;
	insert(d, (object*)this);
}

timer::~timer()
{
	if (s_state != TERMINATED) task_error(E_TIMERDEL, this);
}

void
timer::reset(long d)
{
	remove();
	insert(d, (object*)this);
}

void
timer::print(int n, int baseClass)
{ 
	if (!baseClass)
		printf("timer %ld == clock+%ld\n",s_time,s_time - get_clock());

	sched::print(n, 1);
}

void
timer::resume()  // time is up; "delete" timer & schedule next task
{
	s_state = TERMINATED;
	alert();
	schedule();
}
