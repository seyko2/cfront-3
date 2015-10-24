/*ident	"@(#)Path:kshtest.c	3.1" */
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

#include <ksh_test.h>

extern "C" {
	// defined in ./ksh/test.c
	int unop_test_Path_ATTLC(int, const char *, Ksh_test::id);
	int binop_test_Path_ATTLC(int, const char *, const char *, Ksh_test::id);
}

int ksh_test(const char *p, Ksh_test::id t) {
	return unop_test_Path_ATTLC(Ksh_test::a, p, t);
}

int ksh_test(Ksh_test::unary c, const char *p, Ksh_test::id t) {
	return unop_test_Path_ATTLC(c, p, t);
}

int ksh_test(const char *p, Ksh_test::binary c, const char *q, Ksh_test::id t) {
	return binop_test_Path_ATTLC(c, p, q, t);
}
