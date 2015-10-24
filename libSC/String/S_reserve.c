/*ident	"@(#)String:S_reserve.c	3.1" */
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

void
String::reserve_grow(int target)
{
    Srep_ATTLC *x = Srep_ATTLC::new_srep(target);
    x->len = d->len;
    if (d->len != 0) {
        memcpy(x->str,d->str,d->len);
    }
    d->rcdec();
    d = x;
}
