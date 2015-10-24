/*ident	"@(#)Time:tm/mystrdup.c	3.2" */
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

/*
 * return a copy of s using malloc
 */

#include <malloc.h>
#include <memory.h>
#include <string.h>

char* mystrdup_Time_ATTLC(const char *s){
	register char*	t;
	register int	n;

	return (char*)((t = new char[n = strlen(s) + 1]) ? memcpy(t, s, n) : 0);
}
