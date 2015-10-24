/*ident	"@(#)G2++:incl/g2manip.h	3.1" */
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

#ifndef G2MANIPH
#define G2MANIPH

#include <String.h>

class istream;
class ostream;

String getname_ATTLC(istream& is);
String g2seek(istream& is);
String g2seek(istream& is, const String& name);

#endif
