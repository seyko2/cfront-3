/*ident	"@(#)fs:fsippsrc/CXXToken.h	3.1" */
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

#ifndef CXXTOKEN_H
#define CXXTOKEN_H

#include "Token.h"

#if 0

The C++ tokens:

NEW		: "new"
DELETE		: "delete"
VOID		: "void"
OPERATOR	: "operator"
VIRTUAL		: "virtual"
INLINE		: "inline"
PR		: one of "public" "private"
AGGR		: one of "struct" "class" "union"
ID		: identifier or keyword other than the above
LITSTRING	: literal string or character constant
LITINT		: literal decimal integer
NL		: newline character (but only considered a token when global flag newlineIsToken is on)
COLON		: ":"
SEMI		: ";"
COMMA		: ","
STAR		: "*"
POUND		: "#"
LP		: "("
RP		: ")"
LC		: "{"
RC		: "}"
LS		: "["
RS		: "]"
LANGLE		: "<"
RANGLE		: ">"
QUAL		: "::"
EOFTOK		: end of input
OTHERTOK	: anything else

#endif


/* Tokens with attributes other than base attributes.
*/
class Litint : public Token {
public:
	Litint() 				{ type = LITINT; }
	Litint(const Token &base): Token(base)	{ type = LITINT; }
	int intval;
};

class Litstring : public Token {
public:
	Litstring() 					{ type = LITSTRING; }
	Litstring(const Token &base): Token(base)	{ type = LITSTRING; }

	// different from lexeme: in thestring, escape sequences are converted and open/close delimeters are missing
	String thestring;
};

#endif
