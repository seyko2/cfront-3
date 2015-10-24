/*ident	"@(#)Time:tm/tmlex.c	3.1" */
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
 * return the tab table index that matches s ignoring case and .'s
 *
 * ntab and nsuf are the number of elements in tab and suf,
 * -1 for 0 sentinel
 *
 * all isalpha() chars in str must match
 * suf is a table of nsuf valid str suffixes 
 * if e is non-null then it will point to first unmatched char in str
 * which will always be non-isalpha()
 */

int tmlex_ATTLC(register char* s, char** e, char** tab, register int ntab, char** suf, int nsuf) {
	register char**	p;

	for (p = tab; ntab-- && *p; p++)
		if (tmword_ATTLC(s, e, *p, suf, nsuf))
			return(p - tab);
	return(-1);
}
