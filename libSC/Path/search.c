/*ident	"@(#)Path:search.c	3.1" */
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
#include <Search_path.h>

/* p is a (possibly empty) colon-separated list of file names
*/
void 
Search_path::build_from(const char * p)
{
	assert(length() == 0);
	char *q = strdup_ATTLC(p);
        char *b = q;
        for(char *e=strchr(b,':'); e?(*e=0,e):0; b=e+1, e=strchr(b,':'))
                put(b);
        put(b); // last segment
	free(q);
}

Search_path::Search_path()
{
}

Search_path::Search_path(const char * p)
{
	// I test for p being 0 to allow the client to say: Search_path(getenv("PATH"))
	// If getenv returns 0, the empty search path is constructed.
	if (p != 0)
		build_from(p);
}

Search_path::Search_path(const Search_path & pth) : List<Path> (pth)
{
}	

Search_path & Search_path::operator=(const Search_path & pth)
{
	List<Path>::operator=(pth);
	return *this;
}

Search_path::operator String() const
{
	String s;
	Listiter<Path> i(*(Search_path*)this);
	int first = 1;
	while (!i.at_end())
	{
		if (!first)
			s += ':';
		first = 0;
		s += (String)*(i.next());
	}
	return s;
}

Search_path::operator const char *() const
{
	return (String)*this;
}


