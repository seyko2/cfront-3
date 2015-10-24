/*ident	"@(#)Pool:Vpool.c	3.1" */
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

static size_t 
blocksize(size_t maxsz, size_t expsz, double occ)
{
	if (occ < 0.1)
		occ = 0.1;
	else if (occ > 0.9)
		occ = 0.9;
	if (expsz < 1)
		expsz = 1;
	else if (expsz > maxsz)
		expsz = maxsz;

	// this blocksize (or bigger) will guarantee an occupancy ratio of occ
	// (assuming, of course, that the client's specification of expsz is in the ballpark)
	size_t bsz = (size_t)((maxsz - expsz*occ)/(1-occ));

	if (bsz < minblocksz)
		bsz = minblocksz;
	else if (bsz > maxblocksz)
	{
		// The maximum value of bsz occurs when expsz = 1 and occ = .9,
		// in which case bsz = 10*maxsz - 9.
//		Vpool::cant_guarantee_occupancy.raise();
		bsz = maxblocksz;
		if (bsz < maxsz)  // have to be able to fit at least one element per block
			bsz = maxsz;
	}
	return bsz;
}

Vpool::Vpool(size_t n, size_t expsz, double occ): 
origsz(n), realsz(roundup(n)), Block_pool_ATTLC(blocksize(roundup(n), expsz, occ))
{
	assert(origsz <= realsz);
	cnt = 0;
	avail = shrinkable = 0;
	totwaste = space_left = 0;
	prev_avail = 0;
	prev_space_left = 0;
}

int 
Vpool::legal_realloc(void *p, size_t n)
{
	return (p == shrinkable && shrinkable != 0 && n <= origsz);
}

void *
Vpool::realloc(void *p, size_t n)
{
	if (!legal_realloc(p, n))
		return 0;
	else
	{
		size_t _sz = roundup(n);
		assert(_sz <= realsz);
		if (prev_space_left >= _sz)  // then move *p to the end of the previous block
		{
			char *to = prev_avail;
			memcpy(to, p, _sz);
			prev_avail += _sz;
			prev_space_left -= _sz;
			totwaste -= _sz;
			do_shrink(p, 0);
			return to;
		}
		else  // just realloc it in place
		{
			do_shrink(p, _sz);
			return p;
		}
	}
}

void *
Vpool::realloc_inplace(void *p, size_t n)
{
	if (!legal_realloc(p, n))
		return 0;
	else
	{
		size_t _sz = roundup(n);
		assert(_sz <= realsz);
		do_shrink(p, _sz);
		return p;
	}
}

void 
Vpool::do_shrink(void *p, size_t _sz)
{
	space_left += (avail - (char*)p) - _sz;
	avail = (char*)p + _sz;
	shrinkable = 0;
}

void
Vpool::grow()
{
	totwaste += space_left;
	prev_space_left = space_left;
	prev_avail = avail;
	avail = expand();
	shrinkable = 0;
	space_left = Block_pool_ATTLC::size();
	assert(space_left >= realsz);
}

#if 0
double Vpool::memory_utilization() const
{
	size_t used = (Block_pool_ATTLC::count() * Block_pool_ATTLC::size()) - space_left;
	double ratio = ((double)used - (double)totwaste)/(double)used;
	return ratio;
}
#endif

Vpool_stats Vpool::stats() const
{
	Vpool_stats vs;

	vs.count = cnt;
	vs.chunk_size = Block_pool_ATTLC::size();
	vs.mem_alloced = Block_pool_ATTLC::count() * vs.chunk_size;
	size_t used = vs.mem_alloced - space_left;
	vs.mem_inuse = used - totwaste;
	vs.mem_waste = totwaste;
	vs.mem_savings = (cnt * realsz) - used;
	vs.mem_util = ((double)used - (double)totwaste)/(double)used;
	vs.realsz = realsz;

	return vs;
}

void *
Vpool::alloc() 
{
	if (space_left < realsz)
		grow();
	// assert(space_left >= realsz);
	register void * p = avail;
	shrinkable = avail;
	avail += realsz;
	space_left -= realsz;
	++cnt;
	return p;
}

void 
Vpool::purge()
{
	Block_pool_ATTLC::free();
	cnt = 0;
	totwaste = space_left = 0;
	avail = shrinkable = 0;
	prev_avail = 0;
	prev_space_left = 0;
}

