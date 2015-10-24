/*ident	"@(#)G2++:g2++lib/manip.c	3.1" */
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

#include "g2ctype.h"
#include "g2debug.h"
#include "g2io.h"
#include "g2manip.h"

#include <stream.h>
#include <String.h>

//  Global data

int seekflag_ATTLC;       // used in getbuf.c and getrec.c
String seekname_ATTLC;    // used in getbuf.c only (?)


String g2seek(
    istream& is,
    const String& name
){
    DEBUG(cerr 
	<< "enter g2seek with name=" 
	<< name 
	<< "\n"
    ;)
    String temp;

    while(
	temp=getname_ATTLC(is)
    ){
	DEBUG(cerr 
	    << "getname_ATTLC returns temp=" 
	    << temp 
	    << "\n"
	;)
	if(
	    name.is_empty() ||
	    temp==name
	){
	    return temp;
	}
    }
    return "";
}

String g2seek(
    istream& is
){
    return g2seek(is,"");
}

String getname_ATTLC(

//  1.  Sync to record boundry
//  2.  Get the name
//  3.  Leave file in position for subsequent 
//      "getbody" or "getbuf_ATTLC"

    istream& 	is
){
    DEBUG(cerr << "enter getname_ATTLC" << "\n";)
    String result;
    int c = '\n'; 
    int last;
    int	sync = 0;
    seekflag_ATTLC = 0;
    /*Mark(is);*/

//  Sync 

    DEBUG(cerr << "ready to sync to record boundary\n";)
    for(;;){
	last = c;
	c = getchar_G2_ATTLC(is);
	DEBUG(
	    cerr 
		<< "is.get() returns c = ASCII " 
		<< c 
		<< " ("
		<< char(c)
		<< " )"
		<< "\n"
	    ;
	)
	if( 
	    c == BS_EOF 
	){
	    DEBUG(
		cerr 
		    << "hit EOF--ready to return result = \"" 
		    << result 
		    << "\"\n"
		;
	    )
	    DEBUG(
		void* state = (void*)is;
		if(
		    state
		){
		    cerr << "stream tests non-zero\n";
		}else{
		    cerr << "stream tests zero\n";
		}
		int e = is.eof();
		cerr << "is.eof() return " << e << "\n";
	    )
	    return result;
	}
	if( 
	    last=='\n' && 
	    isname1_ATTLC(c) 
	){
	    DEBUG(cerr << "ready to break from for loop\n";)
	    break;
	}
	sync = 1;
    }
    if( 
	sync 
    ){
	DEBUG(cerr << "after for loop, sync is true\n";)
	is.putback(c);
	DEBUG(
	    cerr 
		<< "is.putback c = ASCII " 
		<< c 
		<< " ("
		<< char(c)
		<< " )"
		<< "\n"
	    ;
	)
	/*Mark(is);*/
	c = getchar_G2_ATTLC(is);
	DEBUG(
	    cerr 
		<< "is.get() returns c = ASCII " 
		<< c 
		<< " ("
		<< char(c)
		<< " )"
		<< "\n"
	    ;
	)
    }

//  Gather name 

    DEBUG(cerr << "gather name:\n";)
    do{
	result += c;
	c = getchar_G2_ATTLC(is);
	DEBUG(
	    cerr 
		<< "is.get() returns c = ASCII " 
		<< c 
		<< " ("
		<< char(c)
		<< " )"
		<< "\n"
	    ;
	)
    }while( 
	isname2_ATTLC(c) 
    );
    DEBUG(cerr << "after do loop, result = " << result << "\n";)

//  Put back seperator 

    is.putback(c);
    DEBUG(
	cerr 
	    << "putback separator c = ASCII " 
	    << c 
	    << " ("
	    << char(c)
	    << " )"
	    << "\n"
	;
    )

//  Set the seekflag so that subsequent stream 
//  extractions employ getbody instead of getrec_ATTLC

    seekflag_ATTLC=1;
    seekname_ATTLC=result;
    DEBUG(cerr 
	<< "ready to return with result = " 
	<< result 
	<< "\n"
    ;)
    return result;
}
