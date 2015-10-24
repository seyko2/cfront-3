/*ident	"@(#)publik:CXXToken.c	3.1" */
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

#include "CXXToken.h"
#include <iostream.h>

static char *tokname[] = { "New", "Delete", "Void", "Id", "Lp", 
	"Rp", "Lc", "Rc", "Star", "Qual", "Operator", "Semi", 
	"Comma", "Pound", "Litint", "Litstring", "Eof", "Othertok", 
	"Colon", "Public", "Private", "Protected", "Virtual", 
	"Class", "Aggr", "Nl", "Ls", "Rs", "CComment", "C++Comment", 
	"Const", "Friend", "La", "Ra", "Template" };

ostream &operator<<(ostream &oo, const Token &t)
{
	oo << "<" << tokname[t.type] << ", " << t.lexeme << ", " << t.lineno << ">" << flush;
	return oo;
}
