/*ident	"@(#)List_old.h	1.1.2.2" */
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

#ifndef OLISTH
#define OLISTH

#include <new.h>
#include <Pool.h>

class lnk_old_ATTLC;
class Lizt_old_ATTLC;
class ostream;

typedef	void*	voidP;

// implements two-way pointers 

struct lnk_old_ATTLC {
	lnk_old_ATTLC* nxt;
	lnk_old_ATTLC* prv;

	lnk_old_ATTLC() {}
	virtual ~lnk_old_ATTLC();

	void	init(lnk_old_ATTLC* p, lnk_old_ATTLC* s) { prv = p; nxt = s; }
	virtual lnk_old_ATTLC* copy();
	virtual int operator==(lnk_old_ATTLC&);
};


/* base class for all [Const_]Listiter(T)'s */

class	Liztiter_old_ATTLC {

	friend class Lizt_old_ATTLC;

	Liztiter_old_ATTLC(Lizt_old_ATTLC* lp) : theLizt(lp), cache(0), nextIt(0) {}

    protected:
	Lizt_old_ATTLC*	theLizt;	// associated List
	Liztiter_old_ATTLC* nextIt;	// next on chain
	lnk_old_ATTLC* 	cache;		// a recently retrieved link
	int		cacheNo;	// its index or garbage if cache == 0
	int		index;		// current position
	lnk_old_ATTLC*	pred;		// current position

	lnk_old_ATTLC*	getAt(int);
	lnk_old_ATTLC*	next();
	lnk_old_ATTLC*	prev();
	inline lnk_old_ATTLC*	peek_next() const;
	inline lnk_old_ATTLC* 	peek_prev() const;
	void		insert_prev(lnk_old_ATTLC*);
	void		insert_next(lnk_old_ATTLC*);
	lnk_old_ATTLC* 	remove_prev();
	lnk_old_ATTLC* 	remove_next();
	void 		reset0();

    public:
	
	Liztiter_old_ATTLC(Lizt_old_ATTLC&);
	Liztiter_old_ATTLC(const Liztiter_old_ATTLC&);
	~Liztiter_old_ATTLC();

	Liztiter_old_ATTLC&	operator=(const Liztiter_old_ATTLC&);

	int	operator==(const Liztiter_old_ATTLC& l) const {	
			return theLizt == l.theLizt && index == l.index;
		}
	int 	operator!=(const Liztiter_old_ATTLC& l) const {
			return !(*this == l);
		}
	int 	position() const {
			return index;
		}
	void 	reset(unsigned = 0);
	void 	end_reset(unsigned = 0);
	int 	at_head() const { return index == 0; }
	int 	at_end() const;
};
inline lnk_old_ATTLC*	Liztiter_old_ATTLC::peek_next() const
{ 
	return at_end() ? (lnk_old_ATTLC*)0 : pred->nxt;
}
inline lnk_old_ATTLC* 	Liztiter_old_ATTLC::peek_prev() const
{ 
	return at_head() ? (lnk_old_ATTLC*)0 : pred;
}

/* base class for all List<T>'s */

class	Lizt_old_ATTLC {
	friend class Liztiter_old_ATTLC;

    protected:
	lnk_old_ATTLC*	t;		// tail
	int  		sz;		// number of elements
	Liztiter_old_ATTLC	myit;	// current position pointer

	Lizt_old_ATTLC();
	Lizt_old_ATTLC(const Lizt_old_ATTLC& x) : myit(this) { init_all(x); }
	Lizt_old_ATTLC(const Lizt_old_ATTLC&, const Lizt_old_ATTLC&);
	~Lizt_old_ATTLC();

	void 	delete_all();		// used by dtor and operator=()
	void	init_all(const Lizt_old_ATTLC&);	// used by ctor and operator=()
	void	init_all_to_empty();   		// used by make_empty()
	void	add_a_link(lnk_old_ATTLC*);	// used by put() and unget()

	lnk_old_ATTLC*  tail() const { return t; }
	lnk_old_ATTLC*  head() const { return t ? t->nxt : 0; }
	operator    void*()	{ return sz ? this : 0; }

	Lizt_old_ATTLC& unget(lnk_old_ATTLC*);
	Lizt_old_ATTLC& unget(const Lizt_old_ATTLC&);
	Lizt_old_ATTLC& put(lnk_old_ATTLC*);
	Lizt_old_ATTLC& put(const Lizt_old_ATTLC&);
	lnk_old_ATTLC*  get();
	lnk_old_ATTLC*  unput();

	lnk_old_ATTLC*  getAt(int i) { return myit.getAt(i); }
	lnk_old_ATTLC*  next() { return myit.next(); }
	lnk_old_ATTLC*  prev() { return myit.prev(); }
	lnk_old_ATTLC*  peek_next() const { return myit.peek_next(); }
	lnk_old_ATTLC*  peek_prev() const { return myit.peek_prev(); }
	void 	    insert_prev(lnk_old_ATTLC* l) { myit.insert_prev(l); }
	void 	    insert_next(lnk_old_ATTLC* l) { myit.insert_next(l); }
	lnk_old_ATTLC*  remove_prev() { return myit.remove_prev(); }
	lnk_old_ATTLC*  remove_next() { return myit.remove_next(); }

    public:
	Lizt_old_ATTLC& operator=(const Lizt_old_ATTLC&);

	int	operator==(const Lizt_old_ATTLC&) const;
	int	operator!=(const Lizt_old_ATTLC& x) const { return !(*this == x); }
	void    make_empty();
	int 	length() const { return sz; }
	int 	position() const { return myit.index; }
	void 	reset(unsigned i = 0) { myit.reset(i); }
	void 	end_reset(unsigned i = 0) { myit.end_reset(i); }
	int 	at_end() const { return myit.at_end(); }
	int 	at_head() const { return myit.at_head(); }
	void 	reset_all_iters();
};

#ifdef __GNUG__
#pragma interface
#endif

template <class T> class List_old;
template <class T> class Listiter_old;
template <class T> class Const_listiter_old;

template <class T>
ostream& operator<<(ostream&, const List_old<T>&);

template <class T>
class	lnnk_old_ATTLC : public lnk_old_ATTLC {

	friend class List_old<T>;
	friend class Const_listiter_old<T>;
	friend class Listiter_old<T>;
	friend	void voidP_List_old_sort_internal(List_old<voidP>&,
				int (*)(const voidP &, const voidP &));

#ifndef __GNUG__
	static	Pool* pool;
#endif
	T	val;

	lnnk_old_ATTLC(T& pp) : val(pp) {}
	~lnnk_old_ATTLC();

	lnk_old_ATTLC* copy();
	int	operator==(lnk_old_ATTLC&);

    public:
	void*	operator new(size_t);
#ifdef __GNUG__
	void	operator delete(void* l) { pool()->free(l); }
	static	Pool *pool();
#else
	void	operator delete(void* l) { pool->free(l); }
	static	void init_pool() {	// should be called by List constructors
			if (pool == 0)
			    pool = new Pool(sizeof(lnnk_old_ATTLC<T>));
		}
#endif

	static	lnk_old_ATTLC* getnewlnnk_old_ATTLC(const T&);
	static	void deletelnnk_old_ATTLC(T&, lnnk_old_ATTLC<T>*);
};

template <class T>
class	List_old : public Lizt_old_ATTLC {

	friend	void voidP_List_old_sort_internal(List_old<voidP>&,
					int(*)(const voidP &, const voidP &));

	List_old(const List_old<T>& a0, const List_old<T>& a1)
		: Lizt_old_ATTLC((Lizt_old_ATTLC&)a0, (Lizt_old_ATTLC&)a1) {}
	List_old(const List_old<T>&, const T&);

    protected:
	T*	getAt(int i);

    public:
	List_old();
	List_old(const List_old<T>&);
	List_old(const T&);
	List_old(const T&, const T&);
	List_old(const T&, const T&, const T&);
	List_old(const T&, const T&, const T&, const T&);
	~List_old() {}

	operator void*() { return Lizt_old_ATTLC::operator void*(); }

	int	 operator==(const List_old<T>& l) const {
			return (Lizt_old_ATTLC&)*this == (Lizt_old_ATTLC&)l;
		 }
	int	 operator!=(const List_old<T>& l) const {
			return (Lizt_old_ATTLC&)*this != (Lizt_old_ATTLC&)l;
		 }

	List_old<T>  operator+(const List_old<T>& ll) {
			return List_old<T>(*this, ll);
		 }
	List_old<T>  operator+(const T& _t) { return List_old<T>(*this, _t); }

	List_old<T>& operator=(const List_old<T>& a) {
			return (List_old<T>&)(*(Lizt_old_ATTLC*)this = *(Lizt_old_ATTLC*)&a);
		 }
	List_old<T>& put(const T& x);
	List_old<T>& put(const List_old<T>& ll) {
			return (List_old<T>&) Lizt_old_ATTLC::put((Lizt_old_ATTLC&) ll);
		 }
	T	 unput();
	int	 unput(T&);
	T	 get();
	int	 get(T&);
	List_old<T>& unget(const T& x);
	List_old<T>& unget(const List_old<T>& ll) { 
			return (List_old<T>&)Lizt_old_ATTLC::unget((Lizt_old_ATTLC&) ll);
		 }

	T	 head() const;
	T	 tail() const;

	int	 find_next(const T&);
	int	 find_prev(const T&);

	int	 next(T&);
	int	 next(T*&);
	T	 next();
	int	 prev(T&);
	int	 prev(T*&);
	T	 prev();

	int	 step_next() { return Lizt_old_ATTLC::next() != 0; }
	int	 step_prev() { return Lizt_old_ATTLC::prev() != 0; }
	int	 peek_next(T&) const;
	int	 peek_next(T*&) const;
	T	 peek_next() const;
	int	 peek_prev(T&) const;
	int	 peek_prev(T*&) const;
	T	 peek_prev() const;

	int	 remove_prev();
	int	 remove_next();
	int	 remove_prev(T&);
	int	 remove_next(T&);

	void	 insert_prev(const T& x);
	void	 insert_next(const T& x);

	int	 replace_prev(const T&);
	int	 replace_next(const T&);
	T&       operator[](int i) {return ((*this).operator[]((unsigned) i));}
   	const T& operator[](int i) const {return ((*this).operator[]((unsigned) i));}
	T&	 operator[](unsigned);
	const T& operator[](unsigned) const;
	void     sort(int (*)(const T&, const T&));
	ostream& print(ostream&) const;
};

template <class T>
class	Const_listiter_old : public Liztiter_old_ATTLC {

    protected:
	T*	 getAt(int i);

    public:
	Const_listiter_old(const List_old<T>&);
	Const_listiter_old(const Const_listiter_old<T>&);
	~Const_listiter_old() {}

	Const_listiter_old<T>&	operator=(const Const_listiter_old<T>& l) {
			return (Const_listiter_old<T>&) ((Liztiter_old_ATTLC&)*this =
						    (const Liztiter_old_ATTLC&)l);
		}

	int	operator==(const Const_listiter_old<T>& l) const {
			return (const Liztiter_old_ATTLC&)*this == (const Liztiter_old_ATTLC&)l;
		}
	int	operator!=(const Const_listiter_old<T>& l) const {
			return (const Liztiter_old_ATTLC&)*this != (const Liztiter_old_ATTLC&)l;
		}

	int	find_next(const T&);
	int	find_prev(const T&);
	int	next(T& t);
	int	next(T*& t);
	T	next();
	int	prev(T& t);
	int	prev(T*& t);
	T	prev();

	int	step_next() {
			return Liztiter_old_ATTLC::next() != 0;
		}
	int	step_prev() {
			return Liztiter_old_ATTLC::prev() != 0;
		}
	int	peek_next(T&) const;
	int	peek_next(T*&) const;
	T	peek_next() const;
	int	peek_prev(T&) const;
	int	peek_prev(T*&) const;
	T	peek_prev() const;

	const List_old<T>* the_list() {
			return (const List_old<T>*)theLizt;
		}
};

template <class T>
class	Listiter_old : public Const_listiter_old<T> {

    public:
	Listiter_old(List_old<T>&);
	Listiter_old(const Listiter_old<T>&);
	~Listiter_old() {}

	Listiter_old<T>&	operator=(const Listiter_old<T>& l) {
				return (Listiter_old<T>&) ((Liztiter_old_ATTLC&)*this =
						      (Liztiter_old_ATTLC&)l);
			}

	int	operator==(const Listiter_old<T>& l) const {
			return (Liztiter_old_ATTLC&)*this == (Liztiter_old_ATTLC&)l;
		}
	int	operator!=(const Listiter_old<T>& l) const {
			return (Liztiter_old_ATTLC&)*this != (Liztiter_old_ATTLC&)l;
		}

	List_old<T>* the_list() { return (List_old<T>*)theLizt; }

	// the following operations change the container
	int	remove_prev();
	int	remove_next();
	int	remove_prev(T&);
	int	remove_next(T&);
	void 	insert_prev(const T& x);
	void 	insert_next(const T& x);
	int	replace_prev(const T&);
	int	replace_next(const T&);
};

template <class T> class List_of_p_old;
template <class T> class List_of_piter_old;

template <class T>
ostream& operator<<(ostream&, const List_of_p_old<T>&);

template <class T>
class List_of_p_old : public List_old<voidP> {

    public:
	List_of_p_old() {}
	List_of_p_old(const T*);
	List_of_p_old(const T*, const T*);
	List_of_p_old(const T*, const T*, const T*);
	List_of_p_old(const T*, const T*, const T*, const T*);
	List_of_p_old(const List_of_p_old<T>& ll) : List_old<voidP>((const List_old<voidP>&) ll) {}
	~List_of_p_old() {}

	T*&       operator[](int i) {return ((*this).operator[]((unsigned) i));}
   	const T*& operator[](int i) const {return ((*this).operator[]((unsigned) i));}
	T*&	  operator[](unsigned);
	const T*& operator[](unsigned) const;

	operator  void*() {
			return List_old<voidP>::operator void*();
		  }

	int	operator==(const List_of_p_old<T>& ll) const {
			return (const List_old<voidP>&)*this == (const List_old<voidP>&)ll;
		}
	int	operator!=(const List_of_p_old<T>& l) const {
			return !(*this == l);
		}

	List_of_p_old<T>&	operator=(const List_of_p_old<T>& ll) {
				return (List_of_p_old<T>&) ((List_old<voidP>&)*this =
					       (const List_old<voidP>&)ll);
			}
	List_of_p_old<T>	operator+(const List_of_p_old<T>&);
	List_of_p_old<T>	operator+(const T*);

	List_of_p_old<T>&	put(const T* _t) {
				return (List_of_p_old<T>&) List_old<voidP>::put((voidP)_t);
			}
	List_of_p_old<T>&	put(const List_of_p_old<T>& ll) {
				return (List_of_p_old<T>&) List_old<voidP>::put((const List_old<voidP>&)ll);
			}

	T*	unput() { return (T*)List_old<voidP>::unput(); }
	int 	unput(T*& _t) { return List_old<voidP>::unput((voidP&)_t); }

	T*	get() { return (T*)List_old<voidP>::get(); }
	int	get(T*& _t) { return List_old<voidP>::get((voidP&)_t); }

	List_of_p_old<T>&	unget(const T* x) {
				return (List_of_p_old<T>&) List_old<voidP>::unget((voidP)x);
			}
	List_of_p_old<T>&	unget(const List_of_p_old<T>& ll) {
				return (List_of_p_old<T>&) List_old<voidP>::unget((const List_old<voidP>&)ll);
			}

	T*	head() const { return (T*)List_old<voidP>::head(); }
	T*	tail() const { return (T*)List_old<voidP>::tail(); }

	int	find_next(const T*const&_t) { return List_old<voidP>::find_next((const voidP &)_t); }
	int	find_prev(const T*const&_t) { return List_old<voidP>::find_prev((const voidP &)_t); }

	T*	next() { return (T*)List_old<voidP>::next(); }
	int	next(T*& _t) { return List_old<voidP>::next((voidP&)_t); }
	int	next(T**& _t) { return List_old<voidP>::next((voidP*&)_t); }

	T* 	prev() { return (T*)List_old<voidP>::prev(); }
	int 	prev(T*& _t) { return List_old<voidP>::prev((voidP&)_t); }
	int 	prev(T**& _t) { return List_old<voidP>::prev((voidP*&)_t); }

	T*  	peek_next() const { return (T*)List_old<void*>::peek_next(); }
	int 	peek_next(T*& _t) const { return List_old<voidP>::peek_next((voidP&)_t); }
	int 	peek_next(T**& _t) const { return List_old<voidP>::peek_next((voidP*&)_t); }

	T* 	peek_prev() const { return (T*)List_old<voidP>::peek_prev(); }
	int 	peek_prev(T*& _t) const { return List_old<voidP>::peek_prev((voidP&)_t); }
	int 	peek_prev(T**& _t) const { return List_old<voidP>::peek_prev((voidP*&)_t); }

	int 	remove_prev() { return List_old<voidP>::remove_prev(); }
	int 	remove_prev(T*& x) { return List_old<voidP>::remove_prev((voidP&)x); }
	int 	remove_next() { return List_old<voidP>::remove_next(); }
	int 	remove_next(T*& x) { return List_old<voidP>::remove_next((voidP&)x); }

	void 	insert_prev(T*const&x) { List_old<voidP>::insert_prev((voidP &)x); }
	void 	insert_next(T*const&x) { List_old<voidP>::insert_next((voidP &)x); }

	int 	replace_prev(T*const&x) { return List_old<voidP>::replace_prev((voidP &)x); }
	int 	replace_next(T*const&x) { return List_old<voidP>::replace_next((voidP &)x); }

	void  	sort(int (*pf)(const T* &, const T* &)) {
			List_old<voidP>::sort((int (*)(const voidP &, const voidP &))pf);
		}

	ostream&	print(ostream&) const;
};

template <class T>
class Const_list_of_piter_old : public Listiter_old<voidP> {

    public:
	Const_list_of_piter_old(const List_of_p_old<T>&);
	Const_list_of_piter_old(const Const_list_of_piter_old<T>&);
	~Const_list_of_piter_old() {}


	int 	operator==(const Const_list_of_piter_old<T>& l) const {
			return (const Listiter_old<voidP>&)*this == ((const Listiter_old<voidP>&)l);
		}

	int 	operator!=(const Const_list_of_piter_old<T>& l) const {
			return !(*this == l);
		}

	Const_list_of_piter_old<T>& 	operator=(const Const_list_of_piter_old<T>& ll) {
			return (Const_list_of_piter_old<T>&)((Listiter_old<voidP>&)*this =
				(const Listiter_old<voidP>&)ll);
		}

	const List_of_p_old<T>* 	the_list() {
			return (const List_of_p_old<T>*)Listiter_old<voidP>::the_list();
		}

	int 	find_next(const T*const& t) { return Listiter_old<voidP>::find_next((voidP&)t); }
	int 	find_prev(const T*const& t) { return Listiter_old<voidP>::find_prev((voidP&)t); }

	T* 	next() { return (T*)Listiter_old<voidP>::next(); }
	int 	next(T* &t) { return Listiter_old<voidP>::next((voidP&)t); }
	int 	next(T**& t) { return Listiter_old<voidP>::next((voidP*&)t); }

	T* 	prev() { return (T*)Listiter_old<voidP>::prev(); }
	int 	prev(T*& t) { return Listiter_old<voidP>::prev((voidP&)t); }
	int 	prev(T**& t) { return Listiter_old<voidP>::prev((voidP*&)t); }

	T* 	peek_next() const { return (T*)Listiter_old<voidP>::peek_next(); }
	int 	peek_next(T*& t) const { return Listiter_old<voidP>::peek_next((voidP&)t); }
	int 	peek_next(T**& t) const { return Listiter_old<voidP>::peek_next((voidP*&)t); }

	T* 	peek_prev() const { return (T*)Listiter_old<voidP>::peek_prev(); }
	int 	peek_prev(T*& t) const { return Listiter_old<voidP>::peek_prev((voidP&)t); }
	int 	peek_prev(T**& t) const { return Listiter_old<voidP>::peek_prev((voidP*&)t); }

};


template <class T>
class List_of_piter_old : public Const_list_of_piter_old<T> {

    public:
	List_of_piter_old(List_of_p_old<T>&);
	List_of_piter_old(const List_of_piter_old<T>&);
	~List_of_piter_old();


	int	operator==(const List_of_piter_old<T>& l) const {
			return (const Const_list_of_piter_old<T>&)*this ==
				((const Const_list_of_piter_old<T>&)l);
		}

	int	operator!=(const List_of_piter_old<T>& l) const {
			return !(*this == l);
		}

	List_of_piter_old<T>&	operator=(const List_of_piter_old<T>& ll) {
			return (List_of_piter_old<T>&)((Listiter_old<voidP>&)*this =
				(const Listiter_old<voidP>&)ll);
		}

	List_of_p_old<T>*	the_list() {
			return (List_of_p_old<T>*)Listiter_old<voidP>::the_list();
		}

	// the following operations change the container

	int	remove_prev() { return Listiter_old<voidP>::remove_prev(); }
	int	remove_prev(T*& x) { return Listiter_old<voidP>::remove_prev((voidP&)x); }
	int	remove_next() { return Listiter_old<voidP>::remove_next(); }
	int	remove_next(T*& x) { return Listiter_old<voidP>::remove_next((voidP&)x); }

	void	insert_prev(T*const& x) { Listiter_old<voidP>::insert_prev((voidP&)x); }
	void	insert_next(T*const& x) { Listiter_old<voidP>::insert_next((voidP&)x); }

	int	replace_prev(T* x) { return Listiter_old<voidP>::replace_prev((voidP)x); }
	int	replace_next(T* x) { return Listiter_old<voidP>::replace_next((voidP)x); }
};

#define List	List_old
#define Listiter	Listiter_old
#define Const_listiter	Const_listiter_old
#define List_of_p	List_of_p_old
#define List_of_piter	List_of_piter_old
#define Const_list_of_piter	Const_list_of_piter_old

#if defined(__GNUG__)
#include "List_old.c"
#else
#if (defined(__edg_att_40) || defined(__GNUG__)) && !defined(__IMPLICIT_INCLUDE)
#include <List_old.c>
#endif
#endif
#endif
