/*ident	"@(#)Time:demos/daylight.c	3.1" */
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

#include "Time.h"
#include <stream.h>

Place dst_table[9];

inline Place& COOK(unsigned int y){
        return dst_table[y-1980];
}
main(){

    //  Set up dst_table:

        COOK(1980)=Place("KDT9:30KST10:00;63/5:00,308/20:00");
        COOK(1981)=Place("KDT9:30KST10:00;67/5:00,311/20:00");
        // ...
        COOK(1988)=Place("KDT9:30KST10:00;64/5:00,303/20:00");
    
    //  Time computations:

        unsigned int y = 1981;
        Time::Month m = Time::june;
	unsigned d = 30;
        
        Time t(y,m,d,COOK(y));
        // ...
        cout << t.make_string(COOK(y)) << endl;
	return 0;
}

