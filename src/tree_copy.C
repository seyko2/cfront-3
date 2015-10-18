/*ident	"@(#)cls4:src/tree_copy.c	1.7" */
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
/* utilities to copy pieces of cfront trees. 
   what we have here is a somewhat parameterizable
   action procedure for the tree walker.

   We may make more versions of this for template expansion
   and saving things in files, or we may just make it possible
   for this to swing more ways. 
*/

#include "cfront.h"
#include "template.h"
#include "tree_walk.h"
#include "tree_copy.h"
#include <memory.h>

/* determine whether type "n" is already DEFINED or has been COPIED */
inline unsigned int type_is_defined(Pnode n)
{
	return Ptype(n)->defined & (DEFINED | COPIED);
}

void
copy_walker (Pnode& node, node_class cl, void * info, 
	     tree_node_action& action, int, Pnode, tree_walk_tree&,
	     int& register_in_hash)
{
    union {
	Pnode node;
	Pvirt vr;
	Plist list;
	Pgen g;
	Pvec vc;
	Pfct f;
	Ptable t;
	Pktab  kt;
	Pbase  bt;
	Pexpr  x;
	Pstmt  s;
	Ptype  tp;
	Penum  e;
	Pclass c;
	Pbcl   bcl;
	Pin    iline;
	ia   * ia;
	Pname  n;
	Pptr p;
      } n;
    Pnode original_node = node;

    tree_copy_info * tci = (tree_copy_info *)info;
    action = tna_continue;

    /* first, give an application-specific hook a shot at the node. */

    tci->check_node(node, cl, action, register_in_hash);
    if(action != tna_continue) return;    

    if(node != original_node) { /* a replacement */
      n.node = node;
    } else /* ok, nothing funny, we just go ahead and copy */
      switch (cl) {
      case nc_unused: 
	break;	
      case nc_eof:
	n.node = Pnode(tci->malloc(sizeof (node)));
	*n.node = *node;
	goto Replace;

      case nc_virt:
	n.vr = Pvirt(tci->malloc(sizeof(virt)));
	*n.vr = *Pvirt(node);
	goto Replace;

      case nc_nlist:
	n.list = Plist(tci->malloc(sizeof(name_list)));
	*n.list = *Plist(node);
	goto Replace;

      case nc_gen:
	n.g = Pgen(tci->malloc(sizeof(gen)));
	*n.g = *Pgen(node);
	goto Replace;

      case nc_vec: 
	n.vc = Pvec(tci->malloc(sizeof(vec)));
	*n.vc = *Pvec(node);
	goto Replace;

      case nc_ptr:
	n.p = Pptr(tci->malloc(sizeof(ptr)));
	*n.p = *Pptr(node);
	goto Replace;

      case nc_fct:
	n.f = Pfct(tci->malloc(sizeof(fct)));
	*n.f = *Pfct(node);
	if(n.f->f_signature) {
	    n.f->f_signature =
		(char *)tci->malloc(strlen(Pfct(node)->f_signature)+1);
	    strcpy(n.f->f_signature, Pfct(node)->f_signature);
	};
	goto Replace;

      case nc_table:
	n.t = Ptable(tci->malloc(sizeof(table)));
	*n.t = *Ptable(node);
	patch_tree::add(n.node);
	n.t->entries = (Pname *)tci->malloc(sizeof(Pname) * n.t->size);
	memcpy((char *)n.t->entries, (char *)Ptable(node)->entries, n.t->size * sizeof(Pname));
	n.t->hashtbl = (short *)tci->malloc(sizeof(short) * n.t->hashsize);
	memcpy((char *)n.t->hashtbl, (char *)Ptable(node)->hashtbl, n.t->hashsize * sizeof (short));
	goto Replace;

      case nc_ktable:
	// hope that nc_table gets the k_t
	n.kt = Pktab(tci->malloc(sizeof(ktable)));
	*n.kt = *Pktab(node);
	patch_tree::add(n.node);
	goto Replace;

      case nc_basetype:
	// Don't copy types that have already been dealt with
	if (type_is_defined(node)) {
	  action = tna_stop ;
	  return ;
	}
	n.bt = Pbase(tci->malloc(sizeof(basetype)));
	*n.bt = *Pbase(node);
	n.bt->defined |= COPIED;
	if(n.bt->discriminator(0) == 2 && n.bt->b_linkage) {
	    n.bt->b_linkage = tci->malloc(strlen(n.bt->b_linkage) + 1);
	    strcpy(n.bt->b_linkage, Pbase(node)->b_linkage);
	}
	goto Replace;

      case nc_expr:
	// cfront needs identity maintaied for these nodes
	if ((node == dummy) || (node == zero)) {
	  action = tna_stop ;
	  return ;
	}
	n.x = Pexpr(tci->malloc(sizeof(expr)));
	*n.x = *Pexpr(node);
	if(n.x->discriminator(1) == 3 && n.x->string) {
	    n.x->string = tci->malloc(strlen(n.x->string) + 1);
	    strcpy(n.x->string, Pexpr(node)->string);
	}
	if(n.x->discriminator(2) == 3 && n.x->string2) {
	    n.x->string2 = tci->malloc(strlen(n.x->string2) + 1);
	    strcpy(n.x->string2, Pexpr(node)->string2);
	}
	goto Replace;

      case nc_stmt:
	n.s = Pstmt(tci->malloc(sizeof(stmt)));
	*n.s = *Pstmt(node);
	goto Replace;

      case nc_enumdef:
	if (type_is_defined(node)) {
	  action = tna_stop ;
	  return ;
	}
	n.e = Penum(tci->malloc(sizeof(enumdef)));
	*n.e = *Penum(node);
	n.e->defined |= COPIED;
	if(n.e->string) {
	    n.e->string = tci->malloc(n.e->e_strlen+1);
	    strcpy(n.e->string, Penum(node)->string);
	}
	if (n.e->nested_sig)
	{
	    n.e->nested_sig = 0;	// deleted in 'name::dcl' anyway
	}
	goto Replace;

      case nc_classdef:
	// Don't copy types that have already been dealt with
	if (type_is_defined(node)) {
	  action = tna_stop ;
	  return ;
	}
	n.c = Pclass(tci->malloc(sizeof(classdef)));
	*n.c = *Pclass(node);
	n.c->defined |= COPIED;
	if(n.c->string) {
	    n.c->string = tci->malloc(strlen(n.c->string)+1);
	    strcpy(n.c->string, Pclass(node)->string);
	}
	if (n.c->nested_sig)
	{
	    n.c->nested_sig = tci->malloc(strlen(n.c->nested_sig)+1);
	    strcpy(n.c->nested_sig, Pclass(node)->nested_sig);
	}
	goto Replace;

      case nc_baseclass:
	n.bcl = Pbcl(tci->malloc(sizeof(struct basecl)));
	*n.bcl = *Pbcl(node);
	goto Replace;

      case nc_tpdef:
	n.tp = Ptype(tci->malloc(sizeof(struct type)));
	*n.tp = *Ptype(node);
	if (n.tp->nested_sig)
	{
	    n.tp->nested_sig = tci->malloc(strlen(n.tp->nested_sig)+1);
	    strcpy(n.tp->nested_sig, Ptype(node)->nested_sig);
	}
	goto Replace;

      case nc_iline:
	n.iline = Pin(tci->malloc(sizeof(iline)));
	*n.iline = *Pin(node);
	goto Replace;

      case nc_ia:
	n.ia = (ia *)tci->malloc(sizeof(ia));
	*n.ia = *(ia *)node;
	goto Replace;

      case nc_name:
	/* check for globalosity */
	if( Pname(node)->string ) {
// ::error('d',"tree_copy: nc_name: %n", Pname(node));
	  Pname n;
	  if ( node->base == TNAME ) {
		n = k_find_name(Pname(node)->string,Gtbl,0);
		if ( n && n->base==NAME ) n = 0;
	  } else n = gtbl->look(Pname(node)->string,0);
	  if ( (Pname)node == n ) {
	    action = tna_stop;
	    return;
	  }
	}
	n.n = Pname(tci->malloc(sizeof(name)));
	*n.n = *Pname(node);
	if (n.n->n_ktable)
		patch_tree::add(n.node);
/* First, hack exprosity */
	{
		int done=0;
		if (n.n->base==MEMQ && n.n->string) {
			Pbase_inst bi = (Pbase_inst)tci->hook_info;
			for (Plist formal=bi->inst_formals; formal; formal = formal->l) {
				if (formal->f->n_template_arg != template_type_formal)
					continue;
				if (strcmp(formal->f->string, n.n->string)==0) {
					Ptype t = formal->f->tp;
					Pname cn = t->is_cl_obj();
					if (cn || eobj) {
						n.n->string = tci->malloc(strlen(eobj ? Penum(eobj->tp)->string : Pclass(cn->tp)->string)+1);
						strcpy(n.n->string, eobj ? Penum(eobj->tp)->string : Pclass(cn->tp)->string);
					} else {
						n.n->string = 0;
					}
					done = 1;
					break;
				}
			}
		} else if(!done && n.x->discriminator(1) == 3 && n.n->string) {
			n.n->string = tci->malloc(strlen(n.n->string) + 1);
			strcpy(n.n->string, Pexpr(node)->string);
		}
	}
	if(n.x->discriminator(2) == 3 && n.n->string2) {
	    n.n->string2 = tci->malloc(strlen(n.n->string2) + 1);
	    strcpy(n.n->string2, Pexpr(node)->string2);
/* ok, name stuff */
	}
	if(n.n->n_anon) {
	    n.n->n_anon = tci->malloc(strlen(n.n->n_anon)+1);
	    strcpy(n.n->n_anon, Pname(node)->n_anon);
	}
	if(n.n->n_template_arg_string) {
	    n.n->n_template_arg_string =
		tci->malloc(strlen(n.n->n_template_arg_string)+1);
	    strcpy(n.n->n_template_arg_string, Pname(node)->n_template_arg_string);
	}
    }
  Replace:
    node = n.node;
    action = tna_continue;
    return;
}

static int call_error (int i, const char * s)
{
    return error (i, s);
}

void
copy_tree (Pnode& node, tree_copy_info& tci, Hash * cht)
{
    tree_walk_control twc;

    twc.call_i_error = 1;
    twc.i_error = call_error; /* ... in type of error confuses compiler */
    twc.action_proc = copy_walker;
    twc.nodes_seen_hash = cht;
    twc.callback_info = (void *)&tci;

    walk_tree (twc, node);
}
