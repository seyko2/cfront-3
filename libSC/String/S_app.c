/*ident	"@(#)String:S_app.c	3.1" */
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

// Appending:

String&
String::newcopy(char c)
{
    register int oldlen = d->len;
    register Srep_ATTLC *x = Srep_ATTLC::new_srep(oldlen+1);
    if (oldlen>0) memcpy(x->str,d->str,oldlen);
    x->str[oldlen] = c;
    d->rcdec();
    d = x;
    return *this;
}

void
String::append(const char * s, unsigned ln)
{
    if(ln==0) 
        return;
    if(ln==1) {  // optimization
        *this += *s;
        return;
    }
    if(d->max==0) {
        assign(s, ln);
        return;
    }

    register int oldlen = d->len;
    register int newln = ln + oldlen;

    if(d->refc > 1 || newln >= d->max) {
        register Srep_ATTLC *x = Srep_ATTLC::new_srep(newln);
        if (oldlen>0) memcpy(x->str,d->str,oldlen);
        if (ln>0) memcpy(x->str+oldlen,s,ln);
        d->rcdec();
        d = x;
        return;
    }
    if (ln>0) memcpy(d->str+oldlen,s,ln);
    d->len += ln;
}

String&
String::operator+=(const char * s)
{
    register int ln = s ? strlen(s) : 0;
    append(s, ln);
    return *this;
}

String&
String::operator+=(const String& s)
{
    append(s.d->str, s.d->len);
    return *this;
}

