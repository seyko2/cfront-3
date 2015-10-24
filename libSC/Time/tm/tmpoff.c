/*ident	"@(#)Time:tm/tmpoff.c	3.1" */
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
#include <stdio.h>

/*
 * append p and hh:mm part of n to s
 *
 * n ignored if n==d
 * end of s is returned
 */

char* tmpoff_ATTLC(register char* s, register char* p, register int n, int d) {
	while (*s = *p++) s++;
	if (n != d)
	{
		if (n < 0)
		{
			n = -n;
			*s++ = '-';
		}
		else *s++ = '+';
		(void)sprintf(s, "%d", n / 60);
		while (*++s);
		if (n %= 60)
		{
			(void)sprintf(s, ":%02d", n);
			while (*++s);
		}
	}
	return(s);
}
