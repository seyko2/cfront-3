/*ident	"@(#)cls4:src/expr3.c	1.38" */
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

expr3.c:

	type check function calls, casts, and explicit coercions

************************************************************************/

#include "cfront.h"
#include "size.h"
#include "template.h"
#include "overload.h"

static Bits intersectRule(const Block(BlockPname)&, int, Pexpr);
static Bits bestMatch(const Block(Pname)&, int, Ptype);
static Bits best_const(const Block(Pname)&, int, Ptype);
static Pname breakTie(const Block(Pname)&, Bits&, Pexpr, int);
static int pr_dominate(Ptype, Ptype);

static int refd;// initialization routine called by ref_init, do not apply itor
static int no_sti;
static int miFlag;
int no_const;
extern int stat_init;
int ambig;
bit in_return=0;

int pr_dominate(Ptype t1, Ptype t2)
/*
*/
{
	Pname cn1 = t1->is_cl_obj();
	Pname cn2 = t2->is_cl_obj();

	if (cn1==0 || cn2==0) {
		Ptype p1 = t1->is_ptr();
		Ptype p2 = t2->is_ptr();
		if (p1 && p2) {			// pointers
			cn1 = Pptr(p1)->typ->is_cl_obj();
			cn2 = Pptr(p2)->typ->is_cl_obj();
			if (cn1==0 || cn2==0) return 0;
		}
		else {
			p1 = t1->is_ref();
			p2 = t2->is_ref();
			if (p1 && p2) {		// references
				cn1 = Pptr(p1)->typ->is_cl_obj();
				cn2 = Pptr(p2)->typ->is_cl_obj();
				if (cn1==0 || cn2==0) return 0;
			}
			else
				return 0;	// not the same and not classes
		}
	}
	Pclass c1 = Pclass(cn1->tp);
	Pclass c2 = Pclass(cn2->tp);

	if (c1->has_base(c2)) return 1;
	if (c2->has_base(c1)) return 2;
	return 0;
}

Pname Ntmp;
Pname Ntmp_refd;
Pname Ntmp_flag;
extern Pname Ntmp_flag_list;

Pname make_tmp(char c, Ptype t, Ptable tbl)
{
	int dt = 0;
	Pname tn = tbl->t_name;
	Pname cn = t->is_cl_obj();

	if (tn && tn->tp) error('s',"defaultA too complicated");
	if (cn && Pclass(cn->tp)->has_dtor()) dt = 1;
	if (Ntmp == 0 && dt ) Ntmp = cn;

//error('d',"tbl %d cstmt %d %d sti %d",tbl,Cstmt,Cstmt?Cstmt->memtbl:0,sti_tbl);
	if (Cstmt) {	//	make Cstmt into a block
		if (Cstmt->memtbl == 0) Cstmt->memtbl = new table(4,tbl,0);
		tbl = Cstmt->memtbl;
	}
	else if (tbl == gtbl && no_sti == 0) {
		if (sti_tbl == 0) sti_tbl = new table(8,gtbl,0);
		tbl = sti_tbl;
	}

	Pname tmpx = new name(make_name(c));
	tmpx->where = no_where;
	tmpx->tp = t;
	(void) t->tsizeof();
	if ( t->base == COBJ ) {
     		Pclass cl = Pclass(Pbase(t)->b_name->tp);
     		if ( cl->lex_level ) tmpx->lex_level = cl->lex_level;
	}

	TOK scop = ARG;
	if (stat_init && dt) { 
		tmpx->n_sto = STATIC; scop = ARGS; 
	} else if (gtbl == tbl) {
		tmpx->n_sto = STATIC;
	}

	// ARG[S]: no init; ARGS: static dtor
	Pname tmp = tmpx->dcl(tbl,scop); 
	delete tmpx;

	// n_scope == ARGS sets static dtor in simpl2.c
	tmp->n_scope = (scop==ARG) ? FCT : ARGS;
	return tmp;
}

Pexpr init_tmp(Pname tmp, Pexpr init, Ptable tbl)
{
	Pname cn = tmp->tp->is_cl_obj();
	Pname ct = cn ? Pclass(cn->tp)->has_itor() : 0;

	tmp->n_assigned_to = 1;
//error('d',"init_tmp %n ct %n refd %d",tmp,ct,refd);
	if (ct) {	// must initialize
		if (refd) {
//error('d',"'orrible %k",init->e1->base);
			switch (init->e1->base) {	// 'orrible 'ack
			case NAME:
			case REF:
			case DEREF:
				if (init->base == QUEST)
					init = init->address();
				else if (init->e1->tp->is_ptr())
					init = init->e1;
				else
					init = new expr(G_CM,init,init->e1->address());
					
			}
			if (ct->tp->base == OVERLOAD) ct = Pgen(ct->tp)->fct_list->f;	// first fct
			tbl = 0;
			// when tbl==0, call_ctor will not force declaration of ct
			if (ct->n_dcl_printed==0) ct->dcl_print(0);
		}
		return call_ctor(tbl,tmp,ct,init,DOT);
	}

	if (ansi_opt)
		tmp->tp->ansi_const=1;

	Pexpr ass;
	if(init->base == ZERO && tmp->tp->memptr())
		ass = mptr_assign(tmp,init);
	else ass = new expr(ASSIGN,tmp,init); // no ctor: can assign
	ass->tp = tmp->tp;
	return ass;
}

int exact3(Pname nn, Ptype at)
/*
	return 1 if
	match with standard conversions
*/
{
	if (nn == 0) return 0;
	Ptype nt = nn->tp->skiptypedefs();

	if (at == nt) return 1;

	switch (nt->base) {
	case RPTR:
		if (nt->base==RPTR && Pptr(nt)->typ->check(at,COERCE)==0) 
			return 1;
		if (at==zero_type && Pptr(nt)->typ->is_ptr()==0) return 0;
		if (nt->check(at,COERCE)) {
			Pptr pt = at->addrof();
			nt->base = PTR;		// handle derived classes
			if (nt->check(pt,COERCE)) {
				nt->base = RPTR;
				delete pt;
				return 0;
			}
			nt->base = RPTR;
			delete pt;
		}
		break;
	default:
		switch (at->base) {
		default: 
			if (nt->check(at,COERCE)) return 0;
			break;
		case OVERLOAD:
			// the actual argument is an overloaded function
			// we'll try each instance until one matches
			register Plist gl;
			int no_match = 1;

			for (gl = Pgen(at)->fct_list; gl; gl=gl->l) {
				if (nt->check(gl->f->tp,COERCE)==0) {
					no_match = 0;
					break;
				}
			}

			if ( no_match ) return 0;
		} 
	}
	return 1;
}

int exact1(Pname nn, Ptype at)
/*
	return 1 if
	exact match with
		T <-> const T
		X -> X&
		T* -> const T*
		T* -> T*const
	taken into account
*/
{
	if (nn == 0) return 0;
	Ptype nt = nn->tp->skiptypedefs();
	if (at == nt) return 1;
	if (at->base == ZTYPE) at = int_type;

	// if the actual argument is an overloaded function
	// we'll see if any instance matches exactly
	if (at->base == OVERLOAD) {
		register Plist gl;

		for (gl = Pgen(at)->fct_list; gl; gl=gl->l) {
			if (nt->check(gl->f->tp,0)==0) {
				return 1;
			}
		}
		return 0;
	}

	Pptr rt = nt->is_ptr();
	Pptr art = at->is_ptr();

	if (nt->check(at,0)) {
		// handle T <-> const T
		if (const_problem && nt->base != PTR)  return 1;	

		// reject ptmfs of different classes
		if (rt && rt->memof || art && art->memof) return 0;

		rt = nt->is_ref();	//handle X -> X&
		if (rt && (at->check(Pptr(rt)->typ,0)==0 || const_problem))
			return 1;

		if (rt && art) return 0; // ptrs do not match refs

		// handle T* -> const T* and
		// T* -> T*const
		if (rt || (rt = nt->is_ptr())) {
			if (art == 0) art = at->is_ref();
			if (art) {
				if (art->typ->check(rt->typ,0)) {
					if (const_problem && 
					    Pbase(art->typ)->b_const != 1)
						return 1;
				}
				else	// T* -> T*const
					 return 1;
			}
		}
		return 0;
	}
	return 1;
}

int exact2(Pname nn, Ptype at)
/*
	return 1 if
	do integral promotion and float->double on at, then match
*/
{
//error('d',"exact2 nt %t at %t",nn?nn->tp:0,at);
	at = at->skiptypedefs();
	switch (at->base) {
	case EOBJ:
		at = Penum(Pbase(at)->b_name->tp)->e_type;
		break;
	case ZTYPE:
		at = int_type;
		break;
	case CHAR:
	case SHORT:
		at =	(Pbase(at)->b_unsigned && at->tsizeof()==SZ_INT) 
			? 
			uint_type 
			: 
			int_type;
		break;
	case FLOAT:
		at = double_type;
	}

	if (nn == 0) return 0;
	return exact1(nn,at);
}

Pname Ncoerce;
static int ref_cast;
int Nstd;

bit can_coerce(Ptype t1, Ptype t2)
/*	
	return number of possible coercions of t2 into t1,
	Ncoerce holds a coercion function (not constructor), if found
*/
{
	int zz = 0;
	Ncoerce = 0;
	no_const = 0;
	if (t2->base == ANY) return 0;

	if (!t1->check(t2,COERCE))	// see if any std conversions
		return 1;

	//t1 = t1->skiptypedefs();

	if (t1->is_ref()) {
		if (t1->check(t2->skiptypedefs()->addrof(),COERCE) == 0) 
			return 1;

		if (!ref_cast) {		// (T&): no coercions
						// except operator T&()
			Ptype tt1 = Pptr(t1->skiptypedefs())->typ->skiptypedefs();
			int bc;
			if ( tt1->base != PTR && tt1->base != RPTR ) {
     				bc = Pbase(tt1)->b_const;
     				Pbase(tt1)->b_const = 0; 
			}
			int i = can_coerce(tt1,t2);
			if ( tt1->base != PTR && tt1->base != RPTR ) 
     				Pbase(tt1)->b_const = bc;
			if (i) return i;
			zz = 1;
		}
	}

	Pname c1 = t1->is_cl_obj();
	Pname c2 = t2->is_cl_obj();
	int val = 0;
	if (c1 && !ref_cast && !zz) {
		Pclass cl = Pclass(c1->tp);
		if (c2 && same_class(Pclass(c2->tp),cl)) return 1;

                // A more comprehensive test for template classes
		if (c2 && same_class(Pclass(c1->tp), Pclass(c2->tp)))
			 return 1 ;

		/* look for a constructor with one argument
		   or with default for second argument of
		   acceptable type */

		Pname ctor = cl->has_ctor();
		register Pfct f = ctor ? Pfct(ctor->tp) : 0;

		if(f && f->base == FCT) {
			if (f->nargs==1 
			    ||
			    f->nargs > 1 && f->argtype->n_list->n_initializer
			) {
				if (exact3(f->argtype,t2)) {
					val = 1;
				}
			}
		}
		else if (f && f->base == OVERLOAD) {
			register Plist gl;

			for (gl=Pgen(f)->fct_list; gl; gl=gl->l) {
				Pname nn = gl->f;
				Pfct ff = Pfct(nn->tp);

				if (ff->nargs==1 
				    ||
			    	    ff->nargs>1
				    &&
				    ff->argtype->n_list->n_initializer
				) {
					if (exact3(ff->argtype,t2)) {
						val = 1;
						break;
					}
				}
			}
		}
		else if (f) {
			error('i',"cannot_coerce(%k)\n",f->base);
		}
	}

	if (c2) {	
		const int REALLY_EXACT = 6;
		Block(Pname) conv;
		int found = 0;
		Pclass cl = Pclass(c2->tp);
		int best = 0;
		for (Pname ox, on=cl->conv; on; on=ox) {
			ox = on->n_list;
			Pfct f = on->get_fct();
			// const check should be here
			Ptype rt = f->returns;
			Pptr rr = rt->is_ref();
			if (rr && !t1->is_ref()) rt = rr->typ;
			Nstd = 0;
			int howgood = 0;
 			if (t1->check(rt,0) == 0) {
				howgood = REALLY_EXACT;
			}
 			else if (t1->check(rt,COERCE)==0) {
				if (!Nstd) howgood = EXACT;
				else {
					Pname t_nn = new name;
					t_nn->tp = t1->skiptypedefs();
					if(exact2(t_nn,rt)) howgood = PROM;
					else howgood = STD;
				}
			}
			// this check here for the sake of an error message
			if(t2->tconst() && f->f_const==0) {
				if(howgood) no_const++;
				continue;
			}
			if(howgood == 0 || howgood < best) continue;
			if(howgood > best) { found = 0; best = howgood; }
			conv.size(found+1);
			conv[found++] = on;
		}
		if (found) {
			if(found == 1) Ncoerce = conv[0];
			else {
				Bits b = best_conv(conv,found,t1->tconst());
				Ncoerce = conv[b.signif() - 1];
			}
			val = val + found;
		}
	}
	if (val) return val;
	if (c1 && Pclass(c1->tp)->has_itor()) return 0;
	if (t1->check(t2,COERCE)) return 0;
	return 1;
}

int matchable(Pname n, Pexpr arg, int constObj)
/*
	look to see if the argument list "arg" can match a call of "n"
	return worst kind of conversion done or NONE
*/
{
	Pfct f = n->fct_type();
	register Pexpr e;
	register Pname nn;
	int worst = EXACT; 	//for compatibilty

	if (n->is_template_fct()) return 0;

	if(constObj && n->n_oper!=CTOR && !f->f_const && !f->f_static) {
		non_const++;
		return NONE;
	}

	for(e=arg, nn=f->argtype; e; e=e->e2, nn=nn->n_list) {
		if (nn == 0) return f->nargs_known==ELLIPSIS;
		Pexpr a = e->e1;
		Ptype at = a->tp;
		if (at->base==ANY) return NONE;
		if (exact1(nn,at)) {worst=min(worst,EXACT);continue;}
		if (exact2(nn,at)) {worst=min(worst,PROM);continue;}
		if (exact3(nn,at)) {worst=min(worst,STD);continue;}
		int cc = can_coerce(nn->tp,at);
		if (cc != 1) return NONE;
		else worst=UDC;
	}
	if (nn && nn->n_initializer==0) return NONE;
	return min(worst,EXACT);
}

int is_arg;

Ptype expr::call_fct(Ptable tbl)
/*
	check "this" call:
		 e1(e2)
	e1->typ() and e2->typ() has been done
*/
{
	Pfct f;
	Pname fn;
	int x;
	int k;
	Pname nn;
	Pexpr e;
	Ptype t;
	Pexpr arg = e2;
	Ptype t1 = e1?e1->tp:0;
	ambig = 0;
	int argno;
	Pexpr etail = 0;
	bit no_change = 0;
	Pname no_virt = 0;	// set if explicit qualifier was used: c::f()
	Pname chk = 0;		// set if visibility check is needed
				// that is if function name might have been
				// found without use of find_name()
	int const_obj = 0;

	if (t1 == any_type) return any_type;

	switch (base) {
	case CALL:
	case G_CALL:	break;
	default:	error('i',"call_fct(%k)",base);
	}

//error('d',"call %d %k %n arg %d",this,e1->base,e1->base==NAME?e1:0,arg);
	if (t1 == 0) error('i',"call_fct(e1=%d,e1->tp=%t)",e1,t1);
	if (arg && arg->base!=ELIST) error('i',"badAL%d%k",arg,arg->base);

	switch (e1->base) {
	case DTOR: // dummy type destructor
		fn = Pname(e1);
		break;
	case NAME:
		fn = Pname(e1);
		switch (fn->n_oper) {
		case 0:
		case CTOR:
		case DTOR:
		case TYPE:
		case NEW:
		case DELETE:
			break;
		default:	// real operator: check for operator+(1,2);
			{
			if (arg == 0) break;
			Pexpr a = arg->e1;	// first operand

			if (Pfct(fn->tp)->memof	// obj.operator(1) is OK
			|| a->tp->is_cl_obj()
			|| a->tp->is_ref()) break;
			a = arg->e2;
			if (a == 0)		// unary
				error("%k of basicT",fn->n_oper);
			else {			// binary
				a = a->e1;	// second operand
				if (a->tp->is_cl_obj() || a->tp->is_ref()) break;
				error("%k of basicTs",fn->n_oper);
			}
			break;
			}
		}
		break;
	case REF:
	case DOT:
		no_virt = Pname(e1->n_initializer);
		e1->n_initializer = 0;
		if (e1 && e1->e1) {
			Ptype t = e1->e1->tp;
			Pptr tt = t->is_ref();
			t = tt ? tt->typ : t;
			Pptr p_t = t->is_ptr();
			t = e1->base==REF && p_t ? p_t->typ : t;
			tt = t->is_ref();
			t = tt ? tt->typ : t;
			const_obj = t->tconst() ;// ? 1 : e1->e1->is_const_obj();
		}
	case MDOT:
	{	Pexpr n = e1->mem;
	lxlx:
		switch (n->base) {
		case MDOT:
			// reverse mdot (see expr::print())
			//	p->a.b()  => (&p->a)->b() => b(&p->a)
			// or	p->a->b() => (p->a)->b()  => b(p->a)
			// or	oo.a.b()  => (&oo.a)->b() => b(&oo.a)
			// or	oo.a->b() => (oo.a)->b()  => b(oo.a)
		{	
			Pexpr r = e1;
			Pexpr p = r->e1;
			for (Pexpr m = r->mem; m->base==MDOT; m = r->mem) {
				p = new mdot(m->string2,p);
				p->i1 = m->i1+2;
				if (p->mem && Pexpr(p->mem) != Pexpr(1))
					p->tp = p->mem->tp;
				r->mem = m->mem;
				r->e1 = p;
			}
		}
		case REF:
		case DOT:
			n = n->mem;
			goto lxlx;
		case NAME:
		case DTOR:
			break;
		default:
			error('i',"ref %k",n->base);
		}
		fn = Pname(n);
		break;
	}
	case MEMPTR:
	default:
		fn = 0;
	}

lll:
//error('d',"lll: %t %k",t1,t1->base);
	switch (t1->base) {
	case TYPE:
		t1 = Pbase(t1)->b_name->tp;
		goto lll;

	case PTR:	// pf() allowed as shorthand for (*pf)()
		switch (Pptr(t1)->typ->skiptypedefs()->base) {
		case FCT:
		case OVERLOAD:
			if (Pptr(t1)->memof) error("O missing in call throughP toMF");
			t1 = Pptr(t1)->typ;
			fn = 0;
			goto lll;
		}

	default:
		if (fn)
			error("call of%n;%n is a%t",fn,fn,e1->tp);
		else
			error("call of%kE ofT%t",e1->base,e1->tp);

	case ANY:
		return any_type;
	
	case OVERLOAD:
	{
		Pgen g = Pgen(t1);
		Pname found = 0;

		// look for an exact match
		found = g->exactMatch(arg,const_obj);

		// code for calls with template versions 
		if(!found && arg && g->has_templ()) {
			found = has_templ_instance(fn,arg);
		}

		// one argument in call: no need for intersect rule 
		if (!found && arg && arg->e2 == 0) {
			found = g->oneArgMatch(arg,const_obj);
		}

		// multiple arguments in call: potential need for 
		// intersect rule and simple rule
		else if (!found && arg) {
			found = g->multArgMatch(arg,const_obj);
		}

		// no functions are matchable
		if (!found) {
			Block(Pname) tmp(1);
			tmp[0] = fn;
			fmError(0,tmp,arg,const_obj);
			return any_type;
		}

		overFound = chk = fn = found;
		f = fn->fct_type();
		break;
	}

        case FCT:
                f = Pfct(t1);
                if (fn) {
                        if (fn->is_template_fct()) {
                                Pname f_inst = has_templ_instance(fn,arg); 
                                if (f_inst) {
                                        e1 = f_inst; fn = f_inst;
                                        t1 = f_inst->tp; f = Pfct(f_inst->tp);
                                }
                        }
                        else
                                switch (fn->n_oper) {
                                case CTOR:
                                case TYPE:
                                        chk = fn;
                                }
                }
        }

	if (chk) {
		Ptype t = 0;
		Pexpr ee = e1->e1;

		switch (e1->base) {
		case REF:	// ptr->chk()
			if (ee == 0) {	// 0->x() fudge handling new x()
				check_visibility(chk,no_virt,Pclass(chk->n_table->t_name->tp),tbl,cc->nof);
				break;
			};
			t = ee->tp->skiptypedefs();
			t = Pptr(t)->typ;
			break;
		case DOT:	// obj.chk()
			t = ee->tp;
		}

		Pname cn = t?t->is_cl_obj():0;
		Pclass cl = cn?Pclass(cn->tp):0; // class of ``this'' for chk

		if (cl) {
			if (chk->n_oper==CTOR
				&& chk->n_protect
				&& cc->nof
				&& cc->nof->n_oper==CTOR)
					// BUG: cannot handle protected base
					// class constructor
				;
			else {
				check_visibility(chk,no_virt,cl,tbl,cc->nof);
			}
		}
	}

	if (fn && f->returns->is_cl_obj() && f->f_result==0) {
		// protect against class cn; cn f(); ... class cn { cn(cn&); ... };
		make_res(f);
		f->returns->tsizeof();	// make sure it is declared
	}

//error('d',"fn %n %t printed %d",fn,fn?fn->tp:0,fn?fn->n_dcl_printed:0);
	if (fn) {
		// ensure printout of class declaration:
		for (Pname nn=f->argtype; nn; nn=nn->n_list)
			if (nn->tp->is_cl_obj()) (void) nn->tp->tsizeof();
	}

	if (fn && fn->n_dcl_printed==0) {
		if (f->f_inline==0 && f->f_imeasure) {
			extern void uninline(Pname fn);
			uninline(fn);
		}

//		// ensure printout of class declaration:
//		for (Pname nn=f->argtype; nn; nn=nn->n_list)
//			if (nn->tp->is_cl_obj()) (void) nn->tp->tsizeof();

		fn->dcl_print(0);
	}

	if (no_virt && f->f_static==0) {
		if (e1->base==REF || e1->base==DOT) e1->n_initializer = fn;
	}
	else
		fct_name = fn;
//error('d',"fn %n %t %d %d",fn,f,f->f_this,f->f_static);
	if (f->f_this) {	//SSS call of non-static memberfunction
		switch (e1->base) {
		case MEMPTR:
		case REF:
		case DOT:
			break;
		default:
			error("O orP missing for%n ofT %t",fct_name,f);
		}
	}
	else if (fn) {	//SSS call of static function
	sss:
		switch (e1->base) {
		case REF:
		case DOT:
		case MDOT:
			e1 = e1->mem;
			goto sss;
		}
	}

	if (fn) fn->use();	// a patch: ctors are sometimes not use()d

	if (f->f_const==0 && e1->tp && !e1->tp->is_ptr()
	&& (fn==0 || (fn->n_oper!=CTOR && fn->n_oper!=DTOR))) {	//CCC
		switch (e1->base) {
		case REF:
		{	Pptr p = e1->e1->tp?e1->e1->tp->is_ptr():0;
			if (p && p->typ->tconst())
				error("non-constMF%n called for constO",fn);
			break;
		}
		case DOT:
		{
			int tc = e1->e1->tp ? e1->e1->tp->tconst() : 0;
			if (e1->tp && tc && (!strict_opt || tc!=2))
				error("non-constMF%n called for constO",fn);
			break;
		}
		case MEMPTR:
		{
			Pptr p = e1->e1->tp?e1->e1->tp->is_ptr():0;
			if (p && p->typ->tconst())
				error("non-constMF called for constO");
			break;
		}
		}
	}

	t = f->returns;
	x = f->nargs;
	k = f->nargs_known;

	e = arg;
	if (k == 0) goto rlab;

	for (nn=f->argtype, argno=1; e||nn; nn=nn->n_list, e=etail->e2, argno++) {
		Pexpr a;
		int save_base = 0;
		char* save_name = 0;
		bit mpt = (nn && nn->tp && nn->tp->skiptypedefs()->memptr());

		if (e) {
			a = e->e1;
			etail = e;

			if (nn) {	/* type check */
				Ptype t1 = nn->tp->skiptypedefs();
//error('d',"argtp %t etp %t a %k",t1,a->tp,a->base);

				switch (t1->base) {
				case RPTR:
				{	
					Ptype pt = Pptr(t1)->typ;
					if (pt->skiptypedefs()->base==VEC ) {
					   if (pt->check(a->tp,IGNORE_CONST)) {
						error("badA %dT for%n:%t (%tX)",argno,fn,a->tp,nn->tp);
						return any_type;
					   };
					}
					if (pt->base != FCT 
					    ||
					    pt->check(a->tp,0)
					) {
					    is_arg = 1;
					    a = ref_init(Pptr(nn->tp),a,tbl);
					    is_arg = 0;
					}
					goto cbcb;
				}
				case COBJ:
					if (a->base!=G_CM
					|| nn->tp->check(a->tp,ASSIGN))
						a = class_init(0,t1,a,tbl);
					else if (a->e2->tp!=nn->tp)
						a->e2=class_init(0,t1,a->e2,tbl);
					if (nn->n_xref) {
						// (temp.ctor(arg),&arg)
						a = a->address();
					}
					else {
						// defend against:
						//	int f(X); ... X(X&);
						Pname cln = Pbase(t1)->b_name;	
						if (cln && Pclass(cln->tp)->has_itor()) {
							// mark X(X&) arguments
							nn->n_xref = 1;
							a = a->address();
						}
					}
	cbcb:
//error('d',"cbcb: a %d %k %t",a->base,a->base,a->tp);
		if (a->base==G_CM) {
			if (a->e1->base==DEREF) a->e1 = a->e1->e2; // (*e1,e2) => (e1,e2)
			if (a->e1->base==G_CALL
			&& Pname(a->e1->fct_name)
			&& Pname(a->e1->fct_name)->n_oper==CTOR
			&& (a->e2->base==G_ADDROF || a->e2->base==ADDROF)) {
				a = a->e1;	// (ctor(&tmp),&tmp) => ctor(&tmp)
				goto cccc;
			}
			else if (a->e2->base==G_ADDROF
			&& a->e2->e2->base==NAME)  {
			cccc:
			    if (t1->base==RPTR && Pptr(t1)->typ->tconst()==0) {
				if (strict_opt)
				    error("temporary used for non-const%tA",nn->tp);
				else
				    error('w',"temporary used for non-const%tA; no changes will be propagated to actual argument (anachronism)",nn->tp);
			    }
			}
		}
					e->e1 = a;
					break;
				case ANY:
					goto rlab;
				case PTR:
				{
					save_base = e->e1->base;
					if(a->tp->base==OVERLOAD)
						save_name = Pgen(a->tp)->fct_list->f->string;
					Pexpr te_a = a;
					e->e1 = a = ptr_init(Pptr(t1),a,tbl);
					no_change = (te_a == a);
					if (Pchecked == 0) goto def;
					break;
				}
				case CHAR:
				case SHORT:
				case INT:
				{	Ptype t = a->tp->skiptypedefs();
					switch (t->base) {
					case LONG:
					case FLOAT:
					case DOUBLE:
					case LDOUBLE:
					    if(!ambig)
						error('w',"A%d: %t passed as %t",argno,a->tp,t1);
					}
				}
					// no break
				case LONG:
					if (Pbase(t1)->b_unsigned
					&& a->base==UMINUS
					&& a->e2->base==ICON)
						error('w',"negativeA for%n, unsignedX",fn);
				default:
				def:
				{	Pexpr x = try_to_coerce(t1,a,"argument",tbl);
					int ct = no_const;
//error('d',"x %d t1 %t nn %t a1 %t",x,t1,nn->tp,a->tp);
					if (x) {
 						if (t1->is_ptr() && Pchecked == 0 && no_change) {
							Pexpr te_x = ptr_init(Pptr(t1), x, tbl);

							if ( te_x != x ) e->e1 = a = te_x; else e->e1=x;
						}
						else
							e->e1 = x;
					}
					else if (nn->tp->check(a->tp,ARG)) {
						error('e',"badA %dT for%n:%t (%tX)",argno,fn,a->tp,nn->tp);
						if (ct) error('c'," (no usable const conversion)\n");
						else error('c',"\n");
						return any_type;
					}
				}
				}

                                Pexpr tt = e->e1;
                                while ( tt->base == CAST || tt->base == G_CAST )
                                        tt = tt->e1;
                                if ( tt->base == ILIST )
                                        e->e1 = tt;

                                if (e->e1->base == ILIST) {
                                        // memptr constant
                                        // f({1,2,f}) ==> memptr t; f((t={1,2,f},t))
					if(save_base == REF) {
						Pptr m = Pptr(a->tp);
						error(
						"address of boundF (try using ``%s::*'' forPT and ``&%s::%s'' for address)",
						m->memof->string,
						m->memof->string,
						save_name
						);
					}
					if (mpt) {
	                                        Pname temp = make_tmp('A',mptr_type,tbl);
       	                                	e->e1 = mptr_assign(temp,e->e1);
                                        	e->e1 = a = new expr(G_CM,e->e1,temp);
                                        	a->tp = temp->tp;
					}
					else {
						Ptype t = e->e1->e1->e1->tp;
						e->e1 = e->e1->e1->e1;
						a->tp = e->e1->tp = t;
					}
                                }

			}
			else {
				if (k != ELLIPSIS) {
					error("unexpected %dA for%n",argno,fn);
					return any_type;
				}
				if (!a->tp->skiptypedefs()->check(void_type, 0))
					error("badA %dT for%n:%t",argno,fn,a->tp);
				Pexpr te=e;
				while(e) {
					if (e->tp)
						e->tp->tsizeof();
                                	if (e->e1->base == ILIST) {
                                        	// memptr constant
                                        	// f({1,2,f}) ==> memptr t; f((t={1,2,f},t))
                                        	Pname temp = make_tmp('A',mptr_type,tbl);
                                        	e->e1 = mptr_assign(temp,e->e1);
                                        	e->e1 = a = new expr(G_CM,e->e1,temp);
                                        	a->tp = temp->tp;
                                	}
					e = e->e2;
				}
				e = te;
				goto rlab;
			}
		}
		else {	/* default argument? */
			a = nn->n_initializer;
			if (a == 0) {
				if (fn == 0)
				    error("A %d ofT%tX for call",argno,nn->tp);
				else
				    error("A %d ofT%tX for%n",argno,nn->tp,fn);
				return any_type;
			}
                        if (a->base == ILIST ||
			    ((a->base == CAST || a->base == G_CAST) && a->e1->base == ILIST)) {
                                // memptr constant
                                // f({1,2,f}) ==> memptr t; f((t={1,2,f},t))
                                Pname temp = make_tmp('A',mptr_type,tbl);
				if (a->base != ILIST)
					a = a->e1;
                                a = mptr_assign(temp,a);
                                a = new expr(G_CM,a,temp);
                                a->tp = temp->tp;
                        }
			a->permanent = 2;	// ought not be necessary, but it is
			e = new expr(ELIST,a,0);
			if (etail)
				etail->e2 = e;
			else
				e2 = e;
			etail = e;
		}
	}

rlab:
//error('d',"rlab fct_name %n %t",fct_name,fct_name?fct_name->tp:0);
	for (; e; e = e->e2) {	// unchecked arguments
		Pexpr a = e->e1;
		Pname cn;

		if (a->base==NAME && a->tp->base==FCT) {
			// function name that escaped the type system:
			// update use count
			a->lval(ADDROF);
		}
		else if (warning_opt && (cn = a->tp->is_cl_obj())) {
			Pclass cl = Pclass(cn->tp);
			if (cl->has_ctor() || cl->memtbl->look("__as",0))//cl->has_oper(ASSIGN)
			{
				if (fct_name)
				error('w',"O ofC%t withK or = copied asA to%n (%t)",cl,fct_name,fct_name->tp);
				else
				error('w',"O ofC%t withK or = copied asA to `...'",cl);
			}
		}
		else if (a->tp->is_ref())
			e->e1 = a->contents();
	}

	if (f->f_result) {		// f(args) => (f(&temp,args),temp)
		Pname oldNtmp = Ntmp; 
		Ntmp = 0;  // set in make_tmp if tn has associated dtor
		Pname tn = make_tmp('R',f->returns,tbl);

		extern bit in_quest;
		if (Ntmp) {
                	if (Ntmp_refd && in_quest) {
			 	tn->n_list = Ntmp_refd;	
                        	Ntmp_refd = tn;
			}
                	else Ntmp_refd = tn;
		}

		// better might be: if (Ntmp == 0 && oldNtmp)
		// but this mimics the old behavior ...
                if (oldNtmp) Ntmp = oldNtmp;

                if (Ntmp_refd && in_quest) {
                        Ntmp_flag = make_tmp('Q',int_type,tbl);
                        Ntmp_flag->n_initializer = new ival(0L);
                        Ntmp_flag->assign();
                        if (Ntmp_flag_list && in_quest) {
				Ntmp_flag->n_list = Ntmp_flag_list;
                                Ntmp_flag_list = Ntmp_flag;
			}
                        else Ntmp_flag_list = Ntmp_flag;
                }

		e2 = new expr(ELIST,tn->address(),e2);
		Pexpr ee = new expr(0,0,0);
		*ee = *this;
		base = G_CM;		// (f(&temp,args),temp)
		e1 = ee;
		if (refd == 2)
			e2 = tn->address();
		else e2 = tn;
		tp = tn->tp;
	}

	return t;
}

Pexpr ref_init(Pptr p, Pexpr init, Ptable tbl)
/*
	initialize the "p" with the "init"
	remember to call ptr_init to ensure that pointers to second bases
	are handled correctly.
*/
{
	register Ptype it = init->tp->skiptypedefs();
	Pptr px = Pptr(p->skiptypedefs());
	Ptype p1 = px->typ;
	Pname c1 = p1->is_cl_obj();
// error('d',"ref_init: p %t, p1 %t, px %t, init->tp %t",p,p1,px,it);
// error('d', "ref_init: nof: %n f_const: %d", cc?cc->nof:0, cc?(cc->nof?Pfct(cc->nof->tp)->f_const:0):0);

	if (init->base == ILIST) error("IrL as RIr");

	if (init->base==NAME
	&& Pname(init)->n_scope==ARG
	&& init->tp->base==FLOAT)
		error('w',"initializing a float& with floatA is non-portable");

	Ptype tt = it->addrof();
	px->base = PTR;	// allow &x for y& when y : public x
			// but not &char for int&
	int x = px->check(tt,COERCE);

	if (x == 0) {	//CCC type is fine check for constness:
		if (init->tp->tconst()
		    && vec_const==0
		    && fct_const==0) {
			// not ``it''
			if (init->base == ELIST) init = init->e1;
			if (px->typ->tconst() == 0) 
				if (cc->nof && in_return)
					error("cannot return a reference to a non-constO from const member function %n",cc->nof); 
				else error("R to constO");
			px->base = RPTR;
			// if we have a const lvalue we can still pass its address
			ignore_const++;
			if (init->lval(0)) {
				init->lval(ADDROF); // force output
				ignore_const--;
//error('d',"in1 %t",init->tp);
				return ptr_init(px,init->address(),tbl);//return init->address();
			}
			ignore_const--;
			goto xxx;
		}
		px->base = RPTR;
                if (init->lval(0)) {	// can pass the address							// no temporary needed 
			init->lval(ADDROF); // force output
//error('d',"px %t init %t ",px,init->tp);
                        {  Pname name_in_deref =0;
                           Pexpr act_param=0, ret_exp=0;
                           if ((init->base == DEREF) &&
                               (init->e1 && (init->e1->base == NAME))
                              ) {
                              name_in_deref = Pname(init->e1);
                           };

                           if (name_in_deref		&&
                               (name_in_deref->n_xref   	&&
                                name_in_deref->n_scope == ARG)
                              ) {

				//"n_xref" is set when a formal class object
				//is treated as a pointer object, in Print phase

				//let us make it as a pointer expression, so
				//ptr_init logic works correctly (generates
                                //valid MDOT expression when necessary).
                                //But have to watch out while printing "mem"
                                //of MDOT expression in expr::print (print.c).

			      if (init->e1->tp) {
                              act_param = new expr (G_ADDROF, 0, init->e1);
                              act_param->tp  = init->e1->tp->addrof(); 
//error('d',"px %t act_param %t ",px,act_param->tp);
                              ret_exp = ptr_init(px, act_param, tbl);
		  	      } else {
			   	error ("i", "No type for actual param %n",
				 	name_in_deref);
                              }
                           } else {
                           ret_exp = ptr_init(px, init->address(),tbl);
			   };
                           return ret_exp;
                        };
		}
		goto xxx;
	}

	px->base = RPTR;

//error('d',"c1 %n",c1);

	if (c1) {	// assigning to a const X & is fine
		ref_cast++;
		Pexpr x = try_to_coerce(p,init,"reference initialization",tbl);
		ref_cast--;
		if (x) {
			init = x;
			goto xxx;
		}
		if (init->tp->tconst() && !vec_const && !p1->tconst()) {
			error("R to constO");
			return init;
		}
		switch ( init->base ) {
			case STRING: case ZERO: case CCON:
			case ICON: case FCON: case IVAL:
			case NAME:
				refd = 1; 
				break;
			default:
				refd = (init->e1 && init->e1->base == NAME && 
					init->e1->tp->base != RPTR &&
					Pname(init->e1)->n_xref == 0) ? 2: 1;
				break;
		}
// error('d', "***** refd: %d", refd );
		Pexpr a = class_init(0,p1,init,tbl);
		refd = 0;
		if (a==init && init->tp!=any_type) goto xxx;
// error('d',"ri a %d %k",a->base,a->base);
		switch (a->base) {
		case G_CALL:
			init = a;
			goto xxx;
		}
		switch (init->base) {
		case CM:
		case G_CM:
			break;
		case NAME:
		case DEREF:
		case REF:
		case DOT:
			if ((it->tconst()==0 || vec_const)
		    	    &&
		    	    (fct_const==0 || p1->is_ptr()==0)
			) 
				break;
		default:
			if (p1 && p1->b_const==0) {
		    	    if (tbl == gtbl || (strict_opt && !is_arg)) 
				error("Ir for%snon-constR not an lvalue", strict_opt?"":" global ");
		    	    else if(!is_arg)
				error('w',"Ir for non-constR not an lvalue (anachronism)");
			}
		}
		a = a->address();
		a =  ptr_init(px,a,tbl);
		return a;
	}

//error('d',"p1 %t it %t",p1,it);
	if (p1->check(it,0)) {

		if (p1->check(it,ASSIGN) == 0) {
			goto def;
		}

		Pexpr x = try_to_coerce(p1,init,"reference",tbl);     // x==init
		if (x==0) 
			x = try_to_coerce(px,init,"reference",tbl); // x&=init
		if (x) {
			init = x;
			goto def;
		}
		int nc = no_const;

		Pptr p1_ptr = p1->is_ptr();

		if (p1_ptr && p1_ptr->typ->skiptypedefs()->base == FCT
		    &&
		    it->is_or_pts_to(OVERLOAD)
		) {
			Pexpr op = ptof(
				Pfct(p1_ptr->typ->skiptypedefs()),
				init,
				tbl
			);
			if(op) {init = op; goto def; }
		}

		error('e',"badIrT:%t (%tX)",it,p);
		if (nc) error('c'," (no usable const conversion)\n");
		else error('c',"\n");
		if (init->base != NAME) init->tp = any_type;
		return init;
	}
	
xxx:	/*
		here comes the test of a ``fundamental theorem'':
		a structure valued expression is
			(1) an lvalue of type T (possibly const)
		or	(2) the result of a function (a _result if X(X&) is defined)
		or	(3) a * or [] or ? or , expression
	*/
//error('d',"xxx %k %d %t",init->base,init->base,init->tp);

	switch (init->base) {
	case NAME:
	case DEREF:
	case REF:
	case DOT:			// init => &init
		{
			bit it_isconst = it->tconst();
			bit vec_const_save = vec_const;
			bit fct_const_save = fct_const;

			if (	(init->base==NAME && Pname(init)->n_stclass==ENUM)
				||
				it_isconst
				&&
				Pbase(p->typ) && !Pbase(p->typ)->tconst()
				&&
				vec_const_save==0 && fct_const_save==0
			) {
				goto def;
			}
			init->lval(ADDROF);

			if (vec_const_save) return init;
			if (fct_const_save && p1->is_ptr()) goto def;	// fptr& = fct
		}
		// no break
	case CM:
	case G_CM:			// & (f(&temp), temp)
		return ptr_init(px,init->address(),tbl);//init->address();
	default:
	def:
	{
// error('d',"def: init->tp %t p1 %t ",init->tp,p1);	
		if (p1 && p1->b_const==0) {
			if (tbl == gtbl || (strict_opt && !is_arg)) 
				error("Ir for%snon-constR not an lvalue", strict_opt?"":" global ");
			else if(!is_arg)
				error('w',"Ir for non-constR not an lvalue (anachronism)");
		}

                Pname tcl = p1->is_cl_obj ();
                if(tcl && Pclass(tcl->tp)->c_abstract) {
                       error("a temporary is needed for a parameter, but the AT is abstractC %t.", tcl->tp);
		       error('C',"%a is a pure virtualF ofC%t",Pclass(tcl->tp)->c_abstract,tcl->tp);
		}

		no_sti=1;
		Pname n = make_tmp('I', unconst_type(p1), tbl);
		no_sti=0;
		n->assign();
		if (tbl == gtbl) n->dcl_print(0);	// a hack
		Pexpr a = 0;
		Pname ic = init->tp->is_cl_obj();

		switch (p1->base) {
		case INT:
		case CHAR:
		case SHORT:
			switch (it->base) {
			case LONG:
			case FLOAT:
			case DOUBLE:
			case LDOUBLE:
				error('w',"%t assigned to %t inRIr",it,p1);
			}
		}

		if (ic && c1 && ic!=c1 && !same_class(Pclass(ic->tp),Pclass(c1->tp))) {
			// derived class1 => must cast: ``it Ix; (Ix=init,(p)&Ix);''
			n->tp = init->tp;
			a = ptr_init(px,n->address(),tbl);//n->address();
			PERM(p);
			a = new texpr(G_CAST,p,a);
			a->tp = p;
		} else if (!ic && !c1 && init->tp->is_ptr() && p1->is_ptr()) {
			Pname icx = (Pptr(init->tp->skiptypedefs()))->typ->is_cl_obj();
			Pname c1x = (Pptr(p1->skiptypedefs()))->typ->is_cl_obj();
			if (icx && c1x && icx!=c1x && !same_class(Pclass(icx->tp),Pclass(c1x->tp))) {
				// Base*const& = Derived*
				init = ptr_init(Pptr(p1->skiptypedefs()),init,tbl);
			}
		}
		if (!a)
			a = n->address();

		refd = 1;
		Pexpr as;
		if (init->tp->memptr()
		    ||
		    init->tp->base == FCT && Pfct(init->tp)->memof
		) {
			Ptype pit = p->typ->skiptypedefs();
			as = mptr_assign(n,ptof(Pfct(pit),init,tbl));
		}
		else {
			as = init_tmp(n,init,tbl);
		}
		refd = 0;
		a = new expr(G_CM,as,a);
		a->tp = a->e2->tp;
		return a;
	}
	}
}

Pexpr class_init(Pexpr nn, Ptype tt, Pexpr init, Ptable tbl)
/*
	initialize "nn" of type "tt" with "init"
	if nn==0 make a temporary,
	nn may not be a name
*/
{	
	if (init == dummy) return 0;
//error('d',"class_init %t with %t init %k refd %d",tt,init->tp,init->base,refd);
	Pname c1 = tt->is_cl_obj();

        if (init == 0) {
                error("emptyIr");
                return dummy;
        }

	if (c1) {
		Pclass cl = Pclass(c1->tp);
		Pname c2 = init->tp->is_cl_obj();


		if ((c1!=c2 && (c2 == 0 || same_class(cl,Pclass(c2->tp),1) == 0))
		     || (refd==0 && cl->has_itor())) 
		{ // really ought to make a temp if refd, 
		  // but ref_init can do that

			int i = can_coerce(tt,init->tp);
//error('d',"i %d nn %n",i,nn);
			switch (i) {
			default:
				error("%d ways of making a%n from a%t",i,c1,init->tp);
				init->tp = any_type;
				return init;
			case 0:
				if (c2 && Pclass(c2->tp)->has_base(cl)) {
					init = init->address();
					Pexpr x = cast_cptr(cl,init,tbl,0);

					if (x == init) {
						Ptype pt = tt->addrof();
						PERM(pt);
						x = new cast(pt,init);
					}

					return x->contents();
				}
				error("cannot make a%t from a%t",cl,init->tp);
				init->tp = any_type;
				return init;
			case 1:
//error('d',"ncoerce %n %k %d",Ncoerce,init->base,init->base);
				if (Ncoerce == 0) {
					Pexpr a = new expr(ELIST,init,0);
					a = new texpr(VALUE,tt->skiptypedefs(),a);
					a->e2 = nn;
					a = a->typ(tbl);
//error('d',"ci a %k %d %t",a->base,a->base,a->tp);
					return a;
				}

				switch (init->base) {
				case CM:
				case G_CM:	//ddd
				case NAME:	/* init.coerce() */
	/* *ref */		case DEREF:
				{
					Pref r = new ref(DOT,init,new name(Ncoerce->string));
					Pexpr rr = r->typ(tbl);
					init = new expr(G_CALL,rr,0);
					break;
				}
				default:	// (temp=init,temp.coerce())
				{	Pname tmp = make_tmp('U',init->tp,tbl); 
					int x = refd;	
					refd = 0;	// ??
					Pexpr ass = init_tmp(tmp,init,tbl);
					refd = x;
					Pref r = new ref(DOT,tmp,new name(Ncoerce->string));
					Pexpr rr = r->typ(tbl);
					Pexpr c = new expr(G_CALL,rr,0);
					c = c->typ(tbl);
					init = new expr(CM,ass,c);
					init->tp = c->tp;
					if (refd) {	// &f() => (t=f(), &t)
						Pname tmp2 = make_tmp('L',c->tp,tbl); 
						ass = init_tmp(tmp2,init,tbl);
						init = new expr(G_CM,ass,tmp2);
					}
				}
				}
//error('d',"nn %n",nn);
				if (nn) {
					Pexpr a = new expr(ELIST,init,0);
					a = new texpr(VALUE,tt->skiptypedefs(),a);
					a->e2 = nn;
					return a->typ(tbl);
				}
			}
//error('d',"c1 %n c2 %n",c1,c2);
			return init->typ(tbl);
		}
		return init;
	}
//error('d',"ci check tt %t init->tp %t",tt,init->tp);
	if (tt->check(init->tp,ASSIGN) && refd==0) {
		error("badIrT:%t (%tX)",init->tp,tt);
		init->tp = any_type;
	}

	return init;
}

extern int bound;	// fudge for bound pointers to functions

Pexpr expr::docast(Ptable tbl)
{	
	// check cast against value, INCOMPLETE

//error('d',"docast %d %t %k",this,tp2,e1->base);
	if (e1 == dummy) {
		error("E missing for cast");
		tp = any_type;
		return this;
	}

	int pmf = 0;
	int ptom_cast = 0;
	int noconst = 0;
	Pexpr ee = e1;

//error('d',"ee %k %d",ee->base,ee->base);
	switch (ee->base) {
	case ADDROF:
		ee = ee->e2;
		switch (ee->base) {
		case NAME:	goto nm;
		case REF:	goto rf;
		}
		break;

	case NAME:
	nm:
		if (Pname(ee)->n_qualifier) pmf = 1;
		break;
		
	case REF:
	rf:
		if (ee->e1->base == THIS) bound = 1;
		break;
	}

	e1 = e1->typ(tbl);

	int b = bound;	// distinguish between explicit and implicit THIS
	bound = 0;
	//pmf = pmf && e1->base==CAST;
	pmf = pmf && ((e1->base==CAST || e1->base==G_CAST) || e1->base==ILIST);

	Ptype etp = e1->tp->skiptypedefs();
	Ptype tt = tp2;
	Ptype t = tt;
	tt->dcl(tbl);

	tt = tt->skiptypedefs();
	
//error('d',"e1 %k etp %t tt %t",e1->base,etp,tt);
	bit isptm = 0;
	switch (etp->base) {
	case PTR:
		if (!etp->memptr())
			break;
		if (etp->memptr()->f_static)
			break;
		isptm = 1;
		break;
	case FCT:
		if (!Pfct(etp)->memof)
			break;
		if (Pfct(etp)->f_static)
			break;
		isptm = 1;
		break;
	case ILIST:
		isptm = 1;
		break;
	}
	if (isptm && !tt->memptr() && tt->base != VOID)
		error(strict_opt ? 0 : 'w', "cast to%t ofP toM (anachronism)", tt);

	switch (etp->base) {
	case PTR:
	case RPTR:
		if (Pptr(etp)->typ->base == OVERLOAD) goto over;
		if (Pptr(etp)->typ->base == FCT && Pfct(Pptr(etp)->typ)->fct_base)
			goto over;
		if (warning_opt && i2==0 && Pptr(etp)->typ->tconst()) {
			switch (tt->base) {
			case FCT:
				break;
			case PTR:
			case RPTR:
				if (Pptr(tt)->typ->tconst()) break;
			default:
				// casting away const
				// should be an error
				// but ANSI says OK so I chicken out
				// to be able to compile strtok(), etc.
				error('w',"const cast away:%t->%t",e1->tp,tp2);
			}
		}
		else
			i2 = 0; // to allow cfront to escape its own checking
		break;
	case COBJ:
	{	ref_cast = 1;
		Pexpr x = try_to_coerce(tt,e1,"cast",tbl);
		if (!x && tt->base == EOBJ)
			x = try_to_coerce(int_type, e1, "cast", tbl);
		noconst = no_const;
		ref_cast = 0;
//error('d',"x %k %t tt %d %t",x?x->base:0,x?x->tp:0,tt,tt);
		if (x) {
			if (x!=e1 && x->base==DEREF && tt->is_ref()) x = x->e1;
			if (tt==x->tp || tt->check(x->tp,0)==0 || const_problem)
				return x;
			else
				return new cast(tt,x);
		}
		break;
	}
	case VOID:
		if (tt->base == VOID) {
			tp = t;
			return this;
		}
		error("cast of void value");
		// no break;
	case ANY:
	any:
		tp = any_type;
		return this;
	case FCT:
		if (tt->base == PTR && Pptr(tt)->typ->base != FCT)
			error('w',"P toF cast toP to nonF");
		if (Pfct(etp)->fct_base)
			goto over;
		break;
	case OVERLOAD:
	over:
		error("cast of overloaded");
		goto any;
	}

//error('d',"tt %t",tt);
	switch (tt->base) {
	case VEC:
		error("cast to arrayT %t", tt);
		break;
	case PTR:
		if (tt->memptr()) {
			if (etp->memptr() == 0
			    &&
			    (etp->base!=FCT || Pfct(etp)->memof == 0)
			) {
				if (etp->base == ZTYPE) {
					e1 = new expr(ELIST,zero,zero);
					e1 = new expr(ILIST,e1,zero);
					e1->tp = tt;
					return e1;
				}
				error("cast toP toM %t",tt);
			}

			else {	// adjust delta in MI case
				// for the moment just suppress the cast
				// all pmfs are the same to cc
				ptom_cast = 1;
				tp2 = mptr_type;
			}
		}

		switch (etp->base) {
		case COBJ:
			error('e',"cannot castCO toP");
			if (noconst) error('c'," (no usable const conversion)\n");
			else error('c',"\n");
			break;
		case FCT:
			e1 = new expr(G_ADDROF,0,e1);
			bound = b;
			e1 = e1->typ(tbl);
			bound = 0;
			if (e1->base == CAST || e1->base == G_CAST)
				pmf = 1;
			else
				break;
			// no break;

		case RPTR:
		case PTR:
		{	Pname cn = Pptr(tt)->typ->is_cl_obj();
			if (cn) {
				Pexpr x = cast_cptr(Pclass(cn->tp),e1,tbl,base==CAST?1:0);

				if (x == e1) {
					PERM(tt);
					e1 = new cast(tt,e1);
					e1->i2 = i2;
				}
				else
					e1 = x;
			}
			if (pmf) {
				tt = tt->skiptypedefs();

				switch (tt->base) {
				case PTR:
					if (Pptr(tt)->memof) break;
				default:
					error("%t cast to%t (%t is not aP toM)",e1->tp,tp2,tp2);
				}
			}
		}
		}
		break;

	case RPTR:		// (x&)e: pretend e is an x
	{	Ptype er = etp;
		Ptype cr = tt;
		do {
			if (er = er->is_ptr_or_ref()) er = Pptr(er)->typ;
			if (cr = cr->is_ptr_or_ref()) cr = Pptr(cr)->typ;
		} while (er && cr);
		int pp = er!=0;	//	if `e' is a suitable pointer cast it:
				// 	(x&)e => (x*)e, otherwise
				//	(x&)e => *(x*)&e
// error('d',"rptr tt %t e1->base %k e1->tp %t",tt,e1->base,e1->tp);
		if (e1->base==G_CM
		|| e1->base==CALL
		|| e1->base==G_CALL
		|| e1->lval(0))
			;
		else if (e1->tp->tconst()) {
				// casting away const
				// should be an error
				// but ANSI says OK so I chicken out
				// to be able to compile strtok(), etc.
			if (warning_opt && Pptr(tt)->typ->tconst()==0)
				error('w',"const cast away:%t->%t",e1->tp,tp2);

		}
		else
			error("cannot cast%t to%t",etp,t);
//error('d',"e1 %k %t %d",e1->base,e1->tp,pp);
		if (pp == 0) e1 = e1->address();	// *(x*)&e
		tp = t;

		// do proper pointer manipulation for multiple inheritance
		Pname cn = Pptr(tt)->typ->is_cl_obj();
		if (cn) {
			Pexpr x = cast_cptr(Pclass(cn->tp),e1,tbl,base==CAST?1:0);

			if (x == e1) {
				PERM(tt);
				e1 = new cast(tt,e1);
				e1->i2 = i2;
			}
			else
				e1 = x;
		}

		return pp ? this : contents();
	}
	case COBJ:
		base = VALUE;	// (x)e => x(e): construct an x from e
		e1 = new expr(ELIST,e1,0);
		return typ(tbl);

	case CHAR:
	case INT:
	case SHORT:
	case LONG:
	case EOBJ:
		switch (etp->base) {
		case FCT:
			e1 = new expr(ADDROF,0,e1);
			e1 = e1->typ(tbl);
		case PTR:
			if(!e1->tp->memptr() && e1->tp->tsizeof()>tt->tsizeof())
				error("type ``%t'' not large enough for values of ``%t ''",tt,etp);
			break;
		case COBJ:
			error('e',"cannot castCO to%k",tt->base);
			if (noconst) error('c'," (no usable const conversion)\n");
			else error('c',"\n");
			break;
		}	
		break;

	case FLOAT:
	case DOUBLE:
	case LDOUBLE:
		switch (etp->base) {
		case FLOAT:
		case DOUBLE:
		case LDOUBLE:
		case CHAR:
		case INT:
		case SHORT:
		case LONG:
		case EOBJ:
		case ZTYPE:
			break;
		default:
			error("cannot cast ``%t '' to ``%t''",etp,tt);
			break;
		}	
		break;

	case FCT:
		error("cannot cast toFT");
		break;
	}

	tp = t;

	if (e1->base==ILIST && ptom_cast==0) { // ptm constant
		Pexpr ee = e1->e1;	// ELIST
		int i;
		switch (ee->e2->base) {
		case IVAL:
			i = int(ee->e2->i1);
			break;
		case ZERO:
			i = 0;
		}

		if (i<0)
			e1 = e1->e2;	// just the function
		else
			e1 = ee->e2;	// just the index
		return this;
	}

	if (etp->memptr()) {
		Pclass cl = Pptr(etp)->memof;

		if (Pptr(tt)->memof==0 && b == 0 ) {
			Pexpr y;
			if((e1->base == G_CAST || e1->base == CAST) && 
			    e1->e1->base == ILIST) {
				e1 = e1->e1;
				y = e1->e2;
			}
			else  {
				y = new mdot("f",e1);
				y->i1 = 9;
			}
			y = new cast(tt,y);
			if (cl->virt_count && b==0) {
				// ERROR: no check for side effects
				Pexpr z = new mdot("i",e1);
				Pexpr x = new mdot("i",e1);
				x->i1 = 9;
				x = new cast(tt,x);
				z->i1 = 9;
				Pexpr q = new expr (QUEST,x,y);
				q->cond = new expr(LE,zero,z);
				q->tp = tt;
				delete this;
				return q;
			}
			delete this;
			return y;
		}
	}

	return this;
}

Pexpr expr::dovalue(Ptable tbl)
{
	Ptype tt = tp2;
	Pclass cl;
	Pname cn;

// error('d',"value %d %t e1 %d e2 %d",tt,tt,e1,e2);
	
	tt->dcl(tbl);

	tt = tt->skiptypedefs();

	switch (tt->base) {
	case EOBJ:
	default:
		if (e1 == 0) {
			//error("value missing in conversion to%t",tt);
			e1 = zero;
		} else { // convert elist to expr 
			if ( e1->e2 == 0 ) {
				e1 = e1->e1;
                                if (e1->base==NAME && e1->permanent == 0)
                                        PERM(e1);
			}
			else { // int( x, y )
				error("more than oneA for basicTK");
				for ( Pexpr e = e1;  e->e2->e2;  e = e->e2 )
					e->base = CM;
				e->base = CM;
				e->e2 = e->e2->e1;
			}
		}
		base = G_CAST;
		return typ(tbl);

	case CLASS:
		cl = Pclass(tt);
		if (cl->this_type)
		tp2 = Pptr(cl->this_type)->typ;
		break;

	case COBJ:
		cn = Pbase(tt)->b_name;
		cl = Pclass(cn->tp);
	}

	Pname ctor = cl->has_ctor();

//error('d',"e1 %k e1->e2 %k",e1->base,e1?e1->e2->base:0);
	if (e1 && e1->e2==0) {		// single argument
		if (e1->e1->base==ELIST) e1->e1 = e1->e1->e1;	// spurious elist
		e1->e1 = e1->e1->typ(tbl);
		if (tt->base==COBJ) {
			Pexpr x = try_to_coerce(tt,e1->e1,"type conversion",tbl);
			if (cl->has_itor()==0) {
				if (x)
					return x;
			}
			else {
				if (x && x->base != DEREF)
					return x;
			}
		}

		Pname acn = e1->e1->tp->is_cl_obj();
//error('d',"acn %n %d",acn,cl->has_itor());
		if (acn && cl->has_itor()==0) {
			Pclass acl = Pclass(acn->tp);
			int hb = acl->has_base(cl);

			// special case handling: Base(Derived);
			if (ctor && hb) {
				Pname n = 0;
				switch (ctor->tp->base) {
				case OVERLOAD:
   					n = Pgen(ctor->tp)->exactMatch(e1,0);
   					if (n) ctor = n;
   					break;
				case FCT: // improbable ever be traversed
					{
   					Pfct f = Pfct(ctor->tp);
   					if ( f->nargs != 1 ) break;
   					n = exact1(f->argtype,e1->e1->tp) ? ctor : 0;
   					break;
					}
				}
				if (n) goto mk_ctor_call;
			}

			if (same_class(acl,cl) || hb) {
				vcllist->clear();
				vcllist=0;
				if (1<is_unique_base(acl,cl->string,0)) error("ambiguous assignment to base %t",cl);
				Pexpr ee = e1->e1;
				if (ee->base == ELIST) ee = ee->e1;	// ???
				if (hb) {	// ee => *(tp2*)&ee
						// remember = may be overloaded
//error('d',"hb %k %t %d",ee->base,ee->tp,ee->lval(0));
					ignore_const++;
					if (ee->lval(0)==0) {
						Pname tmp = make_tmp('T',ee->tp,tbl);
						ee = init_tmp(tmp,ee,tbl);
						ee = new expr(G_CM,ee,tmp->address());
					}
					else
						ee = ee->address();
					ignore_const--;
					ee = new texpr(G_CAST,new ptr(PTR,tp2),ee); //new cast(new ptr(PTR,tp2),ee);
					ee = ee->contents();
					ee->typ(tbl);
				}

				if (e2) {	// x(x_obj) => e2=x_obj
					base = ASSIGN;
					e1 = e2;
					e2 = ee;
					tp = tp2;
					return this;
				}
				return ee;	// strip ELIST: x(x_obj) => x_obj
			}
		}
	}


	/* x(a) => obj.ctor(a); where e1==obj */
	if (ctor == 0) {
		error("cannot make a%t",cl);
		return dummy;
	}

// error('d',"e2 %k",e2?e2->base:0);
mk_ctor_call: // beats duplicating the code

	if (e2 == 0) {		// x(a) => x temp; (temp.x(a),temp)
		if (e1 && e1->e1 && !e1->e2) {
			char* s = e1->e1->string;
			if (s && s[0] == '_' && s[1] == '_' && s[2] == 'V' &&
			    e1->e1->tp && tp2 && !e1->e1->tp->check(tp2, 0))
				return e1;
		}
		no_sti = 1;
		Ntmp = 0;
		Pname n = make_tmp('V',tp2,tbl);
		no_sti = 0;
		n->assign();
		if (tbl == gtbl) n->dcl_print(0);	// a hack
		Pexpr c = call_ctor(tbl,n,ctor,e1,DOT);
		extern bit in_quest;
		if (Ntmp && in_quest) {
                	if (Ntmp_refd) {
			 	n->n_list = Ntmp_refd;	
                        	Ntmp_refd = n;
			}
                	else Ntmp_refd = n;

  			Ntmp_flag = make_tmp('Q',int_type,tbl);
  			Ntmp_flag->n_initializer = new ival(0L);
  			Ntmp_flag->assign();

                        if (Ntmp_flag_list) {
				Ntmp_flag->n_list = Ntmp_flag_list;
                                Ntmp_flag_list = Ntmp_flag;
			}
                        else Ntmp_flag_list = Ntmp_flag;

			Pexpr e = new expr(ASSIGN,Ntmp_flag,one);
			e->tp = int_type;
			Ptype t = c->tp;
			Ntmp_flag = 0;
			c = new expr(G_CM,e,c);
			c->tp = t;
		}

		c = new expr(G_CM,c,n);
		c->tp = n->tp;
		return c;
	}
	else {
		Pexpr c = call_ctor(tbl,e2,ctor,e1,DOT);
		c = new expr(DEREF,c,0); // deref value returned by constructor
		c->tp = c->e1->tp;
		return c;
	}
}

Pname 
gen::exactMatch(Pexpr arg, int constObj)
/*
	look through this gen for an exact match with arg
	if found, return it;
	if ambiguous, issue error and return dummy function
	if not found, return 0;
*/
{
	if (pure_templ()) return 0; // only holds templates
	register Plist gl;
	register int ok;
	Block(Pname) funVec;

	register int numEx = 0;
	for (gl=fct_list; gl; gl=gl->l) {
		register Pname nn = gl->f;
		if (nn->is_template_fct()) continue;
		Pfct f = nn->fct_type();
		register Pname n = f->argtype;
		if(constObj && nn->n_oper!=CTOR && !f->f_const && !f->f_static){
			non_const++;
			continue;
		}
		ok = 0;
		if (!arg) ok = 1;
		else {
		    for(Pexpr e=arg; e; e=e->e2, n=n->n_list) {
			if (!n && f->nargs_known!=ELLIPSIS) break;
			Pexpr a = e->e1;
			Ptype at = a->tp;
			if(at->base == ANY) break;
			if (at->base == ZTYPE) at = int_type;
			if (!exact1(n,at)) break;
			if (!e->e2) ok = 1;
		    }
		}
		if(!ok || n && !n->n_initializer) continue;

		funVec.reserve(numEx+1);
		funVec[numEx++] = nn;
	}

	if (!numEx) return 0;
	if (numEx==1) return funVec[0];

	// see if ``const'' break ties
	Bits bestOnes = ~(Bits(0,numEx));
	return breakTie(funVec,bestOnes,arg,constObj);
}

Pname 
gen::oneArgMatch(Pexpr aarg, int constObj) 
/*
	for a call with one argument:
	look through this gen for the best match for arg
	if found, return it;
	if ambiguous, issue error and return dummy function
	if not found, return 0;
*/
{
	if (pure_templ()) return 0;
	register Plist gl;
	int numFunc = 0;
	Block(Pname) ArgVec;
	Block(Pname) funVec;
	Pname fn = fct_list->f;

	for (gl=fct_list; gl; gl=gl->l) {
		Pname nn = gl->f;
		Pfct ft = nn->fct_type();
		Pname nnargs = ft->argtype;

		if (nn->is_template_fct()) continue;

		if(constObj && fn->n_oper!=CTOR && 
		    !ft->f_const && !ft->f_static) {
			non_const++;
			continue;
		}
		if (!nnargs && ft->nargs_known != ELLIPSIS)
			continue;
		if (nnargs && nnargs->n_list && !nnargs->n_list->n_initializer) 
			continue;
		ArgVec.reserve(numFunc+1);
		funVec.reserve(numFunc+1);
		ArgVec[numFunc] = nnargs ? nnargs : (Pname)ELLIPSIS;
		funVec[numFunc++] = nn;
	}
	if(!numFunc) return 0;

	Bits bestOnes = bestMatch(ArgVec, numFunc, aarg->e1->tp);

	int numFuncs = bestOnes.count();
	if(!numFuncs) return 0;
	if(numFuncs == 1)
		return funVec[bestOnes.signif() - 1];

	return breakTie(funVec,bestOnes,aarg,constObj);
}

Pname 
gen::multArgMatch(Pexpr arg, int constObj)
/*
	for a call with multiple arguments:
	look through this gen for the best match for arg
	if found, return it;
	if ambiguous, issue error and return dummy function
	if not found, return 0;
*/
{
	if (pure_templ()) return 0;
	int numargs = 1;
	Pexpr tmp = arg;
	if(!tmp) return 0;
	while((tmp=tmp->e2)) numargs++;
	Block(BlockPname) intFun(numargs);

	miFlag = 0;
	register int numFunc = 0;
	Block(Pname) funVec;

	for (Plist gl=fct_list; gl; gl=gl->l) {
		register Pname nn = gl->f;

		if (nn->is_template_fct()) continue;

		// first weed out unmatchable functions
		if (!matchable(nn,arg,constObj)) continue;

		// store types in ``matrix'' for bestMatching
		register int ai = 0;
		Pfct tf = nn->fct_type();

		funVec.reserve(numFunc+1);
		funVec[numFunc] = nn;
		for (Pname x=tf->argtype; x&&ai<numargs; x=x->n_list) {
			intFun[ai].reserve(numFunc+1);
			intFun[ai][numFunc] = x;
			ai++;
		} 

		// extend ellipsis arguments
		if(tf->nargs_known == ELLIPSIS) {
			while(ai < numargs) {
			    intFun[ai].reserve(numFunc+1);
			    intFun[ai++][numFunc] = (Pname)ELLIPSIS;
			}
		}
		numFunc++;
	}

	// no matchable functions
	if(numFunc == 0) return 0;

	// finished: only one matchable function
	if(numFunc == 1) return funVec[0];

	// more matchable functions: need intersect rule
	if(numFunc > 1) {  

		Bits bestFuncs = intersectRule(intFun,numFunc,arg);

		Pname best = 0;
		register int sigbit = bestFuncs.signif() - 1;

		switch(bestFuncs.count()) {
		    case 0:	// null intersection
			fmError(1,funVec,arg,constObj);
			best = funVec[0];
			break;

		    default:	//multiple elements in intersection
			best = breakTie(funVec,bestFuncs,arg,constObj);
			sigbit = bestFuncs.signif() - 1;

		    case 1:	// one element in intersection
		    		// before or after breakTie
			if (miFlag==1 && numFunc > 2) {  
				// suspect: need simple rule
				for(int K = 0; K < numFunc; K++) {
					if(K == sigbit) continue;
			    		int gotit = 0;
					Pexpr targ = arg;
					for(int I=0;I<numargs;I++) {
					    if(bestOfPair(intFun[I][sigbit],
						intFun[I][K],targ->e1->tp)) {
						gotit = 1;
						break;
					    }
					    targ = targ->e2;
					}
					if(!gotit) {
					    if(!best) {
						fmError(1,funVec,arg,constObj);
						break;
					    }
					    Bits temp = bestFuncs;
					    bestFuncs.set(K);
			   		    if(breakTie(
						funVec,
						temp,
						arg,
						constObj)!=funVec[sigbit]) {
						    fmError(1,funVec,arg,constObj);
						    break;
					    }
					}
				}
			}
			best = funVec[sigbit];
		}
		return best;
	}

	error('i', "fall off end of gen::multArgMatch()");
	return 0;
}

Bits bestMatch(const Block(Pname)& AV, int nav, Ptype at)
/*
	find the indices of the elements of AV which best match at.
	return a Bits with bits set which correspond to these indices
*/
{
	Bits zeroBits(0,nav);

	Bits result = zeroBits;
	Block(int) rate(nav);
	Block(Pname) udcBlock(nav);

	int i = -1;
        while(++i < nav) {

		Pname aa = AV[i];

		if(aa == 0) continue;
		
		if(aa == (Pname)ELLIPSIS) {
			rate[i] = ELLIP;
			continue;
		}
		Ptype t1 = aa->tp;

		if (t1==at || exact1(aa,at)) {
			rate[i] = EXACT;
			continue;
		}
		if (exact2(aa,at)) {
			rate[i] = PROM;
			continue;
		}
		if (exact3(aa,at)) {
			rate[i] = STD;
			continue;
		}

		int cc = can_coerce(t1,at);
		if (cc == 1) {
			udcBlock[i] = Ncoerce;
			rate[i] = UDC;
			continue;
		}

		rate[i] = NONE;
	}

	int max = NONE;
	for(i=0;i<nav;i++) {
		if( rate[i] > max ) {
			max = rate[i];
			result = zeroBits;
		}
		if( rate[i] && rate[i] == max ) {
			result.set(i);
		}
	}

	if (result.count() <= 1) return result;

	// break ties for STD's involving inheritance
	if (max == STD ) {
		if(at->is_ptr_or_ref()) at = Pptr(at)->typ;
		if (!at->is_cl_obj()) return result;  

		Bits tempBits = result;
		tempBits.reset(tempBits.signif() - 1);

		// nice little algo to find ``real'' best matches
		// taking derivation and MI into account
		while(tempBits.count()) {
			int tempPtr = tempBits.signif() - 1;
			Ptype t1  = AV[tempPtr]->tp;
			Pptr p_t1 = t1->is_ptr();
			for(int k = nav - 1; k > tempPtr; k--) {
				if(!result[k]) continue;
				Ptype t2 = AV[k]->tp;
				if (t1->check(t2,0)==0 || const_problem)
					continue;
				int r = pr_dominate(t1,t2);
				Pptr p_t2 = t2->is_ptr();
				if (r==1 || p_t2 && p_t2->typ->base==VOID) {
					result.reset(k);
				}
				if (r==2 || p_t1 && p_t1->typ->base==VOID) {
					result.reset(tempPtr);
					break;
				}
				if(r==0 && miFlag==0) miFlag = 1;
			}
			tempBits.reset(tempPtr);
		}
	}

	// find UDC sequences which are prefix's of others
	if (max == UDC ) {
		Bits tempBits = result;
		int sigbit = tempBits.signif() - 1;
		tempBits.reset(sigbit);

		while(tempBits.count()) {
			int tempPtr = tempBits.signif() - 1;
			Pname tname  = AV[tempPtr];
			bit done = 0;
			for(int k = nav - 1; k > tempPtr; k--) {
				if(!result[k] || !udcBlock[k]) continue;
				if (udcBlock[k]->tp->base == OVERLOAD) {
					for (Plist gl = Pgen(udcBlock[k]->tp)->fct_list; gl; gl=gl->l) {
						Ptype tt = gl->f->fct_type()->returns;
						Pname r = udcBlock[tempPtr]==udcBlock[k] ?
							bestOfPair(tname,AV[k],tt) : 0;
						if (r==tname)
							result.reset(k);
						if (r==AV[k]) {
							result.reset(tempPtr);
							done = 1;
							break;
						}
					}
				}
				else {
					Ptype tt = udcBlock[k]->fct_type()->returns;
					Pname r = udcBlock[tempPtr]==udcBlock[k] ?
						bestOfPair(tname,AV[k],tt) : 0;
					if (r==tname)
						result.reset(k);
					if (r==AV[k]) {
						result.reset(tempPtr);
						break;
					}
				}
				if (done)
					break;
			}
			tempBits.reset(tempPtr);
		}
	}

	return result;
}

Pname bestOfPair(Pname a1, Pname a2, Ptype at)
/*
    return the bestMatch of a pair of names to at if
    one exists.
    otherwise, return 0;
*/
{
	if(a1->tp == a2->tp) return 0;

	Block(Pname) tryBlock(3);
	tryBlock[0] = a1;
	tryBlock[1] = a2;
	Bits bestBits = bestMatch(tryBlock,2,at);
	if (bestBits.count()==1) {
		return tryBlock[bestBits.signif() - 1];
	}
	return 0;
}

Bits intersectRule(const Block(BlockPname)& intFun, int numFunc, Pexpr arg)
/*
	intersect rule
*/
{
	Bits zeroBits(0,numFunc);
	Bits result = ~zeroBits;

	int ai = 0;
	for(Pexpr aargu = arg; aargu; aargu = aargu->e2) {
		Ptype at = aargu->e1->tp;
		Bits tryit = bestMatch(intFun[ai++],numFunc,at);
		if(tryit.count()==1) miFlag = -1;
		result &= tryit;
		if(!result.count()) { return zeroBits; }
	}
	return result;
}

Pname breakTie(const Block(Pname)& FV,Bits& bestOnes,Pexpr arg,int cO)
/*
	all functions in Block are equal after the intersect rule
	use a mini-intersect rule on the array to see if one dominates 
	all others when trivial conversions involving const are 
	considered.

	if so, return it;
	if not, issue ambiguity error and return any function
*/
{
	register int numFunc = bestOnes.size();
	Bits zeroBits(0,numFunc);
	Bits result = ~zeroBits;

	Block(Pname) rfunc(numFunc);

	int i = 0;
	while(FV[i]) {
		if(bestOnes[i]) rfunc[i]=FV[i]->fct_type()->argtype;
		i++;
	}

	int stat = FV[bestOnes.signif()-1]->fct_type()->f_static;
	for(int k = 0; k < numFunc; k++) {
		if(bestOnes[k]) {
			if(stat != FV[k]->fct_type()->f_static) {
				fmError(1,FV,arg,cO);
				miFlag = 0;
				return FV[bestOnes.signif() - 1];
			}
		}
	}

	// see if ``const'' breaks tie
	for(Pexpr aargu = arg; aargu; aargu = aargu->e2) {
		Ptype at = aargu->e1->tp;

		// best_const
		Bits temp = best_const(rfunc,numFunc,at);
		if(temp.count()) result &= temp;
		if(!result.count()) break;

		for(int k = 0; k < numFunc; k++) {
			if(rfunc[k]) rfunc[k] = rfunc[k]->n_list;
		}
	}

	Pfct pf = FV[0]->fct_type();
	if(result.count()>=1 && pf->memof) { // && FV[0]->n_oper != CTOR) {
		Bits temp = zeroBits;
		for(int k = 0; k < numFunc; k++) {
                        if(bestOnes[k]) {
			    if(stat != FV[k]->fct_type()->f_static) {
				result = zeroBits;
				break;
			    }
			    if(cO == FV[k]->fct_type()->f_const) temp.set(k);
                        }
                }
		if(temp.count()) result &= temp;
	}

	if(result.count()==0 || result.count()>=2) {
		fmError(1,FV,arg,cO);
		miFlag = 0;
	}
	else  bestOnes = result; 

	return FV[bestOnes.signif() - 1];
}

Bits best_const(const Block(Pname)& CONV, int nfound, Ptype at)
{
	Bits zeroBits(0,nfound);
	Bits result = ~zeroBits;
	
	Bits tempBits = ~zeroBits;
	int sigbit = tempBits.signif() - 1;
	tempBits.reset(sigbit);

	for (int i=0; i<nfound; i++) {
		if (CONV[i] == 0) {
			tempBits.reset(i);
			result.reset(i);
		}
	}

	while(tempBits.count()) {
		int tempPtr = tempBits.signif() - 1;
		Pname t1name = CONV[tempPtr];
		for(int k = nfound - 1; k > tempPtr; k--) {
			if(!result[k]) continue;
			Pname t2name  = CONV[k];
			Ptype t1 = t1name->tp;
			Ptype t2 = t2name->tp;
			Pptr p1=t1->is_ref(),p2=t2->is_ref();
			if (p1 && !p2) t1 = p1->typ;
			if (p2 && !p1) t2 = p2->typ;
			p1=t1->is_ptr(),p2=t2->is_ptr();
			if (p1 && p2) {
			    if (at->check(t1,OVERLOAD)==0
				&&
				at->check(t2,OVERLOAD)
			    )
				result.reset(k);
			    else if (at->check(t1,OVERLOAD)
				&&
				at->check(t2,OVERLOAD)==0
			    ) {
				result.reset(tempPtr);
				break;
			    }
			}
			else {
			    Pptr p1=t1->is_ref();
			    Pptr p2=t2->is_ref();

			    if (p1 && p2) {
				if (at->check(p1->typ,OVERLOAD)==0
				    &&
				    at->check(p2->typ,OVERLOAD)
				)
				    result.reset(k);
				else if (
				    at->check(p1->typ,OVERLOAD)
				    &&
				    at->check(p2->typ,OVERLOAD)==0
				) {
				    result.reset(tempPtr);
				    break;
				}
			    }
			}
		}
		tempBits.reset(tempPtr);
	}
	return result;
}

void fmError(int errorKind, const Block(Pname)& FV, Pexpr arg, bit co)
{
	Pname fn = FV[0]->tp->base==OVERLOAD ? 
			Pgen(FV[0]->tp)->fct_list->f : FV[0];

	switch (errorKind) {
	    case 0:
		error('e',"no match for call: ");
		break;
	    case 1:
		ambig = 1;
		error('e',"ambiguous call: ");
		break;
	}

	// call
	Pclass tmp = fn->get_fct()->memof;

	if (tmp) error('c',"%s %t* -> ",co?"const":"",tmp);
	if(fn->n_oper && fn->n_oper!=CTOR) 
		error('c',"operator %s(",keys[fn->n_oper]);
	else if(fn->n_oper==CTOR) {
		error('c',"%t::%t(",tmp,tmp);
	}
	else error('c',"%s(",fn->string);

	if(arg) {
		Pexpr tmp = arg;
		error('c',"%t",tmp->e1->tp->skiptypedefs());
		while((tmp=tmp->e2)) {
			error('c',",%t",tmp->e1->tp->skiptypedefs());
		}
	}
	error('c',")\n");

	// possible functions
	error('C',"choice of%ns:\n",fn);

	if(FV[0]->tp->base == OVERLOAD) {
		int num_templ=0;
		int no_const=0;
		int num_all=0;
		for(Plist gl = Pgen(FV[0]->tp)->fct_list;gl;gl=gl->l) {
			if(gl->f->is_template_fct()) {
				num_templ++;
			}
			else error('C',"	%a;\n",gl->f);
			if (co && !Pfct(gl->f->tp)->f_const) no_const++;
			num_all++;
		}
		if(num_templ) {
			error(
			    'C',
			    "	%d template version%s;\n",
			    num_templ,
			    num_templ==1 ? "" : "s"
			);
		}
		if(no_const == num_all)
			error('C',"(no usable const member function)\n");

		return;
	}

	int numFunc = FV.size();
	for(int i=0; i<numFunc; i++) {
		if(FV[i]) error('C',"	%a;\n",FV[i]);
	}
}
