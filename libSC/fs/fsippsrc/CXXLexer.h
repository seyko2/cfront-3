/*ident	"@(#)fs:fsippsrc/CXXLexer.h	3.1" */
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

#ifndef CXXLEXER_H
#define CXXLEXER_H

#include "Lexer.h"
#include "CXXToken.h"

class CXXLexer: public Lexer
{
public:
	DEFINE_GET(Litint, LITINT)
	DEFINE_GET(Litstring, LITSTRING)
protected:
	Token *gettok();
private:
	Litint *getLitint(const Token *base);
	Litstring *getLitstring(const Token *base);
	void getKeywordOrId(Token *base);
	void getQualOrColon(Token *base);
	void getAngleOrShift(char c, Token *base);
	void gobbleCommentsAndWhitespace(Token *base);
};

#endif
