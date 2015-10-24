/*ident	"@(#)Args:Argsiter.c	3.1" */
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

#include "Argslib.h"

Optsiter::Optsiter(const Args& args_) :
	begin(1),
	opt(0),
	args(&args_) {
}

int
Optsiter::next(const Opt*& x) {
	if (begin == 1) {
		begin = 0;
		opt = args->firstopt;
	}
	else if (opt != 0) {
		opt = opt->next;
	}
	x = opt;
	return x != 0;
}

Suboptsiter::Suboptsiter(const Opt& opt_) :
	begin(1),
	subopt(0),
	opt(&opt_) {
}

int
Suboptsiter::next(const Subopt*& x) {
	if (begin == 1) {
		begin = 0;
		if (opt != 0)
			subopt = opt->firstsubopt;
		else
			subopt = 0;
	}
	else if (subopt != 0) {
		subopt = subopt->next;
	}
	x = subopt;
	return x != 0;
}

Argsiter::Argsiter(const Args& args_) :
	begin(1),
	arg(0),
	args(&args_) {
}

int
Argsiter::next(const char*& x) {
	if (begin == 1) {
		begin = 0;
		arg = args->firstarg;
	}
	else if (arg != 0) {
		arg = arg->next;
	}
	if (arg != 0)
		x = arg->p;
	else
		x = 0;
	return x != 0;
}
