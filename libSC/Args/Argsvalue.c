/*ident	"@(#)Args:Argsvalue.c	3.1" */
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

const char* 
Args::value(char option) const {
	for (Opt* opt = lastopt; opt != 0; opt = opt->prev) {
		if (opt->is(option))
			return opt->value_;
	}
	return 0;
}

const char* 
Args::value(const char* option) const {
	for (Opt* opt = lastopt; opt != 0; opt = opt->prev) {
		if (opt->is(option))
			return opt->value_;
	}
	return 0;
}
