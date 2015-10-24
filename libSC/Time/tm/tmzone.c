/*ident	"@(#)Time:tm/tmzone.c	3.1" */
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
 * return minutes west of GMT for timezone name and type
 *
 * if type==0 then all time zone types match
 * otherwise type must be a value returned by tmtype()
 *
 * if end is non-null then it will point to the next
 * unmatched char in name
 *
 * if dst is non-null then it will point to the DST
 * offset for standard zones and 0 for daylight zones
 *
 * TM_LOCALZONE returned for no match
 */

int tmzone_ATTLC(register char* name, char** end, char* type, int* dst) {
	register struct tz*	zp;
	register char*		prev;

	tmset();
	zp = tm_local_ATTLC;
	prev = 0;
	do
	{
		if (zp->type) prev = zp->type;
		if (!type || type == prev || !prev)
		{
			if (tmword_ATTLC(name, end, zp->standard, (char**)0, 0))
			{
				if (dst) *dst = zp->dst;
				return(zp->west);
			}
			if (zp->dst && tmword_ATTLC(name, end, zp->daylight, (char**)0, 0))
			{
				if (dst) *dst = 0;
				return(zp->west + zp->dst);
			}
		}
		if (zp == tm_local_ATTLC) zp = &tm_zone_ATTLC[0];
		else zp++;
	} while (zp->standard);
	return(TM_LOCALZONE);
}
