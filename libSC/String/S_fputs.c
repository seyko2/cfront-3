/*ident	"@(#)String:S_fputs.c	3.1" */
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

int
fputs(const String& s1, FILE* iop)
{
    register char*	s = s1.d->str;
    register char*	e = s + s1.d->len;
    register int r;
    register int c;

    while (s < e) {
    	c = *s++;
    	r = putc(c, iop);
    }
    return(r);
}
