/*ident	"@(#)Regex:demos/final-ex.c	3.1" */
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

#include <Regex.h>
#include <stream.h>

// A match on r just produced subs.  Display the info in subs.
//
void show_subs(const Regex &r, const Subex &subs) {
        for (int i = 1; i <= Regex::max_num_subexes; i++) {
            String s;
            Substrinfo ss = subs(i, s);
            if (ss) {
                String subex;
                r.subex(i, subex);
                cout << "subexpression " << subex;
                cout << " matched target substring \"";
                cout << s << "\" at " << ss.i << endl;
            }
        }
}

void show_all_matches(const Regex &r, const char *target) {
        Regexiter i(r, target);
        Subex subs;
        String mss;
        Substrinfo ss;
        while (ss = i.next(subs, mss)) {
            cout << "pattern matches substring \"" << mss;
            cout << "\" at " << ss.i << endl;
            show_subs(r, subs);
            cout << endl;
        }
}

main() {
    Regex r("(foo)((bar)*)|(baz)");
    show_all_matches(r, "foobarbazfoobaz");
    return 0;
}
