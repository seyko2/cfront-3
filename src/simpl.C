/*ident	"@(#)cls4:src/simpl.c	1.19" */
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

simpl.c:

	simplify the typechecked function
	remove:		classes:
				class fct-calls
				operators
				value constructors and destructors
			new and delete operators (replace with function calls)
			initializers		(turn them into statements)
			constant expressions		(evaluate them)
			inline functions		(expand the calls)
			enums				(make const ints)
			unreachable code		(delete it)
	make implicit coersions explicit

	in general you cannot simplify something twice

*******************************************************************/

#include "cfront.h"
#include "size.h"

Pname new_fct;
Pname del_fct;
Pname vec_new_fct;
Pname vec_del_fct;

Pstmt del_list;
Pstmt break_del_list;
Pstmt continue_del_list;

//bit not_inl;	// is the current function an inline?
Pname curr_fct;	// current function
Pexpr init_list;
Pexpr one;

Ptype Pfct_type;
Ptype Pvptr_type;
Pbase mptr_type;
Ptype size_t_type;

Pstmt trim_tail(Pstmt tt);
Pname find_vptr(Pclass);
char *get_classname(char*);
char *drop_classname(char*);
loc no_where;	// 0,0

int imeasure;	// a counter trying to measure the complexity of a function
		// body to try to avoid expanding ``monster'' inlines.

static Pclass topclass;

int replace_ret(Pstmt sl, int level, Ptable tbl)
{
	int nl = 0;
	Pstmt ls;
	static char* buf;

	if (!level)
		buf = 0;

	for (Pstmt s = sl; s; ls = s, s = s->s_list) {
		switch (s->base) {
			case WHILE:
			case DO:
			case FOR:
			case SWITCH:
			case BLOCK:
			case LABEL:
			case CASE:
			case DEFAULT:
				nl += replace_ret(s->s, level + 1, tbl);
				break;
			case IF:
				nl += replace_ret(s->s, level + 1, tbl);
				if (s->else_stmt)
					nl += replace_ret(s->else_stmt, level + 1, tbl);
				break;
			case PAIR:
				nl += replace_ret(s->s, level + 1, tbl);
				nl += replace_ret(s->s2, level + 1, tbl);
				break;
			case RETURN: {
				if (!buf)
					buf = make_name('Z');
				Pname lab = tbl->insert(new name(buf), LABEL);
				nl++;
				s->gt = new lstmt(GOTO, s->where, lab, 0);
				break;
			}
		}
	}

	if (!level && nl) {
		Pname lab = tbl->look(buf, LABEL);
		ls->s_list = new lstmt(LABEL, curloc, lab,
		    new estmt(SM, curloc, dummy, 0));
	}

	return nl;
}

void simpl_init()
{
       char* ns = oper_name(NEW);
       char* ds = oper_name(DELETE);

	size_t_type = ulong_type; /*Pvoid_type->tsizeof()>uint_type->tsizeof()?ulong_type:uint_type; Plan9 */

       Pname nw = new name(ns);
       nw->n_oper = NEW;
       Pname a = new name;
       a->tp = size_t_type;
       nw->tp = new fct(Pvoid_type,a,1);
       new_fct = nw->dcl(gtbl,EXTERN); // void* operator new(long);
       delete nw;
 //    new_fct->use();

       Pname dl = new name(ds);
       dl->n_oper = DELETE;
       a = new name;
       a->tp = Pvoid_type;
       dl->tp = new fct(void_type,a,1);
       del_fct = dl->dcl(gtbl,EXTERN);
       delete dl;
 //    del_fct->use();


	Pname vn = new name("__vec_new");
	Pname vd = new name("__vec_delete");

	a = new name;
	a->tp = Pvoid_type;
	Pname al = a;
	a = new name;
	a->tp = int_type;
	a->n_list = al;
	al = a;
	a = new name;
	a->tp = int_type;
	a->n_list = al;
	al = a;
	a = new name;	
	a->tp = Pvoid_type;
	a->n_list = al;
	al = a;				/* (Pvoid, int, int, Pvoid) */

	vec_new_fct = gtbl->insert(vn,0);
	delete vn;
	vec_new_fct->tp = new fct(Pvoid_type,al,1);
	Pfct(vec_new_fct->tp)->f_linkage = linkage_C;
	vec_new_fct->n_scope = EXTERN;
	PERM(vec_new_fct);
	PERM(vec_new_fct->tp);
	vec_new_fct->use();
	vec_new_fct->dcl_print(0);

	a = new name;
	a->tp = int_type;
	al = a;
	a = new name;
	a->tp = int_type;
	a->n_list = al;
	al = a;
	a = new name;
	a->tp = Pvoid_type;
	a->n_list = al;
	al = a;
	a = new name;
	a->tp = int_type;
	a->n_list = al;
	al = a;
	a = new name;
	a->tp = int_type;
	a->n_list = al;
	al = a;
	a = new name;	
	a->tp = Pvoid_type;
	a->n_list = al;
	al = a;				/* (Pvoid, int, int, Pvoid, int, int) */

	vec_del_fct = gtbl->insert(vd,0);
	delete vd;
	vec_del_fct->tp = new fct(void_type,al,1);
	Pfct(vec_del_fct->tp)->f_linkage = linkage_C;
	vec_del_fct->n_scope = EXTERN;
	PERM(vec_del_fct);
	PERM(vec_del_fct->tp);
	vec_del_fct->use();
	vec_del_fct->dcl_print(0);

	one = new ival(1);
	one->tp = int_type;
	PERM(one);
	
	Pfct_type = new fct(int_type,0,1);	// int (*)()
	Pfct_type = Pfct_type->addrof();
	PERM(Pfct_type);

	if (ansi_opt)
		putstring("typedef int (*__vptp)(void);\n");
	else putstring("typedef int (*__vptp)();\n");
	putstring("struct __mptr {short d; short i; __vptp f; };\n");
	Pname b = new name("__mptr");
        b->tp = new classdef(STRUCT);
        b->tp->defined = DEFINED;
        Pclass(b->tp)->obj_size = SZ_SHORT + SZ_SHORT + SZ_WPTR;
	mptr_type = new basetype(COBJ,b);
	PERM(mptr_type);

	Pvptr_type = mptr_type->addrof();
	PERM(Pvptr_type);
}

Ptable scope;		/* current scope for simpl() */
Pname expand_fn;	/* name of function being expanded or 0 */
Ptable expand_tbl;	/* scope for inline function variables */

Pname classdef::has_oper(TOK op)
{
	Pexpr n = find_name(oper_name(op), 0, 0, 1);
	if (n == 0) return 0;

	while (n->base==REF || n->base==MDOT) {
		Pexpr e = Pexpr(n);
		n = Pname(n->mem);
		delete e;
	}
	if (n->tp->base==FCT && Pname(n)->n_dcl_printed==0) Pname(n)->dcl_print(0);
	return Pname(n);
}

int is_expr(Pstmt s)
/*
	is this statement simple enough to be converted into an expression for
	inline expansion?
*/
{
	int i = 0;
	while (s->base == BLOCK) {
		if (s->s == 0) return 1;
		s = s->s;
	}
	for (Pstmt ss = s; ss; ss = ss->s_list) {
//error('d',"ss %k",ss->base);
		switch (ss->base) {
		case BLOCK:
			if (Pblock(ss)->memtbl || is_expr(ss->s)==0) return 0;
		case SM:
			if (ss->e && ss->e->base==ICALL) {
				Pname fn = ss->e->il->fct_name;
				Pfct f = Pfct(fn->tp);
				if (f->f_expr == 0) return 0;
			}
			break;
		case IF:
			if (is_expr(ss->s)==0) return 0;
			if (ss->else_stmt && is_expr(ss->else_stmt)==0) return 0;
			break;
		default:
			return 0;
		}
		i++;
	}
	return i;
}

int no_of_returns;

int size_complexity_measure = 20;

void name::simpl()
{
//error('d',"name::simpl%n %d %k",this,tp->base,tp->base);
	if (base == PUBLIC) return;

	if (tp == 0) error('i',"%n->N::simple(tp==0)",this);
	if ( tp->base == CLASS && n_key == REF ) return;
	if ( tp->base == CLASS && (Pclass(tp)->defined & DEFINED) == 0) return;

	switch (tp->base) {
	case 0:
		error('i',"%n->N::simpl(tp->B==0)",this);

	case TYPE:
	case VEC:
	case PTR:
	{
		Ptype t = tp;
	xx:
		switch (t->base) {
		case TYPE:	t = Pbase(t)->b_name->tp;	goto xx;
		case VEC:	t = Pvec(t)->typ;		goto xx;
		case PTR:	t = Pptr(t)->typ;		goto xx;
		case FCT:	break;
		}
		break;
	}

	case OVERLOAD:
	{	for (Plist gl = Pgen(tp)->fct_list; gl; gl=gl->l) gl->f->simpl();
		break;
	}

	case FCT:
	{	Pfct f = Pfct(tp);
		Pname n;
		Pname th = f->f_this;

		if (th) {
			// Make "this" a register if it is used more than twice:
//			if (th->n_addr_taken) error("&this");
			th->n_stclass = (!th->n_addr_taken && 2 < th->n_used) ? REGISTER : AUTO;
			if (
			    th->n_assigned_to
			    &&
			    th->n_assigned_to!=FUDGE111
			)
				error('w',&where,"assignment to this (anachronism)");
		}

		if ( tp->defined&SIMPLIFIED ) return;
		if ((tp->defined&DEFINED) == 0) return;

		for (n=f->f_args; n; n=n->n_list) n->simpl();

		if (f->body) {
			Ptable oscope = scope;
			Pname ocurr = curr_fct;
			int oim = imeasure;
			scope = f->body->memtbl;
//error('d',"body");
			if (scope == 0) error('i',"%n memtbl missing",this);
			curr_fct = this;
			f->simpl();
			if (f->f_inline==0 || debug_opt)
				f->f_imeasure = 0;	// not a converted inline
			else {
				if (warning_opt && f->f_virtual)
 					error('w',"virtual function %n cannot be inlined",this);
				// first check if the function is too large to
				// be worth inlining
				if (size_complexity_measure<f->f_imeasure) {	
					if (warning_opt) {
 						error('w',"%n too large for inlining",this);
 					}
					f->f_inline = 0;// a converted inline
//error('d',"don't inline%n %d %d %d",this,this,f,f->f_imeasure);
					scope = oscope;
					tp->defined |= SIMPLIFIED;
					return;
				}
				int i = 0;
				for (n=f->f_args; n; n=n->n_list) {
					n->base = ANAME;
					n->argno = i++;
	// ??				if (n->n_table != scope) error('i',"aname scope: %d %n %d %d\n",n,n,n->n_table,scope);
				}
				expand_tbl = (f->returns->base!=VOID || n_oper==CTOR) ? scope : 0;
				expand_fn = this;

				if (expand_tbl) {
				genlab:
						// value returning: generate expr
						// the body still holds the memtbl
					Pexpr ee = Pexpr(f->body->expand());
					Ptype t = 0;
					if (f->s_returns) {
						if (ee->tp!=f->s_returns) t = f->s_returns;
					}
					else if (ee->tp!=f->returns)
						t = f->returns;
					// VCVC assumes VOID is output as CHAR:
                                       if (t && t==void_type)
                                               ee = new expr(CM,ee,new cast(ansi_opt?void_type:char_type,zero));
					f->f_expr = (ee->base==CM) ? ee : new expr(CM,zero,ee);
					// print.c assumes expansion into comma expression
				}
				else {
					if (is_expr(f->body)) {
						// can generate expr: do
						expand_tbl = scope;
						goto genlab;
					}
					// not value return: can generate block
					f->f_expr = 0;
					replace_ret(f->body->s, 0, f->body->memtbl);
					f->body = Pblock(f->body->expand());
				}
				expand_fn = 0;
				expand_tbl = 0;
			}
			scope = oscope;
			curr_fct = ocurr;
			imeasure = oim;
		}
		break;
	}

	case CLASS:
		Pclass(tp)->simpl();
	}

//	if (n_initializer) n_initializer->simpl();
//SYM   if (n_key != NESTED && n_initializer) n_initializer->simpl();
        if (base != TNAME && n_initializer) n_initializer->simpl();
	tp->defined |= SIMPLIFIED;
}

Pexpr call_ctor(Ptable tbl, Pexpr p, Pexpr ctor, Pexpr args, int d, Pexpr vb_args)
{
	Pexpr ee = new ref(d,p,ctor);
	if (p==0 || tbl==0) ee->tp = ctor->tp;
// error('d',"call_ctor(tbl %d, p %d, %n, args %d)",tbl,p,ctor,args);
	if (args && args->base!=ELIST) args = new expr(ELIST,args,0);
	ee = new expr(G_CALL,ee,args);
	Pname n = Pname(ctor);

	while (n->base == MDOT) n = Pname(n->mem);
	if (n->tp->base == FCT) ee->fct_name = n;

	if (tbl)
		ee = ee->typ(tbl);
	else 
	if (n->tp->base == FCT)
		ee->tp = Pfct(n->tp)->s_returns;

	if (ee->tp == any_type) return ee;
	args = ee->e2;

	if (vb_args == 0) {		// attach zero vbase arguments
		Pfct f = Pfct(ee->fct_name->tp);	// not n->fct_name
		for (Pname nn = f->f_args->n_list; nn && nn!=f->argtype; nn=nn->n_list)
			args = new expr(ELIST,zero,args);
	}
	else {				// attach vbase arguments
		if (args) {
			for (Pexpr d = vb_args; d->e2; d=d->e2);
			d->e2 = args;
		}
		args = vb_args;
	}


	if (n->tp->base == FCT) { // take care of possible default arguments
		Pfct f = (Pfct) n->tp;
		Pexpr ee;
		Pname nn;
		if (f->argtype && args)
			for (nn=f->argtype->n_list,ee=args;nn;nn=nn->n_list,ee=ee->e2)
			{
				if (ee->e2==0) {
					Pexpr a=nn->n_initializer;
					Pexpr e=new expr(ELIST,a,0);
					ee->e2=e;
				}
			}
	}

	ee->e2 = args;
	return ee;
}

Pexpr call_dtor(Pexpr p, Pexpr dtor, Pexpr arg, int d, Pexpr vb_args)
{
// error('d',"call dtor %k %n %t vb_args %d",d,dtor,dtor->tp,vb_args);
        // see call_dtor in expr::typ, d==0 means REF[->/.]x::~x()
	// need to not set ``d'' to suppress virtual below ...
	Pexpr r = new ref(d?d:REF,p,dtor);

	if (arg && vb_args) 
		arg = new ival(3);
	else if (vb_args) 
		arg = new ival(2);
	else if (arg==0)
		arg = zero;
	if (arg->tp == 0) arg->tp = int_type;

	Pexpr aa = new expr(ELIST,arg,0); // argument controlling deallocation
					  // 2: destroy vbases
					  // 1: dealllocate, 0: don't

	Pfct f = Pfct(dtor->tp);		// attach virtual base arguments
//	for (Pname nn = f->f_args->n_list; nn && nn->n_list; nn=nn->n_list) {
//		Pexpr xx = vb_args ? new cast(nn->tp,one) : zero;
//		aa = new expr(ELIST,xx,aa);
//	}


	Pexpr ee = new call(r,aa);
	while (dtor->base == MDOT) dtor = dtor->mem;
	if (d == REF)	// could be virtual
		ee->fct_name = Pname(dtor);
	else {	// virtual suppressed, store fct name 
		r->n_initializer = dtor;
		ee->fct_name = 0;
	}
	ee->base = G_CALL;
	ee->tp = void_type;

        if (f->memof->c_body == 1) f->memof->dcl_print(0);
	if (Pname(dtor)->n_dcl_printed==0) Pname(dtor)->dcl_print(0);

	return ee;
}

//extern int new_used;	// pre-define new and delete only if the user didn't

Pstmt fct::dtor_simpl(Pclass cl, Pexpr th)
{
	Pstmt dtail = 0;
//error('d',"simpl_dtor(%t) a %d",cl,f_args);
//	if (new_used == 0) new_init();

	Pname fa = new name("__free");	// fake argument for dtor
	fa->tp = int_type;
	Pname free_arg = fa->dcl(body->memtbl,ARG);
	free_arg->where = no_where;
	delete fa;
	Pname a = f_args;
	if (a==0 || a->n_list==0) error('i',"__freeA missing in destructor for %t",cl);
	Pname p = 0;
	for(;;p=a,a=a->n_list) {	// replace nameless fake argument
//error('d',"a %d %t",a,a->tp);
		if (a->n_list == 0) {
		//	a->n_list = free_arg;
			DEL(p->n_list);
			p->n_list = free_arg;
			break;
		}
	} 

	// generate calls to destructors for all members of class cl:
	Ptable tbl = cl->memtbl;
	int i = 1;
	for (Pname m=tbl->get_mem(i); m; NEXT_NAME(tbl,m,i) ) {
		if ( m->base == TNAME || m->tp==0 ) continue;

		if (m->n_stclass == STATIC || m->base == PUBLIC) continue;

		Ptype t = m->tp;
		Pexpr ee = 0;
		Pname cn;
		Pname dtor;

		if (cn = t->is_cl_obj()) {
			Pclass cl = (Pclass)cn->tp;
			if (dtor = cl->has_dtor()) {	// dtor(this,0,ones);
				ee = new ref(REF,th,m);
				ee->tp = m->tp;
				ee = call_dtor(ee,dtor,0,DOT,one);
				check_visibility(dtor,0,Pclass(dtor->n_table->t_name->tp),tbl,curr_fct);
			}
		}
		else if (cl_obj_vec) {
			Pclass cl = Pclass(cl_obj_vec->tp);
			if (dtor = cl->has_dtor()) {
	Pfct f = Pfct(dtor->tp);
	int i = 0;
	for (Pname nn = f->f_args->n_list; nn && nn->n_list; nn=nn->n_list) i++;
//error('d',"dtor %n i %d",dtor,i);
				ee = new ref(REF,th,m);
				ee->tp = m->tp;
				ee = cdvec(vec_del_fct,ee,cl,dtor,0,new ival(i));
				check_visibility(dtor,0,Pclass(dtor->n_table->t_name->tp),tbl,curr_fct);
			}
		}

		if (ee) {
			Pstmt es = new estmt(SM,curloc,ee,0);
			// reverse order of destructors for members
			es->s_list = del_list;
			del_list = es;
			if (dtail == 0) dtail = es;
		}
	}


	Pexpr ee = 0;
	// look for bases with destructors:
	// generate: dtor(base,0);

	Pbcl b = 0;	// get dtors in order with virtual bases last
	Pbcl t = 0;
	for (Pbcl l = cl->baselist; l; l=l->next) {
		if (l->base != VIRTUAL) {
			Pbcl x = new basecl(l->bclass,0);
			if (t == 0)
				b  = x;
			else
				t->next = x;
			t = x;
			x->base = l->base;
			x->obj_offset = l->obj_offset;
		}
	}
	for (l = cl->baselist; l; l=l->next) {
		if (l->base == VIRTUAL) {
			b = new basecl(l->bclass,b);
			b->base = l->base;
			b->obj_offset = l->obj_offset;
		}
	}

	for (; b; b=l) {
		Pclass bcl = b->bclass;
		l = b->next;
		Pname dtor = bcl->has_dtor();

		if (dtor) { 
			Pexpr val = rptr(bcl->this_type,th,b->obj_offset);
			val = val->contents();
			Pexpr e = call_dtor(val,dtor);
			if (b->base == VIRTUAL) {
				// if (x) this->x.dtor(); where x is a vbase
				for (Pname dd = f_this->n_list; dd!=argtype; dd=dd->n_list) 
				if (strcmp(dd->string,bcl->string)==0) break;

 				if (ansi_opt) { // q?void:int would be an error
					e = new expr(G_CM,e,zero);
					e->tp = zero_type;
				}
				e = new expr(QUEST,e,zero);
				Pexpr two = new ival(2);
				two->tp = int_type;
				e->cond = new expr (AND,free_arg,two);//dd;
			}
			ee = ee ? new expr(CM,e,ee) : e;
		}
		delete b;
	}

	Pstmt es = ee ? new estmt(SM,curloc,ee,0) : 0;

	ee = new expr(ELIST,th,0);	// free storage
	Pname n = new name(oper_name(DELETE));
	Pexpr del = find_name(n,cl,scope,CALL,curr_fct);
	if (del->tp->base==OVERLOAD || Pfct(del->tp)->nargs==2) {
		Pexpr ss = new texpr(SIZEOF,cl,0);
		ss->tp = uint_type;
		ee->e2 = new expr(ELIST,ss,0);
	}
	ee = new call(del,ee);
	ee->tp = ee->call_fct(scope);

	Pstmt ess = new estmt(SM,curloc,ee,0);
	ess = new ifstmt(curloc,new expr(AND,free_arg,one),ess,0);
	if (es)
		es->s_list = ess;
	else
		es = ess;

	free_arg->use();
	Pname(th)->use();

	if (dtail)
		dtail->s_list = es;
	else
		del_list = es;

	// Remove second check of this == 0 in dtor
	if (!perf_opt)
		del_list = new ifstmt(curloc,th,del_list,0);
	if (del_list) del_list->simpl();
	return dtail;
}


Pclass find_vbase_ptr(Pclass cl, Pclass vbase)
{
	for (Pbcl bb = cl->baselist; bb; bb = bb->next) {
		if (bb->base==VIRTUAL) {
			if (same_class(bb->bclass,vbase) && bb->ptr_offset) return cl;
		}
		else {
			Pclass f = find_vbase_ptr(bb->bclass,vbase);
			if (f) return f;
		}
	}
	return 0;
}

Pclass second_base(Pclass cl, Pclass base)
{
//error('d',"second_base(%t,%t)",cl,base);
	for (Pbcl b = cl->baselist; b && b->base==NAME; b = b->next) {
		if (same_class(b->bclass,base)) {
			if (b==cl->baselist) return 0;
			return base;
		}
		Pclass bb = second_base(b->bclass,base);
		if (bb == Pclass(-1)) continue;	// not found
		if (bb) {
			if (b!=cl->baselist) error('s',"C hierarchy too complicated");
			return bb;	// second base of b->bclass
		}
		if (b==cl->baselist) return 0;
		return b->bclass;	// b->bclass is second base
	}
	return Pclass(-1);
}

//Pclass topclass;

Pexpr classdef::get_vptr_exp(char *s)
{
//error('d',"%t::get_vptr_exp(%s)",this,s?s:"0");
	if (c_body == 1) dcl_print(0);
	if (c_body == 3) print_all_vtbls(this);
	if (s == 0) return find_vptr(this);
	Pbcl b = get_base(get_classname(s));
	if ( strcmp(s,b->bclass->string)==0 ) s = 0;
	Pexpr vp = b->bclass->get_vptr_exp(s?drop_classname(s):0);
	if (b==baselist && b->base!=VIRTUAL) return vp;
	vp = new mdot(b->bclass->string,vp);
	if (c_body == 1) b->bclass->dcl_print(0);
	if (c_body == 3) b->bclass->print_all_vtbls(b->bclass);
	if (b->base == VIRTUAL) {
		vp->i1 = 1;
		if (b->ptr_offset == 0) { // pointer to base in intermediate base
			Pclass bb = find_vbase_ptr(this,b->bclass);
			Pclass sb = second_base(topclass,bb);
//error('d',"mdot %t %t : %d %t ",topclass,bb,sb,sb==Pclass(-1)?0:sb);
			if (sb && sb!=Pclass(-1)) vp = new mdot(sb->string,vp);
		}
	}
	return vp;
}

int fct::ctor_simpl(Pclass cl, Pexpr th)
{
	Ptable tbl = cl->memtbl;

	int ass_count = 0;
	init_list = 0;
// error('d',"ctor_simpl %t: %t",cl,this);
	/*
		initialization order:
		(1)	virtual base pointers and virtual bases
				(they may be used in non-virtual bases)
		(2)	non-virtual bases
		(3)	virtual function pointers
		(4)	members
	*/

	// initialize virtual base pointers and virtual base objects
	for (Pbcl l = cl->baselist; l; l=l->next) {
		Pexpr i = l->init;
// error('d',"ctor_simpl: i %k", i?i->base:0);

		if (l->base != VIRTUAL) continue;
		l->init = 0;
// error('d',"simpl virtual base %t i %d ",l->bclass,i);

		// assign virtual OP to virtual base AP
		Pclass bc = l->bclass;
		Pexpr dp = 0;

		// dd = pointer argument for this base;
		// non-zero if already initialized
		for (Pname dd = f_this->n_list; dd!=argtype; dd=dd->n_list) 
			if (strcmp(dd->string,bc->string)==0) break;

		// initialize virtual base object
// error('d',"bc %t dd %n offset %d init %d",bc,dd,l->obj_offset,l->init);
		// => bc_arg = (bc*)((char*)this+offset)
		int off = l->obj_offset;
		Pexpr val = rptr(bc->this_type,th,off);
		dp = new expr(ASSIGN,dd,val);
		dd->assign();

		// => bc::bc()
		if (i)  {
// error('d',"ctor_simpl: i: %k", i->base);
			switch (i->base) {
			case ASSIGN:
			case CM:
				i->simpl();
				break;
			case CALL:
			case G_CALL:
			{	Pcall cc = Pcall(i);
				Pname bn = cc->fct_name;
				ass_count = Pfct(bn->tp)->f_this->n_assigned_to;
				cc->simpl();
				break;
			}
			default:
				error('i',"badBCIr %k",i->base);
			}
			dp = new expr(CM,dp,i);
		}

		// => (bc_arg==0)?dp:bc_arg
		for (Pname a = f_args->n_list; a; a = a->n_list)
			if (strcmp(bc->string,a->string)==0) {
				dp = new expr(QUEST,dp,a);
				dp->cond = new expr(EQ,a,zero);
				break;
			}

		//	Pname dpp = find_vbase_ptr(bc,cl);
		for (Pbcl ll=cl->baselist; ll; ll=ll->next) {
			if (same_class(ll->bclass,bc) && ll->ptr_offset) {
				// make sure that the delegate_arg is needed
				// here and not just in a base

				// this->Pd = (bc_arg==0)?dp:bc_arg
				Pexpr dpp = new mdot(bc->string,th);
				dpp->i1 = 3;
				dp = new expr(ASSIGN,dpp,dp);
//error('d',"dpp %n",dpp);
				break;
			}
		}

		// reverse init order
		if (dp) init_list = init_list ? new expr(CM,dp,init_list) : dp;
	}

	//	generate: this=base::base(args) (non-virtual bases)
{	
	for (Pbcl l = cl->baselist; l; l=l->next) {
		Pexpr i = l->init;
		if (i==0 || l->base==VIRTUAL) continue;
//error('d',"simpl base %t i %d ",l->bclass,i);
		l->init = 0;
	
		switch (i->base) {
		case ASSIGN:
		case CM:
			i->simpl();
			break;
		case CALL:
		case G_CALL:
		{	Pcall cc = Pcall(i);
			Pname bn = cc->fct_name;
			ass_count = Pfct(bn->tp)->f_this->n_assigned_to;
			cc->simpl();
			// assign to ``this'' only from only base
			if (l==cl->baselist && cl->baselist->next==0) i = new expr(ASSIGN,th,cc);
			break;
		}
		default:
			error('i',"badBCIr %k",i->base);
		}

		init_list = init_list ? new expr(G_CM,init_list,i) : i;
	}
}

	// initialize the vptrs that are updated by this class
	for (Pvirt blist = cl->virt_list; blist; blist = blist->next) {
	//	if (blist->string==0 && find_vptr(cl)==0) {	//opt
//error('d',"!!!vptr init %s in %s",blist->string,cl->string);
	//		continue;
	//	}
//error('d',"vptr init %s in %s",blist->string,cl->string);
		topclass = cl;
		Pexpr vp = cl->get_vptr_exp(blist->string);

                char *str = 0;
		char *cs = cl->nested_sig?cl->nested_sig:cl->string;
                if (cl->lex_level && cl->nested_sig==0) {
			str = new char[ cl->c_strlen + 1 ];
			strcpy(str,cl->local_sig);
		}
                Pexpr vtbl = new text_expr(blist->string,str?str:cs);

		Pexpr ee = new ref(REF,th,vp);
		ee->tp = vp->tp;
		ee = new expr(ASSIGN,ee,vtbl);
		init_list = init_list ? new expr(CM,init_list,ee) : ee;
	}

	// initialize members in declaration order:
	int i;
	for (Pname m=tbl->get_mem(i=1); m; NEXT_NAME(tbl,m,i) ) {
		if ( m->base == TNAME ) continue;
		Ptype t = m->tp;
		Pname cn;
		Pname ctor;
		if (t == 0) continue;

		switch (t->base) {
		case FCT:
		case OVERLOAD:
		case CLASS:
		case ENUM:
			continue;
		}

		switch (m->n_stclass) {
		case STATIC:
		case ENUM:
			continue;
		}

		if (m->base == PUBLIC) continue;

		Pexpr ee = m->n_initializer;
		if (ee) m->n_initializer = 0;	// from fct must not persist until next fct
//error('d',"simpl m %n ee %d",m,ee);
		if (ee) {
			// init of non-class mem
			// set in fct::mem_init()
		}
		else if (cn=t->is_cl_obj()) {	// try for default
			Pclass cl = Pclass(cn->tp);
			if (ctor = cl->has_ictor()) {
				Pexpr r = new ref(REF,th,m);
				ee = call_ctor(tbl,r,ctor,0,DOT);
				check_visibility(ctor,0,Pclass(ctor->n_table->t_name->tp),tbl,curr_fct);
			}
			else if (cl->has_ctor()) {
				error("M%n needsIr (no defaultK forC %s)",m,cl->string);
			}
		}
		else if (cl_obj_vec) {
			Pclass cl = Pclass(cl_obj_vec->tp);
			if (ctor = cl->has_ictor()) { // _new_vec(vec,noe,sz,ctor);
				Pexpr mm = new ref(REF,th,m);
				mm->tp = m->tp;
				// ctor may contain default arguments: check and replace
				Pname vctor = cl->has_vtor();
				ee = cdvec(vec_new_fct,mm,cl,vctor?vctor:ctor,-1,0);
				check_visibility(ctor,0,Pclass(ctor->n_table->t_name->tp),tbl,curr_fct);
			}
			else if (cl->has_ctor()) {
				error("M%n[] needsIr (no defaultK forC %s)",m,cl->string);
			}
		}
		else if (t->is_ref()) {
			error("RM%n needsIr",m);
		}
		else if (t->tconst() && vec_const==0) {
			error("constM%n needsIr",m);
		}
	
		if (ee) {
			ee->simpl();
			init_list = init_list ? new expr(CM,init_list,ee) : ee;
		}
	} // for m
//error('d',"ctor->");
	return ass_count;
}

void fct::simpl()
/*
	call only for the function definition (body != 0)

	simplify argument initializers, and base class initializer, if any
	then simplify the body, if any

	for constructor:call allocator if this==0 and this not assigned to
			(auto and static objects call constructor with this!=0,
			the new operator generates calls with this==0)
			call base & member constructors
	for destructor:	call deallocator (no effect if this==0)
			case base & member destructors

	for arguments and function return values look for class objects
	that must be passed by constructor X(X&).

	Allocate temporaries for class object expressions, and see if
	class object return values can be passed as pointers.

	call constructor and destructor for local class variables.
*/
{
	Pexpr th = f_this;
	Ptable tbl = body->memtbl;
	Pstmt tail;
	Pclass cl = 0;
	if (th && th->tp && Pptr(th->tp)->typ && Pbase(Pptr(th->tp)->typ)->b_name)
		cl = Pclass(Pbase(Pptr(th->tp)->typ)->b_name->tp);
	Pstmt dtail = 0;

	Pname ocurr_fct = curr_fct;
	int oret = no_of_returns;
	int oim = imeasure;

//error('d',"fct::simpl %n %t",curr_fct,this);
//	not_inl = f_inline==0;
	del_list = 0;
	continue_del_list = 0;
	break_del_list = 0;
	scope = tbl;
	if (scope == 0) error('i',"F::simpl()");
	int ass_count = 0;
	imeasure = 0;
	no_of_returns = 0;

	cc->stack();
	cc->nof = curr_fct;
	cc->ftbl = tbl;

//      modification for local classes defined within inline functions
// error( 'd', "simpl local_class: %d", local_class );
	for ( Plist l = local_class; l; l = l->l ) {
    		Pname n = l->f; 
    		Pclass cl = Pclass(Pbase(n->tp)->b_name->tp);
    		if ( cl->c_body == 1 ) cl->dcl_print(0);
	}

	Pfct(cc->nof->tp)->local_class = local_class; 
	local_class = 0;
// error( 'd', "simpl nof: %n %d local_class: %d", cc->nof, cc->nof, Pfct(cc->nof->tp)->local_class );

	switch (curr_fct->n_scope) {
	case 0:
	case PUBLIC:
		cc->not = curr_fct->n_table->t_name;
		cc->cot = Pclass(cc->not->tp);
		cc->tot = cc->cot->this_type;
	}

	switch (curr_fct->n_oper) {
	case  DTOR:
		dtail = dtor_simpl(cl,th);
		break;
	case CTOR:
		ass_count = ctor_simpl(cl,th);
	}

	extern bit need_lift;
	bit onl = need_lift;
	Ptable ott = tmp_tbl;
	need_lift = 0;
	tmp_tbl = new table(CTBLSIZE, 0, 0);

	tail = body->simpl();

	need_lift = onl;
	delete tmp_tbl;
	tmp_tbl = ott;

	if (!body)
		return;

	if (returns->skiptypedefs()->base!=VOID || f_result) {	// return must have been seen
		if (no_of_returns) {		// could be OK
			Pstmt tt = (tail->base==RETURN || tail->base==LABEL) ? tail : trim_tail(tail);

			if (tt)
			switch (tt->base) {
			case RETURN:
			case GOTO:
				del_list = 0;	// no need for del_list
				break;
			case SM:
				if (tt->e)
					switch (tt->e->base) {
					case ICALL:
					case G_CALL:
						goto chicken;
					}
			default:
				if (warning_opt || strcmp(curr_fct->string,"main"))	
					error('w',"maybe no value returned from%n",curr_fct);
			case IF:
				if (tt->base==IF && tail->base==IF && tt->else_stmt==0 && no_of_returns==1) {
				    if (strcmp(curr_fct->string,"main")==0
					&& returns->skiptypedefs()->base != VOID) {
					error('w',&tt->where,"maybe no value returned from main()");
				    } else
					error(&tt->where,"if with return but no else with return");
				}
			case SWITCH:
			case DO:
			case WHILE:
			case FOR:
			case LABEL:
			chicken:		// don't dare write a warning
				break;
			}
		}
		else {	// must be an error
			// but we don't dare complain about main()
		//	if (strcmp(curr_fct->string,"main"))
		//		error(Pfct(curr_fct->tp)->returns->is_cl_obj()?0:'w',"no value returned from%n",curr_fct);
		//	else if (warning_opt)	
		//		error('w',"no value returned from%n",curr_fct);
			if (Pfct(curr_fct->tp)->f_inline
			&& Pfct(curr_fct->tp)->returns->skiptypedefs()!=void_type
			&& Pfct(curr_fct->tp)->returns->is_cl_obj())
				// can cause code generation errors if allowed
				error("no value returned from%n",curr_fct);
			else if (strcmp(curr_fct->string,"main"))	
				{
				if (Pfct (curr_fct->tp)->is_templ() && Pfct(curr_fct->tp)->returns==defa_type)
					error("no value returned from%n",curr_fct);
				else if (Pfct (curr_fct->tp)->returns == defa_type && ! Pfct(curr_fct->tp)->memof && !curr_fct->n_oper && simpl_friend==0)

					error(strict_opt?0:'w',"no value returned from%n (anachronism)",curr_fct);
				else
					error("no value returned from%n",curr_fct);
				}
			if (warning_opt && strcmp(curr_fct->string,"main")==0 &&Pfct(curr_fct->tp)->returns->skiptypedefs()!=void_type)
				error('w',"no value returned from main()");
		}
		if (del_list) goto zaq;
	}
	else if (del_list) {	// return may not have been seen
	zaq:
		if (tail)
			tail->s_list = del_list;
		else
			body->s = del_list;
		tail = dtail;
	}

	if (curr_fct->n_oper == DTOR) {	// body => if (this == 0) body

// reset the vptrs that were set by this class
	for (Pvirt blist = cl->virt_list; blist; blist = blist->next) {
//error('d',"vptr init %s in %s",blist->string,cl->string);
		topclass = cl;
		Pexpr vp = cl->get_vptr_exp(blist->string);

                char *str = 0;
		char *cs = cl->nested_sig?cl->nested_sig:cl->string;
                if (cl->lex_level && cl->nested_sig==0) {
			str = new char[ cl->c_strlen + 1 ];
			strcpy(str,cl->local_sig);
		}
                Pexpr vtbl = new text_expr(blist->string,str?str:cs);

		Pexpr ee = new ref(REF,th,vp);
		ee->tp = vp->tp;
		ee = new expr(ASSIGN,ee,vtbl);
		Pstmt es = new estmt(SM,curloc,ee,0);
		es->s_list = body->s;
		body->s = es;
	}

		body->s = new ifstmt(body->where,th,body->s,0);
	}

	if (curr_fct->n_oper == CTOR) {

		loc temploc=body->where;
		Pstmt tempss=body->s;
		while (tempss) {
			temploc=tempss->where;
			tempss=tempss->s_list;
		}
		
		if  (Pname(th)->n_assigned_to == 0) {
		/* generate:
			if (this || (this=_new( sizeof(class cl) ))) {
				init_list ;
				body;
			}
		*/
			Pname(th)->n_assigned_to = ass_count ? ass_count : FUDGE111;
			Pexpr sz = new texpr(SIZEOF,cl,0);
			(void) cl->tsizeof();
			sz->tp = uint_type;
 			Pexpr ee;
 			Pname n;
 			Pexpr p;
 			if(!perf_opt) {
                                 // Don't generate the if this == 0 test
				ee = new expr(ELIST,sz,0);
				n = new name(oper_name(NEW));
				p = find_name(n,cl,scope,CALL,curr_fct);
  //error('d',"in ctor %n call %n",curr_fct,p);
				ee = new call(p,ee);
				(void) ee->call_fct(cl->memtbl);
				ee->simpl();
				ee = new expr(ASSIGN,th,ee);
				ee = new expr(OROR,th,ee);
			}
			/*ifs->simpl();
				do not simplify
				or "this = " will cause an extra call of base::base
			*/
			if (init_list) {
				Pstmt es = new estmt(SM,body->where,init_list,0);
				es->s_list = body->s;
				body->s = es;
			//	if (tail == 0) tail = es;
			}
			else if (body->s == 0)
				body->s = new estmt(SM,body->where,0,0);
			else if(tail->base==RETURN) {
				if(body->s == tail) {
					delete body->s;
					body->s = new estmt(SM,body->where,0,0);
				}
				else {
					Pstmt pr = body->s;
					while(pr->s_list != tail)
						pr = pr->s_list;
					delete pr->s_list;
					pr->s_list = 0;
				}
			}

                       if (perf_opt) {
                                // Don't include the if this == 0
                                // check.  Space will be allocated
                                // or constructor will not be called.
				if (tail == 0)
					tail = body->s;
                        }
                        else {
                                ifstmt* ifs = new ifstmt(body->where,ee,body->s,0);
                                body->s = ifs;
                        //      if (tail == 0) 
                                tail = ifs;
                        }

		}
		// generate: body; return this;
		Pstmt st = new estmt(RETURN,temploc,th,0);
		if (tail)
			tail->s_list = st;
		else
			body->s = st;
		tail = st;
	}
	f_imeasure = imeasure;
	curr_fct = ocurr_fct;
	no_of_returns = oret;
	imeasure = oim;
	cc->unstack();
}


void classdef::simpl()
{
	int i;
//error('d',"classdef::simpl %s %d",string,defined&SIMPLIFIED);

	if (defined&SIMPLIFIED) return;
	if ((defined&DEFINED) == 0) return; // forward dec only 

	cc->stack();
	cc->cot = this;
	cc->tot = this_type;
	cc->not = memtbl->t_name;

	for (Pname m=memtbl->get_mem(i=1); m; NEXT_NAME(memtbl,m,i) ) {
		if ( m->base == TNAME ) continue;
		if ( has_ctor()
		// ctor (function) name should have been mangled
		&&   strcmp(m->string,string) == 0
		)
			error(&m->where,"nonFM%n inCWK",m);
		Pexpr i = m->n_initializer;
		m->n_initializer = 0;
		m->simpl();
		m->n_initializer = i;
	}

	for (Plist fl=friend_list; fl; fl=fl->l) {	// simplify friends
		Pname p = fl->f;
		switch (p->tp->base) {
		case FCT:
		case OVERLOAD:
			p->simpl();
		}
	}

	defined |= SIMPLIFIED;
	cc->unstack();
}
