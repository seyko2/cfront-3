/*ident	"@(#)fs:libsrc/placenew.c	3.1" */
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

#ifndef __GNUG__
#include <new.h>
#endif
#include "fslib.h"

void* operator new (size_t size, void* p)
{
	_fs_register_ATTLC(0, "?", -1, "?", size, -2, p);
	return p;
}
