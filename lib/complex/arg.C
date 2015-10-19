/* @(#) arg.c 1.2 1/27/86 17:47:29 */
/*ident	"@(#)cfront:lib/complex/arg.c	1.2"*/
#include "complex.h"

double
arg(complex z)
{
	return atan2(z.im,z.re);
}
