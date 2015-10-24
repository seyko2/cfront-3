/*ident	"@(#)Path:Pathlib.h	3.1" */
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

#include <Path.h>
#include <ksh_test.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <malloc.h>
#include <assert.h>

extern "C" {
	char *strdup_ATTLC(const char*);

	// I stole the following ksh routine, and added the callback functions.
	int path_expand_Path_ATTLC(const char *, 
		void (*found)(const char *), 
		void (*poolcreate)(size_t, size_t), 
		char * (*alloc)(), 
		void (*shrink)(char *, size_t));
}
