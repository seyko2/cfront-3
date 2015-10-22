/*ident	"@(#)cls4:lib/stream/manip.c	1.3" */
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
#include <iomanip.h>
#include <ctype.h>

istream& ws(istream& i)
{
	if ( !i.ipfx(1) ) return i ;
	register streambuf *nbp = i.rdbuf();
	register int c = nbp->sgetc();
	while (isspace(c)) c = nbp->snextc();
	if (c == EOF) i.clear(ios::eofbit) ;
	return i ;
}

ostream& ends(ostream& i) 
{
	return i.put(0) ;
}

ostream& endl(ostream& i) 
{
	i.put('\n') ;
	i.flush() ;
	return i ;
}

static const int basebits = ios::dec|ios::oct|ios::hex ;

static ios& setb(ios& i, int b)
{
	switch(b) {
		case 10 : i.setf(ios::dec,basebits) ; break ;
		case 8 : i.setf(ios::oct,basebits) ; break ;
		case 16 : i.setf(ios::hex,basebits) ; break ;
		default : i.setf(0,basebits) ; break ;
		}
	return i ;
	}

SMANIP(int) setbase(int b) { return SMANIP(int)(setb,b) ; }

static ios& resetiosflags(ios& i,long b) { i.setf(0,b) ; return i ; }
SMANIP(long) resetiosflags(long b) { return SMANIP(long)(resetiosflags,b) ; }

static ios& setiosflags(ios& i,long b) { i.setf(b) ; return i ; }
SMANIP(long) setiosflags(long b) { return SMANIP(long)(setiosflags,b) ; }

static ios& setfill(ios& i,int f) { i.fill(f) ; return i ; }
SMANIP(int) setfill(int f) { return SMANIP(int)(setfill,f) ; }

static ios& setprecision(ios& i,int p) { i.precision(p) ; return i ; }
SMANIP(int) setprecision(int p) { return SMANIP(int)(setprecision,p) ; }

static ios& setw(ios& i,int w) { i.width(w) ; return i ; }
SMANIP(int) setw(int w) { return SMANIP(int)(setw,w) ; }

ios& hex(ios& s) { s.setf(ios::hex,basebits ) ; return s ; }
ios& dec(ios& s) { s.setf(ios::dec,basebits ) ; return s ; }
ios& oct(ios& s) { s.setf(ios::oct,basebits ) ; return s ; }

ostream& flush(ostream& s) { s.flush() ; return s ; }

ostream& ostream::operator<<( ios& (*f)(ios&) )
{
	f(*this) ;
	return *this ;
	}

istream& istream::operator>>( ios& (*f)(ios&) )
{
	f(*this) ;
	return *this ;
	}
