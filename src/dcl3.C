/*ident	"@(#)cls4:src/dcl3.c	1.30" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
#include "cfront.h"
#include "size.h"
#include "template.h"
#include <ctype.h>

Pblock top_block;
int New_in_arg_list =0;

// check for 0 as ptr to mem initializer
static int is_zero(Pexpr e)
{
	if (!e)
		return 0;
	if (e->base == ZERO)
		return 1;
	if (e->base != CAST && e->base != G_CAST)
		return 0;
	return e->e1->base == ZERO;
}

static void vbase_pointers(Pname fn, Pclass cl)
/*
	insert argument for virtual base pointers (if any)
	after f_this and before f_argtype
*/
{
//error('d',"vbase_pointers(%n,%t) %d %k",fn,cl,fn->tp,fn->n_oper);
	Pfct f = Pfct(fn->tp);
	if (fn->n_oper == CTOR) {
		Pname d = 0;
		for (Pbcl b = cl->baselist; b; b=b->next) {
			if (b->base != VIRTUAL)
				continue;
			Pname a = new name(b->bclass->string);
			a->tp = b->bclass->this_type;
			a->n_list = d;
			a->n_table = f->body ? f->body->memtbl : 0;
			a->where = fn->where;
			d = a;
		}	
				
		if (d) {
			for (Pname dd =d;;) {
				if (d->n_list == 0) {
					d->n_list = f->f_args->n_list;
					break;
				}
				d = d->n_list;
			}
			f->f_args->n_list = dd;
		}
	}
	if (fn->n_oper == DTOR) {	// add __free argument
//error('d',"add __free to%n",fn);
		Pname fa = new name;
		fa->tp = int_type;
		fa->n_scope = ARG;
		fa->where = fn->where;

		Pname a = f->f_args;
		if (a == 0)
			f->f_args = fa;
		else {
			for(;;a = a->n_list) {
// error('d',"a %d %t",a,a->tp);
				if (a->n_list == 0) {
					a->n_list = fa;
					break;
				}
			}
		}
	}
}

void make_res(Pfct f)
/*
	returns X where X(X&) has been declared
	add "_result" argument of type X*
*/
{
	Pname cl = f->returns->is_cl_obj();
	if (cl==0 || Pclass(cl->tp)->has_itor()==0)
		return;

	Pname rv = new name("_result");
	rv->tp = f->returns->addrof();
	PERM(rv);
	PERM(rv->tp);
	rv->n_scope = FCT;	// not a ``real'' argument
	rv->n_used = 1;
	rv->n_list = f->argtype;
	if (f->f_this)
		f->f_this->n_list = rv;
	else
		f->f_args = rv;
	f->f_result = rv;
	f->s_returns = void_type;
}

void name::check_oper(Pname cn)
/*
	check declarations of operators, ctors, dtors
*/
{
DB(if(Ddebug>=2) error('d', &where, "%n->check_oper(%n ): n_oper:%k", this, cn, n_oper ); );
	switch (n_oper) {
	case CALL:
	case DEREF:
	case REF:
		if (cn == 0)
			error("operator%s must be aM",keys[n_oper]);
		break;
	case ASPLUS:
	case ASMINUS:
	case ASMUL:
	case ASDIV:
	case ASMOD:
	case ASAND:
	case ASOR:
	case ASER:
	case ASLS:
	case ASRS:
		if ( warning_opt ) {
			if ( cn == 0 || Pfct(tp)->f_static )
				error('w', "operator%s should be a non-staticMF",keys[n_oper]);
		}
		break;
	case ASSIGN:
		if (cn == 0) 
			error("non-member operator%k()",n_oper);
		break;
	case LT: case LE: case GT: case GE:case EQ:case NE:
		if (cn && Pfct(tp)->f_static) {
			error("operator%s cannot be static",keys[n_oper]);
			Pfct(tp)->f_static=0;
		}
		break;
	case NOT: 	/* unary operators only */
	case COMPL:
		{
		Pfct f = Pfct(tp);
		if (cn && f->argtype) 
			error("%n::%n takes noAs",cn, this);
		else if (f->nargs == 2)
			error("%n takes 1A only",this);
		if (cn && Pfct(tp)->f_static) {
			error("operator%s cannot be static",keys[n_oper]);
			Pfct(tp)->f_static=0;
		}
		break;
		}
	case INCR:
	case DECR:
		// check for postscript instance
		{
		Pfct f = Pfct(tp);
		if (cn) {	// member
			if ( f->argtype && f->nargs == 1 ) {
				Pname n = f->argtype;
				Ptype at=n->tp->skiptypedefs();
				if ( at->base != INT || Pbase(at)->b_unsigned)
					error("%n must takeA ofT int, not %t",this,n->tp);
			}
		}
		else if (f->nargs == 2) {	// non-member
			Pname n = f->argtype->n_list;
			Ptype at=n->tp->skiptypedefs();
			if ( at->base != INT || Pbase(at)->b_unsigned)
				error("%n must takeA ofT int, not %t",this,n->tp);
		}
		break;
		}
	case 0:
	case TNAME:		// may be a constructor
		if (
			cn
			&&
			(
				strcmp(cn->string,string)==0
				||
				Pclass(cn->tp)->class_base == INSTANTIATED
				&&
				strcmp(string, Ptclass(cn->tp)->unparametrized_tname()->string) == 0
			)
		) 
		{
			if (tp->base == FCT) {
				Pfct f = Pfct(tp);
				if (f->returns!=defa_type)
					error("%s::%s()W returnT",string,string);
				f->returns = void_type;
				string = "__ct";
				n_oper = CTOR;
			}
			else
				error('s',"struct%nM%n",cn,cn);
		}
		else
			n_oper = 0;
		break;

	case DTOR:	/* must be a destructor */
//error('d',"dtor %s",string);
		if (cn == 0) {
			n_oper = 0;
			error("destructor ~%s() not inC",string);
		}
		else if (
			strcmp(cn->string,string) == 0
			||
			Pclass(cn->tp)->class_base == INSTANTIATED
			&&
			strcmp(string, Ptclass(cn->tp)->unparametrized_tname()->string)==0
		) {
		dto:
			Pfct f = (Pfct)tp;
			string = "__dt";
			if (tp->base != FCT) {
				error("%s::~%s notF",cn->string,cn->string);
				tp = new fct(void_type,0,1);
			}
			else if (f->returns!=defa_type) {
				if (
					f->returns != void_type
					||
					f->body != 0
					||
					friend_in_class == 0
				) 
					error("%s::~%s()W returnT",cn->string,cn->string);
			}

			if (f->argtype || f->nargs_known == ELLIPSIS) {
				error("%s::~%s()WAs",cn->string,cn->string);
				f->nargs = 0;
				f->nargs_known = 1;
				f->argtype = 0;
			}
			f->returns = void_type;
		}
		else {
			if (strcmp(string,"__dt") == 0)
				goto dto;
			error("~%s in %s",string,cn->string);
			n_oper = 0;
		}
		break;

	case TYPE: // cond stores the type of the operator function
//error('d',"this%n cn%n cond%t tp%t",this,cn,cond,tp);
		if (cn == 0) {
			error("operator%t() not aM",Ptype(cond));
			n_oper = 0;
			cond = 0;
		}
		else	// note: could be called twice in friend declaration
			//    -- only process once
		if ( cond ) {
			Pfct f = Pfct(tp);
			Ptype tx = Ptype(cond);
			cond = 0;
			if (tx->skiptypedefs()->base==FCT)
				error("badT for%n::operator%t() - cannot return aF",cn,tx);
			if (f->base != FCT)
				error("badT for%n::operator%t()",cn,tx);
			if (f->returns != defa_type) {
				error("resultT for%n::operator%t()",cn,tx);
				DEL(f->returns);
			}
			if (f->argtype || f->nargs_known == ELLIPSIS) {
				error("%n::operator%t()WAs",cn,tx);
				f->argtype = 0;
			}
			f->returns = tx;
			char buf[1024];
			char* bb = tx->signature(buf);
			int l2 = bb-buf;
			if (1023<l2)
				error('i',"N::check_oper():N buffer overflow");
			char* p = new char[l2+5];
			p[0] = '_';
			p[1] = '_';
			p[2] = 'o';
			p[3] = 'p';
			strcpy(p+4,buf);
			string = p;
		}
		break;
	}
}

Pexpr vbase_args(Pfct a, Pname bn)
/*
	constructor a calls the constructor bn for a base class
	generate argument list needed for virtual base arguments
*/
{
	Pfct b = Pfct(bn->tp);
//error('d',"vbase_args%n: %t %k",bn,b,b->base);
	Pexpr args = 0;
	Pexpr tail = 0;
	if (b->base == OVERLOAD)
		b = Pfct(Pgen(b)->fct_list->f->tp);	// doesn't matter which
	for (Pname d = b->f_args->n_list; d!=b->argtype; d=d->n_list) {
		for (Pname dd = a->f_args->n_list; dd; dd=dd->n_list)
			if (strcmp(dd->string,d->string)==0)	// using strcmp is a trick
				break;

		Pexpr aa = new expr(ELIST,dd,0);
		if (args == 0)
			args = aa;
		else
			tail->e2 = aa;
		tail = aa;
	}
	return args;
}

void fct::init_bases(Pclass cl, Pexpr)
/*
	in "cl"'s constructor "this" generate code to initialize base classes
	and members using the initializers "f->f_init"
	
	this->f_init == list of names of classes to be initialized
		COLON(b)	=> base class b
				=> constructor call in f_init->n_initializer
		COLON()		=> unnamed base class
				=> constructor call in f_init->n_initializer
		NAME(m)		=> member m
				=> constructor call in m->n_initializer
*/
{
	Ptable ftbl = body->memtbl;
	DB( if(Ddebug>=1) error('d',"init_bases %t init %d",cl,f_init); );

	// explicit initializers
	if ( cl && cl->csu == UNION && f_init && f_init->n_list )
		error(&f_init->where,"multipleIrs in unionK %s:: %s",cl->string,cl->string);
	for (Pname nx, nn=f_init; nn; delete nn,(nn=nx) ) {
		Pexpr i = nn->n_initializer;
		nn->n_initializer = 0;
		nx = nn->n_list;

// error('d',"init_base %s %d",nn->string,i);
		if (nn->string) {
			// lookup in case type name hides a "real" member
			{
				Pname mmm = cl->memtbl->look(nn->string,0);
				if ( mmm )
					nn->base = mmm->base;
			}
			if (nn->base == TNAME) {	// base class
				char *bn;
				while ( nn->tp && nn->tp->base == TYPE )
					nn->tp = Pbase(nn->tp)->b_name->tp;
				if ( nn->tp && nn->tp->base == COBJ )
					bn = Pbase(nn->tp)->b_name->string;
				else
					bn = nn->string;
				for (Pbcl l = cl->baselist; l; l=l->next) {
					Pclass bcl = l->bclass;
					if (
						strcmp(bcl->string,bn) == 0
						||
						bcl->class_base == INSTANTIATED
						&&
						strcmp(Ptclass(bcl)->inst->def->classtype()->string,bn) == 0
					) {
						// l->init is zeroed out in ctor_simpl
						// if error_count, simpl() not invoked
						if (l->init && error_count == 0) 
							error("twoIrs for%t",bcl);
						else
							l->init = base_init(bcl,i,ftbl,l->obj_offset);
						goto con;
					}
				}
				if (nn->base == TNAME && !nn->tpdef &&
				    (!nn->tp || (nn->tp->base != COBJ &&
				    nn->tp->base != EOBJ)))
					error(&nn->where,"unexpectedAL: noBC %s", nn->string);
				else
					error(&nn->where,"unexpectedAL: noBC%n",nn);
			con:
				continue;
			}
			else {		// member initializer
				Pname m = cl->memtbl->look(nn->string,0);
				if (m && m->n_table==cl->memtbl)
					m->n_initializer = mem_init(m,i,ftbl);
				else
					error(&nn->where,"%n not inC %s",nn,cl->string);
			}
		}
		else {			// unnamed base class
			Pbcl l = cl->baselist;
			if (l == 0) {
				error("unexpectedAL: noBC called");
				continue;
			}

			if (l->next) {
				bit cnt = 0, rvb = 0; // remote virtual base classes
				for (Pbcl ll = l; ll; ll = ll->next, ++cnt ) 
					if (ll->base==VIRTUAL && ll->promoted)
						++rvb;
	 			if ( rvb ) 
					error("unnamedBCIr: %dBCes(%d non-explicit virtualBC%s)",cnt,rvb,rvb==1?"":"es");
				else error("unnamedBCIr: %dBCes",cnt);
				continue;
			}
			if (l->init)
				error("twoIrs for%t",l->bclass);
			else {
				error(strict_opt?0:'w',&nn->where,"N ofBC%t missing from BCIr (anachronism)",l->bclass);
				l->init = base_init(l->bclass,i,ftbl,l->obj_offset);
			}
		}
	}	// for

	for (Pbcl l = cl->baselist; l; l=l->next) {
		// default initialization of base classes
		Pname ctor;
		Pclass bcl = l->bclass;
		if (l->init==0 && (ctor=bcl->has_ctor()))
			l->init = base_init(bcl,0,ftbl,l->obj_offset);
	}
}

int inline_restr;	/* report use of constructs that the inline expanded cannot handle here */

void fct::dcl(Pname n)
{
	int nmem = TBLSIZE;
	Pname a;
	Pname ll;
	Ptable ftbl;

	int const_old = const_save;

	int bit_old = bit_offset;
	int byte_old = byte_offset;
	int max_old = max_align;

	if (base != FCT)
		error('i',"F::dcl(%d)",base);
	if (body == 0)
		error('i',"F::dcl(body=%d)",body);
	if (n==0 || n->base!=NAME)
		error('i',"F::dcl(N=%d %d)",n,(n)?n->base:0);
	DB( if(Ddebug>=1) error('d',&n->where,"fct::dcl(%n) %k %d %t",n,n->n_scope,body->own_tbl,this); );
	if (body->own_tbl)
		return;	// done already

	if (body->memtbl == 0)
		body->memtbl = new table(nmem+3,gtbl,0);
	body->own_tbl = 1;
	ftbl = body->memtbl;
	ftbl->real_block = body;
	extern void for_check_delete();
	for_check_delete();

	max_align = 0;		//AL_FRAME;
	bit_offset = 0;

	cc->stack();
	cc->nof = n;
	cc->ftbl = ftbl;

	switch (n->n_scope) {
	case 0:
	case PUBLIC:
	{
		cc->not = n->n_table->t_name;
		cc->cot = Pclass(cc->not->tp);
		cc->tot = cc->cot->this_type;
		if (f_this)
			f_this->n_table = ftbl;		// fake for inline printout
		cc->c_this = f_this;

		Pclass cl = Pclass(cc->not->tp);

		if (
			cl->c_body==3
			&&
			n->n_initializer==0
			&&
			n->n_sto!=STATIC
			&&
			f_inline==0
			&&
			f_imeasure==0
			&&
			f_virtual!=0
		) {		// could be the function where we need to
				// output the vtbl
			int i;
			for (Pname nn=cl->memtbl->get_mem(i=1); nn; NEXT_NAME(cl->memtbl,nn,i)) {
				if ( nn->base == TNAME ) continue;
				Ptype t = nn->tp;

				if (t==0)
					continue;

				switch (t->base) {
				case FCT:
					if (nn == n)
						goto prnt;
					if (
						nn->n_initializer
						||
						nn->n_sto==STATIC
						||
						Pfct(nn->tp)->f_inline
						||
						Pfct(nn->tp)->f_imeasure
						||
						Pfct(nn->tp)->f_virtual==0
					)
						break;
					goto zaq;

				case OVERLOAD:
				{
					for (Plist gl=Pgen(t)->fct_list; gl; gl=gl->l) {
						Pname nn = gl->f;
						if (nn == n)
							goto prnt;
						if (
							nn->n_initializer
							||
							nn->n_sto==STATIC
							||
							Pfct(nn->tp)->f_inline
							||
							Pfct(nn->tp)->f_imeasure
							||
							Pfct(nn->tp)->f_virtual==0
						)
							continue;
						goto zaq;
					}
				}
				}
			}
			goto zaq;
		prnt:
			cl->print_all_vtbls(cl);
			goto zaq;
		}
	}
	}
zaq:					// protect against: class x; x f(); class x { x(x&); ....
	if (f_result == 0)
		make_res(this);
	if (f_result)
		f_result->n_table = ftbl;		// fake for inline printout

	returns->tsizeof();	// make sure size is known

	Pname ax;
	for (a=argtype, ll=0; a; a=ax) {
		ax = a->n_list;
		Pname nn = a->dcl(ftbl,ARG);
		++nn->lex_level;
		Pname cn = nn->tp->is_cl_obj();
		if (cn == 0)
			cn = cl_obj_vec;
		if (cn)
			(void)cn->tp->tsizeof();	// make sure it is printed
		nn->n_assigned_to = nn->n_used = nn->n_addr_taken = 0;
		nn->n_list = 0;

		switch (nn->tp->base) {
		case CLASS:
		case ENUM:	/* unlink types declared in arg list */
			nn->dcl_print(0);
			break;
		default:
			if (ll)
				ll->n_list = nn;
			else
				f_args = argtype = nn;
			ll = nn;
		}
		delete a;
	}

	if (f_result) {		// link in f_result
		f_args = f_result;
		f_result->n_list = argtype;
	}

	if (f_this) {		// link in f_this
		if (body)
			f_this->where.line = n->where.line;
		f_args = f_this;
		f_this->n_list = f_result ? f_result : argtype;
	}

	if (n->n_oper==CTOR || n->n_oper==DTOR)
		vbase_pointers(n,cc->cot);

	if (n->n_oper == CTOR) {
		const_save = 1;
		init_bases(cc->cot,f_init);
	}
	else if (f_init)
		error(&f_init->where,"unexpectedAL: not aK");

	PERM(returns);
	const_save = f_inline&&debug_opt==0;
	inline_restr = 0;
	top_block = body;

	if (	se_opt && 
		(n->inst_body()==0 || n->finst_body()==0) 
	)
		suppress_error++;

	body->dcl(ftbl);

	if (	se_opt && 
		(n->inst_body()==0 || n->finst_body()==0)
	)
		suppress_error--;
	
	defined |= DEFINED;
	if (f_inline && inline_restr && returns->base!=VOID) {
		f_inline = 0;
		char* s = (inline_restr & 32) ? "continue"
			: (inline_restr & 16) ? "break"
			: (inline_restr & 8) ? "loop"
			: (inline_restr & 4) ? "switch"
			: (inline_restr & 2) ? "goto"
			: (inline_restr & 1) ? "label"
			: "" ;
		if (warning_opt) {
			error('w', "\"inline\" ignored,%n contains %s",n,s);
			error('w', "out-of-line copy of%n created",n);
		}
		n->simpl();	//BS6
                if ( cc->cot &&
                        cc->cot->class_base == INSTANTIATED &&
                        cc->cot->c_body == 1 ) {
                                current_instantiation = cc->cot;
                                cc->not->dcl_print(0);
                                current_instantiation = 0;
                }
		n->dcl_print(0);
	}
	const_save = const_old;

	if (f_inline && debug_opt==0)
	{
		if (dtpt_opt && n->inst_body()==0) {
			f_inline=0;
			n->fct_type()->body=0; 
		}
		isf_list = new name_list(n,isf_list);
	}

	bit_offset = bit_old;
	byte_offset = byte_old;
	max_align = max_old;
	cc->unstack();

//error('d',"fct-> returns %t",returns);
}

Pexpr this_handler = 0;
Pexpr fct::base_init(Pclass bcl, Pexpr i, Ptable ftbl, int offset)
/*
	have base class bcl and expr list i
	return "( *(base*)this ) . ctor( i )"
	ctor call generated in expr.typ()
*/
{
	Ptype ty = bcl->this_type;
	Pexpr th = rptr(ty,f_this,offset);		// base*
	Pname ctor = bcl->has_ctor();

// error('d',"fct::B_init(C %t, i %d, %d) ctor%n",bcl,i,i?i->tp:0,ctor);
	Pexpr ii = (i && i->base==ELIST)?i->e1:i;

	if ( ii &&
	     ii->base==DEREF &&
	     (ii->e1->base==CAST || ii->e1->base==G_CAST) &&
    	     (th->base==CAST || th->base==G_CAST) )
			th->i2 = ii->e1->i2;

	if (ctor == 0) {
		if (i && i->base!=ELIST)
			i = new expr(ELIST,i,0);

		Pexpr v = new texpr(VALUE,bcl,i);	// ?.base(i)
		v->e2 = new expr(DEREF,th,0);		// (*(base*)this).base(i)
		v = v->typ(ftbl);			// *base(&*(base*)this,i)
//error('d',"v %k",v->base);
		switch (v->base) {
		case DEREF:
			return v->e1;			// base(&*(base*)this,i)
		case ASSIGN:				// degenerate base(base&): *(base*)this=i
			th = new texpr(G_CAST,ty,f_this);
			v = new expr(CM,v,th);		// (*(base*)this=i,(base*)this);
			return v->typ(ftbl);
		default:
			return 0;
		}
	}

	Pname icn;
	if (i) {
		int na = (i->base != ELIST ? 1 : (i->e2 != 0 ? 2 : 1));
		ii = ii->typ(ftbl);
		if (
			bcl->has_itor()==0
			&&
			(icn=ii->tp->is_cl_obj())
			&&
			na == 1
			&&
			(
				same_class(Pclass(icn->tp),bcl)
				||
				Pclass(icn->tp)->has_base(bcl)
			)
		) {
			// degenerate base(base&): *(base*)this=i
			// memberwise copy
//error('d',"copy %t",ty);
			this_handler = th;
			th = new texpr(G_CAST,ty,f_this);
			th = th->contents();
			th = th->typ(ftbl);
			if (!same_class(Pclass(icn->tp),bcl)) {	// cast needed
				Pptr r = new ptr(RPTR,Pptr(ty)->typ);
				ii = new texpr(G_CAST,r,ii);
				ii = ii->typ(ftbl);
			}
			ii = new expr(ASSIGN,th,ii);
			ii->tp = th->tp;
					// simulate `return this':
					// *(base*)this=i,(base*)this
//			ii = new expr(CM,ii,new cast(ty,f_this));
//			ii->tp = th->tp;

			ii = new expr(CM,ii,this_handler);
			this_handler = 0;
			ii->tp = ty;

			return ii;
		}
		if (i->base == ELIST)
			i->e1 = ii;
	}
//error('d',"call%n%t -> %d%k",ctor,ctor->tp,x,x->base);
	return call_ctor(ftbl,th,ctor,i,REF,vbase_args(this,ctor));
}


Pexpr fct::mem_init(Pname mn, Pexpr i, Ptable ftbl)
/*
	return "member_ctor( m, i )"
*/
{
// a new entry for B::B_pub, in general, has no tp and no
// real info: all the tp-> only work on our systems because
// 0 pointer dereference isn't system memory. it core dumps
// in set_const since no test is made on this == 0.
//error('d',"mem_init%n",mn);

//	if (mn->n_assigned_to) // Disallow multiple member initializations
//		error("%n multiply initialized",mn);
	switch (mn->n_stclass) {
	case STATIC:
		error("MIr for static%n",mn);
		break;
	case ENUM:
		error("MIr for enumeration constant%n", mn);
		break;
	}

	Pname member =	(mn->base==PUBLIC && mn->n_qualifier)
			?
			mn->n_qualifier
			:
			mn;

	if (i)
		i = i->typ(ftbl);

	Pname cn = member->tp->is_cl_obj();	// first find the class name
	Pref tn = new ref(REF,f_this,member);

	tn->tp = member->tp;
//error('d',"MI for%n%t =%t",member,member->tp,i?i->tp:0);
//error('d',"fthis %d %t member%n tp%t",f_this,f_this->tp,member,tn->tp);
	if (cn) {
		Pclass mcl = Pclass(cn->tp);	// then find the classdef
		Pname ctor = mcl->has_ctor();
		Pname icn;

		if (
			i
			&&
			mcl->has_itor()==0
			&&
			(icn=i->tp->is_cl_obj())
			&&
			same_class(Pclass(icn->tp),mcl)
		) {					// bitwise copy
			Pexpr init = new expr(ASSIGN,tn,i);
			init->tp = tn->tp;
			member->assign();
			return init;
		}

		if (ctor)
			return call_ctor(ftbl,tn,ctor,i,DOT);

		error("Ir forM%nW noK",member);
		return 0;
	}

	if (cl_obj_vec) {
		if (i && i->base == ELIST)
			error("illegalIrL for %t%nWinM initializationL",mn->tp,mn);
		else
			error('s',"Ir forCM %t%nWK",mn->tp,mn);
		return 0;
	}

	if (i && i->base == ELIST) {
		if (i->e2)
			error("Ir for%n not a simpleE",member);
		i = i->e1;
	}

	if (member->tp->is_ref() && (i == 0)) {
		error("emptyIr for reference%n", member);
		return 0 ;
	}

// error( 'd', "fct_mem_init:%n%k", member, member->tp->base );
	switch (member->tp->base) {
	case VEC:
	case FCT:
	case OVERLOAD:
		error("Ir for%n ofT %t",member,member->tp);
		return 0;
	}

//error('d',"tp %t",member->tp);
	if (member->tp->tconst()) {
		ignore_const++;
		i = new expr(ASSIGN,tn,i);
		i = i->typ(ftbl);
		ignore_const--;
		return i;
	}

	Pptr pt;
	if (pt = member->tp->is_ref()) {
		switch (pt->typ->base) {
		case FCT:
		case OVERLOAD:
			i = ptr_init(pt,i,ftbl);
			break;
		default:
			i = ref_init(pt,i,ftbl);
		}
		i = new expr(ASSIGN,tn,i);
		i->tp = tn->tp;
		member->assign();	// cannot call typ: would cause dereference
		return i;
	}

	i = new expr(ASSIGN,tn,i);
	return i->typ(ftbl);		// typ performs the type check on the assignment
}

Pexpr replace_temp(Pexpr e, Pexpr n)
/*
	e is on the form
				f(&temp,arg) , temp
			or
				&temp->ctor(arg) , temp
			or
				x->f(&temp,arg) , temp
	change it to
				f(n,arg)
			or
				n->ctor(arg)
*/
{
	Pexpr c = e->e1;	// f(&temp,arg) or &temp->ctor(args)
	Pexpr ff = c->e1;
	Pexpr a = c->e2;	// maybe ELIST(&temp,arg)
	Pexpr tmp = e->e2;

//error('d',"suppress(%d%k)%n",tmp->base,tmp->base,tmp->base==NAME?tmp:0);
	if (tmp->base==DEREF)
		tmp = tmp->e1;
	if (tmp->base==CAST || tmp->base==G_CAST)
		tmp = tmp->e1;
	if (tmp->base==ADDROF || tmp->base==G_ADDROF)
		tmp = tmp->e2;
	if (tmp->base != NAME)
		return e;		//error('i',"replace %k",tmp->base);
	tmp->tp = any_type;		// temporary not used: suppress it

//error('d',"replace_temp(%k %k) c %k ff %k",e->base,n->base,c->base,ff->base);
	switch (ff->base) {
	case REF:
		if (ff->e1->base==G_ADDROF && ff->e1->e2==tmp)
			a = ff;				// &tmp -> f()
		break;
	case DOT:
		if (ff->e1->base==NAME && ff->e1==tmp) {
			a = ff;				// tmp . f()
			a->base = REF;
		}
		break;
	}
	a->e1 = n;
	return c;
}

Pname classdef::has_ictor()
/*
	does this class have a constructor taking no arguments?
*/
{
	Pname c = has_ctor();
	if (c == 0)
		return 0;
	
	Pfct f = Pfct(c->tp);

	switch (f->base) {
	default:
		error('i',"%s: badK (%k)",string,c->tp->base);
	
	case FCT:
		switch (f->nargs) {
		case 0:
			return c;
		default:
			if (f->argtype->n_initializer)
				return c;
		}
		return 0;

	case OVERLOAD:
	{
		for (Plist l=Pgen(f)->fct_list; l; l=l->l) {
			Pname n = l->f;
			f = (Pfct)n->tp;
			switch (f->nargs) {
			case 0:
				return n;
			default:
				if (f->argtype->n_initializer)
					return n;
			}
		}
		return 0;
	}
	}
}

int add_first;				// fudge, use ctor arg instead
static int add_without_find;            // specialized instance

Pname gen::add(Pname n)
/*
	add "n" to the tail of "fct_list"
	(overloaded names are searched in declaration order)

	detect:	 	multiple identical declarations
			declaration after use
			multiple definitions
*/
{
	Pfct f = Pfct(n->tp);
	Pname nx;
//error('d',"add(%n) %d",n,add_first);
	if (f->base != FCT)
		error("%n: overloaded nonF",n);

        // holds_templ keeps track of fct_list:
        // pure_templ: no attempt at argument resolution
        // some_temp: try, but skip each template instance
        // no_templ: ordinary vanilla match
        if ( f->is_templ() ) {
                if ( !has_templ() ) {
                        if (fct_list)
                                holds_templ = some_templ;
                        else holds_templ = all_templ;
                }
        } else {
                if ( pure_templ() )
                        holds_templ = some_templ;
        }
// error('d',"gen::add(%n) is_templ: %d holds_templ: %d", n,f->is_templ(),holds_templ);

        if ( fct_list && add_without_find == 0 && (nx=find(f,1)) ) {
// error('d',"gen::add: found%n%t",nx,nx->tp);
		Linkage l1 = Pfct(nx->tp)->f_linkage;
		Linkage l2 = f->f_linkage;
		if ( l2 != linkage_default && l1 != l2 )
			error("inconsistent linkage specifications for%n",n);
		Nold = 1;
	}
	else {
		if (add_first==0 && f->f_signature==0 && f->fct_base != FCT_TEMPLATE)
			f->sign();
//error('d',"signature: %d \"%s\" fct_list %d",f->f_signature,f->f_signature,fct_list);
		nx = new name;
		*nx = *n;
		nx->n_gen_fct_name = n->string;
		PERM(nx);
		Nold = 0;
		if (fct_list) {
			int clink = (f->f_linkage==linkage_C);
			Plist gl=fct_list;
			for(;;) {
				if (clink && Pfct(gl->f->tp)->f_linkage == linkage_C ) {
					error("two%ns with c linkage",n);
					if(f->f_signature==0)
						f->sign();
				}
				if (gl->l)
					gl = gl->l;
				else
					break;
			}
			gl->l = new name_list(nx,0); 
		}
		else
			fct_list = new name_list(nx,0);
		nx->n_list = 0;
	}
	return nx;
}

void fct::sign()
{
	switch ( f_linkage ) {
	case linkage_C:
		f_signature = "";
		return;
	case linkage_Cplusplus:
	case linkage_default:
		break;
	}
	char buf[1024];
	char* bb = signature(buf);
	int ll = bb-buf;
	if (1023 < ll)
		error('i',"gen::add():N buffer overflow");
	char* p = new char[ll+1];
	strcpy(p,buf);
	f_signature = p;
//error('d',"fct::sign %s",p);
}

Pname gen::find(Pfct f, bit warn)
{	
	for (Plist gl=fct_list; gl; gl=gl->l) {
		Pname n = match(gl->f,f,warn);
		if (n)
			return n;
	}
	return 0;
}

Pname gen::match(Pname nx, Pfct f, bit warn)
{
	Pfct fx = Pfct(nx->tp);
	Pname a, ax;
	int op = 0;	// overloading problem: const, ref, vec/ptr, or basetype
//error('d',"fx %d %d f %d %d",fx->nargs_known,fx->nargs,f->nargs_known,f->nargs);

      	if (f->is_templ()) return 0;  // template function
      	if (fx->is_templ()) return 0; // template function

	// bets are off: must rely on checks at call points
	if (f->nargs_known != fx->nargs_known)
		return 0;				

	if (f->f_const != fx->f_const)
		return 0;

	if (
		fx->nargs != f->nargs
		&&
		fx->nargs_known==1
		&&
		f->nargs_known==1
	)
		return 0;				// no warning for potential
							// problems due to default args
	
	for (ax=fx->argtype, a=f->argtype; a&&ax; ax=ax->n_list, a=a->n_list) {
		Ptype at = ax->tp;
		Ptype atp = a->tp;

		// if (at->check(atp,OVERLOAD) == 0) {
		if (at->check(atp,PT_OVERLOAD) == 0) {
			continue;
		}

		if (warn == 0)
			goto xx;

		/*
			warn against:
				overload f(X&), f(X);		error
				overload f(int), f(const);	error
				overload f(int*), f(int[10]);	warn
			etc.
		*/

		at = at->skiptypedefs();
		atp = atp->skiptypedefs();

		if (const_problem) {	// differ only in X vs const X
			if (
			    at->is_ptr() && atp->is_ptr()
			    ||
			    at->is_ref() && atp->is_ref()
			) {
				Ptype ap = Pptr(at)->typ;
				Ptype app = Pptr(atp)->typ;
				if(ap->check(app,PT_OVERLOAD)!=0)
					return 0;
			}
			op++;
			continue;
		}

		if(atp->base == RPTR) { // differ only by X vs X& ?
			// require strict-any-check for templates
			// if (Pptr(atp)->typ->check(at,0)==0) {
			if (
			    Pptr(atp)->typ->check(at,255)==0
			    || 
			    const_problem && (
				at->is_ptr()==0 
				|| 
				at->tconst() != Pptr(atp)->typ->tconst()
			    )
			) {
				op++;
				continue;
			}
		}

		if (at->base == RPTR) { // differ only by X& vs X ?
			// require strict-any-check for templates
			if (
			    Pptr(at)->typ->check(atp,255)==0
			    || 
			    const_problem && (
				atp->is_ptr()==0 
				|| 
				atp->tconst() != Pptr(at)->typ->tconst()
			    )
			) {
				op++;
				continue;
			}
			break;
		}
		// some argument is really different
		// e.g. f(int), f(char*);
		return 0;
	}

	// arguments checked. Now look at leftover args, return type,etc.

	if (a || ax)
		return 0;

	if (op == 0) {
		// if (warn && fx->returns->check(f->returns,0))
		if (warn && fx->returns->check(f->returns,255))
			error("two different return valueTs for%n: %t and %t",nx,fx->returns,f->returns);

		return nx;
	}
xx:
	if (warn && op)
		error("the overloading mechanism cannot tell a%t from a%t",fx,f);

	return 0;
}

int name::no_of_names()
{
	register int i = 0;
	register Pname n;
	for (n=this; n; n=n->n_list)
		i++;
	return i;
}

static Pexpr lvec[20], *lll, *curr_e;
static Pexpr last_il = 0;
static Pexpr list_back = 0;
static Pexpr last_el = 0, *last_lll;

void new_list(Pexpr lx)
{
	if (lx->base != ILIST)
		error('i',"IrLX");

	lll = last_lll = lvec;
	lll++;
	*lll = last_el = lx->e1;
}

Pexpr next_elem()
{
	Pexpr e;
	Pexpr lx;

	if (lll == lvec)
		return 0;

 	lx = *lll;

	if (list_back) {
		e = list_back;
		list_back = 0;
		return e;
	}

	if (lx == 0) {				/* end of list */
		lll--;
		return 0;
	}

	switch (lx->base) {
	case ELIST:
		e = lx->e1;
		curr_e = &lx->e1;
		last_el = lx;
		last_lll = lll;
		*lll = lx->e2;
		switch (e->base) {
		case ILIST:
			if (e->tp && e->tp->memptr())
				return e;
			lll++;
			*lll = e->e1;
			last_il = e;
			return Pexpr(1);	// start of new ILIST
		case ELIST:
			error("nestedEL");
			return 0;
		default:
		{
			if (need_sti(e))
				error('s',"generalIr inIrL");
			return e;
		}
		}
	case IVAL:
	case ZERO:
		lll--;
		return 0;
	default:
		error('i',"IrL %k",lx->base);
		return 0;
	}
}

static Pexpr insert_init(Pexpr newval) {
// splice an initializer in front of the next element in the
// initializer list. Provides initializers for unnamed bitfields.
	Pexpr t = new expr(ELIST,last_el->e1,last_el->e2);

	last_el->e1=newval;
	last_el->e2=t;
	lll = last_lll;
	*lll = last_el;
	return next_elem();
}

void skip_ilist()
// skip ilist use to represent pointer to member function literal
{
	Pexpr e = next_elem();
	e = next_elem();
}

static
int pts_to_templ(Ptype t)
{
    Ptype tt = t->skiptypedefs();
    if (tt->base != PTR) return 0;
    tt = Pptr(tt)->typ->skiptypedefs();
    return (tt->base == FCT && Pfct(tt)->fct_base == FCT_TEMPLATE);
}

static
void grab_braces(Pexpr* base, int lst)
{
	int flag = 0;

	while (lll > base - lst) {
		if (next_elem() && !flag++)
			error("IrL too long");
	}
}

void list_check(Pname nn, Ptype t, Pexpr il, Ptable tbl)
/*
	see if the list "lll" can be assigned to something of type "t"
	"nn" is the name of the variable for which the assignment is taking place.
	"il" is the last list element returned by next_elem()
*/
{
	Pexpr e;
	bit lst = 0;
	int i;
	Pclass cl;
	int tdef = 0;

//error('d',"list_check%n: %t (%d)",nn,t,il);
	if (il == Pexpr(1)) {
		lst = 1;
		e = il;
	}
	else if (il)
		list_back = il;

zzz:
	switch (t->base) {
	case TYPE:
		t = Pbase(t)->b_name->tp;
		tdef = 1;
		goto zzz;

	case VEC:
	{
		Pvec v = Pvec(t);
		Ptype vt = v->typ->skiptypedefs();

		if (v->size) {	/* get at most v->size initializers */
			if (vt->base == CHAR) {
				e = next_elem();
				if (e->base == STRING) {	// v[size] = "..."
					int isz = Pvec(e->tp)->size;
					if (v->size < isz)
						error("Ir too long (%d characters) for%n[%d]",isz,nn,v->size);
					break;
				}
				else
					list_back = e;
			}
			Pexpr* lllsave_vec = lll;
			for (i=0; i<v->size; i++) {	// check next list element type
				Pfct MP = 0;
			ee:
				e = next_elem();
				if (e == 0) {
					if (!ansi_opt)
						goto xsw;
					if (lll < lllsave_vec)
						goto xsw;
					goto ee;
				}
				// redundant braces: int x[] = {{{1}}};
				if (lll - lllsave_vec >= 2 && ansi_opt)
					error("unexpectedIrL");
			vtz:
//error('d',"vtz: %d",vt->base);
				switch (vt->base) {
				case TYPE:
					vt = Pbase(vt)->b_name->tp;
					goto vtz;
				case VEC:
				case COBJ:
					list_check(nn,vt,e);
					if (lll < lllsave_vec && ansi_opt)
						return;
					break;
				case PTR:
					if ((MP = vt->memptr()) && e==Pexpr(1)) {
						if (vt->check(last_il->tp,ASSIGN))
							error("badIrT for%n:%t (%tX)",v,last_il->tp,vt);
						skip_ilist();
						break;
					}
					if (MP && is_zero(e)) {
						*curr_e = new expr(ELIST,zero,zero);
						*curr_e = new expr(ILIST,*curr_e,zero);
						(*curr_e)->tp = zero_type;
						break;
					}
					if (e && e!=Pexpr(1) && 
					   (e->tp->is_or_pts_to(OVERLOAD)|| pts_to_templ(e->tp)))
  					{
						Pexpr op = ptof(Pfct(Pptr(vt)->typ),e,tbl);
						if (op && vt->check(op->tp,ASSIGN))
							error("badIrT for%n:%t (%tX)",nn,op->tp,vt);
						if(op) {
							*curr_e = op;
							break;
						}
					}
					// no break
				default:
				{
					if (e == (Pexpr)1) {
						if (!ansi_opt)
							error("unexpectedIrL");
						goto ee;
					}

					if (vt->check(e->tp,ASSIGN))
						error("badIrT for%n:%t (%tX)",nn,e->tp,vt);

					Pptr p;
					if (
						vt->check(e->tp,0)
						&&
						(p=vt->is_ptr())
						&&
						Ptype(p)!=zero_type
						&&
						p->typ!=char_type
					) {
						Pexpr te = e;
						Ptype t = p->typ;
						while ( t->base == TYPE )
							t = Pbase(t)->b_name->tp;
						if ( t->base == COBJ )
							te = ptr_init( p, e, tbl );
						if ( te == e )
							*curr_e = new cast(vt,e);
						else
							*curr_e = te;
					}
				}
				}
			}
			if (ansi_opt) {
				grab_braces(lllsave_vec, lst);
			}
			else {
				if ( lst && (e=next_elem()) )
					error("end ofIrLX after array");
			}
		xsw:;
		}
		else {		/* determine v->size */
			i = 0;
			(void) v->typ->tsizeof();
			Pexpr* lllsave_vec2 = lll;
		xx:
			Pfct MP = 0;
			i++;
		xx2:
			e=next_elem();
			if (e == 0) {
				if (!ansi_opt || lll < lllsave_vec2) {
					i--;
					goto done;
				}
				goto xx2;
			}
			if (lll - lllsave_vec2 >= 2 && ansi_opt)
				error("unexpectedIrL");
			vtzz:
//error('d',"vtzz");
				switch (vt->base) {
				case TYPE:
					vt = Pbase(vt)->b_name->tp;
					goto vtzz;
				case VEC:
				case COBJ:
					list_check(nn,vt,e);
					break;
				case PTR:
					if((MP = vt->memptr()) && e==Pexpr(1)) {
						if (vt->check(last_il->tp,ASSIGN))
							error("badIrT for%n:%t (%tX)",v,last_il->tp,vt);
						skip_ilist();
						break;
					}
					if (MP && is_zero(e)) {
						*curr_e = new expr(ELIST,zero,zero);
						*curr_e = new expr(ILIST,*curr_e,zero);
						(*curr_e)->tp = zero_type;
						break;
					}
					// if (e && e!=Pexpr(1) && e->tp->is_or_pts_to(OVERLOAD)){
					if (e && e!=Pexpr(1) && 
					   (e->tp->is_or_pts_to(OVERLOAD)|| pts_to_templ(e->tp)))
					{
						Pexpr op = ptof(Pfct(Pptr(vt)->typ),e,tbl);
						if (op && vt->check(op->tp,ASSIGN))
							error("badIrT for%n:%t (%tX)",nn,op->tp,vt);
						if(op) {
							*curr_e = op;
							break;
						}
					}
					// no break
				default:
				{
					if (e == Pexpr(1)) {
						if (!ansi_opt) {
							error("unexpectedIrL");
							goto xx;
						}
						goto xx2;
					}

					if (vt->check(e->tp,ASSIGN))
						error("badIrT for%n:%t (%tX)",nn,e->tp,vt);

					Pptr p;
					if (
						vt->check(e->tp,0)
						&&
						(p=vt->is_ptr())
						&&
						Ptype(p)!=zero_type
						&&
						p->typ!=char_type
					) {
						Pexpr te = e;
						Ptype t = p->typ;
						while ( t->base == TYPE )
							t = Pbase(t)->b_name->tp;
						if ( t->base == COBJ )
							te = ptr_init( p, e, tbl );
						if ( te == e )
							*curr_e = new cast(vt,e);
						else
							*curr_e = te;
					}
				}
				}
			goto xx;
done:
			if (ansi_opt) {
				grab_braces(lllsave_vec2, lst);
			}
			if (tdef==0) v->size = i;
		}
		break;
	}

	case CLASS:
		cl = Pclass(t);
		goto ccc;

	case COBJ:					// initialize members
	{
		cl = Pclass(Pbase(t)->b_name->tp);
	ccc:
		if (cl->defined == 0) {
			lll = lvec;	// we are lost: ignore rest of list
			return;
		}

		if (cl->c_body == 1)
			cl->dcl_print(0);

		Ptable tbl = cl->memtbl;
		Pname m;
		if (ansi_opt==0 && cl->csu==UNION)
			error('s',"initialization of union withIL");

		Pexpr* lllsave_cobj = lll;

		if (cl->baselist) {
			if (cl->baselist->next) error("IrL forO ofC with multipleBCs");
			list_check(nn,cl->baselist->bclass,0);
			if (lll < lllsave_cobj && ansi_opt)
				return;
		}

		for (m=tbl->get_mem(i=1); m; NEXT_NAME(tbl,m,i)) {
			if ( m->base == TNAME ) continue;
			Ptype mt = m->tp;
			Pfct MP = 0;
			if ( mt == 0 ) continue;
			switch (mt->base) {
			case FCT:
			case OVERLOAD:
			case CLASS:
			if (mt->base==CLASS) {
				Pclass cl = (Pclass) mt;
				if (!cl->is_simple())
					error("cannotI%nWIrL because class %s has private or protected members",nn,cl->string);
			}
			case ENUM:
				continue;
			}
			if (m->n_stclass == STATIC || m->n_stclass == ENUM)
				continue;
			/* check assignment to next member */
		dd:
			while (mt->base == TYPE)
				mt = Pbase(mt)->b_name->tp;

			if (lll - lllsave_cobj >= 2 && ansi_opt)
				error("unexpectedIrL");

			if (
				(MP = mt->memptr())
				&&
				e==Pexpr(1)
				&&
				last_il->tp->base == PTR
			) {
				if(i==1)
					lst=0;
			}
			else
				e = next_elem();

			if (e == 0) {
				if (!ansi_opt || lll < lllsave_cobj)
					return;
				goto dd;
			}

			if(
				mt->base == FIELD
				&&
				m->string[0]=='_'
				&&
				m->string[1]=='_'
				&&
				m->string[2]=='F'	// unnamed bitfield
			) {
				e = insert_init(zero);
			}

//error('d',"mtz%n: %d",m,mt->base);
			switch (mt->base) {
			case CLASS:
			case ENUM:
				break;
			case VEC:
			case COBJ:
				list_check(nn,m->tp,e);
				if (lll < lllsave_cobj && ansi_opt)
					return;
				break;
			case PTR:
				if (MP && e==Pexpr(1)) {
					if (mt->check(last_il->tp,ASSIGN))
						error("badIrT for%n:%t (%tX)",m,last_il->tp,mt);
					skip_ilist();
					break;
				}
				if (MP && is_zero(e)) {
					*curr_e = new expr(ELIST,zero,zero);
					*curr_e = new expr(ILIST,*curr_e,zero);
					(*curr_e)->tp = zero_type;
					break;
				}
				// if (e && e!=Pexpr(1) && e->tp->is_or_pts_to(OVERLOAD)) {
				if (e && e!=Pexpr(1) && 
				   (e->tp->is_or_pts_to(OVERLOAD)|| pts_to_templ(e->tp)))
				{
					Pexpr op = ptof(Pfct(Pptr(mt)->typ),e,tbl);
					if (op && mt->check(op->tp,ASSIGN))
						error("badIrT for%n:%t (%tX)",nn,op->tp,mt);
					if(op) {
						*curr_e = op;
						break;
					}
				}
				// no break
			default:
			{
				if (e == Pexpr(1)) {
					if (!ansi_opt)
						error("unexpectedIrL");
					goto dd;
				}

				if (mt->check(e->tp,ASSIGN))
					error("badIrT for%n:%t (%tX)",m,e->tp,m->tp);

				if(MP && e && (e->base==CAST || e->base==G_CAST))
					*curr_e = e->e1;

				Pptr p;
				if (
					mt->check(e->tp,0)
					&&
					(p=mt->is_ptr())
					&&
					Ptype(p)!=zero_type
					&&
					p->typ!=char_type
				)
					*curr_e = new cast(mt,e);
			}
			}
			if (cl->csu == UNION)
				break;
		}
		if (ansi_opt) {
			grab_braces(lllsave_cobj, lst);
		}
		else {
			if (lst && (e=next_elem()) ) error("end ofIrLX afterCO");
		}
		break;
	}

	default:
		e = next_elem();

		if (e == 0) {
			error("noIr forO");
			break;
		}
		
		if (e == Pexpr(1)) {
			error("unexpectedIrL");
			break;
		}
//error('d',"t %t e->tp %t",t,e->tp);
		if (t->check(e->tp,ASSIGN))
			error("badIrT for%n:%t (%tX)",nn,e->tp,t);
		Pptr p;
		if (
			t->check(e->tp,0)
			&&
			(p=t->is_ptr())
			&&
			Ptype(p)!=zero_type
			&&
			p->typ!=char_type
		)
			*curr_e = new cast(t,e);
		if (lst && (e=next_elem()))
			error("end ofIrLX afterO");
		break;
	}
}

#if 0
int
is_anon(char* string)
{
// error('d',"is_anon: %s", string );
	if ( string == 0 )
		return 0;

	if (
		string[0]=='_'
		&&
		string[1]=='_'
		&&
		(
			string[2]=='C'
			||
			string[2]=='E'
		)
	) 
		return 1;
	return 0;
}
#endif

Pname dclass(Pname n, Ptable tbl)
{
	Pclass cl;
	Pbase bt;
	Pname bn;
	Pname ntbl = tbl->t_name;
	Ptype ntp = 0; 

	if ( n->tp==0 || n->tp->base != CLASS )
		error('i',&n->where,"dclass%n: non-class",n);
	Pname nx = Pclass(n->tp)->k_tbl->find_cn(n->string);
	if ( nx==0 || nx->base!=TNAME )
		error('i',&n->where,"dclass%n: missingTN",n);

	if (ntbl && ntbl->tp)
		ntp = ntbl->tp;

	DB( if(Ddebug>=1) error( 'd', &n->where, "dclass n%n %d nx %d", n,n->lex_level, nx); );
// error( 'd', &n->where, "dclass(%n%n ) nx%n" , n,tbl->t_name, nx); 

	bt=0; bn=0; cl=0;
	//SYM -- delete chk for FUDGE007 -- transitional only
	//SYM removed nested/local stuff
	if ( nx ) {
		bt = Pbase(nx->tp);			// COBJ
		if ( bt->base != COBJ ) {
			//SYM deleted stuff
			error('i',&n->where,"%n is not aCN", n );
		}
		bn = bt->b_name;
	}
	else {
		error('i',&n->where,"%n is not aCN", n );
	}

  	// nx->where is point of instantiation, not definition (n has that)
  	if (Pclass(bn->tp)->class_base == INSTANTIATED) nx->where = n->where;
	bn->where = nx->where;

	Pname bnn = tbl->insert(bn,CLASS);		// copy for member lookup
	cl = Pclass(bn->tp);
// error('d',"dclass%n%t cl%t %d",n,n->tp,cl,cl==n->tp);

	if ( !cl->in_class && cl->class_base==CL_TEMPLATE )
		error("C%n defined previously asYC", bn);

	bit no_err = cl->in_class && cl->in_class->class_base == CL_TEMPLATE && tbl != gtbl;
// error('d',"no_err: %d defined: %d tb: %n",no_err,cl->defined&(DEFINED|SIMPLIFIED));

	if (cl->defined&(DEFINED|SIMPLIFIED) && no_err == 0)  { 
		    if ( nx->n_key == HIDDEN )
			error("hiddenC%t defined twice",cl);
		    else {
		        if (cl->in_class && cl->in_class->class_base == CL_TEMPLATE)
			    error("cannot provide a specialized instance ofC%t nestedWinYC%t<>",cl,cl->in_class);
			else error("C%t defined twice",cl);
		    }
	}
	else {
		if (bn->n_scope == ARG)
			bn->n_scope = ARGT;
		Pktab otbl = Ctbl;//SYM
		Ctbl = cl->k_tbl;//SYM
		cl->dcl(bn,tbl);
		Ctbl = otbl;//SYM
	}
	n->tp = cl;
	return bnn;
}

Pname denum(Pname n, Ptable tbl)
{
	Penum en;
	Pbase bt;
	Pname bn;
	Pname ntbl = tbl->t_name;
	Ptype ntp = 0; 

	Pktab tb = n->tp->in_class && n->tp->nested_sig ? n->tp->in_class->k_tbl : 0;
	Pname nx = k_find_name(n->string,tb?tb:Ctbl,HIDDEN);//SYM		
	if (ntbl && ntbl->tp)
		ntp = ntbl->tp;

// error( 'd', &n->where, "denum n%n ll %d nx%n tbl:%n", n,n->lex_level, nx, tbl->t_name); 

	//SYM -- remove nested/local stuff
	if ( nx ) {
// error( 'd', &n->where, "denum nx->tp%t ntp%t", nx->tp,ntp); 
		if ( nx->tp == 0 || nx->tp->base != EOBJ ) {
		    nx = nx->n_hidden; // check for error recovery
		    if ( nx == 0 || nx->n_key != VOID || nx->tp->base != EOBJ )
			error('i',&n->where,"%n is not an enum",n);
		}
		bt = (Pbase)nx->tp;
		bn = bt->b_name;
		en = (Penum)bn->tp;
	}
	else {
		error('i',&n->where,"%n is not an enum",n);
	}

	Pname bnn = tbl->insert(bn,CLASS);

	if (en->defined&(DEFINED|SIMPLIFIED))
	{
		if ( nx->n_key == HIDDEN )
		    error("hidden enum%n defined twice",n);
		else 
		    error("enum%n defined twice",n);
	} else {
		if (bn->n_scope == ARG)
			bn->n_scope = ARGT;
		en->dcl(bn,tbl);
	}
	n->tp = en;
	return bnn;
}

int is_probably_temp( char *str ) 
{
// error( 'd', "is_probably_temp( %s )", str );

	if ( str[0] != '_' || str[1] != '_' )
		return 0;

	switch (str[2]) {
		default:
			return 0;
		case 'A': case 'C': case 'D': case 'E': case 'F':
		case 'I': case 'K': case 'L': case 'M': case 'N': 
		case 'Q': case 'R': case 'S': case 'T': case 'U': 
		case 'V': case 'W':
			if (isdigit(str[3]))
				return 1;
			return 0;
		case 'X':
			return 1;
	}

}

static int
illegal_local_ref( Pname nn )
{
	switch ( nn->n_scope ) {
	default:
		return 0;
	case ARG:
		return 1;
	case FCT:
		if ( nn->n_sto == STATIC || nn->n_sto == EXTERN )
			return 0;
		else {
			int cc = nn->tp->tconst();
			Ptype tt = nn->tp->skiptypedefs();
			while ( cc && tt->base == VEC ) {
				tt = Pvec(tt)->typ;
				cc = tt->tconst();
				tt = tt->skiptypedefs();
			}
			if ( !cc ) {
				switch ( tt->base ) {
				case FCT: case OVERLOAD:
					break;
				default:
					return 1;
				}
			}
		} // else
		return 0;
	} // switch
}

static int
size_t_check(Ptype tp) 
/* returns 0 if the given type is compatible with size_t_type,
   returns 1 otherwise */
{
	if (tp->check(size_t_type,0) == 0) return 0;
	int size_t_size = size_t_type->tsizeof();
	if (size_t_size == uvlong_type->tsizeof() &&
		tp->check(uvlong_type,0) == 0) return 0;
	if (size_t_size == ulong_type->tsizeof() &&
		tp->check(ulong_type,0) == 0) return 0;
	if (size_t_size == ushort_type->tsizeof() &&
		tp->check(ushort_type,0) == 0) return 0;
	return 1;
}


static void
check_for_local( Pexpr ee )
{
	static Pname n[2] = {0,0}; // try not to flag multiple errors 
	static int index = 0;

	if ( ee==0 ) return; 

// error('d', "check_for_local( %k ) e1: %d e2: %d", ee->base, ee->e1, ee->e2);

	switch ( ee->base ) {
		case NAME:
		{
			Pname nn = Pname(ee);
			if (
				(nn->n_scope==FCT || nn->n_scope==ARG) 
				&&
				illegal_local_ref(nn)
				&&
				is_probably_temp(nn->string) == 0
				&&
				n[0]!=nn
				&&
				n[1]!=nn
			) {
				error("local%n used in defaultA", nn );
				n[index] = nn;
				index = index==0?1:0;
			}	
			// no break;
		}
		case TNAME:
		case STRING:
		case IVAL:
		case ICON:
		case CCON:
		case FCON:
		case ZERO:
		case DUMMY: // null or error
		case DTOR:  // dummy type destructor
		case SIZEOF:
		case BLOCK:
		case TEXT:
		case ANAME:
			return;
		case QUEST:
			check_for_local( ee->cond );
			break;
		case MDOT:
			check_for_local( ee->mem );
			return;
	}

	check_for_local( ee->e1 );
	check_for_local( ee->e2 );
}

void dargs(Pname, Pfct f, Ptable tbl)
{
	int argnamesize = 0; // if +a1, make sure arg names can be printed
	int oo = const_save;

	if ( f->memof ) {
		cc->stack();
		cc->cot = f->memof;
		cc->not = f->memof->memtbl->t_name;
		cc->tot = f->memof->this_type;
		tbl = f->memof->memtbl;
	}

	const_save = 1;
	if ( ansi_opt ) {
		Pname th = f->f_this;
		if ( th && th->string )
			argnamesize += strlen(th->string) + 1;
		th = f->f_result;
		if ( th && th->string )
			argnamesize += strlen(th->string) + 1;
	}

	for (Pname a=f->argtype; a; a=a->n_list) {
		Pexpr init;

		if (a->tp == 0) {
			error( "A has noT" );
			a->tp = any_type;
			a->n_list = 0;
			continue;
		}
		if (ansi_opt && a->string)
			argnamesize += strlen(a->string) + 1;

		Pname cln = a->tp->is_cl_obj();
//error('d',"dargs %t",a->tp);

		{
		Ptype t = a->tp->skiptypedefs();
		Pclass cl = t->base == COBJ ? t->classtype() : 0;
		if (cl && cl->c_abstract && (cl->defined&DEFINED)) {
			error("abstractC%t cannot be used as anAT",cl);
			error('C',"\t%a is a pure virtualF ofC%t\n",cl->c_abstract,cl);
		}
		}

		if (cln && Pclass(cln->tp)->has_itor())		// mark X(X&) arguments
			a->n_xref = 1;
		else {
			Ptype t = a->tp;
			while (t->base == TYPE)
				t = Pbase(t)->b_name->tp;
			if (t->base == FCT)
				a->tp = new ptr(PTR,a->tp);
		}

		if (
		//SYM	a->n_key != NESTED &&
			(init = a->n_initializer)
		) {						// default argument
			Pptr pt;
			if (init == dummy) {
				error("emptyIr");
				a->n_initializer = 0;
				continue;
			}
			if (cln) {
				if (init->base==VALUE) {
					switch (init->tp2->base) {
					case CLASS:
						if (!same_class(Pclass(init->tp2),Pclass(cln->tp)))
							goto inin2;
						break;
					default:
						{
						Pname n2 = init->tp2->is_cl_obj();
						if (n2==0 || !same_class(Pclass(n2->tp),Pclass(cln->tp)))
							goto inin2;
						}
					}

					a->n_initializer = init = 0;
					error('s',"K as defaultA");
				}
				else {
				inin2:
					if (init->base == ILIST)
						error("list as AIr");
					Pexpr i = init->typ(tbl);
					init = class_init(a,a->tp,i,tbl);
					if (i!=init && init->base==DEREF) {
						error('s',"K needed forAIr");
						init = 0;
					}
					else {
						dosimpl(init,cc->nof);
						init->permanent = 2;
					}
					a->n_initializer = init;
				}
			}
			else if (pt = a->tp->is_ref()) {
				ref_initializer++;
				init = init->typ(tbl);
				ref_initializer--;
				int tcount = stcount;
				init = ref_init(pt,init,tbl);
				if (tcount != stcount) {
					error('s',"needs temporaryV to evaluateAIr");
					init = 0;
				}
				else {
					dosimpl(init,cc->nof);
					init->permanent = 2;
				}
				a->n_initializer = init;
			}
			else {
				Pptr p = a->tp->is_ptr();
				if (perf_opt && (init->base == NEW ||
				   init->base == GNEW))
				   	New_in_arg_list = 1;
				init = init->typ(tbl);
				if (p)
					init = ptr_init(p,init,tbl);

				if (a->tp->check(init->tp,ARG)) {
					int i = can_coerce(a->tp,init->tp);

					switch (i) {
					case 1:
						if (Ncoerce) {
							Pname cn = init->tp->is_cl_obj();
							Pname xx = new name(Ncoerce->string);
							Pref r = new ref(DOT,init,xx);
							init = new expr(G_CALL,r,0);
							init = init->typ(tbl);
						}
						break;
					default:
						error("%d possible conversions for defaultA",i);
					case 0:
						error("badIrT%t forA%n (%tX)",init->tp,a,a->tp);
						DEL(init);
						a->n_initializer = init = 0;
					}
				}

				if (init && init->tp->base!=ANY) {
					dosimpl(init,cc->nof);
					init->permanent = 2;
					a->n_initializer = init;
					Neval = 0;
					long i = init->eval();
					if (Neval == 0) {
						a->n_evaluated = 1;
						a->n_val = i;
					}
				}
			}
			if ( a->n_initializer ) 
				check_for_local(a->n_initializer);
		}
	}
	if ( ansi_opt && argnamesize ) {
		char* ps = new char[ argnamesize ];
		Pname a = f->f_this;
		if ( a && a->string ) {
			int i = strlen(a->string) + 1;
			if ( (argnamesize -= i) < 0 )
				goto bad;
			strcpy(ps,a->string);
			a->string = ps;
			ps += i;
		}
		a = f->f_result;
		if ( a && a->string ) {
			int i = strlen(a->string) + 1;
			if ( (argnamesize -= i) < 0 )
				goto bad;
			strcpy(ps,a->string);
			a->string = ps;
			ps += i;
		}
		for ( a = f->argtype; a; a = a->n_list ) {
			if ( a->string == 0 )
				continue;
			int i = strlen(a->string) + 1;
			if ( (argnamesize -= i) < 0 )
				goto bad;
			strcpy(ps,a->string);
			a->string = ps;
			ps += i;
		}
		if ( argnamesize )
			bad:
			error('i',"bad argN size for%t",f);
	}
	const_save = oo;
	if ( f->memof ) cc->unstack();
}

void merge_init(Pname nn, Pfct f, Pfct nf)
{
	Pname a1 = f->argtype;
	Pname a2 = nf->argtype; 

	for (; a1; a1=a1->n_list, a2=a2->n_list) {
		int i1 = a1->n_initializer || a1->n_evaluated;
		int i2 = a2->n_initializer || a2->n_evaluated;

		if (i1 && i2)
			if (a1->string==0)	// unnamed argument
				error(&a1->where,"twoIrs for%nA",nn);
			else
				error(&a1->where,"twoIrs for%nA%n",nn,a1);

		if (i1) {
			a2->n_initializer = a1->n_initializer;
			a2->n_evaluated = a1->n_evaluated;
			a2->n_val = a1->n_val;
		}
		if (i2) {
			a1->n_initializer = a2->n_initializer;
			a1->n_evaluated = a2->n_evaluated;
			a1->n_val = a2->n_val;
		}

	}
}

Pexpr try_to_coerce(Ptype rt, Pexpr e, char* s, Ptable tbl)
/*
	``e'' is of class ``cn'' coerce it to type ``rt''
*/
{
	int i;
	Pname cn;
//error('d',"try_to_coerce(%t, %t, %s, %d)",rt,e->tp,s,tbl);

	if (
		(cn=e->tp->is_cl_obj())
		&&
		(i=can_coerce(rt,e->tp))
		&&
		Ncoerce
	) {
		if (1 < i)
			error("%d possible conversions for %s",i,s);
//error('d',"coerce%n",Ncoerce);
		Pname xx = new name(Ncoerce->string);
		Pref r = new ref(DOT,e,xx);
		Pexpr c = new expr(G_CALL,r,0);
		c = c->typ(tbl);
//error('d',"coerce -> %k %t",c->base,c->tp);
		return c;
	}
//error('d',"coerce ->0");
	return 0;
}

int in_class_dcl;

Pname name::dofct(Ptable tbl, TOK scope)
{	
	Pfct f = Pfct(tp);
	Pname class_name;
	Ptable etbl;
	int old_in_class_dcl = in_class_dcl;
	int just_made = 0;

	DB(if(Ddebug>=1){
		error('d',&where,"%n->dofct(tbl%d,scope%k) tp%t",this,tbl,scope,tp);
		error('d',&where,"  n_oper%k init %d f_virtual %d f_inline %d",n_oper,n_initializer,f->f_virtual,f->f_inline);
	});

	in_class_dcl = cc->not!=0;

	if (f->f_inline)
		n_sto = STATIC;

	if (n_stclass)
		switch (n_stclass) {
		case EXTERN:
		case STATIC:
		case OVERLOAD:
			break;
		default:
			error("%n declared%k",this,n_stclass);
			n_stclass = EXTERN;
		}

	if (n_qualifier) {	// qualified name: c::f() checked above
		class_name = Pbase(n_qualifier->tp)->b_name;
		etbl = Pclass(class_name->tp)->memtbl;
	}
	else {
		class_name = cc->not;

		// beware of local function declarations in member functions
		if (class_name && tbl!=cc->cot->memtbl) {
			class_name = 0;
			in_class_dcl = 0;
		}
	}
	if ( class_name ) f->memof = Pclass(class_name->tp);

	tp->dcl(tbl);		// must be done before the type check

	{
	Ptype tt = Pfct(tp)->returns->skiptypedefs();
	Pclass cl = tt->base == COBJ ? tt->classtype() : 0;
	if (cl && cl->c_abstract && (cl->defined&DEFINED)) {
		error("abstractC%t cannot be used as aF returnT",cl);
		error('C',"\t%a is a pure virtualF ofC%t\n",cl->c_abstract,cl);
	}
	}

	if (n_qualifier) {	// qualified name: c::f() checked above
		if (f->f_virtual) {
			 error("virtual specifier illegal outsideCD(%n::%s())",class_name,this->string);
			 f->f_virtual = 0;
		}

		if (
			n_sto
			&&
			n_sto!=FRIEND		// friend X::f();
			&&
			f->f_inline==0
		) {				// inline causes n_sto==STATIC
			error("%k specified for QdN%n",n_sto,this);
			n_sto = 0;
		}
	}
	else {
		if (f->f_static && f->f_virtual) {
 			 error("virtual staticM");
			 f->f_virtual = 0;
		}

		if (n_oper)
			check_oper(class_name);
		etbl = tbl;
	}

	if (class_name) {
		Pclass cl = Pclass(class_name->tp);
		if (f->f_virtual==0 && find_virtual(f->memof,this))
			f->f_virtual = VTOK;
//error('d',"class_name: %s fct: %s virtual: %d", class_name->string, string, f->f_virtual );

		if (f->f_static && f->f_virtual) {
 			 error("virtual staticM");
			 f->f_virtual = 0;
		}

		if ( cl->csu == UNION && f->f_virtual ) { // don't worry about ANON
			// set n_table so "fancy" name will print
			Ptable tt = n_table;
			n_table = etbl;
			error( "%n: cannot declare a virtualFWin union", this ); 
			n_table = tt;
		}
	}

	if(f->f_const && f->memof==0) {
		error("onlyMFs can be constant");
	}

	if (etbl==0 || etbl->base!=TABLE)
		error('i',"N::dcl: etbl=%d",etbl);

	switch (n_oper) {
	case CTOR:
		if (f->f_virtual) {
			error("virtualK");
			f->f_virtual = 0;
		}
		if (f->f_const) {
			error("constructors cannot be const");
			f->f_const=0;
		}
		break;
	case DTOR:
		if (f->f_const) {
			error("destructors cannot be const");
			f->f_const=0;
		}
		break;
	case REF:
		if (f->argtype) 
			error("%n takes noAs",this);
		if (f->returns->is_ptr() == 0) {
			Pname cn = f->returns->is_cl_obj();
			if (cn==0 && f->returns->base==RPTR)
				cn = Pptr(f->returns)->typ->is_cl_obj();
			if (
			    cn==0 
			    ||
			    Pclass(cn->tp)->has_oper(REF)==0
			) {
				if (cn && class_name 
				    &&
				    strcmp(cn->string, class_name->string)==0
				)
					error("%s::%n cannot return aR orCO ofC%n",cn->string,this,cn);
				else
					error("%n must return aP toCO, aR toCO, or aCO",this);
				tp = any_type;	// suppress further checking
			}
		}
		else {
		    Pname cn = f->returns->is_ptr()->typ->is_cl_obj();
		    if (cn == 0)
		     	error("%n must return aP toCO, aR toCO, or aCO",this);
		}

		break;

	case NEW:	// void* operator new(long)
		if (f->f_virtual)
			error("virtual%n (operator new() is static)",this);
		if (class_name)
			f->f_static = 1;	// if member: static by default
		if (f->nargs_known != 1)
			error("ATs must be fully specified for%n",this);
		else if (f->nargs<1)
			error("%n requires a firstA ofT size_t",this);
		else if (size_t_check(f->argtype->tp)) {
			error("%n requires a firstA ofT size_t",this);
			f->argtype->tp = size_t_type;
			if (f->f_signature)
				f->sign();
		}
		else {
			Ptype t = f->s_returns ? f->s_returns : f->returns;
			if (t->check(Pvoid_type,0))
				error("bad returnT for%n",this);
		}
		break;

	case DELETE:	// void operator delete(void*) or
			// void operator delete(void*, long)
		if (f->f_virtual)
			error("virtual%n (operator delete() is static)",this);

		Pclass cl;
		if (class_name) {
			f->f_static = 1;  // if member: static
			cl = Pclass(class_name->tp);
			if (in_class_dcl
			    &&
			    cl->memtbl->look("__dl",0)
			)
				error("operator delete() cannot be overloaded");
		}
					
		if (f->nargs_known != 1)
			error("ATs must be fully specified for%n",this);
		else {
			Ptype t = f->s_returns ? f->s_returns : f->returns;
			if (t->check(void_type,0))
				error("bad returnT for%n", this);
			else {
				switch (f->nargs) {
				default:
					error("%n takes 1 or 2As",this);
					break;
				case 1:
				case 2:
				{
					Pname a = f->argtype;
					if (a->tp->check(Pvoid_type,0))
						error("%n's 1stA must be a void*",this);
					else if (a = a->n_list) {
						if (class_name == 0)
							error("%n takes only oneA",this);
						else if (a->tp->check(size_t_type,0)) {
							error("%n's 2ndA must be a size_t",this);
							a->tp = size_t_type;
							if (f->f_signature)
								f->sign();
						}
					}
				}
				}
			}
		}
		break;

	case ASSIGN:
		if (class_name && f->nargs==1) {
			Ptype t = f->argtype->tp;
			Pname an = t->is_cl_obj();		// X::operator=(X) ?
			if (an==0 && (t=t->is_ref())) {		// X::operator=(X&) ?
				t = Pptr(t)->typ;
			rx1:
				switch (t->base) {
				case TYPE:
					if (warning_opt && Pbase(t)->b_const==0)
						error('w',"assignment operators should take a const %t& not a %t&",t,t);
					t = Pbase(t)->b_name->tp;
					goto rx1;
				case COBJ:
					an = Pbase(t)->b_name;
				}
			}
			if (an && an==class_name)
				Pclass(an->tp)->c_xref |= C_ASS;
		}
		else if (f->nargs == 2) {
			Ptype t = f->argtype->tp;
			Pname an1;
			if (t=t->is_ref()) {			// operator=(X&,?) ?
				t = Pptr(t)->typ;
			rx2:
				switch (t->base) {
				case TYPE:
					t = Pbase(t)->b_name->tp;
					goto rx2;
				case COBJ:
					an1 = Pbase(t)->b_name;
				}
			}
			t = f->argtype->n_list->tp;
			Pname an2 = t->is_cl_obj(); 		// operator=(X&,X) ?
			if (an2==0 && (t=t->is_ref())) { 	// operator=(X&,X&) ?
				t = Pptr(t)->typ;
			rx3:
				switch (t->base) {
				case TYPE:
					t = Pbase(t)->b_name->tp;
					goto rx3;
				case COBJ:
					an2 = Pbase(t)->b_name;
				}
			}
			if (an1 && an1==an2)
				Pclass(an1->tp)->c_xref |= C_ASS;
		}
	}

	switch (scope) {
	case FCT:
	case ARG:
		if (n_sto == STATIC)
			error("D of staticF in aF");
		else {					// detect local re-definition
			Pname nx = gtbl->look(string,0);
			if (nx) {
				switch (nx->tp->base) {
				case FCT:
					if (tp->check(nx->tp,0) && n_sto!=EXTERN)
						error('w',"%n has been locally re-declared as%t",this,tp);
					else {
						if(Pfct(nx->tp)->f_signature==0)
							Pfct(nx->tp)->sign();
						if (Pfct(tp)->f_signature == 0)
							Pfct(tp)->sign();
						if ( Pfct(nx->tp)->f_linkage!=Pfct(tp)->f_linkage)
							error('w',"%n of type %t has been locally re-declared with different linkage",this,tp);
					}
					break;
				case OVERLOAD:
				{
					Pname ny = Pgen(nx->tp)->find(f,0);
					if (ny == 0)
						error('w',"overloadedF%n has been locally declared as%t",this,tp);
					else {
						if(Pfct(ny->tp)->f_signature==0)
							Pfct(ny->tp)->sign();
						if (Pfct(tp)->f_signature == 0)
							Pfct(tp)->sign();
						if (Pfct(ny->tp)->f_linkage!=Pfct(tp)->f_linkage)
							error('w',"overloadedF%n of type %t has been locally re-declared with different linkage",this,tp);
					}
				}
					break;
				}	// switch nx->base
			}	// if nx
		}	// else
	}	// switch scope

	Pname nn;
	if (n_sto==EXTERN) {
		nn=gtbl->insert(this,0);
		nn->lex_level = 0;
	} else nn=etbl->insert(this,0);
	if ( f->body )
		nn->where = where;
	nn->assign();
	if (nn->tp->base==FCT) {
		Pfct nnf = (Pfct)nn->tp;
		if (nnf->f_this)
			nnf->f_this->where=where;
	}
	n_table = etbl;
//error('d',"%n->dofct(): n_initializer:%d f->f_virtual:%d",this,n_initializer,f->f_virtual);
	if (n_initializer) {
		if (f->f_virtual == 0)
			error("Ir for non-virtualF%n",this);
		if (n_initializer != zero)
			error("virtualFIr must be 0");
	}

	if (Nold && f->fct_base != INSTANTIATED) {
		Pfct nf = Pfct(nn->tp);
// error('d',"old%n:%t and%t",nn,nf,tp);
		if (nf->base==ANY || f->base==ANY)
			; // wild card -- do nothing
		else 
		if (nf->base == OVERLOAD) {
			string = nn->string;
			nn = Pgen(nf)->add(this);
			nn->where=where;
			if (Pfct(nn->tp)->f_this!=0)
				Pfct(nn->tp)->f_this->where=where;

			if (Nold == 0) {
				if (f->body && n_qualifier) {
					error("badAL for%n",this);
					return 0;
				}
				goto thth;
			}
			nf = Pfct(nn->tp);
			if (f->body && nf->body) {
			 	// Preserve the original definition 
				// in the case of a PT class; i.e, 
				// the one supplied by the user
				if (
					class_name==0
					||
					Pclass(class_name->tp)->class_base != INSTANTIATED
					|| 
					nn->n_redefined==0
				) {
					error("two definitions of%n",nn);
					f->body = 0;
				}
				return 0;
			}
			if (f->body)
				goto bdbd;
			goto stst;
		}
		else if (nf->base != FCT) {
			error("%n declared both as%t and asF",this,nf);
			f->body = 0;
		}
		else {
  			{TOK t = (nf->fct_base == FCT_TEMPLATE || f->fct_base == FCT_TEMPLATE) 
  				? PT_OVERLOAD : OVERLOAD;

			/* one can be a specialization of the other.
			 * if one is simply an general template definition
			 * then type::check cannot distinquish them 

			 * sp_templ is set if one instance is template version and
			 * second instance is nontemplate instance
			 */

			bit both_templs = nf->fct_base && f->fct_base;
			bit sp_templ = both_templs ? 0 : nf->fct_base + f->fct_base;

// error('d',"both_templs: %d sp_templ: %d", both_templs, sp_templ );
// error('d',"%t->check(%t) -> %d t %d",nf,f,nf->check(f,t),t);

                        if (nf->check(f,t) || sp_templ) {
				if (f->body && n_qualifier) {
					error("%nT mismatch: %a and %a",nn,nn,this);
					return 0;
				}
				if (!strcmp(nn->string,"main"))
					error("cannot overload main()!");
				Pgen g = new gen;
				add_first = 1;
				Pname n1 = g->add(nn);
				add_first = 0;
				string = nn->string;
                                add_without_find = sp_templ;
				Pname n2 = g->add(this);
                                add_without_find = 0;
				nn->tp = g;
				nn = n2;
				goto thth;
  			}} // extra parens for TOK definition because of goto

			if (in_class_dcl) {
			}
			else if (nf->f_static && f->f_inline==0 && n_sto==STATIC) {
//error('d',"MF%n declared static outsideF",this);
				n_sto = 0;
			}
			else if (n_sto && n_sto!=nn->n_scope) {
				if (n_sto==EXTERN && nn->n_scope==STATIC)
					error('w',"%n declared extern after being declared static",this);
				else if (nf->f_inline==0 && f->f_inline==0) {
					if (nn->tp==new_fct->tp || nn->tp==del_fct->tp)
						nn->n_sto = n_sto;
					else
						error("%n declared as both%k and%k",this,n_sto,(nn->n_sto)?nn->n_sto:EXTERN);
				}
			}

//error('d',"fct%n:%k%k scope%k",this,n_sto,nn->n_sto,nn->n_scope);	
//error('d',"link %d lcount %d sig %s",linkage,lcount,nf->f_signature);

			{ 
				Linkage l1 = nf->f_linkage;
				Linkage l2 = f->f_linkage;
				if ( l2!=linkage_default && l1!=l2)
					error("inconsistent linkage specifications for%n",this);
			}
			if (nf->body && f->body) {
			 	// Preserve the original definition 
				// in the case of a PT class; i.e, 
				// the one supplied by the user
				if (
					class_name==0
					||
					Pclass(class_name->tp)->class_base != INSTANTIATED
					|| 
					nn->n_redefined==0
				) {
					error("two definitions of%n",this);
					f->body = 0;
				}
				return 0;
			}

			if (f->body)
				goto bdbd;

			goto stst;
	
		bdbd: 
// error('d',"nn%n init: %d f_virt: %d f->body: %d", nn,nn->n_initializer,nf->f_virtual,f->body);
			if (f->nargs_known && nf->nargs_known)
				if (n_sto==EXTERN)	
					nf->argtype=f->argtype;
				else merge_init(nn,f,nf);
			f->f_virtual = nf->f_virtual;
			f->f_this = nf->f_this;
			f->f_result = nf->f_result;
			f->s_returns = nf->s_returns;
			f->f_args = nf->f_args;
			f->f_signature = nf->f_signature;
			f->f_const = nf->f_const;
			f->f_static = nf->f_static;
			nn->tp = f;
			if (f->f_inline) {
				if (nf->f_inline==0) {
					if (nn->n_used && nn->n_sto!=STATIC)
						error("%n declared with external linkage and called before defined as inline",nn);
					else if (nn->n_used) {
						nn->take_addr(); // force printout
						if (warning_opt)
							error('w',"%n called before defined as inline",nn);
					}
				}
				nf->f_inline = 1;
				nn->n_sto = STATIC;
			}
			else if (nf->f_inline) {
				f->f_inline = 1;
			}
			goto stst2;

		stst:
//error('d',"stst");
			if (f->nargs_known && nf->nargs_known)
				if (n_sto==EXTERN || f->f_inline)
					nf->argtype=f->argtype;
				else merge_init(nn,f,nf);
			f->f_args = nf->f_args;
		stst2:
//error('d',"stst2%n printed %d",nn,nn->n_dcl_printed);
			if (f->f_inline)
				n_sto = STATIC;
 			if(
				n_sto == STATIC
				&&
				nn->n_sto == EXTERN
				&&
 				(
					strcmp(string,"__nw")==0
					||
					strcmp(string,"__dl")==0
				)
			)
 				nn->n_sto = STATIC;
			n_scope = nn->n_scope; // first specifier wins
			n_sto = nn->n_sto;
		}
	}
	else {	// new function: make f_this for member functions
	thth:
		just_made = 1;
		if (f->f_inline)
			nn->n_sto = STATIC;
		else if (class_name==0 && n_sto==0 && f->body==0)
			nn->n_sto = EXTERN;
//error('d',"thth%n%t static %d sto%k",nn,f,f->f_static,nn->n_sto);
		if (f->f_static)
			switch (n_oper) {	// what about + ??
			case CTOR:
			case DTOR:
			case TYPE:
			case CALL:
			case DEREF:
			case REF:
			case ASSIGN:
				error("%n cannot be a staticMF",nn);
				f->f_static = 0;
			}

		if (
			class_name
			&&
			f->f_static==0		// no ``this'' in static members
			&&
			n_oper!=NEW	 	// X::operator new() static by default
			&&
			n_oper!=DELETE		// X::operator delete() static by default
			&&
			etbl!=gtbl		// beware of implicit declaration 
		) {
			Pname tt = new name("this");
			tt->n_scope = ARG;
			tt->where = this->where;
			tt->tp = Pclass(class_name->tp)->this_type;
			PERM(tt);
			Pfct(nn->tp)->f_this =
				f->f_this =
				Pfct(nn->tp)->f_args =
				f->f_args =
				tt;
			tt->n_list = f->argtype;
//error('d',"nn%n tp%t const %d",nn,nn->tp,f->f_const);
			if (f->f_const) {
				Pbase x = Pbase(Pptr(tt->tp)->typ);
				Pbase y = new basetype(COBJ,0);
				*y = *x;
				y->b_const = 1;
				tt->tp = new ptr(PTR,y);
				Pptr(tt->tp)->b_const = 1;
				PERM(tt->tp);
			}
		}
		else {
			Pfct(nn->tp)->f_args =
				f->f_args =
					f->f_result
					?
					f->f_result
					:
					f->argtype;
			Pfct(nn->tp)->f_signature = f->f_signature;
			Pfct(nn->tp)->f_const = f->f_const;
			Pfct(nn->tp)->f_static = f->f_static;
		}

		// if C++ linkage encode type in function name
		if (Pfct(nn->tp)->f_signature==0)
			Pfct(nn->tp)->sign();

		if (f->f_result == 0) {
//error('d',"re1%n%t %d",this,f,f);
			make_res(f);
		}
		else if (f->f_this)
			f->f_this->n_list = f->f_result;

		if (nn->n_oper==CTOR || nn->n_oper==DTOR)
			vbase_pointers(nn,Pclass(class_name->tp));

		if (f->f_virtual) {
			switch (nn->n_scope) {
			default:
				error("nonC virtual%n",this);
				break;
			case 0:
			case PUBLIC:
				cc->cot->virt_count = 1;
				Pfct(nn->tp)->f_virtual = f->f_virtual;
				break;
			}
		}
	}

		//	an operator must take at least one class object or
		//	reference to class object argument

	if (just_made)
		switch (n_oper) {
		case CTOR:
			switch (f->nargs) {	// check for X(X) and X(X&)
			case 0:
				break;
			default:		// handle X(X&, int i = 0)
			{
				Pname n2 = f->argtype->n_list;
				if (n2->n_initializer==0 && n2->n_evaluated==0)
					break;
			}
			case 1:
			{
				Ptype t = f->argtype->tp;
			clll:
				switch (t->base) {
				case TYPE:
					t = Pbase(t)->b_name->tp;
					goto clll;
				case RPTR:			/* X(X&) ? */
					t = Pptr(t)->typ;
				cxll:
					switch (t->base) {
					case TYPE:
						if (warning_opt && Pbase(t)->b_const==0)
							error('w',"copy constructors should take a const %t& not a %t&",t,t);
						t = Pbase(t)->b_name->tp;
						goto cxll;
					case COBJ:
						if (class_name == Pbase(t)->b_name)
							Pclass(class_name->tp)->c_itor = nn;
					}
					break;
				case COBJ:			/* X(X) ? */
					if (class_name == Pbase(t)->b_name) {
						error("badK %s(%s) use %s(%s&)",class_name->string,class_name->string,class_name->string,class_name->string);
						f->argtype->tp = any_type;
					}
				}
			}
			}
			if (Pclass(class_name->tp)->c_ctor == 0)
				Pclass(class_name->tp)->c_ctor = nn;
			break;

		case TYPE:
			// somewhat simple minded solution to the inheritance of
			// conversion operator problem
			nn->n_list = Pclass(class_name->tp)->conv;
			Pclass(class_name->tp)->conv = nn;
			break;

		case DTOR:
			Pclass(class_name->tp)->c_dtor = nn;
			break;

		case NEW:	
		case DELETE:
		case CALL:
		case 0:
			break;

		default:
			{		
			for (Pname a=f->argtype; a; a=a->n_list) {
				if ( a->n_initializer )
					error( "%n: operatorFs cannot take defaultA", this );
			}

			if (f->nargs_known != 1) {
				error("ATs must be fully specified for%n",nn);
			}
			if (class_name==0) {
				for (a=f->argtype; a; a=a->n_list) {
					Ptype tx = a->tp->skiptypedefs();
					if (tx->is_ref())
						tx = Pptr(tx)->typ;
					if (tx->is_cl_obj())
						break;
				}
				if (!a)
					error("%n must take at least oneCTA",nn);
			}
			// this doesn't catch unary operator off by one errors
			// for simplicity, placed that check in check_oper(), above
			bit memberonly=0;
			switch (n_oper) {
				case COMPL:	// unary
				case NOT:
					if ((!class_name&&f->nargs==1) || class_name)
						;
					else
						error("%n takes 1A",nn);
					break;
				case DEREF:	// binary
				case ASSIGN:
					memberonly=1;
				case REFMUL:
				case MEMPTR:
				case DIV:
				case MOD:
				case LS:
				case RS:
				case LT:
				case LE:
				case GT:
				case GE:
				case EQ:
				case NE:
				case ER:
				case OR:
				case ANDAND:
				case OROR:
				case ASPLUS:
				case ASMINUS:
				case ASMUL:
				case ASDIV:
				case ASMOD:
				case ASAND:
				case ASOR:
				case ASER:
				case ASLS:
				case ASRS:
				case CM:
					if ((!class_name&&(memberonly||f->nargs==2)) || (class_name&&f->nargs==1))
						;
					else
						error(class_name?"%n takes 1A":"%n takes 2As",nn);
					break;

				case MUL:	// unary or binary
				case AND:
				case PLUS:
				case MINUS:
				case INCR:
				case DECR:
					if ((!class_name&&(f->nargs==1||f->nargs==2)) || (class_name&&f->nargs<=1))
						;
					else
						error(class_name?"%n takes 0 or 1A":"%n takes 1 or 2As",nn);
					break;
			}
			}
		}

	int i = 0;	// check that every argument after an argument with
			// initializer have an initializer
	for (Pname a = f->f_args; a; a=a->n_list) {
		if (a->n_initializer)
			i = 1;
		else if (i)
			if (a)
				error("trailingA%n withoutIr",a);
			else
				error("trailingA withoutIr");
	}

	/*
		the body cannot be checked until the name
		has been checked and entered into its table
	*/
	if (f->body)
		f->dcl(nn);
// error('d', "end of do-fct, in_class_dcl: %d old: %d",in_class_dcl,old_in_class_dcl);
	in_class_dcl = old_in_class_dcl;
	return nn;
}
