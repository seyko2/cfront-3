/*ident	"@(#)cls4:src/node_classes.h	1.7" */
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
 * node_classes.h 
 */

#ifndef _node_classes
#define _node_classes

enum discriminator_error {
    discrim_none_valid = 0,
    discrim_bad_index = -1,
    discrim_inconsistent_node = -2,
};

enum node_class {
    nc_unused = 0,
    nc_eof = 1,
    nc_virt = 2,
    nc_nlist = 3,
    nc_gen = 4,
    nc_vec = 5,
    nc_ptr = 6,
    nc_fct = 7,
    nc_table = 8,
    nc_basetype = 9,
    nc_name = 10,
    nc_expr = 11,
    nc_stmt = 12,
    nc_enumdef = 13,
    nc_classdef = 14,
    nc_baseclass = 15,
    nc_iline = 16,
    nc_ia = 17,
    nc_tpdef = 18,
    nc_ktable = 19,
};
    
/* 
	searching this is slower than a directly indexed array,
	but this is more maintainable. We can speed it up later. 
*/

#ifdef DEFINE_TOKEN_CLASS_TABLE
struct token_class {
    TOK token;
    node_class nclass;
} token_classes[] = {
    { EOFTOK, nc_eof },
    { ASM, nc_stmt },
    { AUTO, nc_basetype },
    { BREAK, nc_stmt },
    { CASE, nc_stmt },
    { CHAR, nc_basetype },
    { CLASS, nc_classdef },
    { CONTINUE, nc_stmt },
    { DEFAULT, nc_stmt },
    { DELETE, nc_expr },
    { DO, nc_stmt },
    { DOUBLE, nc_basetype },
    { ELSE, nc_stmt },
    { ENUM, nc_enumdef },
    { EXTERN, nc_basetype },
    { FLOAT, nc_basetype },
    { FOR, nc_stmt },
    { FORTRAN, nc_unused },
    { FRIEND, nc_basetype },
    { GOTO, nc_stmt },
    { IF, nc_stmt },
    { INT, nc_basetype },
    { LONG, nc_basetype },
    { TPDEF, nc_tpdef },
    { NEW, nc_expr },
    { OPERATOR, nc_fct },
    { RETURN, nc_stmt },
    { PUBLIC, nc_name },
    { CONST, nc_basetype },
    { REGISTER, nc_basetype },
    { RETURN, nc_stmt },
    { SHORT, nc_basetype },
    { SIZEOF, nc_expr },
    { STATIC, nc_basetype },
    { STRUCT, nc_unused },
    { SWITCH, nc_stmt },
    { THIS, nc_expr },
    { TYPEDEF, nc_basetype },
    { UNION, nc_unused },
    { UNSIGNED, nc_basetype },
    { VOID, nc_basetype },
    { WHILE, nc_stmt },
    { LP, nc_unused },
    { RP, nc_unused },
    { LB, nc_unused },
    { RB, nc_unused },
    { REF, nc_expr },
    { DOT, nc_expr },
    { NOT, nc_expr },
    { COMPL, nc_expr },
    { INCR, nc_expr },
    { DECR, nc_expr },
    { MUL, nc_expr },
    { DIV, nc_expr },
    { AND, nc_expr },
    { MOD, nc_expr },
    { PLUS, nc_expr },
    { MINUS, nc_expr },
    { LS, nc_expr },
    { RS, nc_expr },
    { LT, nc_expr },
    { LE, nc_expr },
    { GT, nc_expr },
    { GE, nc_expr },
    { EQ, nc_expr },
    { NE, nc_expr },
    { ER, nc_expr },
    { OR, nc_expr },
    { ANDAND, nc_expr },
    { OROR, nc_expr },
    { QUEST, nc_expr },
    { COLON, nc_unused },
    { ASSIGN, nc_expr },
    { CM, nc_expr },
    { SM, nc_stmt },
    { SM_PARAM, nc_stmt },
    { LC, nc_unused },
    { RC, nc_unused },
    { INLINE, nc_fct },
    { OVERLOAD, nc_gen },
    { VIRTUAL, nc_fct },	/* but might be a basecl */
    { COERCE, nc_unused },
    { PROTECTED, nc_name },
    { ID, nc_unused },
    { STRING, nc_expr },
    { ICON, nc_expr },
    { FCON, nc_expr },
    { CCON, nc_expr },
    { NAME, nc_name },		/* but might be basecl */
    { ZERO, nc_expr },
    { ASOP, nc_expr },
    { RELOP, nc_expr },
    { EQUOP, nc_expr },
    { DIVOP, nc_expr },
    { SHIFTOP, nc_expr },
    { ICOP, nc_expr },
    { UNOP, nc_expr },
    { TYPE, nc_basetype },
    { CATCH, nc_unused },
    { THROW, nc_unused },
    { TRY, nc_unused },
    { UMINUS, nc_expr },
    { FCT, nc_fct },
    { CALL, nc_expr },
    { VEC, nc_vec },
    { DEREF, nc_expr },
    { ADDROF, nc_expr },
    { CAST, nc_expr },
    { FIELD, nc_basetype },
    { LABEL, nc_stmt },
    { BLOCK, nc_stmt },
    { DCL, nc_stmt },
    { COBJ, nc_basetype },
    { EOBJ, nc_basetype },
    { TNAME, nc_name },
    { ILIST, nc_expr },
    { PTR, nc_ptr },
    { ASPLUS, nc_expr },
    { ASMINUS, nc_expr },
    { ASMUL, nc_expr },
    { ASDIV, nc_expr },
    { ASMOD, nc_expr },
    { ASAND, nc_expr },
    { ASOR, nc_expr },
    { ASER, nc_expr },
    { ASLS, nc_expr },
    { ASRS, nc_expr },    
    { ARG, nc_basetype },
    { ZTYPE, nc_basetype },
    { ARGT, nc_basetype },
    { ELIST, nc_expr },
    { ANY, nc_basetype },
    { TABLE, nc_table },
    { KTABLE, nc_ktable },
    { LOC, nc_unused },
    { DUMMY, nc_expr },
    { G_ADDROF, nc_expr },
    { G_CALL, nc_expr },
    { G_CAST, nc_expr },
    { G_CM, nc_expr },
    { IVAL, nc_expr },
    { ELLIPSIS, nc_unused },
    { AGGR, nc_unused },
    { RPTR, nc_ptr },
    { HIDDEN, nc_unused },
    { MEM, nc_expr },
    { CTOR, nc_unused },
    { DTOR, nc_name }, /* for p->~X(); */
    { CONST_PTR, nc_ptr },
    { CONST_RPTR, nc_ptr },
    { TEXT, nc_expr },
    { PAIR, nc_stmt },
    { ANON, nc_unused },
    { ICALL, nc_expr },
    { ANAME, nc_expr },
    { VOLATILE, nc_basetype },
    { SIGNED, nc_basetype },
    { UPLUS, nc_expr },
    { MEMPTR, nc_unused },
    { PRIVATE, nc_name },
    { PR, nc_name },
    { MEMQ, nc_name },
    { MDOT, nc_expr },
    { TSCOPE, nc_unused },
    { DECL_MARKER, nc_unused },
    { REFMUL, nc_expr },
    { LDOUBLE, nc_basetype },
    { LINKAGE, nc_unused },
    { GNEW, nc_expr },
    { TEMPLATE, nc_unused },
    { STAT_INIT, nc_unused },
    { GDELETE, nc_expr },
    { XVIRT, nc_virt },
    { XNLIST, nc_nlist },
    { XILINE, nc_iline },
    { XIA, nc_ia },
    { VALUE, nc_expr },
    { XDELETED_NODE, nc_unused },
};
#endif

node_class classify_node (Pnode, int&);
node_class classify_node (Pnode);

#endif
