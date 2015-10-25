/*ident	"@(#) @(#)assign.c	1.1.1.2" */
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
#include <iostream.h>

#if defined(__edg_lucent_41)
using namespace std;
#endif

main() {
	string s1;
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<>\n";
	string s2 = "abc";
	cout << "s2=<" << s2 << ">\n";
	cout << "s2=<abc>\n";
	string s3("abcdefghi", 5);
	cout << "s3=<" << s3 << ">\n";
	cout << "s3=<abcde>\n";

	s1 = s2;
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abc>\n";

	s1 = "def";
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<def>\n";
	
	s1 = 'z';
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<z>\n";

	s1.assign(s3, 1, 3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<bcd>\n";

	s1.assign(s3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcde>\n";

	s1.assign("abcjkl", 4);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcj>\n";

	s1.assign("abcjkl");
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcjkl>\n";

	s1.assign(5, 'c');
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<ccccc>\n";

	return (0);
}
