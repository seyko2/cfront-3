/*#ident	"@(#)cfront:lib/complex/cos.c	1.3"*/

#include "complex.h"
#include "const.h"

complex
cos(complex z)
/*
	The cosine of z: cos(z)=cosh(iz).
*/
{
	complex	y(-z.im, z.re);	
	return cosh(y);
}



complex
cosh(complex z)
/*
	The complex hyperbolic cosine of z
*/
{
	double 	cosh_x, sinh_x, cos_y, sin_y;
	c_exception ex( "cosh", z );
	int write( int , char *, unsigned int );

#define	COSH_GOOD	1e7

	if (ABS(z.im) > COSH_GOOD)
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
		ex.retval.im = (sin_y >= 0) ? HUGE : -HUGE;
		ex.retval.re = (cos_y >= 0) ? HUGE : -HUGE;
   		if ( !complex_error( ex ))
			errno = ERANGE;
		return ex.retval;
    	}
	else if (z.re < MIN_EXPONENT)
	{
		ex.type = OVERFLOW;
		ex.retval.im = (sin_y >= 0) ? -HUGE : HUGE;
		ex.retval.re = (cos_y >= 0) ? HUGE : -HUGE;
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

	return complex(cos_y*cosh_x, sin_y*sinh_x);
}
