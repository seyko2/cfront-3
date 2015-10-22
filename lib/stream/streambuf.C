/*ident	"@(#)cls4:lib/stream/streambuf.c	1.5" */
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
#include "streamdefs.h"
#include <string.h>
#include <memory.h>

/*
	Allocate some space for the buffer.
	Returns:	EOF on error
			0 on success
*/
int streambuf::doallocate()
{
	char *buf = new char[STREAMBUFSIZE] ;
	if ( !buf ) return EOF ;
	setb(buf,buf+STREAMBUFSIZE,1) ;
	return 0;
}

/*
	Come here on a put to a full buffer.  Allocate the buffer if 
	it is uninitialized.
	Returns:	EOF on error
			the argument on success
*/
int streambuf::overflow(int c)
{
	if ( c==EOF ) 			return zapeof(c) ;
	if ( allocate() == EOF)		return EOF;

	if ( x_pptr <= x_epptr )  	return sputc(c) ;
	else				return EOF ;
}

/*
	Fill a buffer.
	Returns:	EOF on error or end of input
			next character on success
*/
int streambuf::underflow()
{
	if ( x_pptr > x_egptr ) setg(x_eback,x_gptr,x_pptr) ;

	if ( x_egptr > x_gptr )	return 0 ;
	else 		    	return EOF ;
}

int streambuf::pbackfail(int)
{
	return EOF;
}

int streambuf::sync()
{
	// It's unclear exactly what this should do.  Should it reset
	// the buffer or what.  One theory (that used to be in the code.
	// was that it should insert a 0.  Which seems to be the
	// right thing for "strings".
	// if ( x_pptr && x_epptr > x_pptr )  sputc(0) ;
	return EOF ;
}


streampos streambuf::seekpos(streampos p, int m)
{
	return seekoff(p, ios::beg, m) ;
	}

streampos streambuf::seekoff(streampos,ios::seek_dir,int) 
{
	return EOF ;
}

int streambuf::xsputn(register const char* s, int n)
{
	register int req = n ;
	if ( unbuffered() ) {
		while( req-- > 0 ) {
			if ( sputc(*s++) == EOF ) return n-req-1 ;
		}
		return n ;
	}
	register int avail = x_epptr-x_pptr ;
	while ( avail < req ) {
		memcpy(x_pptr,s,avail) ;
		s += avail ;
		pbump(avail) ;
		req -= avail ;
		if ( overflow(zapeof(*s++)) == EOF ) return n-req ;
		--req ;
		avail = x_epptr-x_pptr ;
	}
	memcpy(x_pptr,s,req ) ;
	pbump(req) ;
	return n ;
}

int streambuf::xsgetn(register char* s, int n)
{
	register char* p = s ;
	register int req = n ;

	if ( req <= 0 ) return 0 ;

	if (  unbuffered()  ) {
		while (req-- > 0 ) {
			register int c ;
			if ( (c=sbumpc() ) != EOF )  *p++ = c ;
			else 			return p-s ;
		}
		return n ;
	}

	register int avail = x_egptr-x_gptr ;
	while (  avail < req ) {
		memcpy(p,x_gptr,avail) ;
		p += avail ; 
		req -= avail ;
		gbump(avail) ;
		if ( underflow()==EOF ) return p-s ;
		avail = x_egptr-x_gptr ;
		}

	memcpy(p,x_gptr,req) ;
	gbump(req) ;
	return n ;
}


streambuf* streambuf::setbuf(char* p , int len)
{

	if ( x_base ) return 0 ;
	if ( len <= 0 || p == 0 ) {
		// make it unbuffered
		setb(0,0,0) ;
		setg(0,0,0) ;
		setp(0,0);
		unbuffered(1) ; 
		}
	else {
		setb(p,p+len,0) ;
		setg(p,p,p) ;
		setp(p,p+len) ;
		unbuffered(0) ;
		} 
	return this;
}

streambuf* streambuf::setbuf(unsigned char* p, int len)
{
	return setbuf((char*)p,len) ;
}

streambuf::streambuf() :
	x_unbuf(0), alloc(0)
{
	setb(0,0,0);
	setg(0,0,0);
	setp(0,0);
}

streambuf::streambuf(char* p, int l) :
	x_unbuf(0), alloc(0)
{
	setb(0,0,0);
	setbuf(p,l) ;
}

streambuf::~streambuf() 
{
	sync() ;
	if (x_base && alloc) delete x_base;
	}

int streambuf::x_snextc()
{
	// called by snextc to handle underflow
	if ( x_egptr==0 || x_gptr != x_egptr ) {
		// we stepped beyond x_gptr meaning snextc was called when
		// x_gptr == x_egptr rather than when x_gptr+1=x_egptr.
		underflow() ;
		gbump(1) ;
		}
	return sgetc() ;
	}
