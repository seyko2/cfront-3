/*ident	"@(#)cls4:src/repr.c	1.5" */
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

#include "cfront.h"

char* oper_name(TOK op)
/*
	return the string representation of operator "op"
*/
{
	switch (op) {
	default:	error('i',"oper_name(%k)",op);
	case GNEW:
	case NEW:	return "__nw";
	case GDELETE:
	case DELETE:	return "__dl";
	case MUL:	return "__ml";
	case DIV:	return "__dv";
	case MOD:	return "__md";
	case UPLUS:
	case PLUS:	return "__pl";
	case MINUS:
	case UMINUS:	return "__mi";
	case LS:	return "__ls";
	case RS:	return "__rs";
	case EQ:	return "__eq";
	case NE:	return "__ne";
	case LT:	return "__lt";
	case GT:	return "__gt";
	case LE:	return "__le";
	case GE:	return "__ge";
	case AND:
	case ADDROF:	return "__ad";
	case OR:	return "__or";
	case ER:	return "__er";
	case ANDAND:	return "__aa";
	case OROR:	return "__oo";
	case NOT:	return "__nt";
	case COMPL:	return "__co";
	case INCR:	return "__pp";
	case DECR:	return "__mm";
	case CALL:	return "__cl";
	case DEREF:	return "__vc";
	case ASSIGN:	return "__as";
	case REF:	return "__rf";
	case ASPLUS:	return "__apl";
	case ASMINUS:	return "__ami";
	case ASMUL:	return "__amu";
	case ASDIV:	return "__adv";
	case ASMOD:	return "__amd";
	case ASLS:	return "__als";
	case ASRS:	return "__ars";
	case ASAND:	return "__aad";
	case ASOR:	return "__aor";
	case ASER:	return "__aer";
	case CTOR:	return "__ct";
	case DTOR:	return "__dt";
//	operator T		"__op"<signature of T>
//	case SIZEOF:	return "sizeof";
	case CM:	return "__cm";
	case REFMUL:	return "__rm";
// library functions:
//				"_vec_delete"
//				"_vec_new"
//				"_main"
//				"__main"	// Plan9 ape uses _main so c++ must use __main
	}
}

char* name_oper(char *s)
{
	if (strcmp(s,"__nw")==0) return "operator new";
	if (strcmp(s,"__dl")==0) return "operator delete";
	if (strcmp(s,"__ml")==0) return "operator mul";
	if (strcmp(s,"__dv")==0) return "operator div";
	if (strcmp(s,"__md")==0) return "operator mod";
	if (strcmp(s,"__pl")==0) return "operator plus";
	if (strcmp(s,"__mi")==0) return "operator minus";
	if (strcmp(s,"__ls")==0) return "operator left shift";
	if (strcmp(s,"__rs")==0) return "operator right shift";
	if (strcmp(s,"__eq")==0) return "operator equal";
	if (strcmp(s,"__ne")==0) return "operator not equal";
	if (strcmp(s,"__lt")==0) return "operator less than";
	if (strcmp(s,"__gt")==0) return "operator greater than";
	if (strcmp(s,"__le")==0) return "operator less than or equal to";
	if (strcmp(s,"__ge")==0) return "operator greater than or equal to";
	if (strcmp(s,"__ad")==0) return "operator addrof";
	if (strcmp(s,"__or")==0) return "operator or";
	if (strcmp(s,"__er")==0) return "operator er";
	if (strcmp(s,"__aa")==0) return "operator andand";
	if (strcmp(s,"__oo")==0) return "operator oror";
	if (strcmp(s,"__nt")==0) return "operator not";
	if (strcmp(s,"__co")==0) return "operator compl";
	if (strcmp(s,"__pp")==0) return "operator increment";
	if (strcmp(s,"__mm")==0) return "operator decrement";
	if (strcmp(s,"__cl")==0) return "operator call";
	if (strcmp(s,"__vc")==0) return "operator deref";
	if (strcmp(s,"__as")==0) return "operator assign";
	if (strcmp(s,"__rf")==0) return "operator ref";
	if (strcmp(s,"__apl")==0) return "operator asplus";
	if (strcmp(s,"__ami")==0) return "operator assignment minus";
	if (strcmp(s,"__amu")==0) return "operator assignment  multiply";
	if (strcmp(s,"__adv")==0) return "operator assignment div";
	if (strcmp(s,"__amd")==0) return "operator assignment mod";
	if (strcmp(s,"__als")==0) return "operator assignment left shift";
	if (strcmp(s,"__ars")==0) return "operator assignment right shift";
	if (strcmp(s,"__aad")==0) return "operator assignment and";
	if (strcmp(s,"__aor")==0) return "operator assignment or";
	if (strcmp(s,"__aer")==0) return "operator assignment er";
	if (strcmp(s,"__ct")==0) return "constructor";
	if (strcmp(s,"__dt")==0) return "destructor";
	if (strcmp(s,"__cm")==0) return "comma operator";
	if (strcmp(s,"__rm")==0) return "operator ->*";
	return s;
}


#define new_op(ss,v) keys[v]=ss

void otbl_init()
/*
	operator representation table
*/
{
	new_op("->",REF);
	new_op("." ,DOT);
	new_op(".* or ->*" ,REFMUL);
	new_op("!" ,NOT);
	new_op("~" ,COMPL);
	new_op("++",INCR);
	new_op("--",DECR);
	new_op("*" ,MUL);
	new_op("&" ,AND);
	new_op("&" ,ADDROF);
	new_op("&" ,G_ADDROF);
	new_op("/" ,DIV);
	new_op("%" ,MOD);
	new_op("+" ,PLUS);
	new_op("+" ,UPLUS);
	new_op("-" ,MINUS);
	new_op("-" ,UMINUS);
	new_op("<<",LS);
	new_op(">>",RS);
	new_op("<" ,LT);
	new_op(">" ,GT);
	new_op("<=",LE);
	new_op(">=",GE);
	new_op("==",EQ);
	new_op("!=",NE);
	new_op("^" ,ER);
	new_op("|" ,OR);
	new_op("&&",ANDAND);
	new_op("||",OROR);
	new_op("?:" ,QUEST);
//	new_op(":" ,COLON);
	new_op("=" ,ASSIGN);
	new_op("," ,CM);
	new_op("," ,G_CM);

	new_op(";" ,SM);
	new_op("{" ,LC);
	new_op("}" ,RC);
	new_op("(" ,LP);
	new_op(")" ,RP);
	new_op("[" ,LB);
	new_op("]" ,RB);

	new_op("+=",ASPLUS);
	new_op("-=",ASMINUS);
	new_op("*=",ASMUL);
	new_op("/=",ASDIV);
	new_op("%=",ASMOD);
	new_op("&=",ASAND);
	new_op("|=",ASOR);
	new_op("^=",ASER);
	new_op(">>=",ASRS);
	new_op("<<=",ASLS);

//	new_op("sizeof",SIZEOF);
//	new_op("new",NEW);
//	new_op("delete",DELETE);

	new_op("0" ,ZERO);
	new_op("[]" ,DEREF);
	new_op("expression list", ELIST);
	new_op("initializer list", ILIST);
	new_op("static initializer", STAT_INIT);
	new_op("()", CALL);
	new_op("generated function call",G_CALL);
	new_op("inline function call",ICALL);
	new_op("cast",CAST);
	new_op("generated cast",G_CAST);
	new_op("inline argument",ANAME);
	new_op("text",TEXT);
	new_op(".*",MEMPTR);

	new_op("class type", COBJ);
	new_op("enum type", EOBJ);
	new_op("union", ANON);

	new_op("function",FCT);
	new_op("pointer",PTR);
	new_op("reference",RPTR);
	new_op("array",VEC);
	new_op("identifier",ID);
	new_op("name",NAME);
	new_op("constructor",CTOR);
	new_op("destructor",DTOR);
	new_op("...",ELLIPSIS);
	new_op("::",MEM);
	new_op("type name",TYPE);
	new_op("tname",TNAME);
	new_op("{}",BLOCK);
	new_op("pair",PAIR);
	new_op("declaration",DCL);
	new_op("character constant",CCON);
	new_op("integer constant",ICON);
	new_op("float constant",FCON);
	new_op("integer value",IVAL);
	new_op("string",STRING);
	new_op("label",LABEL);
	new_op("'class', 'struct', or 'union'",AGGR);
	new_op(" argument",ARG);
	new_op(" empty expression",DUMMY);
	new_op(" ::new",GNEW);
	new_op(" constructor call",VALUE);
	new_op(" ::delete",GDELETE);
	new_op(ansi_opt?" long double":" double",LDOUBLE);
//	new_op(" long long",VLONG);
	new_op(" typedef",TPDEF); // did not want to introduce new TOKEN
}
