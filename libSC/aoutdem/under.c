/*ident	"@(#)aoutdem:under.c	3.1" */
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

#include "aoutdem.h"

/* This function returns true if this symbol table entry type 
* has an extra prepended underscore.
*/

int hasextra_(int 
#ifndef SYSV
		entryType
#endif
			)
{
#ifdef SYSV
#if EXTRA__
	return 1;
#else
	return 0;
#endif
#else
	return ((entryType|N_EXT) == (N_TEXT|N_EXT) ||
		(entryType|N_EXT) == (N_DATA|N_EXT) ||
		(entryType|N_EXT) == (N_BSS|N_EXT));
#endif
}
