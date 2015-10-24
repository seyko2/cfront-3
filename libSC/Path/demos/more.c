/*ident	"@(#)Path:demos/more.c	3.1" */
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

#include <Search_path.h>
#include <stream.h>

main() {
    Path p("cat");
    Path cat;
    if (PATH.find(p, cat))
        cout << "cat is " << cat << endl;
    else
        cout << "cat not found" << endl;
    return 0;
}
