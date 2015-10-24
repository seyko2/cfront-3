/*ident	"@(#)Path:canon.c	3.1" */
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

static int component_is(const char * scan, const char * what, int whatlen)
{
	return ((strncmp(scan, what, whatlen) == 0) && (scan[whatlen] == '/' || scan[whatlen] == 0));
}

static void skip_component(const char * & scan)
{
	while (*scan != 0 && *scan != '/')
		scan++;
	while (*scan == '/')
		scan++;
}

static void copy_component(const char * buf, char * & put, const char * & scan)
{
	// remember to separate it from the previous component (if present) with a slash
	if (put > buf && put[-1] != '/')			
		*put++ = '/';
#if 0
	char * rem = put;
#endif
	while (*scan != 0 && *scan != '/')
		*put++ = *scan++;
#if 0
	if ((Path::get_max_file_length() != -1) && (put - rem > Path::get_max_file_length()))
	{
		char save = *put;
		*put = 0;
		int r = Path::filename_too_long.raise(rem);
		*put = save;
		if (r != 0)
			put = rem + Path::get_max_file_length();
	}
#endif
	while (*scan == '/')
		scan++;
}

static int has_preceding_component_and_its_not_dotdot(const char * buf, const char * p)
{
	if (p == buf)
		return 0;
	if (p == buf + 1)
		return (p[-1] != '/');
	if (p[-1] == '.' && p[-2] == '.' && (p == buf + 2 || p[-3] == '/'))
		return 0;
	return 1;
}			
	
static void handle_dotdot(const char * buf, char * & put, const char * & scan)
{
	// If there is no previous component and this is an absolute path, then
	// just ignore the .., otherwise, contract off the previous component if possible.
	// A contraction is possible iff there is a preceding component other than ..

	if (put == buf+1 && *buf == '/')
	{
		skip_component(scan);
	}
	else if (has_preceding_component_and_its_not_dotdot(buf, put))
	{
		// reset put to before the preceding component, then skip the ..
		while (put != buf && *--put != '/')
			;
		if (put == buf && *put == '/')
			put++;
		skip_component(scan);
	}
	else
	{
		copy_component(buf, put, scan);
	}
}

/* unixlike_slashes:
*	Initialize scan to point to beginning of buf
* gnulike_slashes:
*	Initialize scan to point to the second / in the rightmost //
* 	(beginning of buf if there is no //)
*/
static void init_scan(const char * buf, const char * & scan)
{
	//if (Path::get_slash_style() == Path::unixlike_slashes)
	{
		scan = buf;
	}
#if 0
	else
	{
		for (scan = buf + strlen(buf) - 1; scan >= buf; scan--)
		{
			if (scan[0] == '/' && scan[1] == '/')
			{
				scan++;
				break;
			}
		}
		if (scan < buf)
			scan = buf;
	}
#endif
}

static void canonicalize(char * buf)
{
	const char *scan;
	init_scan(buf, scan);
	char *put = buf;
	if (*scan == '/')  // absolute path name
	{
		*put++ = '/';
		do scan++;
		while (*scan == '/');
	}
	while (*scan)
	{
		// INVARIANT: scan is pointing at beginning of next component (i.e., NOT at /).

		if (component_is(scan, ".", 1))
			skip_component(scan);
		else if (component_is(scan, "..", 2))
			handle_dotdot(buf, put, scan);
		else
			copy_component(buf, put, scan);
	}
	if (put == buf) // if it is the empty relative path
		*put++ = '.';
	*put = 0;
}

void Path::canonicalize()
{
	//char *buf = new char [rep.length() + 1];
	char *buf = new char [rep.length() + 3]; // should be +2 but add one
						 // more for safety
	rep.dump(buf);
	::canonicalize(buf);
	rep = buf;
	delete buf;
#if 0
	check_length();
#endif
}
