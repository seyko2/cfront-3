/*ident	"@(#)cls4:Patch/dtors.c	1.3" */
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
/* __dtors() for the patch version of the C++ translator*/

struct __linkl {
    struct __linkl *next;
    int (*ctor)();
    int (*dtor)();
};

extern struct __linkl *__head;

void __dtors()
{
	static int one_time = 0;
	struct __linkl *ptr;
	if (one_time == 0)
	{
		one_time = 1;
		for (ptr = __head; ptr; ptr = ptr->next)
		{
			if (ptr->dtor)
				(*(ptr->dtor))();
		}
	}
}
