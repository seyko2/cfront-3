/*ident	"@(#)cls4:lib/complex/complex/exp.c	1.3" */
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
#include "const.h"
#include <osfcn.h>

complex
exp(complex z)
/*
	The complex exponentiation function: e**z, e being 2.718281828...

	In case of overflow, return ( HUGE, HUGE ).
	In case of underflow return 0.
	In case of ridiculous input to "sin" and "cos", return 0.
*/
{
	complex	answer;
	double 	radius, sin_theta, cos_theta;
	c_exception ex( "exp", z );

#define EXPGOOD	1e7

	if (z.im > EXPGOOD || z.im < -EXPGOOD)
	{
		ex.type = OVERFLOW;
		if ( !complex_error( ex ))
			errno = ERANGE;
		return( ex.retval );
	}

	if (z.re < MIN_EXPONENT)
	{
		ex.type = UNDERFLOW;
		if ( !complex_error( ex ))
			errno = ERANGE;
   		return( ex.retval );
   	}

	sin_theta = sin(z.im);
	cos_theta = cos(z.im);

	if (z.re > MAX_EXPONENT)
	{
		ex.type = OVERFLOW;
		ex.retval.re = (cos_theta > 0) ? HUGE : -HUGE;
		ex.retval.im = (sin_theta > 0) ? HUGE : -HUGE;
		if ( !complex_error( ex ))
			errno = ERANGE;
   		return( ex.retval );
    	}
	else
    		radius = exp(z.re);

	answer.re = radius * cos_theta;
	answer.im = radius * sin_theta;
	return answer;
}
