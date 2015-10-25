/*ident	"@(#) @(#)output.c	1.1.1.2" */
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

#include <string>
#include <stdio.h>
#include <iostream.h>

main() {
	char *str1 = "this is a test";
	string s = str1;
	
	cout << s << "\n";
	cout << "this is a test\n";

	s = "this";
	cout << "<";
	cout.width(10);
	cout << s << ">\n";
	cout << "<      this>\n";

	cout.setf(ios::left, ios::adjustfield);
	cout << "<";
	cout.width(10);
	cout << s << ">\n";
	cout << "<this      >\n";

	cout << "<";
	cout.width(3);
	cout << s << ">\n";
	cout << "<this>\n";

	return (0);
}
