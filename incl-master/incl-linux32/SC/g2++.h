/*ident	"@(#)G2++:incl/g2++.h	3.1" */
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

#ifndef G2PLUSPLUSH
#define G2PLUSPLUSH

//  Untyped I/O

#include <g2manip.h>
#include <g2tree.h>

class istream;
class ostream;

istream& operator>>(istream& is,G2BUF& tree);
ostream& operator<<(ostream& os,G2BUF& tree);

#endif
