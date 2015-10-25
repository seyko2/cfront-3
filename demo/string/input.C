/*ident	"@(#) @(#)input.c	1.1.1.2" */
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
#include <strstream.h>

#if defined(__edg_lucent_41)
using namespace std;
#endif

main() {
	char *str1 = "this is a test\nmore";
	char *str2 = "\t\ttest\n\tTEST";
	string s;
	
	istrstream is1(str1);
	is1 >> s;
	printf("%s\n", s.c_str());
	printf("this\n");

	istrstream is2(str2);
	is2 >> s;
	printf("%s\n", s.c_str());
	printf("test\n");
	is2 >> s;
	printf("%s\n", s.c_str());
	printf("TEST\n");
	
	istrstream is3(str2);
	is3.width(4);
	is3 >> s;
	printf("%s\n", s.c_str());
	printf("tes\n");

	istrstream is4(str1);
	getline(is4, s);
	printf("%s\n", s.c_str());
	printf("this is a test\n");
	getline(is4, s);
	printf("%s\n", s.c_str());
	printf("more\n");

	return (0);
}
