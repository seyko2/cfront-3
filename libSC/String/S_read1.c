/*ident	"@(#)String:S_read1.c	3.1" */
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

int
String::read(istream& is, int nbytes)
{

    if(nbytes < 0) return -1;

#ifdef IOSTREAMH
    streambuf* sb = is.rdbuf();
#endif

    if(d->refc > 1 || nbytes >= d->max) {
        d->rcdec();
        d = Srep_ATTLC::new_srep(nbytes);
    }
#ifndef IOSTREAMH
    int rem = nbytes;
    char *p = d->str;
    char *end = d->str + nbytes;
    while (1) {
	is.get(p, rem, '\0');
	while (*p) p++;
	if (!is) break; /* reached EOF or error */
	if (p >= end) break; /* enough characters have been read */
	is.get(*p);
	p++;
	rem = end - p;
    }
    return d->len = p - d->str;
#else
    return d->len = sb->sgetn(d->str,nbytes);
#endif
}
