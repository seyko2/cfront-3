/*ident	"@(#)cls4:lib/stream/stream.c	1.4" */
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

void (*ios::stdioflush)() = 0 ;

long ios::nextbit = 1L<<16 ;
long ios::nextword = 0 ;

const long ios::adjustfield = ios::left|ios::right|ios::internal ;
const long ios::floatfield = ios::fixed|ios::scientific ;
const long ios::basefield = ios::dec|ios::hex|ios::oct ;

#define ISTREAM istream
#define OSTREAM ostream
#define IOSTREAM iostream

union ios_user_union { long i ; void* p ; } ;

void ios::init(streambuf* b)
{
	// Must be called by other constructors. */

/* initialize private members */
	nuser = 0 ;
	x_user = 0 ;

/* initialize protected members */
	bp = b ;
	state = 0 ;
	ispecial = 0 ;
	ospecial = 0 ;
	isfx_special = 0 ;
	osfx_special = 0 ;
	delbuf = 0 ;
	x_tie = 0 ;
	x_flags = 0 ;
	x_precision = 6 ;
	x_fill = ' ' ;
	x_width = 0 ;

	if (!bp) setstate(hardfail|failbit) ;
	flags(skipws|ios::basefield) ;
	tie(0) ;
	}

ios::ios(streambuf* b) { init(b) ; }

ios::~ios()
{
	if (bp) bp->sync() ;
	if (delbuf) {
		delete bp ;
		bp = 0 ;
		}
	if (x_user) {
		delete x_user ; 
		x_user = 0 ;
		}
	}

iostream::iostream(streambuf* b) { init(b) ; }
iostream::~iostream() { }
istream::istream(streambuf* b) { init(b) ; }
istream::~istream() { }
ostream::ostream(streambuf* b) { init(b) ; }
ostream::~ostream() { }

IOSTREAM::IOSTREAM() { } 
ISTREAM::ISTREAM() { } 
OSTREAM::OSTREAM() { }

ostream& OSTREAM::flush()
{
	if ( !opfx() ) return *this ;
	if ( bp->out_waiting() )  {
		if ( bp->overflow() == EOF ) setstate(ios::badbit) ;
		}	
	else if ( bp->in_avail() ) {
		if ( bp->sync() == EOF ) setstate(ios::badbit) ;
		}
	return *this ;
}

streampos OSTREAM::tellp()
{
	return bp->seekoff(0,cur,out) ;
}

streampos ISTREAM::tellg()
{
	return bp->seekoff(0,cur,in) ;
}
	
ostream& OSTREAM::seekp(streampos p)
{
	if ( bp->seekpos(p,out) == EOF ) setstate(badbit) ;
	return *this ;
}

typedef ios::seek_dir Sdir ; /** Gets around a bug in release 2.0 beta 5 **/

ostream& OSTREAM::seekp(streamoff o, Sdir d)
{
	if ( bp->seekoff(o,d,out) == EOF ) setstate(badbit) ;
	return *this ;
}
	
istream& ISTREAM::seekg(streampos p)
{
	if ( bp->seekpos(p,in) == EOF ) setstate(badbit) ;
	else clear(rdstate()&~(eofbit|badbit));
	return *this ;
}

istream& ISTREAM::seekg(streamoff o, Sdir d)
{
	if ( bp->seekoff(o,d,in) == EOF ) setstate(badbit) ;
	else clear(rdstate()&~(eofbit|badbit));
	return *this ;
}

ostream* ios::tie(ostream* s) 
{
	ostream* t = x_tie ;
	x_tie = s ;

	if ( s ) {
		ispecial |= tied ;
		ospecial |= tied ;
		}
	else {
		ispecial &= ~tied ;
		ospecial &= ~tied ;
		}
	return t ;
}

char ios::fill(char c)
{
	char oldf = x_fill ;
	x_fill = c ;
	return oldf ;
	}

int ios::precision(int p)
{
	register int oldp = x_precision ;
	x_precision = p;
	return oldp ;
	}

long ios::setf(long b, long f)
{
	long oldf = x_flags ;
	x_flags = (b&f) | (x_flags&~f) ;

	if (x_flags&skipws ) 	ispecial |= skipping ;
	else			ispecial &= ~skipping ;

	osfx_special = (x_flags&(unitbuf|stdio)) != 0 ;
			
	return oldf ;
	}

long ios::setf(long b)
{
	long oldf = x_flags ;
	x_flags |= b ;

	if (x_flags&skipws ) 	ispecial |= skipping ;
	else			ispecial &= ~skipping ;

	osfx_special = (x_flags&(unitbuf|stdio)) != 0 ;
	return oldf ;
	}

long ios::unsetf(long b)
{
	long oldf = x_flags ;
	x_flags &= ~b ;

	if (x_flags&skipws ) 	ispecial |= skipping ;
	else			ispecial &= ~skipping ;

	osfx_special = (x_flags&(unitbuf|stdio)) != 0 ;
	return oldf ;
	}

long ios::flags(long f) 
{
	long oldf = x_flags ;
	x_flags = f ;

	if (x_flags&skipws ) 	ispecial |= skipping ;
	else			ispecial &= ~skipping ;

	osfx_special = (x_flags&(unitbuf|stdio)) != 0 ;
	return oldf ;
	}
	
int ISTREAM::do_ipfx(int noskipws)
{
	if ( state&hardfail) return 0 ;
	// note that we flush tied stream even when !this->good().
	if ( x_tie && x_tie->rdbuf()->out_waiting()
			&& (noskipws==0 || rdbuf()->in_avail()<noskipws) ){
		x_tie->flush() ;
		}
	if ( good() && !noskipws && (ispecial&skipping) ) eatwhite() ;
	if ( eof() ) {
		// if we were only skipping this wouldn't be a failure.
		// but the presumption is that this is a prefix operation
		// prior to inputting something else.
		setstate(ios::failbit) ;
	 	return 0 ;
		}
	return good() ;
	}

int OSTREAM::do_opfx()
{
	if ( state&hardfail) return 0 ;
	if ( x_tie && x_tie->rdbuf()->out_waiting()) {
		x_tie->flush() ;
		}
	return good() ;
	}

void OSTREAM::do_osfx()
{
	if ( (x_flags & stdio) && stdioflush ) { (*stdioflush)() ; }
	if ( x_flags & unitbuf ) flush() ;
	}

void ios::operator=(ios& rhs) { bp = rhs.bp ; }

ios::ios() { assign_private = state ; state = hardfail ; } 

istream_withassign::istream_withassign() 
{
	// In order for the standard streams to be properly initialized
	// it is essential that nothing is done by the combination
	// of this constructor and ios::ios().  So we undo the effect of
	// ios::ios() 
	state = assign_private ;
	}

istream_withassign::~istream_withassign()  { } 

istream_withassign& istream_withassign::operator=(istream& s)
{
	init(s.rdbuf()) ;
	return *this ;
	}

ostream_withassign::~ostream_withassign()  { } 

istream_withassign& istream_withassign::operator=(streambuf* sb)
{
	init(sb) ;
	return *this ;
	}

ostream_withassign::ostream_withassign() 
{
	// In order for the standard streams to be properly initialized
	// it is essential that nothing is done by the combination
	// of this constructor and ios::ios().  So we undo the effect of
	// ios::ios() 
	state = assign_private ;
	}

ostream_withassign& ostream_withassign::operator=(ostream& s)
{
	init(s.rdbuf()) ;
	return *this ;
	}

ostream_withassign& ostream_withassign::operator=(streambuf* sb)
{
	init(sb) ;
	return *this ;
	}

iostream_withassign::iostream_withassign() 
{
	// In order for the standard streams to be properly initialized
	// it is essential that nothing is done by the combination
	// of this constructor and ios::ios().  So we undo the effect of
	// ios::ios() 
	state = assign_private ;
	}

iostream_withassign::~iostream_withassign()  { } 


iostream_withassign& iostream_withassign::operator=(ios& s)
{
	init(s.rdbuf()) ;
	return *this ;
	}

iostream_withassign& iostream_withassign::operator=(streambuf* sb)
{
	init(sb) ;
	return *this ;
	}

void ios::uresize(int n)
{
	if ( n < nuser ) return ;
	ios_user_union* newu = new ios_user_union[n+1] ;
	for ( int x = 0 ; x < nuser ; ++x ) {
		newu[x] = x_user[x] ;
		} ;
	delete [] x_user ;
	nuser = n+1 ;
	x_user = newu ;
	}

long & ios::iword(int x)
{
	if ( x < 0 ) x = 0 ;
	if ( x >= nuser ) uresize(x) ;
	return x_user[x].i ;
	}

void* & ios::pword(int x)
{
	if ( x < 0 ) x = 0 ;
	if ( x >= nuser ) uresize(x) ;
	return x_user[x].p ;
	}


long ios::bitalloc()
{
	long w = nextbit ;
	nextbit = nextbit << 1 ;
	return w ;
	}

int ios::xalloc()
{
	return nextword++ ;
	}
