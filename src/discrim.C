/*ident	"@(#)cls4:src/discrim.c	1.6" */
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
 * union discriminator functions for nodes in cfront. 
 * all return 0 for none,
 *   -1 for bad union index, -2 for inconsistent
 *  or otherwise messed up nodes 
 */
#include "cfront.h"
#define DEFINE_TOKEN_CLASS_TABLE
#include "node_classes.h"

int basetype::discriminator(int which_union)
{
    switch(which_union)
    {
      case 0:
	switch(base) {
	  case FIELD: return 1;
	  case FCT: return 2;
	  default: return discrim_none_valid;
	}
      default:
	return discrim_bad_index;
    }
}

#if 0
int fct::discriminator(int)
{
    return discrim_bad_index;
}
#endif

int expr::discriminator(int which_union)
{
    switch(which_union) {
      case 0:
	return 1;		/* tp, never syn_class */
      case 1:			/* e1, i1, string */
	switch(base) {
	  case DEREF:
	  case ICALL:
	  case REF:
	  case DOT:
	  case VALUE:
	  case SIZEOF:
	  case NEW:
	  case GNEW:
	  case DELETE:
	  case CAST:
	  case G_CAST:
	  case CALL:
	  case G_CALL:
	  case ASSIGN:
	  case EQ:
	  case NE:
	  case GT:
	  case GE:
	  case LE:
	  case LT:
	  case ELIST:
	  case ILIST:
	  case QUEST:
	  case CM:
	  case G_CM:
	  case PLUS:
	  case MINUS:
	  case MUL:
	  case DIV:
	  case MOD:
	  case LS:
	  case RS:
	  case AND:
	  case OR:
	  case ER:
	  case ANDAND:
	  case OROR:
	  case ASOR: 
	  case ASER: 
	  case ASAND:
	  case ASPLUS:
	  case ASMINUS:
	  case ASMUL:
	  case ASMOD:
	  case ASDIV:
	  case ASLS:
	  case ASRS:
	  case DECR:
	  case INCR:
	    return 1;
	  case MDOT:
	  case IVAL:
	    return 2;
	  case TNAME:
	  case NAME:
	  case ICON:
	  case FCON:
	  case CCON:
	  case ID:
	  case STRING:
	  case TEXT:
	    return 3;
	  default: return discrim_none_valid;
	}
      case 2:			/* e2, i2, string2, n_initializer */
	/* i2 is a complete mystery. It is set to 1,
	   and never referenced. But I'm not sure that
	   someone somewhere doesn't test one of the other
	   union elements for equal to 1, so therefore this test. */
	if(i2 == 1)return 2;
	switch(base) {
	  case DELETE:
	  case VALUE:
	  case ICALL:
	  case CALL:
	  case G_CALL:
	  case ASSIGN:
	  case EQ:
	  case NE:
	  case GT:
	  case GE:
	  case LE:
	  case LT:
	  case DEREF:
	  case ELIST:
	  case QUEST:
	  case CM:
	  case G_CM:
	  case UMINUS:
	  case NOT:
	  case COMPL:
	  case ADDROF:
	  case G_ADDROF:
	  case PLUS:
	  case MINUS:
	  case MUL:
	  case DIV:
	  case MOD:
	  case LS:
	  case RS:
	  case AND:
	  case OR:
	  case ER:
	  case ANDAND:
	  case OROR:
	  case ASOR: 
	  case ASER: 
	  case ASAND:
	  case ASPLUS:
	  case ASMINUS:
	  case ASMUL:
	  case ASMOD:
	  case ASDIV:
	  case ASLS:
	  case ASRS:
	  case DECR:
	  case INCR:
	  case NEW:	/*- the placement expression list hangs off e2 -*/
	    return 1;
	  case TEXT:
	    return 3;
	  case TNAME:
	    if (tp->base != COBJ && tp->base != EOBJ) 
		return 5; // typedef: tpdef field
		// no break;
	  case NAME:
	    /* The n_initializer field is used for TNAMEs when describing base */
	    /* class initializations. The TNAME refers to the base class, and */
	    /* the actuals arguments are hung off the n_initializer list */
	    return 4;
	  default: return discrim_none_valid;
	}
      case 3:			/* tp2, fct_name, cond, mem, as_type, n_table, il, query_this */
	switch(base) {
	  case VALUE:
	  case SIZEOF:
	  case NEW:
	  case GNEW:	
	  case CAST:
	  case G_CAST:
	    return 1;
	  case CALL:
	  case G_CALL:
	    return 2;
	  case QUEST:
	    return 3;
	  case REF:
	  case DOT:
	  case MDOT: 
	    return 4;
	  case ASOR: 
	  case ASER: 
	  case ASAND:
	  case ASPLUS:
	  case ASMINUS:
	  case ASMUL:
	  case ASMOD:
	  case ASDIV:
	  case ASLS:
	  case ASRS:
	  case DECR:
	  case INCR:
	  case ASSIGN:
	    return 5;

	  case NAME:
	  case TNAME:
	    if (Pname(this)->n_oper==TYPE &&
		    (strcmp(Pname(this)->string, "_type")==0)) 
            {
// error('d',"string %s n_oper: %k", string, Pname(this)->n_oper);
			return 3;
	    }
	    return 6;

	  case ICALL:
	  case ANAME: 
	    return 7;

	  default: return discrim_none_valid;
	}
	    
      default:
	return discrim_bad_index;
    }
}

int name::discriminator (int which_union)
{
    switch(which_union) {
      case 0:			/* n_qualifier, n_realscope */
	if(base == LABEL) return 2;
	else return 1;
      default:
	return discrim_bad_index;
    }
}

int stmt::discriminator (int which_union)
{
    switch(which_union) {
      case 0:			/* d, e2, has_default, case_value, ret_tp */
	switch(base) {
	  case BLOCK:
	  case GOTO:
	  case LABEL:
	  case DCL:
	    return 1;
	  case FOR:
	    return 2;
	  case SWITCH:
	    return 3;
	  case PAIR:
	  case RETURN:
	    return 4;
	  default: return discrim_none_valid;
	}
      case 1:			/* e, own_tbl, s2 */
	switch(base) {
	  case FOR:
	  case IF:
	  case WHILE:
	  case DO:
	  case RETURN:
	  case SWITCH:
	  case SM:
	  case SM_PARAM:
	  case CASE:
	    return 1;
	  case BLOCK:
	    return 2;
	  case PAIR:
	    return 3;
	  default:
	    return discrim_none_valid;
	}
      case 2:			/* for_init, else_stmt, case_list */
	switch(base) {
	  case FOR:
	    return 1;
	  case IF:
	    return 2;
	  case SWITCH:
	  case CASE:
	    return 3;
	  default:
	    return discrim_none_valid;
	}
      default:
	return discrim_bad_index;
    }
}

static node_class token_to_class_map[DUMMY_LAST_NODE];
static char map_initialized;

node_class classify_node (Pnode node, int& error)
{
    int ncx;
    node_class nclass;

    error = 0;

    if (! map_initialized) {
	map_initialized = 1 ;
	for(ncx = 0; ncx < sizeof (token_classes) / sizeof (token_class);
	    ncx ++)
	    token_to_class_map[token_classes[ncx].token]
		= token_classes[ncx].nclass;
    }

    if (!((node->base > 0) && (node->base < DUMMY_LAST_NODE))) {
	error = 1;
	return nc_unused;
    }

    nclass = token_to_class_map[node->base];

    switch(nclass) {
      case nc_fct:
      case nc_name:
	if(node->baseclass) nclass = nc_baseclass;
    }

    return nclass;
}

node_class classify_node (Pnode node)
{
    int err;
    node_class nclass = classify_node (node, err);
    if(err) error ('i', "failed to classify node.");
    return nclass;
}
