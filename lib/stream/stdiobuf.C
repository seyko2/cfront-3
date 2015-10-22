/*ident	"@(#)cls4:lib/stream/stdiobuf.c	1.5" */
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
#include <stdiostream.h>

int stdiobuf::overflow(int c) 
{
	if ( fp == 0 || c==EOF) return EOF ;
	if ( last_op == ios::in ) {
		if ( egptr()!=gptr() ) ungetc(*gptr(),fp) ;
		// stdio requires seeks between reads and writes
		fseek(fp,0,ios::cur) ;
		setg(0,0,0) ;
		}
	last_op = ios::out ;
	
	return putc(c,fp) ;
}

int stdiobuf::underflow() {
	if ( fp == 0 ) return EOF ;
	if ( last_op == ios::out ) {
		fseek(fp,0,0) ;
		}
	if ( feof(fp) ) return EOF ;
	int c = getc(fp) ;
	if ( c == EOF ) return EOF ;
	setg(buf,buf,buf+1) ;
	buf[0] = c ;
	return c ;
}

int stdiobuf::pbackfail(int c) 
{
	return ungetc(c,fp) ;
}

int stdiobuf::sync()
{
	if ( last_op==ios::out ) fflush(fp) ;
	else if ( last_op==ios::in && gptr()!=egptr() ) {
		ungetc(*gptr(),fp) ;
		setg(0,0,0) ;
	}
	return fseek(fp,0,ios::cur) ;
}
	
streampos stdiobuf::seekoff(streamoff p,ios::seek_dir d,int) 
{

	if ( gptr()!=egptr() ) ungetc(*gptr(),fp) ;
	setg(0,0,0) ;
	return fseek(fp,p,d) ;
}

stdiobuf::stdiobuf(FILE* f) :
	fp(f)
{
	setbuf((char*)0,0) ;
}

stdiobuf::~stdiobuf() 
{
	if (fp) fflush(fp) ;
}

stdiostream::stdiostream(FILE* f) :  buf(f)
{
	init(&buf) ;
}

stdiostream::~stdiostream() { }

stdiobuf* stdiostream::rdbuf() { return &buf ; }
