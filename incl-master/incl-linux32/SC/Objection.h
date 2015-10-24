/*ident	"@(#)Objection:incl/Objection.h	3.1" */
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

#ifndef OBJECTXXXH
#define OBJECTXXXH

typedef int Objection_action(const char*);

class Objection{

    //  The hander associated with this Objection
    Objection_action* handler;

    //  The default action associated with this Objection
    Objection_action* default_action;

    //  Two trivial objection handlers
    static int Ignore(const char*);
    static int Default(const char*);

public:
    Objection():
	handler(Objection::Default),
	default_action(0)  
    { }
    Objection(Objection_action* d):
        handler(Objection::Default),
        default_action(d)
    { }

    //  Designate a handler
    Objection_action* appoint(Objection_action *h = 0){
        Objection_action* result = handler;
        handler = h ? h : Objection::Default;
        return result;
    }
    Objection_action* ignore(){
        return appoint(Objection::Ignore);
    }

    //  Raise the Objection
    int raise(const char* s = "");
};

#endif
