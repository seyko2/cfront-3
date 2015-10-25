/*ident	"@(#) @(#)const.c	1.1.1.2" */
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
	string s4(1, 'a');
	cout << "s4=<" << s4 << ">\n";
	cout << "s4=<a>\n";
	string s5(5, 'a');
	cout << "s5=<" << s5 << ">\n";
	cout << "s5=<aaaaa>\n";
	string s6(s2);
	cout << "s6=<" << s6 << ">\n";
	cout << "s6=<abc>\n";
	string s7(s3, 2);
	cout << "s7=<" << s7 << ">\n";
	cout << "s7=<cde>\n";
	string s8(s3, 1, 3);
	cout << "s8=<" << s8 << ">\n";
	cout << "s8=<bcd>\n";
	string s9(s3, 1, string::npos);
	cout << "s9=<" << s9 << ">\n";
	cout << "s9=<bcde>\n";

	return (0);
}
