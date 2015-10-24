/*ident	"@(#)List:List.c	3.1" */
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

#include <List.h>

lnk_ATTLC::~lnk_ATTLC()
{
}

// Base class copy and ==

lnk_ATTLC*
lnk_ATTLC::copy()
{
	return this;
}

int
lnk_ATTLC::operator==(lnk_ATTLC&)
{
	return 0;
}

Lizt_ATTLC::Lizt_ATTLC() : sz(0), t(0), iter_head(0)
{
}

Lizt_ATTLC::Lizt_ATTLC(const Lizt_ATTLC& a0, const Lizt_ATTLC& a1) : iter_head(0)
{
	init_all(a0);
	put(a1);
}

Lizt_ATTLC::~Lizt_ATTLC() 
{ 
	delete_all();
	register Liztiter_ATTLC* s;
	register Liztiter_ATTLC* anIt;
	for (anIt = iter_head; anIt; anIt = s) {
	    s = anIt->nextIt;
	    anIt->nextIt = 0;
	    anIt->theLizt = 0;
	    anIt->pred = 0;
	    anIt->index = 0;
	}
}

void    // don't specify head or tail
Lizt_ATTLC::add_a_link(lnk_ATTLC *nl)
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
Lizt_ATTLC::operator==(const Lizt_ATTLC& x) const
{
	if (this == &x) return 1;
	if (length() != x.length()) return 0;
	if (length() == 0) return 1;
	register lnk_ATTLC* mine = t;
	register lnk_ATTLC* yours = x.t;
	do {
	    if (!(mine->operator==(*yours))) return 0;
	    mine = mine->nxt;
	    yours = yours->nxt;
	} while (mine != t);
	return 1;
}

void
Lizt_ATTLC::delete_all()
{
	register lnk_ATTLC* aLink = t;
	if (aLink)
	    do {
	        register lnk_ATTLC*    victim = aLink;
	        aLink = aLink->prv;
	        delete victim;
	    } while (aLink != t);
}

void
Lizt_ATTLC::init_all(const Lizt_ATTLC& ll)
{
	register lnk_ATTLC* aLink;
	sz = ll.sz;
	//iter_head = 0;
	if ((aLink = ll.t) == 0) {
	    t = 0;
	} else {
	    register lnk_ATTLC*    myLink;
	    myLink = t = aLink->copy();
	    while ((aLink = aLink->prv) != ll.t) {
	        register lnk_ATTLC*    newLink = aLink->copy();
	        newLink->nxt = myLink;
	        myLink->prv = newLink;
	        myLink = newLink;
	    }
	    myLink->prv = t;
	    t->nxt = myLink;
	}
}

// partial evaluation of Lizt_ATTLC::init_all(Lizt_ATTLC())
void
Lizt_ATTLC::init_all_to_empty()
{
	sz = 0;
	t = 0;
	iter_head = 0;
}

void 
Lizt_ATTLC::make_empty()
{
	delete_all();
	init_all_to_empty();
}

Lizt_ATTLC&
Lizt_ATTLC::operator=(const Lizt_ATTLC& a)
{
	if (this == &a) return *this;
	delete_all();
	init_all(a);
	reset_all_iters();
	return *this;
}

void
Lizt_ATTLC::reset_all_iters() 
{ 
	register Liztiter_ATTLC *anIt;
	for (anIt = iter_head; anIt; anIt = anIt->nextIt)
	    anIt->reset0();
}

Lizt_ATTLC&
Lizt_ATTLC::unget(lnk_ATTLC *nl)
{
	add_a_link(nl);

	register Liztiter_ATTLC *anIt;
	for (anIt = iter_head; anIt; anIt = anIt->nextIt) {
	    // myit.cacheNo++;
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

Lizt_ATTLC&
Lizt_ATTLC::unget(const Lizt_ATTLC& ll)
{
	if (ll.sz) {
	    register lnk_ATTLC*    aLink = ll.t;
	    do unget(aLink->copy());
	    while ((aLink = aLink->prv) != ll.t);
	}
	return *this;
}

Lizt_ATTLC&
Lizt_ATTLC::put(lnk_ATTLC *nl)
{
	add_a_link(nl);
	t = nl;
	register Liztiter_ATTLC *anIt;
	for (anIt = iter_head; anIt; anIt = anIt->nextIt) {
	    if (anIt->index==0) {
	        anIt->pred = nl;
	        anIt->index = 0;
	    }
	}
	return *this;
}

Lizt_ATTLC&
Lizt_ATTLC::put(const Lizt_ATTLC& ll)
{
	if (ll.sz) {
	    register lnk_ATTLC*    aLink = ll.head();
	    do put(aLink->copy());
	    while (aLink != ll.t && (aLink = aLink->nxt, 1));
	}
	return *this;
}

lnk_ATTLC*    // removes the link but doesn't destroy it
Lizt_ATTLC::get()
{
	if (!t)
	    return NULL;
	lnk_ATTLC *oh = t->nxt;
	if (t == oh)
	    t = NULL;
	else {
	    lnk_ATTLC *nh = oh->nxt;
	    nh->prv = t;
	    t->nxt = nh;
	}
	sz--;
	register Liztiter_ATTLC *anIt;
	for (anIt = iter_head; anIt; anIt = anIt->nextIt) {
	    if (anIt->cache == oh) anIt->cache = NULL;
	    else anIt->cacheNo--;
	    if (anIt->index <= 1) anIt->pred = t;
	    if (anIt->index > 0) anIt->index--;
	}
	return oh;
}

lnk_ATTLC*    // removes the link but doesn't destroy it
Lizt_ATTLC::unput()
{
	if (!t)
	    return NULL;
	lnk_ATTLC *ot = t;
	lnk_ATTLC *hh = t->nxt;
	if (hh == ot)
	    t = NULL;
	else {
	    lnk_ATTLC *nt = t->prv;
	    hh->prv = t = nt;
	    t->nxt = hh;
	}
	sz--;
	register Liztiter_ATTLC *anIt;
	for (anIt = iter_head; anIt; anIt = anIt->nextIt) {
	    if (anIt->cache == ot) anIt->cache = NULL;
	    if (anIt->pred == ot)    // head or tail
	        anIt->pred = t;
	    if (anIt->index > length()) anIt->index--;
	}
	return ot;
}

lnk_ATTLC*
Lizt_ATTLC::getAt(int ii)
{
	register int count;
	int forward;
	register lnk_ATTLC* from;
	if (ii >= sz) return NULL;
	if (ii >= sz - ii) {
	    count = sz - ii - 1;
	    forward = 0;
	    from = t;
	} else {
	    count = ii;
	    forward = 1;
	    from = head();
	}
	if (forward)
	    while (count--)
	        from = from->nxt;
	else
	    while (count--)
	        from = from->prv;
	return from;
}

// Liztiter_ATTLC operations

void
Liztiter_ATTLC::reset0()
{
	index = 0;
	pred = theLizt->t;
}

void
Liztiter_ATTLC::end_reset(unsigned i)
{
	register unsigned len = theLizt->sz;
	if(i>len) i=len;
	reset(len - i);
}
 
int
Liztiter_ATTLC::at_end() const
{
	return index == theLizt->sz;
}

Liztiter_ATTLC::Liztiter_ATTLC(Lizt_ATTLC& l) : theLizt(&l), cache(0), cacheNo(0)
{
	nextIt = l.iter_head;
	l.iter_head = this;
	reset0();
}

Liztiter_ATTLC::Liztiter_ATTLC(const Liztiter_ATTLC& l) : theLizt(l.theLizt), cache(0), cacheNo(0)
{
	nextIt = (l.theLizt)->iter_head;
	(l.theLizt)->iter_head = this;
	pred = l.pred;
	index = l.index;
}

Liztiter_ATTLC::~Liztiter_ATTLC() 
{ 
	if(theLizt && theLizt->iter_head) {
	    if(this==(theLizt->iter_head))
	        theLizt->iter_head = nextIt;
	    else {
		register Liztiter_ATTLC *anIt;
	        for (
	            anIt = theLizt->iter_head; 
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

Liztiter_ATTLC&
Liztiter_ATTLC::operator=(const Liztiter_ATTLC& l)
{
	if(theLizt && theLizt->iter_head) {
	    if(this==theLizt->iter_head)
	        theLizt->iter_head = nextIt;
	    else {
		register Liztiter_ATTLC *anIt;
	        for (
	            anIt = theLizt->iter_head;
	            anIt->nextIt != this;
	            anIt = anIt->nextIt
	        ) ;
	        anIt->nextIt = nextIt;
	    }
	}
	theLizt = l.theLizt;
	nextIt = (l.theLizt)->iter_head;
	(l.theLizt)->iter_head = this;
	pred = l.pred;
	index = l.index;
	return *this;
}

lnk_ATTLC*
Liztiter_ATTLC::getAt(int ii)
{
	register int count;
	int forward;
	register lnk_ATTLC* from;
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
Liztiter_ATTLC::reset(unsigned i)
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
Liztiter_ATTLC::insert_prev(lnk_ATTLC* nl)
{
	if (pred) {
	    nl->init(pred, pred->nxt);
	    pred->nxt->prv = nl;
	    pred->nxt = nl;
	    register Liztiter_ATTLC *anIt;
	    for (anIt=(theLizt->iter_head); anIt; anIt=anIt->nextIt) {
	        if(this!=anIt && index < anIt->index) anIt->index++;
	    }
	} 
	else {
	    nl->init(nl, nl);
	    register Liztiter_ATTLC *anIt;
	    for (anIt=(theLizt->iter_head); anIt; anIt=anIt->nextIt) {
	        anIt->pred = nl;
	    }
	}
	if (at_end()) theLizt->t = nl;
	pred = nl;
	index++;
	theLizt->sz++;
}

void
Liztiter_ATTLC::insert_next(lnk_ATTLC* nl)
{
	register Liztiter_ATTLC *anIt;
	for (anIt=(theLizt->iter_head); anIt; anIt=anIt->nextIt) {
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

lnk_ATTLC*
Liztiter_ATTLC::remove_prev()
{
	if (at_head())
	    return NULL;
	lnk_ATTLC *doomed = pred;
	int oldIndex = index;
	if (pred == pred->nxt) {    // this is only item
	    theLizt->t = 0;
	    register Liztiter_ATTLC *anIt;
	    for (anIt=(theLizt->iter_head); anIt; anIt=anIt->nextIt) {
	        anIt->pred = 0;
	        anIt->index = 0;
	    }
	} else {
	    if (doomed == theLizt->t)    // deleting tail
	        theLizt->t = doomed->prv;
	    register Liztiter_ATTLC *anIt;
	    for (anIt=(theLizt->iter_head); anIt; anIt=anIt->nextIt) {
	        if(anIt->pred==doomed) anIt->pred = doomed->prv;
	        if(anIt->index >= oldIndex) anIt->index--;
	    }
	    pred->nxt = doomed->nxt;
	    doomed->nxt->prv = pred;
	}
	theLizt->sz--;
	return doomed;
}

lnk_ATTLC*
Liztiter_ATTLC::remove_next()
{
	if (at_end())
	    return NULL;

	lnk_ATTLC *doomed = pred->nxt;
	if (pred == pred->nxt) {    // this is only item
	    theLizt->t = 0;
	    register Liztiter_ATTLC *anIt;
	    for (anIt=(theLizt->iter_head); anIt; anIt=anIt->nextIt) {
	        anIt->pred = 0;
	        anIt->index = 0;
	    }
	} else {
	    if (doomed == theLizt->t)    // deleting tail
	        theLizt->t = doomed->prv;
	    register Liztiter_ATTLC *anIt;
	    for (anIt=(theLizt->iter_head); anIt; anIt=anIt->nextIt) {
	        if(anIt->pred == doomed) anIt->pred = doomed->prv;
	        if(anIt->index > index) anIt->index--;
	    }
	    pred->nxt = doomed->nxt;
	    doomed->nxt->prv = pred;
	}
	theLizt->sz--;
	return doomed;
}

lnk_ATTLC*
Liztiter_ATTLC::next()
{
	if (at_end()) return NULL;
	pred = pred->nxt;
	index++;
	return pred;
}

lnk_ATTLC*
Liztiter_ATTLC::prev()
{
	if (at_head()) return NULL;
	lnk_ATTLC *op = pred;
	pred = pred->prv;
	index--;
	return op;
}

