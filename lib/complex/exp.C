/*#ident	"@(#)cfront:lib/complex/exp.c	1.3"*/

#include "complex.h"
#include "const.h"


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
	int write( int , char *, unsigned int );

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
