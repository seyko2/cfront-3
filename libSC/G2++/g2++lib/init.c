/*ident	"@(#)G2++:g2++lib/init.c	3.1" */
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
#include <g2io.h>
#include <memory.h>
#include <stream.h>

G2BUF* g2init_ATTLC( 
    G2BUF* bp 
){

//  set up *bp:
//
//        tattoo     ?
//        root           o--> 
//        end            o-------------->|
//        base           o------->|      |
//        ptr            o------->|      |
//                                |      |
//        buf      xxxxxxx <------       |
//                 xxxxxxx               |     
//                 xxxxxxx               |
//                 xxxxxxx               |
//                         <-------------

//  bp->tattoo = G2MOTHER_ATTLC;
    bp->root = NULL;

    bp->buf.reserve(G2BUFSIZE_ATTLC);
    DEBUG(cerr 
	<< "in g2init, reserve " 
	<< (G2NODE*)bp->buf.end()-(G2NODE*)bp->buf 
	<< " nodes\n"
    ;)
    bp->base = bp->ptr = bp->buf;
    bp->end = bp->buf.end();

    DEBUG(cerr << "*bp =\n";)
    DEBUG(showbuf_ATTLC(bp);)

    return bp;
}
