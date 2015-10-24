/*ident	"@(#)fs:demos/prog3.c	3.1" */
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

#include "prog.h"

// X is a silly class that just remembers a single string.
//
class X {
	char *rep;
public:
	X() : rep(0) {}
	~X() { delete rep; }
        void set(const char *s) { 
		size_t l = strlen(s); 
		rep = new char[l+1]; 
		memcpy(rep, s, l+1); 
	}
        const char *get() const { 
		return rep; 
	}
};

void f() {
        X *x = new X;
        x->set("hello, world!");
	delete x;
}

main() {
	for (int i = 0; i < 4; i++) {
                f();
        }
	cout << endl;
	return 0;
}        

