/*ident	"@(#)Time:tm/tmleap.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#include <tm.h>

/*
 * return clock with leap seconds adjusted
 * for direct localtime() access
 */

time_t tmleap_ATTLC(register time_t* clock) {
	register struct tl*	lp;
	time_t			now;

	tmset();
	if (clock) now = *clock;
	else (void)time(&now);
	if (tm_flags_ATTLC & TM_ADJUST)
	{
		for (lp = &tm_leap_ATTLC[0]; now < (lp->time - lp->total); lp++);
		now += lp->total;
	}
	return(now);
}
