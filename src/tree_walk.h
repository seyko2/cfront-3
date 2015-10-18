/*ident	"@(#)cls4:src/tree_walk.h	1.5" */
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

#ifndef _TREE_WALK
#define _TREE_WALK

#include "cfront.h"
#include "node_classes.h"
#include "hash.h"

enum tree_node_action {
    tna_continue = 0,		/* plain vanilla */
    tna_stop = 1,		/* don't proceed */
    tna_error = 2,		/* stop entire walk now. */
};

/* procedural interface to indirect via a pointer. */
typedef int (*tree_fetch_proc) (void * info,
				void * pointer,
				unsigned long length,
				int zero_stop,  /* for character strings.*/
				void * target);


const tree_fetch_proc null_tfp = (tree_fetch_proc) 0;

class tree_walk_tree {
  public:
    tree_walk_tree * parent;
    char storage[1];		/* variable amt, specified by caller. */
};

/* The trailing int here controls whether to remember the
   node in the hash table to short-circuit meeting it again */

typedef void (*tree_pre_action)
    (Pnode&, node_class, void *, tree_node_action&, int, Pnode, 
     tree_walk_tree&, int&);


typedef void (*tree_post_action)
    (Pnode&, node_class, void *, tree_node_action&, int, Pnode, 
     tree_walk_tree&);

typedef void (*errorp) (char, const char *);

class tree_walk_control {
  public:
    int version;
    void * callback_info;
    tree_fetch_proc fetcher;
    tree_pre_action action_proc;
    tree_post_action post_action_proc;
    int (*i_error) (int, const char *); /* usually part of cfront */
    int call_i_error;		/* if 0, just use the following */
    Hash * nodes_seen_hash;	/* allows this to be global over
				 more than one call to the walker. */
    int resolve_by_name;
    int alloc_stack_bytes;
    /* if on, n_list and s_list are ignored for the very top node. 
       this is for the benefit of the printer. */
    int dont_chase_lists_top;
    tree_walk_control () {
	version = 1;
	callback_info = 0;
	fetcher = null_tfp;
	call_i_error = 0;
	nodes_seen_hash = 0;
	resolve_by_name = 1;	/* the dumper turns this off */
	alloc_stack_bytes = 0;
	post_action_proc = 0;
	dont_chase_lists_top = 0;
    }
};

typedef class patch_tree *Ppatch_tree;

class patch_tree {
public:
	static void init(Hash *_ht) { head = 0; ht = _ht; }
	static void add(Pnode);
	static void patch();
private:
	static void patch_a_ktable(Pktab);
	static void patch_a_table(Ptable);
	static void patch_a_name(Pname);
	static Pnode probe(Pnode);
	static Ppatch_tree head;
	static Hash *ht;
	Pnode node;
	Ppatch_tree next;
	patch_tree(Pnode _node, Ppatch_tree _next) : node(_node), next(_next) {}
	patch_tree(const patch_tree &);
	~patch_tree() {}
};

tree_node_action walk_tree (tree_walk_control& c, Pnode& n);

#endif
