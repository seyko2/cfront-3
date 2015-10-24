/*ident	"@(#)Path:strdup.c	3.1" */
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

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>

extern "C" {
	char *strdup_ATTLC(const char *);
}

char *strdup_ATTLC(const char *s)
{
	size_t n = strlen(s)+1;
	char *t = (char*)malloc(n);
	if (t != 0)
		memcpy(t, s, n);
	return t;
}
