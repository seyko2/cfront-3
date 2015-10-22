/*ident       "@(#)cls4:lib/stream/flt.c	1.2.1.7" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/

#include <iostream.h>
#include <ctype.h>
#include <stdio.h>
#include <libc.h>
#include <string.h>
#include <math.h>	// for frexp()

#define OSTREAM ostream
#define ISTREAM istream

// This file contains all the functions having to do with i/o of
// floats and doubles.  It drags in lots of stuff from stdio, which
// is why I made it separate.

istream& ISTREAM::operator>>(double& d)
/*
	{+|-} d* {.} d* { e|E {+|-} d+ } 
	except that
		- a dot must be pre- or succeeded by at least one digit
		- an exponent must be preceded by at least one digit
*/
{
	register int c = 0;
	register int anydigits = 0 ;
	char buf[256];
	register char* p = buf;
	register streambuf* nbp = bp;
	
	if (!ipfx() ) return *this ;

	/* get the sign */
	switch (c = nbp->sgetc()) {
	case EOF:
		setstate(eofbit|failbit) ;
		return *this;
	case '-':
	case '+':
		*p++ = c;
		c = bp->snextc();
	}

	/* get integral part */
	while (isdigit(c)) {
		*p++ = c;
		c = bp->snextc();
		anydigits = 1 ;
	}

	/* get fraction */
	if (c == '.') {
		do {
			*p++ = c;
			c = bp->snextc();
			anydigits = 1 ;
		} while (isdigit(c));
	}

	/* get exponent */
	if ( anydigits && (c == 'e' || c == 'E')) {
		*p++ = c;
		switch (c = nbp->snextc()) {
		case EOF:
			setstate(eofbit|failbit) ;
			return *this;
		case '-':
		case '+':
			*p++ = c;
			c = bp->snextc();
		}
		while (isdigit(c)) {
			*p++ = c;
			c = bp->snextc();
		}
	}

	*p = 0;
	d = atof(buf);

	if (c == EOF) setstate(eofbit) ;
	if (!anydigits) setstate(badbit) ;
	return *this;
}


istream& ISTREAM::operator>>(float& f)
{
	double d;


	if (!ipfx() ) return *this ;

	*this >> d ;
	if ( good() || rdstate() == eofbit ) f = d;

	return *this;
}

ostream& OSTREAM::operator<<(float f)  { return *this << (double)f ; }

//static const int fltbits = ios::scientific|ios::fixed|ios::uppercase ;

ostream& OSTREAM::operator<<(double d)
{
	char buf[256];
	int w = width(0) ;
	int p = precision() ;
	int explicitp = ( p > 0 ) || (flags()&ios::fixed) ;
	if (!opfx()) return *this;
//	long bits = flags() & fltbits ;
	char fmt[256] ;
	strcpy(fmt,"%") ;
	if ( flags() & ios::showpoint ) {
		strcat(fmt,"#") ;
		}
	if ( flags() & ios::showpos ) {
		strcat(fmt,"+") ;
		}
	if ( explicitp ) {
		strcat(fmt,".*") ;
		}

	long mode = flags()
		    & (ios::scientific|ios::fixed|ios::uppercase);
	
	switch(mode) {
	case ios::scientific :		strcat(fmt,"e") ; break ;
	case ios::scientific|ios::uppercase :
						strcat(fmt,"E") ; break;
	case ios::fixed :			strcat(fmt,"f") ; break ;
	case ios::fixed|ios::uppercase:	strcat(fmt,"f") ; break ;
	case ios::uppercase :		strcat(fmt,"G") ; break ;
	default :				strcat(fmt,"g") ; break ;
	}
 
	char *result;
	char *localbuf = 0;
	result = buf;
	if ( explicitp ) {
		if (flags()&ios::fixed) {
			int exponent;
			/*double jnk =*/ frexp(d, &exponent);
			localbuf = new char[p+(exponent>0 ? 4+exponent/3 : 4)];
				// need 3 extra cells, one for sign,
				// one for decimal point, one for null
			sprintf(localbuf,fmt,p,d) ;
			result = localbuf;
		}
		else if (p < 25) {
			sprintf(buf,fmt,p,d) ;
			}
		else {
			localbuf = new char[p + 10];
			sprintf(localbuf,fmt,p,d) ;
			result = localbuf;
			}
		}
	else {
		sprintf(buf,fmt,d) ;
		}

	int padf = w-strlen(result) ;
	if ( padf < 0 ) padf = 0 ;
	if ( padf && !(flags()&ios::left) ) {
		if (flags()&ios::internal)
			if (*result == '+' || *result == '-')
				put(*result++);
		while ( padf-- > 0 ) put(fill()) ;
		}
	*this << result ;
	while ( padf-- > 0 ) put(fill()) ;
	osfx() ;
	if (localbuf != 0) delete localbuf;
	return *this ;
	}
