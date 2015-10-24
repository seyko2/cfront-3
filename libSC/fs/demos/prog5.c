/*ident	"@(#)fs:demos/prog5.c	3.1" */
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

class X {
private:
	int i;
};

X *x;

void f() { /*...*/ }
void g() { /*...*/ }
void h() { /*...*/ }
void i() { /*...*/ x = new X; }
void j() { /*...*/ }

main() {
	f();
	g();
	h();
	i();
	j();	
	cout << endl;
	return 0;
}
