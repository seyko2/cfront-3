/*ident	"@(#)String:S_square.c	3.1" */
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

#define IN_STRING_LIB
#include "String.h"

/* SUBCHAR STUFF */
char&
String::operator[](unsigned i)
{
    if(d->refc!=1) {
	register Srep_ATTLC* r = Srep_ATTLC::new_srep(length());
	if (length() > 0)
	    memcpy(r->str,d->str,length());
	if (d->refc > 0)
	    --(d->refc);
	d = r;
    }
    return d->str[i];
}
