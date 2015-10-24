/*ident	"@(#)Regex:demos/giveme.c	3.1" */
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
    cout << "Give me a pattern, please: ";
    String s;
    cin >> s;
    Regex r(s);
    while (!r) {
        cout << "invalid: " << r.the_error() << endl;
        cout << "Please give me a valid pattern: ";
        cin >> s;
        r.assign(s);
    }
    cout << endl;
    return 0;
}
