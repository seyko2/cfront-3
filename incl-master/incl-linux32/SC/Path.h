/*ident	"@(#)Path:incl/Path.h	3.1" */
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

#ifndef _PATH_H
#define _PATH_H 1

#include <String.h>
#include <List.h>
#include <sys/file.h>
#include <Objection.h>

class istream;
class ostream;

//class List<Path>;

class Path
{
public:

// enums
	enum completion { no_completion = 0, unique_completion = 1, several_completions = 2 };

// constructors
	Path();
	Path(const char *);
	Path(const String &);
	Path(const List<Path> &);

	// Final '/' on <dirpath> is optional.
	Path(const char * dirpath, const char * base, const char * ext = 0);	

// copy and assign
	Path(const Path &);
	Path & operator=(const Path &);

// components
	Path dirname() const;
	String basename() const;
	String basename(const char * suffix) const;
	void explode(List<Path> &) const;

// catenation
	Path & operator/=(const Path &);

// relative and absolute versions of path names
	int is_relative() const;
	int is_absolute() const;
	Path absolute_version() const;
	Path relative_version() const;
	Path relative_version(Path wrt) const;
	static void cachewd(int on = 1);

// wildcard and tilde expansion
	void expand_wildcards(List<Path> &) const;
	int expand_tilde();

// file name completion
	completion complete(String &) const;

// conversions
	operator const char * () const;
	operator String() const;

// objections
	static Objection no_wd;
		// default:  abort with error message
		// recovery: set working directory to "/"

// length
	int length() const;
	int ncomponents() const;
	
// miscellaneous
	int is_wd() const;
	int truncate_components(int);

protected:
	String rep;
	static Path xgetwd();
	static int die(const char * msg);
	void canonicalize();
private:
	void build_from(const char *, const char *, const char *);
	static Path* cachedwd;
};

/******************	
* global functions
******************/
Path operator/(const Path & s, const Path & t);
int operator<(const Path &, const Path &);
int operator==(const Path &, const Path &);
int operator!=(const Path &, const Path &);
int componentwise_prefix(const Path &, const Path &);

ostream & operator<<(ostream &, const Path &);
istream & operator>>(istream &, Path &);

#ifdef __edg_att_40
#pragma can_instantiate Listiter<Path>
#endif

#endif /* _PATH_H */
