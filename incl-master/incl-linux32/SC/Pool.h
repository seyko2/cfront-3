/*ident	"@(#)Pool:incl/Pool.h	3.1" */
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

#ifndef _POOL_H
#define _POOL_H 1

#ifndef __STDDEF_H
#include <stddef.h>
#endif

class Block_header_ATTLC {
	Block_header_ATTLC* next;
	friend class Block_pool_ATTLC;
};

class Block_pool_ATTLC {
	size_t sz;
	unsigned cnt;
	Block_header_ATTLC* head;
	Block_pool_ATTLC(const Block_pool_ATTLC&);	// copy protection
	void operator= (const Block_pool_ATTLC&);	// copy protection
public:
	size_t size() const { return sz; }
	unsigned count() const { return cnt; }
	Block_pool_ATTLC(size_t);
	~Block_pool_ATTLC();
	char *expand();
	void free();
};

class Pool_element_header {
	Pool_element_header *next;
	friend class Pool;
};

class Pool: private Block_pool_ATTLC {
	size_t elsize;
	unsigned count;
	Pool_element_header *head;
	Pool(const Pool&);		// copy protection
	void operator= (Pool&);		// copy protection
	void grow();
public:
	Pool(size_t n);
	void *alloc() {
		if (!head)
			grow();
		register Pool_element_header *p = head;
		head = p->next;
		return p;
	}
	void free(void *b) {
		register Pool_element_header *p = (Pool_element_header*) b;
		p->next = head;
		head = p;
	}
	void purge() {
		Block_pool_ATTLC::free();
		head = 0;
	}
};

struct Vpool_stats {
	unsigned count;     // number of elements currently in the Vpool
	double mem_util;    // current memory utilization
	size_t mem_alloced; // total memory currently allocated to Vpool
	size_t mem_inuse;   // bytes currently holding elements
	size_t mem_waste;   // bytes wasted due to fragmentation
	size_t mem_savings; // bytes saved due to reallocing
	size_t chunk_size;  // underlying memory chunk size
	size_t realsz;      // underlying element size (>= max size)
};

class Vpool: private Block_pool_ATTLC {
	unsigned cnt;
	size_t origsz, realsz;
	size_t space_left, totwaste;
	char *avail, *shrinkable;
	char *prev_avail;
	size_t prev_space_left;

	Vpool(const Vpool&);			// copy protection
	void operator=(const Vpool&);		// copy protection
	void grow();
	void do_shrink(void *, size_t);
	int legal_realloc(void *, size_t);
public:
	Vpool(size_t max, size_t expected, double desired_occupancy = 0.8);
	void *alloc();
	void *realloc(void *, size_t);
	void *realloc_inplace(void *, size_t);
	void purge();
	Vpool_stats stats() const;
};

#endif
