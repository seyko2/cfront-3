/*ident	"@(#)cls4:src/dcl2.c	1.12" */
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
#include "overload.h"
extern Pexpr make_dot(Pexpr, Ptable, char* s = "i");
extern Pblock top_block;

struct for_info {
	char* s;
	Pstmt rb;
	for_info* next;
};
static for_info* for_info_head = 0;
static int for_check(Pname n, Pstmt rb)
{
	for_info* p;
	if (!n || !rb)
		error('i', "for_check(n=%d,rb=%d)", n, rb);
	if (n->tp->base == FCT || n->tp->base == OVERLOAD)
		return 0;
	p = for_info_head;
	while (p && (strcmp(p->s, n->string) || p->rb != rb))
		p = p->next;
	if (p) {
		return 1;
	}
	else {
		p = new for_info;
		p->s = new char[strlen(n->string) + 1];
		strcpy(p->s, n->string);
		p->rb = rb;
		p->next = for_info_head;
		for_info_head = p;
		return 0;
	}
}
void for_check_delete()
{
	for_info* p;
	for_info* r;

	p = for_info_head;
	while (p) {
		r = p;
		delete p->s;
		p = p->next;
		delete r;
	}
	for_info_head = 0;
}

static Pname is_decl(Pblock cb, Pname v)
{
	if (!cb->d) return 0;
	for (Pname n=cb->d; n; n=n->n_list) {
		if (strcmp(v->string,n->string)==0) 
			return n;
	}
	return 0;
}

static Pname redecl = 0;

Pstmt find_return(Pstmt s, Pname v)
{
	
    while (s) {
	switch(s->base) {
		case RETURN:
		{
			Pname te = s->e->base==NAME ? Pname(s->e) : 0;
			if (!te
			    ||
			    strcmp(te->string,v->string)
			) {
				return s;
			}
			if (te == v) return 0;
			if (redecl) return s;
			else return 0;
		}
		case BLOCK:
		{
			if(!redecl) redecl = is_decl(Pblock(s),v);
			Pstmt tt = find_return(Pblock(s)->s,v);
			if (tt) return tt;
			redecl = 0;
			break;
		}
		case DO:
		case WHILE:
		case FOR:
		case SWITCH:
		case CASE:
		case DEFAULT:
		{
			Pstmt tt = find_return(s->s,v);
			if (tt) return tt;
			break;
		}
		case IF:
		{
			Pstmt tt = find_return(s->s,v);
			if (tt) return tt;
			tt = find_return(s->else_stmt,v);
			if (tt) return tt;
			break;
		}
	}
	s = s->s_list;
    }
    return 0;
}

Pname hier_dominates(Pname on1, Pname on2)
/*
	compare for duplicates and dominance:

	on1 and on2 are two conversion operator functions
	return the the one that dominates the other (according 
	to the class hierarchy) otherwise 0 (0 thus indicates 
	ambiguous conversion)
*/
{ 
	Pfct f1 = on1->get_fct();
	Pfct f2 = on2->get_fct();

	Ptype r1 = f1->returns;
	Ptype r2 = f2->returns;

	if (r1==r2 || r1->check(r2,0)==0) {
		Pclass m1 = f1->memof;
		Pclass m2 = f2->memof;
		if (m1->has_base(m2)) return on1;
		if (m2->has_base(m1)) return on2;
		return 0;
	}
	Pptr p1 = r1->is_ptr_or_ref();
	Pptr p2 = r2->is_ptr_or_ref();
	if (p1 && p2) {
		Pname cn1 = p1->typ->is_cl_obj();
		Pname cn2 = p2->typ->is_cl_obj();
		if (cn1 && cn2) {
			Pclass c1 = Pclass(cn1->tp);
			Pclass c2 = Pclass(cn2->tp);
			if (c1 && c2) {
				if (c1->has_base(c2)) 
					return on1;
				else if (c2->has_base(c1)) 
					return on2;
			}
		}
	}
	return 0;
}

Bits best_conv(const Block(Pname)& CONV, int& nfound, bit cO)
{
	Bits zeroBits(0,nfound);
	Bits result = ~zeroBits;
	
	Bits tempBits = ~zeroBits;
	int sigbit = tempBits.signif() - 1;
	tempBits.reset(sigbit);

	while(tempBits.count()) {
		int tempPtr = tempBits.signif() - 1;
		Pname tname  = CONV[tempPtr];
		for(int k = nfound - 1; k > tempPtr; k--) {
			if(!result[k]) continue;
			Pname r = really_dominate(tname,CONV[k],cO);
			if (r==tname)
				result.reset(k);
			if (r==CONV[k]) {
				result.reset(tempPtr);
				break;
			}
		}
		tempBits.reset(tempPtr);
	}
	nfound = result.count();
	return result;
}

Pstmt curr_loop;
Pstmt curr_switch;
Pblock curr_block;
static Pstmt curr_case; // case or default 
static Pblock block_since_case; // set to curr_block if it follows curr_case 

void stmt::reached()
{
	register Pstmt ss = s_list;

	if (ss == 0) return;

	switch (ss->base) {
	case LABEL:
	case CASE:
	case DEFAULT:
		break;
	default:
		if (ss->where.line != 0)
			error('w',&ss->where,"S after%k not reached",base);
		else
			error('w',"S after%k not reached",base);
		for (; ss; ss=ss->s_list) {	// delete unreacheable code
			switch (ss->base) {
			case LABEL:
			case CASE:
			case DEFAULT:		// reachable
				s_list = ss;
				return;
			case DCL:		// the dcl may be used later
						// keep to avoid cascading errors
			case IF:
			case DO:
			case WHILE:
			case SWITCH:
			case FOR:
			case BLOCK:		// may hide a label
				s_list = ss;
				return;
			}
		}
		s_list = 0;
	}
}

bit oper_okay(Ptype t, TOK b)
{
	if (t->is_ref()) t = Pptr(t)->typ;

	switch (t->base) {
	case PTR:
		if (b == UMINUS) break;
	case FLOAT:
	case DOUBLE:
	case LDOUBLE:
		if (b == COMPL || b == DEREF || b == SWITCH)
			break;
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case EOBJ:
		return 1;
	}

	return 0;
}

Pexpr check_cond(Pexpr e, TOK b, Ptable tbl)
{

	Pname cn;
	if (e == dummy) error("empty condition for %k",b);

	int const_obj = 0;
	const_obj = e->tp->tconst() ? 1 : e->is_const_obj();

	if (cn = e->tp->is_cl_obj()) {	
		int no_const = 0;	// for better error reporting
		Pclass cl = Pclass(cn->tp);
		Pname found = 0;
		Pname other = 0;
		int nfound = 0;
		Block(Pname) conv;
		for (Pname on = cl->conv; on; on=on->n_list) {
			Pfct f = on->get_fct();
			Ptype t = f->returns->skiptypedefs();

			if (oper_okay(t,b) == 1)
			{
				if (const_obj && !f->f_const) {
					no_const++;
					continue;
				}
				
				conv.reserve(nfound+1);
				conv[nfound++] = on;
			}
		}
		if (nfound==1) found = conv[nfound-1];
		if (nfound > 1) {
			Bits b = best_conv(conv,nfound,const_obj);
			int i = b.signif() - 1;
			found = conv[i];
			if(b.count() == 2) {
				b.reset(i);
				other = conv[b.signif() - 1];
			}
		}

		switch (nfound) {
		case 0:
		    if (no_const) 
			error("%nO in%kE (no usable const conversion)",cn,b);
		    else error("%nO in%kE",cn,b);
		    break;
		case 1:
		{
		    Pname xx = new name(found->string);
		    Pexpr c;

		    if (e->lval(0)) {
		        Pref r = new ref(DOT,e,xx);
			Pexpr rr = r->typ(tbl);
			c = new expr(G_CALL,rr,0);
		    }
		    else {	// (temp=init,temp.coerce())
			//Pname tmp = make_tmp('U',e->tp,tbl); 
			//Pexpr ass = init_tmp(tmp,e,tbl);
			Pref r = new ref(DOT,e,xx);
			Pexpr rr = r->typ(tbl);
			c = new expr(G_CALL,rr,0);
			//c = new expr(CM,ass,c);
		    }
		    Pexpr E = c->typ(tbl);
		    if(E->tp->memptr()) {
			E = make_dot(E,tbl);
			E = new expr(NE,E,zero);
			E->tp = int_type;
		    }
		    e = E;
		    break;
		}
		case 2:
		    error("two conversions for%nO in%kE: %n and %n",
			   cn,b,found,other);
		    break;
		default:
		    error("multiple conversions for%nO in%kE",cn,b);
		    break;
		}
		return e;
	}
	if(e->tp->base == FCT) {
 		e = new expr(G_ADDROF,0,e);
 		e = e->typ(tbl);
 	}
	if (e->tp->memptr()) {
		if (b == SWITCH) {
			error("P toM as switchE");
			return e;
		}
		if (e->base == ILIST ||
		    ((e->base == CAST || e->base == G_CAST) && e->e1->base == ILIST)) {
			if (e->base != ILIST)
				e = e->e1;
			if (e->e1->e2 != zero)
				e = one;
			else
				e = zero;
		}
		else {
			e = make_dot(e,tbl);
			e = new expr(NE,e,zero);
			e->tp = int_type;
		}
	}
	else if (e->tp->num_ptr(b) == FCT)
		error("%k(F)",b);
	return e;
}

void stmt::dcl(int forflag)
/*
	typecheck statement "this" in scope "curr_block->tbl"
*/
{
	Pstmt ss;
	Pname n;
	Pname nn;
	Pstmt ostmt = Cstmt;
	DB( if(Ddebug>=1) error('d',&where,"'%k'->stmt::dcl()",base); );
	for (ss=this; ss; ss=ss->s_list) {
		Pstmt old_loop, old_switch;
		Cstmt = ss;
		Ptable tbl = curr_block->memtbl;
//error('d',"stmt::dcl %k",ss->base);
		switch (ss->base) {
		case BREAK:
			inline_restr |= 16;
			if (curr_loop==0 && curr_switch==0)
				error("break not in loop or switch");
			ss->reached();
			break;

		case CONTINUE:
			inline_restr |= 32;
			if (curr_loop == 0)
				error("continue not in loop");
			ss->reached();
			break;

		case DEFAULT:
			if (curr_switch == 0) {
				error("default not in switch");
				break;
			}
			if (curr_switch->has_default)
				error("two defaults in switch");
			curr_case = ss;
			block_since_case = 0;
			curr_switch->has_default = ss;
			ss->s->s_list = ss->s_list;
			ss->s_list = 0;
			ss->s->dcl();
			break;

		case SM:
		{
			if (ss->e ==0)
				break;
			TOK b = ss->e->base;
			switch (b) {
			case DUMMY: // null or error
			case DTOR:  // dummy type destructor
				ss->e = 0;
				break;
					// check for unused results
					// don't check operators that are likely
					// to be overloaded to represent "actions":
					// ! ~ < <= > >= << >>
			case EQ:
			case NE:
			case GT:
			case GE:
			case LT:
			case LE:
			case PLUS:
			case MINUS:
			case REF:
			case DOT:
			case MUL:
			case DIV:
			case ADDROF:
			case AND:
			case OR:
			case ER:
			case DEREF:
			case ANDAND:
			case OROR:
			case NAME:
			case VALUE:
				if (ss->e->tp)
					break;	// avoid looking at generated code
				ss->e = ss->e->typ(tbl);
				if (ss->e->base == CALL)
					break;
				if (ss->e->tp->base != VOID) {
					if ( ss->e->base == DEREF )
						error('w',"result of %sE not used",ss->e->e2?"[]":"*");
					else {
						error('w',"result of%kE not used",b);
						if ( ss->e->base == NAME
						&&   ss->e->tp->base == FCT ) {
						    int i = Pname(ss->e)->n_addr_taken;
						    Pname(ss->e)->n_addr_taken = 1;
						    Pname(ss->e)->dcl_print(0);
						    Pname(ss->e)->n_addr_taken = i;
						}
					}
					if (ss->e->not_simple()==0)
						ss->e = dummy;
				}
				if (ss->e->base == ILIST) {
					Pname tt = make_tmp('A',mptr_type,tbl);
					ss->e = mptr_assign(tt,ss->e);
				}
				break;
			default:
				ss->e = ss->e->typ(tbl);
			}
			break;
		}
		case RETURN:
		{
			Pname fn = cc->nof;
			Pfct f = Pfct(fn->tp);
			Ptype rt = f->returns->skiptypedefs();
			Pexpr v = ss->e;
//error('d',"rt %t",rt);
			if (v != dummy) {
				if (rt->base == RPTR) {
					ref_initializer++;
					v = v->typ(tbl);
					ref_initializer--;
				} else
					v = v->typ(tbl);

				if (
					fn->n_oper==CTOR
					||
					fn->n_oper==DTOR
					||
					rt->base==VOID 
				) {
					error("unexpected return value");
					// refuse to return the value:
					ss->e = dummy;
				}
				else {
					switch (rt->base) {
					case RPTR:
						switch (v->base) {
						case NAME:
							if (
								Pname(v)->n_scope==FCT
								||
								Pname(v)->n_scope==ARG
							)
								error('w',"R to localV returned");
							break;
						case ICON:
						case CCON:
						case FCON:
						case STRING:
							if (Pptr(rt)->typ->tconst()==0)
								error('w',"R to literal returned");
						}
						in_return=1;
						v = ref_init(Pptr(rt),v,tbl);
						in_return=0;
						if (
							v->base==G_CM
							&&
							v->e2->base==G_ADDROF
							&&
							v->e2->e2->base==NAME
							&&
							is_probably_temp(v->e2->e2->string)
						)
							error('w',"R to temporary returned (return value is not lvalue or of wrongT)");
					case ANY:
						break;
					case COBJ:
						if (v->base == DEREF) {
							Pexpr v1 = v->e1;
							if (v1->base==CAST ||
							    v1->base==G_CAST) {
								Pexpr v2 = v1->e1;
								if (v2->base == G_CM) {
										// *(T)(e1,e2) => (e1,*(T)e2)
									Pexpr v3 = v2->e2;
									v2->e2 = v;
									v2->tp = v->tp;
									v = v2;
									v1->e1 = v3;
								}
							}
						}
						if (f->f_result) {
							if (
								v->base==G_CM
								&&
								rt->check(v->tp,0/*ASSIGN*/)==0
							)
								v = replace_temp(v,f->f_result);
							else {
								if (!f->nrv)
									v = class_init(f->f_result->contents(),rt,v,tbl);
								Pname rcn = rt->is_cl_obj();
								if (Pclass(rcn->tp)->has_itor()==0) {
									// can happen for virtuals and for user defined conversions
									v->tp = rt;
									v = new expr(ASSIGN,f->f_result->contents(),v);
									v->tp = rt;
								}
							}
						}
						else
							v = class_init(0,rt,v,tbl);
						break;

					case PTR:
					{	v = ptr_init(Pptr(rt),v,tbl);
						if (
							v->base == ADDROF
							&&
							v->e2->base == NAME
							&&
							Pname(v->e2)->n_stclass == AUTO
						)
							error('w',"P to local variable%n returned",Pname(v->e2));
						if (Pchecked == 0)
							goto def;
						goto ret_save;
					}

					case INT:
					case CHAR:
					case LONG:
					case SHORT:
						if (
							Pbase(rt)->b_unsigned
							&&
							v->base==UMINUS
							&&
							v->e2->base==ICON
						)
							error('w',"negative returned fromF returning unsigned");
					default:
					def:
					{
						Pexpr x = try_to_coerce(rt,v,"return value",tbl);
						int ct = no_const;
						if (x)
							v = x;
						else if (rt->check(v->tp,ASSIGN)) {
							error('e',"bad return valueT for%n:%t (%tX)",fn,v->tp,rt);
							if (ct) error('c'," (no usable const conversion)\n");
							else error('c',"\n");
						}
						break;
					}

					}
				ret_save:
					ss->ret_tp = rt;
					ss->e = v;
				}
			}
			else {
				if (rt->base != VOID)
					error("return valueX");
			}
			ss->reached();
			break;
		}

		case DO:	// in DO the stmt is before the test
			inline_restr |= 8;
			old_loop = curr_loop;
			curr_loop = ss;
			{
				Pstmt st = ss->s;
			  	while(st && st->base == FOR)
					st = st->for_init;
			  	if (st && st->base == DCL)
			  		if (st==ss->s)
						error("D as onlyS in do-loop");
			}
			ss->s->dcl();
			ss->e = ss->e->typ(tbl);
			ss->e = check_cond(ss->e,DO,tbl);
			curr_loop = old_loop;
			break;

		case WHILE:
			inline_restr |= 8;
			old_loop = curr_loop;
			curr_loop = ss;
			ss->e = ss->e->typ(tbl);
			ss->e = check_cond(ss->e,WHILE,tbl);
			{
				Pstmt st = ss->s;
			  	while(st && st->base == FOR)
					st = st->for_init;
			  	if (st && st->base == DCL)
					if(st==ss->s)
						error("D as onlyS in while-loop");
			}
			ss->s->dcl();
			curr_loop = old_loop;
			break;

		case SWITCH:
		{
			int ne = 0;
			Pstmt old_case = curr_case;
			Pblock old_bsc = block_since_case;
			curr_case = 0; block_since_case = 0;
			inline_restr |= 4;
			old_switch = curr_switch;
			curr_switch = ss;
			ss->e = ss->e->typ(tbl);
			ss->e = check_cond(ss->e,SWITCH,tbl);
			{
				Pstmt st = ss->s;
			  	while(st && st->base == FOR)
					st = st->for_init;
			  	if (st && st->base == DCL)
					if(st==ss->s)
						error("D as onlyS in switchS");
			}
			{
				Ptype tt = ss->e->tp->skiptypedefs();

				switch (tt->base) {
				case EOBJ:
					ne = Penum(Pbase(tt)->b_name->tp)->no_of_enumerators;
				case ZTYPE:
				case ANY:
				case CHAR:
				case SHORT:
				case INT:
				case LONG:
				case FIELD:
					break;
				default:
					error("%t switchE",ss->e->tp);
				}
			}
			ss->s->dcl();
			if (ne) {	// see if the number of cases is "close to"
					// but not equal to the number of enumerators
				int i = 0;
				Pstmt cs;
				for (cs=ss->case_list; cs; cs=cs->case_list)
						i++;
				if (i && i!=ne) {
					if (ne < i) {
				ee:		if (ss->has_default==0) error('w',"switch (%t)W %d cases (%d enumerators)",ss->e->tp,i,ne);
					}
					else {
						switch (ne-i) {
						case 1: if (3<ne) goto ee;
						case 2: if (7<ne) goto ee;
						case 3: if (23<ne) goto ee;
						case 4: if (60<ne) goto ee;
						case 5: if (99<ne) goto ee;
						}
					}
				}
			}
			curr_switch = old_switch;
			curr_case = old_case;
			block_since_case = old_bsc;
			break;
		}

		case CASE:
			if (curr_switch == 0) {
				error("case not in switch");
				break;
			}
			curr_case = ss;
			block_since_case = 0;
			ss->e = ss->e->typ(tbl);
			ss->e->tp->num_ptr(CASE);
			{
				Ptype tt = ss->e->tp->skiptypedefs();

				switch (tt->base) {
				case ZTYPE:
				case ANY:
				case CHAR:
				case SHORT:
				case INT:
				case LONG:
				case EOBJ:
				{
					Neval = 0;
					long i = ss->e->eval();
					if (Neval == 0) {
						Pstmt cs;
						if (largest_int<i)
							error("long case value");
						for (cs=curr_switch->case_list; cs; cs=cs->case_list) {
							if (cs->case_value == i)
								error("case %d used twice in switch",i);
						}
						ss->case_value = int(i);
						ss->case_list = curr_switch->case_list;
						curr_switch->case_list = ss;
					}
					else
						error("bad case label: %s",Neval);
					break;
				}
				default:
					error("%t caseE",ss->e->tp);
				}
			}
			if (ss->s->s_list)
				error('i',"case%k",ss->s->s_list->base);
			ss->s->s_list = ss->s_list;
			ss->s_list = 0;
			ss->s->dcl();
			break;

		case GOTO:
			inline_restr |= 2;
			ss->reached();

		case LABEL:
			// Insert label in function mem table;
			// labels have function scope.
			n = ss->d;
			nn = cc->ftbl->insert(n,LABEL);

			// Set a ptr to the mem table corresponding to the scope
			// in which the label actually occurred.  This allows the
			// processing of goto's in the presence of ctors and dtors
			if (ss->base == LABEL) {
				nn->n_realscope = curr_block->memtbl;
				inline_restr |= 1;
			}

			if (Nold) {
				if (ss->base == LABEL) {
					if (nn->n_initializer)
						error("twoDs of label%n",n);
					nn->n_initializer = (Pexpr)1;
				}
				if (n != nn)
					ss->d = nn;
			}
			else {
				if (ss->base == LABEL)
					nn->n_initializer = (Pexpr)1;
				nn->where = ss->where;
			}
			if (ss->base == GOTO)
				nn->use();
			else {
				if (ss->s->s_list)
					error('i',"label%k",ss->s->s_list->base);
				ss->s->s_list = ss->s_list;
				ss->s_list = 0;
				nn->assign();
			}
			if (ss->s)
				ss->s->dcl();
			break;

		case IF:
		{	
			Pexpr ee = ss->e->typ(tbl);
			if (ee->base == ASSIGN) {
				Neval = 0;
				(void)ee->e2->eval();
				if (Neval == 0)
					error('w',"constant assignment in condition");
			}
			ss->e = ee = check_cond(ee,IF,tbl);

			if (ss->s && (ss->s->base == DCL || ss->s->base==FDCL))
				error(&ss->s->where,"D as onlyS after `if'");

			// pointer to member returns with a tp set to 0
			if ( ee->tp )
				switch (ee->tp->base) {
				case INT:
				case EOBJ:
				case ZTYPE:
				{
					long i;
					Neval = 0;
					i = ee->eval();
	
					if (Neval == 0) {
						Pstmt sl = ss->s_list;
						if (i) {
							DEL(ss->else_stmt);
							ss->s->dcl();
							*ss = *ss->s;
						}
						else {
							DEL(ss->s);
							if (ss->else_stmt) {
								ss->else_stmt->dcl();
								*ss = *ss->else_stmt;
							}
							else {
								ss->base = SM;
								ss->e = dummy;
								ss->s = 0;
							}
						}
						ss->s_list = sl;
						continue;
					}
				}
				}
			if (ss->s->base !=FDCL) ss->s->dcl();
			if (ss->else_stmt) {
				if (ss->else_stmt->base == DCL || ss->else_stmt->base==FDCL)
					error(&ss->else_stmt->where,"D as onlyS after `else'");
				if (ss->else_stmt->base != FDCL) ss->else_stmt->dcl();
			}
			break;
		}

		case FOR:
			inline_restr |= 8;
			old_loop = curr_loop;
			curr_loop = ss;
			if (ss->for_init) {
				Pstmt fi = ss->for_init;
				switch (fi->base) {
				case SM:
					if (fi->e == dummy) {
						ss->for_init = 0;
						break;
					}
					fi->dcl();
					break;
				default:
					// for (stmt; e1; e2) stmt1 stmt2
					// => {stmt; for(; e1; e2) stmt1 stmt2}
					//    if stmt != declaration
					// if stmt == declaration, temporarily
					//    rewrite to avoid symbol table
					//    problems in some contexts.
					//    Then put decl back to avoid
					//    extraneous {}.
					// Note: to maintain pointers, ss
					//	must not change
				{
					Pstmt tmp = new stmt (SM,curloc,0);
					*tmp = *ss;	// tmp = original for
					tmp->for_init = 0;
					fi->s_list = tmp;
					*ss = *fi;
					curr_loop = old_loop;
					ss->dcl(1);
					tmp = ss->s_list;
					if (
						ss->base == DCL
						&&
						tmp->base == FOR   // sanity check
						&&
						tmp->for_init == 0 // sanity check
					) {
						// put DCL back in for init
						*fi = *ss;
						*ss = *tmp;
						ss->for_init = fi;
						fi->s_list = 0;
					}
					else {
						// non-decl stmt in for init
						// put stmts in block in case
						//    they follow a condition...
						// allocate tmp to be sure
						//    fields are initialized
						*fi = *ss;
						tmp = new block(ss->where,0,fi);
						tmp->own_tbl = 0;
						tmp->memtbl = curr_block->memtbl;
						tmp->permanent = ss->permanent;
						*ss = *tmp;
						tmp->permanent = 0; delete tmp;
					}
					// don't repeat stmt::dcl() for
					//    remaining stmts
					goto done;
				}
				}
			}
			if (ss->e == dummy)
				ss->e = 0;
			else {
				ss->e = ss->e->typ(tbl);
  				// handle temporary of class object with dtor
  				if (Ntmp) {
  					Ntmp_dtor = new expr(ELIST,ss->e,Ntmp_dtor);
// error('d',"dcl2: stmt::simpl (e %d) temp %n",ss->e,Ntmp);
 				}
				ss->e = check_cond(ss->e,FOR,tbl);
			}
			{
				Pstmt st = ss->s;
			  	while(st && st->base == FOR)
					st = st->for_init;
			  	if (st && (st->base == DCL || st->base == FDCL))
					if(st==ss->s)
						error(&ss->s->where,"D as onlyS in for-loop");
			}
			if (ss->s->base != FDCL) ss->s->dcl();
			ss->e2 = (ss->e2 == dummy) ? 0 : ss->e2->typ(tbl);
			curr_loop = old_loop;
			break;

		case DCL:	/* declaration after statement */
		{
			// collect all the contiguous DCL nodes from the
			// head of the s_list. find the next statement
			int non_trivial = 0;
			int count = 0;
			Pname tail = ss->d;
			for (Pname nn=tail; nn; nn=nn->n_list) {
				//	find tail;
				//	detect non-trivial declarations
				count++;

				if (nn->n_list)
					tail = nn->n_list;
				Pname n = tbl->look(nn->string,0);

				if (n && n->n_table==tbl)
					non_trivial = 2;
				if (non_trivial >= 2)
					continue;
				if (
					nn->n_sto==STATIC
					&&
					nn->tp->base!=FCT
					||
					nn->tp->is_ref()
					||
					nn->tp->tconst()
					&&
					fct_const==0
				) {
					non_trivial = 2;
					continue;
				}

				if ( nn->base == NAME ) {
				    Pexpr in = nn->n_initializer;
				    if (in)
					switch (in->base) {
					case ILIST:
					case STRING:
						non_trivial = 2;
						continue;
					}

				    if (in)
					non_trivial=3;
				    else non_trivial = 1;
				}
				Pname cln = nn->tp->is_cl_obj();
				if (cln == 0)
					cln = cl_obj_vec;
				if (cln == 0)
					continue;
				if (Pclass(cln->tp)->has_ctor()) {
					non_trivial = 3;
					continue;
				}
				if (Pclass(cln->tp)->has_dtor())
					non_trivial = 2;
			}

			while( ss->s_list && ss->s_list->base==DCL ) {
				Pstmt sx = ss->s_list;
				tail = tail->n_list = sx->d;	// add to tail
				for (nn=sx->d; nn; nn=nn->n_list) {
					//	find tail;
					//	detect non-trivial declarations
					count++;
					if (nn->n_list)
						tail = nn->n_list;
					Pname n = tbl->look(nn->string,0);
					if (n && n->n_table==tbl)
						non_trivial = 2;
					if (non_trivial >= 2)
						continue;
					if (
						nn->n_sto==STATIC
						&&
						nn->tp->base!=FCT
						||
						nn->tp->is_ref()
						||
						nn->tp->tconst()
						&&
						fct_const==0
					) {
						non_trivial = 2;
						continue;
					}
					if ( nn->base == NAME ) {
					    Pexpr in = nn->n_initializer;
					    if (in)
						switch (in->base) {
						case ILIST:
						case STRING:
							non_trivial = 2;
							continue;
						}
					}

					non_trivial = 1;
					Pname cln = nn->tp->is_cl_obj();
					if (cln == 0)
						cln = cl_obj_vec;
					if (cln == 0)
						continue;
					if (Pclass(cln->tp)->has_ctor()) {
						non_trivial = 2;
						continue;
					}
					if (Pclass(cln->tp)->has_dtor())
						non_trivial = 2;
				}
				ss->s_list = sx->s_list;
			}

			Pstmt next_st = ss->s_list;
//error('d',"dcl stmt : d %n  non_trivial %d  curr own_tbl %d  inline_restr 0%o",ss->d,non_trivial,curr_block->own_tbl,inline_restr);
			if (
				non_trivial==3
				||
				non_trivial==2			// must
				||
				non_trivial==1			// might
				&&
				(
					curr_block->own_tbl==0	// why not?
					||
					inline_restr&3		// label seen
				)
			) {
				if (curr_switch && non_trivial>=2) {
					if ( curr_case == 0
					||   block_since_case == 0 )
						if (non_trivial==3)
							error("jump past initializer (did you forget a '{ }'?)");
						else
							error("non trivialD in switchS (try enclosing it in a block)");
				}

				//	Create a new block,
				//	put all the declarations at the head,
				//	and the remainder of the slist as the
				//	statement list of the block.

				//	check that there are no redefinitions
				//	since the last "real" (user-written,
				//	non-generated) block

				{
					Pname lastnn = 0;
					for( nn=ss->d; nn; nn=nn->n_list ) {
					Pname n;
//error('d',"checking %n  lex_level: %d  n: %n  n->lex_level: %d",nn,nn->lex_level,n,n?n->lex_level:0);
//error('d',"   own_tbl: %d  curr_block: %d  n->n_table: %d",curr_block->own_tbl,curr_block,n->n_table);
//error('d',"   real_block: %d  n's real_block: %d",curr_block->memtbl->real_block,n->n_table->real_block);
					if (
						(n=curr_block->memtbl->look(nn->string,0))
						&&
						n->n_table->real_block==curr_block->memtbl->real_block
						&&
						n->tp->base!=FCT
						&&
						n->tp->base!=OVERLOAD
						&&
						nn->lex_level == n->lex_level
						||
						(forflag && for_check(n=nn, curr_block->memtbl->real_block))
					) {
						error("twoDs of%n",n);
						if (lastnn==0)
							ss->d=nn->n_list;
						else
							lastnn->n_list=nn->n_list;
					}
					else
						lastnn = nn;
				  	}	// for nn
				}

				//	attach the remainder of the s_list
				//	as the statement part of the block.
				ss->s = next_st;
				ss->s_list = 0;

				//	create the table in advance,
				//	in order to set the real_block
				//	ptr to that of the enclosing table
				ss->memtbl = new table(count+4,tbl,0);
				ss->memtbl->real_block = curr_block->memtbl->real_block;
				Pblock(ss)->dcl(ss->memtbl);
			}
			else {	//	to reduce the number of symbol tables,
				//	do not make a new block,
				//	instead insert names in enclosing block,
				//	and make the initializers into expression
				//	statements.
				Pstmt sss = ss;
				{
					Pname lastnn = 0;
					for( nn=ss->d; nn; nn=nn->n_list ) {
						Pname n;
						if (
							(n=curr_block->memtbl->look(nn->string,0))
							&&
							n->n_table->real_block==curr_block->memtbl->real_block
							&&
							n->tp->base!=FCT
							&& n->tp->base!=OVERLOAD
							&&
							nn->lex_level == n->lex_level
							||
							(forflag && for_check(n=nn, curr_block->memtbl->real_block))
						) {
							error("twoDs of%n",n);
							n = 0;
							if (lastnn==0)
								ss->d=nn->n_list;
							else
								lastnn->n_list=nn->n_list;
						}
						else {
							n = nn->dcl(tbl,FCT);
							lastnn=nn;
						}

						if (n == 0) {
							if (ss) {
								ss->base = SM;
								ss->e = 0;
							}
							continue;
						}
//error('d',"hoisted %n to outer blk",n);

						if (ss) {
							sss->base = SM;
							ss = 0;
						}
						else
							sss = sss->s_list = new estmt(SM,sss->where,0,0);
						if (n->base==NAME && n->n_initializer) {
							Pexpr in = n->n_initializer;
							n->n_initializer = 0;
							switch (in->base) {
							case G_CALL:	/* constructor? */
							{
								Pname fn = in->fct_name;
								if (fn && fn->n_oper==CTOR)
									break;
							}
							default:
								in = new expr(ASSIGN,n,in);
								in->tp = n->tp;
							}
							sss->e = in;
							sss->where=n->where;
						}
						else
							sss->e = dummy;
					}	// for nn
				}

				ss = sss;
				ss->s_list = next_st;
			}
			break;
		}

		case BLOCK:
			Pblock(ss)->dcl(tbl);
			break;

		case ASM:
			/* save string */
		{
			char* s = (char*)ss->e;
			int ll = strlen(s);
			char* s2 = new char[ll+1];
			strcpy(s2,s);
			ss->e = Pexpr(s2);
			break;
		}

		default:
			error('i',"badS(%p %d)",ss,ss->base);
		}
	}
done:
	Cstmt = ostmt;
}

void block::dcl(Ptable tbl)
/*
	Note: for a block without declarations memtbl denotes the table
	for the enclosing scope.
	A function body has its memtbl created by fct::dcl().
*/
{
	int bit_old = bit_offset;
	int byte_old = byte_offset;
	Pblock block_old = curr_block;
	Pstmt save_case = curr_case;
	Pblock old_bsc = block_since_case;

	Pfct f = cc->nof->fct_type();

	if (base != BLOCK && base != DCL)
		error('i',"block::dcl(%d)",base);

	curr_block = this;
	if ( curr_case && base == BLOCK )
		block_since_case = this;

	Pname re_decl = 0;
	if (f->f_result && s && f->nrv == 0
	    &&
	    (this == top_block || memtbl && memtbl->real_block == top_block)
	) {
		Pstmt tmp = s;
		while (tmp->base != RETURN && tmp->s_list) 
			tmp = tmp->s_list;
		if(tmp->base == RETURN) {
			Pname tt = tmp->e->base == NAME ? Pname(tmp->e) : 0;
			Pname td;
			if (tt && (td=is_decl(curr_block,tt))
			    &&
			    td->n_sto != STATIC
			    &&
			    td->tp->check(f->returns,0) == 0
			) {
				f->nrv = tt;
			}
		}
		// disable optimization if something other than
		// the nrv variable is returned
		if (f->nrv) {
			if (find_return(s,f->nrv)) f->nrv = 0;
		}
	}
	else if (f->nrv) re_decl = is_decl(curr_block,f->nrv);

	Pktab otbl = Ctbl;//SYM
	if ( k_tbl )
		Ctbl = k_tbl;//SYM
	if (d) {
		own_tbl = 1;
		base = BLOCK;
		if (memtbl == 0) {
			int nmem = d->no_of_names()+4;
			memtbl = new table(nmem,tbl,0);
			memtbl->real_block = this;
			//	this is a "real" block from the
			//	source text, and not one created by DCL's
			//	inside a block.
		}
		else if (memtbl != tbl)
			error('i',"block::dcl(?)");

		Pname nx;
		for (Pname n=d; n; n=nx) {
			nx = n->n_list;
			if (f->nrv && !re_decl 
			    &&
			    strcmp(f->nrv->string,n->string)==0
			) {	// found nrv in declaration
				n->string = new char[6];
				strcpy(n->string,"__NRV");
			}
			n->dcl(memtbl,FCT);
			switch (n->tp->base) {
			case CLASS:
			case ANON:
			case ENUM:
				break;
			default:
				delete n;
			}
		}
	}
	else if ( base == BLOCK ) {
		own_tbl = 1;
		if (memtbl == 0) {
			int nmem = 4;
			memtbl = new table(nmem,tbl,0);
			memtbl->real_block = this;
			//	this is a "real" block from the
			//	source text, and not one created by DCL's
			//	inside a block.
		}
		else if (memtbl != tbl)
			error('i',"block::dcl(?)");
	}
	else {
		base = BLOCK;
		memtbl = tbl;
	}

	Pname odcl = Cdcl;
	if (s)
		s->dcl();
	if (own_tbl) {
		Pname m;
		int i;
		for (m=memtbl->get_mem(i=1); m; NEXT_NAME(memtbl,m,i)) {
			Ptype t = m->tp;

			if (in_class_dcl)
				m->lex_level -= 1;

			if (t == 0) {
				if (m->n_assigned_to == 0)
					error("label %sU",m->string);
				if (m->n_used == 0)
					error('w',&m->where,"label %s not used", m->string);
				continue;
			}

			t = t->skiptypedefs();

			switch (t->base) {
			case CLASS:
			case ANON:
			case ENUM:
			case FCT:
				continue;
			}

			if (m->n_addr_taken == 0) {
				if (m->n_used) {
					if (
						m->n_assigned_to==0
						&&
						t->base != VEC
						&&
						m->n_scope==FCT
					) {
						Cdcl = m;
						if (m->string[0] != '_' && m->string[1] != '_' )
							error('w',&m->where,"%n used but not set",m);
					}
				}
				else {
					if (
						m->n_assigned_to==0
						&&
						(m->string[0]!='_' || m->string[1]!='_')
						&&
						(m->n_scope==ARG || m->n_scope==FCT)
					) {
						Cdcl = m;
						error('w',&m->where,"%n not used",m);
					}
				}
			}
		}
	}
	Cdcl = odcl;

	d = 0;

	if (bit_offset)
		byte_offset += SZ_WORD;
	bit_offset = bit_old;
	byte_offset = byte_old;
	if ( curr_case == save_case ) block_since_case = old_bsc;
	curr_block = block_old;
	Ctbl = otbl;//SYM
}

void name::field_align()
/*
	 adjust alignment
*/
{
	Pbase fld = (Pbase)tp;
	int nbits = fld->b_bits;

	int a =	(F_SENSITIVE)
		?
		fld->b_fieldtype->align()
		:
		SZ_WORD;
	if (max_align < a)
		max_align = a;

	if (nbits == 0) {		// force word alignment
		int b;
		if (bit_offset)
			nbits = BI_IN_WORD - bit_offset;
		else if (b = byte_offset%SZ_WORD)
			nbits = b * BI_IN_BYTE;
		if (max_align < SZ_WORD)
			max_align = SZ_WORD;
	}
	else if (bit_offset == 0) {	// take care of part of word
		int b = byte_offset%SZ_WORD;
		if (b) {
			byte_offset -= b;
			bit_offset = b*BI_IN_BYTE;
		}
	}
//error('d',"byteoff %d bitoff %d bits %d",byte_offset,bit_offset,nbits);
	int x = (bit_offset += nbits);
	if (SZ_INT * BI_IN_BYTE < x) {
		fld->b_offset = 0;
		byte_offset += SZ_INT;
		bit_offset = nbits;
	}
	else {
		fld->b_offset = bit_offset;
		if (SZ_INT * BI_IN_BYTE == x) {
			bit_offset = 0;
			byte_offset += SZ_INT;
		}
		else
			bit_offset = x;
	}
	n_offset = byte_offset;
}
