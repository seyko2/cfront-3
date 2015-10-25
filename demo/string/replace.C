/*ident	"@(#) @(#)replace.c	1.1.1.2" */
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

	s1.replace(s1.length(), 0, "def");
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcdef>\n";
	
	s1.replace(0, 1, s2);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcbcdef>\n";

	s1.replace(0, 2, s3, 1, 3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<bcdcbcdef>\n";

	s1 = s2;
	s1.replace(1, 2, s3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<aabcde>\n";

	s1 = s2 + "xx";
	s1.replace(1, 2, "abcjkl", 4);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<aabcjxx>\n";

	s1.replace(2, 3, "", 0);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<aaxx>\n";

	s1 = s3;
	s1.replace(1, 2, 'c');
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<accde>\n";

	s1 = s2;
	s1.replace(&s1[1], &s1[2], s3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<aabcdec>\n";

	s1 = s2;
	s1.replace(&s1[1], &s1[2], "xyzlmn", 4);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<axyzlc>\n";

	s1 = s2;
	s1.replace(&s1[1], &s1[2], "xyz");
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<axyzc>\n";

	s1 = s2;
	s1.replace(&s1[1], s1.end(), 'j');
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<ajj>\n";

	return (0);
}
