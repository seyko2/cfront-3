/*ident	"@(#)fs:libsrc/_delete.c	3.1" */
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
#include "fslib.h"

typedef void (*PFVV)();
extern PFVV _new_handler;

/* Deregister it, then delete it.
*/
extern void operator delete(void *p)
{
	_fs_unregister_ATTLC(0, 0, p);
	_delete(p);
}

/* This is `lib/new/_delete.c`operator delete
*/
extern void _delete(void *p)
{
	if (p) free((char*)p);
}

