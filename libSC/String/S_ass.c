/*ident	"@(#)String:S_ass.c	3.1" */
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

// STRING OPERATORS:

// Assignment:

String&
String::operator=(Stringsize s)           
{ 
    if(d->refc>1) {
        Srep_ATTLC *x = Srep_ATTLC::new_srep(d->len);
        if (d->len>0) memcpy(x->str,d->str,d->len);
        d->rcdec();
        d = x;
    }
        
    if(d->len==0) {
        d->rcdec();
        d = Srep_ATTLC::new_srep(s.i);
        d->len=0;
        return *this;
    }

    if(s.i >= d->max) {
        Srep_ATTLC *x = Srep_ATTLC::new_srep(s.i);
        x->len = d->len;
        if (d->len>0) memcpy(x->str,d->str,d->len);
        d->rcdec();
        d = x;
    }
    if(s.i<d->len) d->len=s.i;
    return *this;
}


void
String::assign(const char* s, unsigned ln)
{
    if (ln==1) {  // optimization
        *this = *s;
        return;
    }
    if(d->refc>1 || ln>=d->max ) {
        d->rcdec();
        d = Srep_ATTLC::new_srep(ln);
    }
    d->len = ln;
    if (ln>0) memcpy(d->str,s,ln);
}


String&
String::operator=(const char* s)
{
    register int ln = s ? strlen(s) : 0;
    assign(s, ln);
    return *this;
}


