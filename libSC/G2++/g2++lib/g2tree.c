/*ident	"@(#)G2++:g2++lib/g2tree.c	3.1" */
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
#include <g2tree.h>
#include <String.h>

#if defined(__edg_att_40)
#pragma instantiate Block<G2NODE>
#endif

G2NODE::G2NODE(){}
G2NODE::~G2NODE(){}

G2NODE* g2child_ATTLC(
    G2NODE* gp

//  Get normal (non-attribute) child node

){
    G2NODE* cp = gp->child;

    while( cp ){

	if( 
	    cp->name.char_at(0) != '.' 
	){
	    return cp;
	}
	cp = cp->next;
    }
    return NULL;
}
G2NODE* g2achild_ATTLC(
    G2NODE* gp

//  Get attribute child node 

){
    G2NODE* cp = gp->child;

    if( 
	cp && 
	cp->name.char_at(0) == '.' 
    ){
	return cp;
    }
    return NULL;
}
G2NODE* g2next_ATTLC(
    G2NODE* gp

//  Get next normal child node 

){
    G2NODE* cp = gp->next;

    while( cp ){

	if( 
	    cp->name.char_at(0) != '.' 
	){
	    return cp;
	}
	cp = cp->next;
    }
    return NULL;
}
G2NODE* g2anext_ATTLC(
    G2NODE* gp

//  Get next attribute child node 

){
    G2NODE* cp = gp->next;

    if( 
	cp && 
	cp->name.char_at(0) == '.' 
    ){
	return cp;
    }
    return NULL;
}
