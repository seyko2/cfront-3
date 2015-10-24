/*ident	"@(#)Path:incl/Tmppath.h	3.1" */
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

#ifndef _TMPPATH_H
#define _TMPPATH_H 1

#include <Path.h>

class Tmppath : public Path {
public:

// static members
	static void use();
	static void use(const char * pfx);
	static void use(const Path & dir);
	static void use(const Path & dir, const char * pfx);
	static void usepid(int pid);

// constructors
	Tmppath();
	Tmppath(const char * pfx, const char * ext = "");
	Tmppath(const Path & dir);
	Tmppath(const Path & dir, const char * pfx, const char * ext = "");

// destructor
	~Tmppath() {}

private:
	// real constructors.  constructors above are
	// mapped to these based upon use() settings.
	void construct(const char * pfx, const char * ext);
	void construct(const Path & dir, const char * pfx, const char * ext);

	int build(const char *dir, const char *pfx, const char *ext);
};

#endif
