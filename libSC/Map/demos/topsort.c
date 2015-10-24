/*ident	"@(#)Map:demos/topsort.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#include "topsort.h"
#include <Map.h>
#include <stream.h>

main() {
	Map<String,token> m;
	String p, s;

	while (cin >> p >> s) {
		if (p == s)
			(void) m[p];
		else {
			m[s].predcnt++;
			m[p].succ.put(s);
		}
	}

	List<String> zeroes;

	for (Mapiter<String,token> i = m.first(); i; ++i) {
		if (i.value().predcnt == 0)
			zeroes.put(i.key());
	}

	int n = 0;
	while (zeroes.get (p)) {
		cout << p << "\n";
		n++;
		List<String>& t = m[p].succ;
		while (t.get (s)) {
			if (--m[s].predcnt == 0)
				zeroes.put (s);
		}
	}
	if (n != m.size())
		cout << "the ordering contains a loop\n";

	return 0;
}
