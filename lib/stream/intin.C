/*ident	"@(#)cls4:lib/stream/intin.c	1.4" */
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

/****************
 * 
 *	This file contains the extraction operations for integer input. 
 *	It suffers from several flaws that ought to be fixed. 
 *
 * 	All other versions depend on the extractor for longs to
 *	do the real work.  This is OK when int==long but on a machine
 * 	where int!=long there will be a large performance penalty.
 *
 *  	Overflow detection is limited to cases where the long is
 *	shortened and changes its value.  This misses a lot of cases.
 *
 ***************/


#define ISTREAM istream

static const int a10 = 'a'-10;
static const int A10 = 'A'-10;

istream& ISTREAM::operator>>(long& i)
{
	// This code ignores overflows.  This ought to be fixed
	// in some future version.

	if ( !ipfx() ) return *this ;

	register int	c;
	register int	base	;

	switch ( flags()&(ios::dec|ios::hex|ios::oct) ) {
		case ios::hex :	base = 16 ; break ;
	  	case ios::oct :	base = 8 ; break ;
		case ios::dec :	base = 10 ; break ;
		default :		base = 0 ; break ;
		}

	register streambuf *nbp	= bp;	// put bp in a reg for efficiency
	register int	x	= 0 ;	// how many chars are processed 
	register int	neg	= 0;	// set to '-' for negative number

	switch (c = nbp->sgetc()) {
	case '-':
		neg = 1 ;
		c = nbp->snextc(); ++x ;
		break;
	case '+':
		c = nbp->snextc(); ++x ;
		break;
	case EOF:
		setstate(failbit|eofbit) ;
		break ;
	}

	if ( base==0 && c=='0' ) {
		c = nbp->snextc() ; ++x ;

		if ( c=='x' || c=='X' ) {
			base = 16 ;
			c = nbp->snextc() ; x = 0;
			}
		else {
			base = 8 ;
		}
	} else if ( base==0 ) base = 10 ;

	// for efficiency we have similar loops
	// Note that when we reach this point c has already been set to
	// the first char of the string to be converted. 

	if ( base==10 ) {
		register long ii = 0;
		for( ; isdigit(c) ; c=nbp->snextc(),++x ) {
			
			/* accumulate as negative to avoid problems
			 * with biggest negative integer on
		 	 * 2's complement machines
			 */
			ii = ii*10-(c-'0');
		}
		i = neg ? ii : -ii;
	} else if ( base < 10 ) {
		register unsigned long ii = 0;
		for( ; isdigit(c)  ; c=nbp->snextc(),++x ) {
			/* accumulate as unsigned */
			register int diff = c-'0' ;
			if ( diff >= base ) break ;
			ii = ii*base+diff ;
		}
		i = neg ? -(long)ii : (long)ii;
	} else if ( base>10 ) { /* hex like base */
		register unsigned long ii = 0;
		/* accumulate as unsigned */
		for( ; isxdigit(c) ; c=nbp->snextc(),++x ) {
			register int diff ;
			if ( isdigit(c) )	diff = (c-'0');
			else if ( isupper(c) )	diff = (c-A10);
			else			diff = (c-a10);
			if ( diff >= base ) break ;
			ii = ii*base+diff ;
		}
		i = neg ? -(long)ii : (long)ii;
	}
	

	if (x == 0 ) setstate(failbit) ;
				// Correct treatment of this case
				// (i.e. no correct digits) 
				// is unclear. Making it an error
				// avoids certain infinite loops.
			
	return *this;
}

istream& ISTREAM::operator>>(int& i)
{
	long l;
	
	if ( !ipfx() ) return *this ;

	*this>>l ;
	if ( good() ) {
		i = (int)l ;
		if  ( i != l ) {
			// overflow
			setstate(failbit) ;
			}
		} 
	return *this;
}

istream& ISTREAM::operator>>(short& i)
{
	long l;

	if (!ipfx() ) return *this ;

	*this>>l ;
	if ( good() ) {
		i = (short)l ;
		if  ( i != l ) {
			// overflow
			setstate(failbit) ;
			}
		} 

	return *this;
}


// The following routines deal with unsigned by reading a long and
// copying.  This is certainly safe for "shorts", but is
// slightly problematic for ints and longs.  It works on "common"
// machines.


istream& ISTREAM::operator>>(unsigned short& u)
{
	long l ;
	*this >> l ;
	if ( good() ) {
		u = (unsigned short)l ;
		if  ( u != l ) {
			// overflow
			setstate(failbit) ;
			}
		} 
	return *this ;
	} 

istream& ISTREAM::operator>>(unsigned int& u)
{
	long l ;
	*this >> l ;
	if ( good() ) {
		u = (unsigned int)l ;
		if  ( u != l ) {
			// overflow
			setstate(failbit) ;
			}
		} 
	return *this ;
	} 

istream& ISTREAM::operator>>(unsigned long& u)
{
	long l ;
	*this >> l ;
	if ( good() ) {
		u = (unsigned long)l ;
		if  ( u != l ) {
			// overflow
			setstate(failbit) ;
			}
		} 
	return *this ;
	} 

