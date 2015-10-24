/*ident	"@(#)Path:v9-tmpnam.c	3.1" */
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

#include <tmpnam.h>

extern char *mktemp(), *strcpy(), *strcat();
static char str[L_tmpnam], seed[] = { 'a', 'a', 'a', '\0' };

char *
tmpnam(s)
char    *s;
{
        register char *p, *q;

        p = (s == 0)? str: s;
        (void) strcpy(p, P_tmpdir);
        (void) strcat(p, seed);
        (void) strcat(p, "XXXXXX");

        q = seed;
        while(*q == 'z')
                *q++ = 'a';
        ++*q;

        (void) mktemp(p);
        return(p);
}
