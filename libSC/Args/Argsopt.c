/*ident	"@(#)Args:Argsopt.c	3.1" */
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

Opt::Opt(char f, char o, const char* v) :
	flag_(f),
	chr_(o),
	key(0),
	value_(v),
	firstsubopt(0),
	lastsubopt(0),
	buf(0),
	next(0),
	prev(0) {
}

Opt::Opt(char f, const char* o, const char* v) :
	flag_(f),
	chr_(nul),
	key(o),
	value_(v),
	firstsubopt(0),
	lastsubopt(0),
	buf(0),
	next(0),
	prev(0) {
}

Opt::~Opt() {
	delete buf;	// delete buffer
	buf=0;
	for (Subopt* sopt = firstsubopt; sopt !=0;) { // go thru suboptions
		Subopt* _next = sopt->next;
		delete sopt;	// delete suboption
		sopt = _next;
	}
	firstsubopt = lastsubopt=0;
}

int 
Opt::is(const char* opt) {
	return key != 0 && strcmp(key, opt) == 0;
}
