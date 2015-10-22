/*ident	"@(#)cls4:lib/complex/complex/log.c	1.3" */
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

#define	LOGDANGER	1e18
#define PERIL(t) (t > LOGDANGER || (t < 1/LOGDANGER && t != 0) )

complex
log(complex z)
/*
	The complex natural logarithm of "z".
	If z = 0, then the answer LOGWILD + 0*i is returned.

	Stu Feldman says that the peril tests for the following function
	are "acceptable for now", but certain things like
	complex variables outside the over/underflow range 
	will cause floating exceptions.
*/
{
	complex	answer;	
	double  partial;
	c_exception ex( "log", z );

	if ( z.re == 0 && z.im == 0)
	{
		ex.type = SING;
		ex.retval.re = HUGE;
		if ( !complex_error( ex ))
		{
			(void) write( 2, "log: singularity: log((0,0))\n", 29 );
			errno = EDOM;
		}
    		return ex.retval;
    	}

	/*
     		 Check for (over/under)flow, and fixup if necessary.
	*/
	double x = ABS(z.re);
	double y = ABS(z.im);

	if ( x>y && PERIL(x) ) {
		z.im /=x;
		z.re /= x;  /* z.re is replaced by 1 or -1 */
		partial = log(x);
	}
	else if (PERIL(y)) {
		z.im /= y;  /* roles of re, im reversed from previous */
 		z.re /= y;
		partial = log(y);
	}
	else partial = 0;

	/*
		z.re*z.re and z.im*z.im should not cause problems now.
	*/
    
	answer.im = atan2(z.im,z.re); 
	answer.re = log(z.re*z.re + z.im*z.im)/2 + partial;
	return answer;
}
