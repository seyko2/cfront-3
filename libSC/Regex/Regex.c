/*ident	"@(#)Regex:Regex.c	3.1" */
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

/*
static const Substrinfo nomatch = {-1, 0};
*/

Regex::operator void*() const {
	return prog;
}

int
Regex::operator!() const {
	return (prog? 0:1);
}

String 
Regex::the_error() const {
	return error;
}
	
void 
Regex::compile() {
	// i always want the match array.  getting it takes no extra time
	// during recomp, and very little extra time during reexec.
	//
	char *mess;
	subexbase = pat;  // base of subex info returned in prog
	prog = recomp_Regex_ATTLC(subexbase, pat.length(), re_match /*|RE_EDSTYLE*/, &mess);

	if (prog)
		error.make_empty();
	else
		error = mess;
}

Regex::Regex(const String &pattern, sensitivity s)
: sens(s), pat(pattern) {
	compile();
}

Regex::Regex(const Regex & r)
//: sens(r.sens), pat(r.pat), error(r.error), prog(redupprog_Regex_ATTLC(r.prog)), subexbase(r.subexbase) {
: prog(0), subexbase(0) {
	assign(r.pat, r.sens);
}

Regex::~Regex() {
	if (prog) 
		refree_Regex_ATTLC(prog);
	subexbase = 0;
	prog = 0;
}

Regex & 
Regex::operator=(const Regex & r) {
	assign(r.pat, r.sens);
	return *this;
}

void 
Regex::assign(const String &_pat, sensitivity _sens) {

	// notice that changing the sensitivity does not
	// require a recompilation, since the sensitivity
	// is only looked at when matching, not when compiling.
	sens = _sens;
	if (pat != _pat) {
		if (prog) {
			refree_Regex_ATTLC(prog);
			prog = 0;
		}
		pat = _pat;
		compile();
	}
}

int 
operator==(const Regex & r1, const Regex & r2) {
	return (r1.pat == r2.pat && r1.sens == r2.sens);
}

int 
operator!=(const Regex & r1, const Regex & r2) {
	return (r1.pat != r2.pat || r1.sens != r2.sens);
}

String 
Regex::the_pattern() const {
	return pat;
}

Regex::sensitivity 
Regex::the_sensitivity() const {
	return sens;
}

void
Regex::set_sensitivity(sensitivity _sens) {
	sens = _sens;
}


