/*ident	"@(#)Path:demos/explode.c	3.1" */
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

#include <Path.h>
#include <stream.h>
#include <List.h>

main() {
    Path p("foo/bar/baz.c");
    List<Path> l;
    p.explode(l);           // l is "( foo bar baz.c )"
    l[(unsigned)1] = "luxury/yacht";  // l is "( foo luxury/yacht baz.c )"
    Path q(l);              // "foo/luxury/yacht/baz.c"
    cout << q << endl;
    return 0;
}
