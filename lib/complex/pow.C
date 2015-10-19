/* @(#) pow.c 1.2 1/27/86 17:47:40 */
/*ident	"@(#)cfront:lib/complex/pow.c	1.2"*/
#include "complex.h"

complex
pow(double base, complex z)
/*
	real to complex power: base**z.
*/
{
       /*register*/ complex y;
	
	if (base == 0) return y;	/* even for singularity */

	if (0 < base) {
		double lb = log(base);
		y.re = z.re * lb;
		y.im = z.im * lb;
		return exp(y);
	}
	
	return pow(complex(base), z);	/* use complex power fct */
}


complex
pow(complex a, int n)
/*
	complex to integer power: a**n.
*/
{
       /*register*/ complex x, p = 1;

	if (n == 0) return p;

	if (n < 0) {
		n = -n;
		x = 1/a;
	}
	else 	x = a;

	for( ; ; ) {
		if(n & 01) {
			register double t = p.re * x.re - p.im * x.im;
			p.im = p.re * x.im + p.im * x.re;
			p.re = t;
		}
		if(n >>= 1) {
			register double t = x.re * x.re - x.im * x.im;
			x.im = 2 * x.re * x.im;
			x.re = t;
		}
		else 	break;
	}
	return p;
}

complex
pow(complex a, double b)
/*
	complex to real power: a**b.
*/
{
	register double logr = log( abs(a) );
	register double logi = atan2(a.im, a.re);
	register double x = exp( b*logr );
	register double y = b * logi;
	return complex(x*cos(y), x*sin(y));
}


complex
pow(complex base, complex sup)
/*
	complex to complex power: base**sup.
*/
{
	complex result;
	register double	logr, logi;
	register double xx, yy;
	double a = abs(base);

	if (a == 0) return result;

	logr = log( a );
	logi = atan2(base.im, base.re);

	xx = exp( logr * sup.re - logi * sup.im );
	yy = logr * sup.im + logi * sup.re;

	result.re = xx * cos(yy);
	result.im = xx * sin(yy);

	return result;
}
