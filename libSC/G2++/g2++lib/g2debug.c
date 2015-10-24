/*ident	"@(#)G2++:g2++lib/g2debug.c	3.1" */
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

#include <g2debug.h>
#include <g2desc.h>
#include <g2tree.h>
#include <stream.h>

void showbuf_ATTLC(G2BUF* bp){
    if(bp!=-0){
	cerr 
	    << "    tattoo = "
	    << bp->tattoo 
	    << "\n"
	    << "    root = "
	    << (void*)bp->root
	    << "\n"
	    << "    (G2NODE*)buf = "
	    << (void*)(G2NODE*)bp->buf
	    << "\n"
	    << "    base = "
	    << (void*)bp->base
	    << "\n"
	    << "    ptr = "
	    << (void*)bp->ptr
	    << "\n"
	    << "    end = "
	    << (void*)bp->end
	    << "\n"
	;
	for( 
	    G2NODE* n=bp->base; 
	    n<bp->ptr; 
	    n++ 
	){
	    cerr << "        " << (void*)n << ":\n";
	    shownode_ATTLC(n);
	}
    }
}
void showdesc_ATTLC(
    G2DESC* rd
){
    cerr 
	<< "    rd->name="
	<< rd->name
	<< "\n"
	<< "    rd->type="
	<< char(rd->type)
	<< "\n"
	<< "    rd->offset="
	<< rd->offset
	<< "\n"
	<< "    rd->size="
	<< rd->size
	<< "\n"
	<< "    rd->nel="
	<< rd->nel
	<< "\n"
	<< "    rd->pfn="
	<< (void*)rd->pfn
	<< "\n"
	<< "    rd->gfn="
	<< (void*)rd->gfn
	<< "\n"
	<< "    rd->nfn="
	<< (void*)rd->nfn
	<< "\n"
    ;
}

void shownode_ATTLC(
    G2NODE* n
){
    if( n!=0 ){
	cerr 
	    << "            name="
	    << n->name
	    << "\n"
	    << "            val="
	    << n->val
	    << "\n"
	    << "            next="
	    << (void*)n->next
	    << "\n"
	    << "            child="
	    << (void*)n->child
	    << "\n"
	;
    }
}

void showtree_ATTLC(
    G2NODE* t,
    int level
){
    if(t==0)return;

    for(int i=0;i<level;i++){
	cerr << "    ";
    }
    cerr << t->name << "    " << t->val << "\n";

    for(
	G2NODE* c=t->child;
	c;
	c=c->next
    ){
	showtree_ATTLC(c,level+1);
    }
}

