/*ident	"@(#)Path:wild.c	3.1" */
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

#include "Pathlib.h"
#include <List.h>

//ostream & operator<<(ostream & o, const Vpool_stats & v);

static Vpool *pool;
static List<Path> *lp;

static void poolcreate(size_t n, size_t exp) {
	pool = new Vpool(n, exp);
}

static void found(const char *p) {
//	cout << "callback: " << p << endl;
	lp->put(Path(p));
}

static char * alloc() {
	return (char*)pool->alloc();
}

static void shrink(char *p, size_t n) {
	pool->realloc_inplace(p, n);
}

void Path::expand_wildcards(List<Path> & ret) const {
	ret.make_empty();
	lp = &ret;
	
	/*int i =*/ path_expand_Path_ATTLC(rep, found, poolcreate, alloc, shrink);
//	cout << pool->stats() << endl;
	delete pool;

	// not really necessary, since put doesn't change current position
	// ret.reset();
}

#if 0
ostream & operator<<(ostream & o, const Vpool_stats & v) {
	o << "There are " << v.count << " elements in the pool, occupying ";
	o << v.mem_inuse << " bytes, and with a waste " << endl;
	o << "of " << v.mem_waste << " bytes, for a current memory utilization of ";
	o << v.mem_util << " and a total savings\nof ";
	o << v.mem_savings << " bytes achieved due to shrinking/reallocing." << endl;
	o << "(Underlying block size is " << v.chunk_size << ".)" << endl;
	o << "(Underlying element size is " << v.elsize << ".)" << endl;

	return o;
}	
#endif
