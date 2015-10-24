/*ident	"@(#)G2++:g2++lib/g2uio.c	3.1" */
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

#include <g2io.h>
#include <g2++.h>
#include <stream.h>

istream& operator>>(istream& is,G2BUF& tree){
    getbuf_ATTLC(&tree,is);
    return is;
}
ostream& operator<<(ostream& os,G2BUF& tree){
    putbuf_ATTLC(&tree,os);
    return os;
}
