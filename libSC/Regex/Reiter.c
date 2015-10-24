/*ident	"@(#)Regex:Reiter.c	3.1" */
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

Regexiter::Regexiter(const Regex &_r, const char *_start, style _s)
: r(_r), start(_start), p(_start), s(_s) {
}	

Regexiter::~Regexiter() {
}
	
#define max(A,B) ((A)>(B)?(A):(B))


Substrinfo 
Regexiter::next() {
	return next(foo);
}

Substrinfo 
Regexiter::next(String &the_substr) {
	return next(foo, the_substr);
}

Substrinfo 
Regexiter::next(Subex &subex, String &the_substr) {
	Substrinfo m = next(subex);
	if (m)
		the_substr.assign(start + m.i, m.len);
	return m;
}

Substrinfo 
Regexiter::next(Subex & subex) {
	if (p == 0)
		return nomatch;
	Substrinfo m = r.match(p, subex, p==start);

	// so indices returned by subex() will be wrt start
	subex.base = start;

	if (!m) {
		p = 0;
		return nomatch;
	}

	p += m.i;
	// make index wrt start
	m.i = p - start;
	if (*p == '\0') {  // if match occurred at end of string
		assert(m.len == 0);
		p = 0;
	}
	else {
		if (s == overlapping)
			++p;
		else
			p += max(m.len, 1);
	}
	return m;
}

const Regex &
Regexiter::the_regex() {
	return r;
}

const char *
Regexiter::the_target() {
	return start;
}

Regexiter::style
Regexiter::the_style() {
	return s;
}
