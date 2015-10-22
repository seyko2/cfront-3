/*ident	"@(#)cls4:lib/complex/complex/io.c	1.3" */
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
#include <complex.h>

ostream&
operator<<(ostream& s, complex a) 
{
	return s<<"( "<<real(a)<<", "<<imag(a)<<")";
}

istream&
operator>>(istream& s, complex& a)
/*
	f
	( f )
	( f , f )
*/
{
	double	re = 0, im = 0;
	char 	c = 0;

	s>>c;
	if (c == '(') {
		s>>re>>c;
		if (c == ',') s>>im>>c;
		if (c != ')') s.clear(ios::badbit);
	}
	else {
		s.putback(c);
		s>>re;
	}

	if (s) a = complex(re,im);
	return s;
}
