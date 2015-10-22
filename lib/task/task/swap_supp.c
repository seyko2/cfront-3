/*ident	"@(#)cls4:lib/task/task/swap_supp.c	1.3" */
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

/* Functions with C linkage, for sswap to call, to avoid dependency
 * on internal name-encoding algorithm.  These functions call the ones
 * sswap really wants.
 */

extern "C" {
int* swap_call_new(int);
void swap_call_delete(int*);
}

int*
swap_call_new(int size)
{
	int*	p;
	p = new int[size];
	while (p == 0) object::task_error(E_STORE, (object*)0);
	return p;
}

void
swap_call_delete(int* p)
{
	delete p;
}
