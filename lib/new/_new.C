/*ident	"@(#)cls4:lib/new/_new.c	1.3" */
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

typedef void (*PFVV)();
extern PFVV _new_handler;

extern "C" {
    extern char* malloc(unsigned);
}

#include <new.h>

extern void* operator new(size_t size)
{
	if (size == 0) size = 1;

	void* _last_allocation;

	while ( (_last_allocation=malloc(unsigned(size)))==0 ) {
		if(_new_handler && size)
			(*_new_handler)();
		else
			return 0;
	}
	return _last_allocation;
}
