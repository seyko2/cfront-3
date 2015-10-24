/*ident	"@(#)hier:hier.h	3.1" */
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

#include <stddef.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <ctype.h>
#include "CXXLexer.h"

extern CXXLexer *lexer;
extern int isolateds;
extern int diffAggs;
extern int diffInher;
extern int rootFileOnly;
extern int doClasses;
extern int doStructs;
extern int doUnions;
extern int whereFrom;
extern int backwardEdges;
extern int asciiOutput;
extern ostream *dagout;
extern ostream *asciiout;

enum Protection { Private, Protected, Public };

int parse();

