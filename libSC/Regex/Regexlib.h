/*ident	"@(#)Regex:Regexlib.h	3.1" */
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

#include "Regex.h"
#include <stdlib.h>
#include <assert.h>

// the following stuff should match the definitions in libx/re.h
//
//assert(Regex::max_num_subexes == re_nmatch);
const int re_nmatch = ('9'-'0'+1);
const int re_all = (1<<0);	// substitute all occurrences
const int re_edstyle = (1<<1); 	// ed(1) style magic characters
const int re_match = (1<<2);	// record matches in reprogram.match

struct rematch {		// sub-expression match
	const char*	sp;	// start in source string
	const char*	ep;	// end in source string
};

struct reprogram {		// compiled regular expression program
	rematch	match[re_nmatch+1];// sub-expression match table
};

extern "C" {
	// I've changed Fowler's recomp to 
	// (0) take an additional patternlen arg,
	// (1) return at the first error,
	// (2) return the error message (if any) in *message, and
	// (3) fill reprogram->match with the subexpression info for the *pattern*.
	//
	reprogram* recomp_Regex_ATTLC(const char* pattern, size_t patternlen, int flags, char **message);

	// I've changed Fowler's reexec to take additional arguments to 
	// (0) specify case sensitivity, and 
	// (1) fill subs, rather than re, with sub-expression matches.
	//
	int reexec_Regex_ATTLC(const reprogram* re, reprogram *subs, const char *target, int case_sensitive, int at_bol);
	void refree_Regex_ATTLC(reprogram* re);

	// Doesn't work.  Was supposed to return pointer to duplicate of program.
	// Too hard to implement, and recompiling is probably almost as fast.
	//
	reprogram* redupprog_Regex_ATTLC(const reprogram *);

	// I don't use these.
	//
	//void resub_Regex_ATTLC(const reprogram* re, const char* old, const char* new, char* destination, int flags);
	//void reerror_Regex_ATTLC(const char* message);
}

