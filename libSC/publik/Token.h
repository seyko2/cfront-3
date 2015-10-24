/*ident	"@(#)publik:Token.h	3.1" */
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

#ifndef _TOKEN_H
#define _TOKEN_H

#include "TokenType.h"
#include <String.h>

class Token 
{
public:
//	Token(): lexeme(Stringsize(20)), ws(Stringsize(50)) {}
//	~Token() {}
	String ws;	// whitespace preceding lexeme.
	int lineno;	// line number of first character of lexeme.
	String lexeme;
	TokenType type;
};

ostream &operator<<(ostream &oo, const Token &t);

#endif
