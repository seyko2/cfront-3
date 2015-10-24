/*ident	"@(#)List_old.c	1.1.2.5" */
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
#include <stream.h>

#ifdef __GNUG__
template <class T>
Pool *lnnk_old_ATTLC<T>::pool() {
	Pool *lcl_pool = 0;
	if (lcl_pool == 0) {
		lcl_pool = new Pool(sizeof(lnnk_old_ATTLC<T>));
	}
	return lcl_pool;
}
#else
template <class T>
Pool* lnnk_old_ATTLC<T>::pool = 0;
#endif

template <class T>
lnnk_old_ATTLC<T>::~lnnk_old_ATTLC()
{
}

template <class T>
lnk_old_ATTLC*
lnnk_old_ATTLC<T>::copy()
{
	return new lnnk_old_ATTLC<T>((T&)val);
}

template <class T>
int
lnnk_old_ATTLC<T>::operator==(lnk_old_ATTLC& x)
{
	return val == ((lnnk_old_ATTLC<T>*)&x)->val;
}

template <class T>
void* 
lnnk_old_ATTLC<T>::operator new(size_t)
{
#ifdef __GNUG__
	return pool()->alloc();
#else
	return pool->alloc();
#endif
}

template <class T>
lnk_old_ATTLC*
lnnk_old_ATTLC<T>::getnewlnnk_old_ATTLC(const T& x)
{
	return (new lnnk_old_ATTLC<T>((T&)x));
}

template <class T>
void
lnnk_old_ATTLC<T>::deletelnnk_old_ATTLC(T& t, lnnk_old_ATTLC<T>* ptr)
{
	t = ptr->val;
	delete ptr;
}

template <class T>
List_old<T>::List_old(const List_old<T>& a0, const T& _t) : Lizt_old_ATTLC((Lizt_old_ATTLC&)a0)
{
	put(_t);
}

template <class T>
List_old<T>::List_old()
{
#ifndef __GNUG__
	lnnk_old_ATTLC<T>::init_pool(); 
#endif
}

template <class T>
List_old<T>::List_old(const T& _t)
{
#ifndef __GNUG__
	lnnk_old_ATTLC<T>::init_pool();
#endif
	put(_t);
}

template <class T>
List_old<T>::List_old(const T& _t, const T& u)
{
#ifndef __GNUG__
	lnnk_old_ATTLC<T>::init_pool();
#endif
	put(_t);
	put(u);
}

template <class T>
List_old<T>::List_old(const T& _t, const T& u, const T& v)
{
#ifndef __GNUG__
	lnnk_old_ATTLC<T>::init_pool();
#endif
	put(_t);
	put(u);
	put(v);
}

template <class T>
List_old<T>::List_old(const T& _t, const T& u, const T& v, const T& w)
{
#ifndef __GNUG__
	lnnk_old_ATTLC<T>::init_pool();
#endif
	put(_t);
	put(u);
	put(v);
	put(w);
}

template <class T>
List_old<T>::List_old(const List_old<T>& a) : Lizt_old_ATTLC((const Lizt_old_ATTLC&)a)
{
#ifndef __GNUG__
	lnnk_old_ATTLC<T>::init_pool(); 
#endif
}

template <class T>
T 
List_old<T>::unput()
{ 
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::unput();
	T ans = ll->val; 
	delete ll; 
	return ans; 
}

template <class T>
T 
List_old<T>::get()
{ 
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::get();
	if (ll == 0)
		return T();
	T ans = ll->val; 
	delete ll; 
	return ans; 
}

template <class T>
T&
List_old<T>::operator[](unsigned ii)
{
	return (T&)*(getAt(ii));
}

template <class T>
const T&
List_old<T>::operator[](unsigned ii) const
{
	return (const T&)*(((List_old<T>*)this)->getAt(ii));
}

template <class T>
T
List_old<T>::head() const
{
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::head();
	if (ll == 0)
		return T();
	return ll->val;
}

template <class T>
T
List_old<T>::tail() const
{
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::tail();
	if (ll == 0)
		return T();
	return ll->val;
}

template <class T>
int
List_old<T>::find_next(const T& _t)
{
	return ((Listiter_old<T>*)(&myit))->find_next(_t);
}

template <class T>
int
List_old<T>::find_prev(const T& _t)
{
	return ((Listiter_old<T>*)(&myit))->find_prev(_t);
}

template <class T>
int
List_old<T>::next(T& _t)
{
	return ((Listiter_old<T>*)(&myit))->next(_t);
}

template <class T>
int
List_old<T>::next(T*& _t)
{
	return ((Listiter_old<T>*)(&myit))->next(_t);
}

template <class T>
T
List_old<T>::next()
{
	return ((Listiter_old<T>*)(&myit))->next();
}

template <class T>
int
List_old<T>::prev(T& _t)
{
	return ((Listiter_old<T>*)(&myit))->prev(_t);
}

template <class T>
int
List_old<T>::prev(T*& _t)
{
	return ((Listiter_old<T>*)(&myit))->prev(_t);
}

template <class T>
T
List_old<T>::prev()
{
	return ((Listiter_old<T>*)(&myit))->prev();
}

template <class T>
int
List_old<T>::peek_next(T& _t) const
{
	return ((Const_listiter_old<T>*)(&((List_old<T>*)this)->myit))->peek_next(_t); 
}

template <class T>
int
List_old<T>::peek_next(T*& _t) const
{
	return ((Const_listiter_old<T>*)(&((List_old<T>*)this)->myit))->peek_next(_t); 
}

template <class T>
T
List_old<T>::peek_next() const
{
	return ((Const_listiter_old<T>*)(&((List_old<T>*)this)->myit))->peek_next(); 
}

template <class T>
int
List_old<T>::peek_prev(T& _t) const
{
	return ((Const_listiter_old<T>*)(&((List_old<T>*)this)->myit))->peek_prev(_t); 
}

template <class T>
int
List_old<T>::peek_prev(T*& _t) const
{
	return ((Const_listiter_old<T>*)(&((List_old<T>*)this)->myit))->peek_prev(_t); 
}

template <class T>
T
List_old<T>::peek_prev() const
{
	return ((Const_listiter_old<T>*)(&((List_old<T>*)this)->myit))->peek_prev(); 
}

template <class T>
int
List_old<T>::remove_prev()
{
	return ((Listiter_old<T>*)(&myit))->remove_prev();
}

template <class T>
int
List_old<T>::remove_next()
{
	return ((Listiter_old<T>*)(&myit))->remove_next();
}

template <class T>
int
List_old<T>::remove_prev(T& _t)
{
	return ((Listiter_old<T>*)(&myit))->remove_prev(_t); 
}

template <class T>
int
List_old<T>::remove_next(T& _t)
{
	return ((Listiter_old<T>*)(&myit))->remove_next(_t); 
}

template <class T>
void
List_old<T>::sort(int (*cmp)(const T&,const T&))
{
	if ( length() < 2 )
		return;

	voidP_List_old_sort_internal(*(List_old<voidP>*)this,
			(int (*)(const voidP &, const voidP &))cmp);
	reset_all_iters();
}

template <class T>
int
List_old<T>::unput(T& _t)
{
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::unput();
	if ( ll )
	{ 
		lnnk_old_ATTLC<T>::deletelnnk_old_ATTLC(_t, ll);
		return 1;
	}
	else
		return 0;
}

template <class T>
int
List_old<T>::get(T& _t)
{ 
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::get();
	if ( ll )
	{ 
		lnnk_old_ATTLC<T>::deletelnnk_old_ATTLC(_t, ll);
		return 1;
	}
	else
		return 0;
}

template <class T>
int
List_old<T>::replace_prev(const T& x)
{
	if ( at_head() )
		return 0;
	else
		return (((lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::peek_prev())->val=x, 1);
}

template <class T>
int
List_old<T>::replace_next(const T& x)
{
	if ( at_end() )
		return 0;
	else
		return (((lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::peek_next())->val=x, 1);
}

template <class T>
List_old<T>&
List_old<T>::put(const T& x)
{
	return (List_old<T>&) Lizt_old_ATTLC::put(lnnk_old_ATTLC<T>::getnewlnnk_old_ATTLC(x));
}

template <class T>
List_old<T>&
List_old<T>::unget(const T& x)
{ 
	return (List_old<T>&)Lizt_old_ATTLC::unget(lnnk_old_ATTLC<T>::getnewlnnk_old_ATTLC(x));
}

template <class T>
void
List_old<T>::insert_prev(const T& x)
{
	Lizt_old_ATTLC::insert_prev(lnnk_old_ATTLC<T>::getnewlnnk_old_ATTLC(x));
}

template <class T>
void	
List_old<T>::insert_next(const T& x)
{
	Lizt_old_ATTLC::insert_next(lnnk_old_ATTLC<T>::getnewlnnk_old_ATTLC(x));
}

template <class T>
T*
List_old<T>::getAt(int i)
{ 
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Lizt_old_ATTLC::getAt(i);
	if ( ll )
		return &(ll->val);
	else
		return (T*)0;
}

template <class T>
Const_listiter_old<T>::Const_listiter_old(const List_old<T>& a)
	: Liztiter_old_ATTLC((Lizt_old_ATTLC&)a)
{
}

template <class T>
Const_listiter_old<T>::Const_listiter_old(const Const_listiter_old<T>& a)
	: Liztiter_old_ATTLC((const Liztiter_old_ATTLC&)a)
{
}

template <class T>
int
Const_listiter_old<T>::next(T& t)
{
	if ( at_end() )
		return 0;
	else
		return (t = ((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::next())->val, 1);
}

template <class T>
int
Const_listiter_old<T>::next(T*& t)
{
	if ( at_end() )
		return 0;
	else
		return (t = &((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::next())->val, 1);
}

template <class T>
T 
Const_listiter_old<T>::next()
{
	// if ( at_end() )
	//	return 0;
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::next();
	return ll->val;
}

template <class T>
int
Const_listiter_old<T>::prev(T& t)
{
	if ( at_head() )
		return 0;
	else
		return (t=((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::prev())->val, 1);
}

template <class T>
int
Const_listiter_old<T>::prev(T*& t)
{
	if ( at_head() )
		return 0;
	else
		return (t= &((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::prev())->val, 1);
}

template <class T>
T 
Const_listiter_old<T>::prev()
{
	// if ( at_head() )
	//	return 0;
	lnnk_old_ATTLC<T>* ll = (lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::prev();
	return ll->val;
}

template <class T>
int
Const_listiter_old<T>::find_prev(const T& x)
{
	if ( at_head() || theLizt->length()==0 )
		return 0;

	lnnk_old_ATTLC<T>* iter = (lnnk_old_ATTLC<T>*) pred->nxt;
	register int i = index;
	do {
		iter = (lnnk_old_ATTLC<T>*) iter->prv;
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
Const_listiter_old<T>::find_next(const T& x)
{
	if ( at_end() || theLizt->length()==0 )
		return 0;

	lnnk_old_ATTLC<T>* iter = (lnnk_old_ATTLC<T>*) pred;
	register int i = index;
	do {
		iter = (lnnk_old_ATTLC<T>*) iter->nxt;
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
T
Const_listiter_old<T>::peek_prev() const
{
	if ( at_head() )
		return T();
	return ((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_prev())->val;
}

template <class T>
int
Const_listiter_old<T>::peek_prev(T& t) const
{
	if ( at_head() )
		return 0;
	else
		return (t = ((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_prev())->val, 1);
}

template <class T>
int
Const_listiter_old<T>::peek_prev(T*& t) const
{
	if ( at_head() )
		return 0;
	else
		return (t = &((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_prev())->val, 1);
}

template <class T>
T
Const_listiter_old<T>::peek_next() const
{
	if ( at_end() )
		return T();
	return ((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_next())->val;
}

template <class T>
int
Const_listiter_old<T>::peek_next(T& t) const
{
	if ( at_end() )
		return 0;
	else
		return (t = ((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_next())->val, 1);
}

template <class T>
int
Const_listiter_old<T>::peek_next(T*& t) const
{
	if ( at_end() )
		return 0;
	else
		return (t = &((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_next())->val, 1);
}

template <class T>
T*
Const_listiter_old<T>::getAt(int i)
{
	lnnk_old_ATTLC<T>* ll = ((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::getAt(i));
	if ( ll )
		return &(ll->val);
	else
		return (T*)0;
}

template <class T>
Listiter_old<T>::Listiter_old(List_old<T>& a) : Const_listiter_old<T>((const List_old<T>&)a)
{
}

template <class T>
Listiter_old<T>::Listiter_old(const Listiter_old<T>& a) : Const_listiter_old<T>((Const_listiter_old<T>)a)
{
}

template <class T>
int
Listiter_old<T>::remove_prev()
{
	lnnk_old_ATTLC<T> *aLink = (lnnk_old_ATTLC<T> *)Liztiter_old_ATTLC::remove_prev();
	if ( aLink ) {
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter_old<T>::remove_prev(T &x)
{
	lnnk_old_ATTLC<T> *aLink = (lnnk_old_ATTLC<T> *)Liztiter_old_ATTLC::remove_prev();
	if ( aLink ) {
		x = aLink->val;
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter_old<T>::remove_next()
{
	lnnk_old_ATTLC<T> *aLink = (lnnk_old_ATTLC<T> *)Liztiter_old_ATTLC::remove_next();
	if ( aLink ) {
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter_old<T>::remove_next(T &x)
{
	lnnk_old_ATTLC<T> *aLink = (lnnk_old_ATTLC<T> *)Liztiter_old_ATTLC::remove_next();
	if ( aLink ) {
		x = aLink->val;
		delete aLink;
		return 1;
	} else
		return 0;
}

template <class T>
int
Listiter_old<T>::replace_prev(const T& x)
{
	if ( at_head() )
		return 0;
	else
		return (((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_prev())->val=x,1);
}

template <class T>
int
Listiter_old<T>::replace_next(const T& x)
{
	if ( at_end() )
		return 0;
	else
		return (((lnnk_old_ATTLC<T>*)Liztiter_old_ATTLC::peek_next())->val=x,1);
}

template <class T>
void 
Listiter_old<T>::insert_prev(const T& x)
{
	Liztiter_old_ATTLC::insert_prev(lnnk_old_ATTLC<T>::getnewlnnk_old_ATTLC(x));
}

template <class T>
void 
Listiter_old<T>::insert_next(const T& x)
{
	Liztiter_old_ATTLC::insert_next(lnnk_old_ATTLC<T>::getnewlnnk_old_ATTLC(x));
}

template <class T>
List_of_piter_old<T>::List_of_piter_old(List_of_p_old<T>& l) : 
	Const_list_of_piter_old<T>((const List_of_p_old<T>&)l)
{
}

template <class T>
List_of_piter_old<T>::List_of_piter_old(const List_of_piter_old<T>& l) : 
	Const_list_of_piter_old<T>((Const_list_of_piter_old<T>)l)
{
}

template <class T>
List_of_piter_old<T>::~List_of_piter_old()
{
}

template <class T>
List_of_p_old<T>::List_of_p_old(const T* x) : List_old<voidP>((voidP) x)
{
}

template <class T>
List_of_p_old<T>::List_of_p_old(const T* x, const T* y) :
	List_old<voidP>((voidP) x, (voidP) y)
{
}

template <class T>
List_of_p_old<T>::List_of_p_old(const T* x, const T* y, const T* z) :
	List_old<voidP>((voidP)x, (voidP)y, (voidP)z)
{
}

template <class T>
List_of_p_old<T>::List_of_p_old(const T* x, const T* y, const T* z, const T* w) :
	List_old<voidP>((voidP) x, (voidP) y, (voidP) z, (voidP) w)
{
}

template <class T>
Const_list_of_piter_old<T>::Const_list_of_piter_old(const List_of_p_old<T>& l) :
	Listiter_old<voidP>((List_old<voidP>&) l)
{
}

template <class T>
Const_list_of_piter_old<T>::Const_list_of_piter_old(const Const_list_of_piter_old<T>& l) :
	Listiter_old<voidP>((Listiter_old<voidP>&) l)
{
}

template <class T>
List_of_p_old<T> List_of_p_old<T>::operator+(const List_of_p_old<T>& ll)
{
	List_old<voidP> result((List_old<voidP>&)*this + (List_old<voidP>&)ll);
	return (List_of_p_old<T>&)result;
}

template <class T>
List_of_p_old<T> List_of_p_old<T>::operator+(const T* _t)
{
	List_old<voidP> result((List_old<voidP>&)*this + (voidP)_t);
	return (List_of_p_old<T>&)result;
}

template <class T>
T*&
List_of_p_old<T>::operator[](unsigned ii)
{
	return (T*&)*getAt(ii);
}

template <class T>
const T*&
List_of_p_old<T>::operator[](unsigned ii) const
{
	return (const T*&)*((List_of_p_old<T>*)this)->getAt(ii);
}

template <class T>
ostream&
operator<<(ostream& oo, const List_old<T>& ll)
{
	int first = 1;
	oo << "( ";
	Const_listiter_old<T> l(ll);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << l.next();
	}
	oo << " )";
	return oo;
}

template <class T>
ostream&
List_old<T>::print(ostream& oo) const
{
	int first = 1;
	oo << "( ";
	Const_listiter_old<T> l(*this);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << l.next();
	}
	oo << " )";
	return oo;
}

template <class T>
ostream&
operator<<(ostream& oo, const List_of_p_old<T>& ll)
{
	int first = 1;
	oo << "( ";
	Const_list_of_piter_old<T> l(ll);
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
List_of_p_old<T>::print(ostream& oo) const
{
	int first = 1;
	oo << "( ";
	Const_list_of_piter_old<T> l(*this);
	while (!l.at_end()) {
		if (!first)
			oo << ", ";
		first = 0;
		oo << *(l.next());
	}
	oo << " )";
	return oo;
}

#endif
