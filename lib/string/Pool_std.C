/*ident	"@(#) @(#)Pool_std.c	1.1.1.2" */
/******************************************************************************
*
* C++ Standard Library
*
* Copyright (c) 1996  Lucent Technologies.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Lucent Technologies.
* The copyright notice above does not evidence any actual or
* intended publication of such source code.
*
******************************************************************************/

#include <Pool_std.h>
#include <memory.h>
#include <assert.h>

#ifdef NAMESPACES_LUCENT
namespace std {
#endif

static const int minblocksz = 1000;
//static const int maxblocksz = 10000;

// most stringent alignment, in chars
// must be a power of 2 for this code to work
static const int round = 8;	

// offset in chars of the data part of a block
static const int blockoff = (sizeof(Block_header_std_LUCENT)+round-1) & (size_t)-round;

// rounds up n to the nearest multiple of round, except
// for 0, which gets rounded up to round
static size_t 
roundup(size_t n)
{
	return ((n + (n == 0) + round - 1) & (size_t)-round);
}

Block_pool_std_LUCENT::Block_pool_std_LUCENT (size_t n) : cnt(0), sz(n), head(0)
{
}

Block_pool_std_LUCENT::~Block_pool_std_LUCENT()
{
	free();
}

void 
Block_pool_std_LUCENT::free()
{
	while (head) {
		Block_header_std_LUCENT* b = head;
		head = head->next;
		delete (char*) b;
		--cnt;
	}
	assert(cnt == 0);
	cnt = 0;
}

char*
Block_pool_std_LUCENT::expand()
{
	Block_header_std_LUCENT* b = (Block_header_std_LUCENT*) new char[size()+blockoff];
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

Pool_std::Pool_std(size_t n): Block_pool_std_LUCENT(ceil(minblocksz, (elsize = roundup(n))))
{
	count = Block_pool_std_LUCENT::size()/elsize;
	head = 0;
}

void
Pool_std::grow()
{
	register char* p = expand();
	register int n = count;

	while (--n >= 0) {
		register Pool_std_element_header* ph = (Pool_std_element_header*) p;
		ph->next = head;
		head = ph;
		p += elsize;
	}
}

#ifdef NAMESPACES_LUCENT
}
#endif
