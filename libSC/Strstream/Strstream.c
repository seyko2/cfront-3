/*ident	"@(#)Strstream:Strstream.c	3.1" */
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

#include <iostream.h>
#include <Strstream.h>

static char sccs_id[] = "@(#)Strstream version, 082096";
#ifdef __GNUG__
#define zapeof(x) ((x)&0377)
#endif

/*
 * There are three basic kinds of Strstreambuf objects:
 *
 *   - created with the default constructor (this kind uses
 *     the "String s" private data member in Strstreambuf as
 *     the internal buffer)
 *   - created with the Strstreambuf(String&) constructor
 *     (this kind of Strstreambuf permits both reading and
 *     writing -- reading the Strstreambuf causes characters
 *     to be deleted from the beginning of the String, writing
 *     to the Strstreambuf causes characters to be added to
 *     the end of the String)
 *   - created with the Strstreambuf(const String&) constructor
 *     (this kind of Strstreambuf is "read-only" -- all writing
 *     operations will fail and reading operations don't change
 *     the value in the String [they just update the
 *     chars_consumed private data member])
 */
Strstreambuf::Strstreambuf()
{
	p = &s ;
#ifndef L5_COMPATIBLE
	chars_consumed = 0;
	output_flag = 1;
#endif
	setb(array, array+sizeof(array)) ;
	setp(0,0) ;
	setg(0,0,0) ;
}

Strstreambuf::Strstreambuf(String& ss) : p(&ss)
{
#ifndef L5_COMPATIBLE
	chars_consumed = 0;
	output_flag = 1;
#endif
	setb(array, array+sizeof(array)) ;
	setp(0,0) ;
	setg(0,0,0) ;
}

#ifndef L5_COMPATIBLE
Strstreambuf::Strstreambuf(const String& ss) : p((String *)&ss)
{
	chars_consumed = 0;
	output_flag = 0;
	setb(array, array+sizeof(array)) ;
	setp(0,0) ;
	setg(0,0,0) ;
}
#endif

Strstreambuf::~Strstreambuf() { }

/*
 *  The Strstreambuf class will append characters to
 *  the end of the String in batches rather than one
 *  at a time.  The size of the batch is usually the
 *  length of Strstreambuf::array.
 */
int Strstreambuf::overflow(int c)
{
#ifndef L5_COMPATIBLE
	if (output_flag == 0) return EOF;
#endif

	if ( gptr() ) sync() ;
	
	if ( pbase() < pptr()) {
		*p += String(pbase(),pptr()-pbase()) ;
	}
	if ( c != EOF ) *p += (char)c ;
	setp(base(),ebuf()) ;
	return zapeof(c) ;
}

/*
 *  If the current supply of input characters has been
 *  exhausted, the underflow() function will copy the
 *  next len characters from the String into the array.
 */
int Strstreambuf::underflow()
{
#ifndef L5_COMPATIBLE
	if (in_avail() > 0) {
		return ((int) *gptr());
	}
#endif
	if ( pptr() ) sync() ;
#ifndef L5_COMPATIBLE
	int len = p->length() - chars_consumed;
#else
	int len = p->length() ;
#endif

	if ( len <= 0 ) {
		setg(0,0,0) ;
		return EOF ;
	}

	if ( len > blen() ) len = blen() ;
	
	/* len is now set to the minimum of the length
	   of the remaining part of the String and the
	   size of the holding area. */
	
	if ( unbuffered() ) {
		setg(array,array,array+1) ;
		len = 1;
	}
	else {
		setg(base(),base(),base()+len) ;
	}

	register char* pc = gptr() ;
	register const char *lclptr = (const char *) *p;
#ifndef L5_COMPATIBLE
	memcpy(pc, lclptr + chars_consumed, len);
	chars_consumed += len;
#else
	memcpy(pc, lclptr, len);
#endif
	
#ifdef L5_COMPATIBLE
	*p = p->chunk(len); // delete the first "len" characters from
			    // the Strstreambuf
#endif
	return zapeof(*gptr()) ;
}

int Strstreambuf::pbackfail(int c) 
{
#ifndef L5_COMPATIBLE
	if (chars_consumed > 0) chars_consumed--;
#else
	p->unget(c) ;
#endif
	return zapeof(c) ;
}

int Strstreambuf::sync() 
{
	if ( pptr() > pbase() ) overflow(EOF) ;

	setp(0,0) ;
	if ( gptr() < egptr() ) {
#ifndef L5_COMPATIBLE
		chars_consumed -= (egptr()-gptr());
#else
		*p = String(gptr(),egptr()-gptr()) + *p ;
#endif
	}
	setg(0,0,0) ;
#ifndef L5_COMPATIBLE
	if (output_flag == 1 && chars_consumed > 0) {
		/* if this is the buffer for a Strstream that permits
		   reading and writing, then we need to delete the
		   characters that have been read already */
		*p = p->chunk(chars_consumed);
		chars_consumed = 0;
	}
#endif

	return 0 ;
}

streambuf* Strstreambuf::setbuf(char* _p, int len)
{
	if ( _p == 0  || len <= 0 ) unbuffered(1) ;
	else {
		unbuffered(0) ;
		setb(_p,_p+len) ;
	}
	return this ;
}
		
#ifndef L5_COMPATIBLE
streampos Strstreambuf::seekoff(streamoff off_val, ios::seek_dir sdir, int rw) {
	streampos retval = EOF;
	if (output_flag == 0 && rw == ios::in) {
		int newval;
		if (sdir == ios::cur) {
			newval = chars_consumed - (egptr() - gptr()) + off_val;
		}
		else if (sdir == ios::beg) {
			newval = off_val;
		}
		else if (sdir == ios::end) {
			newval = p->length() - 1 + off_val;
		}
		if (newval >= 0 && newval < p->length()) {
			chars_consumed = newval;
			retval = newval;
			setg(0,0,0);
		}
	}
	return retval;
}
streampos Strstreambuf::seekpos(streampos a, int mode) {
	return seekoff(a, ios::beg, mode);
}
int Strstreambuf::xsgetn(char *buf, int n) {
	int n1 = n;
	while (n1 > 0) {
		*buf++ = sbumpc();
		n1--;
	}
	return (n);
}
int Strstreambuf::doallocate() {
	setb(array, array+sizeof(array));
	setp(0,0);
	setg(0,0,0);
	return 0;
}
#endif

String Strstreambuf::str()
{
	sync() ;
	return *p ;
}

Strstreambase::Strstreambase()  { init(&b) ; }

Strstreambase::Strstreambase(String& s) : b(s) { init(&b) ; }
#ifndef L5_COMPATIBLE
Strstreambase::Strstreambase(const String& s) : b(s) { init(&b) ; }
#endif

Strstreambase::~Strstreambase() { }

Strstreambuf* Strstreambase::rdbuf() { return &b ; }

#ifndef L5_COMPATIBLE
iStrstream::iStrstream(const String& s) : Strstreambase(s) { }
#else
iStrstream::iStrstream(String& s) : Strstreambase(s) { }
#endif
iStrstream::~iStrstream() { }

oStrstream::oStrstream(String& s) : Strstreambase(s) { }
oStrstream::oStrstream() { }
oStrstream::~oStrstream() { }
String oStrstream::str() { return rdbuf()->str() ; }

Strstream::Strstream(String& s) : Strstreambase(s) { }
Strstream::Strstream() { }
Strstream::~Strstream() { }
String Strstream::str() { return rdbuf()->str() ; }
