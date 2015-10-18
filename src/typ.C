/*ident	"@(#)cls4:src/typ.c	1.18" */
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
#include "size.h"

Pbase short_type;
Pbase int_type;
Pbase char_type;
Pbase long_type;
Pbase vlong_type;

Pbase uchar_type;
Pbase ushort_type;
Pbase uint_type;
Pbase ulong_type;
Pbase uvlong_type;

Pbase zero_type;
Pbase float_type;
Pbase double_type;
Pbase ldouble_type;
Pbase void_type;
Pbase any_type;

Ptype Pint_type;
Ptype Pchar_type;
Ptype Pvoid_type;
Ptype Pfctvec_type;

Ptable gtbl;
Ptable ptbl;

Pname Cdcl;
Pstmt Cstmt;

bit new_type;

Pclass type::classtype() {
	return (base==COBJ)?Pclass(Pbase(this)->b_name->tp)
		: (error('i',"T::classtype(): %k cobjX",base),Pclass(0));
}

Ptype np_promote(TOK oper, TOK r1, TOK r2, Ptype t1, Ptype t2, TOK p, bit perr)
/*
	an arithmetic operator "oper" is applied to "t1" and "t2",
	types t1 and t2 has been checked and belongs to catagories
	"r1" and "r2", respectively:
		A	ANY
		Z	ZERO
		I	CHAR, SHORT, INT, LONG, VLONG, FIELD, or EOBJ
		F	FLOAT DOUBLE LDOUBLE
		P	PTR (to something) or VEC (of something)
	test for compatability of the operands,
	if (p) return the promoted result type
*/
{
	if (r2 == 'A')
		return t1;

	switch (r1) {
	case 'A':
		return t2;
	case 'Z':
		switch (oper) {
		case ASMOD:
		case ASAND:
		case ASOR:
		case ASLS:
		case ASRS:
		case ASPLUS:
		case ASMINUS:
			return any_type;
		}
		switch (r2) {
		case 'Z':		return int_type;
		case 'F':
			switch (oper) {
			case MOD:
			case AND:
			case ER:
			case OR:
			case LS:
			case RS:
				return any_type;
			}
			// no break
		case 'I':
			switch (oper) {
			case DEREF:
				return any_type;
			case LS: // result type is that of promoted left op
			case RS:
				return int_type;
			}
			return	(p)
				?
				Pbase(t2->skiptypedefs())->arit_conv(0)
				:
				0;
		case 'P':
			switch (oper) {
			case MOD:
			case AND:
			case ER:
			case OR:
			case LS:
			case RS:
			case GE:
			case GT:
			case LE:
			case LT:
				return any_type;
			case PLUS:
			case ASPLUS:
				if(t2!=Pvoid_type)
					break;
			case EQ:
			case NE:
			case QUEST:
				break;
			default:
				return any_type;
			}
			return t2;
		case FCT:
			switch (oper) {
			case QUEST:
				return any_type;
			case EQ:
			case NE:
				return t2;
			}
			if(perr) error("zero%kF",oper);
			return any_type;
		default:
			error('i',"zero(%d)",r2);
		}
	case 'I':
		switch (r2) {
		case 'Z':
			t2 = 0;
			switch (oper) {
			case DEREF:
				return any_type;
			}
			return	(p)
				?
				Pbase(t1->skiptypedefs())->arit_conv(Pbase(t2))
				:
				0;
		case 'F': 
			switch (oper) {
			case MOD:
			case AND:
			case ER:
			case OR:
			case LS:
			case RS:
			case ASMOD:
			case ASAND:
			case ASOR:
			case ASLS:
			case ASRS:
				return any_type;
			}
			// no break;
		case 'I':
			switch (oper) {
			case DEREF:
				return any_type;
			case LS: // result type is that of promoted left op
			case RS:
				return (p)
					?
					Pbase(t1->skiptypedefs())->arit_conv(Pbase(0))
					:
					0;
			}
			return	(p)
				?
				Pbase(t1->skiptypedefs())->arit_conv(Pbase(t2))
				:
				0;
		case 'P':
			switch (oper) {
			case DEREF:
				break;
			case PLUS:
			case ASPLUS:
				if(t2!=Pvoid_type)
					break;
			default:
				if(perr) error("int%kP",oper);
				return any_type;
			}
			return t2;
		case FCT:
			if(perr) error("int%kF",oper);
			return any_type;
		default:
			error('i',"int(%d)",r2);
			return any_type;
		}
	case 'F':
		switch (oper) {
		case MOD:
		case ASMOD:
		case AND:
		case ER:
		case OR:
		case ASAND:
		case ASOR:
		case LS:
		case RS:
		case ASLS:
		case ASRS:
			return any_type;
		}
		switch (r2) {
		case 'Z':
			t2 = 0;
		case 'I':
		case 'F': 
			if(oper==DEREF)
				return any_type;
			return	(p)
				?
				Pbase(t1->skiptypedefs())->arit_conv(Pbase(t2))
				:
				0;
		case 'P':
			if(perr) error("float%kP",oper);
			return any_type;
		case FCT:
			if(perr) error("float%kF",oper);
			return any_type;
		default:
			error('i',"float(%d)",r2);
			return any_type;
		}
	case 'P':
		switch(oper) {
		case LS:
		case RS:
		case ASLS:
		case ASRS:
		case MOD:
		case ASMOD:
		case ER:
		case OR:
		case ASOR:
		case AND:
		case ASAND:
		case DIV:
		case MUL:
			return any_type;
		}
		switch (r2) {
		case 'Z':
			switch (oper) {
			case GE:
			case GT:
			case LE:
			case LT:
				return any_type;
			}
			return t1;
		case 'I':
			switch (oper) {
			case DEREF:
			case PLUS:
			case MINUS:
			case ASPLUS:
			case ASMINUS:
				if (t1->check(Pvoid_type,0)==0) {
					return any_type;
				}
				break;
			default:
				if(perr) error("P%k int",oper);
				return any_type;
			}
			return t1;
		case 'F':
			if(perr) error("P%k float",oper);
			return any_type;
		case 'P':
			if (t1->check(t2,ASSIGN)) {
				Ptype tt1 = t1->is_ptr()->typ->skiptypedefs();
				Ptype tt2 = t2->is_ptr()->typ->skiptypedefs();
				switch (oper) {
				case EQ:
				case NE:
				case LE:
				case GE:
				case GT:
				case LT:
				case MINUS:
				case QUEST:
				    if (tt1 && tt2 
					&& 
					tt1->base==COBJ && tt2->base==COBJ
				    ) {
					Pclass c1=tt1->classtype();
					Pclass c2=tt2->classtype();
					if (c1 && c2 
					    && 
					    c1->baseof(c2) || c2->baseof(c1)
					) goto zz; 
				    }
				    if (t2->check(t1,ASSIGN) == 0) {
					if (oper == QUEST) return t2;
					goto zz;
				    }
				    break;
				case REFMUL:
					{
				    Pname cn = tt1->is_cl_obj();
				    if (cn && tt2->base == FCT 
					&& 
					same_class(Pclass(cn->tp),Pfct(tt2)->memof)
				    ) 
					return t2;
					}
				}
				if(perr) error("T mismatch:%t %k%t",t1,oper,t2);
				return any_type;
			}
			zz:
			switch (oper) {
			case MINUS:	
				return	(t2!=Pvoid_type)
					?
					int_type
					:
					any_type;
			case ASMINUS:
				if(perr) error("P -=P");
				return any_type;
			case PLUS:
				if(perr) error("P +P");
				return any_type;
			case ASPLUS:
				if(perr) error("P +=P");
				return any_type;
			case MOD:
			case ASMOD:
			case AND:
			case ER:
			case OR:
			case ASAND:
			case ASOR:
			case ASLS:
			case ASRS:
			case LS:
			case RS:
			case DEREF:
				return any_type;
			default:
				return t1;
			}
		case FCT:
			return t1;
		default:
			error('i',"P(%d)",r2);
		}
	case FCT:
		if(oper == QUEST) {
			switch (r2) {
			case 'Z':
				return any_type;
			case 'P':
				return t2;
			case 'I':
			case 'F':
				if(perr) error("F%k%t",oper,t2);
			default:
				return t1;
			}
		}
		if(Pfct(t1)->memof && r2=='P' && t2->memptr())
			return t2;
		if((oper == EQ || oper == NE) && r2=='Z') return t1;
		if(perr) error("F%k%t",oper,t2);
		return any_type;
	default:
		error('i',"np_promote(%d,%d)",r1,r2);
		return 0;
	}
}

TOK type::kind(TOK oper, TOK v, bit perr)
/*	v ==	'I'	integral
		'N'	numeric
		'P'	numeric or pointer
*/
{
	Ptype t = this;
	if (this == 0)
		error('i',"type::kind(): this==0");

	t = t->skiptypedefs();

	switch (t->base) {
	case ANY:
		return 'A';
	case ZTYPE:
		return 'Z';
	case FIELD:
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case VLONG:
	case EOBJ:
		return 'I';
	case FLOAT:
	case LDOUBLE:
	case DOUBLE:
		if (v == 'I')
			if(perr) error("float operand for %k",oper);
		return 'F';
	case VEC:
	case PTR:
		if (v != 'P')
			if(perr) error("P operand for %k",oper);
		switch (oper) {
		case INCR:
		case DECR:
		case MINUS:
		case PLUS:
		case ASMINUS:
		case ASPLUS:
			if (
				t->base==PTR
				&&
				(
					Pptr(t)->memof
					||
					Pptr(t)->typ->base==FCT
				)
			) {
				if(perr) error("%t operand of%k",this,oper);
			}
			else {
				Pptr(t)->typ->tsizeof(); // get increment
			}
			break;
		default:
			if (
				t->base==PTR
				&&
				(
					Pptr(t)->memof
					||
					Pptr(t)->typ->base==FCT
				)
			)
				if(perr) error("%t operand of%k",this,oper);
		case LT:
		case LE:
		case GT:
		case GE:
		case ANDAND:
		case OROR:
		case ASSIGN:
		case NE:
		case EQ:
		case IF:
		case WHILE:
		case DO:
		case FOR:
		case QUEST:
		case NOT:
				break;
		}
		return 'P';
	case RPTR:
		if(perr) error("R operand for %k",oper);
		return 'A';
	case FCT:
		if (v != 'P')
			if(perr) error("F operand for %k",oper);
		return FCT;
	case OVERLOAD:
		if(perr) error("overloaded operand for %k",oper);
		return 'A';
	case CLASS:
	case ENUM:
		if(perr) error("%k operand for %k",base,oper);
		return 'A';
	default:
		if(perr) error("%t operand for %k",this,oper);
		return 'A';
	}
}

void type::dcl(Ptable tbl)
/*
	go through the type (list) and
	(1) evaluate vector dimensions
	(2) evaluate field sizes
	(3) lookup struct tags, etc.
	(4) handle implicit tag declarations
*/
{
	Ptype t = this;

	// processing_sizeof suppresses errors for refs to names in the arg 
	// to sizeof.  Turn errors back on for exprs within type specs
	// (such as array subscripts)
	int os = processing_sizeof;
	processing_sizeof = 0;

	if (this == 0)
		error('i',"T::dcl(this==0)");
	if (tbl->base != TABLE)
		error('i',"T::dcl(%d)",tbl->base);

xx:
	switch (t->base) {
	case TYPE:
		t = Pbase(t)->b_name->tp;
		goto xx;
	case PTR:
	case RPTR:
	{
		Pptr p = Pptr(t);
		if(p->memof == 0 && p->ptname) { // T::*, where T is a template formal
			Ptype tp = p->ptname->tp->skiptypedefs();
			switch (tp->base) {
			    case COBJ:
				{
				p->memof = tp->classtype();
				if (p->typ)
				{
					Ptype t = p->typ->skiptypedefs();
					if (t && t->base==FCT)
					{
						Pfct(t)->memof = p->memof;
					}
				}
				break;
				}
			    case CLASS:
				{
				p->memof = Pclass(tp);
				Ptype t = p->typ->skiptypedefs();
				Pfct f = Pfct(t); // safe???
				f->memof = p->memof;
				break;
				}	
			    default:
				error("illegalZizedP toM %t::*",tp);
				break;
			}
		}
		t = p->typ;
		if (t->base == TYPE) {
			Ptype tt = Pbase(t)->b_name->tp;
			if (tt->base == FCT)
				p->typ = tt;
			goto done;
		}
		goto xx;
	}

	case VEC:
	{
		Pvec v = Pvec(t);
		Pexpr e = v->dim;
		if (e) {
			Ptype et;
			v->dim = e = e->typ(tbl);
			if (e->tp->skiptypedefs()->base == COBJ) {
				e = check_cond(e,DEREF,tbl);
				v->dim = e;
			}
			et = e->tp;
			if (et->integral(0) == 'A') {
				error("UN in array dimension");
			}
			else {
				long i;
				Neval = 0;
				i = e->eval();
				if (Neval == 0) {
					if (largest_int<i)
						error("array dimension too large");
					v->size = int(i);

					DEL(v->dim);
					v->dim = 0;
				}

				if (new_type) {
					if (Neval)
						;
					else if (i == 0)
						v->dim = zero;
					else if (i < 0) {
						error("negative array dimension");
						i = 1;
					}
				}
				else {
					if (Neval)
						error("%s",Neval);
					else if (i == 0) {
						error("array dimension == 0");
						v->dim=e;
					}
					else if (i < 0) {
						error("negative array dimension");
						i = 1;
					}
				}
			}
		}
		t = v->typ;
	llx:
		switch (t->base) {
		case TYPE:
			t = Pbase(t)->b_name->tp;
			goto llx;
		case FCT:
			v->typ = t;
			break;
		case VEC:				
			if (Pvec(t)->dim==0 && Pvec(t)->size==0)
				error("null dimension (something like [][] seen)");
		}
		goto xx;
	}

	case FCT:
	{
		Pfct f = Pfct(t);
		void dargs(Pname, Pfct, Ptable);

		if (f->argtype)
			dargs(0,f,tbl);
		for (Pname n=f->argtype; n; n = n->n_list) {
			Ptype t = n->tp;
			n->tp->dcl(tbl);
			while(t->base==TYPE)
				t = Pbase(t)->b_name->tp;
			if(t->base==VEC)
				n->tp = new ptr(PTR,Pvec(t)->typ);
		}
		Pname cn = f->returns->is_cl_obj();
		if (cn && Pclass(cn->tp)->has_itor())
			make_res(f);
		else if (f->f_this == 0)
			f->f_args = f->argtype;

		t = f->returns;
		goto xx;
	}

	case FIELD:
	{
		Pbase f = Pbase(t);
		Pexpr e = Pexpr(f->b_name);
		long i;
		Ptype et;
		e = e->typ(tbl);
		f->b_name = Pname(e);
		et = e->tp;
		if (et->integral(0) == 'A') {
			error("UN in field size");
			i = 1;
		}
		else {
			Neval = 0;
			i = e->eval();
			if (Neval)
				error("%s",Neval);
			else if (i < 0) {
				error("negative field size");
				i = 1;
			}
			else if (f->b_fieldtype->tsizeof()*BI_IN_BYTE < i)
				error("field size > sizeof(%t)",f->b_fieldtype);
			DEL(e);
		}
		f->b_bits = int(i);
		f->b_name = 0;
		break;
	}
	}
done:
	processing_sizeof = os;
	return;
}

static bit 
fm_same_class(Pclass c1, Pclass c2) 
/* Predicate to determine whether two classes are indeed the same. 
 * as arguments to a template function: uninstantiated: t
 * template <class T> void f(X<T> t));
 * X<T> results in an UNINSTANTIATED template class
 * friend X<T> void foo( X<T> ) results in a CL_TEMPLATE
 * because the template nature is extrapolated from the declaration
 * and class objects referenced within friend declarations are not
 * placed on the class templ_ref list for instantiation
 */
{
// error('d',"fm_same_class: %t %d %t %d",c1,c1,c2,c2);
// error('d',"fm_same_class: c1->cl_base %d c2->cl_base %d",c1->class_base,c2->class_base);

	if (same_class(c1,c2)) return 1;

	Templ_type t1 = c1->class_base;
	Templ_type t2 = c2->class_base;

  	if ((t1 == UNINSTANTIATED || t1 == CL_TEMPLATE) &&
      		(t2 == UNINSTANTIATED || t2 == CL_TEMPLATE) &&
      		(strcmp(c1->string,c2->string)==0))
    			return 1;

  	return 0;
}

bit const_problem;	// types differ only in const
bit return_error;	// functions differ only in return
static bit pt_ptm;	// template pointer to member
static bit pt_over;	// strong check of template_classes
int Vcheckerror;
extern int template_hier; // permit derived/base conversion in templates

bit type::check(Ptype t, TOK oper, bit level)
/*
	check if "this" can be combined with "t" by the operator "oper"

	used for check of
			assignment types		(oper==ASSIGN)
			declaration compatability	(oper==0)
			decl. compatibility without "const"ness (oper=254) 
						(oper == IGNORE_CONST)
			parameterized type formals	(oper==255)
			  as for (oper==0) but
			  special checking for ANY types
			argument types			(oper==ARG)
			return types			(oper==RETURN)
			overloaded function name match	(oper==OVERLOAD)
			overloaded function coercion	(oper==COERCE)
			virtual function match		(oper==VIRTUAL)

	NOT for arithmetic operators

	return 1 if the check failed
*/
{

	register Ptype t1 = this, t2 = t;
	bit cnst1 = 0, cnst2 = 0;
	TOK b1, b2;
	bit vv=0, over=0, strict_any_check = 0;
	TOK rec_oper;		// value of oper for recursive calls to type::check
				//	oper, 255, or PT_OVERLOAD
	TOK rec_oper0;		// value of oper for recursive calls to type::check
				//	0, 255, or PT_OVERLOAD

	if (t1==0 || t2==0)
		error('i',"check(%p,%p,%d)",t1,t2,oper);

	if(t1==t2)
		return 0;

	switch(oper) {
	case VIRTUAL:
		vv = 1;
		Vcheckerror = 0;
		oper = 0;
		break;
	case OVERLOAD:
		over = 1;
		oper = 0;
		break;
	case PT_OVERLOAD:
		over = 1;
		// no break
	case 255:
		oper = 0;
		strict_any_check=1;
		break;
	}

	if(level==0) {
		const_problem = 0;
		return_error = 0;
		pt_ptm = 0;
		pt_over = over;
	}

	rec_oper = strict_any_check ? ( over ? PT_OVERLOAD : 255 ) : oper;
	rec_oper0 = strict_any_check ? ( over ? PT_OVERLOAD : 255 ) : 0;

	t1 = t1->skiptypedefs(cnst1);

/*
**	GLOG: the following test on ANY must be done before unrolling
**	t2, to accommodate templates (in a way I don't yet understand)
*/
	if (t1->base == ANY || t2->base == ANY)
		if (over==0 || strict_any_check==0) 
			return strict_any_check ? t1!=t2 : 0;

	t2 = t2->skiptypedefs(cnst2);

	if(t1==t2)
		goto const_check;

	b1 = t1->base;
	b2 = t2->base;

	if (b1 != b2) {
		switch (b1) {
		case PTR:
			switch (b2) {
			case VEC:
				if (
					level>0
					||
					(oper == 0 || oper == IGNORE_CONST)  
					&&
					over==0
					||
					Pptr(t1)->memof
					||
					Pptr(t1)->ptname
					||
					Pptr(t1)->typ->check(Pvec(t2)->typ,rec_oper,level+1)
				)
					return 1;

				goto const_check;

			case FCT:
				if (
					level>0
					||
					Pptr(t1)->typ->check(t2,rec_oper,level+1)
				)
					return 1;
				goto const_check;
			}
			break;

		case FCT:
			if( b2==PTR ) {
				if (
					level>0
					||
					t1->check(Pptr(t2)->typ,rec_oper,level+1)
				)
					return 1;
				goto const_check;
			}
			break;

		case VEC:
			if (b2==PTR) {
				if (
					level>0
					||
					(oper==0 || oper == IGNORE_CONST)  
					&&
					over==0
					||
					Pptr(t2)->memof
					||
					Pptr(t2)->ptname
					||
					Pvec(t1)->typ->check(Pptr(t2)->typ,rec_oper,level+1)
				)
					return 1;

				goto const_check;
			}
			break;
		}

		if(level>0) {
			if((oper != 0 && oper != IGNORE_CONST) && b1==VOID && level==1) {
				if(b2==FCT) {
					Pfct f = Pfct(t2);
					if(f->memof && f->f_static==0)
						return 1;
				}
				goto const_check;
			}
			return 1;
		}

		switch (oper) {
		case 0:
		case IGNORE_CONST:	
			if (
				b2 == ZTYPE && b1==INT && Pbase(t1)->b_unsigned==0
				||
				b1 == ZTYPE && b2==INT && Pbase(t2)->b_unsigned==0
			)
				goto const_check;
			return 1;
		case ARG:
		case ASSIGN:
		case RETURN:
		case COERCE:
			switch (b1) {
			case ZTYPE:
			case CHAR:
			case SHORT:
			case INT:
			case LONG:
			case VLONG:
			case FLOAT:
			case DOUBLE:
			case LDOUBLE:
			case FIELD:
				switch (b2) {
				case LONG:
				case VLONG:
				case FLOAT:
				case DOUBLE:
				case LDOUBLE:
				case EOBJ:
				case ZTYPE:
				case CHAR:
				case SHORT:
				case INT:
				case FIELD:
					if(oper==COERCE)
						Nstd++;
					goto const_check;
				}
				return 1;
			case PTR:
			case VEC:
				if (b2==ZTYPE) {
					if(oper==COERCE)
						Nstd++;
					goto const_check;
				}
			case RPTR:
			case COBJ:
			case FCT:
			case EOBJ:
			default:
				return 1;
			}
		}
		goto const_check;
	}

	switch (b1) {
	case VEC:
		if (
			Pvec(t1)->size!=Pvec(t2)->size
			&&
			(
			level>0
			||
			(oper==0 || oper==IGNORE_CONST)	
			&&
			strict_any_check==0
			&&
			Pvec(t1)->size
			&&
			Pvec(t2)->size
			)
		)
			return 1;

		if(Pvec(t1)->typ->check(Pvec(t2)->typ,rec_oper,level+1))
			return 1;
		break;

	case PTR:
	case RPTR:
	{
		Pptr p1 = Pptr(t1);
		Pptr p2 = Pptr(t2);

		if ((p1->ptname && p2->ptname) && (!p1->memof || !p2->memof))
			return 1;

		if (!same_class(p1->memof,p2->memof)) {
			
			// T::* requires we defer setting up memof
			// until instantiation of the Template type T
			// ptname holds the formal parameter T
			// can't merge with other if because of memof usage
			if(
				p1->memof==0 && p1->ptname
				||
				p2->memof==0 && p2->ptname
			)
			 	pt_ptm = 1; 
			else	
			if(
				p1->memof==0 
				||
				p2->memof==0
				||
				(p1->memof->baseof(p2->memof)==0 &&
				 same_class(p1->memof,p2->memof,1)==0)
			)
				return 1;

			if (pt_ptm == 0 && (oper==0 || oper==IGNORE_CONST) && 
			    same_class(p1->memof,p2->memof)==0)
				return 1;

			if(oper==COERCE)
				Nstd++;
		}

#if 0
		if (!level && (oper == 0 || oper == ASSIGN || oper == COERCE ||
		    oper == ARG || oper == RETURN)) {
			Ptype t11;
			Ptype t22;
			Ptype ta;
			Ptype tb;
			int i = 0;
			int j = 0;
			int k = 0;
			ta = t1;
			while ((ta->base == PTR || ta->base == RPTR) &&
			    (tb = ta->is_ptr_or_ref())) {
				ta = Pptr(tb)->typ;
				i++;
				k += ta->tconst();
			}
			t11 = ta;
			ta = t2;
			while ((ta->base == PTR || ta->base == RPTR) &&
			    (tb = ta->is_ptr_or_ref())) {
				ta = Pptr(tb)->typ;
				j++;
			}
			t22 = ta;
			if (i >= 2 && j == i && t11->tconst() &&
			    !t22->tconst() && k != i)
				return const_problem = 1;
		}
#endif

		if(p1->typ->check(p2->typ,rec_oper,level+1))
			return 1;

		break;
	}

	case FCT:
	{
		Pfct f1 = Pfct(t1);
		Pfct f2 = Pfct(t2);
		Pname a1 = f1->argtype;
		Pname a2 = f2->argtype;
		TOK k1 = f1->nargs_known;
		TOK k2 = f2->nargs_known;
		int n1 = f1->nargs;
		int n2 = f2->nargs;
		// if pt_ptm, want to check arguments and return type
		// but template ptm has no memof until instantiation
		if (!same_class(f1->memof,f2->memof) && pt_ptm == 0) {
			if (f1->memof==0 && f2->f_static)
				goto sss;
			if (vv == 0)	// match even if private base class
				if (
					f1->memof==0
					||
					f2->memof==0
					||
					(
					    level > 1
					    ||
					    f1->memof->baseof(f2->memof)==0
					)
					&&
					same_class(f1->memof,f2->memof)==0
				)
					return 1;
			if(oper==COERCE)
				Nstd++;
			sss:;	//SSS
		}

		if (k1 != k2)
			return 1;

		if (n1!=n2 && k1 && k2) {
			goto aaa;
		}
		else if (a1 && a2) {
			while (a1 && a2) {
				if (a1->tp->check(a2->tp,rec_oper0,level+1))
					return 1;
				a1 = a1->n_list;
				a2 = a2->n_list;
			}
			if (a1 || a2)
				goto aaa;
		}
		else if (a1 || a2) {
		aaa:
			if (k1 == ELLIPSIS) {
				switch (oper) {
				case 0:
				case IGNORE_CONST:	
					if (a2 && k2==0)
						break;
					return 1;
				case ASSIGN:
					if (a2 && k2==0)
						break;
					return 1;
				case ARG:
					if (a1)
						return 1;
					break;
				case COERCE:
					return 1;
				}
			}
			else if (k2 == ELLIPSIS) {
				return 1;
			}
			else if (k1 || k2) {
				return 1;
			}
		}

		cnst1 = f2->f_const;
		cnst2 = f1->f_const;

		if(f1->returns->check(f2->returns,rec_oper0,level+1)) {
			if(vv && cnst1==cnst2) {
				bit fail = 1;
				Ptype t1 = f1->returns;
				Ptype t2 = f2->returns;
				if ((t1->is_ptr() && t2->is_ptr()) ||
				    (t1->is_ref() && t2->is_ref())) {
					t1 = Pptr(t1->is_ptr_or_ref())->typ;
					t2 = Pptr(t2->is_ptr_or_ref())->typ;
					if (!t1->is_ptr_or_ref() &&
					    !t2->is_ptr_or_ref()) {
						t1 = t1->skiptypedefs();
						t2 = t2->skiptypedefs();
						if (t1->base == COBJ &&
						    t2->base == COBJ) {
							Pclass c1 = t1->classtype();
							Pclass c2 = t2->classtype();
							Nvis = 0;
							if (c2->has_base(c1,0,1) && !Nvis)
								fail = 0;
						}
					}
				}
				if (fail)
					Vcheckerror = 1;
				else
					break;
			}
			if (rec_oper0 == PT_OVERLOAD && level == 0)
				return_error = 1;
			return 1;
		}

		break;
	}

	case FIELD:
		switch (oper) {
		case 0:
		case IGNORE_CONST:	
		case ARG:
			error('i',"check field?");
		}
		return 0;

	case FLOAT:
	case DOUBLE:
	case LDOUBLE:
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case VLONG:
		if (Pbase(t1)->b_unsigned != Pbase(t2)->b_unsigned) {
			if (level>0 || (oper==0 || oper==IGNORE_CONST))
				return 1;
			if (oper==COERCE)
				Nstd++;
		}
		goto const_check;

	case EOBJ:
		if (Pbase(t1)->b_name->tp != Pbase(t2)->b_name->tp)
			return 1;
		goto const_check;

	case CLASS:
	case COBJ:
	{
		Pname n1, n2;
		if (b1 == COBJ) {
			n1 = Pbase(t1)->b_name;
			n2 = Pbase(t2)->b_name;
			if (n1 == n2)
				goto const_check;
		}

		// once again, a more comprehensive check for classes,
		// since they may be parameterized.
		// same_class: handles class templates: instantiated
		// fm_same_class: handles matching uninstantiated templates
		// 	used as formal arguments for template functions

		// hack: see comment on classdef::same_class in template.c
		extern int is_arg;
		int access = template_hier || pt_over;
		if (
			same_class(Pclass(b1==COBJ?n1->tp:t1),Pclass(b1==COBJ?n2->tp:t2),access||is_arg)
			||
			rec_oper==PT_OVERLOAD
			&&
			fm_same_class(Pclass(b1==COBJ?n1->tp:t1),Pclass(b1==COBJ?n2->tp:t2))
		)
			goto const_check;

		// permit a derived class to match public base class
		if (template_hier != 0)
			goto pt_hack;

		switch (oper) {
		case ARG:
		case ASSIGN:
		case RETURN:
		case COERCE:
		{
		pt_hack:
			ppbase = PUBLIC;
			if (level<=1 && ((Pclass(b1==COBJ?n2->tp:t2))->is_base(b1==COBJ?n1->string:Pclass(t1)->string))) {
				if (ppbase!=PUBLIC) {
					const_problem = 0;
					return 1;	// private or protected base
				}
				if(oper==COERCE)
					Nstd++;
				goto const_check;
			}
		}
			// no break
		case 0:
		case IGNORE_CONST:	
			const_problem = 0;
			return 1;
		}

		goto const_check;
	}

	case ZTYPE:
	case VOID:
		goto const_check;
	default:
		error('i',"T::check(o=%d %d %d)",oper,b1,b2);
	}

const_check:


	if(cnst1==cnst2)
		return 0;

	switch(oper) {
	case IGNORE_CONST:	
		return 0;//ignore "const"ness for oper=IGNORE_CONST

	case 0:
		const_problem=1;
		return 1;

	case ASSIGN:
	case COERCE:
	case ARG:
	case RETURN:
		if(level>0) {
			if(cnst2)
				const_problem=1;
			return cnst2;
		}
		return 0;
	default:
		error('i',"oper = %k in type::check()",oper);
	}
	return 0;
}
