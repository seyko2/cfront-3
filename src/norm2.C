/*ident	"@(#)cls4:src/norm2.c	1.4" */
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

norm2.c:

	"normalization" handles problems which could have been handled
	by the syntax analyser; but has not been done. The idea is
	to simplify the grammar and the actions associated with it,
	and to get a more robust error handling

****************************************************************************/

#include "cfront.h"
#include "size.h"

#ifdef DBG
long node_id = 0;
#define DBCHECK() if(node::allocated) error('i',"allocated node (id %d, base%k) on free list! (src: \"%s\", %d",node::id,node::base,__FILE__,__LINE__);
#else
#define DBCHECK() /**/
#endif

fct::fct(Ptype t, Pname arg, TOK known)
{
	base = FCT;
	nargs_known = known;
	returns = t;
	argtype = arg; 
	DBID();

	if (arg==0 || arg->base==ELIST) return;
//error('d',"fct::fct %d sig %d",this,f_signature);
	register Pname n;
	Pname pn = 0;
	for (n=arg; n; pn=n,n=n->n_list) {
		if( n->n_sto==EXTERN ) error("cannot specify extern linkage for anA");
		if( n->n_sto==STATIC ) error("cannot specify static forA%n",arg);
 
		switch (n->tp->skiptypedefs()->base) {
		case VOID:
			argtype = 0;
			nargs_known = 1;
			if(n->n_initializer)
				error("voidFA");
			else if (n->string)
				error("voidFA%n",n);
			else if (nargs || n->n_list) {
				error("voidFA");
				nargs_known = 0;
			}
			nargs = 0;
			break;
		case CLASS:
		case ENUM:
			error("%k defined inAL (will not be in scope at point of call)",n->tp->base);
			if (n == argtype)
				argtype = n->n_list;
			else
				pn->n_list = n->n_list;
			break;
		default:
			nargs++;
		}
	}
}

expr::expr(TOK ba, Pexpr a, Pexpr b)
{
	DBCHECK();

	base = ba;
	e1 = a;
	e2 = b;
	DBID();
}

stmt::stmt(TOK ba, loc ll, Pstmt a)
{
	DBCHECK();

	base = ba;
	where = ll;
	s=a;
	memtbl = 0;
	k_tbl = 0;//SYM
	DBID();
}

classdef::classdef(TOK b)
{
	base = CLASS;
	csu = b;
	memtbl = new table(CTBLSIZE,0,0);
	k_tbl = 0;//SYM
	DBID();
}

classdef::~classdef()
{
	delete memtbl;
}

basetype::basetype(TOK b, Pname n)
{
	switch (b) {
	case 0:				break;
	case TYPEDEF:	b_typedef = 1;	break;
	case INLINE:	b_inline = 1;	break;
	case VIRTUAL:	b_virtual = 1;	break;
	case CONST:	b_const = 1;	break;
	case UNSIGNED:	b_unsigned = 1;	break;
	case FRIEND:
	case OVERLOAD:
	case EXTERN:
	case STATIC:
	case AUTO:
	case REGISTER:	b_sto = b;	break;
	case SHORT:	b_short = 1;	break;
	case LONG:	b_long = 1;	break;
	case VLONG:	b_vlong = 1;	break;
	case ANY:
	case ZTYPE:
	case VOID:
	case CHAR:
	case INT:
	case FLOAT:
	case LDOUBLE:
	case DOUBLE:	base = b; 	break;
	case TYPE:
	case COBJ:
	case EOBJ:
	case FIELD:
	case ASM:
		base = b;
		b_name = n;
		break;
	case SIGNED:
	case VOLATILE:
		error('w',"\"%k\" not implemented (ignored)",b);
		break;
	default:
		error('i',"badBT:%k",b);
	}
	DBID();
}

name::name(char* s) : expr(NAME,0,0)
{
	// DBCHECK() called in expr::expr()
	string = s;
	where = curloc;
	lex_level = bl_level;
}

name_list::name_list(Pname ff, Plist ll) 
{ 
	base = XNLIST; 
	f=ff; 
	l=ll; 
};

nlist::nlist(Pname n)
{
	head = n;
	for (Pname nn=n; nn->n_list; nn=nn->n_list);
	tail = nn;
}

void nlist::add_list(Pname n)
{
	if (n->tp && (n->tp->defined & IN_ERROR)) return;

	tail->n_list = n;
	for (Pname nn=n; nn->n_list; nn=nn->n_list);
	tail = nn;
}

Pname name_unlist(Pnlist l)
{
	if (l == 0) return 0;
	Pname n = l->head;

	delete l;
	return n;
}

Pstmt stmt_unlist(Pslist l)
{
	if (l == 0) return 0;
	Pstmt s = l->head;
//	NFl++;

	delete l;
	return s;
}

Pexpr expr_unlist(Pelist l)
{
	if (l == 0) return 0;
	Pexpr e = l->head;
//	NFl++;

	delete l;
	return e;
}

void sig_name(Pname n)
{
	static char buf[1024];
	buf[0] = '_';
	buf[1] = '_';
	buf[2] = 'o';
	buf[3] = 'p';
	char* p = n->tp->signature(buf+4);
	if (255 < p-buf) error('i',"sig_name():N buffer overflow");
	char *s = new char [ p - buf + 1 ];
	strcpy(s,buf);
	n->string = s;
	n->tp = 0;
}

Ptype tok_to_type(TOK b)
{
	Ptype t;
	switch (b) {
	case CHAR:	t = char_type; break;
	case SHORT:	t = short_type; break;
	case LONG:	t = long_type; break;
	case VLONG:	t = vlong_type; break;
	case UNSIGNED:	t = uint_type; break;
	case FLOAT:	t = float_type; break;
	case DOUBLE:	t = double_type; break;
	case LDOUBLE:	t = ldouble_type; break;
	case VOID:	t = void_type; break;
	default:	error("illegalK:%k",b);
	case INT:	t = int_type;
	}
	return t;
}

Pbase defa_type;
Pbase moe_type;
Pexpr dummy;
Pexpr zero;

Pclass ccl;
//SYM -- tn stuff removed
Plist local_class = 0; //SYM -- preserve for use in del.c

void memptrdcl(Pname bn, Pname tn, Ptype ft, Pname n)
{
	Pptr p = new ptr(PTR,0);
	p->memof = Pclass(Pbase(bn->tp)->b_name->tp);
	Pbase b = new basetype(TYPE,tn);
	PERM(p);
	Pfct f = Pfct(ft);
	Ptype t = n->tp;
	if (t) {
		p->typ = t;
	ltlt:
		switch (t->base) {
		case PTR:
		case RPTR:
		case VEC:
			if (Pptr(t)->typ == 0) {
				Pptr(t)->typ = b;
				break;
			}
			t = Pptr(t)->typ;
			goto ltlt;
		default:
			error('s',"P toMFT too complicated");
		}
	}
	else
	p->typ = b;	
	f->returns = p;
	n->tp = f;
}
