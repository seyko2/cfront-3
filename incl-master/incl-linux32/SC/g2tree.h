/*ident	"@(#)G2++:incl/g2tree.h	3.1" */
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

#ifndef G2TREEH
#define G2TREEH

#include <Block.h>
#include <String.h>

struct G2NODE{
    String 	name;
    String 	val;
    G2NODE* 	next;     // next sibling 
    G2NODE* 	child;    // firstborn 

//  The following were needed to fix inline problems

    G2NODE();
    ~G2NODE();
};

struct G2BUF{
    int			tattoo; // distinguishing mark (G2MOTHER_ATTLC) 
    G2NODE* 		root;   // the real thing
    G2NODE* 		base;   // node buffer base
    G2NODE* 		ptr;    // node buffer ptr (post-increment)
    G2NODE* 		end;   	// node buffer guard
    Block<G2NODE> 	buf;	// node buffer
};

//  The following routines provide a functional interface
//  to G2BUFs that may be used as as alternative to
//  direct manipulation.

inline G2NODE* g2root_ATTLC( G2BUF* gbuf )
{
    return gbuf->root;
}

inline String g2name_ATTLC( G2NODE* gp )
{
    return gp->name;
}

inline String g2val_ATTLC( G2NODE* gp )
{
    return gp->val;
}

G2NODE* g2child_ATTLC( G2NODE* parent );
G2NODE* g2next_ATTLC( G2NODE* parent );
G2NODE* g2anext_ATTLC( G2NODE* parent );
G2NODE* g2achild_ATTLC( G2NODE* parent );

#endif
