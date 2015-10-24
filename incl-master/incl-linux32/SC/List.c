/*ident	"@(#)List.c	1.1.2.4" */
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

#ifndef _LIST_C_
#define _LIST_C_

#include <List.h>
#include <stream.h>

#ifndef __GNUG__
#ifndef voidP_List
#define voidP_List List<voidP>
#define voidP_Listiter Listiter<voidP>
#endif
#endif

#ifdef __GNUG__
template <class T>
Pool *lnnk_ATTLC<T>::pool() {
	Pool *lcl_pool = 0;
	if (lcl_pool == 0) {
		lcl_pool = new Pool(sizeof(lnnk_ATTLC<T>));
	}
	return lcl_pool;
}
#else
template <class T>
Pool* lnnk_ATTLC<T>::pool = 0;
#endif

#ifndef __GNUG__
template <class T>
lnnk_ATTLC<T>::~lnnk_ATTLC()
{
}

template <class T>
lnk_ATTLC*
lnnk_ATTLC<T>::copy()
{
	return new lnnk_ATTLC<T>((T&)val);
}

template <class T>
int
lnnk_ATTLC<T>::operator==(lnk_ATTLC& x)
{
	return val == ((lnnk_ATTLC<T>*)&x)->val;
}
#endif

template <class T>
void* 
lnnk_ATTLC<T>::operator new(size_t)
{
#ifdef __GNUG__
	return pool()->alloc();
#else
	return pool->alloc();
#endif
}

template <class T>
lnk_ATTLC*
lnnk_ATTLC<T>::getnewlnnk_ATTLC(const T& x)
{
	return (new lnnk_ATTLC<T>((T&)x));
}

template <class T>
void
lnnk_ATTLC<T>::deletelnnk_ATTLC(T& t, lnnk_ATTLC<T>* ptr)
{
	t = ptr->val;
	delete ptr;
}

template <class T>
List<T>::List(const List<T>& a0, const T& _t) : Lizt_ATTLC((Lizt_ATTLC&)a0)
{
	put(_t);
}

template <class T>
List<T>::List()
{
#ifndef __GNUG__
	lnnk_ATTLC<T>::init_pool(); 
#endif
}

template <class T>
List<T>::List(const T& _t)
{
#ifndef __GNUG__
	lnnk_ATTLC<T>::init_pool();
#endif
	put(_t);
}

template <class T>
List<T>::List(const T& _t, const T& u)
{
#ifndef __GNUG__
	lnnk_ATTLC<T>::init_pool();
#endif
	put(_t);
	put(u);
}

template <class T>
List<T>::List(const T& _t, const T& u, const T& v)
{
#ifndef __GNUG__
	lnnk_ATTLC<T>::init_pool();
#endif
	put(_t);
	put(u);
	put(v);
}

template <class T>
List<T>::List(const T& _t, const T& u, const T& v, const T& w)
{
#ifndef __GNUG__
	lnnk_ATTLC<T>::init_pool();
#endif
	put(_t);
	put(u);
	put(v);
	put(w);
}

template <class T>
List<T>::List(const List<T>& a) : Lizt_ATTLC((const Lizt_ATTLC&)a)
{
#ifndef __GNUG__
	lnnk_ATTLC<T>::init_pool(); 
#endif
}

template <class T>
T 
List<T>::unput()
{ 
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Lizt_ATTLC::unput();
	T ans = ll->val; 
	delete ll; 
	return ans; 
}

template <class T>
T 
List<T>::get()
{ 
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Lizt_ATTLC::get();
	// if (ll == 0)
	//	return 0;
	T ans = ll->val; 
	delete ll; 
	return ans; 
}

template <class T>
T*
List<T>::getAt(int i)
{ 
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Lizt_ATTLC::getAt(i);
	if ( ll )
		return &(ll->val);
	else
		return (T*)0;
}

template <class T>
T&
List<T>::operator[](unsigned ii)
{
	return (T&)*(getAt(ii));
}

template <class T>
const T&
List<T>::operator[](unsigned ii) const
{
	return (const T&)*(((List<T>*)this)->getAt(ii));
}

template <class T>
T*
List<T>::head() const
{
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Lizt_ATTLC::head();
	if (ll == 0)
		return 0;
	return &(ll->val);
}

template <class T>
T*
List<T>::tail() const
{
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Lizt_ATTLC::tail();
	if (ll == 0)
		return 0;
	return &(ll->val);
}

template <class T>
void
List<T>::sort(int (*cmp)(const T&,const T&))
{
	if ( length() < 2 )
		return;

	voidP_List_sort_internal(*(List<T>*)this, (int (*)(const T &, const T &))cmp);
	reset_all_iters();
}

template <class T>
int
List<T>::unput(T& _t)
{
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Lizt_ATTLC::unput();
	if ( ll )
	{ 
		lnnk_ATTLC<T>::deletelnnk_ATTLC(_t, ll);
		return 1;
	}
	else
		return 0;
}

template <class T>
int
List<T>::get(T& _t)
{ 
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Lizt_ATTLC::get();
	if ( ll )
	{ 
		lnnk_ATTLC<T>::deletelnnk_ATTLC(_t, ll);
		return 1;
	}
	else
		return 0;
}

template <class T>
List<T>&
List<T>::put(const T& x)
{
	return (List<T>&) Lizt_ATTLC::put(lnnk_ATTLC<T>::getnewlnnk_ATTLC(x));
}

template <class T>
List<T>&
List<T>::unget(const T& x)
{ 
	return (List<T>&)Lizt_ATTLC::unget(lnnk_ATTLC<T>::getnewlnnk_ATTLC(x));
}

template <class T>
Const_listiter<T>::Const_listiter(const List<T>& a) : Liztiter_ATTLC((Lizt_ATTLC&)a)
{
}

template <class T>
Const_listiter<T>::Const_listiter(const Const_listiter<T>& a) : Liztiter_ATTLC((const Liztiter_ATTLC&)a)
{
}

template <class T>
int
Const_listiter<T>::next(T& t)
{
	if ( at_end() )
		return 0;
	else
		return (t = ((lnnk_ATTLC<T>*)Liztiter_ATTLC::next())->val, 1);
}

template <class T>
int
Const_listiter<T>::next(T*& t)
{
	if ( at_end() )
		return 0;
	else
		return (t = &((lnnk_ATTLC<T>*)Liztiter_ATTLC::next())->val, 1);
}

template <class T>
T*
Const_listiter<T>::next()
{
	if ( at_end() )
		return 0;
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Liztiter_ATTLC::next();
	return &(ll->val);
}

template <class T>
int
Const_listiter<T>::prev(T& t)
{
	if ( at_head() )
		return 0;
	else
		return (t=((lnnk_ATTLC<T>*)Liztiter_ATTLC::prev())->val, 1);
}

template <class T>
int
Const_listiter<T>::prev(T*& t)
{
	if ( at_head() )
		return 0;
	else
		return (t= &((lnnk_ATTLC<T>*)Liztiter_ATTLC::prev())->val, 1);
}

template <class T>
T*
Const_listiter<T>::prev()
{
	if ( at_head() )
		return 0;
	lnnk_ATTLC<T>* ll = (lnnk_ATTLC<T>*)Liztiter_ATTLC::prev();
	return &(ll->val);
}

template <class T>
int
Const_listiter<T>::find_prev(const T& x)
{
	if ( at_head() || theLizt->length()==0 )
		return 0;

	lnnk_ATTLC<T>* iter = (lnnk_ATTLC<T>*) pred->nxt;
	register int i = index;
	do {
		iter = (lnnk_ATTLC<T>*) iter->prv;
		if (iter->val==x) {
			index = i;
			pred = iter;
			return 1;
		}
		i--;
	} while ( i > 0 );
	return 0;
}

template <class T>
int
Const_listiter<T>::find_next(const T& x)
{
	if ( at_end() || theLizt->length()==0 )
		return 0;

	lnnk_ATTLC<T>* iter = (lnnk_ATTLC<T>*) pred;
	register int i = index;
	do {
		iter = (lnnk_ATTLC<T>*) iter->nxt;
		if (iter->val==x) {
			index = i;
			pred = iter->prv;
			return 1;
		}
		i++;
	} while ( i < theLizt->length() );
	return 0;
}

template <class T>
T*
Const_listiter<T>::peek_prev() const
{
	if ( at_head() )
	 	return 0;
	return &(((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_prev())->val);
}

template <class T>
int
Const_listiter<T>::peek_prev(T& t) const
{
	if ( at_head() )
		return 0;
	else
		return (t = ((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_prev())->val, 1);
}

template <class T>
int
Const_listiter<T>::peek_prev(T*& t) const
{
	if ( at_head() )
		return 0;
	else
		return (t = &((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_prev())->val, 1);
}

template <class T>
T*
Const_listiter<T>::peek_next() const
{
	if ( at_end() )
	 	return 0;
	return &(((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_next())->val);
}

template <class T>
int
Const_listiter<T>::peek_next(T& t) const
{
	if ( at_end() )
		return 0;
	else
		return (t = ((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_next())->val, 1);
}

template <class T>
int
Const_listiter<T>::peek_next(T*& t) const
{
	if ( at_end() )
		return 0;
	else
		return (t = &((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_next())->val, 1);
}

template <class T>
T*
Const_listiter<T>::getAt(int i)
{
	lnnk_ATTLC<T>* ll = ((lnnk_ATTLC<T>*)Liztiter_ATTLC::getAt(i));
	if ( ll )
		return &(ll->val);
	else
		return (T*)0;
}

template <class T>
Listiter<T>::Listiter(List<T>& a) : Const_listiter<T>((const List<T>&)a)
{
}

template <class T>
Listiter<T>::Listiter(const Listiter<T>& a) : Const_listiter<T>((Const_listiter<T>)a)
{
}

template <class T>
int
Listiter<T>::remove_prev()
{
	lnnk_ATTLC<T> *aLink = (lnnk_ATTLC<T> *)Liztiter_ATTLC::remove_prev();
	if ( aLink ) {
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter<T>::remove_prev(T &x)
{
	lnnk_ATTLC<T> *aLink = (lnnk_ATTLC<T> *)Liztiter_ATTLC::remove_prev();
	if ( aLink ) {
		x = aLink->val;
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter<T>::remove_next()
{
	lnnk_ATTLC<T> *aLink = (lnnk_ATTLC<T> *)Liztiter_ATTLC::remove_next();
	if ( aLink ) {
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter<T>::remove_next(T &x)
{
	lnnk_ATTLC<T> *aLink = (lnnk_ATTLC<T> *)Liztiter_ATTLC::remove_next();
	if ( aLink ) {
		x = aLink->val;
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter<T>::replace_prev(const T& x)
{
	if ( at_head() )
		return 0;
	else
		return (((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_prev())->val=x,1);
}

template <class T>
int
Listiter<T>::replace_next(const T& x)
{
	if ( at_end() )
		return 0;
	else
		return (((lnnk_ATTLC<T>*)Liztiter_ATTLC::peek_next())->val=x,1);
}

template <class T>
void 
Listiter<T>::insert_prev(const T& x)
{
	Liztiter_ATTLC::insert_prev(lnnk_ATTLC<T>::getnewlnnk_ATTLC(x));
}

template <class T>
void 
Listiter<T>::insert_next(const T& x)
{
	Liztiter_ATTLC::insert_next(lnnk_ATTLC<T>::getnewlnnk_ATTLC(x));
}

template <class T>
List_of_piter<T>::List_of_piter(List_of_p<T>& l) : 
	Const_list_of_piter<T>((const List_of_p<T>&)l)
{
}

template <class T>
List_of_piter<T>::List_of_piter(const List_of_piter<T>& l) : 
	Const_list_of_piter<T>((Const_list_of_piter<T>)l)
{
}

template <class T>
List_of_piter<T>::~List_of_piter()
{
}

template <class T>
List_of_p<T>::List_of_p(const T* x) : voidP_List((voidP) x)
{
}

template <class T>
List_of_p<T>::List_of_p(const T* x, const T* y) :
	voidP_List((voidP) x, (voidP) y)
{
}

template <class T>
List_of_p<T>::List_of_p(const T* x, const T* y, const T* z) :
	voidP_List((voidP)x, (voidP)y, (voidP)z)
{
}

template <class T>
List_of_p<T>::List_of_p(const T* x, const T* y, const T* z, const T* w) :
	voidP_List((voidP) x, (voidP) y, (voidP) z, (voidP) w)
{
}

template <class T>
Const_list_of_piter<T>::Const_list_of_piter(const List_of_p<T>& l) :
	voidP_Listiter((voidP_List&) l)
{
}

template <class T>
Const_list_of_piter<T>::Const_list_of_piter(const Const_list_of_piter<T>& l) :
	voidP_Listiter((voidP_Listiter&) l)
{
}

template <class T>
List_of_p<T> List_of_p<T>::operator+(const List_of_p<T>& ll)
{
	voidP_List result((voidP_List&)*this + (voidP_List&)ll);
	return (List_of_p<T>&)result;
}

template <class T>
List_of_p<T> List_of_p<T>::operator+(const T* _t)
{
	voidP_List result((voidP_List&)*this + (voidP)_t);
	return (List_of_p<T>&)result;
}

template <class T>
T*&
List_of_p<T>::operator[](unsigned ii)
{
	return (T*&)*getAt(ii);
}

template <class T>
const T*&
List_of_p<T>::operator[](unsigned ii) const
{
	return (const T*&)*((List_of_p<T>*)this)->getAt(ii);
}

template <class T>
ostream&
operator<<(ostream& oo, const List<T>& ll)
{
	int first = 1;
	oo << "( ";
	Const_listiter<T> l(ll);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << *(l.next());
	}
	oo << " )";
	return oo;
}

template <class T>
ostream&
List<T>::print(ostream& oo) const
{
	int first = 1;
	oo << "( ";
	Const_listiter<T> l(*this);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << *(l.next());
	}
	oo << " )";
	return oo;
}

template <class T>
ostream&
operator<<(ostream& oo, const List_of_p<T>& ll)
{
	int first = 1;
	oo << "( ";
	Const_list_of_piter<T> l(ll);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << *(l.next());
	}
	oo << " )";
	return oo;
}

template <class T>
ostream&
List_of_p<T>::print(ostream& oo) const
{
	int first = 1;
	oo << "( ";
	Const_list_of_piter<T> l(*this);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << *(l.next());
	}
	oo << " )";
	return oo;
}

template <class T>
void
voidP_List_sort_internal(List<T>& aList, int (*lessThan)(const T &, const T &))
{
    const int logN = 32;    // max capacity will be 2^logN
    register lnnk_ATTLC<T>*    temp;
    register lnnk_ATTLC<T>*    newCh;
    register lnnk_ATTLC<T>*    oldCh;
    lnnk_ATTLC<T>*    bitPos[logN];
    lnnk_ATTLC<T>**    bitPtr;
    lnnk_ATTLC<T>**    bitPtrMax = &bitPos[0];
    for (bitPtr = &bitPos[0]; bitPtr < &bitPos[logN]; *bitPtr++ = 0) ;
    lnnk_ATTLC<T>* nextPtr = aList.t ? (lnnk_ATTLC<T>*) aList.t->nxt: 0;
    aList.t->nxt = 0;
    lnnk_ATTLC<T>*    ans;
    while (newCh = nextPtr) {
        nextPtr = (lnnk_ATTLC<T>*)nextPtr->nxt;
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
                    while ((temp = (lnnk_ATTLC<T>*)oldCh->nxt) &&
                        !(*lessThan)(newCh->val, temp->val))
                        oldCh = temp;
                    oldCh->nxt = newCh;
                    if ((oldCh = temp) == 0) {
                        newCh = ans;
                        break;
                    }
bMerge:
                    while ((temp = (lnnk_ATTLC<T>*)newCh->nxt) &&
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
    lnnk_ATTLC<T>**    bPtr2;
    for (bitPtr = &bitPos[0];; bitPtr = bPtr2) {
        while (*bitPtr == 0) bitPtr++;
        if (bitPtr == bitPtrMax) break;
        for (bPtr2 = bitPtr + 1; *bPtr2 == 0; bPtr2++) ;
        oldCh = *bPtr2;
        newCh = *bitPtr;
        if (!(*lessThan)(newCh->val, oldCh->val)) {
            ans = oldCh;
            for(;;) {
                while ((temp = (lnnk_ATTLC<T>*)oldCh->nxt) &&
                    !(*lessThan)(newCh->val, temp->val))
                    oldCh = temp;
                oldCh->nxt = newCh;
                if ((oldCh = temp) == 0) {
                    newCh = ans;
                    break;
                }
eMerge:
                while ((temp = (lnnk_ATTLC<T>*)newCh->nxt) &&
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
    for (newCh = *bitPtr; newCh->nxt; newCh = (lnnk_ATTLC<T>*)newCh->nxt)
        newCh->nxt->prv = newCh;
    newCh->nxt = *bitPtr;
    (*bitPtr)->prv = newCh;
    aList.t = newCh;
}

#endif
