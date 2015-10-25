/*ident	"@(#) @(#)append.c	1.1.1.2" */
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

#include <iostream.h>
#include <string>

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

	s1 += "def";
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcdef>\n";
	
	s1 += s2;
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcdefabc>\n";

	s1.append(s3, 1, 3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcdefabcbcd>\n";

	s1 = s2;
	s1.append(s3);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcabcde>\n";

	s1 = s2;
	s1.append("abcjkl", 4);
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcabcj>\n";

	s1.append("xyz");
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcabcjxyz>\n";

	s1 = s2;
	s1.append(5, 'c');
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcccccc>\n";

	s1 = s2;
	s1 += 'j';
	cout << "s1=<" << s1 << ">\n";
	cout << "s1=<abcj>\n";

	return (0);
}
