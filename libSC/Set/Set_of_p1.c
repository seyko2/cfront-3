/*ident	"@(#)Set:Set_of_p1.c	3.1" */
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

#include "Set.h"
#include <assert.h>

pst_iter_old_ATTLC::pst_iter_old_ATTLC(const pst_ATTLC& ps)
	: my_pst((pst_ATTLC*)(&ps)), next_it(ps.iter_head)
{
	((pst_ATTLC*)&ps)->iter_head = this;
}

pst_iter_old_ATTLC::pst_iter_old_ATTLC(const pst_iter_old_ATTLC& pi)
	: my_pst(pi.my_pst), next_it(my_pst->iter_head)
{
	my_pst->iter_head = this;
}

pst_iter_old_ATTLC::~pst_iter_old_ATTLC()
{
	if (my_pst == 0) return;
	if (this == my_pst->iter_head)
		my_pst->iter_head = next_it;
	else {
		register pst_iter_old_ATTLC*	x;
		register pst_iter_old_ATTLC* an_it;
		for (an_it = my_pst->iter_head;
					(x = an_it->next_it) != this;
					an_it = x) ;
		an_it->next_it = next_it;
	}
}

pst_iter_old_ATTLC&
pst_iter_old_ATTLC::operator=(const pst_iter_old_ATTLC& pi)
{
	if (this == my_pst->iter_head) {
		my_pst->iter_head = next_it;
	} else {
		pst_iter_old_ATTLC* p = my_pst->iter_head;
		while (p->next_it != this) {
		    p = p->next_it;
		}
		p->next_it = next_it;
	}
	my_pst = pi.my_pst;
	next_it = my_pst->iter_head;
	my_pst->iter_head = this;
	return *this;
}

pst_ATTLC::pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_union_ATTLC*) :
			sze(0), iter_head(0)
{
	contents.make_null();
	make_union(a, b);
}

pst_ATTLC::pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_inter_ATTLC*) :
			sze(0), iter_head(0)
{
	contents.make_null();
	make_intersection(a, b);
}

pst_ATTLC::pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_diff_ATTLC*) :
			sze(0), iter_head(0)
{
	contents.make_null();
	make_difference(a, b);
}

pst_ATTLC::pst_ATTLC(const pst_ATTLC& a, const pst_ATTLC& b, Set_of_p_xor_ATTLC*) :
			sze(0), iter_head(0)
{
	contents.make_null();
	make_xor(a, b);
}

