/*ident	"@(#)G2++:g2++lib/g2util.c	3.1" */
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

//  Utility functions for G2

#include <g2ctype.h>
#include <g2debug.h>
#include <g2io.h>
#include <stream.h>

int g2sync_ATTLC(

//  Sync file to next g2 record boundary.
//
//  A 'g2 record boundry' is either: 
//
//      (1) the position just after two
//          consecutive newlines, or 
//      (2) the position just after a newline
//          and just before a alphabetic or 
//          underscore character.

    istream& is
){
    return _g2sync_ATTLC(0,is);
}

int _g2sync_ATTLC(

//  _g2sync is used by getrec_ATTLC & getbuf_ATTLC

    int      c, 
    istream& is
){
    DEBUG(cerr << "enter _g2sync\n";)
    int	lastc = c;

    do{
	c = getchar_G2_ATTLC(is);

	if( 
	    lastc == '\n' 
	){
	    if(
		c == '\n'
	    ){
		break;
	    }
	    if(
		isname1_ATTLC(c)
	    ){
		is.putback(c);
		break;
	    }
	}
	lastc = c;
    }while (
	c != BS_EOF
    );
    DEBUG(
	cerr 
	    << "ready to return with c = ASCII " 
	    << c 
	    << " ("
	    << char(c)
	    << ")"
	    << "\n"
	;
    )
    return c != BS_EOF;
}
int _g2otoi_ATTLC(

//  Ascii octal string to int conversion

    const char* s
){

    int	rval = 0;

    while( 
	isdigit_ATTLC(*s) 
    ){
	rval = (rval<<3) + *s++ - '0';
    }
    return rval;
}
