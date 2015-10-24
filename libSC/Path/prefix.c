/*ident	"@(#)Path:prefix.c	3.1" */
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
#include <memory.h>

int componentwise_prefix(const Path& p1, const Path& p2) {
	// shorter points to shorter of two strings.
	// if they're equal, points to either one.
	const char *shorter, *longer;
	int shortlen;
	int yesret;
	if (p1.length() < p2.length()) {
		shortlen = p1.length();
		shorter = (String)p1;
		longer = (String)p2;
		yesret = -1;
	}
	else {
		shortlen = p2.length();
		shorter = (String)p2;
		longer = (String)p1;
		yesret = 1;
	}
	if (shorter[1] == '\0') {
		// the path "." is a componentwise prefix of every relative path
		if (*shorter == '.' && *longer != '/') {
			return yesret;
		}
		// the path "/" is a componentwise prefix of every absolute path
		if (*shorter == '/' && *longer == '/') {
			return yesret;
		}
	}
	if (memcmp(shorter, longer, shortlen) == 0) {
		char next = longer[shortlen];
		if (next == '\0' || next == '/') {
			return yesret;
		}
	}
	return 0;
}
