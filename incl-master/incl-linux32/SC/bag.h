/*ident	"@(#)Set:incl/bag.h	3.1" */
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

#ifndef BAGH
#define BAGH

//  The following include is required so that the instantiation
//  files that include bag.h will get what they need from Set.h

#include <Set.h>

class ostream;

//  The 32 bits of a Set_or_Bag_hashval are broken into
//  8 4-bit groups.  Each 4-bit group indexes
//  into a Internal_node of 16 Bag_internal_item_ATTLCs.

static const int BAG_NODE_SIZE_ATTLC = 16;
static const int BAG_POSITIONS_ATTLC = 8;
static const int BAG_MASK_BITS_ATTLC = 0XF;
static const int BAG_INITIAL_SHIFT_ATTLC = 0;
static const int BAG_SHIFT_INCR_ATTLC = 4;

extern int BAG_LT_ATTLC(Set_or_Bag_hashval a,Set_or_Bag_hashval b);

//  Index to non-parametric classes:

class Bag_bucket_non_param_ATTLC;
class Bag_internal_item_ATTLC;
class Bag_internal_node_ATTLC;
class Bag_position_ATTLC;

//  The following ATTLC class declarations are
//  needed to disambiguate the copy-avoidance
//  constructors:

class Bag_select_ATTLC;
class Bag_reject_ATTLC;
class Bag_map_ATTLC;
class Bag_union_ATTLC;
class Bag_inter_ATTLC;
class Bag_diff_ATTLC;
class Bag_xor_ATTLC;

extern Pool* bag_internal_node_pool_ptr_ATTLC;

struct Bag_bucket_non_param_ATTLC {
    Set_or_Bag_hashval hashval;
};

struct Bag_internal_item_ATTLC {
    //  An Bag_internal_item_ATTLC is a single "slot" in an
    //  Bag_internal_node_ATTLC.  It may contain:
    //
    //	o  An Bag_internal_node_ATTLC*
    //	o  A Bag_bucket_non_param_ATTLC*
    //	o  NULL
    //
    //  Bag_bucket_non_param_ATTLC* and Bag_internal_node_ATTLC* 
    //  pointers can't be byte-aligned, so the low order 
    //  bit is free to be used as the union discriminant.
    //
    //   o  When the low-order bit is 1, the item
    //	is a Bag_bucket_non_param_ATTLC*
    //
    //   o  When the low order bit is 0, the item
    //	is a Bag_internal_node_ATTLC* or NULL.

    union {
	void* ext_leaf;
	Bag_internal_node_ATTLC* nodep;
	long this_is_leaf;
    };
    inline int is_leaf_or_null() const;

    inline int is_node() const;
    inline int is_leaf() const;
    inline int is_null() const;
    inline Bag_internal_node_ATTLC* next_node() const;
    inline void* external_leaf() const;
    inline void make_leaf(void*);
    inline void make_node(Bag_internal_node_ATTLC*);
    inline void make_null();

    void show(int) const;
};

struct Bag_internal_node_ATTLC {
    //  A Bag_internal_node_ATTLC has 16 Internal_slots
    //  plus a count of how many are in use.
    //  Each Internal_slot is directly indexed
    //  by a 4-bit chunk.

    static void initialize();
    Bag_internal_item_ATTLC item[BAG_NODE_SIZE_ATTLC];
    int	busy_count;
    Bag_internal_node_ATTLC();
    inline ~Bag_internal_node_ATTLC();
    void* operator new(size_t);
    inline void operator delete(void*);
    void show(int) const;
};

struct Bag_position_ATTLC {
    //  A Bag_position_ATTLC caches the result of the most
    //  recent search, down to the level of the
    //  Bucket:
    //
    //	o  curr_value is the Set_or_Bag_hashval that we
    //	   searched for
    //
    //	o  curr_pos is a stack of pointers to
    //	   Bag_internal_item_ATTLCs which remembers a
    //	   path of Bag_internal_item_ATTLCs through the
    //	   index:
    //
    //	   o  curr_pos[0] points to the Bag_internal_item_ATTLC
    //		selected by the first 4 bit chunk
    //
    //	   o  curr_pos[1] points to the Bag_internal_item_ATTLC
    //		selected by the second 4 bit chunk
    //
    //	   o  curr_pos[curr_depth] points to the
    //		Bag_internal_item_ATTLC which points to
    //		a Bucket<TYPE>
    //
    //  If Bag mutation occurs, a Bag_position_ATTLC is
    //  canceled by setting its curr_depth to -1;
    //  curr_value can be used subsequently to rebuild
    //  the cur_pos stack so that iteration can resume
    //

    Bag_internal_item_ATTLC* curr_pos[BAG_POSITIONS_ATTLC];
    int curr_depth;
    Set_or_Bag_hashval curr_value;
    inline Bag_position_ATTLC();
    void show() const;
};

//  Bag_internal_item_ATTLC inlines

int
Bag_internal_item_ATTLC::is_leaf_or_null() const
{
    return this_is_leaf & 01;
}

int
Bag_internal_item_ATTLC::is_node() const
{
    return (!is_leaf_or_null() && nodep);
}

int
Bag_internal_item_ATTLC::is_leaf() const
{
    return (is_leaf_or_null() && ((long)ext_leaf & ~(long)01));
}

int
Bag_internal_item_ATTLC::is_null() const
{
    return !nodep;
}

Bag_internal_node_ATTLC*
Bag_internal_item_ATTLC::next_node() const
{
#ifdef DEBUG_ATTLC
    assert(is_node());
#endif
    return nodep;
}

void*
Bag_internal_item_ATTLC::external_leaf() const
{
#ifdef DEBUG_ATTLC
    assert(is_leaf());
#endif
    return (void*)((long)ext_leaf & ~(long)01);
}

void
Bag_internal_item_ATTLC::make_leaf(void* p)
{
#ifdef DEBUG_ATTLC
    assert(((long)p & 01) == 0);
#endif
    ext_leaf = (void*)((long)p | 01);
}

void
Bag_internal_item_ATTLC::make_node(Bag_internal_node_ATTLC* cp)
{
#ifdef DEBUG_ATTLC
    assert(((long)cp & 01) == 0);
#endif
    nodep = cp;
}

void
Bag_internal_item_ATTLC::make_null()
{
    nodep = 0;
}

//  Bag_internal_node_ATTLC inlines

void
Bag_internal_node_ATTLC::operator delete(void* p)
{
    bag_internal_node_pool_ptr_ATTLC->free(p);
}

Bag_internal_node_ATTLC::~Bag_internal_node_ATTLC()
{
}

//  Bag_position_ATTLC inlines

Bag_position_ATTLC::Bag_position_ATTLC(): curr_depth(-1), curr_value(0)
{
}


//  The following are for public consumption
#ifdef __GNUG__
#pragma interface
#endif

template <class T> class Bag_pair;
template <class T> class Bag_bucket_ATTLC;
template <class T> class Bag_bucketiter_ATTLC;
template <class T> class Bagiter;
template <class T> class Bag;

//  Each leaf item in the tree-structured index
//  points to a pair of values:
//
//	o  'hashval' is the hash key
//	o  'collision_list' is a list of pairs of
//	   (values,count) pairs such that the value
//	   hashes to hashval
//
//  Note that ALIGN values in the original version
//  become Bag_bucket_ATTLC<T>* values in this version.
//

template <class T>
struct Bag_pair {
    T value;
    unsigned count;
    int operator==(const Bag_pair<T>& b) {
	return (b.value==value && b.count==count);
    }
};


template <class T>
struct Bag_bucket_ATTLC : public Bag_bucket_non_param_ATTLC {
    List< Bag_pair<T> > collision_list;
    void show() const;
};

template <class T>
class Bag_bucketiter_ATTLC {
    friend class Bag<T>;
protected:
    Bag<T>* my_bag;
    Bag_bucketiter_ATTLC<T>* next_it;
    Bag_position_ATTLC pos;
public:
    Bag_bucketiter_ATTLC(const Bag<T>&);
    Bag_bucketiter_ATTLC(const Bag_bucketiter_ATTLC<T>&);
    ~Bag_bucketiter_ATTLC();
    const Bag_bucket_ATTLC<T>* first();
    const Bag_bucket_ATTLC<T>* next();
    Bag_bucketiter_ATTLC& operator=(const Bag_bucketiter_ATTLC<T>&);
    void clobber() {
	pos.curr_depth = -1;
    }
};

template <class T>
class Bagiter : public Bag_bucketiter_ATTLC<T> {
private:
    int inited;
    Listiter< Bag_pair<T> >* itp;
public:
    Bagiter(const Bag<T>& b)
	: Bag_bucketiter_ATTLC<T>(b), inited(0), itp(0) {}
    Bagiter(const Bagiter<T>& bi)
	: Bag_bucketiter_ATTLC<T>(bi), inited(0), itp(0) {}
    ~Bagiter() {
	if (itp)
	    delete itp;
    }
    const Bag_pair<T>* first();
    const Bag_pair<T>* next();
    int next(const Bag_pair<T>*& t) {
	t = next();
	return t!=0;
    }
    void reset() {
	inited=0;
    }
    const Bag<T>* the_bag() const {
	return my_bag;
    }
    Bagiter<T>& operator=(const Bagiter<T>& bi) {
    	return (Bagiter<T>&)((Bag_bucketiter_ATTLC<T>&)(*this) =
			 (const Bag_bucketiter_ATTLC<T>&)bi);
    }
};


template <class T>
class Bag {
    friend class Bag_bucketiter_ATTLC<T>;
    friend class Bagiter<T>;

private:
    int sze;
    int	sze_unique;
    Bag_internal_item_ATTLC contents;
    Bag_bucketiter_ATTLC<T>* iter_head;
    Bag_position_ATTLC pos;

//  The following functions are only used
//  on empty Bags

    void make_setlogic(const Bag<T>&, const Bag<T>&, int);
    Bag(const Bag<T>&, const Bag<T>&, Bag_union_ATTLC*);
    Bag(const Bag<T>&, const Bag<T>&, Bag_inter_ATTLC*);
    Bag(const Bag<T>&, const Bag<T>&, Bag_diff_ATTLC*);
    Bag(const Bag<T>&, const Bag<T>&, Bag_xor_ATTLC*);
    void warn_iterators() const;

public:

//  Constructors

    Bag();
    Bag(const T&);
    Bag(const T&, const T&);
    Bag(const T&, const T&, const T&);
    Bag(const T&, const T&, const T&, const T&);
    Bag(const Bag<T>&);
    ~Bag();

//  Size

    unsigned int size() const {
	return (unsigned)sze;
    }
    unsigned int size_unique() const {
	return (unsigned)sze_unique;
    }
    operator const void*() const {
	return (size()? (this): (0));
    }

//  Assignment

    Bag<T>& operator=(const Bag<T>&);

//  Relations

    int operator==(const Bag<T>& b) const {
	return containment(b, 0);
    }
    int operator!=(const Bag<T>& b) const {
	return !(*this == b);
    }
    int operator<=(const Bag<T>& b) const {
	return containment(b, 1);
    }
    int operator<(const Bag<T>& b) const {
	return containment(b, 2);
    }
    int operator>=(const Bag<T>& b) const {
	return b <= *this;
    }
    int operator>(const Bag<T>& b) const {
	return b < *this;
    }
private:
    int containment(const Bag<T>&, int) const;
public:

//  Membership

    const Bag_pair<T>* contains(const T&) const;
    unsigned int count(const T&) const;

//  Insert and Remove elements

    const Bag_pair<T>* insert(const T&, int count=1);
    unsigned int remove(const T&, int count=1);
    unsigned int remove_all(const T&);
    unsigned int remove_all();

//  Select an arbitrary element

    const Bag_pair<T>* select() const;

//  Bag algebra

    Bag<T> operator|(const Bag<T>& b) const {
	return Bag<T>(*this,b,(Bag_union_ATTLC*)0);
    }
    Bag<T> operator-(const Bag<T>& b) const {
	return Bag<T>(*this, b,(Bag_diff_ATTLC*)0);
    }
    Bag<T> operator&(const Bag<T>& b) const {
	return Bag<T>(*this,b,(Bag_inter_ATTLC*)0);
    }
    Bag<T> operator^(const Bag<T>& b) const {
	return Bag<T>(*this,b,(Bag_xor_ATTLC*)0);
    }
    Bag<T>& operator|=(const Bag<T>&);
    Bag<T>& operator-=(const Bag<T>&);
    Bag<T>& operator&=(const Bag<T>&);
    Bag<T>& operator^=(const Bag<T>&);

//  Performance tuning

    void histogram(Map<Set_or_Bag_hashval, unsigned>&) const;

//  Debugging

    void check() const;
    void show() const;

//  Hash function

    static Set_or_Bag_hashval hash(const T&);

    ostream& print(ostream&) const;
};

template <class T>
ostream&
operator<<(ostream&, const Bag<T>&);

template <class T>
Set_or_Bag_hashval
Bag<T>::hash(const T&)
{
    return 0;
}


// Debugging code - don't want to require including <iostream.h>
// Developer can uncomment for debugging

#if 0
template <class T>
void Bag_bucket_ATTLC<T>::show() const
{
    cout << "    hashval=" << hashval << "\n";
    cout << "    collision_list=";
    Listiter< Bag_pair<T> > it(collision_list);
    int first=1;
    cout << "<";

    while ( !it.at_end() ) {
	if ( first )
	    first=0;
	else
	    cout << ",";

	Bag_pair<T>* result;
	it.next(result);
	cout << "(" << result->count << ","  << result->value << ")";
    }
    cout << ">\n";
}

template <class T>
void
Bag<T>::show() const
{
    cout << "sze = " << sze << "\n";
    cout << "sze_unique = " << sze_unique << "\n";
    cout << "pos =\n";
    pos.show();
    cout << "contents =\n";
    contents.show(0);
}

#endif

template <class T>
ostream&
operator<<(ostream& os,const Bag<T>& b)
{
    Bag_bucketiter_ATTLC<T> bi(b);
    const Bag_bucket_ATTLC<T>* bp = bi.first();
    os << "{";

    while ( bp ) {
	Listiter< Bag_pair<T> > li(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

	while ( !li.at_end() ) {
	    Bag_pair<T>* tp;
	    li.next(tp);
	    os 	<< "("
		<< tp->count
		<< ","
		<< tp->value
		<< ")"
	    ;
	}
	bp = bi.next();
    }
    os << "}";
    return os;
}

#if (defined(__edg_att_40) || defined(__GNUG__)) && !defined(__IMPLICIT_INCLUDE)
#include <bag.c>
#endif
#endif
