/* @(#) timer.c 1.2 1/27/86 17:48:11 */
/*ident	"@(#)cfront:lib/task/timer.c	1.2"*/
/* @(#) timer.c 1.1 1/17/85 14:48:50 */
#include "task.h"

timer.timer(int d) : (TIMER)
{
DB(("x%x->timer( %d )\n", this, d));
	s_state = IDLE;
	insert(d,this);
}

timer.~timer()
{
DB(("x%x->~timer()\n", this));
	if (s_state != TERMINATED) task_error(E_TIMERDEL,this);
}

void timer.reset(int d)
{
DB(("x%x->~timer::reset( %d )\n", this,d));
	remove();
	insert(d,this);
}

void timer.print(int /*n*/)
{
	long tt = s_time;
	printf("timer %ld == clock+%ld\n",tt,tt-clock);
}
