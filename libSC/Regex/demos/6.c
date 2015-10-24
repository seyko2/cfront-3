/*ident	"@(#)Regex:demos/6.c	3.1" */
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

main() {
    Regex r("(foo)|(oof)");
    Regexiter i(r, "foofoof", Regexiter::overlapping);
    Subex subs;
    while (i.next(subs)) {
        String s;
        Substrinfo ss = subs(2, s);
        if (ss) {
            cout << '\"' << s << "\" at " << ss.i;
            cout << " matched subexpression 2\n";
        }
    }
    return 0;
}
