/*ident	"@(#)Path:dirname.c	3.1" */
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

#include "Pathlib.h"

Path Path::dirname() const
{
	int i = rep.strrchr('/');

		// the i+1 is actually a little trickier than 
		// it first appears, but it does work.
	String s = rep.chunk(0, i+1);

	return Path(s);
}

String Path::basename() const
{
	int i = rep.strrchr('/');
	return rep.chunk(i+1);
}

String Path::basename(const char * suffix) const
{
	String s = basename();

	int j = s.length() - strlen(suffix);
	const char * sp = (const char *)s + j;
	if (j >= 0 && strcmp(sp, suffix) == 0) // if j < 0, suffix can't match
		s(j) = "";

	return s;
}

// This is problematic.  What is the extension of foo..c?  Of foo.b.c?
//
//String Path::extension() const
//{
//	int i = rep.strrchr('.');
//	if (i < 0)
//		return "";
//	else
//		return rep.chunk(i+1);
//}	
