/*ident	"@(#)Args:Argsarg.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#include "Argslib.h"

const char* 
Args::arg(int i) const {
	if (i >= nargs_ || i < 0)
		return 0;
	Arg *arg;
	for (arg = firstarg; i--; arg = arg->next);
	return arg->p;
}
