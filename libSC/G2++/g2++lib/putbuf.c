/*ident	"@(#)G2++:g2++lib/putbuf.c	3.1" */
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
#include <g2ctype.h>
#include <g2io.h>
#include <g2tree.h>
#include <stream.h>

//  Local functions

static void putstr(
    ostream&		os,
    const String& 	s
);
static void puttabs (
    ostream&	os,
    int 	n
);
static void puttree(
    int 	level, 
    G2NODE* 	np, 
    ostream&	os
);

static String stack[G2MAXDEPTH_ATTLC];
static int stacklevel;	// previously emitted, still valid levels 
static int nonempty;

int putbuf_ATTLC(
    G2BUF* 	bp, 
    ostream&	os
){
    DEBUG(cerr << "in putbuf_ATTLC, *bp=\n";)
    DEBUG(showbuf_ATTLC(bp);)
    if( 
	bp->root 
    ){
	nonempty = 0;
	stacklevel = 0;
	puttree(0, bp->root, os);

	if( 
	    !nonempty 
	){
	    putstr(os, bp->root->name);
	    os << '\n';
	}
	if( 
	    Cchksum(os) 
	){
	    _g2putdot_ATTLC(os);
	}else{
	    os << '\n';
	}
	/*Eor(os);*/
    }
    return(Check_error(os)? -1 : 0);
}

static void putstr(
    ostream&		os,
    const String& 	s
){
    int i=0;

    while( 
	i< s.length() && 
	isprint_ATTLC(s.char_at(i)) 
    ){
	os << s.char_at(i);
	i++;
    }
}

static void puttabs(
    ostream&	os,
    int 	n
){
    while( 
	--n >= 0 
    ){
	os << '\t';
    }
}

static void puttree(
    int 	level, 
    G2NODE* 	np, 
    ostream&	os
){
    if( 
	np->child 
    ){
	register G2NODE* tp = np->child;
	stack[level] = np->name;

	do{
	    puttree(level+1, tp, os);
	}while( tp = tp->next );

    }else if( 
	!(np->val).is_empty() 
    ){
	nonempty = 1;

	for(
	    ; 
	    stacklevel < level; 
	    stacklevel++
	){
	    puttabs(os, stacklevel);
	    putstr(os, stack[stacklevel]);
	    os << '\n';
	}
	puttabs(os, level);
	putstr(os, np->name);	
	os << '\t';
	putstr(os, np->val);
	os << '\n';
    }
    if(
	stacklevel > level
    ){
	stacklevel = level;
    }
}
