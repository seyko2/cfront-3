/*ident	"@(#)cls4:lib/new/_arr_map.c	1.5" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/


// manage the global table of array pointers and counts
#include <values.h>
#include <new.h>

// !!!!  only works if sizeof(long) >= sizeof(void*)  !!!!

typedef void*	KEYTYP;	// generic array address
int	__insert_new_array(KEYTYP key, int count);
	// key is a pointer to a new array.  It must
	//	be non-zero
	//	not already be in the table
	// count is the number of elements in the array.  May be zero
int	__remove_old_array(KEYTYP key);
	// removes an old array from the table.  Returns the count or -1 if not found

// choose 2 <= DIGIT_SIZE <= 6
// smaller numbers waste less space and are slower, larger numbers waste more
// space and are faster

// control the size of blocks grabbed from the free store (and frequency of
// allocations) with RECORDS_BLOCK_COUNT and NODES_BLOCK_COUNT

// a trie based on groups of DIGIT_SIZE bits from the key
#define DIGIT_SIZE	5
// each node contains 2 ** DIGIT_SIZE entries
#define NODE_SIZE	(1 << DIGIT_SIZE)
#define MASK_BITS	(1 << DIGIT_SIZE) - 1
// a few low order bits are uninteresting and are rotated around to the top
#define LOW_BITS	2
#define LOW_MASK	((1 << LOW_BITS) - 1)
// the number of groups in a word
#define POSITIONS	((BITS(long) - 1) / DIGIT_SIZE + 1)
// control the size of blocks grabbed from new
#define RECORDS_BLOCK_COUNT	150
#define NODES_BLOCK_COUNT	10

// avoid name space clashes
#define pnd __pnd
#define Node_Pool __Node_Pool
#define Record_Pool __Record_Pool
#define pnd_internal_node __pnd_internal_node

class pnd;
class pnd_internal_node;

class RECORD {
friend class pnd;
friend int	__insert_new_array(KEYTYP addr, int count);
	inline void*	operator new(size_t);
	inline void	operator delete(void* p, size_t);
		inline RECORD(unsigned long k, int cnt);
		inline ~RECORD();
	unsigned long	key;	// rotated address of array
	int	count;  // number of elements of array
};

class Record_Pool;

class Record_shell {
friend class Record_Pool;
	Record_shell*	next;
	char	dummy[sizeof(RECORD) - sizeof(Record_shell*)];
};

class Record_Pool {
friend class RECORD;
	static Record_shell*	top;
		Record_Pool();
	Record_shell	slot[RECORDS_BLOCK_COUNT];
	static void*	alloc() {
			void*	ans = 0;
			if (top == 0)
				new Record_Pool;
			if (top != 0) {
				ans = top;
				top = top->next;
			}
			return ans;
		}
	static void	free(Record_shell* p) {
			p->next = top;
			top = p;
		}
};
Record_shell*	Record_Pool::top = 0;  // definition of static member

inline void*
RECORD::operator new(size_t)
{
	return Record_Pool::alloc();
}
inline void
RECORD::operator delete(void* p, size_t)
{
	Record_Pool::free((Record_shell*)p);
}

inline
RECORD::RECORD(unsigned long k, int cnt)
: key(k), count(cnt)
{}

inline RECORD::~RECORD() {}

class pnd_internal_item {
friend class pnd;
friend class pnd_internal_node;
	union	{
		RECORD*	ext_leaf;
		pnd_internal_node*	nodep;
	};
	int	this_is_leaf;
	int	is_node() { return !this_is_leaf && nodep; }
	int	is_leaf() { return this_is_leaf; }
	int	is_null() { return !nodep; }
	pnd_internal_node*	next_node() { return nodep; }
	RECORD*	external_leaf() { return ext_leaf; }
	void	make_leaf(RECORD* p) { this_is_leaf = 1; ext_leaf = p; }
	void	make_node(pnd_internal_node* cp) { this_is_leaf = 0; nodep = cp; }
	void	make_null() {this_is_leaf = 0; nodep = 0; }
};

class Node_Pool;

class pnd_internal_node {
friend class pnd;
friend int	__insert_new_array(KEYTYP key, int count);
	inline void*	operator new(size_t i);
	inline void	operator delete(void* p, size_t i);
	pnd_internal_node();
	inline ~pnd_internal_node();
	pnd_internal_item	item[NODE_SIZE];
	int	busy_count;
};
	
class Node_shell {
friend class Node_Pool;
	Node_shell*	next;
	char	dummy[sizeof(pnd_internal_node) - sizeof(Node_shell*)];
};

class Node_Pool {
friend class pnd_internal_node;
	static Node_shell*	top;
		Node_Pool();
	Node_shell	slot[NODES_BLOCK_COUNT];
	static void*	alloc() {
			void*	ans = 0;
			if (top == 0)
				new Node_Pool;
			if (top != 0) {
				ans = top;
				top = top->next;
			}
			return ans;
		}
	static void	free(void* p) {
			((Node_shell*)p)->next = top;
			top = (Node_shell*)p;
		}
};
Node_shell*	Node_Pool::top = 0;  // definition of static member

inline void*
pnd_internal_node::operator new(size_t)
{
	return Node_Pool::alloc();
}
inline void
pnd_internal_node::operator delete(void* p, size_t)
{
	Node_Pool::free(p);
}

inline pnd_internal_node::~pnd_internal_node() {}

class pnd  {
friend int	__insert_new_array(KEYTYP key, int count);
friend int	__remove_old_array(KEYTYP key);
	static pnd*	the_table;
	pnd_internal_item	contents;
	// int	sze;
		pnd();
	static void	initialize();
	int	insert(KEYTYP, int);
	int	remove(KEYTYP);	// returns count or -1 if not found
};
pnd*	pnd::the_table = 0;  // definition of static member

int
__insert_new_array(KEYTYP key, int count)
{
	if (pnd::the_table == 0)
		pnd::initialize();
	if (pnd::the_table == 0)
		return -1;
	if (pnd::the_table->insert(key, count) == -1)
		return -1;
	return 0;
}

int
__remove_old_array(KEYTYP key)
{
	// the conditional resolves a perversion
	return pnd::the_table == 0 ? -1 : pnd::the_table->remove(key);
}

pnd_internal_node::pnd_internal_node()
{
	register pnd_internal_item*	itemp = &item[0];
	register int i = NODE_SIZE;
	while (i--)
		(itemp++)->make_null();
	busy_count = 0;
}

int
pnd::insert(KEYTYP addr, int cnt)
{
	register unsigned long	mask = MASK_BITS;
	register unsigned long	key = (unsigned long)addr & LOW_MASK;
	if (key)
		key <<= BITS(long) - LOW_BITS;
	key |= (unsigned long)addr >> LOW_BITS;
	RECORD*	new_rec = new RECORD(key, cnt);
	register int	unshift = 0;
	register pnd_internal_item*	itemp = &contents;
	register pnd_internal_node*	nodep = 0;
	if (new_rec == 0)
		return -1;
	for (;; mask <<= DIGIT_SIZE, unshift += DIGIT_SIZE) {
		if (itemp->is_null())
			break;
		if (itemp->is_leaf()) {
			if (itemp->external_leaf()->key == key){
				itemp->external_leaf()->count = cnt;
				delete new_rec;  // didn't need it after all
				return 0;	// should rarely happen
			}
			break;
		}
		nodep = itemp->next_node();
		// assert(nodep);
		itemp = &nodep->item[(key & mask) >> unshift];
	}
	// sze++;
	if (itemp->is_null()) {
		itemp->make_leaf(new_rec);
		if (nodep)
			nodep->busy_count++;
		return 0;
	}
	// assert(itemp->is_leaf());
	RECORD*	temp = itemp->external_leaf();
	unsigned long	other_key = temp->key;
	// assert(other_key != key);
	unsigned long	ind1, ind2;
	for (;; mask <<= DIGIT_SIZE, unshift += DIGIT_SIZE) {
		nodep = new pnd_internal_node;
		if (nodep == 0)
			return -1;
		itemp->make_node(nodep);
		// assert(pos.curr_depth < POSITIONS);
		ind1 = (other_key & mask) >> unshift;
		ind2 = (key & mask) >> unshift;
		if (ind1 != ind2) break;	// I hope so!
		nodep->busy_count = 1;
		itemp = &nodep->item[ind1];
	}
	nodep->item[ind1].make_leaf(temp);
	nodep->item[ind2].make_leaf(new_rec);
	nodep->busy_count = 2;
	return 0;
}

int
pnd::remove(KEYTYP addr)
{
	pnd_internal_item*	curr_pos[POSITIONS];
	int	curr_depth;
	if (addr == 0) return -1;
	register unsigned long	mask = MASK_BITS;
	register unsigned long	key = (unsigned long)addr & LOW_MASK;
	if (key)
		key <<= BITS(long) - LOW_BITS;
	key |= (unsigned long)addr >> LOW_BITS;
	register int	unshift = 0;
	register pnd_internal_item*	itemp = &contents;
	register pnd_internal_node*	nodep = 0;
	for (curr_depth = -1;; mask <<= DIGIT_SIZE, unshift += DIGIT_SIZE) {
		// assert(curr_depth < POSITIONS);
		if (itemp->is_null())
			return -1;
		if (itemp->is_leaf())
			if (itemp->external_leaf()->key == key)
				break;
			else
				return -1;
		nodep = itemp->next_node();
		// assert(nodep);
		curr_pos[++curr_depth] = itemp =
				&nodep->item[(key & mask) >> unshift];
	}
	// assert(itemp->is_leaf());
	// assert(itemp->external_leaf()->key == key);
	RECORD*	old_p = itemp->external_leaf();
	// sze--;
	int	ans = old_p->count;
	delete old_p;
	itemp->make_null();
	if (curr_depth == -1 ||	// it was a singleton set
			--(nodep->busy_count) > 1)	// easy case
		return ans;
	// assert(nodep->busy_count == 1);
	register pnd_internal_item*	itp = &nodep->item[0];
	while (itp->is_null() || itp == itemp) itp++;
	if (!itp->is_leaf())	// unfortunate case
		return ans;
	RECORD*	temp = itp->external_leaf();
	// assert(temp->key != key);
	for (;;) {
		delete nodep;
		if (curr_depth-- == 0) {
			// assert(sze == 1);
			contents.make_leaf(temp);
			return ans;
		}
		nodep = curr_depth == 0 ? contents.next_node() :
				curr_pos[curr_depth-1]->next_node();
		if (nodep->busy_count > 1) {
			curr_pos[curr_depth]->make_leaf(temp);
			return ans;
		}
		// assert(nodep->busy_count == 1);
	}
}

pnd::pnd()
/* : sze(0) */
{
	contents.make_null();
}

void
pnd::initialize()
{
	the_table = new pnd;
}

Record_Pool::Record_Pool()
{
	register int	i = RECORDS_BLOCK_COUNT;
	register Record_shell*	p = slot;
	register Record_shell*	q;
	while (--i) {
		q = p++;
		q->next = p;
	}
	p->next = top;
	top = slot;
}

Node_Pool::Node_Pool()
{
	register int	i = NODES_BLOCK_COUNT;
	register Node_shell*	p = slot;
	register Node_shell*	q;
	while (--i) {
		q = p++;
		q->next = p;
	}
	p->next = top;
	top = slot;
}


