/*ident	"@(#)Set:incl/set.c	3.1" */
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

#ifndef _set_C_
#define _set_C_

//  T is the type parameter;
//  H is the name of the hash function
//  (Set_zero should be used if the client does
//  not have an explicit function).

template <class T>
Bucketiter_ATTLC<T>::~Bucketiter_ATTLC()
{
    if ( my_set == 0 )
	return;

    if ( this == my_set->iter_head ) {
	//  Case 1: iterator at head of chain

	my_set->iter_head = next_it;
    }
    else {
	//  Case 2: iterator in middle of chain

	register Bucketiter_ATTLC<T>* x;

	register Bucketiter_ATTLC<T>* an_it;
	for ( an_it=my_set->iter_head ;
	      (x = an_it->next_it) != this ; an_it = x);

	an_it->next_it = next_it;
    }
}

template <class T>
Bucketiter_ATTLC<T>::Bucketiter_ATTLC(const Set<T>& s):
    my_set((Set<T>*)&s),
    next_it(s.iter_head)
{ 
    ((Set<T>*)(&s))->iter_head = this;
}

template <class T>
Bucketiter_ATTLC<T>::Bucketiter_ATTLC(const Bucketiter_ATTLC<T>& bi):
    my_set(bi.my_set),
    next_it(my_set->iter_head)
{ 
    my_set->iter_head = this;
}

template <class T>
Bucketiter_ATTLC<T>&
Bucketiter_ATTLC<T>::operator=(const Bucketiter_ATTLC<T>& bi)
{
	if (this == my_set->iter_head) {
		my_set->iter_head = next_it;
	} else {
		Bucketiter_ATTLC<T>* p = my_set->iter_head;
		while (p->next_it != this) {
		    p = p->next_it;
		}
		p->next_it = next_it;
	}
	my_set = bi.my_set;
	next_it = my_set->iter_head;
	my_set->iter_head = this;
	return *this;
}

//  first() and next() are similar to Shopiro's original
//  pset iterator functions  first() and next(); they
//  walk the leaves of the tree, returning Bucket(T))*.

template <class T>
const Bucket_ATTLC<T>*
Bucketiter_ATTLC<T>::first() 
{
    register Internal_item_ATTLC* itemp;
    register Internal_node_ATTLC* nodep;
    Bucket_ATTLC<T>* bp;

    if ( my_set == 0 )
	return 0;

    pos.curr_depth = -1;

    if ( my_set->contents.is_null() )
	return 0;

    if ( my_set->contents.is_leaf() ) {
	//  'contents' is a Bucket_ATTLC<T>*

	bp = (Bucket_ATTLC<T>*)my_set->contents.external_leaf();
    }
    else {
	//  'contents' is an Internal_node*.  Search the
	//  subtree rooted at this node looking for the
	//  leftmost leaf.

	nodep = my_set->contents.next_node();

	for ( ; ; ) {
#ifdef DEBUG_ATTLC
	    assert(pos.curr_depth < SET_POSITIONS_ATTLC);
#endif
	    //  Scan this Internal_node from left to right looking
	    //  for a non-null Internal_item

	    for ( itemp = &nodep->item[0] ; ; itemp++ ) {
#ifdef DEBUG_ATTLC
		assert(itemp < &nodep->item[SET_NODE_SIZE_ATTLC]);
#endif

		if ( !itemp->is_null() )
		    break;
	    }
	    pos.curr_pos[++pos.curr_depth] = itemp;

//  If this Internal_item is a leaf, the search is over;
//  otherwise, iteratively 'recurse'

	    if ( itemp->is_leaf() )
		break;

	    nodep = itemp->next_node();
	}
	bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
    }

//  bp now points to a Bucket_ATTLC<T>

    pos.curr_value = bp->hashval;
    return bp;
}

template <class T>
const Bucket_ATTLC<T>*
Bucketiter_ATTLC<T>::next()
{
    register Internal_item_ATTLC* itemp;
    register Internal_node_ATTLC* nodep;
    register Bucket_ATTLC<T>* bp;

    if ( my_set == 0 )
	return 0;

    //  Search the index for pos.curr_value.  Since
    //  pos caches the result of the last iterator
    //  access, the loop should exit almost immediately,
    //  unless set mutation has occurred since the last
    //  access.

    if (pos.curr_depth == -1 )
	itemp = &my_set->contents;
    else
	itemp = pos.curr_pos[pos.curr_depth];

    int unshift = SET_INITIAL_SHIFT_ATTLC + SET_SHIFT_INCR_ATTLC +
    		  pos.curr_depth * SET_SHIFT_INCR_ATTLC;
    register long mask = SET_MASK_BITS_ATTLC << unshift;
    nodep = 0;

    for ( ; ; mask <<= SET_SHIFT_INCR_ATTLC, unshift += SET_SHIFT_INCR_ATTLC ) {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < SET_POSITIONS_ATTLC);
#endif
	if ( itemp->is_null() )
	    break;
	else if ( itemp->is_leaf() ) {
	    //  While searching for current_value, we have found
	    //  a leaf.  This may or may not be the leaf containing
	    //  the current value, depending on whether the Bucket
	    //  containing the current value has been deleted or not.

	    bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	    Set_or_Bag_hashval hval = bp->hashval;

	    if ( hval == pos.curr_value || SET_LT_ATTLC(hval,pos.curr_value) )
		//  Case 1: we either (a) found the current value or
		//  (b) passed it (it must have been deleted).
		break;
	    else {
		//  The leaf points to a Bucket containing a hash value
		//  GREATER THAN the current value, so the current value
		//  must have been deleted.  The Bucket pointed to is,
		//  by definition, the "next" Bucket, the one we are
		//  looking for.

		pos.curr_value = hval;
		return bp;
	    }
	}
	else {
	    //  Node: search subindex

	    nodep = itemp->next_node();
#ifdef DEBUG_ATTLC
	    assert(nodep);
#endif
	    pos.curr_pos[++pos.curr_depth] = itemp =
	        &nodep->item[(((unsigned long)pos.curr_value) & mask) >> unshift];
	}
    }

    //  pos.curr_pos is now up-to-date and we are ready
    //  to locate the NEXT Bucket_ATTLC<T> starting from here.

    if ( pos.curr_depth == -1 )
	//  There was only one hash Bucket; the iteration
	//  is therefore terminated.
	return 0;

    //  Find the next Bucket_ATTLC<T>

    if ( nodep == 0 )
	if ( pos.curr_depth == 0 )
	    nodep =  my_set->contents.next_node();
	else
	    nodep =  pos.curr_pos[pos.curr_depth-1]->next_node();

    for ( ; ; ) {
#ifdef DEBUG_ATTLC
	assert(&nodep->item[0] <= itemp  &&  itemp < &nodep->item[SET_NODE_SIZE_ATTLC]);
#endif
	//  Scan rightward within this node, looking for a
	//  non-null item

	while ( itemp < &nodep->item[SET_NODE_SIZE_ATTLC-1] )
	    if ( !(++itemp)->is_null() )
		goto found;

	//  Scan reached end of node without finding anything;
	//  pop up one level in order to continue the walk

	if ( pos.curr_depth-- == 0 )
	    //  The set is exhausted, so the iteration is terminated
	    return 0;

	itemp = pos.curr_pos[pos.curr_depth];
	if ( pos.curr_depth == 0 )
	    nodep = my_set->contents.next_node();
	else
	    nodep = pos.curr_pos[pos.curr_depth-1]->next_node();
    }

found:

    //  itemp now points either to the leaf we're looking
    //  for or the root of a subtree containing the leaf.

    pos.curr_pos[pos.curr_depth] = itemp;

    while ( !itemp->is_leaf() ) {
#ifdef DEBUG_ATTLC
	assert(itemp->is_node());
#endif
	nodep = itemp->next_node();

	//  Scan to right within this node

	for ( itemp = &nodep->item[0] ; ; itemp++ ) {
#ifdef DEBUG_ATTLC
	    assert(itemp < &nodep->item[SET_NODE_SIZE_ATTLC]);
#endif

	    if ( !itemp->is_null() )
		break;
	}
	pos.curr_pos[++pos.curr_depth] = itemp;
    }
    bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
    pos.curr_value = bp->hashval;
    return bp;
}

template <class T>
const T*
Setiter<T>::first()
{
    //  This version has been simplified by using
    //  Bucketiter::first()

    const Bucket_ATTLC<T>* bp = ((Bucketiter_ATTLC<T>*)this)->first();

    if (  bp == 0 )
	return 0;

    delete itp;
    itp=new Listiter<T>(((Bucket_ATTLC<T>*)bp)->collision_list);
    T* result;
    itp->next(result);

    //  Cast the pointer to const so the client can't
    //  modify the element

    return (const T*)result;
}

template <class T>
const T*
Setiter<T>::next()
{
    register Internal_item_ATTLC* itemp;
    register Internal_node_ATTLC* nodep;
    register Bucket_ATTLC<T>* bp;

    //  TBD_see if this code can be simplified by using
    //  Bucketiter_ATTLC<T>::next()

    if ( my_set == 0 )
	return 0;

    //  New List-style iterator has next() as only public
    //  member function; first() is private and called
    //  internally only if this is the first call to next(),
    //  i.e., only if inited=0.

    if ( inited==0 ) {
	const T* tp = first();
	if (tp) inited=1;
	return tp;
    }

    //  Search the index for the pos.curr_value.  Since
    //  pos normally caches the results of the
    //  most recent iterator access, the loop should
    //  exit almost immediately.

    if ( pos.curr_depth == -1 )
	itemp = &my_set->contents;
    else
	itemp = pos.curr_pos[pos.curr_depth];

    int unshift = SET_INITIAL_SHIFT_ATTLC + SET_SHIFT_INCR_ATTLC +
		  pos.curr_depth * SET_SHIFT_INCR_ATTLC;
    register long mask = SET_MASK_BITS_ATTLC << unshift;
    nodep = 0;

    for ( ; ; mask <<= SET_SHIFT_INCR_ATTLC, unshift += SET_SHIFT_INCR_ATTLC ) {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < SET_POSITIONS_ATTLC);
#endif

	if ( itemp->is_null() )
	    break;
	else if ( itemp->is_leaf()) {
	    //  While searching for current_value, we have found
	    //  a leaf.  This may or may not be the leaf containing
	    //  the current value, depending on whether the Bucket
	    //  containing the current value has been deleted or not.

	    bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	    Set_or_Bag_hashval hval = bp->hashval;

	    if ( hval == pos.curr_value || SET_LT_ATTLC(hval,pos.curr_value) ) {
		//  Case 1: we either (a) found the current value or
		//  (b) passed it (it must have been deleted)
		break;
	    }
	    else {
		//  The leaf we have found points to a Bucket containing
		//  a next GREATER THAN the current value (the current
		//  value must have been deleted).

		pos.curr_value = hval;
		delete itp;
		itp = new Listiter<T>(((Bucket_ATTLC<T>*)bp)->collision_list);
		T* result;
		itp->next(result);
		return (const T*)result;
	    }

	}
	else {
	    //  Node: search subindex

	    nodep = itemp->next_node();
#ifdef DEBUG_ATTLC
	    assert(nodep);
#endif
	    pos.curr_pos[++pos.curr_depth] = itemp =
		&nodep->item[(((unsigned long)pos.curr_value) & mask) >> unshift];
	}
    }

    //  curr_pos is now up-to-date and we are ready
    //  to locate the next value starting from here.

    if ( pos.curr_depth == -1) {
	//  There is only one hash Bucket; if we have not
	//  not yet returned all values on the collision list,
	//  return the next one; otherwise, the iteration
	//  is terminated
	//
	//  TBD_note:  It is possible that Set mutation occurred
	//  that (1) deleted all the elements in a List and then
	//  (2) re-created a List with the same hash value.  In
	//  this case, *itp refers to a non-existeng List.  We
	//  guard against this case by using Listiter::the_list().

	if ( !itp->the_list() || itp->at_end() )
	    return 0;
	else {
	    T* result;
	    itp->next(result);
	    return (const T*)result;
	}
    }
    if ( itemp->is_leaf() &&
	((Bucket_ATTLC<T>*)itemp->external_leaf())->hashval == pos.curr_value &&
	!itp->at_end() )
    {
	//  There are still values on the collision list in
	//  the current Bucket

	T* result;
	itp->next(result);
	return (const T*)result;

    }
    else {
	//  Find the next leaf.
	//  TBD_can we use Bucketiter_ATTLC<T>::next() here?

	if ( nodep == 0 )
	    if ( pos.curr_depth == 0 )
		nodep = my_set->contents.next_node();
	    else
		nodep = pos.curr_pos[pos.curr_depth-1]->next_node();

	for (  ; ; ) {
#ifdef DEBUG_ATTLC
	    assert(&nodep->item[0] <= itemp  &&  itemp < &nodep->item[SET_NODE_SIZE_ATTLC]);
#endif
	    //  Scan rightward within this node, looking for a
	    //  non-null item

	    while ( itemp < &nodep->item[SET_NODE_SIZE_ATTLC-1] ) {
		if ( !(++itemp)->is_null() )
		    goto found;
	    }

	    //  Scan reached end of node without finding anything;
	    //  pop up one level in order to continue the walk

	    if ( pos.curr_depth-- == 0 )
		//  The set is exhausted, so the iteration is terminated
		return 0;

	    itemp = pos.curr_pos[pos.curr_depth];
	    if ( pos.curr_depth == 0 )
		nodep = my_set->contents.next_node();
	    else
		nodep = pos.curr_pos[pos.curr_depth-1]->next_node();
	}
found:
	//  itemp now points either to the leaf we're looking
	//  for or the root of a subindex containing the leaf.

	pos.curr_pos[pos.curr_depth] = itemp;

	while ( !itemp->is_leaf() ) {
#ifdef DEBUG_ATTLC
	    assert(itemp->is_node());
#endif
	    nodep = itemp->next_node();

	    //  Scan to right within this node

	    for ( itemp = &nodep->item[0];;itemp++ ) {
#ifdef DEBUG_ATTLC
		assert(itemp < &nodep->item[SET_NODE_SIZE_ATTLC]);
#endif
		if ( !itemp->is_null() )
		    break;
	    }
	    pos.curr_pos[++pos.curr_depth] = itemp;
	}
    }
    bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
    pos.curr_value = bp->hashval;
    delete itp;
    itp = new Listiter<T>(((Bucket_ATTLC<T>*)bp)->collision_list);
    T* result;
    itp->next(result);
    return (const T*)result;
}

template <class T>
Set<T>::Set(const T& t0): sze(0), iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    insert(t0);
}

template <class T>
Set<T>::Set(const T& t0, const T& t1): sze(0), iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    insert(t0);
    insert(t1);
}

template <class T>
Set<T>::Set(const T& t0, const T& t1, const T& t2):
    sze(0),
    iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    insert(t0);
    insert(t1);
    insert(t2);
}

template <class T>
Set<T>::Set(const T& t0, const T& t1, const T& t2, const T& t3):
    sze(0),
    iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    insert(t0);
    insert(t1);
    insert(t2);
    insert(t3);
}

template <class T>
Set<T>::Set(const Set<T>& a, const Set<T>& b, Set_union_ATTLC*):
    sze(0),
    iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,1);
}

template <class T>
Set<T>::Set(const Set<T>& a, const Set<T>& b, Set_inter_ATTLC*):
    sze(0),
    iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,0);
}

template <class T>
Set<T>::Set(const Set<T>& a, const Set<T>& b, Set_diff_ATTLC*):
    sze(0),
    iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,2);
}

template <class T>
Set<T>::Set(const Set<T>& a, const Set<T>& b, Set_xor_ATTLC*):
    sze(0),
    iter_head(0)

{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,3);
}

template <class T>
Set<T>&
Set<T>::operator=(const Set<T>& oo)
{
    if ( this != &oo ) {
	remove_all();
	Bucketiter_ATTLC<T> bi(oo);
	const Bucket_ATTLC<T>* bp = bi.first();

	while ( bp ) {
	    Listiter<T> li(((Bucket_ATTLC<T>*)bp)->collision_list);

	    while ( !li.at_end() ) {
		T* tp;
		li.next(tp);
		insert(*tp);
	    }
	    bp = bi.next();
	}
    }
    return *this;
}

template <class T>
int
Set<T>::containment(const Set<T>& b, int comp_type) const
{
    switch (comp_type) {
    case 0: // set equality
	if (size() != b.size() )
	    return 0;
	break;
    case 1: // subset
	if (size() > b.size() )
	    return 0;
	break;
    case 2: // strict subset
	if (size() >= b.size() )
	    return 0;
    }

    Bucketiter_ATTLC<T>     a_it(*this);
    Bucketiter_ATTLC<T>     b_it(b);
    register const Bucket_ATTLC<T>* ap = a_it.first();
    register const Bucket_ATTLC<T>* bp = b_it.first();

    while ( ap && bp ) {
	if ( ap->hashval == bp->hashval ) {
	    //  Make sure ap->collision_list is a subset of
	    //  bp->collision_list

	    if ( ap->collision_list.length() > bp->collision_list.length() )
		return 0;

	    Listiter<T> lia(((Bucket_ATTLC<T>*)ap)->collision_list);
	    Listiter<T> lib(((Bucket_ATTLC<T>*)bp)->collision_list);

	    while ( !lia.at_end() ) {
		T* atp;
		lia.next(atp);
		lib.reset();
		int found=0;

		while ( !lib.at_end() ) {
		    T* btp;
		    lib.next(btp);

		    if ( *atp == *btp ) {
			found=1;
			break;
		    }
		}
		if ( !found )
		    return 0;
	    }
	    ap = a_it.next();
	    bp = b_it.next();

	}
	else if ( comp_type == 0 || SET_LT_ATTLC(ap->hashval,bp->hashval) ) {
	    //  *ap contains values that can't be in oo, since we've
	    //  already passed the point where they would be found;
	    //  return failure

	    return 0;

	}
	else {
	    //  *ap contains values that may be in a future 
	    //  Bucket_ATTLC of oo; increment b_it

	    bp = b_it.next();
	}
    }

    //  The relation is true only if a_it is exhausted (for subset
    //  comparisons) or only if both iterators are exhausted (for
    //  equality comparison)

    if (comp_type == 0)
	return ap == bp;

    return ap == 0;
}

template <class T>
const T*
Set<T>::insert(const T& value,int count)
{
    if ( count<=0 )
	return 0;

    register Internal_item_ATTLC* itemp;
    register Internal_node_ATTLC* nodep;
    Bucket_ATTLC<T>* bp;
    T* result;

    //  Hash the value

    Set_or_Bag_hashval hval = hash(value);

    //  See how much of hval matches pos.curr_value
    //  (equivalently, see how much of pos is valid.)
    //  This will determine where we start looking for the
    //  insertion point.  This is an optimization that takes
    //  advantage of the phenomenon that many operations
    //  create a Set by walking an existing index and
    //  inserting its elements into the new Set.

    long mask = SET_MASK_BITS_ATTLC << SET_INITIAL_SHIFT_ATTLC;

    int depth;
    for ( depth = -1 ; depth < pos.curr_depth ;
	  depth++, mask <<= SET_SHIFT_INCR_ATTLC )
	if ( (pos.curr_value & mask) != (hval & mask) )
	    break;

    //  If depth = -1, no bits match;
    //  if depth = 0, bits 0..3 match, etc.

    register int unshift = SET_INITIAL_SHIFT_ATTLC + SET_SHIFT_INCR_ATTLC +
    			   depth * SET_SHIFT_INCR_ATTLC;
    if ( depth == -1 )
	itemp = &contents;
    else
	itemp = pos.curr_pos[depth];

    //  Clobber all the active iterators

    warn_iterators();

    //  Find the insertion point

    pos.curr_value = hval;
    nodep = 0;

    for ( pos.curr_depth = depth ; ;
	mask <<= SET_SHIFT_INCR_ATTLC, unshift += SET_SHIFT_INCR_ATTLC ) {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < SET_POSITIONS_ATTLC);
#endif

	if ( itemp->is_null() )
	    break;
	else if ( itemp->is_leaf() ) {
	    bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	    if ( bp->hashval == hval ) {
		//  The 'value' goes in this Bucket

		Listiter<T> it(((Bucket_ATTLC<T>*)bp)->collision_list);
		T* tp;

		while ( it.next(tp) ) {
		    if ( *tp == value )
			//  The value is already in the Bucket; return failure
			return 0;
		}

		//  The value is not already in the Bucket; insert it

		sze++;
		it.insert_next(value);
		it.peek_next(result);
		return result;

	    }
	    else {
		//  This Bucket is not the one where the value belongs;
		//  a subindex will have to be created containing both
		//  this Bucket and a new one created to hold 'value'

		break;
	    }
	}
	else {
	    //  Node: search subindex

	    nodep = itemp->next_node();
#ifdef DEBUG_ATTLC
	    assert(nodep);
#endif
	    pos.curr_pos[++pos.curr_depth] = itemp =
		&nodep->item[(hval & mask) >> unshift];
	}
    }

    //  ASSERT: itemp is either null or is a leaf that must
    //  be moved down into a subindex containing BOTH the
    //  existing leaf and a new leaf pointing to a new Bucket
    //  containing 'value.'  In either case, the cardinality
    //  of the Set is increased by one.

    sze++;

    if ( itemp->is_null() ) {
	//  Make this null item into a leaf pointing to a new
	//  Bucket containing 'value'

	itemp->make_leaf( new Bucket_ATTLC<T> );
	bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	bp->hashval = hval;
	// bp->collision_list.insert_next(value);
	// bp->collision_list.peek_next(result);

	Listiter<T> cit(bp->collision_list);
	cit.insert_next(value);
	cit.peek_next(result);

	// bp->collision_list.put(value); result = value;

	if ( pos.curr_depth > 0 )
	    pos.curr_pos[pos.curr_depth-1]->next_node()->busy_count++;
	else if ( pos.curr_depth == 0 )
	    contents.next_node()->busy_count++;

	return result;
    }
    else {
#ifdef DEBUG_ATTLC
	assert(itemp->is_leaf());
#endif
	//  Replace this leaf by a subindex containing both the
	//  original leaf and a new leaf pointing to a new
	//  Bucket containing 'value'

	bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	Set_or_Bag_hashval temp = bp->hashval;
	int ind1,ind2;

	for ( ; ; mask <<= SET_SHIFT_INCR_ATTLC, unshift += SET_SHIFT_INCR_ATTLC )
	{
	    itemp->make_node(nodep = new Internal_node_ATTLC);
#ifdef DEBUG_ATTLC
	    assert(pos.curr_depth < SET_POSITIONS_ATTLC);
#endif
	    ind1 = int(((unsigned long)temp & mask) >> unshift);
	    ind2 = int((hval & mask) >> unshift);

	    if ( ind1 != ind2 )
		//  Serendipitous case
		break;

	    nodep->busy_count = 1;
	    itemp = &nodep->item[ind1];
	    pos.curr_pos[++pos.curr_depth] = itemp;
	}

	//  Move the existing leaf down into the subindex

	nodep->item[ind1].make_leaf(bp);

	//  Make a new leaf and Bucket for 'value'

	itemp = &nodep->item[ind2];
	itemp->make_leaf( new Bucket_ATTLC<T> );
	bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	bp->hashval = hval;
	// bp->collision_list.insert_next(value);
	// bp->collision_list.peek_next(result);

	Listiter<T> bit(bp->collision_list);
	bit.insert_next(value);
	bit.peek_next(result);

	// bp->collision_list.put(value); result = value;

	pos.curr_pos[++pos.curr_depth] = itemp;
	nodep->busy_count = 2;
	return result;
    }
}

template <class T>
unsigned
Set<T>::remove(const T& value,int count)
{
    if ( count<=0 )
	return 0;

    register Internal_item_ATTLC* itemp;
    register Internal_node_ATTLC* nodep;
    Bucket_ATTLC<T>* bp=0;

    //  Hash the value

    Set_or_Bag_hashval hval = hash(value);

    //  See how much of hval matches pos.curr_value
    //  (equivalently, see how much of pos is valid.)
    //  This will determine where we start looking for the
    //  deletion point.  This is an optimization that takes
    //  advantage of the phenomenon that many operations
    //  create a Set by walking an existing index and inserting
    //  its elements into the new Set.

    register long mask = SET_MASK_BITS_ATTLC << SET_INITIAL_SHIFT_ATTLC;

    int depth;
    for ( depth = -1 ; depth < pos.curr_depth ;
	  depth++, mask <<= SET_SHIFT_INCR_ATTLC )
	if ( (pos.curr_value & mask) != (hval & mask) )
	    break;

    //  If depth = -1, no bits match;
    //  If depth = 0, bits 0...3 match, etc.

    register int unshift = SET_INITIAL_SHIFT_ATTLC + SET_SHIFT_INCR_ATTLC +
    			   depth * SET_SHIFT_INCR_ATTLC;
    if ( depth == -1 )
	itemp = &contents;
    else
	itemp = pos.curr_pos[depth];

    nodep = 0;
    pos.curr_value = hval;
    warn_iterators();

    //  Find the leaf pointing to the Bucket containing
    //  the value to be deleted.

    for ( pos.curr_depth = depth ; ;
	  mask <<= SET_SHIFT_INCR_ATTLC, unshift += SET_SHIFT_INCR_ATTLC ) {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < SET_POSITIONS_ATTLC);
#endif

	if ( itemp->is_null() ) {
	    //  The value must have been deleted already.
	    //  Return failure.

	    return 0;
	}
	if ( itemp->is_leaf() ) {
	    bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	    if ( bp->hashval == hval ) {
		//  The value may be in the Bucket pointed to by
		//  this leaf.

		break;
	    }
	    else {
		//  The leaf must have been deleted and subsequently
		//  replaced by a different Bucket (one with a different
		//  hash value).  Return failure

		return 0;
	    }
	}
	else {
	    //  Node: search subindex

	    nodep = itemp->next_node();
#ifdef DEBUG_ATTLC
	    assert(nodep);
#endif
	    pos.curr_pos[++pos.curr_depth] = itemp =
	        &nodep->item[(pos.curr_value & mask)>>unshift];
	}
    }
#ifdef DEBUG_ATTLC
    assert(bp && bp->hashval==hval);
#endif
    
    //  We found the Bucket; locate the value in the
    //  collision list and remove it

    Listiter<T> it(((Bucket_ATTLC<T>*)bp)->collision_list);
    T* tp;
    int found=0;

    while ( it.next(tp) ) {
	if ( *tp == value ) {
	    found=1;
	    it.remove_prev();
	    break;
	}
    }
    if ( !found )
	//  The value was not found; return failure
	return 0;

    //  The value was found and deleted.  We must now
    //  worry about the Bucket becoming empty.

    sze--;
    if ( bp->collision_list.length() > 0 )
	//  The collision list still has one or more elements.
	return 1;

    //  The collision list has become empty as a result of
    //  this deletion; we must delete the Bucket and fix up
    //  the index accordingly.

    if ( pos.curr_depth == -1 ) {
	//  The Set had only one element in it

#ifdef DEBUG_ATTLC
	assert(sze == 0);
#endif
	delete bp;
	itemp->make_null();
	return 1;
    }

    //  Get the parent node (if not already known)

    if ( nodep == 0 )
	if ( pos.curr_depth == 0 )
	    nodep = contents.next_node();
	else
	    nodep = pos.curr_pos[pos.curr_depth-1]->next_node();

    if ( --(nodep->busy_count) > 1 ) {
	//  Easy case: node still has more than one busy item

	delete bp;
	itemp->make_null();
	return 1;
    }

    //  Hard case: node has only one busy item; since nodes
    //  ideally contain two or more busy items, we should
    //  try to absorb the item

#ifdef DEBUG_ATTLC
    assert(nodep->busy_count == 1);
#endif

    //  Find the busy item, itp

    register Internal_item_ATTLC* itp;
    for ( itp = &nodep->item[0] ;
	  itp < &nodep->item[SET_NODE_SIZE_ATTLC] ; itp++ )
	if ( itp != itemp && !itp->is_null() )
	    break;

    if ( !itp->is_leaf() ) {
	//  Complicated case: punt (we won't try to absorb
	//  a node)

	delete bp;
	itemp->make_null();
	return 1;
    }

    //  Relatively easy case: absorb a leaf

    Bucket_ATTLC<T>* temp = (Bucket_ATTLC<T>*)itp->external_leaf();
    delete bp;

    for ( ; ; ) {
	delete nodep;

	if ( pos.curr_depth-- == 0 ) {
	    contents.make_leaf(temp);
	    break;
	}
	if ( pos.curr_depth == 0 )
	    nodep = contents.next_node();
	else
	    nodep = pos.curr_pos[pos.curr_depth-1]->next_node();

	if ( nodep->busy_count > 1 ) {
	    pos.curr_pos[pos.curr_depth]->make_leaf(temp);
	    break;
	}
#ifdef DEBUG_ATTLC
	assert(nodep->busy_count == 1);
#endif
    }
    return 1;
}

template <class T>
unsigned 
Set<T>::remove_all()
{
    unsigned result = (unsigned)sze;
    warn_iterators();
    register Internal_item_ATTLC* itemp = &contents;

    if ( itemp->is_null() ) {
#ifdef DEBUG_ATTLC
	assert(sze == 0);
#endif
	return result;
    }
    if ( itemp->is_leaf() ) {
#ifdef DEBUG_ATTLC
	assert(sze >= 1);
#endif
	Bucket_ATTLC<T>* bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	delete bp;
	itemp->make_null();
	sze = 0;
	return result;
    }
    register Internal_node_ATTLC* nodep = itemp->next_node();
    itemp = &nodep->item[0];
    pos.curr_depth = -1;

    for ( ; ; ) {
	register Internal_item_ATTLC* stopper = &nodep->item[SET_NODE_SIZE_ATTLC];
	for ( ; itemp < stopper ; /* itemp++ */ ) {
	    if ( itemp->is_node() ) {
		pos.curr_pos[++pos.curr_depth] = itemp;
		nodep = itemp->next_node();
		itemp = &nodep->item[0];
		stopper = &nodep->item[SET_NODE_SIZE_ATTLC];
	    }
	    else itemp++;
	}

	//  Unlike Set_of_p, we must destroy the buckets
	//  pointed to by this node

	for ( itemp = &nodep->item[0] ; itemp < &nodep->item[BAG_NODE_SIZE_ATTLC] ; itemp++ )
	{
	    if ( itemp->is_leaf() ) {
		Bucket_ATTLC<T>* bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
		delete bp;
		itemp->make_null();
	    }
	}
	delete nodep;

	if ( pos.curr_depth < 0 )
	    break;

	itemp = pos.curr_pos[pos.curr_depth--] + 1;
	if ( pos.curr_depth < 0 )
	    nodep = contents.next_node();
	else
	    nodep = pos.curr_pos[pos.curr_depth]->next_node();
    }
    pos.curr_depth = -1;
    contents.make_null();
    sze = 0;
    return result;
}

template <class T>
const T*
Set<T>::contains(const T& value) const
{
    register Internal_item_ATTLC* itemp;
    register Internal_node_ATTLC* nodep;
    Bucket_ATTLC<T>* bp;

    //  Hash the value

    Set_or_Bag_hashval hval = hash(value);

    //  See how much of pos is still good

    register long mask = SET_MASK_BITS_ATTLC << SET_INITIAL_SHIFT_ATTLC;

    int depth;
    for ( depth = -1 ; depth < pos.curr_depth ; depth++, mask <<= SET_SHIFT_INCR_ATTLC )
	if ( (pos.curr_value & mask) != (hval & mask) )
	    break;

    register int unshift = SET_INITIAL_SHIFT_ATTLC + SET_SHIFT_INCR_ATTLC +
    			   depth * SET_SHIFT_INCR_ATTLC;
    if ( depth == -1 )
	itemp = &(((Set<T> *)this)->contents);
    else
	itemp = pos.curr_pos[depth];

    nodep = 0;
    ((Position_ATTLC *) &((Set<T>*)this)->pos)->curr_value = hval;

    for ( ((Position_ATTLC *) &((Set<T>*)this)->pos)->curr_depth = depth ; ;
	  mask <<= SET_SHIFT_INCR_ATTLC, unshift += SET_SHIFT_INCR_ATTLC )
    {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < SET_POSITIONS_ATTLC);
#endif
	if ( itemp->is_null() )
	    return 0;

	if ( itemp->is_leaf() ) {
	    bp = (Bucket_ATTLC<T>*)itemp->external_leaf();
	    Listiter<T> it(((Bucket_ATTLC<T>*)bp)->collision_list);
	    T* tp;

	    while ( it.next(tp) )
		if ( value == *tp )
		    return tp;

	    return 0;
	}
	else {
	    nodep = itemp->next_node();
#ifdef DEBUG_ATTLC
	    assert(nodep);
#endif
	    ((Position_ATTLC *) &((Set<T>*)this)->pos)->curr_pos[++((Position_ATTLC *) &((Set<T>*)this)->pos)->curr_depth] = itemp =
		&nodep->item[(pos.curr_value & mask) >> unshift];
	}
    }
}

template <class T>
Set<T>&
Set<T>::operator|=(const Set<T>& oo)
{
    if ( this != &oo ) {
	Bucketiter_ATTLC<T> bi(oo);
	const Bucket_ATTLC<T>* bp = bi.first();

	while ( bp ) {
	    Listiter<T> li(((Bucket_ATTLC<T>*)bp)->collision_list);
	    while ( !li.at_end() ) {
		T* tp;
		li.next(tp);
		(void)insert(*tp);
	    }
	    bp = bi.next();
	}
    }
    return *this;
}

template <class T>
Set<T>&
Set<T>::operator-=(const Set<T>& oo)
{
    if ( this != &oo ) {
	Bucketiter_ATTLC<T> bi(oo);
	const Bucket_ATTLC<T>* bp = bi.first();

	while ( bp ) {
	    Listiter<T> li(((Bucket_ATTLC<T>*)bp)->collision_list);
	    while ( !li.at_end() ) {
		T* tp;
		li.next(tp);
		(void)remove(*tp);
	    }
	    bp = bi.next();
	}
    }
    else
	remove_all();

    return *this;
}

template <class T>
Set<T>&
Set<T>::operator&=(const Set<T>& oo)
{
    if ( this != &oo ) {
	Bucketiter_ATTLC<T> bi(*this);
	const Bucket_ATTLC<T>* bp = bi.first();

	while ( bp ) {
	    Listiter<T> li(((Bucket_ATTLC<T>*)bp)->collision_list);

	    //  remove() may delete the collision list;
	    //  beware of dangling list iterator;

	    while ( li.the_list() && !li.at_end() ) {
		T* tp;
		li.next(tp);
		if ( !oo.contains(*tp))
		    remove(*tp);
	    }
	    bp = bi.next();
	}
    }
    return *this;
}

template <class T>
Set<T>&
Set<T>::operator^=(const Set<T>& b)
{
    if ( this != &b ) {
	Bucketiter_ATTLC<T> a_it(*this); 
	Bucketiter_ATTLC<T> b_it(b);
	const Bucket_ATTLC<T>* ap = a_it.first(); 
	const Bucket_ATTLC<T>* bp = b_it.first();

	while ( ap && bp ) {
	    if ( ap->hashval == bp->hashval ) {
		//  The two hash values are equal; this means the two
		//  buckets may contain equal values (which must be 
		//  weeded out).

		Listiter<T> lia(((Bucket_ATTLC<T>*)ap)->collision_list);
		Listiter<T> lib(((Bucket_ATTLC<T>*)bp)->collision_list);

		lib.reset();

		while ( !lib.at_end() ) {
		    T* atp;
		    T* btp;
		    lib.next(btp);
		    int found=0;

		    //  Guard against the case where removing from this list
		    //  causes the list to disappear

		    if ( lia.the_list() ) {
			lia.reset();
			while ( !lia.at_end() ) {
			    lia.next(atp);
			    if ( *atp == *btp ) {
				found=1;
				break;
			    }
			}
		    }
		    if ( found )
			remove(*atp);
		    else
			insert(*btp);
		}
		ap = a_it.next();
		bp = b_it.next();
	    }
	    else if ( SET_LT_ATTLC(ap->hashval,bp->hashval) ) {
		ap = a_it.next();
	    }
	    else {
		Listiter<T> lib(((Bucket_ATTLC<T>*)bp)->collision_list);

		while ( !lib.at_end() ) {
		    T* btp;
		    lib.next(btp);
		    insert(*btp);
		}
		bp = b_it.next();
	    }
	}
	while ( bp ) {
	    Listiter<T> lib(((Bucket_ATTLC<T>*)bp)->collision_list);
	    while ( !lib.at_end() ) {
		T* btp;
		lib.next(btp);
		insert(*btp);
	    }
	    bp = b_it.next();
	}
    }
    else {
	remove_all();
    }
    return *this;
}

template <class T>
void
Set<T>::warn_iterators() const
{
    register Bucketiter_ATTLC<T>* it;
    for ( it = iter_head ; it ; it = it->next_it )
	it->clobber();
}

#ifdef __GNUG__
inline void dummyxxx() {
	Map<Set_or_Bag_hashval,unsigned> m1;
}
#endif
template <class T>
void Set<T>::histogram(Map<Set_or_Bag_hashval,unsigned>& m) const {
    //  Iterate over buckets, creating a Map from
    //  bp->hashval to bp->collision_list.length().

    Bucketiter_ATTLC<T> bi(*this);
    const Bucket_ATTLC<T>* bp = bi.first();
    m = Map<Set_or_Bag_hashval,unsigned>();

    while ( bp ) {
	m[bp->hashval] = bp->collision_list.length();
	bp = bi.next();
    }
}

template <class T>
void
Set<T>::check() const
{
    Bucketiter_ATTLC<T> bi(*this);
    const Bucket_ATTLC<T>* bp = bi.first();
#ifdef DEBUG_ATTLC
    Set_or_Bag_hashval oldhashval=0;  // initialize to avoid warning
#endif
    int first=1;

    while ( bp )
    {
	//  Buckets must be stored in increasing order of
	//  hash value

	if ( first )
	    first=0;
	else {
#ifdef DEBUG_ATTLC
	    assert(SET_LT_ATTLC(oldhashval,bp->hashval));
#endif
	}
#ifdef DEBUG_ATTLC
	oldhashval = bp->hashval;
#endif

	//  Collision lists may not be empty

#ifdef DEBUG_ATTLC
	assert(bp->collision_list.length()>0);
#endif

	//  Collision lists may not contain duplicates

	Listiter<T> it1(((Bucket_ATTLC<T>*)bp)->collision_list);
	Listiter<T> it2(((Bucket_ATTLC<T>*)bp)->collision_list);

	while ( !it1.at_end() ) {
	    it2.reset();
	    while ( !it2.at_end() ) {
		T* p1;
		T* p2;
		it1.next(p1);
		it2.next(p2);
#ifdef DEBUG_ATTLC
		assert(*p1 == *p2);
#endif
	    }
	}
	bp = bi.next();
    }
}

template <class T>
Set<T>::~Set()
{
    //  TBD_study this

/*
    for ( register Bucketiter_ATTLC<T>* it = iter_head ; it ; it = it->next_it )
    {
	//  Break const

	Set<T>	// cheat = (Set<T>//)&it->my_set;
	*cheat = 0;
    }
*/
    remove_all();
}

template <class T>
Set<T>::Set(const Set<T>& oo) : sze(0), iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    Bucketiter_ATTLC<T> bi(oo);
    const Bucket_ATTLC<T>* bp = bi.first();

    while ( bp ) {
	Listiter<T> li(((Bucket_ATTLC<T>*)bp)->collision_list);
	while ( !li.at_end() ) {
	    T* tp;
	    li.next(tp);
	    insert(*tp);
	}
	bp = bi.next();
    }
}

template <class T>
void
Set<T>::make_setlogic(const Set<T>& a, const Set<T>& b, int l_type)
{
    //  l_type = 0 (intersection), 1 (union), 2 (set difference), 3 (xor)

#ifdef DEBUG_ATTLC
    assert(sze == 0);
#endif
    Bucketiter_ATTLC<T>     a_it(a), b_it(b);
    register const Bucket_ATTLC<T> *ap = a_it.first(), *bp = b_it.first();
    T *atp, *btp;

    while ( ap && bp ) {
	Listiter<T> lia(((Bucket_ATTLC<T>*)ap)->collision_list);
	Listiter<T> lib(((Bucket_ATTLC<T>*)bp)->collision_list);

	if ( ap->hashval == bp->hashval ) {
	    // insert all of the necessary items from B

	    if ( l_type == 1 ) {
		while ( !lib.at_end() ) {
		    lib.next(btp);
		    insert(*btp);
		}
	    }
	    else if ( l_type == 3 ) {
		while ( !lib.at_end() ) {
		    lib.next(btp);
		    lia.reset();
		    int found=0;
		    while ( !lia.at_end() ) {
			lia.next(atp);
			if ( *atp == *btp ) {
			    found=1;
			    break;
			}
		    }
		    if ( !found )
			insert(*btp);
		}
	    }

	    // insert all the necessary items from A

	    lia.reset();
	    while ( !lia.at_end() ) {
		lia.next(atp);
		lib.reset();
		int found=0;
		while ( !lib.at_end() ) {
		    lib.next(btp);
		    if ( *atp == *btp ) {
			found=1;
			break;
		    }
		}
		if ( found ) {
		    if ( l_type == 0 )
			insert(*atp);
		}
		else {
		    if ( l_type != 0 )
			insert(*atp);
		}
	    }
	    ap = a_it.next();
	    bp = b_it.next();
	}
	else if ( SET_LT_ATTLC(ap->hashval,bp->hashval) ) {
	    // an entire bucket of values in A but not in B

	    if (l_type != 0 )
		while ( !lia.at_end() )
		{
		    lia.next(atp);
		    insert(*atp);
		}

	    ap = a_it.next();
	}
	else {
	    // an entire bucket of values in B but not in A

	    if (l_type == 1 || l_type == 3 )
		while ( !lib.at_end() ) {
		    lib.next(btp);
		    insert(*btp);
		}

	    bp = b_it.next();
	}
    }

    //  Insert whatever's left over from a and b

    if ( l_type != 0 )
	for ( ;ap;ap = a_it.next() ) {
	    Listiter<T> lia(((Bucket_ATTLC<T>*)ap)->collision_list);
	    while ( !lia.at_end() ) {
		lia.next(atp);
		insert(*atp);
	    }
	}

    if ( l_type == 1 || l_type == 3 ) {
	for ( ; bp ; bp = b_it.next() ) {
	    Listiter<T> lib(((Bucket_ATTLC<T>*)bp)->collision_list);
	    while ( !lib.at_end() ) {
		lib.next(btp);
		insert(*btp);
	    }
	}
    }
}

template <class T>
const T*
Set<T>::select() const
{
    Bucketiter_ATTLC<T> bi(*this);
    const Bucket_ATTLC<T>* bp = bi.first();

    if ( !bp )
	return 0;

    Listiter<T> li(((Bucket_ATTLC<T>*)bp)->collision_list);
    T* tp;
    li.next(tp);
    return tp;
}

#include <stream.h>

template <class T>
ostream&
Set<T>::print(ostream& os) const
{
    os << "{";
    Setiter<T> it(*this);
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

#endif
