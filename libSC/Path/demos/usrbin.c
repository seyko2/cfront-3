/*ident	"@(#)Path:demos/usrbin.c	3.2" */
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
#include <stdlib.h>

main() {
    Path usrbin("/usr/bin");
    Listiter<Path> it(PATH);
    Path itp;
    while (!it.at_end()) {
        if ((itp = *(it.peek_next())) == usrbin) {
	    cout << itp << ": removed" << endl;
            it.remove_next();
        }
	else {
            it.next();
	}
    }
    String s = "PATH=";
    s += (String)PATH;
    putenv((char*)(const char*)s);
    return 0;
}
