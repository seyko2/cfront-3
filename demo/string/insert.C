/*ident	"@(#) @(#)insert.c	1.1.1.2" */
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

	s1.insert(s1.length(), "def");
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcdef>\n";
	
	s1.insert(s1.length(), s2);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcdefabc>\n";

	s1.insert(0, s3, 1, 3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<bcdabcdefabc>\n";

	s1 = s2;
	s1.insert(0, s3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcdeabc>\n";

	s1 = s2;
	s1.insert(2, "abcjkl", 4);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<ababcjc>\n";

	s1 = s2;
	s1.insert(1, 5, 'c');
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<acccccbc>\n";

	s1 = s2;
	s1.insert(&s1[1], 'j');
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<ajbc>\n";

	s1 = s2;
	s1.insert(&s1[1], 3, 'j');
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<ajjjbc>\n";

	return (0);
}
