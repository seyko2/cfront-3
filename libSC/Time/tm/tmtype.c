/*ident	"@(#)Time:tm/tmtype.c	3.1" */
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
 * return the tm_zone[] time zone type name for s
 *
 * if e is non-null then it will point to the first
 * unmatched char in s
 *
 * 0 returned for no match
 */

char* tmtype_ATTLC(register char* s, char** e) {
	register struct tz*	zp;
	register char*		t;

	tmset();
	zp = tm_local_ATTLC;
	do
	{
		if ((t = zp->type) && tmword_ATTLC(s, e, t, (char**)0, 0)) return(t);
		if (zp == tm_local_ATTLC) zp = &tm_zone_ATTLC[0];
		else zp++;
	} while (zp->standard);
	return(0);
}
