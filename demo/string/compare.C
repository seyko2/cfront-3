/*ident	"@(#) @(#)compare.c	1.1.1.2" */
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
	string s3("abcde");
	cout << "s3=<" << s3 << ">\n";
	cout << "s3=<abcde>\n";
	string s4("abcde");
	cout << "s4=<" << s4 << ">\n";
	cout << "s4=<abcde>\n";
	string s5("bcdef");
	cout << "s5=<" << s5 << ">\n";
	cout << "s5=<bcdef>\n";

	cout << "s3.compare(s4)=" << s3.compare(s4) << "\n";
	cout << "s3.compare(s4)=0\n";

	cout << "(s1.compare(s2)<0)=" << (s1.compare(s2)<0) << "\n";
	cout << "(s1.compare(s2)<0)=1\n";

	cout << "(s2.compare(s3)<0)=" << (s2.compare(s3)<0) << "\n";
	cout << "(s2.compare(s3)<0)=1\n";

	cout << "(s2.compare(s3,1)>0)=" << (s2.compare(s3,1)>0) << "\n";
	cout << "(s2.compare(s3,1)>0)=1\n";

	cout << "(s4.compare(s5)<0)=" << (s4.compare(s5)<0) << "\n";
	cout << "(s4.compare(s5)<0)=1\n";


	cout << "s3.compare(\"abcde\", 0, 5)=" << s3.compare("abcde", 0, 5) << "\n";
	cout << "s3.compare(\"abcde\", 0, 5)=0\n";

	cout << "(s1.compare(\"xyz\")<0)=" << (s1.compare("xyz")<0) << "\n";
	cout << "(s1.compare(\"xyz\")<0)=1\n";

	cout << "(s2.compare(\"abcdefg\")<0)=" << (s2.compare("abcdefg")<0) << "\n";
	cout << "(s2.compare(\"abcdefg\")<0)=1\n";

	cout << "(s2.compare(\"abcde\",1)>0)=" << (s2.compare("abcde",1)>0) << "\n";
	cout << "(s2.compare(\"abcde\",1)>0)=1\n";

	cout << "(s4.compare(\"bcdef\")<0)=" << (s4.compare("bcdef")<0) << "\n";
	cout << "(s4.compare(\"bcdef\")<0)=1\n";


	cout << "(s2==s3)=" << (s2==s3) << "\n";
	cout << "(s2==s3)=0\n";
	cout << "(s2!=s3)=" << (s2!=s3) << "\n";
	cout << "(s2!=s3)=1\n";
	cout << "(s2<=s3)=" << (s2<=s3) << "\n";
	cout << "(s2<=s3)=1\n";
	cout << "(s2>=s3)=" << (s2>=s3) << "\n";
	cout << "(s2>=s3)=0\n";
	cout << "(s2<s3)=" << (s2<s3) << "\n";
	cout << "(s2<s3)=1\n";
	cout << "(s2>s3)=" << (s2>s3) << "\n";
	cout << "(s2>s3)=0\n";

	cout << "(s4==s3)=" << (s4==s3) << "\n";
	cout << "(s4==s3)=1\n";
	cout << "(s4!=s3)=" << (s4!=s3) << "\n";
	cout << "(s4!=s3)=0\n";
	cout << "(s4<=s3)=" << (s4<=s3) << "\n";
	cout << "(s4<=s3)=1\n";
	cout << "(s4>=s3)=" << (s4>=s3) << "\n";
	cout << "(s4>=s3)=1\n";
	cout << "(s4<s3)=" << (s4<s3) << "\n";
	cout << "(s4<s3)=0\n";
	cout << "(s4>s3)=" << (s4>s3) << "\n";
	cout << "(s4>s3)=0\n";

	cout << "(s2==\"abcde\")=" << (s2=="abcde") << "\n";
	cout << "(s2==\"abcde\")=0\n";
	cout << "(s2!=\"abcde\")=" << (s2!="abcde") << "\n";
	cout << "(s2!=\"abcde\")=1\n";
	cout << "(s2<=\"abcde\")=" << (s2<="abcde") << "\n";
	cout << "(s2<=\"abcde\")=1\n";
	cout << "(s2>=\"abcde\")=" << (s2>="abcde") << "\n";
	cout << "(s2>=\"abcde\")=0\n";
	cout << "(s2<\"abcde\")=" << (s2<"abcde") << "\n";
	cout << "(s2<\"abcde\")=1\n";
	cout << "(s2>\"abcde\")=" << (s2>"abcde") << "\n";
	cout << "(s2>\"abcde\")=0\n";

	cout << "(s4==\"abcde\")=" << (s4=="abcde") << "\n";
	cout << "(s4==\"abcde\")=1\n";
	cout << "(s4!=\"abcde\")=" << (s4!="abcde") << "\n";
	cout << "(s4!=\"abcde\")=0\n";
	cout << "(s4<=\"abcde\")=" << (s4<="abcde") << "\n";
	cout << "(s4<=\"abcde\")=1\n";
	cout << "(s4>=\"abcde\")=" << (s4>="abcde") << "\n";
	cout << "(s4>=\"abcde\")=1\n";
	cout << "(s4<\"abcde\")=" << (s4<"abcde") << "\n";
	cout << "(s4<\"abcde\")=0\n";
	cout << "(s4>\"abcde\")=" << (s4>"abcde") << "\n";
	cout << "(s4>\"abcde\")=0\n";

	return (0);
}
