/*ident	"@(#)Regex:Rematch.c	3.1" */
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

#include "Regexlib.h"

static Subex foo;
static const Substrinfo nomatch = {-1, 0};

Substrinfo 
Regex::match(const char *target) const {
	return match(target, foo);
}

Substrinfo 
Regex::match(const char *target, String &the_substr) const {
	return match(target, foo, the_substr);
}	

Substrinfo 
Regex::match(const char *target, Subex &subex, String &the_substr) const {
	Substrinfo m = match(target, subex);
	if (m)
		the_substr.assign(target + m.i, m.len);
	return m;
}
		
Substrinfo 
Regex::match(const char *target, Subex &subex, int at_bol) const {
	if (prog == 0) {
		return nomatch;
	}
	subex.base = target;
	if (reexec_Regex_ATTLC(prog, subex.prog, target, sens==case_sensitive, at_bol)) {
		Substrinfo ss = subex(0);
		assert(ss);
		return ss;
	}
	assert(!subex(0));
	return nomatch;
} 

#ifndef initbugfixed
Regex	Regex::Alphanumeric("^[0-9a-z]+$", Regex::case_insensitive);
Regex	Regex::Alpha("^[a-z]+$", Regex::case_insensitive);
Regex	Regex::Double	("^(\\+|-)?((\\.[0-9]+)|([0-9]+(\\.[0-9]*)?))(e(\\+|-)?[0-9]+)?$", Regex::case_insensitive);
Regex 	Regex::Float	("^(\\+|-)?((\\.[0-9]+)|([0-9]+(\\.[0-9]*)?))$");
Regex	Regex::Identifier("^[a-z_][a-z0-9_]*$", Regex::case_insensitive);
Regex	Regex::Int	("^(\\+|-)?[0-9]+$");
#endif

