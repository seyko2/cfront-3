/*ident	"@(#)String:S_index.c	3.1" */
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
String::index(char c, unsigned int pos) const
{
    char *end = d->str + d->len;
    char *begin = d->str + pos;
    while(begin < end) {
        if(*begin++ == c) return pos;
        pos++;
    }
    return -1;
}

int
String::index(const String& pattern, unsigned int pos) const
{
    register int plen = pattern.d->len;
    register int slen = d->len;
    register char* pp = pattern.d->str;
    register char* sbp = d->str + pos;
    register char* sep = d->str + slen;

    for(int i=pos;sbp<sep-plen+1;i++) {
        if(!memcmp(sbp++,pp,plen)) return i;
    }
    return -1;
}
