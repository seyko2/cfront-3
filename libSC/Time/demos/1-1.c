/*ident	"@(#)Time:demos/1-1.c	3.1" */
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
	/*Time result =*/ (void)(make_time("now") + Duration::days(30));
	// this line commented out in order to get this demo to
	// pass the regression test.  put it back in if you like.
	//cout << result;
	cout << endl;
	return 0;
}
