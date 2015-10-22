/*ident	"@(#) @(#)std_in.c	1.1.1.3" */
/******************************************************************************
*
* C++ Standard Library
*
* Copyright (c) 1996  Lucent Technologies.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Lucent Technologies.
* The copyright notice above does not evidence any actual or
* intended publication of such source code.
*
******************************************************************************/

#include <ctype.h>
#include <iostream.h>
#include "std_string.h"
#ifdef NAMESPACES_LUCENT
namespace std {
#endif

#if defined(COMPILER_IS_PC_LUCENT)
#define IOSTREAMH
#endif

// STREAMS INPUT/OUTPUT

istream&
operator>>(istream& ii, string& ss)
{
#ifndef IOSTREAMH
    /*
     * skip whitespace -- return a null string if error encountered
     * while skipping whitespace
     */
    char c;
    while (ii && ii.get(c) && isspace(c)) {}
    if (!ii) {
	ss = string();
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
	    register int w = ss.d->max_size;
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
	ss = string();
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
	ss = string();
	return ii;
    }

    register streambuf *nbp = ii.rdbuf();

    int fwidth = ii.width();
    if (fwidth != 0) {
	register int c = nbp->sgetc();
	if (c == EOF) {
	    /* no characters have been read in */
	    ss = string();
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
	    register int w = ss.d->max_size;
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
	ss = string();
	ii.clear(ios::badbit | ii.rdstate());
    }
    return ii;
#endif
}

istream& getline(istream &ii, string &ss, char delim) {
#ifndef IOSTREAMH
    const N = 64;
    int first = 1;

    /*
     * The first time through the loop, don't expand the size
     * of the existing string.  On the second and succeeding
     * times, append the new characters to the end.
     */
    while (ii && !ii.eof()) {
	if (c == EOF || c == delim) {
	    break;
	}

	if (first) {
	    first = 0;
	    ss.uniq();
	    register int w = ss.d->max_size;
	    char *start = ss.d->str;
	    register char *p = start;

	    while (c != delim && ii && c != EOF && --w > 0) {
		*p++ = c;
		ii.get(c);
	    }

	    ss.d->len = p - start;
	    if (c == delim) {
		break;
	    }
	}
	else {
	    char buf[N];
	    register int w = N;
	    register char *p = buf;

	    while (c != delim && ii && c != EOF && --w > 0) {
		*p++ = c;
		ii.get(c);
	    }
	    ss.append(buf, p - buf);
	    if (c == delim) {
		break;
	    }
	}
    }
    if (first) {
	/* no characters have been read in */
	ss = string();
	ii.clear(_bad);
    }
    else if (!ii) {
	ii.clear(_good);
    }
    return ii;

#else

    register streambuf *nbp = ii.rdbuf();
    const int N = 64;
    int first = 1;

    if (ii && (ii.eof() || nbp->sgetc() == EOF)) {
	ii.clear(ios::failbit | ii.rdstate());
	return ii;
    }

    /*
     * The first time through the loop, don't expand the size
     * of the existing string.  On the second and succeeding
     * times, append the new characters to the end.
     */
    while (ii && !ii.eof()) {
	register int c = nbp->sgetc();
	if (c == EOF || c == delim) {
	    if (c == delim) nbp->snextc();
	    break;
	}

	if (first) {
	    first = 0;
	    ss.uniq();
	    register int w = ss.d->max_size;
	    char *start = ss.d->str;
	    register char *p = start;

	    while (c != delim && c != EOF && --w > 0) {
		*p++ = c;
		c = nbp->snextc();
	    }

	    ss.d->len = p - start;
	}
	else {
	    char buf[N];
	    register int w = N;
	    register char *p = buf;

	    while (c != delim && c != EOF && --w > 0) {
		*p++ = c;
		c = nbp->snextc();
	    }
	    ss.append(buf, p - buf);
	}
    }

    if (first) {
	/* no characters have been read in */
	ss = string();
	ii.clear(ios::badbit | ii.rdstate());
    }
    return ii;
#endif
}

#ifdef NAMESPACES_LUCENT
}
#endif
