/*ident	"@(#)Path:complete.c	3.1" */
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
#include <List.h>

static String common_prefix(List<Path> & l) {
	// l.reset();
	Listiter<Path> li(l);
	String gcp = (String)*(li.peek_next());  // Greatest Common Prefix
	while (!li.at_end()) {
		int i = gcp.match((String)*(li.peek_next()));
		gcp(i) = "";
		if (i == 0)
			break;
		li.next();
	}
	return gcp;
}

Path::completion Path::complete(String & s) const {
	Path copy(*this);
	copy.rep += '*';
	List<Path> all;
	copy.expand_wildcards(all);

	completion ret = Path::no_completion;
	if (all.length() == 1) {
		// s = all.get()->rep;
		s = all.get().rep;
		ret = Path::unique_completion;
	}
	else if (all.length() > 1) {
		s = common_prefix(all);
		ret = Path::several_completions;
	}	
	return ret;
}

