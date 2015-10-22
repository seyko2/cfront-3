/*ident	"@(#) @(#)Pool_std.h	1.1.1.2" */
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

#ifndef STDSTRINGPOOLH 
#define STDSTRINGPOOLH

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#include "compiler_type.h"

#ifdef NAMESPACES_LUCENT
namespace std {
#endif

class Block_header_std_LUCENT {
	Block_header_std_LUCENT* next;
	friend class Block_pool_std_LUCENT;
};

class Block_pool_std_LUCENT {
	size_t sz;
	unsigned cnt;
	Block_header_std_LUCENT* head;
	Block_pool_std_LUCENT(const Block_pool_std_LUCENT&);	// copy protection
	void operator= (const Block_pool_std_LUCENT&);	// copy protection
public:
	size_t size() const { return sz; }
	unsigned count() const { return cnt; }
	Block_pool_std_LUCENT(size_t);
	~Block_pool_std_LUCENT();
	char *expand();
	void free();
};

class Pool_std_element_header {
	Pool_std_element_header *next;
	friend class Pool_std;
};

class Pool_std: private Block_pool_std_LUCENT {
	size_t elsize;
	unsigned count;
	Pool_std_element_header *head;
	Pool_std(const Pool_std&);		// copy protection
	void operator= (Pool_std&);		// copy protection
	void grow();
public:
	Pool_std(size_t n);
	void *alloc() {
		if (!head)
			grow();
		register Pool_std_element_header *p = head;
		head = p->next;
		return p;
	}
	void free(void *b) {
		register Pool_std_element_header *p = (Pool_std_element_header*) b;
		p->next = head;
		head = p;
	}
	void purge() {
		Block_pool_std_LUCENT::free();
		head = 0;
	}
};

#ifdef NAMESPACES_LUCENT
}
#endif

#endif
