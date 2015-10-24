/*ident	"@(#)Time:tm/tmword.c	3.1" */
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

#include <ctype.h>
#include <tm.h>

/*
 * match s against t ignoring case and .'s
 *
 * suf is an n element table of suffixes that may trail s
 * if all isalpha() chars in s match then 1 is returned
 * and if e is non-null it will point to the first unmatched
 * char in s, otherwise 0 is returned
 */

int tmword_ATTLC(register char* s, char** e, register char* t, char** suf, int n) {
	register int	c;
	char*	b;

	if (*s && *t)
	{
		b = s;
		while (c = *s++)
		{
			if (c != '.')
			{
				if (!isalpha(c) || c != *t && (islower(c) ? toupper(c) : tolower(c)) != *t) break;
				t++;
			}
		}
		s--;
		if (!isalpha(c))
		{
			if (e) *e = (char*)s;
			return(s > b);
		}
		if (!*t && s > (b + 1))
		{
			b = s;
			while (n-- && (t = *suf++))
			{
				s = b;
				while (isalpha(c = *s++) && (c == *t || (islower(c) ? toupper(c) : tolower(c)) == *t)) t++;
				if (!*t && !isalpha(c))
				{
					if (e) *e = (char*)s - 1;
					return(1);
				}
			}
		}
	}
	return(0);
}
