/*ident	"@(#)cls4:lib/complex/complex/sqrt.c	1.3" */
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


#define	SQRT_DANGER	1e17
# define PERIL(t)	(t > SQRT_DANGER || (t < 1/SQRT_DANGER && t != 0))

/*
 *
 * 7-25-83, note from Leonie Rose -
 * Stu Feldman says that the peril tests for the following function
 * are "acceptable" for now, but certain things like
 * sqrt(1e10 + 1e-30*i) will cause floating exceptions.
 *
 */

complex
sqrt(complex z)
{
	complex  answer;
	double  r_old,  partial;

/*
	Check for possible overflow, and fixup if necessary.
*/

	double x = abs(z.re);
	double y = abs(z.im);

	if (x > y && PERIL(x)) {
     		z.im /= x;
    		z.re /= x;  /* z.re is replaced by 1 or -1 */
     		partial = sqrt(x);
     	}
	else if PERIL(y) { 
     		z.im /= y;  /* roles of z.re, z.im reversed from previous */
     		z.re /= y;
     		partial = sqrt(y);
     	}
	else partial = 1;

/*
	Main computation:
	Use half angle formulas to compute angular part of the square root.
	The sign of sin_old is the same as that for sin_new, which means that the
	upper half plane gets mapped to the first quadrant, and
	the lower half plane to the fourth quandrant.
*/


	if (r_old = sqrt(z.re*z.re + z.im*z.im)) {
    		double r_new = partial * sqrt(r_old);

    		double cos_old = z.re/r_old;
    		double sin_old = z.im/r_old;
    		double cos_new = sqrt( (1 + cos_old)/2 );
   		double sin_new = (cos_new == 0)? 1 : sin_old/(2*cos_new);

    		answer.re = r_new * cos_new;
    		answer.im = r_new * sin_new;
    	}

	return answer;
}
