/*ident	"@(#)Map:Map.c	3.1" */
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

#ifndef _MAP_C_
#define _MAP_C_

#include <Map.h>

Mapnodebase_ATTLC::~Mapnodebase_ATTLC() { 
	delete L_; 
	delete R_; 
}

static char minus1 = -1;

#ifdef __GNUG__
void
#else
inline void
#endif
Mapbase_ATTLC::rot_dir (Mapnodebase_ATTLC* p, char bal)
{
	if (bal == minus1) rotL(p);
	else rotR(p);
}
void
Mapbase_ATTLC::remove_deadwood() {
	Mapnodebase_ATTLC *p = successor_(0);
	while (p != 0) {
		if (p->remove_mark != 0) {
			Mapnodebase_ATTLC *q = p;
			p = successor_(p);
			removenode(q);
			delete q;
		}
		else {
			p = successor_(p);
		}
	}
}

void
Mapbase_ATTLC::make_empty() {
	delete head_;
	head_ = 0;
	n = 0;
}

void
Mapbase_ATTLC::rotL (Mapnodebase_ATTLC* p)
{
	Mapnodebase_ATTLC* q = p->R_;
	Mapnodebase_ATTLC* u = p->U_;
	p->attach (p->R_, q->L_);
	q->attach (q->L_, p);
	if (!u)
		head_ = q;
	else if (u->L_ == p)
		u->L_ = q;
	else
		u->R_ = q;
	q->U_ = u;
}

void
Mapbase_ATTLC::rotR (Mapnodebase_ATTLC* p)
{
	Mapnodebase_ATTLC* q = p->L_;
	Mapnodebase_ATTLC* u = p->U_;
	p->attach (p->L_, q->R_);
	q->attach (q->R_, p);
	if (!u)
		head_ = q;
	else if (u->R_ == p)
		u->R_ = q;
	else
		u->L_ = q;
	q->U_ = u;
}

void
Mapbase_ATTLC::newnode(Mapnodebase_ATTLC* ptr){

	/* rebalance */

	/*
	 * phase 1 - run up the tree, looking for an unbalanced
	 * node and unbalancing all the balanced nodes we traverse
	 */
	Mapnodebase_ATTLC* p = ptr->U_;
	Mapnodebase_ATTLC* q = ptr;

	while (p && p->bal == 0) {
		p->bal = (char) ((p->L_ == q) ? -1 : +1);
		q = p;
		p = p->U_;
	}

	/*
	 * p is now 0 or points at an unbalanced node.  If 0, we're done.
	 * Otherwise calculate the new balance factor for p.
	 */
	if (p) {
		if (p->bal == (char) ((p->L_ == q) ? +1 : -1))
			p->bal = 0;
		else {
			/* we have become truly unbalanced */
			char bbal = (char)-(p->bal);
			if (q->bal == p->bal) {
				rot_dir (p, bbal);
				p->bal = 0;
				q->bal = 0;
			} else {
				Mapnodebase_ATTLC* r = 
					(p->bal == +1) ? q->L_ : q->R_;
				rot_dir (q, p->bal);
				rot_dir (p, bbal);
				if (r->bal == bbal) {
					q->bal = p->bal;
					p->bal = 0;
				}
				else if (r->bal == p->bal) {
					p->bal = bbal;
					q->bal = 0;
				}
				else { /* r->bal == 0 */
					p->bal = 0;
					q->bal = 0;
				}
				r->bal = 0;
			}
		}
	}
}

/*
 *  Mapbase_ATTLC::removenode() - this function is a helper function for
 *  the function Mapnode_ATTLC(S,T)::remove().  It takes one argument --
 *  a pointer to the node to be deleted.  The function removes
 *  the node from the tree, but it doesn't reclaim the space
 *  that the node uses (that is left to Mapnode_ATTLC(S,T)::remove().
 *
 *  This function assumes that the tree is balanced, and it
 *  rebalances the tree after the deletion.
 */
void
Mapbase_ATTLC::removenode(Mapnodebase_ATTLC *p) {
	Mapnodebase_ATTLC *q = p->U_;
	Mapnodebase_ATTLC *r;
	if (p->R_ == 0 || p->L_ == 0) {
		r = (p->R_ == 0) ? p->L_ : p->R_;
		if (q == 0) {
			head_ = r;
		}
		else {
			del_balance(p);
			if (q->L_ == p) {
				q->L_ = r;
			}
			else {
				q->R_ = r;
			}
		}
		if (r != 0) {
			r->U_ = q;
		}
	}
	else {
		/*
		 * We're trying to delete an internal node.
		 * The best way to do this is to delete the
		 * successor node, then replace the internal
		 * node with the deleted node (see Knuth Vol. 3,
		 * p. 428).
		 */
		Mapnodebase_ATTLC *psucc = p->R_;
		while (psucc->L_ != 0) {
			psucc = psucc->L_;
		}
		removenode(psucc);
		psucc->attach(psucc->L_, p->L_);
		psucc->attach(psucc->R_, p->R_);
		psucc->U_ = p->U_;
		psucc->bal = p->bal;

		q = psucc->U_;
		if (q == 0) {
			head_ = psucc;
		}
		else {
			if (q->L_ == p) {
				q->L_ = psucc;
			}
			else {
				q->R_ = psucc;
			}
		}
	}
	p->L_ = 0;
	p->R_ = 0;
}


/*
 *  Mapbase_ATTLC::del_balance() -- this function is a helper function
 *  for Mapbase_ATTLC::remove().  It takes a pointer "p" to the top node
 *  of a subtree that is known to have been reduced in height by
 *  a deletion operation.  The subtree pointed to by "p" is assumed
 *  to be balanced.
 *
 *  The function proceeds to balance the entire tree of which the
 *  given subtree is a part.  It walks up the tree so long as
 *  it is making the subtree pointed to by "q" (the parent of "p")
 *  shorter.
 *
 *  See Knuth, Vol. 3, pp. 465-466 for the details.
 */
void
Mapbase_ATTLC::del_balance(Mapnodebase_ATTLC *p) {
	Mapnodebase_ATTLC *q, *r, *x;
	char a;
	while (1) {
		q = p->U_;
		if (q == 0) break;
		
		a = (q->L_ == p) ? -1 : +1;
		if (q->bal == 0) {
			q->bal = -a;
			break;
		}
		if (q->bal == a) {
			q->bal = 0;
/*			break; */
		}
		else {
			r = (q->bal == +1) ? q->R_ : q->L_;
			if (r->bal == q->bal) {
				/* case 1: single rotation */
				rot_dir(q, (char)-(q->bal));
				r->bal = 0;
				q->bal = 0;
				q = r;
			}
			else if (r->bal == (char)-(q->bal)) {
				/* case 2: double rotation */
				x = (q->bal == +1) ? r->L_ : r->R_;
				rot_dir(r, q->bal);
				rot_dir(q, (char)-(q->bal));
				if (x->bal == 0) {
					q->bal = 0;
					r->bal = 0;
				}
				else if (x->bal == q->bal) {
					r->bal = 0;
					q->bal = -(q->bal);
				}
				else {
					q->bal = 0;
					r->bal = -(r->bal);
				}
				x->bal = 0;
				q = x;
			}
			else {
				/* case 3: single rotation and quit */
				rot_dir(q, (char)-(q->bal));
				r->bal = -(q->bal);
				break;
			}
		}
		p = q;
	}
}

Mapnodebase_ATTLC *
Mapbase_ATTLC::successor_(Mapnodebase_ATTLC *p) const {
	if (p) {
		if (p->R_)
			p = p->R_;
		else {
			while (p) {
				Mapnodebase_ATTLC* q = p;
				p = p->U_;
				if (p && p->L_ == q)
					return p;
			}
		}
	} else
		p = head_;

	/* p = low(p) */
	if (p)
		while (p->L_)
			p = p->L_;

	return p;
}
Mapnodebase_ATTLC *
Mapbase_ATTLC::predecessor_(Mapnodebase_ATTLC *p) const {
	if (p) {
		if (p->L_)
			p = p->L_;
		else {
			while (p) {
				Mapnodebase_ATTLC* q = p;
				p = p->U_;
				if (p && p->R_ == q)
					return p;
			}
		}
	} else
		p = head_;

	/* p = low(p) */
	if (p)
		while (p->R_)
			p = p->R_;

	return p;
}

#endif
