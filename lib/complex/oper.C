/* @(#) oper.c 1.2 1/27/86 17:47:38 */
/*ident	"@(#)cfront:lib/complex/oper.c	1.2"*/
#include "complex.h"
#include "const.h"

complex
operator*(complex a1,complex a2) 
{
	return complex(a1.re*a2.re-a1.im*a2.im, a1.re*a2.im+a1.im*a2.re);
}


complex
operator/(complex a1, complex a2)
{
	register double r = a2.re;	/* (r,i) */
	register double i = a2.im;
	register double ti;		/* (tr,ti) */
	register double tr;

	tr = ABS(r);
	ti = ABS(i);

	if (tr <= ti) {
		ti = r/i;
		tr = i * (1 + ti*ti);
		r = a1.re;
		i = a1.im;
	}
	else {
		ti = -i/r;
		tr = r * (1 + ti*ti);
		r = -a1.im;
		i = a1.re;
	}

	return complex( (r*ti + i)/tr, (i*ti - r)/tr );
}

void
complex.operator*=(complex a)
{
	register double r = re*a.re - im*a.im;
	register double i = re*a.im + im*a.re;
	re = r;
	im = i;
}

void
complex.operator/=(complex a)
{
	complex quot, temp1, temp2;

	if ( (temp2.re = a.re) < 0 ) temp2.re = -temp2.re;
	if ( (temp2.im = a.im) < 0 ) temp2.im = -temp2.im;
	if ( temp2.re <= temp2.im) {
		temp2.im = a.re/a.im;
		temp2.re = a.im * (1 + temp2.im*temp2.im);
		temp1 = *this;
	}
	else {
		temp2.im = -a.im/a.re;
		temp2.re = a.re * (1 + temp2.im*temp2.im);
		temp1.re = -im;
		temp1.im = re;
	}
	re = (temp1.re * temp2.im + temp1.im) / temp2.re;
	im = (temp1.im * temp2.im - temp1.re) / temp2.re;
}
