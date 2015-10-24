/*ident	"@(#)G2++:g2++lib/getdot.c	3.1" */
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

#include <g2debug.h>
#include <g2ctype.h>
#include <g2io.h>
#include <stream.h>
#include <String.h>

//  Local functions

static int 
_ato64(
    String s
);
static int 
_64toa(
    String s,
    unsigned int n
);
int 
_g2getdot_ATTLC(
    istream& is
){
    //char name[100]; 
    //char value[100];
    int	c;
    int	checksum = Mark(is);

//  Collect attribute name 

    String name;

    for( ;; ){
	c = getchar_G2_ATTLC(is);
	if( !isname2_ATTLC(c) )break;
	name += c;
    }

    if( 
	c == BS_EOF 
    ){
	return BS_EOF;
    }
    if( 
	c == '\n' 
    ){
	char result;
	result = getchar_G2_ATTLC(is);
	return result;
    }
    if( 
	name == "c"
    ){

//  Checksum 

	String value;

	for( ;; ){
	    c = getchar_G2_ATTLC(is);
	    if( c=='\n' || c==BS_EOF )break;
	    value += c;
	}

	if( 
	    _ato64(value) != checksum 
	){
	    g2error_ATTLC(G2BADCKSUM);
	}

    }else{

//  Consume up thru newline

	for( ;; ){
	    c = getchar_G2_ATTLC(is);
	    if( c=='\n' || c==BS_EOF )break;
	}
    }
    if( 
	c == BS_EOF 
    ){
	return c;
    }

//  Return first character of next line

    char result;
    result = getchar_G2_ATTLC(is);
    return result;
}

void 
_g2putdot_ATTLC(
    ostream& os
){
    DEBUG(cerr << "enter _g2putdot_ATTLC\n";)
    int	checksum = Mark(os);
    DEBUG(cerr << "checksum=" << checksum << "\n";)
    String nbr="                    ";

    os << ".c\t";
    int i = _64toa(
	nbr, 
	checksum
    );
    DEBUG(cerr << "_64toa returns i=" << i << "\n"; )
    DEBUG(cerr << "insert into os: ";)
    while( i<nbr.length() ){
	DEBUG(cerr << nbr[i];)
	os << nbr[(unsigned)i];
    }
    DEBUG(cerr << "\n";)
    os << "\n\n";
    /*Mark(os);*/
}

static int 
_ato64(
    String s
){
    const char* p = (const char*)s;
    DEBUG(cerr 
	<< "enter _ato64 with s =" 
	<< s 
	<< "\n"
    ;)
    int n = 0;

    while(*p){
	n = (n<<6) + *p++ - '0';
	DEBUG(
	    cerr 
		<< "in for loop, n=" 
		<< oct << n << dec
		<< "\n"
	    ;
	)
    }
    DEBUG(cerr << "exit from for loop\n";)
    return n;
}
static int 
_64toa(
    String s,
    unsigned int n
){
    DEBUG(cerr 
	<< "enter _64toa with s="
	<< s
	<< "\n"
	<< "n="
	<< n
	<< "\n"
    ;)
    int i;
    for(
	i=s.length()-1;
	n;
	n >>= 6,i--
    ){
	s[(unsigned)i] = (n&077) + '0';
	DEBUG(cerr 
	    << "in loop, s=" 
	    << s 
	    << "\n"
	;)
    }
    DEBUG(cerr 
	<< "ready to return i+1=" 
	<< i+1 
	<< "\n"
    ;)
    return i+1;
}
