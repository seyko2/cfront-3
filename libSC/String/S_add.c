/*ident	"@(#)String:S_add.c	3.1" */
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

// Addition:

Tmpstring
operator+(const String& s,const String& r)
{

    register int slen = s.d->len;
    register int rlen = r.d->len;

    if(rlen == 0) return Tmpstring(s);
    if(slen == 0) return Tmpstring(r);

    register int newln = rlen + slen;
    register Srep_ATTLC* rd = Srep_ATTLC::new_srep(newln);

    memcpy(rd->str,s.d->str,slen);
    memcpy(rd->str+slen,r.d->str,rlen);

    return Tmpstring(rd);
}

Tmpstring
operator+(const String& s,const char* st)
{

    register int slen = s.d->len;
    register int ln = st ? strlen(st) : 0;

    if(ln==0) return Tmpstring(s);

    register int newln = ln + slen;
    register Srep_ATTLC* rd = Srep_ATTLC::new_srep(newln);

    memcpy(rd->str,s.d->str,slen);
    memcpy(rd->str+slen,st,ln);

    return Tmpstring(rd);
}

Tmpstring
operator+(const char* st,const String& s)
{

    register int ln = st ? strlen(st) : 0;
    register int slen = s.d->len;

    if(ln==0) return Tmpstring(s);

    register int newln = ln + slen;
    register Srep_ATTLC* rd = Srep_ATTLC::new_srep(newln);

    memcpy(rd->str,st,ln);
    memcpy(rd->str+ln,s.d->str,slen);

    return Tmpstring(rd);
}

Tmpstring
operator+(const String& s,char c)
{
    register int slen = s.d->len;

    register int newln = 1 + slen;
    register Srep_ATTLC* rd = Srep_ATTLC::new_srep(newln);

    memcpy(rd->str,s.d->str,slen);
    rd->str[slen] = c;

    return Tmpstring(rd);
}

Tmpstring
operator+(char c,const String& s)
{

    register int slen = s.d->len;

    register int newln = 1 + slen;
    register Srep_ATTLC* rd = Srep_ATTLC::new_srep(newln);

    memcpy(rd->str+1,s.d->str,slen);
    rd->str[0] = c;

    return Tmpstring(rd);
}

Tmpstring&
Tmpstring::operator+(char c)
{
    register int oldlen = d->len;
    if(d->refc > 1 || oldlen == d->max - 1) {
        register Srep_ATTLC *x = Srep_ATTLC::new_srep(oldlen+1);
        memcpy(x->str,d->str,oldlen);
        x->str[oldlen] = c;
        d->rcdec();
        d = x;
        return *this;
    }
    d->str[oldlen] = c;
    d->len++;
    return *this;
}

Tmpstring&
Tmpstring::operator+(const char *s)
{
    register int ln = s ? strlen(s) : 0;
    if(ln==0) return *this;
    register int oldlen = d->len;
    register int newln = ln + oldlen;

    if(d->refc > 1 || newln >= d->max) {
        register Srep_ATTLC *x = Srep_ATTLC::new_srep(newln);
        memcpy(x->str,d->str,oldlen);
        memcpy(x->str+oldlen,s,ln);
        d->rcdec();
        d = x;
        return *this;
    }
    memcpy(d->str+oldlen,s,ln);
    d->len += ln;
    return *this;
}

Tmpstring&
Tmpstring::operator+(const String& s)
{
    register int ln = s.d->len;

    if(ln==0) return *this;

    register int oldlen = d->len;
    register int newln = ln + oldlen;
    if(d->refc > 1 || newln >= d->max) {
        register Srep_ATTLC *x = Srep_ATTLC::new_srep(newln);
        memcpy(x->str,d->str,oldlen);
        memcpy(x->str+oldlen,s.d->str,ln);
        d->rcdec();
        d = x;
        return *this;
    }
    memcpy(d->str+oldlen,s.d->str,ln);
    d->len += ln;
    return *this;
}
