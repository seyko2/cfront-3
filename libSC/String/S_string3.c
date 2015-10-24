/*ident	"@(#)String:S_string3.c	3.1" */
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
// string (3) stuff

/* position of first occurrence of char */
int
String::strchr(char c) const
{
    register char *p = d->str-1, *q = d->str + d->len;
    while ( ++p < q )
        if ( *p == c )
            return p - d->str;
    return -1;
}

/* ... last ... (-1 if char not there) */
int
String::strrchr(char c) const
{
    register char *p = d->str + d->len, *q = d->str;
    while ( --p >= q )
        if ( *p == c )
            return p - d->str;
    return -1;
}

int
String::strpbrk(const String &s) const
{
    char  tab[256];
    register char *p, *q;
    for ( p = tab, q=&tab[256]; p<q;)
        *p++ = 0;
    for ( p = s.d->str, q=s.d->str+s.d->len; p<q; )
        tab[(unsigned char)*p++] = 1;
    for ( p = d->str-1, q=d->str+d->len; ++p < q; )
        if ( tab[(unsigned char)*p] )
            return p - d->str;
    return -1;
}

int
String::strspn(const String &s) const
{
    char    tab[256];
    register char    *p, *q;
    for ( p = tab, q=&tab[256]; p<q;)
        *p++ = 0;
    for ( p = s.d->str, q=s.d->str+s.d->len; p<q; )
        tab[(unsigned char)*p++] = 1;
    for ( p = d->str-1, q=d->str+d->len; ++p < q; )
        if ( !tab[(unsigned char)*p] )
            break;
    return p - d->str;
}

int
String::strcspn(const String &s) const
{
    char    tab[256];
    register char    *p, *q;
    for ( p = tab, q=&tab[256]; p<q;)
        *p++ = 0;
    for ( p = s.d->str, q=s.d->str+s.d->len; p<q; )
        tab[(unsigned char)*p++] = 1;
    for ( p = d->str-1, q=d->str+d->len; ++p < q; )
        if ( tab[(unsigned char)*p] )
            break;
    return p - d->str;
}

