/*ident	"@(#)String:S_queue.c	3.1" */
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
String::getX(char& c)
{
    if ( d->len < 1 ) return 0;

    c = *(d->str);

    register Srep_ATTLC *r = Srep_ATTLC::new_srep(d->max - 1);
    r->len = d->len - 1;
    memcpy(r->str,d->str+1,r->len);
    d->rcdec();
    d = r;
 
    return 1;
}

// remove from the back

int
String::unputX(char& c)		/* remove from back */
{
    if (d->len < 1) return 0;

    c = d->str[d->len-1];

    if (d->len == 1) {
        d->rcdec();
        d = Srep_ATTLC::nullrep();
    }
    else {
        if (d->refc == 1) d->len--;
        else {
            Srep_ATTLC *r = Srep_ATTLC::new_srep(d->len-1);
            memcpy(r->str,d->str,r->len);
            d->rcdec();
            d = r;
        }
    }
    return 1;
}

// unget(c) sticks c onto the front of the String

String&
String::unget(char c)
{
    switch (d->len) {
    case 0:
        d = Srep_ATTLC::new_srep(1);
        d->str[0]=c;
        break;
    case 1:
        if(d->refc > 1) {
            Srep_ATTLC *r = Srep_ATTLC::new_srep(2);
            r->str[1] = d->str[0];
            --d->refc;
            d = r;
        }    
        else {
            d->str[1] = d->str[0];
            d->len++;
        }
        d->str[0]=c;
        break;
    default:
        if (d->refc == 1 && d->len < d->max - 1) {
            d->str[d->len] = d->str[d->len-1];
            Memcpy_String_ATTLC(d->str+1, d->str, d->len - 1);
            d->len++;
            d->str[0] = c;
        }
        else {
            Srep_ATTLC *r = Srep_ATTLC::new_srep(d->len + 1);
            memcpy(r->str+1, d->str, d->len);
            r->str[0] = c;
            d->rcdec();
            d = r;
        }
    }
    return *this;
}

String&
String::unget(const String& s)    /* prepend */
{
    register int newlen = d->len+s.d->len;
    if (s.d->len == 0) return *this;
    if (d->len == 0 && d->max==0) return *this = s;

    register int mln = newlen >= d->max ? newlen : d->max -1;
    register Srep_ATTLC *temp = Srep_ATTLC::new_srep(mln);
    temp->len = newlen;
    memcpy(temp->str, s.d->str, s.d->len);
    memcpy(temp->str+s.d->len, d->str, d->len);
    d->rcdec();
    d = temp;
    return *this;
}

