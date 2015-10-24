/*ident	"@(#)String:S_substr.c	3.1" */
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
#include <assert.h>

// Substring stuff:

Substring
String::operator()(unsigned n, unsigned m)
{
    return Substring(*this,n,m);
}

Substring
String::operator()(unsigned n)
{
#if !defined(NDEBUG)
    assert(d->len >= n);
#endif
    return Substring(*this,n,d->len-n);
}

Substring::Substring(const Substring &s):ss(s.ss),oo(s.oo),ll(s.ll){ }

void
Substring::operator=(const String& s)
{ 
    register int oldlen = ss->d->len;
    register int newlen = oldlen + s.d->len - ll;

    if(ll == oldlen) {        // substring is whole string
        *ss = s;
        return;
    }
    if(ll==0 && s.d->len==0) return;

    if(newlen == 0) {
        ss->d->rcdec();
        ss->d = Srep_ATTLC::nullrep();
        return;
    }

    if(ss->d->refc != 1 || newlen >= ss->d->max) {
	    register int mlen = newlen > oldlen ? newlen : oldlen;
        register Srep_ATTLC* x = Srep_ATTLC::new_srep(mlen);
	    // copy string
        memcpy(x->str,ss->d->str,oldlen);
        ss->d->rcdec();
        ss->d = x;
    }
    ss->d->len = newlen;

    if(ll == s.d->len) {
        Memcpy_String_ATTLC(ss->d->str+oo,s.d->str,s.d->len);
        return;
    }
    
    // copy end
    Memcpy_String_ATTLC(ss->d->str+oo+(s.d->len),ss->d->str+oo+ll,oldlen-oo-ll);

    // copy middle
    Memcpy_String_ATTLC(ss->d->str+oo,s.d->str,s.d->len);

    return;
}

void
Substring::operator=(const char* s)
{ 
    register int slength = s ? strlen(s) : 0;
    register int oldlen = ss->d->len;
    register int newlen = oldlen + slength - ll;

    if(ll == oldlen) {        // substring is whole string
        *ss = s;
        return;
    }
    if(ll==0 && slength==0) return;

    if(newlen == 0) {
        ss->d->rcdec();
        ss->d = Srep_ATTLC::nullrep();
        return;
    }

    if(ss->d->refc != 1 || newlen >= ss->d->max) {
	    register int mlen = newlen > oldlen ? newlen : oldlen;
        register Srep_ATTLC* x = Srep_ATTLC::new_srep(mlen);
	// copy string
        memcpy(x->str,ss->d->str,oldlen);
        ss->d->rcdec();
        ss->d = x;
    }
    ss->d->len = newlen;

    if(ll == slength) {
        Memcpy_String_ATTLC(ss->d->str+oo,s,slength);
        return;
    }
    
    // copy end
    Memcpy_String_ATTLC(ss->d->str+oo+slength,ss->d->str+oo+ll,oldlen-oo-ll);

    // copy middle
    Memcpy_String_ATTLC(ss->d->str+oo,s,slength);
}

String
String::chunk(unsigned n,unsigned m) const
{
    register char* sp = d->str + n;
    return String(sp, m);
}

String
String::chunk(unsigned n) const
{
    register int len = d->len;
    if(n == len) return String();
    register char* sp = d->str + n;
    return String(sp, len-n);
}
