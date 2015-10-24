/*ident	"@(#)publik:Lexer.c	3.1" */
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

#include "Lexer.h"

#include <stdlib.h>
#include <osfcn.h>

//List_of_pimplement(Token)

int Lexer::docontract(int i, ContractAction ca, int is_explicit)
{
	if (is_explicit && frozen == 2) 
	{
		ContractingFrozenLexer.raise("Attempt to explicitly contract a completely frozen lexer!");
		return 0;
	}
	int n = 0;
	if (is_explicit || !frozen) 
	{
		while (i-- > 0 && curToki > 0 && theWindow.length() > 0) 
		{ 
			Token *t = (Token*)(theWindow.get());
			if (ca)
				(*ca)(t);
			delete t;
			curToki--; 
			n++; 
		}
	}
	return n;
}

int Lexer::extend(int i)
{
	while (i-- > 0)
	{
		Token *t = gettok();
		theWindow.put(t);
		if (verboselevel > 1) cerr << *t << endl;
	}
	return 0;
}

static int byebye(const char *s)
{
	cerr << s << endl;
	abort();
	return 0;
}

Objection Lexer::DestroyingFrozenLexer(byebye);
Objection Lexer::ContractingFrozenLexer(byebye);
Objection Lexer::DiscardedToken(byebye);
Objection Lexer::BadHandshake(byebye);

