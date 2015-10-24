/*ident	"@(#)G2++:incl/g2debug.h	3.1" */
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

#include <g2tree.h>
#include <g2desc.h>

//  To de-activate debug printing, change 
//  the DEBUG macro definition from
//
//      #define DEBUG(x) x
//
//  to
//
//      #define DEBUG(x)
//

#define DEBUG(x)

void showbuf_ATTLC(
    G2BUF* bp
);
void showdesc_ATTLC(
    G2DESC* rd
);
void shownode_ATTLC(
    G2NODE* np
);
void showtree_ATTLC(
    G2NODE*,int
);
