/*ident	"@(#)String:S_fgets.c	3.1" */
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

static const int N = 128;
static char buf[N];
// last available space in buf
static char * const bufend = buf + (N-1);

static int 
_fgets(String& s1, int n, FILE* iop, int unbounded)
{
    // optimization.
    int do_assign = (s1.length() > 0);

    char *p = buf;

    register int c = 0;
    while (
	(unbounded || (--n>0)) && 
	((c = getc(iop))>=0)
	) {
        if(p == bufend) {
            *p = '\0';
            if (do_assign) {
                s1.assign(buf, N-1);
                do_assign = 0;
            }
            else
                s1.append(buf, N-1);
            p = buf;
        }
        *p++ = c;
        if (c=='\n')
                break;
    }
    *p = '\0';
    unsigned l=p-buf;
    if (l==1) { // optimization
        if (do_assign)
            s1 = *buf;
        else
            s1 += *buf;
    }
    else {
        if (do_assign)
            s1.assign(buf, l);
        else
            s1.append(buf, l);
    }
    if (c<0 && s1.length()==0)
        return 0;
    return 1;
}

int
fgets(String& s1, FILE* iop)
{
	return _fgets(s1, 0, iop, 1);
}

int
fgets(String& s1, int n, FILE* iop)
{
	return _fgets(s1, n, iop, 0);
}
