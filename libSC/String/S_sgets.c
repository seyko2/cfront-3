/*ident	"@(#)String:S_sgets.c	3.1" */
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

#define IN_STRING_LIB
#include "String.h"
#include <iostream.h>
#ifdef __GNUG__
#define IOSTREAMH
#endif

String
sgets(istream& ii) {
	const int N = 1024;
	static char buf[N];
#ifndef IOSTREAMH
	if (!ii) return String("");
	ii.get(buf, N, '\n');
	char *p = buf;
	while (*p) p++;
	String ret(buf, p - buf);

	char c;
	while (ii.get(c) && c != '\n' && c != EOF) {
		*buf = c;
		ii.get(buf+1, N-1, '\n');
		p = buf;
		while (*p) p++;
		ret.append(buf, p - buf);
	}
#else
	ii.get(buf, N, '\n');
	String ret(buf, ii.gcount());

	int c;
	while ((c = ii.get()) != '\n' && c != EOF) {
		*buf = c;
		ii.get(buf+1, N-1, '\n');
		ret.append(buf, ii.gcount()+1);
	}
#endif
	return ret;
}
