/*ident	"@(#)cls4:src/tree_walk.c	1.14" */
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
/* 
	tree_walk.c
	Utilities for tree-walking 
*/

#include "cfront.h"
#include "tree_walk.h"
#include "hash.h"
/*
// #include <alloca.h>
// ************ need to add an explicit call of free
// ************ make it an ifdef
// ??? #include <streamdefs.h>
*/

class walker {
    tree_walk_control control;
    Pnode orig_addr;
    Hash *nodes_seen_hash;
    int depth;
    int made_ht;
    tree_walk_tree * cur_tree;
  public:
    walker(const tree_walk_control& c) ;
 
    ~walker () {
	if(made_ht) delete nodes_seen_hash;
    }
    tree_node_action walk (Pnode&);
    tree_node_action walk_ (Pnode&);

    tree_node_action walk(Pgen& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pvec& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pptr& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Ptype& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pfct& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Ptable& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pktab& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pbase& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pname& n) 
    { return walk_ ((struct node * &)n); };
     tree_node_action walk(Pexpr& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pstmt& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pblock& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Penum& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pclass& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pvirt& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Plist& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pin& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pia& n) 
    { return walk_ ((struct node * &)n); };
    tree_node_action walk(Pbcl& n) 
    { return walk_ ((struct node * &)n); };
  private:
    int fetching () {  return (control.fetcher != null_tfp); } ;
    void free_fetched (void *);
    int fetch (void *, unsigned long, void *&);
    int fetch (void * a, unsigned long l, Pnode& p) 
    {
	int ret;
	void * t;		/* this is an output argument */
	ret = fetch(a,l,t);
	if(!ret) {
	    p = Pnode(t);
	}
	return ret;
    };
    void error ( char *, unsigned long=0 );
    tree_node_action pre_act_on_node (Pnode node, node_class nc,
				      Pnode node_copy, Pnode& replacement);
				       
    tree_node_action a_gen (Pnode, Pgen, Pnode&);
    tree_node_action a_tpdef (Pnode, Ptype, Pnode&);
    tree_node_action a_vec (Pnode, Pvec, Pnode&);
    tree_node_action a_ptr (Pnode, Pptr, Pnode&);
    tree_node_action a_fct (Pnode, Pfct, Pnode&);
    tree_node_action a_table (Pnode, Ptable, Pnode&);
    tree_node_action a_ktable (Pnode, Pktab, Pnode&);
    tree_node_action a_basetype (Pnode, Pbase, Pnode&);
    tree_node_action a_name(Pnode, Pname, Pnode&);
    tree_node_action a_expr (Pnode, Pexpr, Pnode&);
    tree_node_action a_stmt (Pnode, Pstmt, Pnode&);
    tree_node_action a_enumdef (Pnode, Penum, Pnode&);
    tree_node_action a_classdef (Pnode, Pclass, Pnode&);
    tree_node_action a_virt (Pnode, Pvirt, Pnode&);
    tree_node_action a_name_list (Pnode, Plist, Pnode&);
    tree_node_action a_iline (Pnode, Pin, Pnode&);
    tree_node_action a_ia (Pnode, struct ia *, Pnode&);
    tree_node_action a_baseclass (Pnode, Pbcl, Pnode&);
    tree_node_action a_expr_guts (Pexpr);
};

tree_node_action walker::walk_ (Pnode& n) 
{
	if(n) {
	    int save_depth = depth;
	    tree_walk_tree * save_cur_tree = cur_tree;

	    depth ++;
	    if(control.alloc_stack_bytes) {
		cur_tree = (tree_walk_tree *)
		    // alloca (control.alloc_stack_bytes + sizeof (tree_walk_tree));
		    new char[control.alloc_stack_bytes + sizeof (tree_walk_tree)];
		cur_tree->parent = save_cur_tree;
	    }
	    tree_node_action r = walk(n);
	    depth = save_depth;
	    return r;
	} else return tna_continue;
}

walker::walker(const tree_walk_control& c) 
{ control = c;
  made_ht = 0;
  if (c.nodes_seen_hash)
    nodes_seen_hash = c.nodes_seen_hash;
  else {
    nodes_seen_hash = new pointer_hash (100);
    made_ht = 1;
  }
  depth = 0;
  cur_tree = 0;
}

Ppatch_tree patch_tree::head = 0;
Hash *patch_tree::ht = 0;

Pnode patch_tree::probe(Pnode n)
{
	int found=0, replacement=0;

	ht->action((int)n, 0, Hash::probe, found, replacement);
	return found ? (Pnode)replacement : n;
}

void patch_tree::patch_a_ktable(Pktab tbl)
{
	tbl->k_next = (Pktab)patch_tree::probe(tbl->k_next);
}

void patch_tree::patch_a_table(Ptable tbl)
{
	for(int i=0; i<tbl->size; i++)
	{
		if (tbl->entries[i])
		{
			tbl->entries[i] = (Pname)patch_tree::probe(tbl->entries[i]);
		}
	}

	tbl->next = (Ptable)patch_tree::probe(tbl->next);
}

void patch_tree::patch_a_name(Pname n)
{
	n->n_ktable = (Pktab)patch_tree::probe(n->n_ktable);
}

void patch_tree::patch()
{
	while (head)
	{
		if (head->node->base == KTABLE)
		{
			patch_a_ktable(Pktab(head->node));
		}
		else if (head->node->base == TABLE)
		{
			patch_a_table(Ptable(head->node));
		}
		else
		{
			patch_a_name(Pname(head->node));
		}

		Ppatch_tree p = head->next;
		delete head;
		head = p;
	}
}

tree_node_action
walk_tree (tree_walk_control& c, Pnode& n)
{
	walker w (c);

	patch_tree::init(c.nodes_seen_hash);
	tree_node_action action = w.walk(n);
	patch_tree::patch();
	return action;
}

/* error messages are of finite length, so no need to run
   around mallocing strings */

void walker::error (char *format, unsigned long v)
{
    if(control.call_i_error) {
	char buf[1000];
	// vsprintf(buf, format, args);
        // vsprintf not universal: by inspection
        // all calls are currently of 1 or 0 arguments
	sprintf(buf, format, v);
	(*control.i_error)('i', buf);
    }
}


void
walker::free_fetched (void * addr)
{
    if (control.fetcher != null_tfp)	/* null indicates no cross-address-space */
       delete addr;
}    

int
walker::fetch (void * addr, unsigned long length, void*& taddr)
{
    int err;

    if (control.fetcher == null_tfp) { 
	taddr = addr;
	return 0;
    } else {
       taddr = (void *)new char[length];
       if(taddr == 0) {
	   error ("walker::fetch: failed to malloc %d bytes.", length);
	   return 1;
       }

       err = (*control.fetcher) (control.callback_info, addr, length, 0, taddr);
       if(err) {
	   error("walker::fetch: fetcher returned %d.", err);
	   return 1;
       }
   }
   ::error('i', "fall off end of walker::fetch()");
   return 0;
}

/* ::walk is called with a node pointer and a reference to
   a replacement node pointer. When it returns,
   replacement will be set if the action procedure
   called on the node decided to copy it or replace it.
   There are two possible modularities. 
   In case there is cross-address-space action,
   ::walk can't call the action procedure until it has
   entered the case on node bases. Once it has,
   it calls the per-structure-type procedure,
   which calls the action proc. If the action 
   proc supplies a replacement, then that replacement
   will be returned up via the reference parameters to 
   the per-structure procedures.

   It the action procedure returns tna_continue,
   then the walk continues against the new copy of the node
   so that further replacements are reflected in the new copies.
   This prevents replacement from being meaningful cross-address-space,
   since the new copy will presumably be in the current 
   (and not the cross) address space. That is, if the node
   is replaced by the action proc, the pointers in the new 
   node will drive the subsequent tree walk. Usually one
   would just bitcopy, and then they would be replaced in turn.
*/

tree_node_action
walker::walk (Pnode& top)
{
    Pnode replacement = 0;
    tree_node_action err;
    int class_err;
    node_class nclass;
    Pnode node = 0; /* assign to shut up compiler,
		       which dosen't recognize pass-by-reference as a set */

    orig_addr = top;

    if(fetching ()) {
	if(fetch((void *)top, sizeof (struct node), node))
	    return tna_error;
    } else node = top;

/* This has a complete catalog of bases, rather than just a list
   of those associated with data structures. Its important
   to detect the errs.
*/   

    nclass = classify_node (node, class_err);

    if(class_err) {
	error("walker::walk: unknown node type %d.", node->base);
	free_fetched ((void *)node);
	err = tna_error;
	goto Return;
    }
    
    switch(nclass)
    {
      default:
      case nc_unused:
	error("walker::walk: unused node type %d.", node->base);
	err = tna_error;
	goto Return;

      case nc_eof:
	break;

      case nc_virt:
	fetch((void *)top, sizeof (struct virt), node);
	err = a_virt(top, Pvirt (node), replacement);
	break;

      case nc_nlist:
	fetch((void *)top, sizeof (struct name_list), node);
	err = a_name_list(top, (struct name_list *)node, replacement);
	break;

      case nc_iline:
	fetch((void *)top, sizeof (struct iline), node);
	err = a_iline(top, (struct iline *)node, replacement);
	break;

      case nc_gen:
	fetch((void *)top, sizeof (struct gen), node);
	err = a_gen(top, Pgen (node), replacement);
	break;

      case nc_tpdef:
	fetch((void *)top, sizeof (struct type), node);
	err = a_tpdef(top, Ptype(node), replacement);
	break;

      case nc_vec:
	fetch((void *)top, sizeof (struct vec), node);
	err = a_vec(top, Pvec(node), replacement);
	break;

      case nc_ptr:
	fetch((void *)top, sizeof (struct ptr), node);
	err = a_ptr(top, Pptr(node), replacement);
	break;

      case nc_fct:
	fetch((void *)top, sizeof (struct fct), node);
	err = a_fct(top, Pfct(node), replacement);
	break;

      case nc_table:
	fetch((void *)top, sizeof (struct table), node);
	err = a_table(top, Ptable(node), replacement);
	break;

      case nc_ktable:
	fetch((void *)top, sizeof (struct ktable), node);
	err = a_ktable(top, Pktab(node), replacement);
	break;

      case nc_basetype:
	fetch((void *)top, sizeof (struct basetype), node);
	err = a_basetype(top, Pbase(node), replacement);
	break;

      case nc_name:
	fetch((void *)top, sizeof (struct name), node);
	err = a_name(top, Pname(node), replacement);
	break;

      case nc_expr:
	fetch((void *)top, sizeof (struct expr), node);
	err = a_expr(top, Pexpr(node), replacement);
	break;

      case nc_stmt:
	fetch((void *)top, sizeof (struct stmt), node);
	err = a_stmt(top, Pstmt(node), replacement);
	break;

      case nc_enumdef:
	fetch((void *)top, sizeof (struct enumdef), node);
	err = a_enumdef(top, Penum(node), replacement);
	break;

      case nc_classdef:
	fetch((void *)top, sizeof (struct classdef), node);
	err = a_classdef(top, Pclass(node), replacement);
	break;

      case nc_ia:
	fetch((void *)top, sizeof (struct ia), node);
	err = a_ia(top, (struct ia *)node, replacement);
	break;

      case nc_baseclass:
	fetch((void *)top, sizeof (struct basecl), node);
	err = a_baseclass(top, Pbcl(node), replacement);
	break;

    }

    if(replacement) {
	if (fetching ()) {
	    error
		("walker::walk: Attempt to replace tree in cross-address space mode.");
	    err = tna_error;
	} 
	else top = replacement;
    }
	
    if (control.post_action_proc && err != tna_error) {
	tree_node_action post_err;
	Pnode& post_repl = node;

	(*control.post_action_proc) (post_repl, nclass, control.callback_info, post_err,
				     depth, orig_addr, *cur_tree);
	if(post_err != tna_continue) err = post_err;
	if(post_repl != node) {
	    if (fetching ()) {
		error
		    ("walker::walk: Attempt to replace tree in cross-address space mode.");
		err = tna_error;
	    } 
	    else top = post_repl;
	}
    }

    free_fetched((void *) node);

Return:
    return err;
}

/* This is called in pre-order for each node. Then
   post_act_on_node is called after whatever recursive
   processing ensues.
   
   This is called from each of the structure-specific procedures
   to give the action procedure an opportunity to act.
   It can return a replacement pointer and control
   whether to examine the insides of the node.
   */

tree_node_action 
walker::pre_act_on_node (Pnode node, node_class nc,
		       Pnode node_copy, Pnode& replacement)
{
/* If we have been here before, then we never proceed */
/* node_copy is != node when a fetcher is in use */

    int found;
    int old_node;
    tree_node_action action;
    Pnode new_node;
    int register_in_hash = 1;

    nodes_seen_hash->action((int)node, 0, Hash::probe, found, old_node);

    if(found) {
	new_node = Pnode(old_node);
	if(new_node != node) replacement = new_node;
	return tna_stop; /* no need to proceed */
    }

    /* OK, we don't know from a previous pass. Call our actor */

    new_node = fetching () && node_copy ? node_copy : node;

    (*control.action_proc)(new_node, nc, control.callback_info, action, 
			   depth, orig_addr, *cur_tree,
			   register_in_hash);

    int zero1 = 0, zero2 = 0;
    if(action != tna_error && !fetching () && new_node != node) {
	replacement = new_node;
	if(register_in_hash)
	    nodes_seen_hash->action((int)node, 
				    (int)new_node, 
				    Hash::insert, zero1, zero2);
    }
    else {
	if(register_in_hash)
	    nodes_seen_hash->action((int)node, (int) node, Hash::insert, zero1, zero2);
    }
    return action;
}			   

tree_node_action walker::a_ktable(Pnode ta, Pktab t, Pnode& replacement) 
{
    tree_node_action action;
    action = pre_act_on_node(ta, nc_ktable, Pnode(t), replacement);
    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	t = Pktab(replacement);

    action=walk(t->k_t);
    if(action == tna_error) return action;

    action=walk(t->k_name);
    if(action == tna_error) return action;

// k_next is patched up after tree is fully walked

    return tna_continue;
}


tree_node_action walker::a_table(Pnode ta, Ptable t, Pnode& replacement) 
{
    /* no unions */

    tree_node_action action;
    action = pre_act_on_node(ta, nc_table, Pnode(t), replacement);
    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	t = Ptable(replacement);

    for(int i = 0; i < t->size; i++)
    {
	if (t->entries[i] && t->entries[i]->base==TNAME)
	{
	    action = walk(t->entries[i]);
	    if(action == tna_error) return action;
	}
    }

    action = walk(t->real_block);
    if(action == tna_error) return action;
    
    action = walk(t->t_name);
    if(action == tna_error) return action;

// t_next is patched up after tree is fully walked

    return tna_continue;
}

tree_node_action walker::a_enumdef (Pnode ta, Penum e, Pnode& replacement) 
{
    tree_node_action action = pre_act_on_node(ta, nc_enumdef, Pnode(e), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	e = Penum(replacement);

    action=walk(e->in_class);
    if(action == tna_error) return action;

    action = walk(e->mem);
    if(action == tna_error) return action;

    action = walk(e->e_type);

    return tna_continue;
}    

tree_node_action walker::a_virt(Pnode ta, Pvirt v, Pnode& replacement) 
{
    /* no unions */

    int nx;
    tree_node_action action = pre_act_on_node(ta, nc_enumdef, Pnode(v), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	v = Pvirt(replacement);

/* an array of velem structures. */

    velem * v_virt_init;

    if(fetching ()) {
	void * t;
	fetch((void *)v->virt_init, v->n_init * sizeof(velem), t);
	v_virt_init = (velem *)t;
    }
    else v_virt_init = v->virt_init;

    for(nx = 0; nx < v->n_init; nx ++) {
	action = walk(v_virt_init[nx].n);
	if(action == tna_error) return action;
    }

    if(fetching ()) free_fetched ((void *)v_virt_init);
    
    action = walk(v->vclass);
    
    return tna_continue;
}

tree_node_action walker::a_classdef(Pnode ta, Pclass c, Pnode& replacement) 
{

// ::error('d',"walker::a_classdef: %t beginning ", c);
    tree_node_action action = pre_act_on_node(ta, nc_classdef, Pnode(c), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	c = Pclass(replacement);

    action = walk(c->baselist);
    if(action == tna_error) return action;

    action=walk(c->mem_list);
    if(action == tna_error) return action;
    
    action=walk(c->memtbl);
    if(action == tna_error) return action;

    action=walk(c->k_tbl);
    if(action == tna_error) return action;

    action=walk(c->friend_list);
    if(action == tna_error) return action;

    action=walk(c->pubdef);
    if(action == tna_error) return action;
    
//SYM    action=walk(c->tn_list);
//SYM    if(action == tna_error) return action;

    action=walk(c->in_class);
    if(action == tna_error) return action;

    action=walk(c->this_type);
    if(action == tna_error) return action;

    action=walk(c->virt_list);
    if(action == tna_error) return action;

    action=walk(c->c_ctor);
    if(action == tna_error) return action;
    action=walk(c->c_dtor);
    if(action == tna_error) return action;
    action=walk(c->c_itor);
    if(action == tna_error) return action;

    action=walk(c->conv);
    if(action == tna_error) return action;

// ::error('d',"walker::a_classdef: %t ending ", c);

    return tna_continue;
}

tree_node_action walker::a_basetype(Pnode ta, Pbase b, Pnode& replacement) 
{
    int derr;

    tree_node_action action = pre_act_on_node(ta, nc_basetype, Pnode(b), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	b = Pbase(replacement);

    action = walk(b->b_name);
    if(action == tna_error) return action;

    action = walk(b->b_table);
    if(action == tna_error) return action;

    // action = walk(b->b_field);
    // if(action == tna_error) return action;

    action = walk(b->b_xname);
    if(action == tna_error) return action;

    switch(derr = b->discriminator(0)) {
      case 0: break;
      case 1:
	action = walk(b->b_fieldtype);
	if(action == tna_error) return action;
	break;
      case 2: break;
      default:
	error ("a_basetype: discrim error %d.", derr);
	return tna_error;
    }

    return tna_continue;
}

tree_node_action walker::a_fct(Pnode ta, Pfct f, Pnode& replacement) 
{

    tree_node_action action = pre_act_on_node(ta, nc_fct, Pnode(f), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	f = Pfct(replacement);

    action = walk(f->returns);
    if(action == tna_error) return action;

    action = walk(f->argtype);
    if(action == tna_error) return action;

    action = walk(f->s_returns);
    if(action == tna_error) return action;

    action = walk(f->f_this);
    if(action == tna_error) return action;

    action = walk(f->memof);
    if(action == tna_error) return action;

    action = walk(f->body);
    if(action == tna_error) return action;

    action = walk(f->f_init);
    if(action == tna_error) return action;

    action = walk(f->f_expr);
    if(action == tna_error) return action;

//    action = walk(f->last_expanded);
//    if(action == tna_error) return action;

    action = walk(f->f_result);
    if(action == tna_error) return action;

    action = walk(f->f_args);
    if(action == tna_error) return action;

    return tna_continue;
}

tree_node_action walker::a_name_list(Pnode ta, Plist l, Pnode& replacement) 
{

    int cl_error;
    tree_node_action action = pre_act_on_node(ta, nc_nlist, Pnode(l), replacement);

    if(action == tna_stop) {
	if(!fetching () && replacement)
	    l = Plist(replacement);
	cl_error = 0;
	if((classify_node(Pnode(l), cl_error) == nc_nlist) && !cl_error) {
	    action = walk(l->l);
	    if(action == tna_error) return action;
	}
    }

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	l = Plist(replacement);

    action = walk(l->f);
    if(action == tna_error) return action;

    action = walk(l->l);
    if(action == tna_error) return action;

    return tna_continue;
}

tree_node_action walker::a_tpdef(Pnode ta, Ptype t, Pnode& replacement)
{
    tree_node_action action = pre_act_on_node(ta, nc_tpdef, Pnode(t), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	t = Ptype(replacement);

    action=walk(t->in_class);
    if(action == tna_error) return action;

    return tna_continue;
}

tree_node_action walker::a_gen(Pnode ta, Pgen g, Pnode& replacement) 
{

    tree_node_action action = pre_act_on_node(ta, nc_gen, Pnode(g), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	g = Pgen(replacement);

    action = walk(g->fct_list);
    if(action == tna_error) return action;

    return tna_continue;
}

tree_node_action walker::a_vec(Pnode ta, Pvec v, Pnode& replacement) 
{

    tree_node_action action = pre_act_on_node(ta, nc_vec, Pnode(v), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	v = Pvec(replacement);

    action = walk(v->typ);
    if(action == tna_error) return action;

    action = walk(v->dim);
    if(action == tna_error) return action;

    return tna_continue;
}

tree_node_action walker::a_ptr(Pnode ta, Pptr p, Pnode& replacement) 
{

    tree_node_action action = pre_act_on_node(ta, nc_ptr, Pnode(p), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	p = Pptr(replacement);

    action = walk(p->typ);
    if(action == tna_error) return action;

    action = walk(p->memof);
    if(action == tna_error) return action;

    action = walk(p->ptname);
    if(action == tna_error) return action;

    return tna_continue;
}


tree_node_action walker::a_expr_guts(Pexpr e)
{
    int derr;
    tree_node_action action;

    switch(derr = e->discriminator (0)) {
      case 1:
	action = walk(e->tp);
	if(action == tna_error) return action;
	break;
      case 0:
	break;
      default:
	error ("a_expr: discrim error %d on union 0.", derr);
	return tna_error;
    }

    switch(derr = e->discriminator (1)) {
      case 0:
	break;
      default:
	error ("a_expr: discrim error %d on union 1.", derr);
	return tna_error;
      case 1:
	action = walk(e->e1);
	if(action == tna_error) return action;
	break;
      case 2:
	break;
      case 3:
	break;
    }

    switch(derr = e->discriminator (2)) {
      case 0:
	break;
      default:
	error ("a_expr: discrim error %d on union 2.", derr);
	return tna_error;
      case 1:
	/* elists are special. e2 for an elist is a peer, not
	   a child. */
	if(e->base != ELIST) {
	    action = walk(e->e2);
	    if(action == tna_error) return action;
	}
	break;
      case 2:
	break;
      case 3:
	break;
      case 4:
	action = walk(e->n_initializer);
	if(action == tna_error) return action;
	break;
      case 5: // nested typedef
	action = walk(e->tpdef);
	if(action == tna_error) return action;
	break;
    }
    
    switch(derr = e->discriminator (3)) {
      case 0:
	break;
      default:
	error ("a_expr: discrim error %d on union 3.", derr);
	return tna_error;
      case 1:
	action = walk(e->tp2);
	if(action == tna_error) return action;
	break;
      case 2:
	action = walk(e->fct_name);
	if(action == tna_error) return action;
	break;
      case 3:
	action = walk(e->cond);
	if(action == tna_error) return action;
	break;
      case 4:
	action = walk(e->mem);
	if(action == tna_error) return action;
	break;
      case 5:
	action = walk(e->as_type);
	if(action == tna_error) return action;
	break;
      case 6:
	action = walk(e->n_table);
	if(action == tna_error) return action;
	break;
      case 7:
	action = walk(e->il);
	if(action == tna_error) return action;
	break;
    }
    return tna_continue;
}


tree_node_action walker::a_expr(Pnode ta, Pexpr e, Pnode& replacement) 
{
    tree_node_action action = pre_act_on_node(ta, nc_expr, Pnode(e), replacement);

    if(action == tna_stop) {
	if(!fetching () && replacement)
	    e = Pexpr(replacement);
	/* ELIST implies that e2 is a peer, not a child */
	if(e->base == ELIST) {
	    action = walk(e->e2);
	    return action;
	}
    }
	
    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	e = Pexpr(replacement);

    action = a_expr_guts(e);
    if (action == tna_error) return action;
    if(e->base == ELIST)
	action = walk(e->e2);
    return action;
}

tree_node_action walker::a_baseclass(Pnode ta, Pbcl b, Pnode& replacement) 
{
    tree_node_action action = pre_act_on_node(ta, nc_baseclass, Pnode(b), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	b = Pbcl(replacement);

    action = walk(b->bclass);
    if(action == tna_error) return action;

    action = walk(b->init);
    if(action == tna_error) return action;

    action = walk(b->next);
    if(action == tna_error) return action;

    return tna_continue;
}


/* a name is also an expr. */
   

tree_node_action walker::a_name(Pnode ta, Pname n, Pnode& replacement) 
{
    int derr;
    tree_node_action action = pre_act_on_node(ta, nc_name, Pnode(n), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	n = Pname(replacement);

    // We don't walk n_tbl_list. Its not part of the graph. 

    switch(derr = n->discriminator(0)) {
      case 0:
	break;
      case 1:
	action = walk(n->n_qualifier);
	if(action == tna_error) return action;
	break;
      case 2:
	action = walk(n->n_realscope);
	if(action == tna_error) return action;
	break;
      default:
	error ("a_name: discrim error %d on union 0.", derr);
	return tna_error;
    }

    if (n->base==DTOR) {
	action = walk(n->n_dtag);
	if(action == tna_error) return action;
	if (n->n_dtag && n->n_dtag->n_template_arg==template_type_formal) {
	    Pname nn = new name;
	    *nn = *n->n_dtag;
	    n->n_dtag = nn;
	    Ptype t = n->n_dtag->tp;
	    Pname cn = t->is_cl_obj();
	    if (cn || eobj) {
		n->n_dtag->string = new char[strlen(eobj ? Penum(eobj->tp)->string : Pclass(cn->tp)->string)+1];
		strcpy(n->n_dtag->string, eobj ? Penum(eobj->tp)->string : Pclass(cn->tp)->string);
	    } else {
		n->tpdef = t;
	    }
	}
    }

    action = a_expr_guts(Pexpr(n));
    if(action == tna_error) return action;

    if(depth > 0 || !control.dont_chase_lists_top) {
	action = walk(n->n_list);
	if(action == tna_error) return action;
    }

// n_ktable is patched up after tree is fully walked
    
    return action;
}
    
/* --- NOTE: s_list should be deferred until AFTER the post-action
   procedure is called, if there is one. Since no one uses
   post-actions yet I haven't bothered to make this fix.
   --benson */

tree_node_action walker::a_stmt(Pnode ta, Pstmt s, Pnode& replacement) 
{
    int cl_error;
    int derr;
    tree_node_action action = pre_act_on_node(ta, nc_stmt, Pnode(s), replacement);

    if(action == tna_stop) {
	if(!fetching () && replacement)
	    s = Pstmt(replacement);
	cl_error = 0;
	if((classify_node(Pnode(s), cl_error) == nc_stmt) && !cl_error) {
	    /* s_list is not our subordinate, it is our peer */
	    if(depth > 0 || !control.dont_chase_lists_top) {
		action = walk(s->s_list); /* continue walk of sibs */
		if (action == tna_error) return tna_error;
	    }
	}
	return tna_stop;
    }

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	s = Pstmt(replacement);

    action = walk(s->s);
    if(action == tna_error) return action;

    action = walk(s->memtbl);
    if(action == tna_error) return action;

    switch(derr = s->discriminator(0)) {
      default:
	error ("a_stmt: discrim error %d on union 0.", derr);
	return tna_error;
      case 4:
      case 0: break;
      case 1:
	action = walk(s->d);
	if(action == tna_error) return action;
	break;
      case 2:
	action = walk(s->e2);
	if(action == tna_error) return action;
	break;
      case 3:
	action = walk(s->has_default);
	if(action == tna_error) return action;
	break;
      case 5:
	action = walk(s->ret_tp);
	if(action == tna_error) return action;
	break;
    }

    switch(derr = s->discriminator(1)) {
      default:
	error ("a_stmt: discrim error %d on union 1.", derr);
	return tna_error;
      case 2:
      case 0: break;
      case 1:
	action = walk(s->e);
	if(action == tna_error) return action;
	break;
      case 3:
	action = walk(s->s2);
	if(action == tna_error) return action;
	break;
    }

    switch(derr = s->discriminator(2)) {
      default:
	error ("a_stmt: discrim error %d on union 2.", derr);
	return tna_error;
      case 0: break;
      case 1:
	action = walk(s->for_init);
	if(action == tna_error) return action;
	break;
      case 2:
	action = walk(s->else_stmt);
	if(action == tna_error) return action;
	break;
      case 3:
	action = walk(s->case_list);
	if(action == tna_error) return action;
	break;
    }

    if(depth > 0 || !control.dont_chase_lists_top) {
	action = walk(s->s_list);
	if(action == tna_error) return action;
    }

    return tna_continue;
}

tree_node_action walker::a_ia(Pnode ta, struct ia * ia, Pnode& replacement) 
{
    tree_node_action action = pre_act_on_node(ta, nc_ia, Pnode(ia), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	ia = (Pia)replacement;

    action = walk(ia->local);
    if(action == tna_error) return action;

    action = walk(ia->arg);
    if(action == tna_error) return action;
    
    action = walk(ia->tp);
    if(action == tna_error) return action;

    return tna_continue;
}

tree_node_action walker::a_iline(Pnode ta, Pin iline, Pnode& replacement) 
{
    tree_node_action action = pre_act_on_node(ta, nc_iline, Pnode(iline), replacement);

    if(action != tna_continue) return action;

    if(!fetching () && replacement)
	iline = Pin(replacement);

    action = walk(iline->fct_name);
    if(action == tna_error) return action;

//    action = walk(iline->i_next);
//    if(action == tna_error) return action;
    
    action = walk(iline->i_table);
    if(action == tna_error) return action;

    action = walk(iline->i_args);
    if(action == tna_error) return action;

    return tna_continue;
}

void patch_tree::add(Pnode node)
{
	head = new patch_tree(node, head);
}
