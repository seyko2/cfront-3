/*ident	"@(#)Fsm:Chararray.c	3.1" */
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

#include "Fsm.h"

Chararray_ATTLC::Chararray_ATTLC(
    const Chararray_ATTLC& x
):
    a(x.a),
    b(x.b),
    c(x.c),
    ab(x.ab)
{
    int size=ab*c;
    p = new unsigned char[size];
    unsigned char* pp=p;
    unsigned char* xp=x.p;
    unsigned char* xguard=xp+size;

    while(xp<xguard){
	*pp++ = *xp++;
    }
}

//  The following was added to allow Fsm to have a public
//  operator=

const Chararray_ATTLC& Chararray_ATTLC::operator=(
    const Chararray_ATTLC& x
){
    delete[/* ab*c */] p;
    a = x.a;
    b = x.b;
    c = x.c;
    ab = x.ab;
    int size=ab*c;
    p = new unsigned char[size];

    unsigned char* pp=p;
    unsigned char* xp=x.p;
    unsigned char* xguard=xp+size;

    while(xp<xguard){
	*pp++ = *xp++;
    }
    return *this;
}
