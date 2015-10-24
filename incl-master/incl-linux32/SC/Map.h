/*ident	"@(#)Map.h	1.1.2.2" */
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

/*
 *	Map is a parameterized type class that stores a table
 *	of keys and associated data objects.  This implementation
 *	is done using AVL trees (see Knuth, The Art of Computer
 *	Programming, volume 3, p. 451).  The helper classes
 *	Mapnodebase_ATTLC and Mapbase_ATTLC implement the basic
 *	insertion and deletion operations (with rebalancing of
 *	the tree after the modification).  The lookup operation
 *	is implemented by Map(S,T)::operator[].
 *
 *	Each node has three pointers:  L_ points to the left
 *	child, R_ points to the right child, and U_ points to
 *	the parent node.  The predecessor_ and successor_ functions
 *	in the class Mapbase_ATTLC find the "inorder" predecessor
 *	and successor of a given node.
 *
 *	A Mapiter (Map iterator) is a pointer into the middle of
 *	a tree.  If there are any iterators active in a Map, any
 *	deletion operations merely mark the node for deletion.
 *	Later lookup and iterator operations skip over nodes that
 *	are marked for deletion.  The deletion is actually performed
 *	when the last iterator for the Map is destroyed.
 *
 *	See the tutorial "Associative Arrays in C++" for information
 *	on how to use the Map class.
 */

#ifndef MAP_H
#define MAP_H

class Mapnodebase_ATTLC {
protected:
	// Really want a declaration by association: 
	//	like this L, R, U;
	Mapnodebase_ATTLC *L_, *R_, *U_;
	char bal;
	char remove_mark;
	friend class Mapbase_ATTLC;
	// Similarly, this should be declared by association too:
	//	void attach (like this & p, like this dest)
	void attach (Mapnodebase_ATTLC*& p, Mapnodebase_ATTLC* dest)
		{ p = dest; if (dest) dest->U_ = this; }

	Mapnodebase_ATTLC() { L_ = 0; R_ = 0; bal = 0; remove_mark = 0; }
	virtual ~Mapnodebase_ATTLC();
};

class Mapbase_ATTLC {
public:
	void make_empty();
protected:
	// Also should be declared by association.
	//	like this head_;
	Mapnodebase_ATTLC *head_;
	int n;

	Mapbase_ATTLC() { head_ = 0; n = 0; }
	~Mapbase_ATTLC() { make_empty(); }

	void remove_deadwood();
	void newnode(Mapnodebase_ATTLC *);
	void rotL (Mapnodebase_ATTLC*);
	void rotR (Mapnodebase_ATTLC*);
	void rot_dir (Mapnodebase_ATTLC*, char);
	void removenode(Mapnodebase_ATTLC *);
	void del_balance(Mapnodebase_ATTLC *);

	// Should be declared by association.
	Mapnodebase_ATTLC *successor_(Mapnodebase_ATTLC *) const;
	Mapnodebase_ATTLC *predecessor_(Mapnodebase_ATTLC *) const;
};

#ifdef __GNUG__
#pragma interface
#endif

template <class S, class T> class Map;
template <class S, class T> class Mapiter;

template <class S, class T>
struct Map_pair {
	const S key;
	T value;
	Map_pair(const S& s) : key(s) { }
};

template <class S, class T>
class Mapnode_ATTLC : public Mapnodebase_ATTLC {

	Mapnode_ATTLC<S,T> *L() const { return (Mapnode_ATTLC<S,T> *)L_; }
	Mapnode_ATTLC<S,T> *R() const { return (Mapnode_ATTLC<S,T> *)R_; }

	Map_pair<S,T> map_data;

	Mapnode_ATTLC(const S&, const T&);
#ifdef __GNUG__
	~Mapnode_ATTLC() {}
#else
	~Mapnode_ATTLC();
#endif

	friend class Map<S,T>;
	friend class Mapiter<S,T>;

	static const S* default_S();
	static const T* default_T();
};

template <class S, class T>
class Mapiter {

	friend class Map<S,T>;

	Map<S,T>* m;
	Mapnode_ATTLC<S,T>* p;
	Mapiter<S,T>* iter_next;

	Mapiter(const Map<S,T>*, Mapnode_ATTLC<S,T>*);

public:
	Mapiter(const Map<S,T>&);
	~Mapiter();

	operator void*() const { return p; }

	void remove();
	void reset() { this->p = 0; }

        const Map<S,T>* the_map() const { return m; }

	Mapiter(const Mapiter<S,T>&);
	Mapiter<S,T>& operator=(const Mapiter<S,T>&);

	Map_pair<S,T>* next() { operator++(); return (p? &(p->map_data) : 0); }
	Map_pair<S,T>* prev() { operator--(); return (p? &(p->map_data) : 0); }
	Map_pair<S,T>* curr() { return (p? &(p->map_data) : 0); }

	// the following functions have been denigrated
	S key() const {
		return *(p? &p->map_data.key:
			(S*)Mapnode_ATTLC<S,T>::default_S());
	}

	T value() const {
		return *(p? &p->map_data.value:
			(T*)Mapnode_ATTLC<S,T>::default_T());
	}

	void setvalue(const T &t) const {
		if (p != 0) ((Mapnode_ATTLC<S,T>*)p)->map_data.value = t;
	}

	Mapiter<S,T>& operator++();
	Mapiter<S,T>& operator--();
};

template <class S, class T>
class Map : public Mapbase_ATTLC {

	Mapnode_ATTLC<S,T>* head() const { 
		return (Mapnode_ATTLC<S,T> *)head_; 
	}

	Mapnode_ATTLC<S,T>* successor(Mapnode_ATTLC<S,T>* p) const { 
		return (Mapnode_ATTLC<S,T> *)successor_(p); 
	}

	Mapnode_ATTLC<S,T>* predecessor(Mapnode_ATTLC<S,T>* p) const { 
		return (Mapnode_ATTLC<S,T> *)predecessor_(p); 
	}

	T def;
	int mi_count;
	char remove_flag;
	Mapiter<S,T>* iter_head;

	T& newnode(Mapnodebase_ATTLC*&, const S&, Mapnode_ATTLC<S,T>*);
	friend class Mapiter<S,T>;
public:
	Map();
	Map(const T&);
	Map(const Map<S,T>&);
	~Map();

	Map<S,T>& operator= (const Map<S,T>&);

//	int operator== (const Map<S,T>&);

	T& operator[] (const S&);
	const T& operator[] (const S&) const;
	int remove(const S&);

	int size() const { return n; }
	Mapiter<S,T> element (const S&) const;
	Mapiter<S,T> first() const;
	Mapiter<S,T> last() const;
	Mapiter<S,T> lub(const S&) const;
	Mapiter<S,T> glb(const S&) const;
};

class ostream;
class istream;

template <class S, class T>
ostream &operator<<(ostream& os, const Map<S,T>& m);

#if defined(__GNUG__)
#include "Map.c"
#else
#if (defined(__edg_att_40) || defined(__GNUG__)) && !defined(__IMPLICIT_INCLUDE)
#include <Map.c>
#endif
#endif
#endif
