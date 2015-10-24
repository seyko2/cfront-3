/*ident	"@(#)Path:explode.c	3.1" */
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
#include <List.h>

void Path::explode(List<Path> & ret) const
{
	ret.make_empty();
	if (rep != ".")
	{
		if (is_absolute())
			ret.put("/");
		char *q = strdup_ATTLC(rep);
		static const char *separator = "/";
		for (const char *f = strtok(q, separator); f != 0; f = strtok(0, separator))
			ret.put(f);
		free(q);
	}
	// not really necessary, since put doesn't change current position
	// ret.reset();
}
