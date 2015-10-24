/*ident	"@(#)Map:demos/topsort.h	3.1" */
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

#ifndef TOKEN_H
#define TOKEN_H

#include <List.h>
#include <String.h>
#include <stream.h>

struct token {
	int predcnt;
	List<String> succ;

	token() {}
	token(const token& t) : predcnt(t.predcnt), succ(t.succ) {}
	~token() {}

	token& operator=( const token& t ) {
			predcnt = t.predcnt;
			succ = t.succ;
			// *this = t;
			return *this;
	}

	int operator==( const token& t ) {
			return( predcnt == t.predcnt && succ == t.succ );
	}
};

ostream& operator<<(ostream& os, token& t) {
	t.succ.print(os) << "[cnt:" << t.predcnt << "]" << endl;
	return os;
}

#endif
