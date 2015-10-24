/*ident	"@(#)Set:Setio.c	3.1" */
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

#include <Set.h>
#include <iostream.h>
  
static void 
indent(int level)
{
    for( int i=level; i; i--)
	cerr << "\t";
}

void 
Internal_item_ATTLC::show(int level)const
{
    if ( is_null())
	cerr << "\t0\n";
    else if ( is_leaf())
	cerr << "\tleaf = " << external_leaf() << "\n";
    else if ( is_node()) {
	cerr << "\tnode = " << next_node() << "\n";
	(next_node())->show(level+1);
    }
}
void 
Internal_node_ATTLC::show(int level)const
{
    for( int i=0; i<SET_NODE_SIZE_ATTLC; i++) {
	if ( !item[i].is_null()) {
	    indent(level);
	    cerr << i;
	    (item[i]).show(level);
	}
    }
}

void 
Position_ATTLC::show()const
{
    int i;
    cerr << "\tcurr_depth = " << curr_depth << "\n";
    cerr << "\tcurr_value = " << curr_value << "\n";
    cerr << "\tcurr_pos =\n";

    for( i=0; i<=curr_depth; i++)
	cerr << "\t\t" << i << "\t" << curr_pos[i] << "\n";
}
