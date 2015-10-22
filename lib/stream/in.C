/*ident	"@(#)cls4:lib/stream/in.c	1.3" */
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

#include <ctype.h>
#include <iostream.h>

#define ISTREAM istream

void ISTREAM::eatwhite ()
{
	register streambuf *nbp = bp;
	register int c = nbp->sgetc();
	while (isspace(c)) c = nbp->snextc();
	if (c == EOF) setstate(eofbit);
}

void ISTREAM::xget(char* c) 
{
	register streambuf* sbp = bp ;
	if ( !ipfx(1) ) return ;
	x_gcount = 0 ; 
	register int newc = sbp->sbumpc() ;
	if ( newc == EOF ) {
		setstate(failbit|eofbit) ;
		return ;
		}
	x_gcount = 1 ;
	*c = newc ;
	}

istream& ISTREAM::operator>>(register char* s)
{
	/* get string */

	register int w = width(0) ;
	if ( flags()&skipws ) {
		// We don't know a maximum number of required
		// characters 
		if ( !ipfx(0) ) return *this ;
		}
	else if ( !ipfx(w) ) {
		return *this ;
		}

	register streambuf *nbp = bp;
	register int c = nbp->sgetc();

	if (c == EOF) setstate(failbit|eofbit) ;

	if ( w > 0 ) {
		while (!isspace(c) && c != EOF && --w > 0 ) {
			*s++ = c;
			c = nbp->snextc();
		}
	} else {
		while (!isspace(c) && c != EOF ) {
			*s++ = c;
			c = nbp->snextc();
		}
	}
	
	*s = '\0';

	if (c == EOF) setstate(eofbit) ;

	return *this;
}

istream& ISTREAM::operator>>(unsigned char* s)
{
	return *this >> (char*)s ;
}

istream& ISTREAM::putback(register char c)
{
	if ( !good() ) return *this ;
	if ( bp->sputbackc(c) == EOF ) setstate(badbit) ;
	return *this;
}
istream& ISTREAM::rs_complicated(unsigned char& c)
{	if ( ipfx(0) ) {
		if (  bp->in_avail() ) {
			c = bp->sbumpc() ;
		} else  xget((char*)&c) ;
	}
	return *this ;
}
istream& ISTREAM::rs_complicated(char& c)
{	if ( ipfx(0) ) {
		if (  bp->in_avail() ) {
			c = bp->sbumpc() ;
		} else  xget((char*)&c) ;
	}
	return *this ;
}
istream& ISTREAM::get_complicated(unsigned char& c)
{
	if ( ipfx(1) && bp->in_avail()) {
		x_gcount = 1 ;
		c = bp->sbumpc() ;
	} else {
		xget((char*)&c) ;
	}
	return *this ;
}
istream& ISTREAM::get_complicated(char& c)
{
	if ( ipfx(1) && bp->in_avail()) {
		x_gcount = 1 ;
		c = bp->sbumpc() ;
	} else {
		xget(&c) ;
	}
	return *this ;
}
/* int ISTREAM::get_complicated()
{
	int c ;
	if ( !ipfx(1) ) return EOF ;
	else {
		c = bp->sbumpc() ;
		if ( c == EOF ) setstate(eofbit) ;
		return c ;
		}
}
*/
