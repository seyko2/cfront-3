/*ident	"@(#)Pool:Pool.c	3.1" */
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

#include "Poollib.h"

Block_pool_ATTLC::Block_pool_ATTLC (size_t n) : cnt(0), sz(n), head(0)
{
}

Block_pool_ATTLC::~Block_pool_ATTLC()
{
	free();
}

void 
Block_pool_ATTLC::free()
{
	while (head) {
		Block_header_ATTLC* b = head;
		head = head->next;
		delete (char*) b;
		--cnt;
	}
	assert(cnt == 0);
	cnt = 0;
}

char*
Block_pool_ATTLC::expand()
{
	Block_header_ATTLC* b = (Block_header_ATTLC*) new char[size()+blockoff];
	b->next = head;
	head = b;
	++cnt;
	return ((char*) b) + blockoff;
}


// largest multiple of q that is <= p
static size_t
floor(size_t p, size_t q)
{
	return p - p % (q + (q==0));
}

// smallest multiple of q that is >= p
static size_t
ceil(size_t p, size_t q)
{
	return floor (p + q - 1, q);
}

Pool::Pool(size_t n): Block_pool_ATTLC(ceil(minblocksz, roundup(n)))
{
        elsize = roundup(n);
	count = Block_pool_ATTLC::size()/elsize;
	head = 0;
}

void
Pool::grow()
{
	register char* p = expand();
	register int n = count;

	while (--n >= 0) {
		register Pool_element_header* ph = (Pool_element_header*) p;
		ph->next = head;
		head = ph;
		p += elsize;
	}
}
