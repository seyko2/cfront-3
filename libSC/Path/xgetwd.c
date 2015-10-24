/*ident	"@(#)Path:xgetwd.c	3.1" */
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
#include <sys/param.h>
#include <stdlib.h>

#ifndef MAXPATHLEN
// hope this is big enough
#define MAXPATHLEN 1026
#endif

Path* Path::cachedwd = 0;

//#define GETWD

void Path::cachewd(int on) {
	char wd[MAXPATHLEN+1];  
#ifdef GETWD
	if (on && getwd(wd)) {
#else
	if (on && getcwd(wd, MAXPATHLEN)) {
#endif
		if (cachedwd == 0)
			cachedwd = new Path;
		*cachedwd = wd;
	}
	else {
		delete cachedwd;
		cachedwd = 0;
	}
}

Path Path::xgetwd()
{
	if (cachedwd)
		return *cachedwd;
	else {
		char wd[MAXPATHLEN+1];  // +1 is just to be safe
#ifdef GETWD
		if (getwd(wd) == 0)
		{
			if (Path::no_wd.raise(wd) == 0)
				die(wd);
			*wd = '/';
			wd[1] = '\0';
		}
#else
		getcwd(wd, MAXPATHLEN);
#endif
		return Path(wd);
	}
}
