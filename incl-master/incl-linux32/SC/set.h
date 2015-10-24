/*ident	"@(#)Set:incl/set.h	3.1" */
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

#ifndef SETH
#define SETH

//  The following include is required so that the instantiation
//  files that include set.h will get what they need from Set.h

#include <Set.h>

class ostream;

//  The 32 bits of a Set_or_Bag_hashval are broken into
//  8 4-bit groups.  Each 4-bit group indexes
//  into a Internal_node of 16 Internal_items.

static const int SET_NODE_SIZE_ATTLC = 16;
static const int SET_POSITIONS_ATTLC = 8;
static const int SET_MASK_BITS_ATTLC = 0XF;
static const int SET_INITIAL_SHIFT_ATTLC = 0;
static const int SET_SHIFT_INCR_ATTLC = 4;

extern int SET_LT_ATTLC( Set_or_Bag_hashval a, Set_or_Bag_hashval b );

struct Space_info_ATTLC {
    int max_level;			// max depth of tree
    long node_count;			// total nodes
    long leaf_count;    		// total leaves
    long overhead;			// size of data structure
    int leaves[SET_POSITIONS_ATTLC];	// leaves at each level
    int occupancy[SET_POSITIONS_ATTLC]; // non-null items at each level
};

//  Index to non-parametric classes:

class Set_bucket_non_param_ATTLC;
class Internal_item_ATTLC;
class Internal_node_ATTLC;
class Position_ATTLC;

//  The following ATTLC class declarations are
//  needed to disambiguate the copy-avoidance
//  constructors:

class Set_select_ATTLC;
class Set_reject_ATTLC;
class Set_map_ATTLC;
class Set_union_ATTLC;
class Set_inter_ATTLC;
class Set_diff_ATTLC;
class Set_xor_ATTLC;

extern Pool* internal_node_pool_ptr_ATTLC;

struct Set_bucket_non_param_ATTLC {
    Set_or_Bag_hashval hashval;
};

struct Internal_item_ATTLC
{
    //  An Internal_item is a single "slot" in an
    //  Internal_node.  It may contain:
    //
    //	o  An Internal_node*
    //	o  A Set_bucket_non_param_ATTLC*
    //	o  NULL
    //
    //  Set_bucket_non_param_ATTLC* and Internal_node* pointers 
    //  can't be byte-aligned, so the low order bit is free to
    //  be used as the union discriminant.
    //
    //   o  When the low-order bit is 1, the item
    //	    is a Set_bucket_non_param_ATTLC*
    //
    //   o  When the low order bit is 0, the item
    //	    is a Internal_node* or NULL.

    union {
	void* ext_leaf;
	Internal_node_ATTLC* nodep;
	long this_is_leaf;
    };
    inline int is_leaf_or_null() const;

    inline int is_node() const;
    inline int is_leaf() const;
    inline int is_null() const;
    inline Internal_node_ATTLC* next_node() const;
    inline void* external_leaf() const;
    inline void make_leaf(void*);
    inline void make_node(Internal_node_ATTLC*);
    inline void make_null();

    void show(int) const;
    void space(int,Space_info_ATTLC&) const;
};

struct Internal_node_ATTLC {
    //  A Internal_node has SET_NODE_SIZE_ATTLC Internal_items
    //  plus a count of how many of these are in use.
    //  Each Internal_item is directly indexed
    //  by a lg(SET_NODE_SIZE_ATTLC) chunk.

    static void initialize();
    Internal_item_ATTLC item[SET_NODE_SIZE_ATTLC];
    int busy_count;
    Internal_node_ATTLC();
    inline ~Internal_node_ATTLC();
    void* operator new(size_t);
    inline void operator delete(void*);
    void show(int) const;
    void space(int,Space_info_ATTLC&) const;
};

struct Position_ATTLC {
    //  A Position caches the result of the most
    //  recent search, down to the level of the
    //  Bucket:
    //
    //	o  curr_value is the Set_or_Bag_hashval that we
    //	   searched for
    //
    //	o  curr_pos is a stack of pointers to
    //	   Internal_items which remembers a
    //	   path of Internal_items through the
    //	   index:
    //
    //	   o  curr_pos[0] points to the Internal_item
    //	      selected by the first 4 bit chunk
    //
    //	   o  curr_pos[1] points to the Internal_item
    //	      selected by the second 4 bit chunk
    //
    //	   o  curr_pos[curr_depth] points to the
    //	      Internal_item which points to
    //	      a Bucket_ATTLC(T)
    //
    //  If set mutation occurs, a Position is
    //  canceled by setting its curr_depth to -1;
    //  curr_value can be used subsequently to rebuild
    //  the cur_pos stack so that iteration can resume
    //

    Internal_item_ATTLC* curr_pos[SET_POSITIONS_ATTLC];
    int curr_depth;
    Set_or_Bag_hashval curr_value;

    inline Position_ATTLC();
    void show() const;
};

//  Internal_item inlines

int
Internal_item_ATTLC::is_leaf_or_null() const
{
    return this_is_leaf & 01;
}

int
Internal_item_ATTLC::is_node() const
{
    return !is_leaf_or_null() && nodep;
}

int
Internal_item_ATTLC::is_leaf() const
{
    return is_leaf_or_null() && ((long)ext_leaf & ~(long)01);
}

int
Internal_item_ATTLC::is_null() const
{
    return !nodep;
}

Internal_node_ATTLC*
Internal_item_ATTLC::next_node() const
{
#ifdef DEBUG_ATTLC
    assert(is_node());
#endif
    return nodep;
}

void*
Internal_item_ATTLC::external_leaf() const
{
#ifdef DEBUG_ATTLC
    assert(is_leaf());
#endif
    return (void*)((long)ext_leaf & ~(long)01);
}

void
Internal_item_ATTLC::make_leaf(void* p)
{
#ifdef DEBUG_ATTLC
    assert(((long)p & 01) == 0);
#endif
    ext_leaf = (void*)((long)p | 01);
}

void
Internal_item_ATTLC::make_node(Internal_node_ATTLC* cp)
{
#ifdef DEBUG_ATTLC
    assert(((long)cp & 01) == 0);
#endif
    nodep = cp;
}

void
Internal_item_ATTLC::make_null()
{
    nodep = 0;
}

//  Internal_node inlines

void
Internal_node_ATTLC::operator delete(void* p)
{
    internal_node_pool_ptr_ATTLC->free(p);
}

Internal_node_ATTLC::~Internal_node_ATTLC()
{
}

//  Position inlines

Position_ATTLC::Position_ATTLC() : curr_depth(-1), curr_value(0)  //  curr_value probably not needed
{
}


#ifdef __GNUG__
#pragma interface
#endif

template <class T> class Bucket_ATTLC;
template <class T> class Bucketiter_ATTLC;

//  The following classes are for public consumption:

template <class T> class Setiter;
template <class T> class Set;

//  Each leaf item in the tree-structured index
//  points to a pair of values:
//
//	o  'hashval' is the hash key
//	o  'collision_list' is a list of values which
//	   hash to hashval
//
//  Note that ALIGN values in the original version
//  become Bucket_ATTLC<T>* values in this version.
//

template <class T>
struct Bucket_ATTLC : public Set_bucket_non_param_ATTLC
{
    List<T> collision_list;
    void show() const;
};

template <class T>
class Bucketiter_ATTLC
{
    friend class Set<T>;
protected:
    Set<T>* my_set;
    Bucketiter_ATTLC<T>* next_it;
    Position_ATTLC pos;
public:
    Bucketiter_ATTLC(const Set<T>&);
    Bucketiter_ATTLC(const Bucketiter_ATTLC<T>&);
    ~Bucketiter_ATTLC();
    const Bucket_ATTLC<T>* first();
    const Bucket_ATTLC<T>* next();
    void clobber() { pos.curr_depth = -1; }
    Bucketiter_ATTLC<T>& operator=(const Bucketiter_ATTLC<T>&);
};

//  This class must be modified to use
//  Bucketiter_ATTLC<T>(*this).first() and next().

template <class T>
class Setiter : public Bucketiter_ATTLC<T>
{
private:
    int inited;					// new
    Listiter<T>* itp;
public:
    const Set<T>* the_set() const {	// new
	return my_set;
    }
    Setiter(const Set<T>& s)
	: Bucketiter_ATTLC<T>(s), inited(0), itp(0) {}
    Setiter(const Setiter<T>& si)
	: Bucketiter_ATTLC<T>(*(si.the_set())), inited(0), itp(0) {}
    ~Setiter() {
	if (itp)
	    delete itp;
    }
    const T* first();			// undocumented - TBD_make private
    const T* next();			// subsumes first, next
    int next(const T*& t) {		// new
	t = next();
	return t!=0;
    }
    void reset() {			// new semantics
	inited=0;
    }
    Setiter<T>& operator=(const Setiter<T>& si) {
	return (Setiter<T>&)((Bucketiter_ATTLC<T>&)*this =
			     (const Bucketiter_ATTLC<T>&)si);
    }
};

template <class T>
class Set
{
    friend class Bucketiter_ATTLC<T>;
    friend class Setiter<T>;
private:
    int sze;
    Internal_item_ATTLC contents;
    Bucketiter_ATTLC<T>* iter_head;	// was Setiter<T>
    Position_ATTLC pos;			// was Position<T>

//  The following are only used on empty sets

    void make_setlogic(const Set<T>&, const Set<T>&, int);

    Set(const Set<T>&, const Set<T>&, Set_union_ATTLC*);
    Set(const Set<T>&, const Set<T>&, Set_inter_ATTLC*);
    Set(const Set<T>&, const Set<T>&, Set_diff_ATTLC*);
    Set(const Set<T>&, const Set<T>&, Set_xor_ATTLC*);
    void warn_iterators() const;

public:

//  Constructors

    Set() : sze(0), iter_head(0) {
	pos.curr_depth = -1;
	contents.make_null();
    }
    Set(const T&);
    Set(const T&, const T&);
    Set(const T&, const T&, const T&);
    Set(const T&, const T&, const T&, const T&);
    Set(const Set<T>&);
    ~Set();

//  Size

    unsigned int size() const {
	return (unsigned)sze;
    }
    unsigned int size_unique() const {
	return (unsigned)sze;
    }
    operator const void*() const {
	return (size()? (this): (0));
    }

//  Assignment

    Set<T>& operator=(const Set<T>&);

//  Relations

    int operator==(const Set<T>& b) const {
	return containment(b, 0);
    }
    int operator!=(const Set<T>& s) const {
	return !(*this == s);
    }
    int operator<=(const Set<T>& b) const {
	return containment(b, 1);
    }
    int operator<(const Set<T>& b) const {
	return containment(b, 2);
    }
    int operator>=(const Set<T>& s) const {
	return s <= *this;
    }
    int operator>(const Set<T>& s) const {
	return s < *this;
    }
private:
    int containment(const Set<T>&, int) const;
public:

//  Membership

    const T* contains(const T&) const;
    unsigned int count(const T& t) const {
	return contains(t)?1:0;
    }

//  Insert and Remove elements

    const T* insert(const T&, int=1);
    unsigned remove(const T&, int=1);
    unsigned remove_all(const T& t) {
	return remove(t);
    }
    unsigned remove_all();

//  Select an arbitrary element

    const T* select() const;

//  Set algebra

    Set<T> operator|(const Set<T>& s) const {
	return Set<T>(*this,s,(Set_union_ATTLC*)0);
    }
    Set<T> operator-(const Set<T>& s) const {
	return Set<T>(*this, s,(Set_diff_ATTLC*)0);
    }
    Set<T> operator&(const Set<T>& s) const {
	return Set<T>(*this,s,(Set_inter_ATTLC*)0);
    }
    Set<T> operator^(const Set<T>& s) const {
	return Set<T>(*this,s,(Set_xor_ATTLC*)0);
    }
    Set<T>& operator|=(const Set<T>&);
    Set<T>& operator-=(const Set<T>&);
    Set<T>& operator&=(const Set<T>&);
    Set<T>& operator^=(const Set<T>&);

//  Performance tuning

    void histogram(Map<Set_or_Bag_hashval, unsigned>&) const;

//  Debugging, performance tuning

    void check() const;
    void show() const;
    Space_info_ATTLC space() const;

//  Hash function

    static Set_or_Bag_hashval hash(const T&);	// user specializes if needed

    ostream& print(ostream&) const;
};

template <class T>
ostream&
operator<<(ostream&, const Set<T>&);

template <class T>
Set_or_Bag_hashval
Set<T>::hash(const T&)
{
    return 0;
}


// Debugging code - don't want to require including <iostream.h>
// Developer can uncomment for debugging

#if 0
template <class T>
void Bucket_ATTLC<T>::show() const
{
    cout << "    hashval=" << hashval << "\n";
    cout << "    collision_list=";
    Listiter<T> it(collision_list);
    int first=1;
    cout << "<";

    while(!it.at_end()) {
	if ( first )
	    first=0;
	else
	    cout << ",";

	cout << it.next();
    }
    cout << ">\n";
}

template <class T>
static void showiter(Listiter<T>* itp)
{
    T e;

    if ( itp ) {
	if ( itp->peek_prev(e) )
	    cout << e;
	else
	    cout << "*";

	if ( itp->peek_next(e) )
	    cout << e;
	else
	    cout << "*";

	cout << ">\n";
    }
    else
	cout << "0";
}

template <class T>
void
Set<T>::show() const
{
    cout << "sze = " << sze << "\n";
    cout << "pos =\n";
    pos.show();
    cerr << "contents =\n";
    contents.show(0);
}

template <class T>
Space_info_ATTLC
Set<T>::space() const
{
    //  contents is level 0; the first node is level 1, etc.

    Space_info_ATTLC result;
    result.max_level = -1;
    result.node_count = 0;
    result.leaf_count = 0;

    if( !contents.is_null() ) {
	int i;
	for ( i=0 ; i<SET_POSITIONS_ATTLC ; i++) {
	    result.leaves[i] = 0;
	    result.occupancy[i] = 0;
	}
	result.occupancy[0] = 1;
	contents.space(0,result);
    }
    result.overhead = (sizeof(Internal_item_ATTLC) +
		       result.node_count * sizeof(Internal_node_ATTLC));
    return result;
}

#endif

template <class T>
ostream& operator<<(ostream& os, const Set<T>& s)
{
    os << "{";
    Setiter<T> it(s);
    const T* tp;
    int first=1;

    while( tp = it.next() ) {
	if ( first )
	    first=0;
	else
	    os << ",";

	os << *tp;
    }
    os << "}";
    return os;
}

#if (defined(__edg_att_40) || defined(__GNUG__)) && !defined(__IMPLICIT_INCLUDE)
#include <set.c>
#endif
#endif
