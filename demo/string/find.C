/*ident	"@(#) @(#)find.c	1.1.1.2" */
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
	string s1 = "the quick brown fox";
	string s2 = "jumped over the lazy dog";
	
	cout << "s1.find('q')=" << s1.find('q') << "\n";
	cout << "s1.find('q')=4" << "\n";
	cout << "s2.find('q')=" << (int) s2.find('q') << "\n";
	cout << "s2.find('q')=-1" << "\n";

	cout << "s1.find(\"quick\")=" << s1.find("quick") << "\n";
	cout << "s1.find(\"quick\")=4" << "\n";
	cout << "s2.find(\"quick\")=" << (int) s2.find("quick") << "\n";
	cout << "s2.find(\"quick\")=-1" << "\n";

	string s3 = "brown";
	cout << "s1.find(\"brown\")=" << s1.find(s3) << "\n";
	cout << "s1.find(\"brown\")=10" << "\n";
	cout << "s2.find(\"brown\")=" << (int) s2.find(s3) << "\n";
	cout << "s2.find(\"brown\")=-1" << "\n";

	cout << "s1.rfind('o')=" << s1.rfind('o') << "\n";
	cout << "s1.rfind('o')=17" << "\n";
	cout << "s2.rfind('o')=" << s2.rfind('o') << "\n";
	cout << "s2.rfind('o')=22" << "\n";
	

	cout << "s1.find_first_of(\"qrs\")="
	     << s1.find_first_of("qrs") << "\n";
	cout << "s1.find_first_of(\"qrs\")=4" << "\n";
	cout << "s2.find_first_of(\"qrs\")="
	     << s2.find_first_of("qrs") << "\n";
	cout << "s2.find_first_of(\"qrs\")=10" << "\n";

	cout << "s1.find_last_of(\"qrs\")="
	     << s1.find_last_of("qrs") << "\n";
	cout << "s1.find_last_of(\"qrs\")=11" << "\n";
	cout << "s2.find_last_of(\"qrs\")="
	     << s2.find_last_of("qrs") << "\n";
	cout << "s2.find_last_of(\"qrs\")=10" << "\n";

	string s4 = "qrs";

	cout << "s1.find_first_of(\"qrs\")="
	     << s1.find_first_of(s4) << "\n";
	cout << "s1.find_first_of(\"qrs\")=4" << "\n";
	cout << "s2.find_first_of(\"qrs\")="
	     << s2.find_first_of(s4) << "\n";
	cout << "s2.find_first_of(\"qrs\")=10" << "\n";

	cout << "s1.find_last_of(\"qrs\")="
	     << s1.find_last_of(s4) << "\n";
	cout << "s1.find_last_of(\"qrs\")=11" << "\n";
	cout << "s2.find_last_of(\"qrs\")="
	     << s2.find_last_of(s4) << "\n";
	cout << "s2.find_last_of(\"qrs\")=10" << "\n";


	cout << "s1.find_first_not_of(\"eth \")="
	     << s1.find_first_not_of("eth ") << "\n";
	cout << "s1.find_first_not_of(\"eth \")=4" << "\n";
	cout << "s2.find_first_not_of(\"eth \")="
	     << s2.find_first_not_of("eth ") << "\n";
	cout << "s2.find_first_not_of(\"eth \")=0" << "\n";

	cout << "s1.find_last_not_of(\"dog\")="
	     << s1.find_last_not_of("dog") << "\n";
	cout << "s1.find_last_not_of(\"dog\")=18" << "\n";
	cout << "s2.find_last_not_of(\"dog\")="
	     << s2.find_last_not_of("dog") << "\n";
	cout << "s2.find_last_not_of(\"dog\")=20" << "\n";

	string s5 = "eth ";

	cout << "s1.find_first_not_of(\"eth \")="
	     << s1.find_first_not_of(s5) << "\n";
	cout << "s1.find_first_not_of(\"eth \")=4" << "\n";
	cout << "s2.find_first_not_of(\"eth \")="
	     << s2.find_first_not_of(s5) << "\n";
	cout << "s2.find_first_not_of(\"eth \")=0" << "\n";

	string s6 = "dog";

	cout << "s1.find_last_not_of(\"dog\")="
	     << s1.find_last_not_of(s6) << "\n";
	cout << "s1.find_last_not_of(\"dog\")=18" << "\n";
	cout << "s2.find_last_not_of(\"dog\")="
	     << s2.find_last_not_of(s6) << "\n";
	cout << "s2.find_last_not_of(\"dog\")=20" << "\n";


	cout << "s1.find_first_of(\"\")="
	     << (int) s1.find_first_of("") << "\n";
	cout << "s1.find_first_of(\"\")=-1\n";
	cout << "s1.find_last_of(\"\")="
	     << (int) s1.find_last_of("") << "\n";
	cout << "s1.find_last_of(\"\")=-1\n";
	cout << "s1.find_first_not_of(\"\")="
	     << s1.find_first_not_of("") << "\n";
	cout << "s1.find_first_not_of(\"\")=0\n";
	cout << "s1.find_last_not_of(\"\")="
	     <<  s1.find_last_not_of("") << "\n";
	cout << "s1.find_last_not_of(\"\")=18\n";

	string s7 = "easy";

	cout << "s7.find_first_of(\"asye\")="
	     << (int) s7.find_first_of("asye") << "\n";
	cout << "s7.find_first_of(\"asye\")=0\n";
	cout << "s7.find_last_of(\"asye\")="
	     << (int) s7.find_last_of("asye") << "\n";
	cout << "s7.find_last_of(\"asye\")=3\n";
	cout << "s7.find_first_not_of(\"asye\")="
	     << (int) s7.find_first_not_of("asye") << "\n";
	cout << "s7.find_first_not_of(\"asye\")=-1\n";
	cout << "s7.find_last_not_of(\"asye\")="
	     <<  (int) s7.find_last_not_of("asye") << "\n";
	cout << "s7.find_last_not_of(\"asye\")=-1\n";

	return (0);
}

