/*ident	"@(#)String:S_setup.c	3.1" */
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

#include "String.h"
#include <stdlib.h>
#include <assert.h>

Pool* Srep_ATTLC::Reelp = 0;
Pool* Srep_ATTLC::SPoolp = 0;
Srep_ATTLC* Srep_ATTLC::nullrep_ = 0;

void
Srep_ATTLC::doinitialize() 
{
    Reelp = new Pool(sizeof(Srep_ATTLC));
    SPoolp = new Pool(sizeof(Srep_ATTLC) + MAXL - MAXS);
    nullrep_ = new Srep_ATTLC();
}

#if 0
void
Srep_ATTLC::cleanup()
{
    delete Reelp;
    delete SPoolp;
    delete nullrep_;
}
#endif

Srep_ATTLC *
Srep_ATTLC::new_srep(unsigned int length) 
{ 
    initialize();
    register Srep_ATTLC* x;
    if(length==0) return nullrep_;
    if(length>= MAXL) {
         x = get_long(length);
    }
    else {
	if (length < MAXS) 
		x = (Srep_ATTLC*) Reelp->alloc();
	else
		x = (Srep_ATTLC*) SPoolp->alloc();
        x->len = length;
        x->refc = 1;
        x->max = length < MAXS ? MAXS : MAXL;
    }
    return x;
}

Srep_ATTLC *
Srep_ATTLC::get_long(unsigned length) 
{ 
    register int m = 128;
    while(m <= length) m<<=1;

    register Srep_ATTLC *x = (Srep_ATTLC *) new char[m + sizeof(Srep_ATTLC) - MAXS]; 
    x->len = length;
    x->max = m;
    x->refc = 1;
    return x;
}

void 
Srep_ATTLC::delete_srep() 
{ 
    assert(Reelp != 0);
    if(max==0) 
	; 
    else if(max <= MAXS) Reelp->free((char*) this); 
    else if(max <= MAXL) SPoolp->free((char*) this); 
    else delete (char *) this; 
}

String::String(const char* st,unsigned length)
{
    if(!st) length=0;
    d = Srep_ATTLC::new_srep(length);
    if(length) memcpy(d->str,st,length);
}

String::String(const char* st)
{
    register int ln = st ? strlen(st) : 0;
    d = Srep_ATTLC::new_srep(ln);
    if(ln) memcpy(d->str,st,ln);
}

char *
Memcpy_String_ATTLC(register char *s1, register const char *s2, int n)
{
	char *	os1 = s1;
	register char *	ns;

	if (s1 <= s2) {
		ns = s1 + n;  /* stopper for s1 */
		while ( s1 < ns )
			*s1++ = *s2++;
	} else {
		ns = s1;
		s1 += n;
		s2 += n;
		while ( ns < s1 )
			*--s1 = *--s2;
	}
	return(os1);
}
/*
 * the overflow function is called when a copy operation overflows
 * the reference count field of the Srep_ATTLC structure.
 */
void
String::overflow() const
{
    String *sptr = (String *) this;
    sptr->d->refc--;
    /* make a copy of the existing String representation */
    register Srep_ATTLC* x = Srep_ATTLC::new_srep(d->len);
    memcpy(x->str,d->str,d->len);
    x->len = d->len;
    sptr->d = x;
}
void
Tmpstring::overflow() const
{
    Tmpstring *sptr = (Tmpstring *) this;
    sptr->d->refc--;
    /* make a copy of the existing String representation */
    register Srep_ATTLC* x = Srep_ATTLC::new_srep(d->len);
    memcpy(x->str,d->str,d->len);
    x->len = d->len;
    sptr->d = x;
}
