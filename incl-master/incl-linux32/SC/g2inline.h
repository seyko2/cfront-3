/*ident	"@(#)G2++:incl/g2inline.h	3.1" */
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

#ifndef G2INLINEH
#define G2INLINEH

#include <g2mach.h>
#include <g2io.h>

static int gcd( int m, int n)
{
    int i;

    if( m < n)
	return gcd(n,m);

    if( i = m %n)
	return gcd(n,i);

    return n;
}

static inline int lcm(int m, int n)
{ 
    return (m * n)/gcd(m,n);
}

static inline int align(int offset, int align)
{
    return ((offset)+align-1)/align*align;
}

static inline int alignof(int ival)
{
    return( ival==LONG_INT_ATTLC ? (LONG_ALIGN_ATTLC) :
	        (ival==STRING_INT_ATTLC ? (STRING_ALIGN_ATTLC) :
		    (ival==CHAR_INT_ATTLC ? (CHAR_ALIGN_ATTLC) :
		        (ival==SHORT_INT_ATTLC ? (SHORT_ALIGN_ATTLC) : (0)
			)
	    	    )
		)
    	  );
}

#endif
