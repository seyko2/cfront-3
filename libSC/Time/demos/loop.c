/*ident	"@(#)Time:demos/loop.c	3.1" */
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
        // loop over all days in 1988 and 1989 
        // (doesn't raise Time::date_objection)

        for(
            int i=1;
            i<=days_in_year(1988)+days_in_year(1989);
            i++
        ){  
            Time t = Time::julian(1988,i);
            // ...
        }
	cout << endl;
	return 0;
}
