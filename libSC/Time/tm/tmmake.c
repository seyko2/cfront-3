/*ident	"@(#)Time:tm/tmmake.c	3.1" */
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
 * return tm struct for clock
 * leap seconds accounted for in return value
 */

struct tm* tmmake_ATTLC(time_t* clock) {
    register struct tm*	tp;
    register struct tl*	lp;
    int			leapsec;
    time_t		now;

    tmset();
    if (clock) now = *clock;
    else (void)time(&now);
    leapsec = 0;
    if(
	(tm_flags_ATTLC & (TM_ADJUST|TM_LEAP)) == (TM_ADJUST|TM_LEAP) && 
	now > 0
    ){
	for( lp = &tm_leap_ATTLC[0]; now < lp->time; lp++ );

	if(lp->total){

	    if(
		now == lp->time && 
		(leapsec = (lp->total-(lp+1)->total)) < 0
	    ){ 
		leapsec = 0;
	    }
	    now -= lp->total;
	}
    }
    if (tm_flags_ATTLC & TM_UTC)
	tp = gmtime_ATTLC(&now);
    else
	tp = localtime_ATTLC(&now);
    tp->tm_sec += leapsec;
    return(tp);
}
