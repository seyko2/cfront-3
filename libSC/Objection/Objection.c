/*ident	"@(#)Objection:Objection.c	3.1" */
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

#include <Objection.h>

int Objection::Default(const char*) { return 0; }
int Objection::Ignore(const char*) { return 1; }

int Objection::raise(const char* s) { 
    //  Call the handler associated with this Objection
    int result = 0;
    if (handler == 0) {
	//  Initialization has not occurred - do it now
	handler = Objection::Default;
	default_action = 0;
    }
    result = (*handler)(s);

    if (result == 0) {
	//  The handler has indicated "failure"
	//  If a default action has been specified, do it
	if (default_action) {
	    (*default_action)(s);
	}
    }
    return result;
}
