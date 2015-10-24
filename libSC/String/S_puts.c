/*ident	"@(#)String:S_puts.c	3.1" */
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
puts(const String& s1)
{
    register char*	s = s1.d->str;
    register char*	e = s + s1.d->len;
    register int c;

    while (s < e) {
    	c = *s++;
    	putchar(c);
    }
    return(putchar('\n'));
}

