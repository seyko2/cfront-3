/*ident	"@(#)String:demos/reverse.c	3.1" */
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

#include <String.h>
#include <stream.h>

String
reverse(String s) {
	String rslt;

	for(int i=length(s)-1;i>=0;i--)
		rslt += s.char_at(i);

	return rslt;
}

main() {
	cout << reverse("foobar") << endl;
	return 0;
}
