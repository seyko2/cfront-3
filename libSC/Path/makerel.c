/*ident	"@(#)Path:makerel.c	3.1" */
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
#include <Pool.h>

Path Path::absolute_version() const
{
	if (is_absolute())
		return *this;
	else
		return xgetwd() / *this;
}

/* Relativize file name f wrt directory wd.
*  Example:
*	If f is				/a/b/c/d/fooo/bar/
*	and wd is			/a/b/c/d/foo/bar/baz/
*	then relativized f is		../../../fooo/bar
*
*  ON ENTRY: f and wd are both absolute and canonical, except that each has a trailing / appended
*/
static void relativize(String & f, const String & wd)
{
		// find the common prefix
	int i;
	for (i = f.match(wd) - 1; wd.char_at(i) != '/'; i--)
		;
		// common prefix is 0..i
		// save fooo/bar portion of absolute file name
	String save = f.chunk(i+1);
		// now count the number of slashes after position i in wd, 
		// and lay down that many ..s
	f.make_empty();
	for (i++; i < wd.length(); i++)
		if (wd.char_at(i) == '/')
			f += "../";
		// append fooo/bar
	f += save;
		// last character must be /, kill it
	f.unput();
		// change empty relative path to "."
	if (f.length() == 0)
		f.assign(".", 1);
}

Path Path::relative_version() const
{
	return relative_version(xgetwd());
}

Path Path::relative_version(Path wrt) const
{
	if (is_relative())
		return *this;
	else
	{
		// algorithm used requires that wrt be absolute
		if (wrt.is_relative())
			wrt = wrt.absolute_version();
		String s = rep;

		// append a / to wrt and s.  This will temporarily violate their
		// canonicalness, but it'll make the algorithm easier.
		if (wrt.rep.length() > 1) wrt.rep += '/';
		if (s.length() > 1) s += '/';

		::relativize(s, wrt.rep);

		return Path(s);
	}
}
