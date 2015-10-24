/*ident	"@(#)fs:fsippsrc/CXXToken.c	3.1" */
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

static char *tokname[] = { "Bof", "New", "Delete", "Void", "Id", "Lp", 
	"Rp", "Lc", "Rc", "Star", "Qual", "Operator", "Semi", 
	"Comma", "Pound", "Litint", "Litstring", "Eof", "Othertok", 
	"Colon", "Pr", "Virtual", "Aggr", "Nl", "Ls", "Rs", "Inline",
	"Langle", "Rangle" };

ostream &operator<<(ostream &oo, const Token &t)
{
	oo << "<" << tokname[t.type] << ", " << t.lexeme << ", " << t.lineno << ">";
	oo.flush();
	return oo;
}
