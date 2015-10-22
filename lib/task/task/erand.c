/*ident	"@(#)cls4:lib/task/task/erand.c	1.3" */
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
#include <task.h>
#include <math.h>
#include "hw_stack.h"

/* NOTE:  This function is in a separate file because it needs the
 * math library. Programs that bring in this file must link with -lm
 */
int
erand::draw()
{
	return (int)(-mean * log( (double)(MAXINT_AS_FLOAT-randint::draw())
				/ MAXINT_AS_FLOAT) + .5);
}

