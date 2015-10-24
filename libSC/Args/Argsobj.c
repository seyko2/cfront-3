/*ident	"@(#)Args:Argsobj.c	3.1" */
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

static const char*
flagopt(char flag, char option) {
	static char buf[3];
	buf[0] = flag;
	buf[1] = option;
	buf[2] = nul;
	return buf;
}

void
Args::unexpected_opt_err(char flag, char option) {
	opterr = 1;
	const char* p = flagopt(flag, option);
	if (unexpected_opt.raise(p) == 0) {
		write(2, progname_, strlen(progname_));
		write(2, ": unexpected option ", 20);
		write(2, p, 2);
		write(2, " (ignored)\n", 11);
	}
}

void
Args::missing_val_err(char flag, const char* option, int iskey) {
	opterr = 1;
	const char* p = (iskey? option-1 : flagopt(flag, *option));
	if (missing_val.raise(p) == 0) {
		write(2, progname_, strlen(progname_));
		write(2, ": value missing for ", 20);
		write(2, p, 2);
		write(2, " (", 2);
		write(2, p, 2);
		write(2, " ignored)\n", 10);
	}
}

void
Args::opt_as_val_err(const char* value) {
	if (opt_as_val.raise(value) == 0) {
	}
}

void
Args::missing_opt_err() {
	opterr = 1;
	if (missing_opt.raise("") == 0) {
		write(2, progname_, strlen(progname_));
		write(2, ": missing option\n", 17);
	}
}
