/*ident	"@(#)Set:incl/set_of_p.h	3.1" */
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

#ifndef SET_OF_PH
#define SET_OF_PH

//  The following include is required so that the instantiation
//  files that include set_of_p.h will get what they need from Set.h


#include <Set.h>

typedef int*	ALIGN;

class Set_of_p_union_ATTLC;
class Set_of_p_inter_ATTLC;
class Set_of_p_diff_ATTLC;
class Set_of_p_xor_ATTLC;
class pst_ATTLC;
class pst_internal_node_ATTLC;
class pst_iter_old_ATTLC;
class pst_new_iter_ATTLC;

#define PSET_NODE_SIZE_ATTLC	32
#define PSET_POSITIONS_ATTLC	6
#define PSET_MASK_BITS_ATTLC	0X1F
#define PSET_INITIAL_SHIFT_ATTLC	2
#define PSET_SHIFT_INCR_ATTLC	5

extern Pool*	pst_internal_node_pool_ptr_ATTLC;

class pst_internal_item_ATTLC {

    friend class pst_ATTLC;
    friend class pst_iter_old_ATTLC;
    friend class pst_internal_node_ATTLC;
private:
    union {
	ALIGN	ext_leaf;
	pst_internal_node_ATTLC*	nodep;
	long	this_is_leaf; // assume sizeof(long) != sizeof(ALIGN)
    };
    int	is_leaf_or_null() const { return this_is_leaf & 01; }
    int	is_node() const { return !is_leaf_or_null() && nodep; }
    int	is_leaf() const { return is_leaf_or_null() &&
			((long)ext_leaf & ~(long)01); }
    int	is_null() const { return !nodep; }
    pst_internal_node_ATTLC*	next_node() const {
#ifdef DEBUG_ATTLC
				    assert(is_node());
#endif
				    return nodep; }
    ALIGN	external_leaf() const {
#ifdef DEBUG_ATTLC
		    assert(is_leaf());
#endif
		    return (ALIGN)((long)ext_leaf & ~(long)01); }
    void	make_leaf(ALIGN p) {
#ifdef DEBUG_ATTLC
		    assert(((long)p & 01) == 0);
#endif
		    ext_leaf = (ALIGN)((long)p | 01); }
    void	make_node(pst_internal_node_ATTLC* cp) {
#ifdef DEBUG_ATTLC
		    assert(((long)cp & 01) == 0);
#endif
		    nodep = cp; }
    void	make_null() { nodep = 0; }
};

class pst_internal_node_ATTLC {

    friend class pst_ATTLC;
    friend class pst_iter_old_ATTLC;
private:
    static void initialize();
    void* operator new(size_t i);
    void operator delete(void* p) {
	    	pst_internal_node_pool_ptr_ATTLC->free(p);
    	}
    ~pst_internal_node_ATTLC() {}
    pst_internal_node_ATTLC();
    pst_internal_item_ATTLC	item[PSET_NODE_SIZE_ATTLC];
    int busy_count;
};
    
class pst_position_ATTLC {

    friend class pst_ATTLC;
    friend class pst_iter_old_ATTLC;
private:
    pst_internal_item_ATTLC*	curr_pos[PSET_POSITIONS_ATTLC];
    int	curr_depth;
    ALIGN	curr_value;
    pst_position_ATTLC() : curr_depth(-1), curr_value(0) {}
};
    
class pst_iter_old_ATTLC : private pst_position_ATTLC {

    friend class pst_ATTLC;
private:
    pst_ATTLC*	my_pst;
    pst_iter_old_ATTLC*	next_it;
public:
    pst_iter_old_ATTLC(const pst_ATTLC&);
    pst_iter_old_ATTLC(const pst_iter_old_ATTLC&);
    ~pst_iter_old_ATTLC();
    ALIGN	first();
    ALIGN	next();
    pst_ATTLC*	the_pst() { return my_pst; }
    const pst_ATTLC*	the_pst() const { return my_pst; }
    pst_iter_old_ATTLC& operator=(const pst_iter_old_ATTLC&);
};

class pst_new_iter_ATTLC {

    pst_iter_old_ATTLC	it;
    int		inited;
    ALIGN	nxt, prv;
protected:
    pst_new_iter_ATTLC(const pst_ATTLC& p) : it(p), inited(0), prv(0) {}
    pst_new_iter_ATTLC(const pst_new_iter_ATTLC& pi) : it(pi.it), inited(0), prv(0) {}
    int	peek_next(ALIGN&) const;
    ALIGN	peek_next() const;
    int	peek_prev(ALIGN&) const;
    ALIGN	peek_prev() const { return inited ? prv : 0; }
    int	next(ALIGN&);
    ALIGN	next();
    const pst_ATTLC*	the_pst() const { return it.the_pst(); }
public:
    pst_new_iter_ATTLC& operator=(const pst_new_iter_ATTLC& pi) {
	 it = pi.it; inited = pi.inited; prv = pi.prv; nxt = pi.nxt;
	 return *this;
    }
    void	reset() { inited = 0; }
};

class pst_ATTLC {

    friend class pst_iter_old_ATTLC;
private:
    int	sze;
    pst_internal_item_ATTLC	contents;
    pst_iter_old_ATTLC*	iter_head;
    pst_position_ATTLC	pos;
    // these functions are only used on empty psets
    void	make_union(const pst_ATTLC&, const pst_ATTLC&);
    void	make_intersection(const pst_ATTLC&, const pst_ATTLC&);
    void	make_difference(const pst_ATTLC&, const pst_ATTLC&);
    void	make_xor(const pst_ATTLC&, const pst_ATTLC&);

    void	warn_iterators() const;
protected:
    pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_union_ATTLC*);
    pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_inter_ATTLC*);
    pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_diff_ATTLC*);
    pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_xor_ATTLC*);
public:
    pst_ATTLC();
    pst_ATTLC(ALIGN p);
    pst_ATTLC(ALIGN p1, ALIGN p2);
    pst_ATTLC(ALIGN p1, ALIGN p2, ALIGN p3);
    pst_ATTLC(ALIGN p1, ALIGN p2, ALIGN p3, ALIGN p4);
    pst_ATTLC(const pst_ATTLC& p);
    ~pst_ATTLC();
    pst_ATTLC&	operator=(const pst_ATTLC&);
    unsigned	size() const { return (unsigned)sze; }
    unsigned	size_unique() const { return (unsigned)sze; }
    operator const void*() const { return size() ? this : 0; }
    int	operator==(const pst_ATTLC&) const;
    int	operator!=(const pst_ATTLC& ps) const { return !(*this == ps); }
    int	operator<=(const pst_ATTLC&) const;
    int	operator<(const pst_ATTLC&) const;
    int	operator>=(const pst_ATTLC& ps) const { return ps <= *this; }
    int	operator>(const pst_ATTLC& ps) const { return ps < *this; }
    ALIGN	insert(ALIGN p); 
    unsigned	remove(const ALIGN p); 
    unsigned	remove_all(); 
    ALIGN	contains(const ALIGN p) const;
    inline pst_ATTLC	operator|(const pst_ATTLC&) const;
    inline pst_ATTLC	operator-(const pst_ATTLC&) const;
    inline pst_ATTLC	operator&(const pst_ATTLC&) const;
    inline pst_ATTLC	operator^(const pst_ATTLC&) const;
    pst_ATTLC&	operator|=(const pst_ATTLC&);
    pst_ATTLC&	operator-=(const pst_ATTLC&);
    pst_ATTLC&	operator&=(const pst_ATTLC&);
    pst_ATTLC&	operator^=(const pst_ATTLC&);
    void	check() const;
};


#ifdef __GNUG__
#pragma interface
#endif

template <class T>
class Set_of_p : public pst_ATTLC {

    Set_of_p(const Set_of_p<T>& a, const Set_of_p<T>& b, Set_of_p_union_ATTLC* x) :
    	     pst_ATTLC((const pst_ATTLC&)a, (const pst_ATTLC&)b, x) {}
    Set_of_p(const Set_of_p<T>& a, const Set_of_p<T>& b, Set_of_p_inter_ATTLC* x) :
    	     pst_ATTLC((const pst_ATTLC&)a, (const pst_ATTLC&)b, x) {}
    Set_of_p(const Set_of_p<T>& a, const Set_of_p<T>& b, Set_of_p_diff_ATTLC* x) :
	     pst_ATTLC((const pst_ATTLC&)a, (const pst_ATTLC&)b, x) {}
    Set_of_p(const Set_of_p<T>& a, const Set_of_p<T>& b, Set_of_p_xor_ATTLC* x) :
	     pst_ATTLC((const pst_ATTLC&)a, (const pst_ATTLC&)b, x) {}
public:
    Set_of_p() {}
    Set_of_p(T* p) : pst_ATTLC((ALIGN)p) {}
    Set_of_p(T* p1, T* p2) : pst_ATTLC((ALIGN)p1, (ALIGN)p2) {}
    Set_of_p(T* p1, T* p2, T* p3) : pst_ATTLC((ALIGN)p1, (ALIGN)p2, (ALIGN)p3) {}
    Set_of_p(T* p1, T* p2, T* p3, T* p4) : pst_ATTLC((ALIGN)p1, (ALIGN)p2,
						     (ALIGN)p3, (ALIGN)p4) {}
    Set_of_p<T>&	operator=(const Set_of_p<T>& p) { return (Set_of_p<T>&)
					(*(pst_ATTLC*)this = p); }
    int	operator==(const Set_of_p<T>& p) const { return (*(pst_ATTLC*)this == p); }
    int	operator!=(const Set_of_p<T>& ps) const { return !(*this == ps); }
    int	operator<=(const Set_of_p<T>& p) const { return (*(pst_ATTLC*)this <= p); }
    int	operator<(const Set_of_p<T>& p) const { return (*(pst_ATTLC*)this < p); }
    int	operator>=(const Set_of_p<T>& p) const { return p <= *this; }
    int	operator>(const Set_of_p<T>& p) const { return p < *this; }

    T* insert(T* p, int count=1) { 
		return (count<=0) ? 0 : (T*)pst_ATTLC::insert((ALIGN)p);
    	}   
    unsigned	remove(const T* p, int count=1) { 
		return (count<=0) ? 0 : pst_ATTLC::remove((ALIGN)p);
    	}
    unsigned	remove_all(const T* p) {
		return pst_ATTLC::remove((ALIGN)p); 
    	}
    unsigned	remove_all() { 
		return pst_ATTLC::remove_all();
    	}

    T* contains(const T* p) const { 
		return (T*)pst_ATTLC::contains((ALIGN)p);
    	}

    unsigned count(const T* p) const { 
		return contains(p)?1:0;
    	}
    T*	select()const;
    Set_of_p<T>	operator|(const Set_of_p<T>& p) const {
	return Set_of_p<T>(*this, p, (Set_of_p_union_ATTLC*)0);
    }
    Set_of_p<T>	operator-(const Set_of_p<T>& p) const {
	return Set_of_p<T>(*this, p, (Set_of_p_diff_ATTLC*)0);
    }
    Set_of_p<T>	operator&(const Set_of_p<T>& p) const {
	return Set_of_p<T>(*this, p, (Set_of_p_inter_ATTLC*)0);
    }
    Set_of_p<T>	operator^(const Set_of_p<T>& p) const {
	return Set_of_p<T>(*this, p, (Set_of_p_xor_ATTLC*)0);
    }
    Set_of_p<T>&	operator|=(const Set_of_p<T>& p) { return (Set_of_p<T>&)
			((pst_ATTLC*)this)->operator|=(p); }
    Set_of_p<T>&	operator-=(const Set_of_p<T>& p) { return (Set_of_p<T>&)
			((pst_ATTLC*)this)->operator-=(p); }
    Set_of_p<T>&	operator&=(const Set_of_p<T>& p) { return (Set_of_p<T>&)
			((pst_ATTLC*)this)->operator&=(p); }
    Set_of_p<T>&	operator^=(const Set_of_p<T>& p) { return (Set_of_p<T>&)
			((pst_ATTLC*)this)->operator^=(p); }

    ostream& print(ostream&) const;
};

template <class T>
ostream& operator<<(ostream& os, const Set_of_p<T>& s);

template <class T>
class Set_of_piter : public pst_new_iter_ATTLC {
public:
    Set_of_piter(const Set_of_p<T>& s) :
			pst_new_iter_ATTLC((const pst_ATTLC&)s) {}
    Set_of_piter(const Set_of_piter<T>& si) :
			pst_new_iter_ATTLC((const pst_new_iter_ATTLC&)si) {}
    int	peek_next(T*& p) const { return
			pst_new_iter_ATTLC::peek_next((ALIGN&)p); }
    T*	peek_next() const { return (T*)pst_new_iter_ATTLC::peek_next(); }
    int	peek_prev(T*& p) const { return
			pst_new_iter_ATTLC::peek_prev((ALIGN&)p); }
    T*	peek_prev() const { return (T*)pst_new_iter_ATTLC::peek_prev(); }
    int	next(T*& p) { return pst_new_iter_ATTLC::next((ALIGN&)p); }
    T*	next() { return (T*)pst_new_iter_ATTLC::/*peek_*/next(); }
    const Set_of_p<T>*	the_set_of_p() const { return
			(Set_of_p<T>*)pst_new_iter_ATTLC::the_pst(); }
    Set_of_piter<T>& operator=(const Set_of_piter<T>& si) {
	return (Set_of_piter<T>&)((pst_new_iter_ATTLC&)*this =
			       (const pst_new_iter_ATTLC&)si);
    }
};

#ifndef __GNUG__
inline pst_ATTLC
pst_ATTLC::operator|(const pst_ATTLC& p) const
{
    return pst_ATTLC(*this, p, (Set_of_p_union_ATTLC*)0);
}

inline pst_ATTLC
pst_ATTLC::operator-(const pst_ATTLC& p) const
{
    return pst_ATTLC(*this, p, (Set_of_p_diff_ATTLC*)0);
}

inline pst_ATTLC
pst_ATTLC::operator&(const pst_ATTLC& p) const
{
    return pst_ATTLC(*this, p, (Set_of_p_inter_ATTLC*)0);
}

inline pst_ATTLC
pst_ATTLC::operator^(const pst_ATTLC& p) const
{
    return pst_ATTLC(*this, p, (Set_of_p_xor_ATTLC*)0);
}
#endif

template <class T>
ostream& operator<<(ostream& os, const Set_of_p<T>& s)
{
    os << "{";
    Set_of_piter<T> it(s);
    T* p;
    int first=1;

    while ( p = it.next() ) {
	if ( first )
	    first=0;
	else
	    os << ",";

	os << *p;
    }
    os << "}";
    return os;
}

#if (defined(__edg_att_40) || defined(__GNUG__)) && !defined(__IMPLICIT_INCLUDE)
#include <set_of_p.c>
#endif
#endif
