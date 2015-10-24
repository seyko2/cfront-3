/*ident	"@(#)publik:CXXToken.h	3.1" */
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

#include "Token.h"

/* This file contains the definitions of those 
*  Tokens with attributes other than base attributes.
*/

class Litint : public Token {
public:
	Litint()				{ type = LITINT; }
	Litint(const Token &base): Token(base)	{ type = LITINT; }
	int intval;
};

class Litstring : public Token {
public:
	Litstring()					{ type = LITSTRING; }
	Litstring(const Token &base): Token(base)	{ type = LITSTRING; }

	// different from lexeme: escape sequences are converted and open/close delimeters are missing
	String thestring;
};


