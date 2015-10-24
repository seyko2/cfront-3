/*ident	"@(#)G2++:compsrc/demos/test2.c	3.1" */
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

#include <stream.h>
#include "g2desc.h"
#include "test1.h"

main() {
	TEST1 t;
	t.one = 1;

	t.two[0].sub1 = 2;
	t.two[0].sub2 = 3;

	cout << t;

	return 0;
}
