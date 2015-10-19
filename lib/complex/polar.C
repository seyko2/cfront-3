/* @(#) polar.c 1.2 1/27/86 17:47:40 */
/*ident	"@(#)cfront:lib/complex/polar.c	1.2"*/
#include	"complex.h"

complex
polar(double r, double theta)
{
	return complex(r * cos(theta), r * sin(theta) );
}
