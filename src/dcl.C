/*ident	"@(#)cls4:src/dcl.c	1.26" */
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


dcl.c:

	``declare'' all names, that is insert them in the appropriate symbol tables.

	Calculate the size for all objects (incl. stack frames),
	and find store the offsets for all members (incl. auto variables).
	"size.h" holds the constants needed for calculating sizes.

	Note that (due to errors) functions may nest

*****************************************************************************/
#include "cfront.h"
#include "size.h"
#include "template.h"

class dcl_context ccvec[MAXCONT], * cc = ccvec;
int byte_offset;
int bit_offset;
int max_align;
int friend_in_class;
extern int no_const;
static Pstmt itail;

Pname dclass(Pname, Ptable);
Pname denum(Pname, Ptable);
void merge_init(Pname, Pfct, Pfct);

static bit check_static_pt(Pname n)
{
	if (!dtpt_opt)
		return 1;
	if (all_flag)
		return 1;
	if (none_flag)
		return 1;
	if (n->n_stclass == AUTO || n->n_stclass == REGISTER)
		return 1;
	if (n->n_sto==STATIC)
		return 1;
	if (curloc.file != first_file)
		return 0;

	return 1;
}

static int is_empty( Pclass cl, bit const_chk = 0 )
{	/*
	** for nested class check, empty means *no* members
	** for const object check, means no *data* members
	*/

	int mbr_cnt = cl->memtbl->max();

	if ( mbr_cnt == 0 )
		return 1;
	if ( cl->baselist == 0 && cl->real_size!=1 )
		return 0;

	// empty class to turn on transitional nested class scope
	if (
		const_chk == 0
		&&
		( cl->baselist != 0 || mbr_cnt > 1 )
	)
		return 0;

	int i = 1;
	for (Pname nn=cl->memtbl->get_mem(i); nn; NEXT_NAME(cl->memtbl,nn,i)) {
		if (
			nn->base==NAME
			&& 
			nn->n_anon==0
			&& 
			nn->tp->base!=FCT
			&& 
			nn->tp->base!=OVERLOAD
			&& 
			nn->tp->base!=CLASS
			&& 
			nn->tp->base!=ENUM
			&& 
			nn->tp->base!=EOBJ
			&& 
			nn->n_stclass != STATIC
		) {
			if (
				nn->string[0]=='_'
				&&
				nn->string[1]=='_'
				&&
				nn->string[2]=='W'
			)
				return 1;
			else
				return 0;
		}
	}

	return 1;	// if here, no data members encountered
}

void dosimpl(Pexpr e, Pname n)
{
	if (n==0) {
		if (dummy_fct == 0)
			make_dummy();
		n = dummy_fct;
	}
	Pname cf = curr_fct;
	curr_fct = n;
	e->simpl();
	curr_fct = cf;
}

static Pexpr co_array_init(Pname n, Ptable tbl)
/*
	handle simple arrays only. To be done well list_check() must
	be rewritten to handle dynamic initialization.
*/
{
	Pexpr init = n->n_initializer;

	if (init->base != ILIST) {
		error("badIr for array ofCOs%n",n);
		return 0;
	}

	Pexpr el = 0;
	Pvec v = Pvec(n->tp->skiptypedefs());
	Pname cn = v->typ->is_cl_obj();
	Pclass cl = cn ? Pclass(cn->tp) : 0;
	int i = v->size;

	int count = 0;
	Pexpr il2;
	for (Pexpr il = init->e1; il; il = il2) {
			// generate	n[0].cl(initializer),
			//		...
			//		n[max].cl(initializer),
		Pexpr e = il->e1;
		il2 = il->e2;
		il->e2 = 0;
		if (e == dummy)
			break;
		if (e->base == VALUE) {
			switch (e->tp2->base) {
			case CLASS:
				if ( !same_class(Pclass(e->tp2),cl) )
					e = new texpr(VALUE,cl,il);
				break;
			default:
				{
				Pname n2 = e->tp2->is_cl_obj();
				if (n2==0 || !same_class(Pclass(n2->tp),cl))
					e = new texpr(VALUE,cl,il);
				}
			}
		}
		else
			e = new texpr(VALUE,cl,il);
		e->e2 = new expr(DEREF,n,new ival(count++));
		e = e->typ(tbl);
		Ptable oscope = scope;
		scope = tbl;
		dosimpl(e,cc->nof);
		scope = oscope;
		if (sti_tbl == tbl) {
			Pstmt ist = new estmt(SM,no_where,e,0);
			if (st_ilist == 0)
				st_ilist = ist;
			else
				itail->s_list = ist;
			itail = ist;
		}
		else {
//			if (il->e1->base==VALUE && il->e1->e1==0 && i!=0) { // no explicit initializer
//				el = new texpr(NEW,n->tp,0);
//				el = el->typ(tbl);
//				el->simpl();
//			}
//			else
			el = el ? new expr(G_CM,el,e) : e;
		}
	}

	if (i==0)
		v->size = count;
	else if (i<count) {
		error("too manyIrs for%n (%d)",n, count);
		return 0;
	}	
	else if (i>count) {
		if (cl->has_ictor())
			error('s',"too fewIrs for%n",n);
		else
			error( "too fewIrs for%n (C %srequires a defaultK)", n, cl->string );
		return 0;
	}
	return el;
}

int need_sti(Pexpr e, Ptable tbl, bit accept_name)
/*
	check if non-static variables or operations are used
	INCOMPLETE 
*/
{
	if (e == 0)
		return 0;

	switch (e->base) {
	case QUEST:
		if (need_sti(e->cond,tbl,0) && tbl==0)
			return 1;
	case PLUS:
	case MINUS:
	case MUL:
	case DIV:
	case MOD:
	case ER:
	case OR:
	case ANDAND:
	case OROR:
	case LS:
	case RS:
	case EQ:
	case NE:
	case LT:
	case LE:
	case GT:
	case GE:
	case INCR:
	case DECR:
	case ASSIGN:
		if (need_sti(e->e1,tbl,accept_name) && tbl==0)
			return 1;
		// no break;

	case UMINUS:
	case UPLUS:
	case NOT:
	case COMPL:
		if (need_sti(e->e2,tbl,accept_name) && tbl==0)
			return 1;
		// no break

	case SIZEOF:
	default:
		return 0;

	case CAST:
	case G_CAST:
		return need_sti(e->e1,tbl,accept_name);

	case ADDROF:
		return need_sti(e->e2,tbl,1);

	case NAME:
		if (accept_name && Pname(e)->n_stclass==STATIC)
			return 0;

		if (e->tp->tconst()) {
			if (vec_const || fct_const)
				return 0;
			Neval = 0;
			e->eval();
			if (Neval == 0)
				return 0;
		}
		return 1;

	case DEREF:
	case REF:
	case DOT:
		if (accept_name || e->tp && e->tp->base==VEC) {
			int x1 = need_sti(e->e1,tbl,e->base == DOT);
			int x2 = need_sti(e->e2,tbl,0);
			return x1 || x2;
		}
		// no break

	case ELIST:
	case G_CM:
	case CM:
		if (e->base==CM) {
			if (need_sti(e->e1,tbl,0) || need_sti(e->e2,tbl,0))
				return 1;
			else
				return 0;
		}
		// no break
	case CALL:
	case G_CALL:
	case NEW:
	case GNEW:
	case 0:			// hack for `new type (expr)'
		if (tbl) {
			need_sti(e->e1,tbl,accept_name);
			need_sti(e->e2,tbl,accept_name);
			if (
				e->tp && e->tp->base == VEC
				&&
				e->base == NEW || e->base == GNEW
			)
				need_sti(Pvec(e->tp)->dim,tbl);		// preserve ICON,STRING,CCON,FCON
		}
		else if (e->base == 0)
			return 0;
		// no break
	case ICALL:
		return 1;
	case ICON:
	case STRING:
	case CCON:
	case FCON:
		if (tbl) {
			char* p = new char[strlen(e->string)+1];
			strcpy(p,e->string);
			e->string = p;
		}
		return 0;
	}
}

static void check_def_name( Pname nn, int scope )
{
	Pfct f = Pfct(nn->tp);
	if (
		def_name==0
		&&
		pdef_name==0
		&&
		friend_in_class == 0
		&&
		scope == EXTERN
		&&
		nn->n_scope != STATIC
		&&
		nn->n_oper != NEW
		&&
		nn->n_oper != DELETE
	) { 
		if (
			f->body
			&&
			f->f_inline == 0
			&&
			f->f_imeasure == 0
		) {
			pdef_name = def_name = nn;
			def_name = 0;
		}
	}

	if (
		strcmp(nn->string,"main")==0
		&&
		nn->n_table==gtbl
		&&
		f->f_inline
	) {

		pdef_name = def_name = nn;
		def_name = 0;
	}
}

static void
export_anon( Pname un, Pclass cl, Ptable tbl )
{
	if (tbl==gtbl) {
	    if (un->n_sto!=STATIC)
		error("extern anonymous union (declare as static)");
	}
	else if (tbl->t_name && un->n_sto==STATIC)
		error('s',"staticM anonymous union");
// cannot cope with use counts for ANONs:
	Pbase(un->tp)->b_name->n_used = 1;
	Pbase(un->tp)->b_name->n_assigned_to = 1;
	Ptable mtbl = cl->memtbl;
	Ptable realtbl = (tbl==gtbl || tbl->t_name) ? tbl : curr_block->memtbl;
	int i;
	for (Pname nn=mtbl->get_mem(i=1); nn; NEXT_NAME(mtbl,nn,i)) {
		if (nn->base == NAME && nn->tp->base == FCT) {
			error(&nn->where,"FM%n for anonymous union",nn);
			continue;
		}
		Ptable tb = nn->n_table;
		nn->n_table = 0;
		nn->n_tbl_list = 0;
		nn->n_scope = un->n_protect?un->n_protect:un->n_scope; 
		if ( nn->n_key == HIDDEN ) // was an error ...
			continue;
		if ( nn->n_key == CLASS 
		&&   nn->string[0] == '_'
		&&   nn->string[1] == '_'
		&&   nn->string[2] == 'C' )
			continue;
//error('d',&nn->where,"exporting anon unionM%n base%k tp%t n_key%k",nn,nn->base,nn->tp,nn->n_key);
		Pname n = realtbl->look(nn->string,0);
//error('d',&nn->where,"nn%n n%n ll %d %d",nn,n,n?n->lex_level:0,nn->lex_level);
//if(n)error('d',&nn->where,"  ntbl %d tbl %d realtbl %d",n->n_table,tbl,realtbl);
		if ( n && n->n_table != tbl ) {
		    if ( tbl==gtbl || tbl->t_name )
			error('i',&nn->where,"table mismatch");
		    if ( n->n_table->real_block != realtbl->real_block
		    ||   n->lex_level != nn->lex_level
		    )
			n = 0;
		}
		if ( n == 0 ) {
		adef:	n = tbl->insert(nn,nn->n_key);
			if ( nn->n_key == CLASS && Nold ) { // class/enum tag 
				// error caught in norm.c
				//error(&nn->where,"twoDs of tag %s (one in anonymous union)",n->string);
				continue;
			}
		} else {
//error('d',&nn->where," -- n%n base%k tp%t n_key%k",n,n->base,n->tp,n->n_key);
		    if ( nn->n_key == CLASS ) { // class/enum tag 
			if ( n->base == TNAME ) { // previously typedef 
			    error(&nn->where,"twoDs ofTN %s (one in anonymous union)",n->string);
			    continue;
			}
			goto adef;
		    }
		    if ( n->base != nn->base  // id and typedef 
		    ||   nn->base == NAME     // both are ids
		    ||   n->tp->check(nn->tp,0) // non-matching typedefs
		    )
			error(&nn->where,"twoDs of %s (one in anonymous union)",n->string);
		    continue;
		}

		n->n_anon = un->string;
		nn->n_table = tb;
		if ( cl->in_class && un->n_sto == STATIC 
		&&   nn->base != TNAME && nn->n_key != CLASS 
		)
			n->n_stclass = STATIC;
	}
}

void
classdef::make_vec_ctor(Pname default_ctor)
{
/* make a non-argument stub ctor for this class that
 * invokes the default argument ctor for use with vec_new */ 
// error('d',"make_vec_ctor(%n )", default_ctor);

	Pname tn = Pfct(default_ctor->tp)->f_this;
        if (tn->base == ANAME) { // inline ctor ...
		tn = new name();
		*tn = *(Pfct(default_ctor->tp)->f_this);
		tn->base = NAME;
	}

	Pname cn = this->k_tbl->find_cn(string);//SYM
	if (cn) cn = Pbase(cn->tp)->b_name;
	cc->stack(); 
	cc->not= cn; 
	cc->cot = this;
        cc->c_this = tn;

	Pexpr th = new expr(THIS,0,0);
	Pstmt s = new estmt(SM,no_where,0,0);
 	Pexpr e = call_ctor(memtbl,th,default_ctor,0,REF);
	s->e = e;

	Pname fn = new name(string);
	Pfct f = new fct(void_type,0,1);
	fn->tp = f;
	f->body = new block(curloc,0,s);
	Pname nn = fn->dcl(memtbl,PUBLIC);
	nn->n_sto = STATIC;
	cc->unstack();

	nn->simpl();
	nn->dcl_print(0);
	this->c_vtor = nn;
	delete fn;
}

int stat_init = 0;	// in an expression initializing a local static
Pname name::dcl(Ptable tbl, TOK scope)
/*
	enter a copy of this name into symbol table "tbl";
		- create local symbol tables as needed
	
	"scope" gives the scope in which the declaration was found
		- EXTERN, FCT, ARG, PUBLIC, or 0
	Compare "scope" with the specified storage class "n_sto"
		- AUTO, STATIC, REGISTER, EXTERN, OVERLOAD, FRIEND, or 0

	After name::dcl()
	n_stclass ==	0		class or enum member
			REGISTER	auto variables declared register
			AUTO		auto variables not registers
			STATIC		statically allocated object
	n_scope ==	0		private class member
			PUBLIC		public class member
			EXTERN		name valid in this and other files
			STATIC		name valid for this file only
			FCT		name local to a function
			ARG		name of a function argument
			ARGT		name of a type defined in an
					argument list
			ARGS		temporary class object with dtor in
					initialization of a local static


	typecheck function bodies;
	typecheck initializers;

	note that functions (error recovery) and classes (legal) nest

	The return value is used to chain symbol table entries, but cannot
	be used for printout because it denotes the sum of all type information
	for the name

	names of typenames are marked with n_oper==TNAME

	WARNING: The handling of scope and storage class is cursed!
*/
{
	Pname nn;
	Pname odcl = Cdcl;
	int sti_vb = 0;		// set if initialize with virtual base class

	Cdcl = this;
	Ptype tx = tp->skiptypedefs();

// error('d',"%n->dcl(%d %k) tp %t ",this,tbl,scope,tp);

	switch (base) {
	case TNAME:
	DB( if(Ddebug>=1) error('d',&where,"dclTN%n%t scope%k",this,tp,scope); );
		{
		nn = tbl->look(string,0);
		tp->dcl(tbl);

		if (tpdef) { // nested 
			Ptype tx = tpdef;
			if ( tx->in_class && ( tx->templ_base==BOUND_TEMPLATE || tx->templ_base==CL_TEMPLATE ))
			{
				delete tx->nested_sig;
				tx->nested_sig = make_nested_name(string,tx->in_class);
			}
		}

//error('d',"%n->dcl TN tp%t nn%n nn->tp%t",this,tp,nn,nn?nn->tp:0);
//error('d',"      ll this %d nn %d tbl %s",lex_level,nn?nn->lex_level:0,tbl==gtbl?"global table":tbl->t_name?tbl->t_name->string:"???");
//if(tpdef)error('d',"  in%t",tpdef->in_class);

                /* actual redefinitions are caught in name::tdef() */
		if ( nn ) {
			Cdcl = odcl;
			return 0;
		}

		PERM(tp);
		nn = new name(string);
		nn->base = TNAME;
		nn->where = where;
		nn->tp = tp;
		nn->lex_level = lex_level;//SYM
		nn->tpdef = tpdef;

		Pname tn = tbl->insert(nn,0);
		PERM(tn);
		extern void typedef_check(Pname);
		typedef_check(tn);
		delete nn;
		Cdcl = odcl;
		return this;
		}

	case CATCH: // avoid errors
		base = NAME;
		scope = ARG;
		// no break

	case NAME:
	DB( if(Ddebug>=1) {
			error('d',&where,"dclN%n%t (q%n) scope%k",this,tp,n_qualifier,scope);
			error('d',&where,"  n_oper%k n_sto%k n_stclass%k n_scope%k",n_oper,n_sto,n_stclass,n_scope);
	});
		switch (n_oper) {
		case COMPL:
			if (tp->base != FCT) {
				error("~%s notF",string);
				n_oper = 0;
			}
			break;
		case TNAME:
			if (tp->base != FCT)
				n_oper = 0;
			break;
		}
		break;
	default:
		error('i',"NX inN::dcl()");
		break;
	}

	if (n_qualifier) {			// c::f()
						// class function,
						// friend declaration, or 
						// static member initializer
		Pname cn = n_qualifier;
		switch (cn->base) {
		case TNAME:
			break;
		case NAME:
			cn = gtbl->look(cn->string,0);
			if (cn && cn->base==TNAME)
				break;
		default:
			error("badQr%n for%n",n_qualifier,this);
			Cdcl = odcl;
			return 0;
		}

		cn->tp = cn->tp->skiptypedefs();
		if (cn->tp->base != COBJ) {
			error(&where,"Qr%n not aCN",n_qualifier);
			Cdcl = odcl;
			return 0;
		}

		cn = Pbase(cn->tp)->b_name;
		if (n_oper)
			check_oper(cn);

		Pclass cl = Pclass(cn->tp);
		if ( same_class(cl,cc->cot) ) {
			n_qualifier = 0;
			goto xdr;
		}
		else if ((cl->defined&(DEFINED|SIMPLIFIED)) == 0) {
			error("C%nU",cn);
			Cdcl = odcl;
			return 0;
		}
		else if (cl->c_body==1)		//III
			cl->dcl_print(0);

		Ptable etbl = cl->memtbl;
		Pname x = etbl->look(string,0);
		if (x==0 || x->n_table!=etbl ) {
			Ptable tt = n_table;
			n_table = etbl;
			error("%n is not aM of%n",this,cn);
			n_table = tt;
			Cdcl = odcl;
			return 0;
		}

		if (tp->base == FCT) {		//III
			if (
				friend_in_class==0
				&&
				n_sto!=FRIEND
				&&
				Pfct(tp)->body==0
			) {			// c::f(); needed for friend
				error("QdN%n inFD",x);
				Cdcl = odcl;
				return 0;
			}

			if (Pfct(tp)->body==0) {
				Pfct(tp)->memof = cl;
				int xx;
				if (x->tp->base==OVERLOAD)
					xx = Pgen(x->tp)->find(Pfct(tp),0)==0;
				else
					xx = x->tp->check(tp,0);

				if (xx) {
					Ptable tt = n_table;
					n_table = etbl;
					error("%n ofT%t is not aM of%n",this,tp,cn);
					n_table = tt;
					Cdcl = odcl;
					return 0;
				}
			}
		}
		else {
			if (x->n_stclass != STATIC) {	// e.g. int c::i = 7
				error("D of non staticCM%n",this);
				Cdcl = odcl;
				return 0;
			}
			if (n_sto) {
				error("staticCM declared%k",n_sto);
				Cdcl = odcl;
				return 0;
			}
			// explicit definition of this static class member
			if (cl->class_base == INSTANTIATED) n_redefined=1;
			tbl = etbl;
		}
	} // if n_qualifier
xdr:
	if (n_oper && tp->base!=FCT && n_sto!=OVERLOAD)
		error("operator%k not aF",n_oper);

	/*	if a storage class was specified
			check that it is legal in the scope 
		else
			provide default storage class
		some details must be left until the type of the object is known
	*/

	n_stclass = n_sto;
	n_scope = scope;	/* default scope & storage class */

	switch (n_sto) {
	default:
		error('i',"unX %k",n_sto);
	case FRIEND:
	{
		Pclass cl = cc->cot;

		switch (scope) {
		case 0:
		case PUBLIC:
			break;
		default:
			error("friend%n not inCD(%k)",this,scope);
			base = 0;
			Cdcl = odcl;
			return 0;
		}

		switch (n_oper) {
		case 0:
		case NEW:
		case DELETE:
		case CTOR:
		case DTOR:
		case TYPE:
			n_sto = 0;
			break;
		default:
			n_sto = OVERLOAD;
		}

//error('d',&where,"dcl friend%n tx%t -%k",this,tx,tx->base);
//error('d',&where,"    n_oper%k n_sto%k scope%k",n_oper,n_sto,scope);
		switch (tx->base) {
		case COBJ:
			nn = Pbase(tx)->b_name;
			break;
		case CLASS:
			nn = this;
			break;
		case FCT:
			Pfct(tx)->def_context = cc->cot;
			cc->stack();
			cc->not = 0;
			cc->tot = 0;
			cc->cot = 0;
			friend_in_class++;
			n_sto = 0;
			//XXXXX should enter in next enclosing scope
			lex_level = 0;
			nn = dcl(gtbl,EXTERN);
			if (nn == 0) {
				Cdcl = odcl;
				return 0;
			}
			friend_in_class--;
			cc->unstack();
			if (nn->tp->base == OVERLOAD)
				nn = Pgen(nn->tp)->find(Pfct(tx),1);
			break;
		default:
			error("badT%t of friend%n",tp,this);
			Cdcl = odcl;
			return 0;
		}

		PERM(nn);
		cl->friend_list = new name_list(nn,cl->friend_list);
		Cdcl = odcl;
		return nn;
	}

	case OVERLOAD:
		error(strict_opt?0:'w',"`overload' used (anachronism)");
		n_sto = 0;
		switch (tp->base) {		// ignore overload!
		case FCT:
			break;
		default:
			base = 0;
			Cdcl = odcl;
			return this;
		}
		break;

	case REGISTER:
		if (tp->base == FCT) {
			error('w',"%n: register (ignored)",this);
			goto ddd;
		}
		// no break
	case AUTO:
		switch (scope) {
		case 0:
		case PUBLIC:
		case EXTERN:
			error("%k not inF",n_sto);
			goto ddd;
		}
		if (n_sto==AUTO)		// always redundant
			n_sto = 0;
		break;

	case EXTERN:
		switch (scope) {
		case 0:
		case PUBLIC:
			/* extern is provided as a default for functions without body */
			if (tp->base != FCT)
				error("externM%n",this);
			// no break
		case ARG:
			goto ddd;

		case FCT:
		{
			Pname nn = gtbl->look( string, 0 );
			tp->dcl(tbl);
			if (
				nn
				&&
				tp->base != FCT
				&&
				tp->check(nn->tp,0)
			) {
				error("twoDs of%n;Ts:%t and%t",this,nn->tp,tp);
				Cdcl = odcl;
				return 0;
			}
		}
		}
		n_stclass = STATIC;
		n_scope = EXTERN;	/* avoid FCT scoped externs to allow better checking */
		break;

	case STATIC:
		switch (scope) {
		case ARG:
			goto ddd;
		case 0:
		case PUBLIC:
			n_stclass = STATIC;
			n_scope = scope;
			break;
		default:
			n_scope = STATIC;
		}
		break;

	case 0:
	ddd:
		switch (scope) {	/* default storage classes */
		case EXTERN:
			n_scope = EXTERN;
			n_stclass = STATIC;
			break;
		case FCT:
			if (tp->base == FCT) {
				n_stclass = STATIC;
				n_scope = EXTERN;
			}
			else
				n_stclass = AUTO;
			break;
		case ARG:
			n_stclass = AUTO;
			break;
		case 0:
		case PUBLIC:
			n_stclass = 0;
			break;
		}
	}

	
	/*
		now insert the name into the appropriate symbol table,
		and compare types with previous declarations of that name

		do type dependent adjustments of the scope
	*/

	static int warn_ldouble=0;

	switch (tx->base) {
	case ASM:
	{
		Pbase b = Pbase(tp);
		Pname n = tbl->insert(this,0);
		n->assign();
		n->use();
		char* s = (char*) b->b_name;	// save asm string. Shoddy
		int ll = strlen(s);
		char* s2 = new char[ll+1];
		strcpy(s2,s);
		b->b_name = Pname(s2);
		Cdcl = odcl;
		return this;
	}
	case CLASS: 
		if ( n_key == REF ) { // class x;
//			Pclass cl = Pclass(tx);
			nn = tbl->insert(this,CLASS); // copy for member lookup
			n_key = REF; // changed by table::insert()
//			if ( ansi_opt && (cl->defined&(DEFINED))==0) { // "class x;"
//			    char* su = (cl->csu==UNION || cl->csu==ANON) ? "union" : "struct";
//			    if ( cl->nested_sig )
//				fprintf(out_file,"%s __%s;\n",su,cl->nested_sig);
//			    else if ( cl->lex_level == 0 )
//				fprintf(out_file,"%s %s;\n",su,cl->string);
//			    else
//				fprintf(out_file,"%s %s;\n",su,cl->local_sig);
//			}
		}
		else
		{
			if ( tx->in_class && ( tx->templ_base==BOUND_TEMPLATE || tx->templ_base==CL_TEMPLATE ))
			{
				delete tx->nested_sig;
				tx->nested_sig = make_nested_name(Pclass(tx)->string,tx->in_class);
			}
			tp = tx;
			nn = dclass(this,tbl);
		}
		Cdcl = odcl;
		return nn;
	case ENUM:
		if ( tx->in_class && ( tx->templ_base==BOUND_TEMPLATE || tx->templ_base==CL_TEMPLATE ))
		{
			delete tx->nested_sig;
			tx->nested_sig = make_nested_name(Penum(tx)->string,tx->in_class);
		}
		tp = tx;
		nn = denum(this,tbl);
		Cdcl = odcl;
		return nn;
	case FCT:
		tp = tx;
		nn = dofct(tbl,scope);
		if (nn == 0) {
			Cdcl = odcl;
			return 0;
		}

		if (pdef_name == 0)
			check_def_name(nn, scope);
		break;

	case FIELD:
		switch (n_stclass) {
		case 0:
		case PUBLIC:
			break;
		default:
			error("%k field",n_stclass);
			n_stclass = 0;
		}

		if (
			cc->not==0
			||
			(cc->cot->csu==UNION && !ansi_opt)
			||
			cc->cot->csu==ANON
		) {
			if (cc->not)
				error('s', "bit-field as member of union");
			else
				error("bit-field not inC");
			PERM(tp);
			Cdcl = odcl;
			return this;
		}

		if (string) {
			nn = tbl->insert(this,0);
			n_table = nn->n_table;
			if (Nold)
				error("twoDs of field%n",this);
		}

		tp->dcl(tbl);
		field_align();
		break;
	
	case COBJ:
	{
		Pclass cl = Pclass(Pbase(tx)->b_name->tp);
//error('d',"%n %d cl%t %d",this,lex_level,cl,cl->lex_level);
//		if (cl->lex_level > lex_level) //SYM now obsolete 
//			error('i',"'%n'->dcl(%d,%k) C%t is not visible in this scope",this,tbl,scope,cl);

		if (cl->csu == ANON) export_anon( this, cl, tbl );

		if (cl->c_abstract) {
			if (cl->string[0]=='_' && cl->string[1]=='_' && cl->string[2]=='C')

				error('e',"D ofO of abstractC - pure virtual function(s) ");
			else 
				error('e',"D ofO of abstractC%t - pure virtual function(s) ",cl);
			for (Pbcl bcl=cl->baselist;bcl;bcl=bcl->next) 
				for (Pvirt n=bcl->bclass->virt_list;n;n=n->next) {
					velem* ivec=n->virt_init;
					Pname vn;
					for (int i=0;vn=ivec[i].n;i++) {
						Pname n=cl->memtbl->look(vn->string,0);
						if (vn->n_initializer && (n==0 || n->base==PUBLIC))
							error('c',"%n ",vn);
					}
				}
			error('c',"have not been defined\n");
		}
		goto cde;
	}

	case VOID:
		if (n_scope != ARG) {
			error("badBT:%k%n",tx->base,this);
			Cdcl = odcl;
			return 0;
		}
		break;

	case LDOUBLE:
		if (warn_ldouble==0 && ansi_opt==0) {
			++warn_ldouble;
			error('w',"long double supported under ``+a1'' option only, generating ``double%n''", this);
		}
		goto cde;

	case PTR:
//		if (ansi_opt && scope==ARG && Pptr(tx)->typ && Pptr(tx)->typ->base==COBJ)
//		{
//			Pclass cl=Pclass(Pbase(Pptr(tx)->typ)->b_name->tp);
//			if (ansi_opt && (cl->defined&(DEFINED))==0) {
//				char* s = cl->csu==UNION || cl->csu==ANON ? "union" : "struct";
//				if ( cl->nested_sig )
//					fprintf(out_file,"%s __%s;\n",s,cl->nested_sig);
//				else
//					fprintf(out_file,"%s %s;\n",s,cl->local_sig?cl->local_sig:cl->string);
//			}
//		}
	case VEC:
	case RPTR:
		tp->dcl(tbl);

	default:
	cde:
		nn = tbl->insert(this,0);
		n_table = nn->n_table;
		nn->n_redefined = n_redefined;

		if (Nold) {
			if ( nn->base == TNAME && base == NAME ) {
			    if ( nn->tpdef && nn->tpdef->in_class && nn->tpdef->in_class->csu==ANON )
				error("twoDs of %s (one in anonymous union)",nn->string);
			    else
				error("%n declared as identifier andTdef",nn);
			    // avoid later errors ...
			    nn->n_key = HIDDEN;
			    n_table = 0;
			    goto cde;
			}
			if ( nn->base == PUBLIC ) {		// X::i
				error("twoDs ofCM%n", nn);
				Cdcl = odcl;
				return 0;
			}

			if (nn->tp->base == ANY)
				goto zzz;

			if ( tp->check(nn->tp,0) ) {
				if ( nn->base != TNAME )//SYM
				    error("twoDs of%n;%t and%t",nn,nn->tp,tp);
				Cdcl = odcl;
				return 0;
			}
			if (n_sto && n_sto!=nn->n_scope) {
				if (n_sto==EXTERN && nn->n_scope==STATIC) {
					error('w',"%n declared extern after being declared static",this);
					goto ext_fudge;
				}
				else
					error("%n declared as both%k and%k",this,n_sto,(nn->n_sto)?nn->n_sto:EXTERN);
			}
			else if (nn->n_scope==STATIC && n_scope==EXTERN) {
				error('w',"static%n followed by definition",this);
			ext_fudge:
				if (n_initializer) {
					n_initializer = 0;
				}
				n_sto = EXTERN;
			}
			else if (nn->n_sto==STATIC && n_sto==STATIC ) 
				error("static%n declared twice",this);
			else {
				if (
					n_sto==0
					&&
					nn->n_sto==EXTERN
					&&
					n_initializer
					&&
					tp->tconst()
				)
				    if ( vec_const==0 )
					n_sto = EXTERN;

				n_scope = nn->n_scope;

				switch (scope) {
				case FCT:
					if (n_sto != EXTERN) {
						error("twoDs of%n",this);
						Cdcl = odcl;
						return 0;
					}
					break;
				case ARG:
					error("twoAs%n",this);
					Cdcl = odcl;
					return 0;
				case 0:
				case PUBLIC:
					error("twoDs ofM%n",this);
					Cdcl = odcl;
					return 0;
				case EXTERN:
					if (n_sto==0 ||
					   (n_sto==EXTERN && n_initializer)) {
						switch(nn->n_sto) {
						case 0:
							error("two definitions of%n",this);
							Cdcl = odcl;
							return 0;
						case EXTERN:
							if (n_sto == 0 &&
							   nn->n_initializer) {
								error("two definitions of%n",this);
								Cdcl = odcl;
							}
							else
								nn->n_sto=0;

							if (nn->n_stclass == STATIC
						    	&& (nn->n_scope == PUBLIC 
							   || nn->n_scope == 0)) {
							   if (tp->skiptypedefs()->base==VEC) {
								Ptype atp = nn->tp->skiptypedefs();
								if (atp && atp->base==VEC)
								   ((Pvec)atp)->typ->tsizeof();
							   } else
								nn->tp->tsizeof();	// check that size is known
							}
							break;
						}
					}
					break;
				}
			}
			n_scope = nn->n_scope;
/* n_val */
			if (n_initializer) {
				if (nn->n_initializer || nn->n_val)
					error("twoIrs for%n",this);
				nn->n_initializer = n_initializer;
			}
			if (tp->base == VEC) {		// handle:	extern v[]; v[200];
							// and		extern u[10]; u[11];
				Ptype ntp = nn->tp->skiptypedefs();

				if (Pvec(ntp)->dim == 0)
					Pvec(ntp)->dim = Pvec(tp)->dim;
				if (Pvec(ntp)->size) {
					if (Pvec(tp)->size && Pvec(ntp)->size!=Pvec(tp)->size)
						error("bad array size for%n: %d %dX",this,Pvec(tp)->size,Pvec(ntp)->size);
				}
				else
					Pvec(ntp)->size = Pvec(tp)->size;
			}
		}
		else {
			if (
				scope!=ARG
				&&
				n_sto!=EXTERN
				&&
				(
					n_sto!=STATIC
					||
					scope!=0
					&&
					scope!=PUBLIC
				)				// static member
				&&
				n_initializer==0
				&&
				tp->skiptypedefs()->base==VEC
				&&
				Pvec(tp->skiptypedefs())->size==0
			)
				if (Pvec(tp)->dim==0)
					error(&where,"dimension missing for array%n",this);

			if (
				scope==EXTERN
				&&
				n_sto==0
				&&
				tp->is_const_object()
				)
					nn->n_sto = n_sto = STATIC;
			}
		
		zzz:
			if (base != TNAME) {
				Ptype t = nn->tp;
	
				if (t->base == TYPE) {
					Ptype tt = Pbase(t)->b_name->tp;
					if (tt->base == FCT)
						nn->tp = t = tt;
				}
	
				switch (t->base) {
				case FCT:
				case OVERLOAD:
					break;
				default:
					fake_sizeof = 1;
					switch (nn->n_stclass) {
					default:
						if (nn->n_scope != ARG) {
							int x = t->align();
							int y = t->tsizeof();
	
							if (max_align < x)
								max_align = x;
	
							while (0 < bit_offset) {
								byte_offset++;
								bit_offset -= BI_IN_BYTE;
							}
							bit_offset = 0;
	
							if (byte_offset && 1<x)
								byte_offset = ((byte_offset-1)/x)*x+x;
							nn->n_offset = byte_offset;
							byte_offset += y;
						}
						break;
					case STATIC:
						if ( n_sto != EXTERN ) {
						    if ( nn->n_scope
						    &&   nn->n_scope!=PUBLIC )
							t->tsizeof();	// check that size is known
						    else // is this a static mem def?
						    if ( tbl->t_name==0 || tbl==gtbl
						    ||  !same_class(Pclass(tbl->t_name->tp),Pclass(nn->n_table->t_name->tp))
						    )
							t->tsizeof();	// check that size is known
						}
					}
					fake_sizeof = 0;
				}
			}
	
		{	Ptype t = nn->tp;
			int const_old = const_save;
			bit vec_seen = 0;
			Pexpr init = n_initializer;
	
			bit td_const = 0;
		lll:
			switch (t->base) {
			case COBJ:
			{
				Pname cn = Pbase(t)->b_name;
				Pclass cl = (Pclass)cn->tp;
				Pname ctor = cl->has_ctor();
				Pname dtor = cl->has_dtor();
				int stct = 0;
				if (dtor) {
					Pstmt dls;
	
					if (!check_static_pt(nn)) {
						nn->assign();
						nn->use();
						goto ggg;
					}

					// if dtor is not public check scope of class object
					if (dtor->n_scope != PUBLIC) {
						switch (nn->n_scope) {
						case ARG:
						case 0:
						case PUBLIC:
							break;
						default: 
							check_visibility( dtor, 0, cl, tbl, cc->nof );
						}
					}
	
					switch ( nn->n_scope ) {
					case 0:
					case PUBLIC:
						if (n_stclass==STATIC) {	//III
							Pclass cl = Pclass(nn->n_table->t_name->tp);
							if (cl->defined&DEFINED)
								goto dtdt;
						}
						break;
					case EXTERN:
						if (init==0 && n_sto==EXTERN)
							break;
	
					case STATIC:
					{
						Pexpr c;
					dtdt:
	// local static class objects have destructors set up in simpl2.c
	// special case: temporary class object generated in init expression
						if (stat_init && scope == ARGS ) {
							nn->n_scope = ARGS;
							goto ggg;
	 					}
	
						if ( nn->lex_level && nn->n_sto == STATIC ) {
							if (ctor==0) 
								error('s',"local static%n has%n but noK(add%n::%n())", nn, dtor, cn, cn );
							goto static_init;
						}
	
						Ptable otbl = tbl;
						// to collect temporaries generated
						// in static destructors where we
						// can find them again (in std_tbl)
						if (std_tbl == 0)
							std_tbl = new table(8,gtbl,0);
						tbl = std_tbl;
						if (vec_seen) {
							c = cdvec(vec_del_fct,nn,cl,dtor,0,zero);
						}
						else {			// nn->cl::~cl(0);
							c = call_dtor(nn,dtor,0,DOT,one);
						}
						c->tp = any_type;	// avoid another check
						dls = new estmt(SM,nn->where,c,0);
						// destructors for statics are executed in reverse order
						if (st_dlist)
							dls->s_list = st_dlist;
						st_dlist = dls;
						tbl = otbl;
					}	// case STATIC
					}	// switch nn->n_scope
				}	// if dtor
	
	// local static class objects must defer setting up static dtor
	static_init:
				if (ctor) {
					Pexpr oo = nn;
					for (int vi=vec_seen; vi; vi--)
						oo = oo->contents();
					int sti = 0;
					if (!check_static_pt(nn)) {
						nn->assign();
						nn->use();
						goto ggg;
					}
					switch (nn->n_scope) {
					case EXTERN:
						if (init==0 && n_sto==EXTERN)
							goto ggg;
					case STATIC:
						if (tbl == gtbl)
							sti = 1;
						else
							stct = 1;
					default:
						if (vec_seen && init) {
							if (1<vec_seen)
								error('s',"Ir for multi-dimensional array%n ofOs ofC%tWK",this,cl);
							else {
								if (sti) {
									if (sti_tbl==0)
										sti_tbl = new table(8,gtbl,0);
									const_save = 1;
									(void) co_array_init(nn,sti_tbl);
									// preserve any new array dimensions for printing
									tp = nn->tp;
									const_save = 0;
									n_initializer = init = 0;
								}
								else {
									n_initializer = init = co_array_init(nn,tbl);
									// preserve any new array dimensions for printing
									tp = nn->tp;
									if (stct)
										nn->n_initializer =
										n_initializer =
										init =
										new expr( STAT_INIT, init, 0 );
								}
							}
							goto ggg;
						}
						break;
					case PUBLIC:
					case 0:
						if (n_stclass==STATIC) {	//III
							if (vec_seen && n_initializer) {
							    if (1<vec_seen)
								error('s',"Ir for multi-dimensional array%n ofOs ofC%tWK",this,cl);
							    else {
								if (sti_tbl==0)
									sti_tbl = new table(8,gtbl,0);
								const_save = 1;
								(void) co_array_init(nn,sti_tbl);
								// preserve any new array dimensions for printing
								tp = nn->tp;
								const_save = 0;
								n_initializer = init = 0;
							    }
							}
							else {
	
								Pclass cl = Pclass(nn->n_table->t_name->tp);
								if (cl->defined&DEFINED) {
									sti = 1;
									break;
								}
							}
						}
	
						if (vec_seen) {
							Pname c = cl->has_ictor();
							if (c == 0)
								error("array ofC%n that does not have aK taking noAs",cn);
							else {	// Check for ambiguous ctor calls
								Pname ctor = cl->has_ctor();
								if (ctor && (!tbl->t_name || !tbl->t_name->tp || tbl->t_name->tp->base != CLASS)) {
 									Pexpr e = call_ctor(tbl,0,ctor,0,REF);
								}
								if (Pfct(c->tp)->nargs)
									{ if (!cl->has_vtor()) cl->make_vec_ctor(c); }
							}
						}
						// no break
					case ARG:
						goto ggg;
					}
	
					const_save = 1;
					nn->assign();
					Ptable otbl = tbl;
					if (sti) {	// to collect temporaries generated
							// in static initializers where we
							// can find them again (in sti_tbl)
						if (sti_tbl == 0)
							sti_tbl = new table(8,gtbl,0);
						tbl = sti_tbl;
						if (n_sto == EXTERN)
							nn->n_sto = n_sto = 0;
					}
	
					if (init) {
						if (init->base==VALUE) {
							switch (init->tp2->base) {
							case CLASS:
								if (!same_class(Pclass(init->tp2),cl))
									goto inin;
								break;
							default:
								{
								Pname n2 = init->tp2->is_cl_obj();
								if (n2==0 || !same_class(Pclass(n2->tp),cl))
									goto inin;
								}
							}
	
							Pexpr ee = init->e1;
							if (ee && vec_seen==0) {
								switch (ee->base) {
								case CALL:	// T a = f();
									init = ee;
									goto inin;
								case ELIST:	// T a(f());
									if (
										ee->e1->base==CALL
										&&
										ee->e2 == 0
									) {
										init = ee->e1;
										goto inin;
									}
								} // end switch
							}
	
							init->e2 = oo;
							init = init->typ(tbl);
	
							if (init->base == G_CM) {	// beware of type conversion operators
								switch (init->tp2->base) {
								case CLASS:
									if (!same_class(Pclass(init->tp2),cl))
										goto inin;
									break;
								default:
									{
									Pname n2 = init->tp2->is_cl_obj();
									if (n2==0 || !same_class(Pclass(n2->tp),cl))
										goto inin;
									}
								}
							}
						}
						else {
						inin:
							init = init->typ(tbl);
							if (
								init->base==G_CM
								&&
								nn->tp->check(init->tp,0)==0
							)
								(void) replace_temp(init,nn->address());
							else
								init = class_init(nn,nn->tp,init,tbl);
						}
					}
					else if (vec_seen == 0) {
						init = new texpr(VALUE,cl,0);
						init->e2 = oo;
						init = init->typ(tbl);
					}
	
					Pname c;
					if (vec_seen) {
						c = cl->has_ictor();
						if (c == 0)
							error("array ofC%n that does not have aK taking noAs",cn);
						else {	// Check for ambiguous ctor calls
							Pname ctor = cl->has_ctor();
							if (ctor && (!tbl->t_name || !tbl->t_name->tp || tbl->t_name->tp->base != CLASS)) {
 								Pexpr e = call_ctor(tbl,0,ctor,0,REF);
							}
							if (Pfct(c->tp)->nargs)
								{ if (!cl->has_vtor()) cl->make_vec_ctor(c); }
						}
					}
	
					if (stct) {
						if (tbl!=gtbl && nn->n_sto==EXTERN) {
							error(&where,"Id local extern%n",this);
							init = 0;
						}
						else if (init) {
						    if ( init->base==DEREF
						    && init->e1->base==G_CALL )
							init->base = STAT_INIT;
						    else {
							init = new expr(ASSIGN,nn,init);
							init->tp = init->e1->tp;
							init = new expr(STAT_INIT,init,0);
							init->tp = init->e1->tp;
						    }
						} else {
							if (tp->base == VEC && Pvec(tp)->size ) {
								if (1<vec_seen)
									error('s',"implicit staticIr for multi-dimensional array ofOs ofC%tWK",cl);
								else {
									Pexpr ilist = 0;
									for (int i=Pvec(tp)->size; i>0; i--) {
										Pexpr e = new texpr(VALUE,cl,0);
										ilist = new expr(ELIST, e, ilist);
									}
									nn->n_initializer=new expr(ILIST,ilist,0);
									init = co_array_init(nn,tbl);
									// preserve any new array dimensions for printing
									tp = nn->tp;
									nn->n_initializer =
										n_initializer =
										init =
										new expr( STAT_INIT, init, 0 );
								}
							}
							else
								error('s',"local staticC%n (%t)",this, tp);
					 	}
					}
	
					if (sti) {
						if (vec_seen) {		// _vec_new(vec,noe,sz,ctor);
							if ( n_stclass == STATIC && n_initializer ) {
								const_save = 1;
								(void) co_array_init(nn,sti_tbl);
								// preserve any new array dimensions for printing
								tp = nn->tp;
								const_save = 0;
								n_initializer = init = 0;
								goto ggg;
							}
							// vctor is stub ctor passed to vec_new for default args ctor
							Pname vctor = cl->has_vtor();
							init = cdvec(vec_new_fct,nn,cl,vctor?vctor:c,-1,0);
							init->tp = any_type;
						}
						else {
							switch (init->base) {
							case DEREF:		// *constructor?
								if (init->e1->base == G_CALL) {	
									Pname fn = init->e1->fct_name;
									if (fn==0 || fn->n_oper!=CTOR)
										goto as;
									init = init->e1;
									break;
								}
								goto as;
							case G_CM:
								init = init->e1;
									// suppress further type checking
								if (init->tp == 0)
									init->tp= any_type; 
								break;
							case ASSIGN:
								if (init->e1 == nn)
									break;	// simple assignment
							as:	
							default:	
								init = new expr(ASSIGN,nn,init);
							}
						}
						Pstmt ist = new estmt(SM,nn->where,init,0);
						// constructors for statics are executed in order
						if (st_ilist == 0)
							st_ilist = ist;
						else
							itail->s_list = ist;
						itail = ist;
						init = 0;	// suppress further processing
					} // if (sti)
					nn->n_initializer = n_initializer = init;
					const_save = const_old;
					tbl = otbl;
				}
				else if (init == 0)		// no initializer
					goto str;
				else if (cl->is_simple() && cl->csu!=ANON) {	// struct
					init = init->typ(tbl);
					if (
						nn->tp->check(init->tp,0)==0
						&&
						init->base==G_CM
					) 
						(void) replace_temp(init,nn->address());
					else goto str;
				}
				else if (init->base == ILIST) {		// class or union
					error("cannotI%nWIrL because class %s has private or protected members",nn,cl->string);
				}
				else {					// bitwise copy ok?
									// possible to get here?
					init = init->typ(tbl);
					if (nn->tp->check(init->tp,0)==0) {
						if (init->base==G_CM) 
							(void) replace_temp(init,nn->address());
						else
							goto str;
					}
					goto str;
				}
				break;
			}
	
			case VEC:	
				t = Pvec(t)->typ;
				vec_seen++;
				nn->assign();
				goto lll;
	
			case TYPE:
				if (init==0 && Pbase(t)->b_const) {
					switch (n_scope) {
					case ARG:
						break;
					case 0:
					case PUBLIC:
						if ( cc->cot )
							break;
					default:
					{
						Pname n = t->is_cl_obj();
						Pclass cl;
						if ( n )
							cl = Pclass( n->tp );
	
						if (
							n_sto!=EXTERN
							&& 
							(
								n==0
								||
								cl->has_ctor()==0
								&&
								is_empty(cl,1)==0
							)
						) 
							error("uninitialized const%n",this);
					}
					}
				}
				t = t->skiptypedefs(td_const);
				goto lll;
	
			case RPTR:
				if (init) {
					if (nn->n_scope == ARG)
						break;
					if (Pptr(nn->tp)->memof)
						error("R toCM%n ofT%t illegal",nn,nn->tp);
					ref_initializer++;
					init = init->typ(tbl);
					ref_initializer--;
					Nvirt = 0;			// set within ref_init() call
					// No longer catch initailizer for static ref
					// if (
						//n_sto==STATIC
						//&&
						//init->lval(0)==0
						//&&
						//fct_const==0
					//)
						//error("Ir for staticR%n not an lvalue",this);
					//else
					nn->n_initializer =
						n_initializer = 
						init =
						ref_init(Pptr(t),init,tbl);
	
					if (Nvirt == VIRTUAL)
						sti_vb = 1;
					nn->assign();
	
					if (init->base==ILIST && init->e2==0) {
						new_list(init);
						list_check(nn,nn->tp,0,tbl);
						if (next_elem())
							error(&where,"IrL too long");
					}
	
				}
				else {
					switch (nn->n_scope) {
					default:
						if (n_sto != EXTERN)
							error("uninitializedR%n",this);
					case ARG:
						break;
					case PUBLIC:
					case 0:
						if ( cc->cot == 0 )
							error("uninitializedR%n",this);
						break;
					}
				}
				goto stgg;
			default:
			str:
				if (init == 0) {
					switch (n_scope) {
					case ARG:
						break;
					case 0:
					case PUBLIC:
						if ( cc->cot )
							break;
					default:
						if (n_sto!=EXTERN && t->tconst())
							error("uninitialized const%n",this);
					}
	
					break;
				}
	
				const_save =
						const_save
						||
						n_scope==ARG
						||
						(t->tconst() && vec_const==0)
						;
	
				if (n_sto==STATIC)
					stat_init++;
				nn->n_initializer = n_initializer = init = init->typ(tbl);
				if (n_sto==STATIC)
					stat_init--;
	
				if (const_save || td_const)
					PERM(init);
				nn->assign();
				const_save = const_old;
				switch (init->base) {
				case ILIST:
					if (init->e2)
						goto dfdf;		// pointer to member
					new_list(init);
					list_check(nn,nn->tp,0,tbl);
					if (next_elem())
						error(&where,"IrL too long");
					break;
				case STRING:
				{
					Ptype v = nn->tp->skiptypedefs();
					if (v->base==VEC) {
						Ptype vv = Pvec(v)->typ->skiptypedefs();
						if(vv->base==CHAR) {
							int sz = Pvec(v)->size;
							int isz = Pvec(init->tp)->size;
							if (sz == 0)
								Pvec(v)->size = isz;
							else if (sz < isz)
								error(&where,"Ir too long (%d characters) for%n[%d]",isz,nn,sz);
							break;
						}
					}
					// no break
				}
				default:
				dfdf:
				{
					bit ntc = 0;
					Ptype nt = nn->tp->skiptypedefs(ntc);
	
					if (vec_seen) {
						error("badIr for array%n",nn);
						break;
					}
	
					Ptype it = init->tp->skiptypedefs();
					switch (nt->base) {
					case INT:
					case CHAR:
					case SHORT:
					case EOBJ:
						switch (it->base) {
						case LONG:
						case FLOAT:
						case DOUBLE:
						case LDOUBLE:
							error('w',"%tIdW%t",nt,init->tp);
						}
						// no break
					case LONG:
					{
						if (
							Pbase(nt)->b_unsigned
							&&
							init->base==UMINUS
							&&
							init->e2->base==ICON
						)
							error('w',"negativeIr for unsigned%n",nn);
	
						if ( nt->base != EOBJ ) {
						    switch ( it->base ) {
						    case SHORT:
						    case INT:
						    case LONG:
							if ( nt->tsizeof() < it->tsizeof() )
								init = new texpr(G_CAST,nt,init)->typ(tbl);
						    }
						}
	
	
						Neval = 0;
						long i = init->eval();
						if (Neval == 0 && ntc && scope!=ARG) {
							nn->n_evaluated = n_evaluated = 1;
							nn->n_val = n_val = i;
							nn->n_initializer = n_initializer = 0;
						}
						break;
					}
	
					case PTR:
						Nvirt = 0;
						nn->n_initializer =
							n_initializer =
							init =
							ptr_init(Pptr(nt),init,tbl);
						if (Nvirt == VIRTUAL)
							sti_vb = 1;
						if (Pchecked)
							goto stgg;
					} 
	
					{
						Pexpr x = try_to_coerce(nt,init,"initializer",tbl);
						if (x) {
							nn->n_initializer = n_initializer = init = x;
							goto stgg;
						}
					}
					int ct = no_const;
					Pname c1 = nt->is_cl_obj();
					Pname c2 = init->tp->is_cl_obj();
					if (
						c1
						&&
						c2
						&&
						Pclass(c2->tp)->has_base(Pclass(c1->tp))
					) {
						init = new texpr(G_CAST,new ptr(PTR,nt),init->address());
						init = init->typ(tbl);
						nn->n_initializer =
							n_initializer =
							init =
							init->contents();
						goto stgg;
					}
	
					if (init->tp->is_or_pts_to(OVERLOAD))
						error("badIrT &overloaded function for%n (%tX)",this,nn->tp);
					else if (nt->check(init->tp,ASSIGN)) {
						error('e',"badIrT%t for%n (%tX)",init->tp,this,nn->tp);
						if (ct) error('c'," (no usable const conversion)\n");
						else error('c',"\n");
						break;
					}
				} // dfdf block inside default
	
				stgg:
	
					if (init && n_stclass==STATIC && (sti_vb || (need_sti(init) && check_static_pt(nn)))) {
						/* check if non-static variables are used */
						int local = (0<lex_level);
						if (local==0)
							need_sti(init,tbl);	// save consts
		
						Pptr r = nn->tp->is_ref();	//III
		
						if (r && !init->tp->is_ptr_or_ref())
							init = init->address();
						init = new expr(ASSIGN,nn,init);
						if (r)
							init->tp = nn->tp;
						else if (nn->tp!=init->tp) { // static member refs
							TOK t = nn->tp->set_const(0);	//JJJ
							init = init->typ(tbl);
							nn->tp->set_const(t);		//JJJ
						}

						if (dtpt_opt && nn->dinst_body()==0) {
							nn->n_initializer=n_initializer=init=0;
							break;
						}
		
						if (local) {
							if (init->base != ASSIGN)
								error('s',"Ir for local static too complicated");
							if (nn->n_sto == EXTERN) {
								error(&where,"Id local extern%n",this);
								init = 0;
							}
							else
								init->base = STAT_INIT;
							nn->n_initializer = n_initializer = init;
						}
						else {
							Pstmt ist = new estmt(SM,nn->where,init,0);
							// constructors for statics are executed in order
				
							if (st_ilist == 0)
								st_ilist = ist;
							else
								itail->s_list = ist;
							itail = ist;
							nn->n_initializer =
								n_initializer =
								init =
								0;	// suppress further processing
							nn->n_val = n_val = 1;
						}
					}
				} /* switch */
			} /* block */
		} /* default */

	} /* switch */
ggg:
	PERM(nn);
	switch (n_scope) {
	case FCT:
		nn->n_initializer = n_initializer;
		break;
	default:
	{	Ptype t = nn->tp;
	px:
		PERM(t);
		switch (t->base) {
		case PTR:
		case RPTR:
		case VEC:	t = Pptr(t)->typ; goto px;
		case TYPE:	t = Pbase(t)->b_name->tp; goto px;
		case FCT:	t = Pfct(t)->returns; goto px; /* args? */
		} 
	}
	}
	
	Cdcl = odcl;
	return nn;
}


char*
make_nested_name(char *s, Pclass cl)
{ // Q<cnt>_<class_names><space><null>
	const int nested_depth = 9;
    	char *str_arr[nested_depth];
    	int  size_arr[nested_depth];
    	int cnt = 1;
    	int size = 4; // Q,<cnt>,<_>,<null>

	str_arr[0] = s;
	size += size_arr[0] = strlen(s); 

	for (Pclass nc = cl; nc && nc->lex_level==cl->lex_level; nc = nc->in_class ) {
		if (cnt > nested_depth-1) error('s',"nested depth class beyond %d unsupported",nested_depth);
		size += size_arr[cnt] = nc->c_strlen;
		str_arr[cnt] = nc->local_sig?nc->local_sig:nc->string;
		++cnt;
	}

	for ( int i=0; i<cnt; i++ ) // <nnn><string>
		size += size_arr[i]>99?3:size_arr[i]<10?1:2;

// error('d', "make_nested_name( %s, %t ) cnt: %d size: %d", s, cl, cnt, size );
	char *result = new char[size];
	sprintf(result, "Q%d_", cnt );
	size = 3;
	for ( i=cnt; i; i-- ) {
		sprintf(result+size,"%d%s", size_arr[i-1], str_arr[i-1]);
		size += size_arr[i-1] + (size_arr[i-1]>99?3:size_arr[i-1]<10?1:2);
	}

//error('d', "size: %d ", size );
	result[size] = '\0';
// error('d', "make_nested_name result: %s", result );
    	return result;
}
