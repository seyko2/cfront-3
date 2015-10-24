/*ident	"@(#)Path:slash.c	3.1" */
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

Path operator/(const Path & s, const Path & t)
{
	String tmp = s;
	tmp += '/';
	tmp += (String)t;
	return Path(tmp);
}

Path & Path::operator/=(const Path & t)
{
	rep += '/';
	rep += t.rep;
	canonicalize();
	return *this;
}

