/*ident	"@(#)Time:tm/tmtime.c	3.1" */
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

#include <Timelib.h>
#include <tm.h>

/*
 * convert struct tm to time_t
 *
 * if west==TM_LOCALZONE then the local timezone is used
 * otherwise west is the number of minutes west
 * of GMT with DST taken into account
 */

time_t tmtime_ATTLC(register struct tm* tp, int west) {
	register time_t		clock;
	register struct tl*	lp;
	int			n;
	int			sec;
	time_t			now;

	tmset();
/*
 * The following equation is correct for the years 1898-2100.
 *
 * ( It is also correct for the interval [1802,1897] except for
 *   y such that (y-1805)%4==0. )
 */
	clock = (tp->tm_year * (4 * 365 + 1) - 69) / 4 - 70 * 365;
	if ((n = tp->tm_mon) > 11) n = 11;
	if (n > 1 && is_leap(1900+tp->tm_year)) clock++;
	clock += tm_msum_ATTLC[n] + tp->tm_mday - 1;
	clock *= 24;
	clock += tp->tm_hour;
	clock *= 60;
	clock += tp->tm_min;
	clock *= 60;
	clock += sec = tp->tm_sec;
	if (!(tm_flags_ATTLC & TM_UTC))
	{
		/*
		 * time zone adjustments
		 */

		if (west == TM_LOCALZONE)
		{
			clock += tm_local_ATTLC->west * 60;
			now = clock;
			if (tmmake_ATTLC(&now)->tm_isdst) clock += tm_local_ATTLC->dst * 60;
		}
		else clock += west * 60;
	}
	if (tm_flags_ATTLC & TM_LEAP)
	{
		/*
		 * leap second adjustments
		 */

		if (clock > 0)
		{
			for (lp = &tm_leap_ATTLC[0]; clock < lp->time - (lp+1)->total; lp++);
			clock += lp->total;
			n = lp->total - (lp+1)->total;
			if (clock <= (lp->time + n) && (n > 0 && sec > 59 || n < 0 && sec > (59 + n) && sec <= 59)) clock -= n;
		}
	}
	return(clock);
}
