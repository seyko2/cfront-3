/*ident	"@(#)publik:TokenType.h	3.1" */
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

#ifndef TOKENTYPEH
#define TOKENTYPEH

#if 0

The C++ tokens:

NEW		: "new"
DELETE		: "delete"
VOID		: "void"
OPERATOR	: "operator"
CONST		: "const"
FRIEND		: "friend"
VIRTUAL		: "virtual"
PUBLIC		: "public"
PRIVATE		: "private"
PROTECTED	: "protected"
CLASS		: "class"
AGGR		: one of "struct" "union"
ID		: identifier or keyword other than the above
LITSTRING	: literal string or character constant
LITINT		: literal decimal integer
NL		: newline character (but only considered a token if the
		  global flag newlineIsToken is on)
CCOMMENT	: C-style comment (but only considered a token if the 
		  global flag commentIsToken is on)
CXXCOMMENT	: C++-style comment (but only considered a token if the
		  global flag commentIsToken is on)
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
QUAL		: "::"
EOFTOK		: end of input
OTHERTOK	: anything else

#endif



/* If you change this enum, make sure to also change the tokname array in 
*  CXXToken.c, and also the keyword table in CXXLexer.c.
*/

enum TokenType { NEW, DELETE, VOID, ID, LP, RP, LC, RC, STAR, 
	QUAL,  OPERATOR, SEMI, COMMA, POUND, LITINT, LITSTRING, 
	EOFTOK, OTHERTOK, COLON, PUBLIC, PRIVATE, PROTECTED, 
	VIRTUAL, CLASS, AGGR, NL, LS, RS, CCOMMENT, CXXCOMMENT, 
	CONST, FRIEND, LANGLE, RANGLE, TEMPLATE
};

#endif
