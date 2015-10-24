/*ident	"@(#)Path:path.c	3.1" */
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

// The algorithms to find the login directory are so complicated 
// (see /home/tools/ksh/src/cmd/ksh/shlib/tilde.c),
// that I think I will be cutting my throat if I try to
// incorporate them in this library.  Better just to
// make the user do something reasonable herself.
//Path Path::tilde(const char * login)
//{
//	String s = "~";
//	if (login)
//		s += login;
//	return Path(s);
//}

Path::Path() : rep(".")
{
}

Path::Path(const char * f) : rep(f)
{
	canonicalize();
}

Path::Path(const String & s) : rep(s)
{
	canonicalize();
}

Path::Path(const Path & f) : rep(f.rep)
{
}

void Path::build_from(const char * dir, const char * base, const char * ext)
{
	rep = dir;
	char c;
	if (rep.lastX(c) && c != '/')
		rep += '/';
	if (base)
		rep += base;
	if (ext)
		rep += ext;
	canonicalize();
}

Path::Path(const char * dir, const char * base, const char * ext)
{
	build_from(dir, base, ext);
}


int Path::length() const
{
	return rep.length();
}

int Path::ncomponents() const
{
	const char *p = rep;
	if (p[1] == '\0')
	{
		if (*p == '.')
			return 0;
		else if (*p == '/')
			return 1;
	}
	int n = 1;
	while (*p != '\0')
	{
		if (*p++ == '/')
			n++;
	}
	return n;
}

int Path::is_absolute() const
{
	return (rep.char_at(0) == '/');
}
	
int Path::is_relative() const
{
	return !is_absolute();
}

Path::operator const char *() const
{
	return rep;
}

Path::operator String() const
{
	return rep;
}

Path & Path::operator=(const Path & t)
{
	rep = t.rep;
	return *this;
}

int operator<(const Path & p1, const Path & p2)
{
	return String(p1) < String(p2);
}

int operator==(const Path & p1, const Path & p2)
{
	return (String)p1 == (String)p2;
}

int operator!=(const Path & p1, const Path & p2)
{
	return (String)p1 != (String)p2;
}

