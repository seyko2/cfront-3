/*ident	"@(#)cls4:Patch/_main.c	1.3" */
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
/* _main for the patch version of the C++ translator*/
/* See patch.c for details*/

struct __linkl {
    struct __linkl *next;
    char (*ctor)();
    char (*dtor)();
};

struct __linkl	* __head = 0;

void _main()
{
    struct __linkl *current = __head;
    struct __linkl *previous = 0;

    while (current)
    {
	struct __linkl *next = current->next;

	if (current->ctor)
		(*(current->ctor))();

	current->next = previous;
	previous = current;
	current = next;
    }

    __head = previous;
}
