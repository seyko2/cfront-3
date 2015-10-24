/*ident	"@(#)Time:demos/paranoid.c	3.1" */
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

Time paranoid(unsigned y,Time::Month m,unsigned d){
	Time result;
	if(Time::valid_date(y,m,d)){
	    result = Time(y,m,d);
	}
	return result;
}

main() {
	Time t;
	cout << t << endl;
	cout << paranoid(1988,Time::january,14) << endl; 
	cout << paranoid(0,Time::january,72) << endl;
	return 0;
}
