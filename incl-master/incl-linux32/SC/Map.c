/*ident	"@(#)Map.c	1.1.2.3" */
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

#ifndef _MAP_C_
#define _MAP_C_

template <class S, class T>
Mapnode_ATTLC<S,T>::~Mapnode_ATTLC()
{
}

template <class S, class T>
Map<S,T>::~Map()
{
	Mapiter<S,T>* p = iter_head;
	while (p) {
		p->m = 0;
		p = p->iter_next;
	}
}

template <class S, class T>
Mapiter<S,T>::Mapiter (const Map<S,T>* map, Mapnode_ATTLC<S,T>* n): p(n)
{
	m = (Map<S,T>*)map;
	m->mi_count++;
	iter_next = m->iter_head;
	m->iter_head = this;
}

template <class S, class T>
Mapiter<S,T>::Mapiter (const Map<S,T>& map): p(0)
{
	m = (Map<S,T>*)(&map);
	m->mi_count++;
	iter_next = m->iter_head;
	m->iter_head = this;
}

template <class S, class T>
Mapiter<S,T>::~Mapiter ()
{
	if (m==0 || m->iter_head == 0) return;
	if (--m->mi_count == 0 && m->remove_flag) {
		m->remove_deadwood();
		m->remove_flag = 0;
	}
	if (this == m->iter_head) {
		m->iter_head = iter_next;
	} else {
		Mapiter<S,T>* _p = m->iter_head;
		while (_p->iter_next!=this) {
			_p = _p->iter_next;
		}
		_p->iter_next = iter_next;
	}
}

template <class S, class T>
Mapnode_ATTLC<S,T>::Mapnode_ATTLC(const S& xs, const T& xt): map_data (xs)
{
	map_data.value = xt;
}

template <class S, class T>
Map<S,T>::Map() : def (*Mapnode_ATTLC<S,T>::default_T())
{
	mi_count = 0; 
	remove_flag = 0; 
	iter_head = 0;
}

template <class S, class T>
Map<S,T>::Map(const T& d) : def (d)
{
	mi_count = 0; 
	remove_flag = 0; 
	iter_head = 0;
}

template <class S, class T>
Map<S,T>&
Map<S,T>::operator= (const Map<S,T>& m)
{
	if (this != &m) {
		make_empty();
		for (Mapiter<S,T> p (m); ++p; ) {
			// S s = p.key();	/* sidestep a cfront bug */
			// (*this) [s] = p.value();
			(*this) [p.key()] = p.value();
		}
//		iter_head=0;
//		mi_count = 0;
		/* march through the iterator list, setting each to vacuous */
		for (Mapiter<S,T> *ip = iter_head; ip; ip = ip->iter_next) {
			/* ip->m = this; */ /* unnecessary */
			ip->p = 0;
		}
   		remove_flag = 0;
	}
	return *this;
}
/**
template <class S, class T>
int
Map<S,T>::operator== (const Map<S,T>& m)
{
	if (m.size() != n) {
		cout << "aaa\n";
		return 0;
	}

	if (this != &m) {
		Mapiter<S,T> q(*this);
		Mapiter<S,T> p(m);
		while (++p) {
			if (++q == 0) 
			    return 0;

			if (q.key() != p.key()) {
			    cout << "bbb:q=" << q.key() << " p=" << p.key() << "\n";
			    return 0;
			}
			if (q.value() != p.value()) {
			    cout << "bbb:q=" << q.value().value() << " p=" << p.value().value() << "\n";
			    return 0;
			}
		}
		if (++q == 0) 
			return 1;
		
		cout << "ccc:" << (void*)p << " " << (void*)q << "\n";
	  	return 0;
	}
	cout << "ddd\n";
	return 1;
}
**/
template <class S, class T>
Map<S,T>::Map (const Map<S,T>& m): def (m.def), mi_count(0), remove_flag(0), iter_head(0)
{
	operator= (m);
}

template <class S, class T>
T&
Map<S,T>::newnode (Mapnodebase_ATTLC*& ptr, const S& s, Mapnode_ATTLC<S,T>* parent)
{
	Mapnode_ATTLC<S,T>* p = new Mapnode_ATTLC<S,T> (s, def);
	ptr = p;
	T& retval = p->map_data.value;

	p->U_ = parent;
	n++;

	Mapbase_ATTLC::newnode(ptr);
	
	return retval;
}

template <class S, class T>
T&
Map<S,T>::operator[] (const S& s)
{
	if (head()) {
		Mapnode_ATTLC<S,T>* t = head();
		for(;;) {
			if (s < t->map_data.key) {
				if (t->L())
					t = t->L();
				else
					return newnode (t->L_, s, t);
			} else if (t->map_data.key < s) {
				if (t->R())
					t = t->R();
				else
					return newnode (t->R_, s, t);
			} else
				break;
		}
		if (t->remove_mark != 0) {
			t->remove_mark = 0;
			t->map_data.value = def;
			n++;
		}
		return t->map_data.value;
	} else
		return newnode (head_, s, 0);
}

template <class S, class T>
const T&
Map<S,T>::operator[] (const S& s) const
{
	return (const T&)(((Map<S,T>*)this)->operator[](s));
}

template <class S, class T>
Mapiter<S,T>
Map<S,T>::element (const S& s) const
{
	Mapnode_ATTLC<S,T>* t = head();
	while (t) {
		if (s < t->map_data.key)
			t = t->L();
		else if (t->map_data.key < s)
			t = t->R();
		else
			break;
	}
	if (t && t->remove_mark == 0)
		return Mapiter<S,T> (this, t);
	else
		return Mapiter<S,T> (this, 0);
}

template <class S, class T>
Mapiter<S,T>
Map<S,T>::lub (const S& s) const
{
	Mapnode_ATTLC<S,T>* t = head();

	while(t){
		if(s < t->map_data.key){
			if(t->L())
				t = t->L();
			else
				break;
		}
		else if(t->map_data.key < s){
			if(t->R())
				t = t->R();
			else {
				t = successor(t);
				break;
			}
		}
		else
			break;
	}
	while (t && t->remove_mark){
		t = successor(t);
	}
	if(t)
		return Mapiter<S,T> (this, t);
	else
		return Mapiter<S,T> (this, 0);
}

template <class S, class T>
Mapiter<S,T>
Map<S,T>::glb (const S& s) const
{
	Mapnode_ATTLC<S,T>* t = head();

	while(t){
		if(t->map_data.key < s){
			if(t->R())
				t = t->R();
			else
				break;
		}
		else if (s < t->map_data.key){
			if(t->L())
				t = t->L();
			else {
				t = predecessor(t);
				break;
			}
		}
		else
			break;
	}
	while (t && t->remove_mark){
		t = predecessor(t);
	}
	if(t)
		return Mapiter<S,T> (this, t);
	else
		return Mapiter<S,T> (this, 0);
}

template <class S, class T>
int
Map<S,T>::remove(const S& s)
{
	Mapnode_ATTLC<S,T>* t = head();
	while (t) {
		if (s < t->map_data.key)
			t = t->L();
		else if (t->map_data.key < s)
			t = t->R();
		else break;
	}
	if (t) {
		if (mi_count > 0) {
			remove_flag = 1;
			if (!t->remove_mark) {
				t->remove_mark = 1;
				n--;
			}
			return (1);
		}
		else {
			removenode(t);
			delete t;
			n--;
			return (1);
		}
	}
	else {
		return (0);
	}
}

template <class S, class T>
Mapiter<S,T> Map<S,T>::first() const {
	return ++Mapiter<S,T> (*this);
}
 
template <class S, class T>
Mapiter<S,T> Map<S,T>::last() const {
	return --Mapiter<S,T> (*this);
}

template <class S, class T>
void
Mapiter<S,T>::remove()
{
	if (p) {
		m->remove_flag = 1;
		p->remove_mark = 1;
		m->n--;
	}
}

template <class S, class T>
Mapiter<S,T>&
Mapiter<S,T>::operator++()
{
	while (1) {
		p = m->successor(p);
		if (p == 0 || p->remove_mark == 0) {
			return (*this);
		}
	}
}

template <class S, class T>
Mapiter<S,T>&
Mapiter<S,T>::operator--()
{
	while (1) {
		p = m->predecessor(p);
		if (p == 0 || p->remove_mark == 0) {
			return (*this);
		}
	}
}

template <class S, class T>
Mapiter<S,T>::Mapiter(const Mapiter<S,T>& mi) : m(mi.m),p(mi.p)
{
	m->mi_count++;
	iter_next = m->iter_head;
	m->iter_head = this;
}

template <class S, class T>
Mapiter<S,T>&
Mapiter<S,T>::operator=(const Mapiter<S,T>& mi)
{
	if (this == m->iter_head) {
		m->iter_head = iter_next;
	} else {
		Mapiter<S,T>* _p = m->iter_head;
		while (_p && _p->iter_next!=this) {
			_p = _p->iter_next;
		}
		if (_p) {
			_p->iter_next = iter_next;
		}
	}
	((Map<S,T>*)m)->mi_count--;
	((Map<S,T>*)mi.m)->mi_count++;
	m = (Map<S,T>*)(mi.m);
	p = mi.p;
	iter_next = m->iter_head;
	m->iter_head = this;
	return *this;
}

template <class S, class T>
const S*
Mapnode_ATTLC<S,T>::default_S()
{
	static S S_default_item;
	return (&S_default_item);
}

template <class S, class T>
const T*
Mapnode_ATTLC<S,T>::default_T()
{
	static T T_default_item;
	return (&T_default_item);
}

template <class S, class T>
ostream &operator<<(ostream& os, const Map<S,T>& m) {
	Mapiter<S,T> mi(m);
	os << '{';
	while (++mi) {
		os << "[ " << mi.curr()->key << ' '
		   << mi.curr()->value << " ]";
	}
	os << '}';
	return os;
}

#endif
