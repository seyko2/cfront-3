/* @(#) abs.c 1.2 1/27/86 17:47:28 */
/*ident	"@(#)cfront:lib/complex/abs.c	1.2"*/
#include "complex.h"
#include "const.h"

double
abs(complex z)
{
	register double temp;
	register double r = z.re;
	register double i = z.im;
	
	if (r < 0) r = -r;
	if (i == 0) return r;

	if (i < 0) i = -i;
	if (r == 0) return i;

	if (i > r) { temp = r; r = i; i = temp; }
	temp = i/r;
	temp = r*sqrt(1.0 + temp*temp);  /*overflow!!*/
	return temp;
}

double
norm(complex z)
{
	register double r = z.re;
	register double i = z.im;

#define	SAFE 6.521908912666391000e+18  /* 0.5*sqrt(GREATEST)*/

	if (r<SAFE && i<SAFE) return r*r+i*i;
	return GREATEST;
}
