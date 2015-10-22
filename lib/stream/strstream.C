/*ident	"@(#)cls4:lib/stream/strstream.c	1.9" */
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
#include <string.h>
#include <strstream.h>
#include <memory.h>

typedef unsigned int Size_t ;	
			// Some day <memory.h> will be ANSIfied and
			// this will be declared there.
			// Until then make it compatible with declaration
			// of third argument of memcpy.

static const int initsize = 16*sizeof(int) ;
static const int increment = 2 ; 	

static const int arbitrary = 1024 ;	// used by ignore_oflow.

static const int ios_atend = ios::ate ;
static const int ios_input = ios::in ;
static const int ios_output = ios::out ;
static const int ios_append = ios::app ;
static const int seek_cur = ios::cur ;
static const int seek_end = ios::end ;
static const int seek_beg = ios::beg ;

strstreambuf::strstreambuf(void* (*a)(long), void (*f)(void*)) :
	afct(a),
	ffct(f),
	froozen(1),
	auto_extend(initsize),
	ignore_oflow(1)
{
	}

void strstreambuf::init(char* b, int size, char* pstart)
{
	if ( b && size > 0 ) {
		setb(b,b+size) ;
		}
	else if ( b && size == 0 ) {
		setb(b,b+strlen(b) ) ;
		}
	else if ( b && size < 0 ) {
		ignore_oflow = 1 ;
		setb(b,b+arbitrary) ;
		}
	else if ( !b && size > initsize ) {
		auto_extend = size ;
		setb(0,0) ;
		}
	else {
		setb(0,0) ;
		}

	if ( pstart && base() && base() <= pstart && pstart <= ebuf() ) {
		setp(pstart,ebuf()) ;
		}
	else {
		setp(0,0) ;
		}
	
	if ( base() ) {
		if (pptr()) setg(base(),base(),pptr());
		else setg(base(),base(),ebuf()) ;
		auto_extend = 0 ;
		froozen = 1 ;
		}
	else {
		setg(0,0,0) ;
		}
	}

strstreambuf::strstreambuf(char* b, int size, char* pstart) :
	afct(0),
	ffct(0),
	froozen(0),
	auto_extend(0),
	ignore_oflow(0)
{
	init(b,size,pstart) ; 
	}

strstreambuf::strstreambuf(unsigned char* b, int size, unsigned char* pstart) :
	afct(0),
	ffct(0),
	froozen(0),
	auto_extend(0),
	ignore_oflow(0)
{
	init((char*)b,size,(char*)pstart) ; 
	}


strstreambuf::strstreambuf(int ae) :
	afct(0),
	ffct(0),
	froozen(1),
	auto_extend(ae>0?ae:initsize),
	ignore_oflow(1)
{
	}

strstreambuf::strstreambuf() :
	afct(0),
	ffct(0),
	froozen(1),
	auto_extend(initsize),
	ignore_oflow(1)
{
	}


int strstreambuf::doallocate()
{
	if ( auto_extend < initsize ) auto_extend = initsize ;
	char* newspace  = afct ? (char*)(*afct)(auto_extend) 
			       : new char[auto_extend] ;

	if ( !newspace ) return EOF ;

	froozen = 0 ;
	setb(newspace,newspace+auto_extend,0) ;
			// Will free this space ourselves if neccessary

	setp(newspace,newspace+auto_extend) ;
	setg(newspace,newspace,newspace) ;

	return 0 ;
	}

int strstreambuf::overflow(int c)
{
	if ( !base() ) {
		allocate() ;
		if ( !base() ) return EOF ;
		}
	else if ( epptr() > pptr() + 1 ) {
		// There is space already available so we
		// don't have to do anything.  This is presumably
		// triggered by an explicit flush or something like
		// that. 
		}
	else if ( auto_extend && !froozen ) {
		Size_t inserted=pptr()-base() ; // number of chars previously
						// inserted into buffer

		Size_t extracted=gptr()-base() ;
						// number of chars previously
						// extracted from buffer

		// after we copy chars from current space to a new
		// (larger) area we have to adjust pointers to take into
		// acount previous activities.

		Size_t newsize = (Size_t)increment*blen() + 4 ;
		if ( newsize < auto_extend ) newsize = auto_extend ;
		char* newspace = afct ? (char*)(*afct)(newsize) 
			     	      : new char[newsize] ;

		if ( !newspace ) return EOF ;

		memcpy(newspace,base(),inserted) ;

		if ( ffct )	(*ffct)(base()) ;
		else		delete base() ;

		setb(newspace,newspace+newsize,0) ;
		setp(base()+inserted,ebuf()) ;
		setg(base(),base()+extracted,pptr()) ;
		}
	else if ( ignore_oflow ) {
		setp(pptr(),pptr()+arbitrary) ; 
		}
	else {
		return EOF ;
		}
	if ( c!=EOF ) return sputc(c) ;
	else          return zapeof(c);
	}

int strstreambuf::underflow()
{
	if ( !pptr() ) return EOF ;
	if ( !egptr() ) return EOF ;

	setg(base(),egptr(),pptr()) ;
	
	if ( egptr() <= gptr() )	return EOF ;
	else				return zapeof( *gptr() ) ;
	}

void strstreambuf::freeze(int n) 
{
	froozen = n ;
	}


char* strstreambuf::str() 
{
//	sputc(0);
	freeze() ;
	return base() ;
	}

strstreambuf::~strstreambuf()
{
	if ( !froozen && base() ) {
		if ( ffct ) 	ffct(base()) ;
		else		delete base() ;
		}
	}

streambuf* strstreambuf::setbuf(char* p, int size) 
{
	if ( p == 0 && ( base()==0 || auto_extend ) ) {
		auto_extend = size ;
		return this ;
		}
	else {
		return 0 ;
		}
	}

streampos strstreambuf::seekoff(streamoff o, ios::seek_dir d, int m)
{
	switch (d) {
	case seek_beg : break ;
	case seek_cur : {
		if ( (m&ios_input) ) {
			o += gptr()-base() ;
			}
		else  {
			o += pptr()-base() ;
			}
		} break ;
	case seek_end : {
		if ( gptr()<=egptr() && egptr()>pptr() ) {
			o += egptr()-base() ;
			}
		else if ( pptr() != NULL && pptr()<=epptr() ) {
			o += pptr()-base();
			}
		} break ;
	    } // end switch
	if ( o < 0 ) return streampos(EOF) ;
	if ( o >= blen() && !ignore_oflow ) return streampos(EOF) ;
	if ( m&ios_input ) setg(base(), base()+o, egptr() ) ;
	if ( m&ios_output) setp(base()+o, epptr() ) ;
	return o ;
	}

strstreambase::strstreambase(char* str, int size, char* pstart)
	:  buf(str,size,pstart) { init(&buf) ; }

strstreambase::strstreambase() : buf() { init(&buf) ; }

strstreambase::~strstreambase() { }

strstreambuf* strstreambase::rdbuf() { return &buf ; }

istrstream::istrstream(char* str)
	: strstreambase(str,strlen(str),0) { }

istrstream::istrstream(char* str, int size)
	: strstreambase(str, size , 0) { }

istrstream::istrstream(const char* str)
	: strstreambase((char *)str,strlen(str),0) { }

istrstream::istrstream(const char* str, int size)
	: strstreambase((char *)str, size , 0) { }

istrstream::~istrstream() { }


static char* pstart(char* str, int size, int m)
{
	if ( (m&(ios_append|ios_output)) == 0 )	return str+size ;
	else if ( (m&(ios_append|ios_atend)) )	return str+strlen(str) ;
	else					return str ;
	}

ostrstream::ostrstream(char* str, int size, int m)
	: strstreambase(str, size, pstart(str,size, (m|ios_output)) )
{ }

ostrstream::ostrstream() : strstreambase() { }


ostrstream::~ostrstream()
{
	if (!strstreambase::rdbuf()->isfrozen())
		ios::rdbuf()->sputc(0) ;
	}

char* ostrstream::str() 
{
	return strstreambase::rdbuf()->str() ;
	}

int ostrstream::pcount() {
	//return ios::rdbuf()->out_waiting() ;
	return strstreambase::rdbuf()->pcount();
	}

int strstreambuf::pcount(){
	return pptr()-base();
}

strstream::strstream() : strstreambase() { }

strstream::strstream(char* str, int size, int m) 
	: strstreambase(str,size,pstart(str,size,m)) 
{ }

char* strstream::str() 
{
	return strstreambase::rdbuf()->str() ;
	}


strstream::~strstream() { }
