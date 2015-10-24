/*ident	"@(#)Set:incl/bag.c	3.1" */
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

#ifndef _BAG_C_
#define _BAG_C_

//  T is the type parameter;
//  H is the name of the hash function
//  (Bag_zero should be used if the client does
//  not supply its own function).

template <class T>
Bag_bucketiter_ATTLC<T>::~Bag_bucketiter_ATTLC()
{
    if ( my_bag == 0 )
	return;

    if ( this == my_bag->iter_head ) {
	//  Case 1: iterator at head of chain

	my_bag->iter_head = next_it;
    }
    else {
	//  Case 2: iterator in middle of chain

	Bag_bucketiter_ATTLC<T>* x;
	Bag_bucketiter_ATTLC<T>* an_it;
	for ( an_it=my_bag->iter_head ;
	      (x = an_it->next_it) != this ; an_it = x );

	an_it->next_it = next_it;
    }
}

template <class T>
Bag_bucketiter_ATTLC<T>::Bag_bucketiter_ATTLC(const Bag<T>& b):
    my_bag((Bag<T>*)&b),
    next_it(b.iter_head)
{ 
    ((Bag<T>*)(&b))->iter_head = this;
}

template <class T>
Bag_bucketiter_ATTLC<T>::Bag_bucketiter_ATTLC(const Bag_bucketiter_ATTLC<T>& bi)
	:   my_bag(bi.my_bag),
	    next_it(my_bag->iter_head)
{ 
    my_bag->iter_head = this;
}
	
template <class T>
Bag_bucketiter_ATTLC<T>&
Bag_bucketiter_ATTLC<T>::operator=(const Bag_bucketiter_ATTLC<T>& bi)
{
	if (this == my_bag->iter_head) {
		my_bag->iter_head = next_it;
	} else {
		Bag_bucketiter_ATTLC<T>* p = my_bag->iter_head;
		while (p->next_it != this) {
		    p = p->next_it;
		}
		p->next_it = next_it;
	}
	my_bag = bi.my_bag;
	next_it = my_bag->iter_head;
	my_bag->iter_head = this;
	return *this;
}


//  first() and next() are similar to Shopiro's
//  original pset iterator functions  first() and
//  next(); they walk the leaves of the tree,
//  returning Bag_bucket_ATTLC<T>)*.

template <class T>
const Bag_bucket_ATTLC<T>*
Bag_bucketiter_ATTLC<T>::first() 
{
    Bag_internal_item_ATTLC* itemp;
    Bag_internal_node_ATTLC* nodep;
    Bag_bucket_ATTLC<T>* bp;

    if ( my_bag == 0 )
	return 0;

    pos.curr_depth = -1;

    if ( my_bag->contents.is_null() )
	return 0;

    if ( my_bag->contents.is_leaf() ) {
	//  'contents' is a Bag_bucket_ATTLC<T>*

	bp = (Bag_bucket_ATTLC<T>*)my_bag->contents.external_leaf();
    }
    else {
	//  'contents' is a Bag_internal_node_ATTLC*.  Search the
	//  subtree rooted at this node looking for the
	//  leftmost leaf.

	nodep = my_bag->contents.next_node();

	for ( ; ; ) {
#ifdef DEBUG_ATTLC
	    assert(pos.curr_depth < BAG_POSITIONS_ATTLC);
#endif
	    //  Scan this Bag_internal_node_ATTLC from left to right
	    //  looking for a non-null Bag_internal_item_ATTLC

	    for ( itemp = &nodep->item[0];;itemp++ ) {
#ifdef DEBUG_ATTLC
		assert(itemp < &nodep->item[BAG_NODE_SIZE_ATTLC]);
#endif
		if ( !itemp->is_null() )
		    break;
	    }
	    pos.curr_pos[++pos.curr_depth] = itemp;

	    //  If this Bag_internal_item_ATTLC is a leaf, the search 
	    //  is over; otherwise, iteratively 'recurse'

	    if ( itemp->is_leaf() )
		break;
	    nodep = itemp->next_node();
	}
	bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
    }

    //  bp now points to a Bag_bucket_ATTLC<T>

    pos.curr_value = bp->hashval;
    return bp;
}

template <class T>
const Bag_bucket_ATTLC<T>*
Bag_bucketiter_ATTLC<T>::next()
{
    Bag_internal_item_ATTLC* itemp;
    Bag_internal_node_ATTLC* nodep;
    Bag_bucket_ATTLC<T>* bp;

    if ( my_bag == 0 )
	return 0;

    //  Search the index for pos.curr_value.  Since
    //  pos caches the result of the last iterator
    //  access, the loop should exit almost immediately,
    //  unless Bag mutation has occurred since the last
    //  access.

    if ( pos.curr_depth == -1 )
	itemp = &my_bag->contents;
    else
	itemp = pos.curr_pos[pos.curr_depth];

    int unshift = BAG_INITIAL_SHIFT_ATTLC + BAG_SHIFT_INCR_ATTLC +
    		  pos.curr_depth * BAG_SHIFT_INCR_ATTLC;
    long mask = BAG_MASK_BITS_ATTLC << unshift;
    nodep = 0;

    for ( ; ; mask <<= BAG_SHIFT_INCR_ATTLC, unshift += BAG_SHIFT_INCR_ATTLC ) {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < BAG_POSITIONS_ATTLC);
#endif
	if ( itemp->is_null() )
	    break;
	else if ( itemp->is_leaf() ) {
	    //  While searching for current_value, we have found
	    //  a leaf.  This may or may not be the leaf containing
	    //  the current value, depending on whether the
	    //  Bag_bucket containing the current value has been
	    //  deleted or not.

	    bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	    Set_or_Bag_hashval hval = bp->hashval;

	    if ( hval == pos.curr_value || BAG_LT_ATTLC(hval,pos.curr_value) ) {
		//  Case 1: we either (a) found the current value or
		//  (b) passed it (it must have been deleted).

		break;
	    }
	    else {
		//  The leaf points to a Bag_bucket containing a hash
		//  value GREATER THAN the current value, so the
		//  current value must have been deleted.  The
		//  Bag_bucket pointed to is, by definition, the "next"
		//  Bag_bucket, the one we arelooking for.

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
    //  to locate the NEXT Bag_bucket_ATTLC<T> starting from
    //  here.

    if ( pos.curr_depth == -1 ) {
	//  There was only one hash Bag_bucket; the iteration
	//  is therefore terminated.

	return 0;
    }

    //  Find the next Bag_bucket_ATTLC<T>

    if ( nodep == 0 ) {
	if ( pos.curr_depth == 0 )
	    nodep = my_bag->contents.next_node();
	else
	    nodep = pos.curr_pos[pos.curr_depth-1]->next_node();
    }
    for (  ; ; ) {
#ifdef DEBUG_ATTLC
	assert(&nodep->item[0] <= itemp  &&  itemp < &nodep->item[BAG_NODE_SIZE_ATTLC]);
#endif
	//  Scan rightward within this node, looking for a
	//  non-null item

	while ( itemp < &nodep->item[BAG_NODE_SIZE_ATTLC-1] )
	    if ( !(++itemp)->is_null() )
		goto found;

	//  Scan reached end of node without finding anything;
	//  pop up one level in order to continue the walk

	if ( pos.curr_depth-- == 0 ) {
	    //  The Bag is exhausted, so the iteration is terminated

	    return 0;
	}
	itemp = pos.curr_pos[pos.curr_depth];
	if ( pos.curr_depth == 0 )
	    nodep = my_bag->contents.next_node();
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

	for ( itemp = &nodep->item[0];;itemp++ ) {
#ifdef DEBUG_ATTLC
	    assert(itemp < &nodep->item[BAG_NODE_SIZE_ATTLC]);
#endif
	    if ( !itemp->is_null() )
		break;
	}
	pos.curr_pos[++pos.curr_depth] = itemp;
    }
    bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
    pos.curr_value = bp->hashval;
    return bp;
}

template <class T>
const Bag_pair<T>*
Bagiter<T>::first()
{
    //  This version has been simplified by using
    //  Bag_bucketiter::first()

    const Bag_bucket_ATTLC<T>* bp = ((Bag_bucketiter_ATTLC<T>*)this)->first();

    if (  bp == 0 )
	return 0;

    delete itp;
    itp=new Listiter< Bag_pair<T> >(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
    Bag_pair<T>* result;
    itp->next(result);

    //  Cast the pointer to const so the client can't
    //  modify the element

    return (const Bag_pair<T>*)result;
}

template <class T>
const Bag_pair<T>*
Bagiter<T>::next()
{
    Bag_internal_item_ATTLC* itemp;
    Bag_internal_node_ATTLC* nodep;
    Bag_bucket_ATTLC<T>* bp;
    Bag_pair<T>* result;

    //  TBD_see if this code can be simplified by using
    //  Bag_bucketiter_ATTLC<T>::next()

#ifdef DEBUG_ATTLC
    assert(my_bag);
#endif

    //  New List-style iterator has next() as only public
    //  member function; first() is private and called
    //  internally only if this is the first call to next(),
    //  i.e., only if inited=0.

    if (  inited==0 ) {
	const Bag_pair<T>* tp = first();
	if ( tp ) inited=1;
	return tp;
    }

    //  Search the index for the pos.curr_value.  Since
    //  pos normally caches the results of the
    //  most recent iterator access, the loop should
    //  exit almost immediately.

    if ( pos.curr_depth == -1)
	itemp = &my_bag->contents;
    else
	itemp = pos.curr_pos[pos.curr_depth];

    int unshift = BAG_INITIAL_SHIFT_ATTLC + BAG_SHIFT_INCR_ATTLC +
		  pos.curr_depth * BAG_SHIFT_INCR_ATTLC;
    long mask = BAG_MASK_BITS_ATTLC << unshift;
    nodep = 0;

    for ( ; ; mask <<= BAG_SHIFT_INCR_ATTLC, unshift += BAG_SHIFT_INCR_ATTLC ) {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < BAG_POSITIONS_ATTLC);
#endif
	if ( itemp->is_null() )
	    break;
	else if ( itemp->is_leaf() ) {
	    //  While searching for current_value, we have found
	    //  a leaf.  This may or may not be the leaf containing
	    //  the current value, depending on whether the
	    //  Bag_bucket containing the current value has been
	    //  deleted or not.

	    bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	    Set_or_Bag_hashval hval = bp->hashval;

	    if ( hval == pos.curr_value || BAG_LT_ATTLC(hval,pos.curr_value) ) {
		//  Case 1: we either (a) found the current value or
		//  (b) passed it (it must have been deleted)

		break;
	    }
	    else {
		//  The leaf we have found points to a Bag_bucket
		//  containing a next GREATER THAN the current value
		//  (the current value must have been deleted).

		pos.curr_value = hval;
		delete itp;
		itp = new Listiter< Bag_pair<T> >(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
		itp->next(result);
		return (const Bag_pair<T>*)result;
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

    if ( pos.curr_depth == -1 ) {
	//  There is only one Bag_bucket; if we have not not 
	//  yet returned all Bag_pairs on the collision list,
	//  return the next one; otherwise, the iteration
	//  is terminated
	//
	//  TBD_note:  It is possible that Bag mutation occurred
	//  that (1) deleted all the elements in a List and then
	//  (2) re-created a List with the same hash value.  In
	//  this case, *itp refers to a non-existeng List.  We
	//  must guard against this case by using Listiter::the_list.

	if ( !itp->the_list() )
	    return 0;
	else {
	    if ( itp->at_end() )
		return 0;
	    else {
		itp->next(result);
		return (const Bag_pair<T>*)result;
	    }
	}
    }

    //  There are multiple Bag_buckets.

    if ( itemp->is_leaf() &&
	 ((Bag_bucket_ATTLC<T>*)itemp->external_leaf())->hashval== pos.curr_value )
    {
	//  See if there are more elements in this bucket

	if ( itp->the_list() ) {
	    if ( !itp->at_end() ) {
		//  Move on to the next element

		itp->next(result);
		return (const Bag_pair<T>*)result;
	    }
	}
    }

    //  Find the next leaf.
    //  TBD_can we use Bag_bucketiter_ATTLC<T>::next() here?

    if ( nodep == 0 ) {
	if ( pos.curr_depth == 0 )
		nodep = my_bag->contents.next_node();
	else
	    nodep = pos.curr_pos[pos.curr_depth-1]->next_node();
    }

    for (  ; ; ) {
#ifdef DEBUG_ATTLC
	assert(&nodep->item[0] <= itemp  &&  itemp < &nodep->item[BAG_NODE_SIZE_ATTLC]);
#endif
	//  Scan rightward within this node, looking for a
	//  non-null item

	while ( itemp < &nodep->item[BAG_NODE_SIZE_ATTLC-1] ) {
	    if ( !(++itemp)->is_null() )
		goto found;
	}

	//  Scan reached end of node without finding anything;
	//  pop up one level in order to continue the walk

	if ( pos.curr_depth-- == 0 ) {
	    //  The Bag is exhausted, so the iteration is terminated

	    return 0;
	}
	itemp = pos.curr_pos[pos.curr_depth];
	if ( pos.curr_depth == 0 )
	    nodep = my_bag->contents.next_node();
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

	for (itemp = &nodep->item[0];;itemp++ ) {
#ifdef DEBUG_ATTLC
	    assert(itemp < &nodep->item[BAG_NODE_SIZE_ATTLC]);
#endif
	    if ( !itemp->is_null() )
		break;
	}
	pos.curr_pos[++pos.curr_depth] = itemp;
    }
    bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
    pos.curr_value = bp->hashval;
    delete itp;
    itp = new Listiter< Bag_pair<T> >(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
    itp->next(result);
    return (const Bag_pair<T>*)result;
}

template <class T>
Bag<T>::Bag() : sze(0),sze_unique(0),iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
}

template <class T>
Bag<T>::Bag(const T& t0) : sze(0),sze_unique(0),iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    insert(t0);
}

template <class T>
Bag<T>::Bag(const T& t0, const T& t1):
    sze(0),
    sze_unique(0),
    iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    insert(t0);
    insert(t1);
}

template <class T>
Bag<T>::Bag(const T& t0, const T& t1, const T& t2):
    sze(0),
    sze_unique(0),
    iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    insert(t0);
    insert(t1);
    insert(t2);
}

template <class T>
Bag<T>::Bag(const T& t0, const T& t1, const T& t2, const T& t3):
    sze(0),
    sze_unique(0),
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
Bag<T>::Bag(const Bag<T>& a, const Bag<T>& b, Bag_union_ATTLC*):
    sze(0),
    sze_unique(0),
    iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,1);
}

template <class T>
Bag<T>::Bag(const Bag<T>& a, const Bag<T>& b, Bag_inter_ATTLC*):
    sze(0),
    sze_unique(0),
    iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,0);
}

template <class T>
Bag<T>::Bag(const Bag<T>& a, const Bag<T>& b, Bag_diff_ATTLC*):
    sze(0),
    sze_unique(0),
    iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,2);
}

template <class T>
Bag<T>::Bag(const Bag<T>& a, const Bag<T>& b, Bag_xor_ATTLC*):
    sze(0),
    sze_unique(0),
    iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    make_setlogic(a,b,3);
}

template <class T>
Bag<T>&
Bag<T>::operator=(const Bag<T>& b)
{
    if ( this != &b ) {
	remove_all();
	Bag_bucketiter_ATTLC<T> bi(b);
	const Bag_bucket_ATTLC<T>* bp = bi.first();

	while ( bp ) {
	    Listiter< Bag_pair<T> > li(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

	    while ( !li.at_end() ) {
		Bag_pair<T>* tp;
		li.next(tp);
		insert(tp->value,tp->count);
	    }
	    bp = bi.next();
	}
    }
    return *this;
}

template <class T>
int
Bag<T>::containment(const Bag<T>& b, int comp_type) const
{
    switch(comp_type) {
    case 0: // bag equality
	if ( size() != b.size() )
	    return 0;
	break;
    case 1: // subset
	if ( size() > b.size() )
	    return 0;
	break;
    case 2: // proper subset
	if ( size() >= b.size() )
	    return 0;
	break;
    }

    Bag_bucketiter_ATTLC<T> a_it(*this);
    Bag_bucketiter_ATTLC<T> b_it(b);
    const Bag_bucket_ATTLC<T>* ap = a_it.first();
    const Bag_bucket_ATTLC<T>* bp = b_it.first();
    while ( ap && bp ) {
	if ( ap->hashval == bp->hashval ) {
	    //  Make sure ap->collision_list is a subset of
	    //  bp->collision_list

	    if ( ap->collision_list.length() > bp->collision_list.length() )
		return 0;

	    Listiter< Bag_pair<T> > lia(((Bag_bucket_ATTLC<T>*)ap)->collision_list);
	    Listiter< Bag_pair<T> > lib(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
	    while ( !lia.at_end() ) {
		Bag_pair<T>* atp;
		lia.next(atp);
		lib.reset();
		int ok=0;
		while ( !lib.at_end() ) {
		    Bag_pair<T>* btp;
		    lib.next(btp);
		    if ( atp->value == btp->value && atp->count <= btp->count ) {
			ok=1;
			break;
		    }
		}
		if ( !ok )
		    return 0;
	    }
	    ap = a_it.next();
	    bp = b_it.next();
	}
	else if ( BAG_LT_ATTLC(ap->hashval,bp->hashval) ) {
	    //  *ap contains values that can't be in b, since we've
	    //  already passed the point where they would be found;
	    //  return failure

	    return 0;
	}
	else {
	    //  *ap contains values that may be in a future
	    //  Bag_bucket of b; increment b_it

	    bp = b_it.next();
	}
    }

    //  The relation is true only if a_it is exhasted

    return ap==0;
}

template <class T>
const Bag_pair<T>*
Bag<T>::insert(const T& value, int count)
{
    if (  count<=0 )
	return 0;

    Bag_internal_item_ATTLC* itemp;
    Bag_internal_node_ATTLC* nodep;
    Bag_bucket_ATTLC<T>* bp;
    Bag_pair<T>* result;

    //  Hash the value

    Set_or_Bag_hashval hval = hash(value);

    //  See how much of hval matches pos.curr_value
    //  (equivalently, see how much of pos is valid.)
    //  This will determine where we start looking for the
    //  insertion point.  This is an optimization that takes
    //  advantage of the phenomenon that many operations
    //  create a Bag by walking an existing index and
    //  inserting its elements into the new Bag.

    long mask = BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC;

    int depth;
    for ( depth = -1 ; depth < pos.curr_depth ;
	  depth++, mask <<= BAG_SHIFT_INCR_ATTLC ) {
	if ( (pos.curr_value & mask) != (hval & mask) )
	    break;
    }

    //  If depth = -1, no bits match;
    //  if depth = 0, bits 0..3 match, etc.

    register int unshift = BAG_INITIAL_SHIFT_ATTLC + BAG_SHIFT_INCR_ATTLC +
			   depth * BAG_SHIFT_INCR_ATTLC;
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
	  mask <<= BAG_SHIFT_INCR_ATTLC, unshift += BAG_SHIFT_INCR_ATTLC  )
    {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < BAG_POSITIONS_ATTLC);
#endif
	if ( itemp->is_null() )
	    break;
	else if ( itemp->is_leaf() ) {
	    bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	    if ( bp->hashval == hval ) {
		//  The 'value' goes in this Bag_bucket

		Listiter< Bag_pair<T> > it(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
		Bag_pair<T>* tp;

		while ( it.next(tp) ) {
		    if ( tp->value == value ) {
			sze += count;
			tp->count += count;
			return tp;
		    }
		}

		//  The value is not already in the Bag_bucket;
		//  insert it

		sze_unique+=1;
		sze += count;
		Bag_pair<T> pair1;
		pair1.value = value;
		pair1.count = count;
		it.insert_next(pair1);
		it.peek_next(result);
		return result;
	    }
	    else {
		//  This Bag_bucket is not the one where the value
		//  belongs; a subindex will have to be created
		//  containing both this Bag_bucket and a new one
		//  created to hold 'value.'

		break;
	    }
	}
	else {
	    //  Node: search subindex

	    nodep = itemp->next_node();
#ifdef DEBUG_ATTLC
	    assert(nodep);
#endif
	    pos.curr_pos[++pos.curr_depth] = itemp = &nodep->item[(hval & mask) >> unshift];
	}
    }

    //  ASSERT: itemp is either null or is a leaf that must
    //  be moved down into a subindex containing BOTH the
    //  existing leaf and a new leaf pointing to a new
    //  Bag_bucket containing 'value.'  In either case, the
    //  cardinality of the Bag is increased by 'count'.

    sze_unique += 1;
    sze += count;
    if ( itemp->is_null() ) {
	//  Make this null slot into a leaf pointing to a new
	//  Bag_bucket containing 'value'

	itemp->make_leaf( new Bag_bucket_ATTLC<T> );
	bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	bp->hashval = hval;
	Bag_pair<T> pair2;
	pair2.value = value;
	pair2.count = count;
	// bp->collision_list.insert_next(pair2);
	// bp->collision_list.peek_next(result);

	Listiter< Bag_pair<T> > cit(bp->collision_list);
	cit.insert_next(pair2);
	cit.peek_next(result);

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
	//  Bag_bucket containing 'value'

	bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	Set_or_Bag_hashval temp = bp->hashval;
	int ind1,ind2;
	for ( ; ; mask <<= BAG_SHIFT_INCR_ATTLC, unshift += BAG_SHIFT_INCR_ATTLC )
	{
	    itemp->make_node(nodep = new Bag_internal_node_ATTLC);
#ifdef DEBUG_ATTLC
	    assert(pos.curr_depth < BAG_POSITIONS_ATTLC);
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

	//  Make a new leaf and Bag_bucket for 'value'

	itemp = &nodep->item[ind2];
	itemp->make_leaf( new Bag_bucket_ATTLC<T> );
	bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	bp->hashval = hval;
	Bag_pair<T> pair;
	pair.value = value;
	pair.count = count;
	// bp->collision_list.insert_next(pair);
	// bp->collision_list.peek_next(result);

	Listiter< Bag_pair<T> > bit(bp->collision_list);
	bit.insert_next(pair);
	bit.peek_next(result);

	pos.curr_pos[++pos.curr_depth] = itemp;
	nodep->busy_count = 2;
	return result;
    }
}

template <class T>
unsigned int
Bag<T>::remove(const T& value, int count)
{
    if (  count<=0 )
	return 0;

    Bag_internal_item_ATTLC* itemp;
    Bag_internal_node_ATTLC* nodep;
    Bag_bucket_ATTLC<T>* bp=0;
    unsigned int result;

    //  Hash the value

    Set_or_Bag_hashval hval = hash(value);

    //  See how much of hval matches pos.curr_value
    //  (equivalently, see how much of pos is valid.)
    //  This will determine where we start looking for the
    //  deletion point.  This is an optimization that takes
    //  advantage of the phenomenon that many operations
    //  create a Bag by walking an existing index and
    //  inserting its elements into the new Bag.

    long mask = BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC;

    int depth;
    for ( depth = -1 ; depth < pos.curr_depth ;
	  depth++, mask <<= BAG_SHIFT_INCR_ATTLC ) {
	if ( (pos.curr_value & mask) != (hval & mask) )
	    break;
    }

    //  If depth = -1, no bits match;
    //  If depth = 0, bits 0...3 match, etc.

    register int unshift = BAG_INITIAL_SHIFT_ATTLC + BAG_SHIFT_INCR_ATTLC +
			   depth * BAG_SHIFT_INCR_ATTLC;
    if ( depth == -1 )
	itemp = &contents;
    else
	itemp = pos.curr_pos[depth];

    nodep = 0;
    pos.curr_value = hval;
    warn_iterators();

    //  Find the leaf pointing to the Bag_bucket containing
    //  the value to be deleted.

    for ( pos.curr_depth = depth ; ; mask <<= BAG_SHIFT_INCR_ATTLC,
	  unshift += BAG_SHIFT_INCR_ATTLC ) {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < BAG_POSITIONS_ATTLC);
#endif
	if ( itemp->is_null() ) {
	    //  The value must have been deleted already.
	    //  Return failure.

	    return 0;
	}
	if ( itemp->is_leaf() ) {
	    bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	    if ( bp->hashval == hval )
		//  The value may be in the Bag_bucket pointed to by
		//  this leaf.
		break;
	    else
		//  The leaf must have been deleted and subsequently
		//  replaced by a different Bag_bucket (one with a
		//  different hash value).  Return failure.
		return 0;
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

    //  We found the Bag_bucket; locate the value in the
    //  collision list and remove it

    Listiter< Bag_pair<T> > it(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
    Bag_pair<T>* tp;
    int found=0;
    while ( it.next(tp) ) {
	if ( tp->value == value ) {
	    found=1;
	    if ( tp->count > count ) {
		result = count;
		tp->count -= result;
	    }
	    else {
		sze_unique -= 1;
		result = tp->count;
		it.remove_prev();
	    }
	    sze -= result;
	    break;
	}
    }
    if ( !found )
	//  The value was not found; return failure
	return 0;

    //  The value was found and deleted.  We must now
    //  worry about the Bag_bucket becoming empty.

    if ( bp->collision_list.length() > 0 )
	//  The collision list still has one or more elements.
	return result;

    //  The collision list has become empty as a result of
    //  this deletion; we must delete the Bag_bucket and
    //  fix up the index accordingly.

    if ( pos.curr_depth == -1 ) {
	//  Contents was a leaf and its Bucket now contains
	//  an empty list; the Bag is now empty

#ifdef DEBUG_ATTLC
	assert(sze == 0);
#endif
	delete bp;
	itemp->make_null();
	return result;
    }

    //  Nodep is the pointer to the node containing
    //  this leaf (it will be zero if the cache was
    //  up-to-date when we entered this function

    if ( nodep == 0 ) {
	if ( pos.curr_depth == 0 )
	    nodep = contents.next_node();
	else
	    nodep = pos.curr_pos[pos.curr_depth-1]->next_node();
    }
    if ( --(nodep->busy_count) > 1 )
    {
	//  Easy case: node still has at least one leaf left

	delete bp;
	itemp->make_null();
	return result;
    }

    //  Hard case: node has only one leaf in it; must
    //  collapse node into a leaf

#ifdef DEBUG_ATTLC
    assert(nodep->busy_count == 1);
#endif

    //  Scan for a non-null item

    Bag_internal_item_ATTLC* itp;
    for ( itp = &nodep->item[0] ;
	  itp < &nodep->item[BAG_NODE_SIZE_ATTLC] ; itp++ ) {
	if ( itp != itemp && !itp->is_null() )
	    break;
    }
    if ( !itp->is_leaf() ) {
	//  Complicated case: punt (we won't try to absorb
	//  a node)

	delete bp;
	itemp->make_null();
	return result;
    }
    Bag_bucket_ATTLC<T>* temp = (Bag_bucket_ATTLC<T>*)itp->external_leaf();
    delete bp;

    for ( ;; ) {
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
    return result;
}

template <class T>
unsigned int
Bag<T>::remove_all(const T& value)
{
    return remove(value,count(value));
}

template <class T>
unsigned int
Bag<T>::remove_all()
{
    unsigned int result = sze;
    warn_iterators();
    Bag_internal_item_ATTLC* itemp = &contents;

    if ( itemp->is_null() ) {
#ifdef DEBUG_ATTLC
	assert(sze == 0);
	assert(sze_unique == 0);
#endif
	return result;
    }
    if ( itemp->is_leaf() ) {
#ifdef DEBUG_ATTLC
	assert(sze >= 1);
	assert(sze_unique >= 1);
#endif
	//  We must destroy the bucket

	Bag_bucket_ATTLC<T>* bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	delete bp;
	itemp->make_null();
	pos.curr_depth = -1;
	sze = 0;
	sze_unique = 0;
	return result;
    }
    Bag_internal_node_ATTLC* nodep = itemp->next_node();
    itemp = &nodep->item[0];
    pos.curr_depth = -1;

    for ( ; ; ) {
	Bag_internal_item_ATTLC* stopper = (&nodep->item[BAG_NODE_SIZE_ATTLC]);
	for ( ; itemp < stopper ; /* itemp++ */ )
	{
	    if ( itemp->is_node() ) {
		pos.curr_pos[++pos.curr_depth] = itemp;
		nodep = itemp->next_node();
		itemp = &nodep->item[0];
		stopper = &nodep->item[BAG_NODE_SIZE_ATTLC];
	    }
	    else itemp++;
	}

	//  Unlike Set_of_p, we must destroy the buckets
	//  pointed to by this node

	for ( itemp = &nodep->item[0] ; itemp < &nodep->item[BAG_NODE_SIZE_ATTLC] ; itemp++ )
	{
	    if ( itemp->is_leaf() ) {
		Bag_bucket_ATTLC<T>* bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
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
    sze_unique = 0;
    return result;
}

template <class T>
const Bag_pair<T>*
Bag<T>::select() const
{
    Bag_bucketiter_ATTLC<T> bi(*this);
    const Bag_bucket_ATTLC<T>* bp = bi.first();

    if ( !bp )
    {
	return 0;
    }
    Listiter< Bag_pair<T> > li(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
    Bag_pair<T>* result;
    li.next(result);
    return result;
}

template <class T>
const Bag_pair<T>*
Bag<T>::contains(const T& value) const
{
    Bag_internal_item_ATTLC* itemp;
    Bag_internal_node_ATTLC* nodep;
    Bag_bucket_ATTLC<T>* bp;

    //  Hash the value

    Set_or_Bag_hashval hval = hash(value);

    //  See how much of pos is still good

    long mask = BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC;

    int depth;
    for ( depth = -1 ; depth < pos.curr_depth ; depth++, mask <<= BAG_SHIFT_INCR_ATTLC )
    {
	if ( (pos.curr_value & mask) != (hval & mask) )
	    break;
    }
    register int unshift = BAG_INITIAL_SHIFT_ATTLC + BAG_SHIFT_INCR_ATTLC +
			   depth * BAG_SHIFT_INCR_ATTLC;
    if ( depth == -1 )
	itemp = &(((Bag<T> *)this)->contents);
    else
	itemp = pos.curr_pos[depth];
    nodep = 0;
    ((Bag_position_ATTLC *) &((Bag<T>*)this)->pos)->curr_value = hval;

    for ( ((Bag_position_ATTLC *) &((Bag<T>*)this)->pos)->curr_depth = depth ; 	;
	  mask <<= BAG_SHIFT_INCR_ATTLC, unshift += BAG_SHIFT_INCR_ATTLC )
    {
#ifdef DEBUG_ATTLC
	assert(pos.curr_depth < BAG_POSITIONS_ATTLC);
#endif
	if ( itemp->is_null() )
	    return 0;

	if ( itemp->is_leaf() ) {
	    //  Search the the collision list

	    bp = (Bag_bucket_ATTLC<T>*)itemp->external_leaf();
	    Listiter< Bag_pair<T> > it(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
	    Bag_pair<T>* tp;

	    while ( it.next(tp) )
		if ( tp->value == value )
		    return tp;

	    return 0;
	}
	else {
	    nodep = itemp->next_node();
#ifdef DEBUG_ATTLC
	    assert(nodep);
#endif
	    ((Bag_position_ATTLC *) &((Bag<T>*)this)->pos)->curr_pos[++((Bag_position_ATTLC *) &((Bag<T>*)this)->pos)->curr_depth] = itemp =
		&nodep->item[(pos.curr_value & mask) >> unshift];
	}
    }
}

template <class T>
unsigned int
Bag<T>::count(const T& value) const
{
    //  New version uses latest version of contains

    const Bag_pair<T>* tp = contains(value);
    if ( tp )
	return tp->count;
    else
	return 0;
}

template <class T>
Bag<T>&
Bag<T>::operator|=(const Bag<T>& b)
{
    if ( this != &b ) {
	Bag_bucketiter_ATTLC<T> bi(b);
	const Bag_bucket_ATTLC<T>* bp = bi.first();
	while ( bp ) {
	    Listiter< Bag_pair<T> > it(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
	    Bag_pair<T>* tp;
	    while ( it.next(tp) )
		insert(tp->value,tp->count);

	    bp = bi.next();
	}
    }
    else {
	//  Union with self: double all counts

	Bag_bucketiter_ATTLC<T> bi(*this);
	const Bag_bucket_ATTLC<T>* bp = bi.first();
	while ( bp ) {
	    Listiter< Bag_pair<T> > it(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
	    Bag_pair<T>* tp;
	    while ( it.next(tp) )
		tp->count *= 2;

	    bp = bi.next();
	}
    }
    return *this;
}

template <class T>
Bag<T>&
Bag<T>::operator-=(const Bag<T>& b)
{
    if ( this != &b ) {
	Bag_bucketiter_ATTLC<T> bi(b);
	const Bag_bucket_ATTLC<T>* bp = bi.first();
	while ( bp ) {
	    Listiter< Bag_pair<T> > it(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
	    Bag_pair<T>* tp;

	    while ( it.next(tp) ) {
		remove(tp->value,tp->count);
	    }

	    bp = bi.next();
	}
    }
    else
	remove_all();

    return *this;
}

template <class T>
Bag<T>&
Bag<T>::operator&=(const Bag<T>& b)
{
    if ( this != &b ) {
	Bag_bucketiter_ATTLC<T> ai(*this);
	const Bag_bucket_ATTLC<T>* ap = ai.first();
	while ( ap ) {
	    Listiter< Bag_pair<T> > it(((Bag_bucket_ATTLC<T>*)ap)->collision_list);
	    Bag_pair<T>* tp;
	    while ( it.the_list() && it.next(tp) ) {
		//  remove may delete the bucket; beware of dangling
		//  list iterator.

		int my_count = tp->count;
#ifdef DEBUG_ATTLC
		assert(my_count>0);
#endif
		int b_count = b.count(tp->value);
		int retain;
		if ( my_count < b_count )
		    retain = my_count;
		else
		    retain = b_count;

		int subtract = my_count - retain;
		if ( subtract > 0 )
		    (void)remove(tp->value,subtract);
	    }
	    ap = ai.next();
	}
    }
    return *this;
}

template <class T>
Bag<T>&
Bag<T>::operator^=(const Bag<T>& b)
{
    if ( this != &b ) {
	Bag_bucketiter_ATTLC<T> a_it(*this);
	Bag_bucketiter_ATTLC<T> b_it(b);
	const Bag_bucket_ATTLC<T>* ap = a_it.first();
	const Bag_bucket_ATTLC<T>* bp = b_it.first();
	while ( ap && bp ) {
	    if ( ap->hashval == bp->hashval ) {
		//  The two hash values are equal; this means the two
		//  collision lists may contain equal values (which must
		//  be weeded out)

		Listiter< Bag_pair<T> >
		lia(((Bag_bucket_ATTLC<T>*)ap)->collision_list);

		Listiter< Bag_pair<T> >
		lib(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

		lib.reset();

		while ( !lib.at_end() ) {
		    Bag_pair<T>* btp;
		    Bag_pair<T>* atp;
		    lib.next(btp);
		    int found=0;

		    //  Guard against the case where removing from this list
		    //  causes the list to disappear

		    if ( lia.the_list() ) {
			lia.reset();
			while ( !lia.at_end() ) {
			    lia.next(atp);
			    if ( atp->value == btp->value ) {
				found=1;
				break;
			    }
			}
		    }
		    if ( found ) {
			int a_count = atp->count;
			int b_count = btp->count;
			int new_a_count = a_count - b_count;
			if ( new_a_count<0 )
			    new_a_count = - new_a_count;

			if ( new_a_count < a_count )
			    remove(atp->value, a_count - new_a_count);
			else {
			    // new_a_count >= a_count

			    insert(atp->value, new_a_count - a_count);
			}
		    }
		    else
			insert(btp->value,btp->count);
		}
		ap = a_it.next();
		bp = b_it.next();
	    }
	    else if ( BAG_LT_ATTLC(ap->hashval, bp->hashval) ) {
		ap = a_it.next();
	    }
	    else {
		Listiter< Bag_pair<T> >
		lib(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

		while ( !lib.at_end() ) {
		    Bag_pair<T>* btp;
		    lib.next(btp);
		    insert(btp->value,btp->count);
		}
		bp = b_it.next();
	    }
	}
	while ( bp ) {
	    Listiter< Bag_pair<T> >
	    lib(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

	    while ( !lib.at_end() ) {
		Bag_pair<T>* btp;
		lib.next(btp);
		insert(btp->value,btp->count);
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
Bag<T>::warn_iterators() const
{
    Bag_bucketiter_ATTLC<T>* it;
    for ( it = iter_head;it;it = it->next_it )
	it->clobber();
}

template <class T>
void
Bag<T>::histogram(Map<Set_or_Bag_hashval,unsigned>& m) const
{
    //  Iterate over Bag_buckets, creating a Map from
    //  bp->hashval to bp->collision_list.length().

    Bag_bucketiter_ATTLC<T> bi(*this);
    const Bag_bucket_ATTLC<T>* bp = bi.first();
    m = Map<Set_or_Bag_hashval,unsigned>();

    while ( bp ) {
	m[bp->hashval] = bp->collision_list.length();
	bp = bi.next();
    }
}

template <class T>
void
Bag<T>::check() const
{
    Bag_bucketiter_ATTLC<T> bi(*this);
    const Bag_bucket_ATTLC<T>* bp = bi.first();
#ifdef DEBUG_ATTLC
    Set_or_Bag_hashval oldhashval=0;
#endif
    int first=1;

    while ( bp ) {
	//  Bag_buckets must be stored in increasing order of
	//  hash value

	if ( first )
	    first=0;
	else {
#ifdef DEBUG_ATTLC
	    assert(BAG_LT_ATTLC(oldhashval,bp->hashval));
#endif
	}
#ifdef DEBUG_ATTLC
	oldhashval = bp->hashval;
#endif

	//  Collision lists may not be empty

#ifdef DEBUG_ATTLC
	assert(bp->collision_list.length()>0);
#endif

	//  Collision lists may not contain duplicates and
	//  counts must be greater than or equal to 1

	Listiter< Bag_pair<T> > it1(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

	while ( !it1.at_end() ) {
	    Bag_pair<T>* p1;
	    it1.next(p1);
#ifdef DEBUG_ATTLC
	    assert(p1->count>=1);
#endif
	    Listiter< Bag_pair<T> > it2(it1);

	    while ( !it2.at_end() ) {
		Bag_pair<T>* p2;
		it2.next(p2);
#ifdef DEBUG_ATTLC
		assert(!(p1->value == p2->value));
#endif
	    }
	}
	bp = bi.next();
    }
}

template <class T>
Bag<T>::~Bag()
{
    //  TBD_study this

/*
    for ( Bag_bucketiter_ATTLC<T>* it = iter_head;it;it = it->next_it ) {
	//  Break const

	Bag<T>// cheat = (Bag<T>//)&it->my_bag;
	*cheat = 0;
    }
*/
    remove_all();
}

template <class T>
Bag<T>::Bag(const Bag<T>& b): sze(0),sze_unique(0),iter_head(0)
{
    pos.curr_depth = -1;
    contents.make_null();
    Bag_bucketiter_ATTLC<T> bi(b);
    const Bag_bucket_ATTLC<T>* bp = bi.first();

    while ( bp ) {
	Listiter< Bag_pair<T> > li(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

	while ( !li.at_end() ) {
	    Bag_pair<T>* tp;
	    li.next(tp);
	    insert(tp->value,tp->count);
	}
	bp = bi.next();
    }
}

template <class T>
void
Bag<T>::make_setlogic(const Bag<T>& a,const Bag<T>& b, int l_type)
{
    // l_type = 0 (intersection), 1 (union), 2 (set difference), 3 (xor)

#ifdef DEBUG_ATTLC
    assert(sze == 0);
#endif
    Bag_bucketiter_ATTLC<T> a_it(a), b_it(b);
    const Bag_bucket_ATTLC<T>* ap = a_it.first(), *bp = b_it.first();
    Bag_pair<T> *atp, *btp;

    while ( ap && bp ) {
	Listiter< Bag_pair<T> > lia(((Bag_bucket_ATTLC<T>*)ap)->collision_list);
	Listiter< Bag_pair<T> > lib(((Bag_bucket_ATTLC<T>*)bp)->collision_list);

	if ( ap->hashval == bp->hashval ) {
	    // insert all of the necessary items from B

	    if ( l_type == 1 ) {
		while ( !lib.at_end() ) {
		    lib.next(btp);
		    insert(btp->value,btp->count);
		}
	    }
	    else if ( l_type == 3 ) {
		while ( !lib.at_end() ) {
		    lib.next(btp);
		    lia.reset();
		    int fcount=btp->count;
		    while ( !lia.at_end() ) {
			lia.next(atp);
			if ( atp->value == btp->value ) {
			    fcount=atp->count - btp->count;
			    if ( fcount < 0 )
				fcount = -fcount;

			    break;
			}
		    }
		    if ( fcount )
			insert(btp->value,fcount);
		}
	    }

	    // insert all of the necessary items from A

	    lia.reset();
	    while ( !lia.at_end() ) {
		lia.next(atp);
		if (l_type == 1 )
		    insert(atp->value,atp->count);
		else {
		    lib.reset();
		    int fcount;
		    if ( l_type == 0 )
			fcount = 0;
		    else
			fcount = atp->count;
		    while ( !lib.at_end() ) {
			lib.next(btp);
			if ( atp->value == btp->value ) {
			    switch(l_type) {
			    case 0:
				fcount=atp->count;
				if ( fcount>btp->count )
				    fcount=btp->count;
				break;
			    case 2:
			        fcount=atp->count-btp->count;
			        break;
			    case 3:
				fcount=0;
			    }
			    break;
			}
		    }
		    if (fcount>0 )
			insert(atp->value,fcount);
		}
	    }
	    ap = a_it.next();
	    bp = b_it.next();
	}
	else if ( SET_LT_ATTLC(ap->hashval,bp->hashval) ) {
	    // an entire bucket of values in A but not in B

	    if (l_type != 0 )
		while ( !lia.at_end() ) {
		    lia.next(atp);
		    insert(atp->value,atp->count);
		}

	    ap = a_it.next();
	}
	else {
	    // an entire bucket of values in B but not in A

	    if ( l_type == 1 || l_type == 3 )
		while ( !lib.at_end() ) {
		    lib.next(btp);
		    insert(btp->value,btp->count);
		}

	    bp = b_it.next();
	}
		
    }

    // Insert the leftovers

    if ( l_type != 0 ) {
	for ( ; ap ; ap = a_it.next() ) {
	    Listiter< Bag_pair<T> > lia(((Bag_bucket_ATTLC<T>*)ap)->collision_list);
	    while ( !lia.at_end() ) {
		lia.next(atp);
		insert(atp->value,atp->count);
	    }
	}
    }
    if ( l_type == 1 || l_type == 3 ) {
	for ( ; bp ; bp = b_it.next() ) {
	    Listiter< Bag_pair<T> > lib(((Bag_bucket_ATTLC<T>*)bp)->collision_list);
	    while ( !lib.at_end() ) {
		lib.next(btp);
		insert(btp->value,btp->count);
	    }
	}
    }
}

#include <iostream.h>

template <class T>
ostream&
Bag<T>::print(ostream& os) const
{
    Bag_bucketiter_ATTLC<T> bi(*this);
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

#endif
