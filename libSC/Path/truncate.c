/*ident	"@(#)Path:truncate.c	3.1" */
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
	
// this is inefficient.  i should do the truncation in place,
// but I don't have access to String internal rep.
//
int Path::truncate_components(int len) 
{
	int truncated = 0;
	if (len < 2) len = 2;

	char *buf = new char[rep.length() + 1];
	rep.dump(buf);
	
	char *put = buf;
	int cnt = 0;
	for (const char *p = buf; *p; p++)
	{
		if (*p == '/')
			cnt = 0;
		else
			cnt++;
		if (cnt <= len)
			*put++ = *p;
		else
			truncated = 1;
	}
	assert(put > buf);  // shortest Path is "."
	*put = '\0';

	rep.assign(buf, put-buf);
	delete buf;

	return truncated;
}
