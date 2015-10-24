/*ident	"@(#)String:S_input.c	3.1" */
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
#include <ctype.h>
#include <iostream.h>
#ifdef __GNUG__
#define IOSTREAMH
#endif

// STREAMS INPUT/OUTPUT

istream&
operator>>(istream& ii, String& ss)
{
#ifndef IOSTREAMH
    /*
     * skip whitespace -- return a null string if error encountered
     * while skipping whitespace
     */
    char c;
    while (ii && ii.get(c) && isspace(c)) {}
    if (!ii) {
	ss = String();
	return ii;
    }
    const N = 64;
    int first = 1;

    /*
     * The first time through the loop, don't expand the size
     * of the existing string.  On the second and succeeding
     * times, append the new characters to the end.
     */
    while (ii && !ii.eof()) {
	if (c == EOF || isspace(c)) {
	    break;
	}

	if (first) {
	    first = 0;
	    ss.uniq();
	    register int w = ss.d->max;
	    char *start = ss.d->str;
	    register char *p = start;

	    while (!isspace(c) && ii && c != EOF && --w > 0) {
		*p++ = c;
		ii.get(c);
	    }

	    ss.d->len = p - start;
	    if (isspace(c)) {
		ii.putback(c);
		break;
	    }
	}
	else {
	    char buf[N];
	    register int w = N;
	    register char *p = buf;

	    while (!isspace(c) && ii && c != EOF && --w > 0) {
		*p++ = c;
		ii.get(c);
	    }
	    ss.append(buf, p - buf);
	    if (isspace(c)) {
		ii.putback(c);
		break;
	    }
	}
    }
    if (first) {
	/* no characters have been read in */
	ss = String();
	ii.clear(_bad);
    }
    else if (!ii) {
	ii.clear(_good);
    }
    return ii;

#else

    /*
     * skip whitespace -- return a null string if error encountered
     * while skipping whitespace
     */
    if (!ii.ipfx(0)) {
	ss = String();
	return ii;
    }

    register streambuf *nbp = ii.rdbuf();

    int fwidth = ii.width(0);
    if (fwidth != 0) {
	register int c = nbp->sgetc();
	if (c == EOF) {
	    /* no characters have been read in */
	    ss = String();
	    ii.clear(ios::badbit | ii.rdstate());
	}
	else {
	    ss.uniq();
	    ss.reserve(fwidth);
	    char *start = ss.d->str;
	    register char *p = start;
	    while (!isspace(c) && c != EOF && --fwidth > 0) {
		*p++ = c;
		c = nbp->snextc();
	    }
	    ss.d->len = p - start;
	}
	return ii;
    }

    const int N = 64;
    int first = 1;

    /*
     * The first time through the loop, don't expand the size
     * of the existing string.  On the second and succeeding
     * times, append the new characters to the end.
     */
    while (ii && !ii.eof()) {
	register int c = nbp->sgetc();
	if (c == EOF || isspace(c)) {
	    break;
	}

	if (first) {
	    first = 0;
	    ss.uniq();
	    register int w = ss.d->max;
	    char *start = ss.d->str;
	    register char *p = start;

	    while (!isspace(c) && c != EOF && --w > 0) {
		*p++ = c;
		c = nbp->snextc();
	    }

	    ss.d->len = p - start;
	}
	else {
	    char buf[N];
	    register int w = N;
	    register char *p = buf;

	    while (!isspace(c) && c != EOF && --w > 0) {
		*p++ = c;
		c = nbp->snextc();
	    }
	    ss.append(buf, p - buf);
	}
    }

    if (first) {
	/* no characters have been read in */
	ss = String();
	ii.clear(ios::badbit | ii.rdstate());
    }
    return ii;
#endif
}
