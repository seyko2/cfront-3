/*ident	"@(#)cls4:lib/stream/out.c	1.5" */
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

#define OSTREAM ostream

const int basebits = ios::dec|ios::oct|ios::hex ;

// At the cost of 100 bytes of table we can measurably speed
// up conversion (at least on a VAX)

static char digit1[] = {
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	'0','1','2','3','4','5','6','7','8','9',
	} ;

static char digit2[] = {
	'0','0','0','0','0','0','0','0','0','0',
	'1','1','1','1','1','1','1','1','1','1',
	'2','2','2','2','2','2','2','2','2','2',
	'3','3','3','3','3','3','3','3','3','3',
	'4','4','4','4','4','4','4','4','4','4',
	'5','5','5','5','5','5','5','5','5','5',
	'6','6','6','6','6','6','6','6','6','6',
	'7','7','7','7','7','7','7','7','7','7',
	'8','8','8','8','8','8','8','8','8','8',
	'9','9','9','9','9','9','9','9','9','9',
	} ;

static char* conv10(long i, char* bufend) 
/* p points to right end of a buffer. Function function returns
 * pointer to left end of converted number.  Number is not 0 terminated.
 *
 * Special care with negatives to avoid problems with
 * biggest negative number on 2's complement machines
 */
{
	register long j = i ;
	register char* p = bufend ;

	/* Above entered low order digit or 0 if i is zero */

	if ( j >= 0 ) {
		register int diff ;
		do {
			register long by100 = j/100 ;
			diff = (int)(j-100*by100) ;
			*p-- = digit1[diff] ;
			*p-- = digit2[diff] ;
			j = by100;
		} while ( j > 0 ) ;
		if ( diff<10 ) ++p ; //compensate for extra 0
	} else { // j < 0
		register int diff ;
		do {
			register long by100 = j/100 ;
			diff = (int)(100*by100-j) ;
			*p-- = digit1[diff] ;
			*p-- = digit2[diff] ;
			j = by100;
		} while ( j < 0 ) ;
		if ( diff<10 ) ++p ; //compensate for extra 0
	}
	return p+1 ;

}

static char* uconv10(unsigned long i, char* bufend) 
/* Same interface as conv10 except unsigned so we don't have
 * to worry about negatives */
{
	register unsigned long j = i ;
	register char* p = bufend ;
	register int diff ;

	do {
		register long by100 = j/100 ;
		diff = (int)(j-100*by100) ;
		*p-- = digit1[diff] ;
		*p-- = digit2[diff] ;
		j = by100;
	} while ( j > 0 ) ;
	if ( diff<10 ) ++p ; //compensate for extra 0
	
	return p+1 ;

}


static char* conv8(register unsigned long i, register char* p) 
{
	do {
		*p-- = (char)('0' + i%8) ;
		} while ( (i >>= 3) > 0 ) ;
	return p+1 ;
}

static char* conv16(register unsigned long i, register char* p) 
{
	do {
		register int dig = (int)(i%16) ;
		if ( dig < 10 )	*p-- = (char)('0' + i%16) ;
		else		*p-- = (char)('a'-10 + dig) ;

		} while ( (i >>= 4) > 0 ) ;
	return p+1 ;
}


static char* conv16u(register unsigned long i, register char* p) 
{
	do {
		register int dig = (int)(i%16) ;
		if ( dig < 10 )	*p-- = (char)('0' + i%16) ;
		else		*p-- = (char)('A'-10 + dig) ;

		} while ( (i >>= 4) > 0 ) ;
	return p+1 ;
}
ostream& OSTREAM::operator<<(const char* s)
{
	// I play some games so that if BREAKEVEN is <= 0 all
	// tests get set the right way at compile time 

#       if BREAKEVEN > 0
		static int	avglen = BREAKEVEN ;
					// running average of the lengths
					// of strings ;
#	else
		static const int avglen = BREAKEVEN ;
					// fixed constant so all tests
					// are fixed at compile time
#	endif

	register int fwidth = width(0) ;

	if (!opfx() ) return *this ;
	if ( s==0 ) return *this;
	register streambuf* nbp = bp ;
	register const char* p ;
	register int len ;
	register int pad ;

	register int leftjust = ( (flags()&left) != 0 ) ;
	if ( BREAKEVEN<0 
			|| BREAKEVEN>0 && avglen<=BREAKEVEN &&
			   (fwidth==0 || leftjust)){
		p = s ;
		while ( *p ) {
			if ( nbp->sputc(*p++) == EOF ) {
				setstate(badbit) ;
				break ;
			}
		}
		len = p-s ;
		pad = fwidth-len ;
	} else {
		len = strlen(s) ;
		pad = fwidth-len ;
		if ( pad>0 && !leftjust ) {
			while ( pad-- > 0 ) {
				if ( nbp->sputc(fill()) == EOF ) {
					setstate(badbit) ;
				}
			}
		}
		write(s,len) ;
	}

	if ( pad > 0  ) {
		while ( pad-- > 0 ) {
			if ( nbp->sputc(fill()) == EOF ) setstate(badbit) ;
		}
	}

 	if ( BREAKEVEN > 0 ) { // will be eliminated at compile time 
		avglen = (3*avglen + len) >> 2;
	}
	osfx() ;
	return *this;
}

static int dofield(
	ostream* ios,
	register char* pfx,
	int pwidth,
	register char* sfx,
	int swidth) 
{
	register streambuf* b = ios->rdbuf() ;
	register int w = ios->width(0)-(pwidth+swidth) ;
	register int f = (int)ios->flags() ;
	register int fchar = ios->fill() ; 

	if ( (f&ios::right) || !(f&(ios::left|ios::internal)) ) {
		while ( w-- > 0 ) {
			if ( b->sputc(fchar) == EOF ) return ios::badbit ;
			}
		}

	while ( *pfx ) {
		if ( b->sputc(*pfx++) == EOF ) return ios::badbit ;
		}

	if ( f&ios::internal ) {
		while ( w-- > 0 ) {
			if ( b->sputc(fchar) == EOF ) return ios::badbit ;
			}
		}
	while ( *sfx ) {
		if ( b->sputc(*sfx++) == EOF ) return ios::badbit ;
		}

	while ( w-- > 0 ) {
		if ( b->sputc(fchar) == EOF ) return ios::badbit ;
		}

	return 0 ;
	}

static const int dbufsize = 32 ;

ostream& OSTREAM::operator<<(long i)
{
	if (!opfx()) {
		width(0) ;
		return *this;
		}
	char buf[dbufsize];

	register char *p ;
	register char* pfx = "" ;
	register int pfxsize = 0 ;

	buf[dbufsize-1] = 0 ;
	switch( flags()&basebits ) {
		case ios::oct :
			p = conv8(i,&buf[dbufsize-2]) ;
			if ( (flags()&showbase) ) {	// && i removed
				pfx = "0" ; pfxsize = 1 ;
				}
			break ;
		case ios::hex :
			if ( flags()&uppercase ) {
				p=conv16u(i,&buf[dbufsize-2]);
				if ( flags()&showbase ) {
					pfx = "0X" ; pfxsize = 2 ;
					}
				}
			else {
				p=conv16(i,&buf[dbufsize-2]); 
				if ( flags()&showbase ) {
					pfx = "0x" ; pfxsize = 2 ;
					}
				}
			break ;
		default:
			p = conv10(i,&buf[dbufsize-2]) ;
			if ( i < 0 ) {
				pfx = "-" ; pfxsize = 1 ;
				}
			else if ( flags()&showpos ) {
				pfx = "+" ; pfxsize = 1 ;
				}
			break ;
		}
	register int err ;
	if ( err = dofield(this,pfx,pfxsize,p,&buf[dbufsize-1]-p) ) {
		setstate(err) ;
		}
	osfx() ;
	return *this ;
	}

ostream& OSTREAM::operator<<(unsigned long i)
{
	if (!opfx()) {
		width(0) ;
		return *this;
		}
	char buf[dbufsize];
	register char *p ;
	register char* pfx = "" ;
	register int pfxsize = 0 ;


	buf[dbufsize-1] = 0 ;
	switch( flags()&basebits ) {
		case ios::oct :
			p = conv8(i,&buf[dbufsize-2]) ;
			if ( (flags()&showbase) && i ) {
				pfx = "0" ; pfxsize = 1 ;
				}
			break ;
		case ios::hex :
			if ( flags()&uppercase ) {
				p=conv16u(i,&buf[dbufsize-2]);
				if ( flags()&showbase ) {
					pfx = "0X" ; pfxsize = 2 ;
					}
				}
			else {
				p=conv16(i,&buf[dbufsize-2]); 
				if ( flags()&showbase ) {
					pfx = "0x" ; pfxsize = 2 ;
					}
				}
			break ;
		default:
			p = uconv10(i,&buf[dbufsize-2]) ;
			break ;
		}
	register int err ;
	if ( err = dofield(this,pfx,pfxsize,p,&buf[dbufsize-1]-p) ) {
		setstate(err) ;
		}
	osfx() ;
	return *this ;
	}
	
ostream& OSTREAM::operator<<(register streambuf* b)
{
	register streambuf* nbp = bp;
	register int c;

	if (!opfx()) return *this;
	if ( !b ) {
		setstate(failbit) ;
		return *this ;
		}
	c = b->sgetc();
	while (c != EOF) {
		if (nbp->sputc(c) == EOF) {
			setstate(badbit) ;
			break;
			}
		c = b->snextc();
		}

	osfx() ;		
	return *this;
	}

ostream& OSTREAM::operator<<( void* p)
{
	long f = setf(ios::showbase|PTRBASE,  basebits|ios::showbase) ;
	*this << (long)p ;
	setf(f,~0) ;
	return *this ;
	}

/*  add this later
ostream& OSTREAM::operator<<( const void* p)
{
	long f = setf(ios::showbase|PTRBASE,  basebits|ios::showbase) ;
	*this << (long)p ;
	setf(f,~0) ;
	return *this ;
	}
*/

ostream& OSTREAM::operator<<(int x)
{
	*this << (long)x ;
	return *this ;
	}

ostream& OSTREAM::operator<<(unsigned int x)
{
	*this << (unsigned long)x ;
	return *this ;
	}

ostream& OSTREAM::complicated_put(char c)
{
	if ( opfx() ) {
		if (  bp->sputc(c) == EOF )  {
			setstate(eofbit|failbit) ;
		}
		osfx() ;
	}
	return *this ;
}
ostream& OSTREAM::ls_complicated(char c)
{
	put(c) ; osfx() ; return *this ;
}
ostream& OSTREAM::ls_complicated(unsigned char c) 
{
	put(c) ; osfx() ; return *this ;
}
