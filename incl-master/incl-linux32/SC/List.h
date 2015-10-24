/*ident	"@(#)List.h	1.1.2.7" */
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

#ifndef LISTH
#define LISTH

#include <new.h>
#include <Pool.h>

class lnk_ATTLC;
class Lizt_ATTLC;
class ostream;

typedef	void*	voidP;

// implements two-way pointers 

struct lnk_ATTLC {
	lnk_ATTLC* nxt;
	lnk_ATTLC* prv;

	lnk_ATTLC() {}
	virtual ~lnk_ATTLC();

	void	init(lnk_ATTLC* p, lnk_ATTLC* s) { prv = p; nxt = s; }
	virtual lnk_ATTLC* copy();
	virtual int operator==(lnk_ATTLC&);
};


// base class for all [Const_]Listiter(T)'s 

class	Liztiter_ATTLC {

	friend class Lizt_ATTLC;

	Liztiter_ATTLC(Lizt_ATTLC* lp) : theLizt(lp), cache(0), nextIt(0) {}

    protected:
	Lizt_ATTLC*	theLizt;	// associated List
	Liztiter_ATTLC* nextIt;		// next on chain
	lnk_ATTLC* 	cache;		// a recently retrieved link
	int		cacheNo;	// its index or garbage if cache == 0
	int		index;		// current position
	lnk_ATTLC*	pred;		// current position

	lnk_ATTLC*	getAt(int);
	lnk_ATTLC*	next();
	lnk_ATTLC*	prev();
	inline lnk_ATTLC*	peek_next() const;
	inline lnk_ATTLC* 	peek_prev() const;
	void		insert_prev(lnk_ATTLC*);
	void		insert_next(lnk_ATTLC*);
	lnk_ATTLC* 	remove_prev();
	lnk_ATTLC* 	remove_next();
	void 		reset0();

    public:
	
	Liztiter_ATTLC(Lizt_ATTLC&);
	Liztiter_ATTLC(const Liztiter_ATTLC&);
	~Liztiter_ATTLC();

	Liztiter_ATTLC&	operator=(const Liztiter_ATTLC&);

	int	operator==(const Liztiter_ATTLC& l) const {	
			return theLizt == l.theLizt && index == l.index;
		}
	int 	operator!=(const Liztiter_ATTLC& l) const {
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
inline lnk_ATTLC*	Liztiter_ATTLC::peek_next() const
{
	return at_end() ? (lnk_ATTLC*)0 : pred->nxt;
}
inline lnk_ATTLC* 	Liztiter_ATTLC::peek_prev() const
{ 
	return at_head() ? (lnk_ATTLC*)0 : pred;
}

// base class for all List<T>'s 

class	Lizt_ATTLC {
	friend class Liztiter_ATTLC;

    protected:
	lnk_ATTLC*	t;		// tail
	int  		sz;		// number of elements
	Liztiter_ATTLC*	iter_head;

	Lizt_ATTLC();
	Lizt_ATTLC(const Lizt_ATTLC& x) : iter_head(0) { init_all(x); }
	Lizt_ATTLC(const Lizt_ATTLC&, const Lizt_ATTLC&);
	~Lizt_ATTLC();

	void 	delete_all();			// used by dtor and operator=()
	void	init_all(const Lizt_ATTLC&);	// used by ctor and operator=()
	void	init_all_to_empty();   		// used by make_empty()
	void	add_a_link(lnk_ATTLC*);		// used by put() and unget()

	lnk_ATTLC*  tail() const { return t; }
	lnk_ATTLC*  head() const { return t ? t->nxt : 0; }
	operator    void*()	{ return sz ? this : 0; }
	operator    void*() const { return sz ? (void*)this : 0; }

	Lizt_ATTLC& unget(lnk_ATTLC*);
	Lizt_ATTLC& unget(const Lizt_ATTLC&);
	Lizt_ATTLC& put(lnk_ATTLC*);
	Lizt_ATTLC& put(const Lizt_ATTLC&);
	lnk_ATTLC*  get();
	lnk_ATTLC*  unput();
	lnk_ATTLC*  getAt(int);

    public:
	Lizt_ATTLC& operator=(const Lizt_ATTLC&);

	int	operator==(const Lizt_ATTLC&) const;
	int	operator!=(const Lizt_ATTLC& x) const { return !(*this == x); }
	void    make_empty();
	int 	length() const { return sz; }

	void 	reset_all_iters();
};

#ifndef __GNUG__
#ifndef voidP_List
#define voidP_List List<voidP>
#define voidP_Listiter Listiter<voidP>
#endif
#endif
#ifdef __GNUG__
class voidP_List : public Lizt_ATTLC {
  friend void voidP_List_sort_internal(voidP_List&,
	int (*)(const voidP &, const voidP &));
  voidP_List(const voidP_List& a0, const voidP_List& a1)
	: Lizt_ATTLC((Lizt_ATTLC&)a0, (Lizt_ATTLC&)a1) {}
  voidP_List(const voidP_List&, const voidP&);
protected:
  void** getAt(int);
public:
  voidP_List();
  voidP_List(const voidP_List&);
  voidP_List(const voidP&);
  voidP_List(const voidP&, const voidP&);
  voidP_List(const voidP&, const voidP&, const voidP&);
  voidP_List(const voidP&, const voidP&, const voidP&, const voidP&);
  ~voidP_List() {}
  operator void*() { return Lizt_ATTLC::operator void*(); }
  operator void*() const { return Lizt_ATTLC::operator void*(); }
  int operator==(const voidP_List& l) const {
	return (Lizt_ATTLC&)*this == (Lizt_ATTLC&)l; }
  int operator!=(const voidP_List& l) const {
	return (Lizt_ATTLC&)*this != (Lizt_ATTLC&)l; }
  voidP_List operator+(const voidP_List& ll) {
	return voidP_List(*this, ll); }
  voidP_List operator+(const voidP_List& ll) const {
	return voidP_List(*this, ll); }
  voidP_List operator+(const voidP& _t) {
	return voidP_List(*this, _t); }
  voidP_List operator+(const voidP& _t) const {
	return voidP_List(*this, _t); }
  voidP_List& operator=(const voidP_List& a) {
	return (voidP_List&)(*(Lizt_ATTLC*)this = *(Lizt_ATTLC*)&a); }
  voidP_List& put(const voidP& x);
  voidP_List& put(const voidP_List& ll) {
	return (voidP_List&) Lizt_ATTLC::put((Lizt_ATTLC&) ll); }
  void* unput();
  int unput(voidP&);
  void* get();
  int get(voidP&);
  voidP_List& unget(const voidP& x);
  voidP_List& unget(const voidP_List& ll) { 
	return (voidP_List&)Lizt_ATTLC::unget((Lizt_ATTLC&) ll); }
  void** head() const;
  void** tail() const;
  voidP& operator[](int i) { return ((*this).operator[]((unsigned) i)); }
  const voidP& operator[](int i) const {
	return ((*this).operator[]((unsigned) i)); }
  voidP& operator[](unsigned);
  const voidP& operator[](unsigned) const;
  void sort(int (*)(const voidP&, const voidP&));
  ostream& print(ostream&) const;
};

class voidP_Const_listiter : public Liztiter_ATTLC {
protected:
  void** getAt(int i);
public:
  voidP_Const_listiter(const voidP_List&);
  voidP_Const_listiter(const voidP_Const_listiter&);
  ~voidP_Const_listiter() {}
  voidP_Const_listiter& operator=(const voidP_Const_listiter& l) {
	return (voidP_Const_listiter&) ((Liztiter_ATTLC&)*this =
		(const Liztiter_ATTLC&)l);
  }
  int operator==(const voidP_Const_listiter& l) const {
	return (const Liztiter_ATTLC&)*this == (const Liztiter_ATTLC&)l; }
  int operator!=(const voidP_Const_listiter& l) const {
	return (const Liztiter_ATTLC&)*this != (const Liztiter_ATTLC&)l; }
  int find_next(const voidP&);
  int find_prev(const voidP&);
  int next(voidP& t);
  int next(voidP*& t);
  void** next();
  int prev(voidP& t);
  int prev(voidP*& t);
  void** prev();
  int step_next() { return Liztiter_ATTLC::next() != 0; }
  int step_prev() { return Liztiter_ATTLC::prev() != 0; }
  int peek_next(voidP&) const;
  int peek_next(voidP*&) const;
  void** peek_next() const;
  int peek_prev(voidP&) const;
  int peek_prev(voidP*&) const;
  void** peek_prev() const;
  const voidP_List* the_list() { return (const voidP_List*)theLizt; }
};

class voidP_Listiter : public voidP_Const_listiter {
public:
  voidP_Listiter(voidP_List&);
  voidP_Listiter(const voidP_Listiter&);
  ~voidP_Listiter() {}
  voidP_Listiter& operator=(const voidP_Listiter& l) {
	return (voidP_Listiter&) ((Liztiter_ATTLC&)*this = (Liztiter_ATTLC&)l);
  }
  int operator==(const voidP_Listiter& l) const {
	return (Liztiter_ATTLC&)*this == (Liztiter_ATTLC&)l; }
  int operator!=(const voidP_Listiter& l) const {
	return (Liztiter_ATTLC&)*this != (Liztiter_ATTLC&)l; }
  voidP_List* the_list() { return (voidP_List*)theLizt; }
	// the following operations change the container
  int remove_prev();
  int remove_next();
  int remove_prev(voidP&);
  int remove_next(voidP&);
  void insert_prev(const voidP& x);
  void insert_next(const voidP& x);
  int replace_prev(const voidP&);
  int replace_next(const voidP&);
};
#endif

#ifdef __GNUG__
#pragma interface
#endif

template <class T> class List;
template <class T> class Listiter;
template <class T> class Const_listiter;

template <class T>
ostream& operator<<(ostream&, const List<T>&);

template <class T>
void voidP_List_sort_internal(List<T>&, int (*)(const T &, const T &));

template <class T>
class	lnnk_ATTLC : public lnk_ATTLC {

	friend class List<T>;
	friend class Const_listiter<T>;
	friend class Listiter<T>;
#if defined(__SUNPRO_CC) || defined(__GNUG__)
	friend void voidP_List_sort_internal(List<T>&,
				int (*)(const T &, const T &));
#else
	template <class Type> friend void voidP_List_sort_internal(List<Type>&,
				int (*)(const Type &, const Type &));
#endif

#ifndef __GNUG__
	static	Pool* pool;
#endif
	T	val;

	lnnk_ATTLC(T& pp) : val(pp) {}
#ifdef __GNUG__
	~lnnk_ATTLC() {}

	lnk_ATTLC* copy() { return new lnnk_ATTLC<T>((T&)val); }
	int	operator==(lnk_ATTLC&x) { return val == ((lnnk_ATTLC<T>*)&x)->val; }
#else
	~lnnk_ATTLC();

	lnk_ATTLC* copy();
	int	operator==(lnk_ATTLC&);
#endif

    public:
	void*	operator new(size_t);
#ifdef __GNUG__
	void	operator delete(void* l) { pool()->free(l); }
	static	Pool *pool();
#else
	void	operator delete(void* l) { pool->free(l); }
	static	void init_pool() {	// should be called by List constructors
			if (pool == 0)
			    pool = new Pool(sizeof(lnnk_ATTLC<T>));
		}
#endif

	static	lnk_ATTLC* getnewlnnk_ATTLC(const T&);
	static	void deletelnnk_ATTLC(T&, lnnk_ATTLC<T>*);
};

template <class T>
class	List : public Lizt_ATTLC {

#if defined(__SUNPRO_CC) || defined(__GNUG__)
	friend void voidP_List_sort_internal(List<T>&,
				int (*)(const T &, const T &));
#else
	template <class Type> friend void voidP_List_sort_internal(List<Type>&,
				int (*)(const Type &, const Type &));
#endif

	List(const List<T>& a0, const List<T>& a1)
		: Lizt_ATTLC((Lizt_ATTLC&)a0, (Lizt_ATTLC&)a1) {}
	List(const List<T>&, const T&);

    protected:
	T*	getAt(int);

    public:
	List();
	List(const List<T>&);
	List(const T&);
	List(const T&, const T&);
	List(const T&, const T&, const T&);
	List(const T&, const T&, const T&, const T&);
	~List() {}

	operator void*() { return Lizt_ATTLC::operator void*(); }
	operator void*() const { return Lizt_ATTLC::operator void*(); }

	int	 operator==(const List<T>& l) const {
			return (Lizt_ATTLC&)*this == (Lizt_ATTLC&)l;
		 }
	int	 operator!=(const List<T>& l) const {
			return (Lizt_ATTLC&)*this != (Lizt_ATTLC&)l;
		 }

	List<T>  operator+(const List<T>& ll) { return List<T>(*this, ll); }
	List<T>  operator+(const List<T>& ll) const { return List<T>(*this, ll); }
	List<T>  operator+(const T& _t) { return List<T>(*this, _t); }
	List<T>  operator+(const T& _t) const { return List<T>(*this, _t); }

	List<T>& operator=(const List<T>& a) {
			return (List<T>&)(*(Lizt_ATTLC*)this = *(Lizt_ATTLC*)&a);
		 }
	List<T>& put(const T& x);
	List<T>& put(const List<T>& ll) {
			return (List<T>&) Lizt_ATTLC::put((Lizt_ATTLC&) ll);
		 }
	T	 unput();
	int	 unput(T&);
	T	 get();
	int	 get(T&);
	List<T>& unget(const T& x);
	List<T>& unget(const List<T>& ll) { 
			return (List<T>&)Lizt_ATTLC::unget((Lizt_ATTLC&) ll);
		 }

	T*	 head() const;
	T*	 tail() const;

	T&	 operator[](int i) {return ((*this).operator[]((unsigned) i));}
	const T& operator[](int i) const {return ((*this).operator[]((unsigned) i));}
	T&	 operator[](unsigned);
	const T& operator[](unsigned) const;
	void     sort(int (*)(const T&, const T&));
	ostream& print(ostream&) const;
};

template <class T>
class	Const_listiter : public Liztiter_ATTLC {

    protected:
	T*	 getAt(int i);

    public:
	Const_listiter(const List<T>&);
	Const_listiter(const Const_listiter<T>&);
	~Const_listiter() {}

	Const_listiter<T>&	operator=(const Const_listiter<T>& l) {
			return (Const_listiter<T>&) ((Liztiter_ATTLC&)*this =
						    (const Liztiter_ATTLC&)l);
		}

	int	operator==(const Const_listiter<T>& l) const {
			return (const Liztiter_ATTLC&)*this == (const Liztiter_ATTLC&)l;
		}
	int	operator!=(const Const_listiter<T>& l) const {
			return (const Liztiter_ATTLC&)*this != (const Liztiter_ATTLC&)l;
		}

	int	find_next(const T&);
	int	find_prev(const T&);
	int	next(T& t);
	int	next(T*& t);
	T*	next();
	int	prev(T& t);
	int	prev(T*& t);
	T*	prev();

	int	step_next() {
			return Liztiter_ATTLC::next() != 0;
		}
	int	step_prev() {
			return Liztiter_ATTLC::prev() != 0;
		}
	int	peek_next(T&) const;
	int	peek_next(T*&) const;
	T*	peek_next() const;
	int	peek_prev(T&) const;
	int	peek_prev(T*&) const;
	T*	peek_prev() const;

	const List<T>* the_list() {
			return (const List<T>*)theLizt;
		}
};

template <class T>
class	Listiter : public Const_listiter<T> {

    public:
	Listiter(List<T>&);
	Listiter(const Listiter<T>&);
	~Listiter() {}

	Listiter<T>&	operator=(const Listiter<T>& l) {
				return (Listiter<T>&) ((Liztiter_ATTLC&)*this =
						      (Liztiter_ATTLC&)l);
			}

	int	operator==(const Listiter<T>& l) const {
			return (Liztiter_ATTLC&)*this == (Liztiter_ATTLC&)l;
		}
	int	operator!=(const Listiter<T>& l) const {
			return (Liztiter_ATTLC&)*this != (Liztiter_ATTLC&)l;
		}

	List<T>* the_list() { return (List<T>*)theLizt; }

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

template <class T> class List_of_p;
template <class T> class List_of_piter;

template <class T>
ostream& operator<<(ostream&, const List_of_p<T>&);

template <class T>
class List_of_p : public voidP_List {

    public:
	List_of_p() {}
	List_of_p(const T*);
	List_of_p(const T*, const T*);
	List_of_p(const T*, const T*, const T*);
	List_of_p(const T*, const T*, const T*, const T*);
	List_of_p(const List_of_p<T>& ll) : voidP_List((const voidP_List&) ll) {}
	~List_of_p() {}

	T*&	 operator[](int i) {return ((*this).operator[]((unsigned) i));}
	const T*& operator[](int i) const {return ((*this).operator[]((unsigned) i));}
	T*&	  operator[](unsigned);
	const T*& operator[](unsigned) const;

	operator  void*() {
			return voidP_List::operator void*();
		  }
	operator  void*() const { return voidP_List::operator void*(); }

	int	operator==(const List_of_p<T>& ll) const {
			return (const voidP_List&)*this == (const voidP_List&)ll;
		}
	int	operator!=(const List_of_p<T>& l) const {
			return !(*this == l);
		}

	List_of_p<T>&	operator=(const List_of_p<T>& ll) {
				return (List_of_p<T>&) ((voidP_List&)*this =
					       (const voidP_List&)ll);
			}
	List_of_p<T>	operator+(const List_of_p<T>&);
	List_of_p<T>	operator+(const T*);

	List_of_p<T>&	put(const T* _t) {
				return (List_of_p<T>&) voidP_List::put((voidP)_t);
			}
	List_of_p<T>&	put(const List_of_p<T>& ll) {
				return (List_of_p<T>&) voidP_List::put((const voidP_List&)ll);
			}

	T*	unput() { return (T*)voidP_List::unput(); }
	int 	unput(T*& _t) { return voidP_List::unput((voidP&)_t); }

	T*	get() { return (T*)voidP_List::get(); }
	int	get(T*& _t) { return voidP_List::get((voidP&)_t); }

	List_of_p<T>&	unget(const T* x) {
				return (List_of_p<T>&) voidP_List::unget((voidP)x);
			}
	List_of_p<T>&	unget(const List_of_p<T>& ll) {
				return (List_of_p<T>&) voidP_List::unget((const voidP_List&)ll);
			}

	T*	head() const {
		    voidP* ptr = voidP_List::head();
		    return (ptr ? (T*)*ptr : 0);
		}
	T*	tail() const {
		    voidP* ptr = voidP_List::tail();
		    return (ptr ? (T*)*ptr : 0);
		}

	void  	sort(int (*pf)(const T* &, const T* &)) {
			voidP_List::sort((int (*)(const voidP &, const voidP &))pf);
		}

	ostream&	print(ostream&) const;
};

template <class T>
class Const_list_of_piter : public voidP_Listiter {

    public:
	Const_list_of_piter(const List_of_p<T>&);
	Const_list_of_piter(const Const_list_of_piter<T>&);
	~Const_list_of_piter() {}


	int 	operator==(const Const_list_of_piter<T>& l) const {
			return (const voidP_Listiter&)*this == ((const voidP_Listiter&)l);
		}

	int 	operator!=(const Const_list_of_piter<T>& l) const {
			return !(*this == l);
		}

	Const_list_of_piter<T>& 	operator=(const Const_list_of_piter<T>& ll) {
			return (Const_list_of_piter<T>&)((voidP_Listiter&)*this =
				(const voidP_Listiter&)ll);
		}

	const List_of_p<T>* 	the_list() {
			return (const List_of_p<T>*)voidP_Listiter::the_list();
		}

	int 	find_next(const T*const& t) { return voidP_Listiter::find_next((voidP&)t); }
	int 	find_prev(const T*const& t) { return voidP_Listiter::find_prev((voidP&)t); }

	T* 	next() {
		    voidP* ptr = voidP_Listiter::next();
		    return (ptr ? (T*)*ptr : 0);
		}
	int 	next(T* &t) { return voidP_Listiter::next((voidP&)t); }
	int 	next(T**& t) { return voidP_Listiter::next((voidP*&)t); }

	T* 	prev() {
		    voidP* ptr = voidP_Listiter::prev();
		    return (ptr ? (T*)*ptr : 0);
		}
	int 	prev(T*& t) { return voidP_Listiter::prev((voidP&)t); }
	int 	prev(T**& t) { return voidP_Listiter::prev((voidP*&)t); }

	T* 	peek_next() const {
		    voidP* ptr = voidP_Listiter::peek_next();
		    return (ptr ? (T*)*ptr : 0);
		}
	int 	peek_next(T*& t) const { return voidP_Listiter::peek_next((voidP&)t); }
	int 	peek_next(T**& t) const { return voidP_Listiter::peek_next((voidP*&)t); }

	T* 	peek_prev() const {
		    voidP* ptr = voidP_Listiter::peek_prev();
		    return (ptr ? (T*)*ptr : 0);
		}
	int 	peek_prev(T*& t) const { return voidP_Listiter::peek_prev((voidP&)t); }
	int 	peek_prev(T**& t) const { return voidP_Listiter::peek_prev((voidP*&)t); }

};


template <class T>
class List_of_piter : public Const_list_of_piter<T> {

    public:
	List_of_piter(List_of_p<T>&);
	List_of_piter(const List_of_piter<T>&);
	~List_of_piter();


	int	operator==(const List_of_piter<T>& l) const {
			return (const Const_list_of_piter<T>&)*this ==
				((const Const_list_of_piter<T>&)l);
		}

	int	operator!=(const List_of_piter<T>& l) const {
			return !(*this == l);
		}

	List_of_piter<T>&	operator=(const List_of_piter<T>& ll) {
			return (List_of_piter<T>&)((voidP_Listiter&)*this =
				(const voidP_Listiter&)ll);
		}

	List_of_p<T>*	the_list() {
			return (List_of_p<T>*)voidP_Listiter::the_list();
		}

	// the following operations change the container

	int	remove_prev() { return voidP_Listiter::remove_prev(); }
	int	remove_prev(T*& x) { return voidP_Listiter::remove_prev((voidP&)x); }
	int	remove_next() { return voidP_Listiter::remove_next(); }
	int	remove_next(T*& x) { return voidP_Listiter::remove_next((voidP&)x); }

	void	insert_prev(T*& x) { voidP_Listiter::insert_prev((voidP&)x); }
	void	insert_next(T*& x) { voidP_Listiter::insert_next((voidP&)x); }

	int	replace_prev(T* x) { return voidP_Listiter::replace_prev((voidP)x); }
	int	replace_next(T* x) { return voidP_Listiter::replace_next((voidP)x); }
};

#if (defined(__edg_att_40) || defined(__GNUG__)) && !defined(__IMPLICIT_INCLUDE)
#include "List.c"
#endif
#endif
