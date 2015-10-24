/*ident	"@(#)Args:Argsset.c	3.1" */
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

int 
Args::isoptset(char option) const {
	for (Opt* opt = lastopt; opt != 0; opt = opt->prev) {
		if (opt->is(option))
			return 1;
	}
	return 0;
}

int 
Args::isoptset(const char* option) const {
	for (Opt* opt = lastopt; opt != 0; opt = opt->prev) {
		if (opt->is(option))
			return 1;
	}
	return 0;
}
int 
Args::isset(char option) const {
	for (Opt* opt = lastopt; opt != 0; opt = opt->prev) {
		if (opt->is(option))
			return 1;
	}
	return 0;
}

int 
Args::isset(const char* option) const {
	for (Opt* opt = lastopt; opt != 0; opt = opt->prev) {
		if (opt->is(option))
			return 1;
	}
	return 0;
}
