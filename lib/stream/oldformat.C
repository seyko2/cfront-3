/*ident	"@(#)cls4:lib/stream/oldformat.c	1.4" */
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

#include <stream.h>
#include "streamdefs.h"
#include <string.h>
#include <strstream.h>
#include <fstream.h>
#include <libc.h>

#ifdef VSPRINTF
#	include <stdarg.h>
#endif

static const int cb_size = 1024 ;
static const int fld_size = 256 ;

/* this error message is defined here so it doesn't get overwritten
   if the buffer overflows */
static char fmsg1[] = "Error in form(): too many characters\n";

/* a circular formating buffer */
static char	formbuf[cb_size];	// some slob for form overflow
static char*	bfree=formbuf;
static char*	max = &formbuf[cb_size-1];

char* chr(register int i, register int w)	/* note: chr(0) is "" */
{
	register char* buf = bfree;

	if (w<=0 || fld_size<w) w = 1;
	w++;				/* space for trailing 0 */
	if (max < buf+w) buf = formbuf;
	bfree = buf+w;
	char * res = buf;

	w -= 2;				/* pad */
	while (w--) *buf++ = ' ';
	if (i<0 || 127<i) i = ' ';
	*buf++ = i;
	*buf = 0;
	return res;
}

char* str(const char* s, register int w)
{
	register char* buf = bfree;
	int ll = strlen(s);
	if (w<=0 || fld_size<w) w = ll;
	if (w < ll) ll = w;
	w++;				/* space for trailing 0 */
	if (max < buf+w) buf = formbuf;
	bfree = buf+w;
	char* res = buf;

	w -= (ll+1);			/* pad */
	while (w--) *buf++ = ' ';
	while (*s) *buf++ = *s++;
	*buf = 0;
	return res;
}

char* form(const char* format ...)
{
	register char* buf = bfree;
	if (max < buf+fld_size) buf = formbuf;

#	ifdef VSPRINTF
		va_list args ;
		va_start(args,format) ;
		VSPRINTF(buf,format,args) ;
		va_end(args) ;
#	else
		// not very portable
		register int* ap = (int*)((char*)&format+sizeof(char*));
		sprintf(buf,format,ap[0],ap[1],ap[2],ap[3],ap[4],ap[5],ap[6],ap[7],ap[8],ap[9]);	
#	endif

	register int ll = strlen(buf);			// not all sprintf's return length

	// If we have scribbled beyond the end of the buffer then
	// who knows what data we've destroyed.  Better to abort here
	// here there is some chance that somebody can associate
	// the location with the error than to continue and die
	// a mysterious death later.
//	if (fld_size < ll) abort();
	if (buf+ll >= formbuf+cb_size-1) {
	    fprintf(stderr, fmsg1);
	    abort();
	}
	
	bfree = buf+ll+1;
	return buf;
}

const char a10 = 'a'-10;

char* hex(long ii, register int w)
{
	int m = sizeof(long)*2;		// maximum hex digits for a long
	if (w<0 || fld_size<w) w = 0;
	int sz = (w?w:m)+1;
	register char* buf = bfree;
	if (max < buf+sz) buf = formbuf;
	register char* p = buf+sz;
	bfree = p+1;
	*p-- = 0;			// trailing 0
	register unsigned long i = ii;

	if (w) {
		do {
			register int h = (int)(i&0xf);
			*p-- = (h < 10) ? h+'0' : h+a10;
		} while (--w && (i>>=4));
		while (0<w--) *p-- = ' ';
	}
	else {
		do {
			register int h = (int)(i&0xf);
			*p-- = (h < 10) ? h+'0' : h+a10;
		} while (i>>=4);
	}
	return p+1;
}

char* oct(long ii, int w)
{
	int m = sizeof(long)*3;		// maximum oct digits for a long
	if (w<0 || fld_size<w) w = 0;
	int sz = (w?w:m)+1;
	register char* buf = bfree;
	if (max < buf+sz) buf = formbuf;
	register char* p = buf+sz;
	bfree = p+1;
	*p-- = 0;			// trailing 0
	register unsigned long i = ii;

	if (w) {
		do {
			register int h = (int)(i&07);
			*p-- = h + '0';
		} while (--w && (i>>=3));
		while (0<w--) *p-- = ' ';
	}
	else {
		do {
			register int h = (int)(i&07);
			*p-- = h+'0';
		} while (i>>=3);
	}

	return p+1;
}

char* dec(long i, int w)
{
	int sign = 0;
	if (i < 0) {
		sign = 1;
		i = -i;
	}	
	int m = sizeof(long)*3;		/* maximum dec digits for a long */
	if (w<0 || fld_size<w) w = 0;
	int sz = (w?w:m)+1;
	register char* buf = bfree;
	if (max < buf+sz) buf = formbuf;
	register char* p = buf+sz;
	bfree = p+1;
	*p-- = 0;			/* trailing 0 */

	if (w) {
		do {
			register int h = (int)(i%10);
			*p-- = h + '0';
		} while (--w && (i/=10));
		if (sign && 0<w) {
			w--;
			*p-- = '-';
		}
		while (0<w--) *p-- = ' ';
	}
	else {
		do {
			register int h = (int)(i%10);
			*p-- = h + '0';
		} while (i/=10);
		if (sign) *p-- = '-';
	}

	return p+1;
}

istream& WS(istream& i) { return i >> ws ; }

void eatwhite(istream& i) { i >> ws ; }

istream::istream(streambuf* b, int sk, ostream* t) 
{
	init(b) ;
	skip(sk) ;
	tie(t) ;
	}

istream::istream(int len, char* b, int sk) 
{
	init( new streambuf(b,len,-len) ) ;
	delbuf = 1 ;
	skip(sk) ;
}

istream::istream(int fd, int sk, ostream* t) 
{
	init( new filebuf(fd) ) ;
	delbuf = 1 ;
	skip(sk) ;
	tie(t) ;
}

ostream::ostream(int fd) 
{
	init( new filebuf(fd) ) ;
	delbuf = 1 ;
}

ostream::ostream(int len, char* b) 
{
	init( new streambuf(b,len,0) ) ;
	delbuf = 1 ;
}

streambuf::streambuf(char* p, int l, int c ) :
	x_unbuf(0), alloc(0)
{
	setb(0,0,0);
	setbuf(p,l,c) ;
}

streambuf* streambuf::setbuf(char* p, int len, int count)
{
	// Three argument setbuf for compatibility with old
	// stream package.
	if ( x_base ) return 0 ;
	if ( len <= 0 || p == 0 ) {
		// make it unbuffered
		setb(0,0,0) ;
		setg(0,0,0) ;
		setp(0,0);
		unbuffered(1) ;
	} else if ( count >= 0 ) {
		setb(p,p+len,0) ;
		setg(p,p,p+count) ;
		setp(p+count,p+len) ;
		unbuffered(0) ;
	} else  { // count < 0 
		// Special case,
		// Used by iostream::iostream(char*,int) 
		// For backwards compatibility with old streams
		setb(p,p+len,0) ;
		setg(p,p,p-count) ;
		setp(p,p+len) ;
		unbuffered(0) ;
		}
	return this;
	}

int ios::skip(int sk)
{
	long f ;
	if ( sk ) f = setf(skipws,skipws) ;
	else	  f = setf(0,skipws) ;
	
	return (f&skipws) != 0 ;
}
