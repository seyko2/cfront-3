/*ident	"@(#)String:S_misc.c	3.1" */
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
String::hashval() const
{
    register int rslt = 0;
    register char* p = d->str;
    register char* q = d->str + d->len;
    while(p < q) {
        rslt += rslt + *p++;
        if(rslt < 0) rslt = -rslt;
    }
    return rslt;
}

// return length of initial common substring
int
String::match(const String& r) const
{
    register char    *p = d->str;
    register char    *q = r.d->str;
    register int    i = (d->len<r.d->len ? d->len : r.d->len)+1;

    while (--i && *p++ == *q++) ;
    return i ? p - d->str -1 : p - d->str;
}

// return length of initial common substring
int
String::match(register const char *q) const
{
    register char    *p = d->str;
    register int    i = d->len+1;
    register char    c;

    while (--i && (c = *q++) &&*p++ == c)
        ;
    return i ? (c ? p - d->str - 1 : p - d->str) : d->len;
}

// return first differing position
int
String::firstdiff(const String& r) const
{
    register char    *p = d->str;
    register char    *q = r.d->str;
    register int    i = (d->len<r.d->len ? d->len : r.d->len)+1;

    while (--i && *p++ == *q++) ;
    /*
     * Three cases --
     *  1. There are no more characters in one of the strings (i == 0), and
     *     d->len <= r.d->str, so the string is a match with r or an
     *     initial substring of q.  In this case we should return -1.
     *  2. There are no more characters in one of the strings (i == 0), and
     *     d->len > r.d->str, so we must have that r is an initial
     *     substring of the String.  We return (p - d->str).
     *  3. We failed the *p++ == *q++ test, so we have found the first
     *     character that is different.  We return (p - d->str - 1)
     *     because we did an extra increment of p.
     */
    return i ? p - d->str -1 : ((d->len>r.d->len) ? p - d->str : -1);
}

// return first differing position
int
String::firstdiff(register const char *q) const
{
    register char    *p = d->str;
    register int    i = d->len+1;
    register char    c;

    while (--i && (c = *q++) &&*p++ == c)
        ;
    /*
     * Three cases --
     *  1. There are no more characters in the string, so we have a
     *     match with q or an initial substring of q.  In this case,
     *     i == 0, so we should return -1.
     *  2. We have reached the end of q (we got c == 0), so we must
     *     have that q is an initial substring of the String.  We
     *     return (p - d->str).
     *  3. We failed the *p++ == c test, so we have found the first
     *     character that is different.  We return (p - d->str - 1)
     *     because we did an extra increment of p.
     */
    return i ? (c ? p - d->str - 1 : p - d->str) : -1;
}

// Case Functions

String
String::upper() const
{
    register char c;

    register Srep_ATTLC* rd = Srep_ATTLC::new_srep(d->len);
    for(int i=0;i<d->len;i++) {
    	if((c=d->str[i]) >= 'a' && c <= 'z') {
       		rd->str[i] = c - ' ';
    	}
    	else rd->str[i] = c;
    }
    return String(rd);
}

String
String::lower() const
{
    register char c;

    register Srep_ATTLC* rd = Srep_ATTLC::new_srep(d->len);
    for(int i=0;i<d->len;i++) {
    	if((c=d->str[i]) >= 'A' && c <= 'Z') {
       		rd->str[i] = c + ' ';
    	}
    	else rd->str[i] = c;
    }
    return String(rd);
}
