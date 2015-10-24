/*ident	"@(#)Time:tm/tmgoff.c	3.1" */
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
#include <ctype.h>

/*
 * return minutes offset from [[-+]hh[:mm[:ss]]] expression
 *
 * if e is non-null then it points to the first unrecognized char in s
 * d returned if no offset in s
 */

int tmgoff_ATTLC(register char* s, char** e, int d) {
	register int	n;
	char*		t;

	if (isdigit(n = *s) || n == '-' || n == '+')
	{
		n = (int)strtol(s, &t, 10);
		if (t > s)
		{
			n *= 60;
			if (*t == ':') n += (n < 0 ? -1 : 1) * (int)strtol(t + 1, &t, 10);
			if (*t == ':') (void)strtol(t + 1, &t, 10);
		}
		else n = d;
	}
	else
	{
		n = d;
		t = s;
	}
	if (e) *e = t;
	return(n);
}
