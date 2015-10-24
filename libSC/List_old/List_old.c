/*ident	"@(#)List_old:List_old.c	3.1" */
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

#ifndef _LIST_OLD_C_
#define _LIST_OLD_C_

#include <List_old.h>

lnk_old_ATTLC::~lnk_old_ATTLC()
{
}

// Base class copy and ==

lnk_old_ATTLC*
lnk_old_ATTLC::copy()
{
	return this;
}

int
lnk_old_ATTLC::operator==(lnk_old_ATTLC&)
{
	return 0;
}

Lizt_old_ATTLC::Lizt_old_ATTLC() : sz(0), t(0), myit(this)
{
	myit.reset0();
}

Lizt_old_ATTLC::Lizt_old_ATTLC(const Lizt_old_ATTLC& a0, const Lizt_old_ATTLC& a1) : myit(this)
{
	init_all(a0);
	put(a1);
	myit.reset0(); 
}

Lizt_old_ATTLC::~Lizt_old_ATTLC() 
{ 
	delete_all();
	register Liztiter_old_ATTLC* s;
	register Liztiter_old_ATTLC* anIt;
	for (anIt = &myit; anIt; anIt = s) {
	    s = anIt->nextIt;
	    anIt->nextIt = 0;
	    anIt->theLizt = 0;
	    anIt->pred = 0;
	    anIt->index = 0;
	}
}

void    // don't specify head or tail
Lizt_old_ATTLC::add_a_link(lnk_old_ATTLC *nl)
{
	if (t) {
	    nl->nxt = head();
	    nl->prv = t;
	    head()->prv = nl;
	    t->nxt = nl;
	} else t = nl->nxt = nl->prv = nl;
	sz++;
}

int
Lizt_old_ATTLC::operator==(const Lizt_old_ATTLC& x) const
{
	if (this == &x) return 1;
	if (length() != x.length()) return 0;
	if (length() == 0) return 1;
	register lnk_old_ATTLC* mine = t;
	register lnk_old_ATTLC* yours = x.t;
	do {
	    if (!(mine->operator==(*yours))) return 0;
	    mine = mine->nxt;
	    yours = yours->nxt;
	} while (mine != t);
	return 1;
}

void
Lizt_old_ATTLC::delete_all()
{
	register lnk_old_ATTLC* aLink = t;
	if (aLink)
	    do {
	        register lnk_old_ATTLC*    victim = aLink;
	        aLink = aLink->prv;
	        delete victim;
	    } while (aLink != t);
}

void
Lizt_old_ATTLC::init_all(const Lizt_old_ATTLC& ll)
{
	register lnk_old_ATTLC* aLink;
	myit.cache = 0;
	sz = ll.sz;
	myit.index = ll.myit.index;
	myit.pred = ll.myit.pred; 
	myit.cacheNo = 0; //done for completeness, test probably for cache
	myit.nextIt = 0;
	if ((aLink = ll.t) == 0) {
	    t = 0;
	} else {
	    register lnk_old_ATTLC*    myLink;
	    myLink = t = aLink->copy();
	    if (aLink == ll.myit.pred) myit.pred = myLink;
	    while ((aLink = aLink->prv) != ll.t) {
	        register lnk_old_ATTLC*    newLink = aLink->copy();
	        newLink->nxt = myLink;
	        myLink->prv = newLink;
	        myLink = newLink;
	        if (aLink == ll.myit.pred) myit.pred = myLink;
	    }
	    myLink->prv = t;
	    t->nxt = myLink;
	}
}

// partial evaluation of Lizt_old_ATTLC::init_all(Lizt_old_ATTLC())
void
Lizt_old_ATTLC::init_all_to_empty()
{
	myit.cache = 0;
	sz = 0;
	myit.index = 0;
	myit.pred = 0;
	myit.cacheNo = 0;
	myit.nextIt = 0;
	t = 0;
}

void 
Lizt_old_ATTLC::make_empty()
{
	delete_all();
	init_all_to_empty();
	reset_all_iters();
}

Lizt_old_ATTLC&
Lizt_old_ATTLC::operator=(const Lizt_old_ATTLC& a)
{
	if (this == &a) return *this;
	delete_all();
	init_all(a);
	reset_all_iters();
	return *this;
}

void
Lizt_old_ATTLC::reset_all_iters() 
{ 
	register Liztiter_old_ATTLC* anIt;
	for (anIt = &myit; anIt; anIt = anIt->nextIt)
	    anIt->reset0();
}

Lizt_old_ATTLC&
Lizt_old_ATTLC::unget(lnk_old_ATTLC *nl)
{
	add_a_link(nl);

	register Liztiter_old_ATTLC* anIt;
	for (anIt = &myit; anIt; anIt = anIt->nextIt) {
	    myit.cacheNo++;
	    if (sz == 1) {
	        anIt->pred = nl;
	        anIt->index = 1;
	    }
	    else {
	        if(anIt->index==0) anIt->pred = nl;
	        anIt->index++;
	    }
	}

	return *this;
}

Lizt_old_ATTLC&
Lizt_old_ATTLC::unget(const Lizt_old_ATTLC& ll)
{
	if (ll.sz) {
	    register lnk_old_ATTLC*    aLink = ll.t;
	    do unget(aLink->copy());
	    while ((aLink = aLink->prv) != ll.t);
	}
	return *this;
}

Lizt_old_ATTLC&
Lizt_old_ATTLC::put(lnk_old_ATTLC *nl)
{
	add_a_link(nl);
	t = nl;
	register Liztiter_old_ATTLC* anIt;
	for (anIt = &myit; anIt; anIt = anIt->nextIt) {
	    if (anIt->index==0) {
	        anIt->pred = nl;
	        anIt->index = 0;
	    }
	}
	return *this;
}

Lizt_old_ATTLC&
Lizt_old_ATTLC::put(const Lizt_old_ATTLC& ll)
{
	if (ll.sz) {
	    register lnk_old_ATTLC*    aLink = ll.head();
	    do put(aLink->copy());
	    while (aLink != ll.t && (aLink = aLink->nxt, 1));
	}
	return *this;
}

lnk_old_ATTLC*    // removes the link but doesn't destroy it
Lizt_old_ATTLC::get()
{
	if (!t)
	    return NULL;
	lnk_old_ATTLC *oh = t->nxt;
	if (t == oh)
	    t = NULL;
	else {
	    lnk_old_ATTLC *nh = oh->nxt;
	    nh->prv = t;
	    t->nxt = nh;
	}
	sz--;
	register Liztiter_old_ATTLC* anIt;
	for (anIt = &myit; anIt; anIt = anIt->nextIt) {
	    if (anIt->cache == oh) anIt->cache = NULL;
	    else anIt->cacheNo--;
	    if (anIt->index <= 1) anIt->pred = t;
	    if (anIt->index > 0) anIt->index--;
	}
	return oh;
}

lnk_old_ATTLC*    // removes the link but doesn't destroy it
Lizt_old_ATTLC::unput()
{
	if (!t)
	    return NULL;
	lnk_old_ATTLC *ot = t;
	lnk_old_ATTLC *hh = t->nxt;
	if (hh == ot)
	    t = NULL;
	else {
	    lnk_old_ATTLC *nt = t->prv;
	    hh->prv = t = nt;
	    t->nxt = hh;
	}
	sz--;
	register Liztiter_old_ATTLC* anIt;
	for (anIt = &myit; anIt; anIt = anIt->nextIt) {
	    if (anIt->cache == ot) anIt->cache = NULL;
	    if (anIt->pred == ot)    // head or tail
	        anIt->pred = t;
	    if (anIt->index > length()) anIt->index--;
	}
	return ot;
}

// Liztiter_old_ATTLC operations

void
Liztiter_old_ATTLC::reset0()
{
	index = 0;
	pred = theLizt->t;
}

void
Liztiter_old_ATTLC::end_reset(unsigned i)
{
	register unsigned len = theLizt->sz;
	if(i>len) i=len;
	reset(len - i);
}
 
int
Liztiter_old_ATTLC::at_end() const
{
	return index == theLizt->sz;
}

Liztiter_old_ATTLC::Liztiter_old_ATTLC(Lizt_old_ATTLC& l) : theLizt(&l), cache(0)
{
	nextIt = (l.myit).nextIt;
	(l.myit).nextIt = this;
	reset0();
}

Liztiter_old_ATTLC::Liztiter_old_ATTLC(const Liztiter_old_ATTLC& l) : theLizt(l.theLizt)
{
	nextIt = ((l.theLizt)->myit).nextIt;
	((l.theLizt)->myit).nextIt = this;
	pred = l.pred;
	index = l.index;
}

Liztiter_old_ATTLC::~Liztiter_old_ATTLC() 
{ 
	if(theLizt && this != &(theLizt->myit)) {
	    if(this==(theLizt->myit).nextIt)
	        (theLizt->myit).nextIt = nextIt;
	    else {
		register Liztiter_old_ATTLC* anIt;
	        for (
	            anIt = (theLizt->myit).nextIt; 
	            anIt->nextIt != this;
	            anIt = anIt->nextIt
	        ) ;
	        anIt->nextIt = nextIt;
	    }
	}
	nextIt = 0;
	theLizt = 0;
	pred = 0;
	index = 0;
}

Liztiter_old_ATTLC&
Liztiter_old_ATTLC::operator=(const Liztiter_old_ATTLC& l)
{
	if(theLizt && this != &(theLizt->myit)) {
	    if(this==(theLizt->myit).nextIt)
	        (theLizt->myit).nextIt = nextIt;
	    else {
		register Liztiter_old_ATTLC* anIt;
	        for (
	            anIt = (theLizt->myit).nextIt;
	            anIt->nextIt != this;
	            anIt = anIt->nextIt
	        ) ;
	        anIt->nextIt = nextIt;
	    }
	}
	theLizt = l.theLizt;
	nextIt = ((l.theLizt)->myit).nextIt;
	((l.theLizt)->myit).nextIt = this;
	pred = l.pred;
	index = l.index;
	return *this;
}

lnk_old_ATTLC*
Liztiter_old_ATTLC::getAt(int ii)
{
	register int count;
	int forward;
	register lnk_old_ATTLC* from;
	//cerr << "cache: " << cache << " c#: " << cacheNo << "\n";
	if (ii >= theLizt->sz) return NULL;
	if (ii >= theLizt->sz - ii) {
	    count = theLizt->sz - ii - 1;
	    forward = 0;
	    from = theLizt->t;
	} else {
	    count = ii;
	    forward = 1;
	    from = theLizt->head();
	}
	//if (cache) {
	    //register altCount = ii - cacheNo;
	    //register a1 = altCount > 0 ? altCount : -altCount;
	    //if (a1 < count) {
	        //count = a1;
	        //forward = a1 == altCount;
	        //from = cache;
	    //}
	//}
	if (forward)
	    while (count--)
	        from = from->nxt;
	else
	    while (count--)
	        from = from->prv;
	cache = from;
	cacheNo = ii;
	return from;
}

void
Liztiter_old_ATTLC::reset(unsigned i)
{
	if(!i) reset0();
	else {
	    if(i > theLizt->length()) i = theLizt->length();
	    index = i;
	    if(i == 0 || i == theLizt->length()) {
	        pred = theLizt->t;
	    }
	    else pred = getAt(i - 1);
	}
}

void
Liztiter_old_ATTLC::insert_prev(lnk_old_ATTLC* nl)
{
	if (pred) {
	    nl->init(pred, pred->nxt);
	    pred->nxt->prv = nl;
	    pred->nxt = nl;
	    register Liztiter_old_ATTLC* anIt;
	    for (anIt=&(theLizt->myit); anIt; anIt=anIt->nextIt) {
	        if(this!=anIt && index < anIt->index) anIt->index++;
	    }
	} 
	else {
	    nl->init(nl, nl);
	    register Liztiter_old_ATTLC* anIt;
	    for (anIt=&(theLizt->myit); anIt; anIt=anIt->nextIt) {
	        anIt->pred = nl;
	    }
	}
	if (at_end()) theLizt->t = nl;
	pred = nl;
	index++;
	theLizt->sz++;
}

void
Liztiter_old_ATTLC::insert_next(lnk_old_ATTLC* nl)
{
	register Liztiter_old_ATTLC* anIt;
	for (anIt=&(theLizt->myit); anIt; anIt=anIt->nextIt) {
	    if(this!=anIt) {
	       if( anIt->index == index || (anIt->pred==pred && anIt->at_head()) )
	           anIt->pred = nl;
	       if( anIt->index >= index) anIt->index++;
	    }
	}
	if (pred) {
	    nl->init(pred, pred->nxt);
	    pred->nxt->prv = nl;
	    pred->nxt = nl;
	} else {
	    nl->init(nl, nl);
	    pred = nl;
	}
	if (at_end()) theLizt->t = nl;
	theLizt->sz++;
}

lnk_old_ATTLC*
Liztiter_old_ATTLC::remove_prev()
{
	if (at_head())
	    return NULL;
	lnk_old_ATTLC *doomed = pred;
	int oldIndex = index;
	if (pred == pred->nxt) {    // this is only item
	    theLizt->t = 0;
	    register Liztiter_old_ATTLC* anIt;
	    for (anIt=&(theLizt->myit); anIt; anIt=anIt->nextIt) {
	        anIt->pred = 0;
	        anIt->index = 0;
	    }
	} else {
	    if (doomed == theLizt->t)    // deleting tail
	        theLizt->t = doomed->prv;
	    register Liztiter_old_ATTLC* anIt;
	    for (anIt=&(theLizt->myit); anIt; anIt=anIt->nextIt) {
	        if(anIt->pred==doomed) anIt->pred = doomed->prv;
	        if(anIt->index >= oldIndex) anIt->index--;
	    }
	    pred->nxt = doomed->nxt;
	    doomed->nxt->prv = pred;
	}
	theLizt->sz--;
	return doomed;
}

lnk_old_ATTLC*
Liztiter_old_ATTLC::remove_next()
{
	if (at_end())
	    return NULL;

	lnk_old_ATTLC *doomed = pred->nxt;
	if (pred == pred->nxt) {    // this is only item
	    theLizt->t = 0;
	    register Liztiter_old_ATTLC* anIt;
	    for (anIt=&(theLizt->myit); anIt; anIt=anIt->nextIt) {
	        anIt->pred = 0;
	        anIt->index = 0;
	    }
	} else {
	    if (doomed == theLizt->t)    // deleting tail
	        theLizt->t = doomed->prv;
	    register Liztiter_old_ATTLC* anIt;
	    for (anIt=&(theLizt->myit); anIt; anIt=anIt->nextIt) {
	        if(anIt->pred == doomed) anIt->pred = doomed->prv;
	        if(anIt->index > index) anIt->index--;
	    }
	    pred->nxt = doomed->nxt;
	    doomed->nxt->prv = pred;
	}
	theLizt->sz--;
	return doomed;
}

lnk_old_ATTLC*
Liztiter_old_ATTLC::next()
{
	if (at_end()) return NULL;
	pred = pred->nxt;
	index++;
	return pred;
}

lnk_old_ATTLC*
Liztiter_old_ATTLC::prev()
{
	if (at_head()) return NULL;
	lnk_old_ATTLC *op = pred;
	pred = pred->prv;
	index--;
	return op;
}

#endif
