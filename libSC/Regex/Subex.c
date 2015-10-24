/*ident	"@(#)Regex:Subex.c	3.1" */
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

static const Substrinfo nomatch = {-1, 0};

Subex::Subex() 
: base(0), prog(new reprogram) {
}

Subex::~Subex() {
	delete prog;
}
	
const char *
Subex::the_target() const {
	return base;
}

Substrinfo
Subex::dosubex(unsigned int i, reprogram *prog, const char *base) {
	assert(prog);
	if (i > Regex::max_num_subexes)
		return nomatch;

	// reexec only clears out prog->match[0] if no match
	if (prog->match[0].sp == 0)
		return nomatch;

	const char *start = prog->match[i].sp;
	if (start == 0)
		return nomatch;
	Substrinfo ss;
	ss.i = start - base;
	ss.len = prog->match[i].ep - start;
	assert((void*)ss);
	return ss;
}

// returns subexpression information for the match
//
Substrinfo 
Subex::operator()(unsigned int i, String &the_substr) const {
	Substrinfo m = (*this)(i);
	if (m)
		the_substr.assign(base + m.i, m.len);
	return m;
}

Substrinfo 
Subex::operator()(unsigned int i) const {
	if (base == 0) {
		return nomatch;
	}
	return dosubex(i, prog, base);
}

Substrinfo 
Regex::subex(unsigned int i) const {
	if (prog == 0) {
		return nomatch;
	}
	return Subex::dosubex(i, prog, subexbase);
}

// returns subexpression information for the pattern
//
Substrinfo 
Regex::subex(unsigned int i, String &the_subex) const {
	Substrinfo m = subex(i);
	if (m)
		the_subex.assign(subexbase + m.i, m.len);
	return m;
}
