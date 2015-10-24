/*ident	"@(#)Stopwatch:demos/3.c	3.1" */
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

#include <Stopwatch.h>
#include <stream.h>

main() {
    Stopwatch w;
    w.start();
    // computation of interest
    // for (int i=0; i < 20000000; i++);
    w.stop();
    cout << "user time = " << w.user() << endl;
    // cout << "real time = " << w.real() << endl;
    return 0;
}
