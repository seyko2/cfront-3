/*ident	"@(#)G2++:g2++lib/g2ctype.c	3.1" */
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

#include <g2ctype.h>

int 
isname_ATTLC( 
    const String& s 
){
    int i=0;

    if( 
	!isname1_ATTLC(s.char_at(i++)) 
    ){
	return 0;
    }
    for( 
	; 
	i<s.length();
	i++ 
    ){
	if( 
	    !isname2_ATTLC(s.char_at(i)) 
	){
	    return 0;
	}
    }
    return 1;
}
int 
isint_ATTLC( 
    const String& s 
){
    int i=0;
    int first=1;

    for( i=0;i<s.length();i++ ){

	if( !isdigit_ATTLC(s.char_at(i)) ){

	    if(first && (s.char_at(i)=='-')){
		continue;
	    }
	    return 0;
	}
	first = 0;
    }
    return 1;
}
