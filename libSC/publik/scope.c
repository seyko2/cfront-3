/*ident	"@(#)publik:scope.c	3.1" */
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

#ifdef __GNUG__
#pragma implementation "List.h"
#endif
#include "scope.h"
#include "publik.h"
#ifdef __GNUG__
template class List_of_p<Token>;
template class List_of_p<Scope>;
#endif

List_of_p<Scope> scopeStack;

void pushClassScope(String tag)
{
	scopeStack.put(new Scope(classScope, tag));
}

void pushOtherScope()
{
	scopeStack.put(new Scope(otherScope));
}

void popScope()
{
	if (scopeStack.length() == 0)
	{
		ADV;
		syntaxError("unmatched right brace");
	}
	else
	{
		delete (scopeStack.unput());
	}
}

/* Change the current protection level of the immediately containing class to pr
*/
void changePr(TokenType pr)
{
	List_of_piter<Scope> scopeStackI(scopeStack);

	for (scopeStackI.end_reset(); !scopeStackI.at_head(); scopeStackI.step_prev())
	{
		if (scopeStackI.peek_prev()->type == classScope)
		{
			scopeStackI.peek_prev()->pr = pr;
			return;
		}
	}
}

bool_type inClass()
{
	TokenType pr;
	return inClass(pr);
}

bool_type inClass(TokenType &pr)
{
	List_of_piter<Scope> scopeStackI(scopeStack);

	for (scopeStackI.end_reset(); !scopeStackI.at_head(); scopeStackI.step_prev())
	{
		if (scopeStackI.peek_prev()->type == classScope)
		{
			pr = scopeStackI.peek_prev()->pr;
			return yes;
		}
	}
	return no;
}


/* returns true just if we're in a public section of a class, 
*  which is in a public section of its containing class, ...
*/
bool_type inUpwardlyPublicPartOfClass()
{
	bool_type inClass = no;
	List_of_piter<Scope> scopeStackI(scopeStack);

	for (scopeStackI.end_reset(); !scopeStackI.at_head(); scopeStackI.step_prev())
	{
		if (scopeStackI.peek_prev()->type == classScope)
		{
			inClass = yes;
			if (scopeStackI.peek_prev()->pr != PUBLIC)
				return no;
		}
	}
	return inClass;
}	

#ifdef __GNUG__
#include <iostream.h>
ostream& operator<<(ostream &os, const Scope &) { return os; }
#endif
