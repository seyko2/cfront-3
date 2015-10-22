/*ident	"@(#)cls4:lib/complex/complex/sin.c	1.3" */
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
sin(complex z)
/*
	sine of z: -i * sinh(i*z)
*/
{
	complex	y = complex(-z.im, z.re);	/*  i * z */
	y = sinh(y);				/*  csinh(y) */
	return complex(y.im, -y.re);		/*  -i * y */
}

complex
sinh(complex z)
/*
	The hyperbolic sine
*/
{
	double cosh_x, sinh_x, cos_y, sin_y;
	c_exception	ex( "sinh", z );

#define SINH_GOOD	1e7

	if (ABS(z.im) > SINH_GOOD)
	{ 
		ex.type = OVERFLOW;
		if ( !complex_error( ex ))
			errno = ERANGE;
    		return ex.retval;
    	}

    	cos_y = cos(z.im);
    	sin_y = sin(z.im);

	if (z.re > MAX_EXPONENT)
	{
		ex.type = OVERFLOW;
		ex.retval.re = (cos_y >= 0) ? HUGE : -HUGE;
		ex.retval.im = (sin_y >= 0) ? HUGE : -HUGE;
		if ( !complex_error( ex ))
			errno = ERANGE;
		return ex.retval;
    	}
	else if (z.re < MIN_EXPONENT)
	{
		ex.type = OVERFLOW;
		ex.retval.re = (cos_y >= 0) ? -HUGE : HUGE;
		ex.retval.im = (sin_y >= 0) ? HUGE : -HUGE;
		if ( !complex_error( ex ))
			errno = ERANGE;
		return ex.retval;
    	}
	else
	{
    		double pos_exp = exp(z.re); 
    		double neg_exp = 1/pos_exp;
   		cosh_x = (pos_exp + neg_exp)/2;
   		sinh_x = (pos_exp - neg_exp)/2;
    	}

	return complex(cos_y*sinh_x, sin_y*cosh_x);
}
