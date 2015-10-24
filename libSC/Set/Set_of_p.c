/*ident	"@(#)Set:Set_of_p.c	3.1" */
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

#include <Set.h>
#include <assert.h>

// curr_depth is always the last valid position in curr_pos[]
// curr_pos[] only contains pointers to internal items
// in case there are no valid entries in curr_pos[], curr_depth == -1
#define CURR_VALUE	((unsigned long)pos.curr_value)
#define P	((unsigned long)p)

void
pst_internal_node_ATTLC::initialize() {
	if (pst_internal_node_pool_ptr_ATTLC == 0)
		pst_internal_node_pool_ptr_ATTLC = new Pool(sizeof(pst_internal_node_ATTLC));
}

void* 
pst_internal_node_ATTLC::operator new(size_t i) { 
	initialize();
	return i == sizeof(pst_internal_node_ATTLC) ?
	pst_internal_node_pool_ptr_ATTLC->alloc() : new char[i]; 
}

int
pst_new_iter_ATTLC::peek_next(ALIGN& targ) const
{
    pst_iter_old_ATTLC* cfront_cludge = &(((pst_new_iter_ATTLC*)this)->it);
    if (!inited) {
	((pst_new_iter_ATTLC*)this)->inited = 1;
	((pst_new_iter_ATTLC*)this)->nxt = cfront_cludge->first();
    } else if (!nxt)
	((pst_new_iter_ATTLC*)this)->nxt = cfront_cludge->next();

    if (nxt) {
	targ = nxt;
	return 1;
    }
    return 0;
}

ALIGN
pst_new_iter_ATTLC::peek_next() const
{
    pst_iter_old_ATTLC* cfront_cludge = &(((pst_new_iter_ATTLC*)this)->it);
    if (!inited) {
	((pst_new_iter_ATTLC*)this)->inited = 1;
	((pst_new_iter_ATTLC*)this)->nxt = cfront_cludge->first();
    } else if (!nxt)
	((pst_new_iter_ATTLC*)this)->nxt = cfront_cludge->next();
    return nxt;
}

int
pst_new_iter_ATTLC::peek_prev(ALIGN& targ) const
{
	if (!inited)
		return 0;
	if (prv) {
		targ = prv;
		return 1;
	}
	return 0;
}

int
pst_new_iter_ATTLC::next(ALIGN& targ)
{
	if (!inited) {
		inited = 1;
		nxt = it.first();
	} else if (!nxt)
		nxt = it.next();
	if (nxt) {
		targ = prv = nxt;
		nxt = 0;
		return 1;
	}
	return 0;
}

ALIGN
pst_new_iter_ATTLC::next()
{
	if (!inited) {
		inited = 1;
		nxt = it.first();
	} else if (!nxt)
		nxt = it.next();
	prv = nxt;
	nxt = 0;
	return prv;
}

pst_internal_node_ATTLC::pst_internal_node_ATTLC()
{
	register pst_internal_item_ATTLC* itemp = &item[0];
	register int i = PSET_NODE_SIZE_ATTLC;
	while (i--)
		(itemp++)->make_null();
	busy_count = 0;
}

ALIGN
pst_ATTLC::contains(register const ALIGN p) const
{
	if (p == 0)
		return 0;

	register long	mask = PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC;
	int depth;
	for (depth = -1; depth < pos.curr_depth; depth++, mask <<= PSET_SHIFT_INCR_ATTLC)
		if ((CURR_VALUE & mask) != (P & mask))
			break;

	register int	unshift = PSET_INITIAL_SHIFT_ATTLC + PSET_SHIFT_INCR_ATTLC + depth * PSET_SHIFT_INCR_ATTLC;
	register pst_internal_item_ATTLC* itemp = depth == -1 ?
				&(((pst_ATTLC *)this)->contents) :
				pos.curr_pos[depth];
	register pst_internal_node_ATTLC* nodep = 0;
	((pst_position_ATTLC *) &pos)->curr_value = p;

	for (((pst_position_ATTLC *) &pos)->curr_depth = depth;; mask <<= PSET_SHIFT_INCR_ATTLC,
					unshift += PSET_SHIFT_INCR_ATTLC) {
		assert(pos.curr_depth < PSET_POSITIONS_ATTLC);
		if (itemp->is_null())
			return 0;
		if (itemp->is_leaf())
			return itemp->external_leaf() == p?p:0;   /* jfi */
		nodep = itemp->next_node();
		assert(nodep);
		((pst_position_ATTLC *) &pos)->curr_pos[++((pst_position_ATTLC *) &pos)->curr_depth] = itemp =
				&nodep->item[(P & mask) >> unshift];
	}
}

inline int
funny_less_than_ATTLC(const ALIGN a, const ALIGN b)
{
#define A ((unsigned long)a)
#define B ((unsigned long)b)
#define CONST1 (PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC)
#define CONST2 (PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC + PSET_SHIFT_INCR_ATTLC)
#define CONST3 (PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC + 2 * PSET_SHIFT_INCR_ATTLC)
#define CONST4 (PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC + 3 * PSET_SHIFT_INCR_ATTLC)
#define CONST5 (PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC + 4 * PSET_SHIFT_INCR_ATTLC)
#define CONST6 (PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC + 5 * PSET_SHIFT_INCR_ATTLC)
	register unsigned long	twa;
	register unsigned long	twb;
/*
	twa = (A & CONST1);
	twb = (B & CONST1);
	if (twa < twb) return 1;
	if (twa > twb) return 0;
	twa = (A & CONST2);
	twb = (B & CONST2);
	if (twa < twb) return 1;
	if (twa > twb) return 0;
	twa = (A & CONST3);
	twb = (B & CONST3);
	if (twa < twb) return 1;
	if (twa > twb) return 0;
	twa = (A & CONST4);
	twb = (B & CONST4);
	if (twa < twb) return 1;
	if (twa > twb) return 0;
	twa = (A & CONST5);
	twb = (B & CONST5);
	if (twa < twb) return 1;
	if (twa > twb) return 0;
	twa = (A & CONST6);
	twb = (B & CONST6);
	if (twa < twb) return 1;
	return 0;
*/
	return 	(twa = (A & CONST1)) < (twb = (B & CONST1)) ? 1 :
				(twa > twb ? 0 :
			(twa = (A & CONST2)) < (twb = (B & CONST2)) ? 1 :
				(twa > twb ? 0 :
			(twa = (A & CONST3)) < (twb = (B & CONST3)) ? 1 :
				(twa > twb ? 0 :
			(twa = (A & CONST4)) < (twb = (B & CONST4)) ? 1 :
				(twa > twb ? 0 :
			(twa = (A & CONST5)) < (twb = (B & CONST5)) ? 1 :
				(twa > twb ? 0 :
			(twa = (A & (unsigned long)CONST6)) < (twb = (B & (unsigned long)CONST6)) ? 1 :
				0)))));

#undef A
#undef B
#undef CONST1
#undef CONST2
#undef CONST3
#undef CONST4
#undef CONST5
#undef CONST6
}

ALIGN
pst_ATTLC::insert(ALIGN p)
{
	if (p == 0) return 0;
	register long	mask = PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC;
	int depth;
	for (depth = -1; depth < pos.curr_depth; depth++, mask <<= PSET_SHIFT_INCR_ATTLC)
		if ((CURR_VALUE & mask) != (P & mask))
			break;
	register int	unshift = PSET_INITIAL_SHIFT_ATTLC + PSET_SHIFT_INCR_ATTLC + depth * PSET_SHIFT_INCR_ATTLC;
	register pst_internal_item_ATTLC* itemp = depth == -1 ?
				&contents : pos.curr_pos[depth];
	register pst_internal_node_ATTLC* nodep = 0;
	pos.curr_value = p;
	register pst_iter_old_ATTLC* it;
	for (it = iter_head; it; it = it->next_it)
		it->curr_depth = -1;
	for (pos.curr_depth = depth;; mask <<= PSET_SHIFT_INCR_ATTLC,
					unshift += PSET_SHIFT_INCR_ATTLC) {
		assert(pos.curr_depth < PSET_POSITIONS_ATTLC);
		if (itemp->is_null())
			break;
		if (itemp->is_leaf())
			if (itemp->external_leaf() == p)
				return 0;
			else
				break;
		nodep = itemp->next_node();
		assert(nodep);
		pos.curr_pos[++pos.curr_depth] = itemp =
				&nodep->item[(P & mask) >> unshift];
	}
	sze++;
	if (itemp->is_null()) {
		itemp->make_leaf(p);
		if (pos.curr_depth > 0)
			pos.curr_pos[pos.curr_depth-1]->next_node()->busy_count++;
		else if (pos.curr_depth == 0)
			contents.next_node()->busy_count++;
		return p;
	}
	assert(itemp->is_leaf());
	ALIGN	temp = itemp->external_leaf();
	assert(temp != p);
	int	ind1, ind2;
	for (;; mask <<= PSET_SHIFT_INCR_ATTLC, unshift += PSET_SHIFT_INCR_ATTLC) {
		itemp->make_node(nodep = new pst_internal_node_ATTLC);
		assert(pos.curr_depth < PSET_POSITIONS_ATTLC);
		ind1 = int(((unsigned long)temp & mask) >> unshift);
		ind2 = int((P & mask) >> unshift);
		if (ind1 != ind2) break;	// I hope so!
		nodep->busy_count = 1;
		itemp = &nodep->item[ind1];
		pos.curr_pos[++pos.curr_depth] = itemp;
	}
	nodep->item[ind1].make_leaf(temp);
	(itemp = &nodep->item[ind2])->make_leaf(p);
	pos.curr_pos[++pos.curr_depth] = itemp;
	nodep->busy_count = 2;
	return p;
}

void
pst_ATTLC::check() const
{
#ifndef NDEBUG
	pst_iter_old_ATTLC	it(*this);
	register ALIGN	old_p = 0;
	int	count = 0;
	register ALIGN p;
	for (p = it.first(); p; p = it.next()) {
		assert(funny_less_than_ATTLC(old_p, p));
		old_p = p;
		count++;
	}
	assert(count==sze);
#endif
}

unsigned
pst_ATTLC::remove(const ALIGN p)
{
	if (p == 0) return 0;
	register long	mask = PSET_MASK_BITS_ATTLC << PSET_INITIAL_SHIFT_ATTLC;
	int depth;
	for (depth = -1; depth < pos.curr_depth; depth++, mask <<= PSET_SHIFT_INCR_ATTLC)
		if ((CURR_VALUE & mask) != (P & mask))
			break;
	register int	unshift = PSET_INITIAL_SHIFT_ATTLC + PSET_SHIFT_INCR_ATTLC + depth * PSET_SHIFT_INCR_ATTLC;
	register pst_internal_item_ATTLC* itemp = depth == -1 ?
				&contents : pos.curr_pos[depth];
	register pst_internal_node_ATTLC* nodep = 0;
	pos.curr_value = p;
	warn_iterators();
	for (pos.curr_depth = depth;; mask <<= PSET_SHIFT_INCR_ATTLC,
					unshift += PSET_SHIFT_INCR_ATTLC) {
		assert(pos.curr_depth < PSET_POSITIONS_ATTLC);
		if (itemp->is_null())
			return 0;
		if (itemp->is_leaf())
			if (itemp->external_leaf() == p)
				break;
			else
				return 0;
		nodep = itemp->next_node();
		assert(nodep);
		pos.curr_pos[++pos.curr_depth] = itemp =
				&nodep->item[(P & mask) >> unshift];
	}
	assert(itemp->is_leaf());
	assert(itemp->external_leaf() == p);
	sze--;
	if (pos.curr_depth == -1) {	// it was a singleton set
		assert(sze == 0);
		itemp->make_null();
		return 1;
	}
	if (nodep == 0)
		nodep = pos.curr_depth == 0 ? contents.next_node() :
				pos.curr_pos[pos.curr_depth-1]->next_node();
	if (--(nodep->busy_count) > 1) {	// easy case
		itemp->make_null();
		return 1;
	}
	assert(nodep->busy_count == 1);
	register pst_internal_item_ATTLC* itp;
	for (itp = &nodep->item[0];
			itp < &nodep->item[PSET_NODE_SIZE_ATTLC]; itp++)
		if (itp != itemp && !itp->is_null())
			break;
	if (!itp->is_leaf()) {	// unfortunate case
		itemp->make_null();
		return 1;
	}
	ALIGN	temp = itp->external_leaf();
	assert(temp != p);
	for (;;) {
		delete nodep;
		if (pos.curr_depth-- == 0) {
			assert(sze == 1);
			contents.make_leaf(temp);
			return 1;
		}
		nodep = pos.curr_depth == 0 ? contents.next_node() :
				pos.curr_pos[pos.curr_depth-1]->next_node();
		if (nodep->busy_count > 1) {
			pos.curr_pos[pos.curr_depth]->make_leaf(temp);
			return 1;
		}
		assert(nodep->busy_count == 1);
	}
}

unsigned
pst_ATTLC::remove_all()
{
	unsigned result = sze;
	warn_iterators();
	register pst_internal_item_ATTLC* itemp = &contents;
	if (itemp->is_null()) {
		assert(sze == 0);
		return 0;
	}
	if (itemp->is_leaf()) {
		assert(sze == 1);
		itemp->make_null();
		sze = 0;
		return result;
	}
	register pst_internal_node_ATTLC* nodep = itemp->next_node();
	itemp = &nodep->item[0];
	pos.curr_depth = -1;
	for(;;) {
		register pst_internal_item_ATTLC* stopper = &nodep->item[PSET_NODE_SIZE_ATTLC];
		for ( ; itemp < stopper; /* itemp++ */)
			if (itemp->is_node()) {
				pos.curr_pos[++pos.curr_depth] = itemp;
				nodep = itemp->next_node();
				itemp = &nodep->item[0];
				stopper = &nodep->item[PSET_NODE_SIZE_ATTLC];
			}
			else itemp++;
		delete nodep;
		if (pos.curr_depth < 0)
			break;
		itemp = pos.curr_pos[pos.curr_depth--] + 1;
		nodep = pos.curr_depth < 0 ? contents.next_node() :
				pos.curr_pos[pos.curr_depth]->next_node();
	}
	contents.make_null();
	sze = 0;
	return result;
}

ALIGN
pst_iter_old_ATTLC::first() 
{
	if (my_pst == 0)
		return 0;
	curr_depth = -1;
	if (my_pst->contents.is_null())
		return 0;
	if (my_pst->contents.is_leaf())
		return curr_value = my_pst->contents.external_leaf();
	register pst_internal_item_ATTLC* itemp;
	register pst_internal_node_ATTLC* nodep = my_pst->contents.next_node();
	for (;;) {
		assert(curr_depth < PSET_POSITIONS_ATTLC);
		for (itemp = &nodep->item[0];; itemp++) {
			assert(itemp < &nodep->item[PSET_NODE_SIZE_ATTLC]);
			if (!itemp->is_null())
				break;
		}
		curr_pos[++curr_depth] = itemp;
		if (itemp->is_leaf())
			break;
		nodep = itemp->next_node();
	}
	return curr_value = itemp->external_leaf();
}

ALIGN
pst_iter_old_ATTLC::next()
{
	if (my_pst == 0 || curr_value == 0)
		return 0;
	int	unshift = PSET_INITIAL_SHIFT_ATTLC + PSET_SHIFT_INCR_ATTLC + curr_depth * PSET_SHIFT_INCR_ATTLC;
	register long	mask = PSET_MASK_BITS_ATTLC << unshift;
	register pst_internal_item_ATTLC* itemp = curr_depth == -1 ?
				&my_pst->contents : curr_pos[curr_depth];
	register pst_internal_node_ATTLC* nodep = 0;
	for (;; mask <<= PSET_SHIFT_INCR_ATTLC, unshift += PSET_SHIFT_INCR_ATTLC) {
		assert(curr_depth < PSET_POSITIONS_ATTLC);
		if (itemp->is_leaf())
			if (itemp->external_leaf() == curr_value ||
					funny_less_than_ATTLC(itemp->external_leaf(),
							curr_value))
				break;
			else
				return curr_value = itemp->external_leaf();
		if (itemp->is_null())
			break;
		nodep = itemp->next_node();
		assert(nodep);
		curr_pos[++curr_depth] = itemp =
				&nodep->item[(((unsigned long)curr_value) & mask) >>
					unshift];
	}
	if (curr_depth == -1)
		return curr_value = 0;
	if (nodep == 0)
		nodep = curr_depth == 0 ? my_pst->contents.next_node() :
				curr_pos[curr_depth-1]->next_node();
	for (;;) {
		assert(&nodep->item[0] <= itemp  &&  itemp < &nodep->item[PSET_NODE_SIZE_ATTLC]);
		while (itemp < &nodep->item[PSET_NODE_SIZE_ATTLC-1])
			if (!(++itemp)->is_null())
				goto break_twice;
		if (curr_depth-- == 0)
			return curr_value = 0;
		itemp = curr_pos[curr_depth];
		nodep = curr_depth == 0 ? my_pst->contents.next_node() :
				curr_pos[curr_depth-1]->next_node();
	}
break_twice:
	curr_pos[curr_depth] = itemp;
	while (!itemp->is_leaf()) {
		assert(itemp->is_node());
		nodep = itemp->next_node();
		for (itemp = &nodep->item[0];; itemp++) {
			assert(itemp < &nodep->item[PSET_NODE_SIZE_ATTLC]);
			if (!itemp->is_null())
				break;
		}
		curr_pos[++curr_depth] = itemp;
	}
	return curr_value = itemp->external_leaf();
}

pst_ATTLC::pst_ATTLC()
: sze(0), iter_head(0)
{
	contents.make_null();
}

pst_ATTLC::pst_ATTLC(ALIGN p)
: sze(0), iter_head(0)
{
	contents.make_null();
	insert(p);
}

pst_ATTLC::pst_ATTLC(ALIGN p1, ALIGN p2)
: sze(0), iter_head(0)
{
	contents.make_null();
	insert(p1);
	insert(p2);
}

pst_ATTLC::pst_ATTLC(ALIGN p1, ALIGN p2, ALIGN p3)
: sze(0), iter_head(0)
{
	contents.make_null();
	insert(p1);
	insert(p2);
	insert(p3);
}

pst_ATTLC::pst_ATTLC(ALIGN p1, ALIGN p2, ALIGN p3, ALIGN p4)
: sze(0), iter_head(0)
{
	contents.make_null();
	insert(p1);
	insert(p2);
	insert(p3);
	insert(p4);
}

pst_ATTLC::~pst_ATTLC()
{
	register pst_iter_old_ATTLC* it;
	for (it = iter_head; it; it = it->next_it) {
		pst_ATTLC**	cheat = (pst_ATTLC**)&it->my_pst;	// break const for giggles
		*cheat = 0;
	}
	remove_all();
}

pst_ATTLC::pst_ATTLC(const pst_ATTLC& oo)
: sze(0), iter_head(0)
{
	contents.make_null();
	pst_iter_old_ATTLC	oo_it(oo);
	register ALIGN p;
	for (p = oo_it.first(); p; p = oo_it.next())
		insert(p);
}

void
pst_ATTLC::warn_iterators() const
{
	register pst_iter_old_ATTLC* it;
	for (it = iter_head; it; it = it->next_it)
		it->curr_depth = -1;
}

int
pst_ATTLC::operator==(const pst_ATTLC& oo) const
{
	if (size() != oo.size())
		return 0;
	pst_iter_old_ATTLC	my_it(*this);
	pst_iter_old_ATTLC	oo_it(oo);
	register ALIGN	my_p = my_it.first();
	if (my_p != oo_it.first())
		return 0;
	while (my_p)
		if ((my_p = my_it.next()) != oo_it.next())
			return 0;
	return 1;
}

int
pst_ATTLC::operator<=(const pst_ATTLC& oo) const
{
	if (size() > oo.size())
		return 0;
	pst_iter_old_ATTLC	my_it(*this);
	pst_iter_old_ATTLC	oo_it(oo);
	register ALIGN	oo_p = oo_it.first();
	register ALIGN my_p;
	for (my_p = my_it.first(); my_p && oo_p;
			my_p = my_it.next(), oo_p = oo_it.next())
		while (my_p != oo_p)
			if (funny_less_than_ATTLC(my_p, oo_p) ||
					(oo_p = oo_it.next()) == 0)
				return 0;
	return my_p == 0;
}

int
pst_ATTLC::operator<(const pst_ATTLC& oo) const
{
	if (size() >= oo.size())
		return 0;
	pst_iter_old_ATTLC	my_it(*this);
	pst_iter_old_ATTLC	oo_it(oo);
	int	strict = 0;
	register ALIGN	oo_p = oo_it.first();
	register ALIGN my_p;
	for (my_p = my_it.first(); my_p && oo_p;
			my_p = my_it.next(), oo_p = oo_it.next())
		while (my_p != oo_p) {
			strict = 1;
			if (funny_less_than_ATTLC(my_p, oo_p) ||
					(oo_p = oo_it.next()) == 0)
				return 0;
		}
	return (strict || oo_p != 0) && (my_p == 0);
}

void
pst_ATTLC::make_union(const pst_ATTLC& a, const pst_ATTLC& b)
{
	assert(sze == 0);
	pst_iter_old_ATTLC	a_it(a);
	pst_iter_old_ATTLC	b_it(b);
	register ALIGN	ap = a_it.first();
	register ALIGN	bp = b_it.first();
	while (ap && bp)
		if (ap == bp) {
			insert(ap);
			ap = a_it.next();
			bp = b_it.next();
		} else if (funny_less_than_ATTLC(ap, bp)) {
			insert(ap);
			ap = a_it.next();
		} else {
			insert(bp);
			bp = b_it.next();
		}
	for (; ap; ap = a_it.next())
		insert(ap);
	for (; bp; bp = b_it.next())
		insert(bp);
}

void
pst_ATTLC::make_intersection(const pst_ATTLC& a, const pst_ATTLC& b)
{
	assert(sze == 0);
	pst_iter_old_ATTLC	a_it(a);
	pst_iter_old_ATTLC	b_it(b);
	register ALIGN	ap = a_it.first();
	register ALIGN	bp = b_it.first();
	while (ap && bp)
		if (ap == bp) {
			insert(ap);
			ap = a_it.next();
			bp = b_it.next();
		} else if (funny_less_than_ATTLC(ap, bp)) {
			ap = a_it.next();
		} else {
			bp = b_it.next();
		}
}

void
pst_ATTLC::make_difference(const pst_ATTLC& a, const pst_ATTLC& b)
{
	assert(sze == 0);
	pst_iter_old_ATTLC	a_it(a);
	pst_iter_old_ATTLC	b_it(b);
	register ALIGN	ap = a_it.first();
	register ALIGN	bp = b_it.first();
	while (ap && bp)
		if (ap == bp) {
			ap = a_it.next();
			bp = b_it.next();
		} else if (funny_less_than_ATTLC(ap, bp)) {
			insert(ap);
			ap = a_it.next();
		} else {
			bp = b_it.next();
		}
	for (; ap; ap = a_it.next())
		insert(ap);
}

void
pst_ATTLC::make_xor(const pst_ATTLC& a, const pst_ATTLC& b)
{
	assert(sze == 0);
	pst_iter_old_ATTLC	a_it(a);
	pst_iter_old_ATTLC	b_it(b);
	register ALIGN	ap = a_it.first();
	register ALIGN	bp = b_it.first();
	while (ap && bp)
		if (ap == bp) {
			ap = a_it.next();
			bp = b_it.next();
		} else if (funny_less_than_ATTLC(ap, bp)) {
			insert(ap);
			ap = a_it.next();
		} else {
			insert(bp);
			bp = b_it.next();
		}
	for (; ap; ap = a_it.next())
		insert(ap);
	for (; bp; bp = b_it.next())
		insert(bp);
}

pst_ATTLC&
pst_ATTLC::operator=(const pst_ATTLC& oo)
{
	if (this != &oo) {
		remove_all();
		pst_iter_old_ATTLC	oo_it(oo);
		register ALIGN oop;
		for (oop = oo_it.first(); oop; oop = oo_it.next())
			insert(oop);
	}
	return *this;
}

pst_ATTLC&
pst_ATTLC::operator|=(const pst_ATTLC& oo)
{
	if (this != &oo) {
		pst_iter_old_ATTLC	oo_it(oo);
		register ALIGN oop;
		for (oop = oo_it.first(); oop; oop = oo_it.next())
			insert(oop);
	}
	return *this;
}

pst_ATTLC&
pst_ATTLC::operator-=(const pst_ATTLC& oo)
{
	if (this != &oo) {
		pst_iter_old_ATTLC	oo_it(oo);
		register ALIGN oop;
		for (oop = oo_it.first(); oop; oop = oo_it.next())
			remove(oop);
	} else
		remove_all();
	return *this;
}

pst_ATTLC&
pst_ATTLC::operator&=(const pst_ATTLC& oo)
{
	if (this != &oo) {
		pst_iter_old_ATTLC	it(*this);
		register ALIGN oop;
		for (oop = it.first(); oop; oop = it.next())
			if (!oo.contains(oop))
				remove(oop);
	}
	return *this;
}

pst_ATTLC&
pst_ATTLC::operator^=(const pst_ATTLC& oo)
{
	if (this == &oo) {
		remove_all();
	} else {
		pst_iter_old_ATTLC	a_it(*this);
		pst_iter_old_ATTLC	b_it(oo);
		register ALIGN	ap = a_it.first();
		register ALIGN	bp = b_it.first();
		while (ap && bp) {
			if (ap == bp) {
				remove(ap);
				ap = a_it.next();
				bp = b_it.next();
			} else if (funny_less_than_ATTLC(ap, bp)) {
				ap = a_it.next();
			} else {
				insert(bp);
				bp = b_it.next();
			}
		}
		for (; bp; bp = b_it.next())
			insert(bp);
	}
	return *this;
}
