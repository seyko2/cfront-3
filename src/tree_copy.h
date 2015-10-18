/* ident "@(#)cls4:src/tree_copy.h	1.5" */
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
/******************************************************************************
*    Copyright (c) 1989 by Object Design, Inc., Burlington, Mass.
*    All rights reserved.
*******************************************************************************/
/* -*- Mode: C -*- begin include file tree_copy.H 
   argument class for copying trees. */

#include "hash.h"
#include "tree_walk.h"

/* The copier procedure expects one of these as its 
    info pointer */

class tree_copy_info {
  public:
/* If non-zero, this procedure is called at each node.
   it returns an action. It cannot currently return
   a node of a different class and also return an
   action other than tna_stop, since the walker won't
   notice that the rug has been removed from beneath its feet. 
   The procedure must set the last argument to indicate whether
   or not to register the node in the hash table and use the
   replacement every future time it is encountered.
*/

    void (* node_hook) (void * info, Pnode&, node_class, tree_node_action&,
			int&);

    /* if this is nonzero it is used to allocate space for
       the copies. We don't run constructors since we always
       bit copy over the new copy. */
    char * (* malloc_hook) (void * info, size_t);
    void * hook_info;
    tree_copy_info ()
	{
	    hook_info = 0;
	    node_hook = 0;
	    malloc_hook = 0;
	}; 
    char * malloc (size_t s) {
	return malloc_hook ? malloc_hook(hook_info, s) : new char[s];
    };
    void check_node (Pnode& n,
		     node_class cl, 
		     tree_node_action& action, 
		     int& never_see_again) {
	if(node_hook)
	    node_hook(hook_info, n, cl, action, never_see_again);
    };
};

void copy_tree (Pnode& n, tree_copy_info& tci, Hash * cht = 0);

/* End tree_copy.H */

