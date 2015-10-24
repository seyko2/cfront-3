/*ident	"@(#)List:List_voidP.c	3.1" */
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

#ifdef __GNUG__
#include <List.h>
/*
 * This file contains the implementation of the classes
 * voidP_List, voidP_lnnk_ATTLC, voidP_Const_listiter,
 * and voidP_Listiter -- which are only defined for
 * GNU G++ because of problems controlling the
 * instantiation of List<void*>.
 *
 * Dennis Mancl - Feb. 26, 1997
 */
class	voidP_lnnk_ATTLC : public lnk_ATTLC {

	friend class voidP_List;
	friend class voidP_Const_listiter;
	friend class voidP_Listiter;
	friend void voidP_List_sort_internal(voidP_List&,
				int (*)(const voidP &, const voidP &));

#ifndef __GNUG__
	static	Pool* pool;
#endif
	void*	val;

	voidP_lnnk_ATTLC(voidP& pp) : val(pp) {}
	~voidP_lnnk_ATTLC();

	lnk_ATTLC* copy();
	int	operator==(lnk_ATTLC&);

    public:
	void*	operator new(size_t);
#ifdef __GNUG__
	void	operator delete(void* l) { pool()->free(l); }
	static	Pool *pool();
#else
	void	operator delete(void* l) { pool->free(l); }
	static	void init_pool() {	// should be called by List constructors
			if (pool == 0)
			    pool = new Pool(sizeof(voidP_lnnk_ATTLC));
		}
#endif

	static	lnk_ATTLC* getnewlnnk_ATTLC(const voidP&);
	static	void deletelnnk_ATTLC(voidP&, voidP_lnnk_ATTLC*);
};

#ifdef __GNUG__
Pool *voidP_lnnk_ATTLC::pool() {
	Pool *lcl_pool = 0;
	if (lcl_pool == 0) {
		lcl_pool = new Pool(sizeof(voidP_lnnk_ATTLC));
	}
	return lcl_pool;
}
#else
Pool* voidP_lnnk_ATTLC::pool = 0;
#endif

voidP_lnnk_ATTLC::~voidP_lnnk_ATTLC()
{
}

lnk_ATTLC*
voidP_lnnk_ATTLC::copy()
{
	return new voidP_lnnk_ATTLC((voidP&)val);
}

int
voidP_lnnk_ATTLC::operator==(lnk_ATTLC& x)
{
	return val == ((voidP_lnnk_ATTLC*)&x)->val;
}

void* 
voidP_lnnk_ATTLC::operator new(size_t)
{
#ifdef __GNUG__
	return pool()->alloc();
#else
	return pool->alloc();
#endif
}

lnk_ATTLC*
voidP_lnnk_ATTLC::getnewlnnk_ATTLC(const voidP& x)
{
	return (new voidP_lnnk_ATTLC((voidP&)x));
}

void
voidP_lnnk_ATTLC::deletelnnk_ATTLC(voidP& t, voidP_lnnk_ATTLC* ptr)
{
	t = ptr->val;
	delete ptr;
}

voidP_List::voidP_List(const voidP_List& a0, const voidP& _t) : Lizt_ATTLC((Lizt_ATTLC&)a0)
{
	put(_t);
}

voidP_List::voidP_List()
{
#ifndef __GNUG__
	voidP_lnnk_ATTLC::init_pool(); 
#endif
}

voidP_List::voidP_List(const voidP& _t)
{
#ifndef __GNUG__
	voidP_lnnk_ATTLC::init_pool();
#endif
	put(_t);
}

voidP_List::voidP_List(const voidP& _t, const voidP& u)
{
#ifndef __GNUG__
	voidP_lnnk_ATTLC::init_pool();
#endif
	put(_t);
	put(u);
}

voidP_List::voidP_List(const voidP& _t, const voidP& u, const voidP& v)
{
#ifndef __GNUG__
	voidP_lnnk_ATTLC::init_pool();
#endif
	put(_t);
	put(u);
	put(v);
}

voidP_List::voidP_List(const voidP& _t, const voidP& u, const voidP& v, const voidP& w)
{
#ifndef __GNUG__
	voidP_lnnk_ATTLC::init_pool();
#endif
	put(_t);
	put(u);
	put(v);
	put(w);
}

voidP_List::voidP_List(const voidP_List& a) : Lizt_ATTLC((const Lizt_ATTLC&)a)
{
#ifndef __GNUG__
	voidP_lnnk_ATTLC::init_pool(); 
#endif
}

void* 
voidP_List::unput()
{ 
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Lizt_ATTLC::unput();
	void* ans = ll->val; 
	delete ll; 
	return ans; 
}

void* 
voidP_List::get()
{ 
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Lizt_ATTLC::get();
	// if (ll == 0)
	//	return 0;
	void* ans = ll->val; 
	delete ll; 
	return ans; 
}

void**
voidP_List::getAt(int i)
{ 
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Lizt_ATTLC::getAt(i);
	if ( ll )
		return &(ll->val);
	else
		return (void**)0;
}

voidP&
voidP_List::operator[](unsigned ii)
{
	return (voidP&)*(getAt(ii));
}

const voidP&
voidP_List::operator[](unsigned ii) const
{
	return (const voidP&)*(((voidP_List*)this)->getAt(ii));
}

void**
voidP_List::head() const
{
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Lizt_ATTLC::head();
	if (ll == 0)
		return 0;
	return &(ll->val);
}

void**
voidP_List::tail() const
{
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Lizt_ATTLC::tail();
	if (ll == 0)
		return 0;
	return &(ll->val);
}

void
voidP_List::sort(int (*cmp)(const voidP&,const voidP&))
{
	if ( length() < 2 )
		return;

	voidP_List_sort_internal(*(voidP_List*)this, (int (*)(const voidP &, const voidP &))cmp);
	reset_all_iters();
}

int
voidP_List::unput(voidP& _t)
{
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Lizt_ATTLC::unput();
	if ( ll )
	{ 
		voidP_lnnk_ATTLC::deletelnnk_ATTLC(_t, ll);
		return 1;
	}
	else
		return 0;
}

int
voidP_List::get(voidP& _t)
{ 
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Lizt_ATTLC::get();
	if ( ll )
	{ 
		voidP_lnnk_ATTLC::deletelnnk_ATTLC(_t, ll);
		return 1;
	}
	else
		return 0;
}

voidP_List&
voidP_List::put(const voidP& x)
{
	return (voidP_List&) Lizt_ATTLC::put(voidP_lnnk_ATTLC::getnewlnnk_ATTLC(x));
}

voidP_List&
voidP_List::unget(const voidP& x)
{ 
	return (voidP_List&)Lizt_ATTLC::unget(voidP_lnnk_ATTLC::getnewlnnk_ATTLC(x));
}

voidP_Const_listiter::voidP_Const_listiter(const voidP_List& a) : Liztiter_ATTLC((Lizt_ATTLC&)a)
{
}

voidP_Const_listiter::voidP_Const_listiter(const voidP_Const_listiter& a) : Liztiter_ATTLC((const Liztiter_ATTLC&)a)
{
}

int
voidP_Const_listiter::next(voidP& t)
{
	if ( at_end() )
		return 0;
	else
		return (t = ((voidP_lnnk_ATTLC*)Liztiter_ATTLC::next())->val, 1);
}

int
voidP_Const_listiter::next(voidP*& t)
{
	if ( at_end() )
		return 0;
	else
		return (t = &((voidP_lnnk_ATTLC*)Liztiter_ATTLC::next())->val, 1);
}

void**
voidP_Const_listiter::next()
{
	if ( at_end() )
		return 0;
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Liztiter_ATTLC::next();
	return &(ll->val);
}

int
voidP_Const_listiter::prev(voidP& t)
{
	if ( at_head() )
		return 0;
	else
		return (t=((voidP_lnnk_ATTLC*)Liztiter_ATTLC::prev())->val, 1);
}

int
voidP_Const_listiter::prev(voidP*& t)
{
	if ( at_head() )
		return 0;
	else
		return (t= &((voidP_lnnk_ATTLC*)Liztiter_ATTLC::prev())->val, 1);
}

void**
voidP_Const_listiter::prev()
{
	if ( at_head() )
		return 0;
	voidP_lnnk_ATTLC* ll = (voidP_lnnk_ATTLC*)Liztiter_ATTLC::prev();
	return &(ll->val);
}

int
voidP_Const_listiter::find_prev(const voidP& x)
{
	if ( at_head() || theLizt->length()==0 )
		return 0;

	voidP_lnnk_ATTLC* iter = (voidP_lnnk_ATTLC*) pred->nxt;
	register int i = index;
	do {
		iter = (voidP_lnnk_ATTLC*) iter->prv;
		if (iter->val==x) {
			index = i;
			pred = iter;
			return 1;
		}
		i--;
	} while ( i > 0 );
	return 0;
}

int
voidP_Const_listiter::find_next(const voidP& x)
{
	if ( at_end() || theLizt->length()==0 )
		return 0;

	voidP_lnnk_ATTLC* iter = (voidP_lnnk_ATTLC*) pred;
	register int i = index;
	do {
		iter = (voidP_lnnk_ATTLC*) iter->nxt;
		if (iter->val==x) {
			index = i;
			pred = iter->prv;
			return 1;
		}
		i++;
	} while ( i < theLizt->length() );
	return 0;
}

void**
voidP_Const_listiter::peek_prev() const
{
	if ( at_head() )
	 	return 0;
	return &(((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_prev())->val);
}

int
voidP_Const_listiter::peek_prev(voidP& t) const
{
	if ( at_head() )
		return 0;
	else
		return (t = ((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_prev())->val, 1);
}

int
voidP_Const_listiter::peek_prev(voidP*& t) const
{
	if ( at_head() )
		return 0;
	else
		return (t = &((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_prev())->val, 1);
}

void**
voidP_Const_listiter::peek_next() const
{
	if ( at_end() )
	 	return 0;
	return &(((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_next())->val);
}

int
voidP_Const_listiter::peek_next(voidP& t) const
{
	if ( at_end() )
		return 0;
	else
		return (t = ((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_next())->val, 1);
}

int
voidP_Const_listiter::peek_next(voidP*& t) const
{
	if ( at_end() )
		return 0;
	else
		return (t = &((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_next())->val, 1);
}

void**
voidP_Const_listiter::getAt(int i)
{
	voidP_lnnk_ATTLC* ll = ((voidP_lnnk_ATTLC*)Liztiter_ATTLC::getAt(i));
	if ( ll )
		return &(ll->val);
	else
		return (void**)0;
}

voidP_Listiter::voidP_Listiter(voidP_List& a) : voidP_Const_listiter((const voidP_List&)a)
{
}

voidP_Listiter::voidP_Listiter(const voidP_Listiter& a) : voidP_Const_listiter((voidP_Const_listiter)a)
{
}

int
voidP_Listiter::remove_prev()
{
	voidP_lnnk_ATTLC *aLink = (voidP_lnnk_ATTLC *)Liztiter_ATTLC::remove_prev();
	if ( aLink ) {
		delete aLink;
		return 1;
	} else
		return 0;
}

int
voidP_Listiter::remove_prev(voidP &x)
{
	voidP_lnnk_ATTLC *aLink = (voidP_lnnk_ATTLC *)Liztiter_ATTLC::remove_prev();
	if ( aLink ) {
		x = aLink->val;
		delete aLink;
		return 1;
	} else
		return 0;
}

int
voidP_Listiter::remove_next()
{
	voidP_lnnk_ATTLC *aLink = (voidP_lnnk_ATTLC *)Liztiter_ATTLC::remove_next();
	if ( aLink ) {
		delete aLink;
		return 1;
	} else
		return 0;
}

int
voidP_Listiter::remove_next(voidP &x)
{
	voidP_lnnk_ATTLC *aLink = (voidP_lnnk_ATTLC *)Liztiter_ATTLC::remove_next();
	if ( aLink ) {
		x = aLink->val;
		delete aLink;
		return 1;
	} else
		return 0;
}

int
voidP_Listiter::replace_prev(const voidP& x)
{
	if ( at_head() )
		return 0;
	else {
		((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_prev())->val = x;
		return 1;
	}
}

int
voidP_Listiter::replace_next(const voidP& x)
{
	if ( at_end() )
		return 0;
	else {
		((voidP_lnnk_ATTLC*)Liztiter_ATTLC::peek_next())->val = x;
		return 1;
	}
}

void 
voidP_Listiter::insert_prev(const voidP& x)
{
	Liztiter_ATTLC::insert_prev(voidP_lnnk_ATTLC::getnewlnnk_ATTLC(x));
}

void 
voidP_Listiter::insert_next(const voidP& x)
{
	Liztiter_ATTLC::insert_next(voidP_lnnk_ATTLC::getnewlnnk_ATTLC(x));
}

ostream&
operator<<(ostream& oo, const voidP_List& ll)
{
	int first = 1;
	oo << "( ";
	voidP_Const_listiter l(ll);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << *(l.next());
	}
	oo << " )";
	return oo;
}

ostream&
voidP_List::print(ostream& oo) const
{
	int first = 1;
	oo << "( ";
	voidP_Const_listiter l(*this);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << *(l.next());
	}
	oo << " )";
	return oo;
}

void
voidP_List_sort_internal(voidP_List& aList, int (*lessThan)(const voidP &, const voidP &))
{
    const int logN = 32;    // max capacity will be 2^logN
    register voidP_lnnk_ATTLC*    temp;
    register voidP_lnnk_ATTLC*    newCh;
    register voidP_lnnk_ATTLC*    oldCh;
    voidP_lnnk_ATTLC*    bitPos[logN];
    voidP_lnnk_ATTLC**    bitPtr;
    voidP_lnnk_ATTLC**    bitPtrMax = &bitPos[0];
    for (bitPtr = &bitPos[0]; bitPtr < &bitPos[logN]; *bitPtr++ = 0) ;
    voidP_lnnk_ATTLC* nextPtr = aList.t ? (voidP_lnnk_ATTLC*) aList.t->nxt: 0;
    aList.t->nxt = 0;
    voidP_lnnk_ATTLC*    ans;
    while (newCh = nextPtr) {
        nextPtr = (voidP_lnnk_ATTLC*)nextPtr->nxt;
        newCh->nxt = 0;
        for (bitPtr = &bitPos[0];; bitPtr++) {
            if (bitPtr > bitPtrMax) bitPtrMax = bitPtr;
            if (*bitPtr == 0) {
                *bitPtr = newCh;
                break;
            }
            oldCh = *bitPtr;
            *bitPtr = 0;
            if (!(*lessThan)(newCh->val, oldCh->val)) {
                ans = oldCh;
                for(;;) {
                    while ((temp = (voidP_lnnk_ATTLC*)oldCh->nxt) &&
                        !(*lessThan)(newCh->val, temp->val))
                        oldCh = temp;
                    oldCh->nxt = newCh;
                    if ((oldCh = temp) == 0) {
                        newCh = ans;
                        break;
                    }
bMerge:
                    while ((temp = (voidP_lnnk_ATTLC*)newCh->nxt) &&
                        (*lessThan)(temp->val, oldCh->val))
                        newCh = temp;
                    newCh->nxt = oldCh;
                    if ((newCh = temp) == 0) {
                        newCh = ans;
                        break;
                    }
                }
            } else {
                ans = newCh;
                goto bMerge;
            }
        }
    }
    // final merge sweep
    voidP_lnnk_ATTLC**    bPtr2;
    for (bitPtr = &bitPos[0];; bitPtr = bPtr2) {
        while (*bitPtr == 0) bitPtr++;
        if (bitPtr == bitPtrMax) break;
        for (bPtr2 = bitPtr + 1; *bPtr2 == 0; bPtr2++) ;
        oldCh = *bPtr2;
        newCh = *bitPtr;
        if (!(*lessThan)(newCh->val, oldCh->val)) {
            ans = oldCh;
            for(;;) {
                while ((temp = (voidP_lnnk_ATTLC*)oldCh->nxt) &&
                    !(*lessThan)(newCh->val, temp->val))
                    oldCh = temp;
                oldCh->nxt = newCh;
                if ((oldCh = temp) == 0) {
                    newCh = ans;
                    break;
                }
eMerge:
                while ((temp = (voidP_lnnk_ATTLC*)newCh->nxt) &&
                    (*lessThan)(temp->val, oldCh->val))
                    newCh = temp;
                newCh->nxt = oldCh;
                if ((newCh = temp) == 0) {
                    newCh = ans;
                    break;
                }
            }
        } else {
            ans = newCh;
            goto eMerge;
        }
        *bPtr2 = ans;
    }
    for (newCh = *bitPtr; newCh->nxt; newCh = (voidP_lnnk_ATTLC*)newCh->nxt)
        newCh->nxt->prv = newCh;
    newCh->nxt = *bitPtr;
    (*bitPtr)->prv = newCh;
    aList.t = newCh;
}
#endif
