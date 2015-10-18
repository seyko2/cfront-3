/*ident	"@(#)cls4:src/overload.h	1.4" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
/************************************************************/
/*    header file for functions and variables shared by     */
/*    function matching and operator overloading            */
/************************************************************/

#include "cfront.h"
#include "Block.h"
#include "Bits.h"

static const int NONE = 0;
static const int ELLIP = 1;
static const int UDC = 2;
static const int STD = 3;
static const int PROM = 4;
static const int EXACT = 5;

extern int ambig;
extern int no_const;
extern int non_const;

inline int min(int i, int j) { return i<j ? i : j; }

Blockdeclare(Pname);
typedef Block(Pname) BlockPname;
Blockdeclare(BlockPname)
Blockdeclare(int)

extern Pname hier_dominates(Pname, Pname);
extern Pname bestOfPair(Pname, Pname, Ptype);
extern int matchable(Pname, Pexpr, int);
extern Bits best_conv(const Block(Pname)&, int&, bit);

extern void fmError(int, const Block(Pname)&, Pexpr, bit=0);
