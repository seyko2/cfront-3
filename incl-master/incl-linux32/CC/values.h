/*ident	"@(#)cls4:incl-master/proto-headers/values.h	1.1" */

/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
#ifndef __VALUES_H
#define __VALUES_H

#define BITS(type)	(BITSPERBYTE * (int)sizeof(type))
#define BITSPERBYTE	8

#define DMAXEXP	((1 << _DEXPLEN - 1) - 1 + _IEEE)
#define DMAXPOWTWO	((double)(1L << BITS(long) - 2) * (1L << DSIGNIF - BITS(long) + 1))
#define DMINEXP (-(DMAXEXP + DSIGNIF - _HIDDENBIT - 3))
#define DSIGNIF	(BITS(double) - _DEXPLEN + _HIDDENBIT - 1)

#define FMAXEXP	((1 << _FEXPLEN - 1) - 1 + _IEEE)
#define FMAXPOWTWO	((float)(1L << FSIGNIF - 1))
#define FMINEXP (-(FMAXEXP + FSIGNIF - _HIDDENBIT - 3))
#define FSIGNIF	(BITS(float)  - _FEXPLEN + _HIDDENBIT - 1)

#define HIBITI	(1 << BITS(int) - 1)
#define HIBITL	(1L << BITS(long) - 1)
#define HIBITS	((short)(1 << BITS(short) - 1))

#define H_PREC	(DSIGNIF % 2 ? (1L << DSIGNIF/2) * M_SQRT2 : 1L << DSIGNIF/2)

#define LN_MAXDOUBLE	(M_LN2 * DMAXEXP)
#define LN_MINDOUBLE	(M_LN2 * (DMINEXP - 1))

#define MAXDOUBLE       1.79769313486231470e+308
#define MAXFLOAT        ((float)3.40282346638528860e+38)
#define MAXINT		(~HIBITI)
#define MAXLONG		(~HIBITL)
#define MAXSHORT	((short)~HIBITS)
#define MINDOUBLE       4.94065645841246544e-324
#define MINFLOAT        ((float)1.40129846432481707e-45)
#define M_LN2		0.69314718055994530942
#define M_PI		3.14159265358979323846
#define M_SQRT2		1.41421356237309504880
#define X_EPS		(1.0/H_PREC)
#define X_PLOSS		((double)(long)(M_PI * H_PREC))
#define X_TLOSS		(M_PI * DMAXPOWTWO)
#define _DEXPLEN        11
#define _EXPBASE	(1 << _LENBASE)
#define _FEXPLEN	8
#define _HIDDENBIT      1
#define _IEEE           1
#define _LENBASE	1

#endif
