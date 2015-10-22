/*ident	"@(#)cls4:lib/complex/complex/abs.c	1.3" */
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
