/*ident	"@(#)Path:demos/wild.c	3.1" */
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
#include <List.h>
#include <stream.h>

main() {
    Path p("~/*.c");
    p.expand_tilde();             // "/my/home/directory/ *.c"
    List<Path> mydotcs;
    p.expand_wildcards(mydotcs);  
              // l is "( /my/home/directory/foo.c /my/home/directory/bar.c )"
    // cout << p << " ==> " << mydotcs << endl;
    cout << p << " ==> ";
    cout << mydotcs << endl;
    return 0;
}
