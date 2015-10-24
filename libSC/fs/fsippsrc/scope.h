/*ident	"@(#)fs:fsippsrc/scope.h	3.1" */
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

#include <String.h>

typedef enum { classScope, otherScope } ScopeType;

class Scope 
{
public:
	ScopeType type;
	String name;  // name of class, if this is a class scope.
	Scope(ScopeType t): type(t) {}
	Scope(ScopeType t, String n): type(t), name(n) {}
	~Scope() {}
};
#ifdef __GNUG__
class ostream;
extern ostream& operator<<(ostream &, const Scope &);
#endif
