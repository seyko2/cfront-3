/*ident	"@(#)Pool:demos/Thinglist.c	3.1" */
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

#include <Pool.h>

struct Thing {
};

struct Thinglist {
	Thing t;
	Thinglist* next;
	static Pool mypool;
	void* operator new(size_t) {
		return mypool.alloc();
	}
	void operator delete(void* p) {
		mypool.free (p);
	}
};

Pool Thinglist::mypool(sizeof(Thinglist));

main() {
	int N = 1000;
	Thinglist* head = 0;
	for (register int j = 0; j < N; j++) {
		Thinglist* tp = new Thinglist;
		tp->next = head;
		head = tp;
	}
	while (head) {
		Thinglist* tp = head->next;
		delete head;
		head = tp;
	}
	return 0;
}
