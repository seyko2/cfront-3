/*ident	"@(#)Time:demos/1-2.c	3.1" */
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

#include <Time.h>
#include <stream.h>

main(){
        Time t1 = Time(1987,Time::july,3) + Duration(0,21);
        Time t2 = Time(1987,Time::july,4) + Duration(0,3);

        Duration d=t1-t2;

        cout << t1 << endl;
        cout << t2 << endl;
        cout << d << endl;
	return 0;
}
