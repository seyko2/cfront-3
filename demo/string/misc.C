/*ident	"@(#) @(#)misc.c	1.1.1.2" */
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

void g(const string&);
void h(const string&);
void k(const string&);

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

	cout << "s3.length()=" << s3.length() << "\n";
	cout << "s3.length()=5\n";
	cout << "s3.size()=" << s3.size() << "\n";
	cout << "s3.size()=5\n";
	cout << "s3.max_size()=" << s3.max_size() << "\n";
	cout << "s3.max_size()=8\n";
	cout << "s3.capacity()=" << s3.capacity() << "\n";
	cout << "s3.capacity()=8\n";

	s2.swap(s3);
	cout << "s2=<" << s2 << ">\n";
	cout << "s2=<abcde>\n";
	cout << "s3=<" << s3 << ">\n";
	cout << "s3=<abc>\n";
	
	char buf[30];
	s2.copy(buf, 4);
	buf[4] = '\0';
	cout << "buf=<" << buf << ">\n";
	cout << "buf=<abcd>\n";

	s2.copy(buf, 4, 2);
	buf[4] = '\0';
	cout << "buf=<" << buf << ">\n";
	cout << "buf=<cded>\n";

	cout << "s3.c_str()=<" << s3.c_str() << ">\n";
	cout << "s3.c_str()=<abc>\n";
	
	cout << "s2.data()=<" << s2.data() << ">\n";
	cout << "s2.data()=<abcde>\n";
	cout << "s2.capacity()=" << s2.capacity() << "\n";
	cout << "s2.capacity()=8\n";

	s2.reserve(30);
	cout << "s2.data()=<" << s2.data() << ">\n";
	cout << "s2.data()=<abcde>\n";
	cout << "s2.capacity()=" << s2.capacity() << "\n";
	cout << "s2.capacity()=64\n";

	int val = s2.empty();
	cout << "s2.empty()=" << val << "\n";
	cout << "s2.empty()=0\n";

	s2.resize(0);
	cout << "s2.c_str()=<" << s2.c_str() << ">\n";
	cout << "s2.c_str()=<>\n";

	val = s2.empty();
	cout << "s2.empty()=" << val << "\n";
	cout << "s2.empty()=1\n";

	s2.resize(5, 'a');
	cout << "s2.data()=<" << s2.data() << ">\n";
	cout << "s2.data()=<aaaaa>\n";

	s2.resize(9, 'b');
	cout << "s2.data()=<" << s2.data() << ">\n";
	cout << "s2.data()=<aaaaabbbb>\n";

	g(s2);
	
	char *q = s2.begin();
	*q = 'x';
	cout << "s2=<" << s2 << ">\n";
	cout << "s2=<xaaaabbbb>\n";
	
	h(s2);
	
	q = s2.end();
	q--;
	*q = 'x';
	cout << "s2=<" << s2 << ">\n";
	cout << "s2=<xaaaabbbx>\n";

	k(s2);
	
	cout << "s2[2]=" << s2[2] << "\n";
	cout << "s2[2]=a\n";
	cout << "s2.at(2)=" << s2.at(2) << "\n";
	cout << "s2.at(2)=a\n";
	s2[2]='c';
	cout << "s2=<" << s2 << ">\n";
	cout << "s2=<xacaabbbx>\n";
	s2.at(2)='d';
	cout << "s2=<" << s2 << ">\n";
	cout << "s2=<xadaabbbx>\n";
	
	cout << "s2.substr(1,5)=<" << s2.substr(1,5) << ">\n";
	cout << "s2.substr(1,5)=<adaab>\n";

	cout << "s2.substr(5)=<" << s2.substr(5) << ">\n";
	cout << "s2.substr(5)=<bbbx>\n";

	return (0);
}
void g(const string& s2) {
	const char *p = s2.begin();
	cout << "p points to " << *p << "\n";
	cout << "p points to a\n";
}
void h(const string& s2) {
	const char *p = s2.end();
	p--;
	cout << "p points to " << *p << "\n";
	cout << "p points to b\n";
}
void k(const string& s2) {
	cout << "s2[2]=" << s2[2] << "\n";
	cout << "s2[2]=a\n";
	cout << "s2.at(2)=" << s2.at(2) << "\n";
	cout << "s2.at(2)=a\n";
}
