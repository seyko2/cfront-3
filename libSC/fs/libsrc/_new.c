/*ident	"@(#)fs:libsrc/_new.c	3.1" */
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

#include <malloc.h>
#include <stdlib.h>
#include <stddef.h>
#include "fslib.h"

typedef void (*PFVV)();
extern PFVV _new_handler;

/* New it, then register it.
*/
void *operator new(size_t size)
{
	return _fs_register_ATTLC(0, "?", -1, "?", size, -2, _new(size));
}

/* This is `lib/new/_new.c`operator new
*/
extern void *_new(size_t size)
{
	void *_last_allocation;
	while ((_last_allocation=malloc(size))==0) {
		if (_new_handler && size)
			(*_new_handler)();
		else
			return 0;
	}
	return _last_allocation;
}
