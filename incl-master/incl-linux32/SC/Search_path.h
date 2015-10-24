/*ident	"@(#)Path:incl/Search_path.h	3.1" */
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

#ifndef _SEARCH_PATH_H
#define _SEARCH_PATH_H 1

#include <Path.h>
#include <ksh_test.h>
#include <List.h>

class Search_path : public List<Path>
{
	friend istream & operator>>(istream &, Search_path &);
	void build_from(const char*);
public:

// constructors
	Search_path();
	Search_path(const char*);

// copy and assign
	Search_path(const Search_path&);
	Search_path & operator=(const Search_path&);

// searching
	int find(const Path&, Path&, Ksh_test::unary = Ksh_test::x,
		 Ksh_test::id = Ksh_test::effective) const;

	int find_all(const Path&, List<Path>&, Ksh_test::unary = Ksh_test::x,
		     Ksh_test::id = Ksh_test::effective) const;

// conversions
	operator const char* () const;
	operator String() const;
};


/******************	
* global variables
******************/
// intentionally not const, so you can change it if you like.
extern Search_path PATH;  

ostream & operator<<(ostream&, const Search_path&);
istream & operator>>(istream&, Search_path&);


#endif /* _SEARCH_PATH_H */
