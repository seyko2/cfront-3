/*ident	"@(#)cls4:lib/stream/rawin.c	1.4" */
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

#define ISTREAM istream

istream& ISTREAM::read(char* s, int n)
{
	if ( !ipfx(n) ) {
		if ( n > 0 ) setstate(eofbit|failbit) ;
		return *this ;
		}
	x_gcount = bp->sgetn(s,n) ; ;
	if ( x_gcount < n ) setstate(eofbit|failbit) ;
	return *this ;
}

istream& ISTREAM::ignore(register int n, register int delim)
{
	x_gcount = 0 ;
	if ( !ipfx(1) ) return *this ;
	if ( n == 0 ) return *this ;
	register streambuf* nbp = bp ;
	register int count = 0 ;

	while ( 1 ) {
		register int c = nbp->sbumpc() ;
		if (  c == EOF ) {
			setstate(eofbit|failbit) ;
			break ;
			}
		++count ;
		if ( c == delim ) break ;
		if ( count == n ) break ;
		}
	x_gcount = count ;
	return *this ;
	}

istream& ISTREAM::get(
	register char* s, 	/* character array to read into */
	register int len,	/* size of character array */
	register char term	/* character that terminates input */
) {
	register int c;
	register streambuf *nbp = bp;


	x_gcount = 0 ;
	if ( len == 0 ) return *this ;

	if ( !ipfx(len) ) {
		*s = 0 ;
		return *this ;
	}

	if ( len<=0 ) return *this ;

	if ((c = bp->sgetc()) == EOF) {
		setstate(failbit | eofbit) ;
		return *this;
	}

	while (c != term && c != EOF && len > 1) {
		*s++ = c;
		c = nbp->snextc();
		len--;
		++x_gcount ;
	}
	*s = '\0';
	if (c == EOF) setstate(eofbit) ;
	return *this;
}


istream& ISTREAM::get(
	register streambuf &s,	/* streambuf to input to */
	register char term	/* termination character */
){
	register int c;

	int oldskip = skip(0) ;
	if ( !ipfx() ) {
		skip(oldskip) ;
		return *this ;
	}
	skip(oldskip) ;
	register streambuf *nbp = bp;

	x_gcount = 0 ;
	if ((c = bp->sgetc()) == EOF) {
		setstate(failbit | eofbit) ;
		return *this;
	}

	while (c != term && c != EOF) {
		if (s.sputc(c) == EOF) break;
		c = nbp->snextc();
		++x_gcount ;
	}
	if (c == EOF) setstate(eofbit) ;
	return *this;
}

istream& ISTREAM::getline(char* b, int len, char d ) 
{
	get(b,len,d) ;
	if ( x_gcount != len-1 && d!=EOF && !eof() && bp->sgetc()==d) {
		++x_gcount ;
		bp->sbumpc() ;
	}
	return *this ;
}

istream& ISTREAM::getline(unsigned char* b, int len, char d ) 
{
	return getline((char*)b,len,d) ;
}

int ISTREAM::gcount()
{
	return x_gcount ;
}

istream& ISTREAM::operator>>(register streambuf* s) {
	register int c;
	if ( !ipfx() ) return *this ;

	register streambuf *nbp = bp;

	if ((c = bp->sgetc()) == EOF) {
		setstate(failbit | eofbit) ;
		return *this;
	}

	while (c != EOF) {
		if (s->sputc(c) == EOF) break;
		c = nbp->snextc();
	}
	if (c == EOF) setstate(eofbit) ;
	return *this;
}

