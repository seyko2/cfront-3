/*ident	"@(#)String:S_gets.c	3.1" */
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
gets(String& s1)
{
    register int c;
    register int i = 0;
    const int N = 64;
    char buf[N];

    s1 = "";
    while ((c = getchar()) != '\n' && c >= 0) {
	if(i==N-1) {
            buf[i]='\0';
            s1 += buf;
            i = 0;
        }
        buf[i++]=c;
    }
    buf[i]='\0';
    s1 += buf;
    if (c<0 && s1.length()==0)
    	return 0;
    return 1;
}
