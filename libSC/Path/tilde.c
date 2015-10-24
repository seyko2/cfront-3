/*ident	"@(#)Path:tilde.c	3.1" */
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

// Thanks to Dave Miller for this implementation of tilde expansion!

#include "Pathlib.h"
#include <pwd.h>
#include <List.h>

static const char* get_home_dir(const String& tilde) 
{
	if(tilde.length() == 1) 
	{
		assert(tilde[0] == '~');

		/*   old way - gets logins home, not $HOME like KSH
		*     char* logname = getlogin();
		*     if(logname != 0) passent = getpwnam(logname);
		*     else {
		*	  int uid = getuid();
		*	  passent = getpwuid(uid);
		*	  home_dir = passent->pw_dir;
		*          }
		*/
		return getenv("HOME");
	}
	else 
	{
		struct passwd* passent = getpwnam((const char *)tilde + 1);
		if (passent == 0)
			return 0;
		return passent->pw_dir;
	}
}

int Path::expand_tilde()
{
	if (rep.length() == 0 || rep[(unsigned)0] != '~' ) 
		return 1;
	List<Path> expl;
	explode(expl);
	String tilde((String)(expl.get()));
	const char* home_dir = get_home_dir(tilde);
	if (home_dir == 0)
		return 0;
	expl.unget(home_dir);
	*this = Path(expl);
	return 1;
}

#if 0
// Don't supply this.  It isn't in the minimal interface.
void Search_path::expand_tildes()
{
	Path* p;
	// whoops, shouldn't use the built-in iterator!
	for(reset(); peek_next(p); step_next() ) {
	     const String val = *p;
	     if( val.length() > 0 && val[0] == '~') {
		  Path x = p->expand_tilde();
		  replace_next(x);
	          }
	     }
}
#endif
