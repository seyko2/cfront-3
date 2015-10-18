/*ident	"@(#)cls4:src/expr.c	1.28" */
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

expr.c:

	type check expressions

************************************************************************/

#include "cfront.h"
#include "size.h"

int const_save;
extern int no_const;
extern Ptype Pfct_type;
extern Pexpr make_dot(Pexpr, Ptable, char* c = "i");

int processing_sizeof = 0;

Pexpr expr::address()
{
// error('d',"address %k %d %s",base,base,base==NAME||base==ANAME?string:"?");
// error('d',"address e1 %d %k e2 %d %k", e1, e1?e1->base:0, e2, e2?e2->base:0); 
	switch (base) {
	case DEREF:
		if (e2 == 0) return e1;			// &*e	=> e
		break;
	case QUEST:					// &(a?b:c) => a?&b:&c
		e1 = e1->address();
		// no break;
	case G_CM:
		if (e1 && (e1->base == G_CALL || e1->base == CALL) &&
			e2 && e2->base == G_ADDROF)
			return this;
	case CM:
		if (e2 && e2->base == G_CALL) {  // (x,f())=>(x,(tmp=f(),&tmp))
                        Pname tmp = make_tmp('Q',e2->tp,cc->ftbl);
			e2 = init_tmp(tmp,e2,cc->ftbl);
			Pexpr aa = tmp->address();
			e2 = new expr(G_CM,e2,aa);
			e2->tp = aa->tp;
		}
		else e2 = e2->address();		// &(e1,e2) => (e1,&e2)
		tp = e2->tp;	//LLL
		return this;
	case INCR:
	case DECR:					// &(++a) => (++a,&a)
		if (e1) break;
		nin++;
		if (e2->not_simple()) error('s',"& of%k",base);
		nin--;
		e1 = new expr(base,0,e2);
		e2 = e2->address();
		base = G_CM;
		tp = e2->tp;
		return this;
	case ASSIGN:					// &(a=b) => ((a=b),&a)
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
		{
		nin++;
		if (e1->not_simple()) error('s',"& of%k",base);
		nin--;
		Pexpr a = new expr(base,e1,e2);
		a->tp = a->e1->tp;
		base = G_CM;
		e1 = a;
		e2 = a->e1->address();
		tp = e2->tp;
		return this;
		}
	case NAME:
		Pname(this)->take_addr();
		if (Pname(this)->n_xref)
			// function argument of class type becomes
			// class* due to user-defined copy ctor
			return this;
		break;
	case CALL:
	case CAST:
	case G_CAST:
	case NEW:
	case GNEW:
		if (tp && tp->is_ptr_or_ref()) {	// hack?
			return this;
		}
		break;
	}

	register Pexpr ee = new expr(G_ADDROF,0,this);
	if (tp) {					// tp==0 ???
		ee->tp = tp->addrof();

		switch (tp->base) {
		case PTR:
			Pptr(ee->tp)->memof = Pptr(tp)->memof;
			break;
		case FCT:
			if (Pfct(tp)->f_static==0)
			Pptr(ee->tp)->memof = Pfct(tp)->memof;
			break;
		case OVERLOAD:
			if (Pfct(Pgen(tp)->fct_list->f->tp)->f_static==0)
			Pptr(ee->tp)->memof = Pfct(Pgen(tp)->fct_list->f->tp)->memof;
		}
	}

	return ee;
}

Pexpr make_dot(Pexpr e, Ptable tbl, char* c)
{
	if(!e->tp->memptr())
		return e;

	if(e->base==CM || e->base==G_CALL || e->base==CALL || e->base==ASSIGN) {
		Pname atmp = make_tmp('A',e->tp,tbl);
		Pexpr as = init_tmp(atmp,e,tbl);
		e = new mdot(c,atmp);
		e->i1 = 9;
		e = new expr(G_CM,as,e);
	}
	else if(e->base == ILIST) {
		if (c[0]=='i') e = e->e1->e2;
		if (c[0]=='f') e = e->e2;
	}
	else {
		e = new mdot(c,e);
		e->i1 = 9;
	}
	return e;
}

Pexpr expr::contents()
{
//error('d',"deref %k %d %t",base,base,tp);
	switch (base) {
	case ADDROF:
	case G_ADDROF:
		return e2;		// *&
	case ELIST:
//error('d',"contents of  elist");
		e1 = e1->contents();
		tp = e1->tp;
		return this;
	};

	register Pexpr ee = new expr(DEREF,this,0);
	if (tp) {			// tp==0 ???
		Ptype tt = tp->skiptypedefs();
		ee->tp = Pptr(tt)->typ;
		Pname cn = ee->tp->is_cl_obj();
		if (cn) {
			// look for first use of class
			Pclass cl = Pclass(cn->tp);
			if (cl->c_body==1) cl->dcl_print(0);	
		}
	}
	return ee;
}

#if 0
static Pexpr 
make_postfix( Pexpr op, Pname fn=0 )
{
/* apply the postfix form of increment/decrement operator
 * if (fn) special call from check_postscipt to handle
 * the two operator instances declared at different scopes
 * i.e., member and non-member, therefore stored as FCTs.
 */
	Pfct f;
	Plist fl;
	Pname n = fn?fn:0; 
	Pexpr e = op->e1->base==NAME?op->e1:op->e1->mem;
	if (n) goto mk_postfix; // sorry(!!!)

	for (fl=Pgen(e->tp)->fct_list;fl; fl=fl->l) {
		n = fl->f;
		f = Pfct(n->tp);
		if ((f->nargs==1 && f->f_this) ||
		    (f->nargs==2 && f->f_this==0)) {

mk_postfix: // rewrite the call expression
			if (op->e1->base == NAME) {
				op->e1 = n;	
				op->e2->e2 = new expr(ELIST,zero,0);
			} else {
				op->e1->mem = n;  
				op->e2 = new expr(ELIST,zero,0); 
			}

			op->fct_name = n; 
			n->dcl_print(0); 
			return op;
		}
	}
	error('w',"no postfix instance of%n, although overloaded",Pname(e));
	return op; // as it were
}

static Pexpr 
check_postfix( Pexpr op )
{
/* special case: member and non-member operator++/-- instances 
 * each stored as FCT, and so oper_overload() returns prefix instance
 * need to make an explicit check if postfix instance exists
 */
	int is_glob = op->e1->base==NAME ? 1: 0;
	Pname fn = is_glob ? Pname(op->e1) : Pname(op->e1->mem);

// error('d',"check_postfix: fn: %n is_glob: %d",fn,is_glob);

	if (is_glob == 0) { // found operator++/--() as member
		Pname n = gtbl->look(fn->string,0);
		Ptype arg_tp = op->e1->e1->tp;

		if (n==0) return op;
		if (n->tp->base == FCT) { // one global instance
			Pfct ff = Pfct(n->tp); 

// error('d',"check_postfix: global n:%n arg: %t act: %t",n,ff->argtype->tp,arg_tp);
extern int exact1(Pname,Ptype); // place in cfront.h

			if (ff->nargs != 2) return op;
			if (exact1(ff->argtype, arg_tp)==0) return op;
			return make_postfix(op,n);
		}
		if (n->tp->base == OVERLOAD) { // multiple global instances
			for (Plist fl=Pgen(n->tp)->fct_list; fl; fl=fl->l) {
				Pname nn = fl->f;
				Pfct ff = Pfct(nn->tp);
				if (ff->nargs != 2) continue;
				if (exact1(ff->argtype, arg_tp)==0) continue;
				return make_postfix(op,nn);
			}
			return op;
		}
	}
	else {  // examine class table for postfix member
		Pfct f = Pfct(fn->tp);
		Pptr p = f->argtype->tp->is_ptr_or_ref();
		Ptype t = p?p->typ:f->argtype->tp;
		Pname cn = t->is_cl_obj();
		if (cn == 0) error('i',"check_postfix: %n(%n %t),CTX",fn,f->argtype,t);

		Pname nn = Pclass(cn->tp)->memtbl->look(fn->string,0);
// error('d',"check_postfix: found: %n %t", nn, nn->tp);
		if (nn) return make_postfix(op,nn);
		return op;
	}
	error('i', "fall off end of check_postfix()");
	return 0;
}
#endif

int bound;
int chars_in_largest;	// no of characters in largest int

static bit 
ptr_is_template_formal(Pptr p) 
{
// error('d',"ptr_is_template_formal: %t",p);

	if (p->typ) {	
	if (p->typ->base != TYPE) return 0;
	Pname n = p->typ->bname();
	return n->is_template_arg();
	} else	
	return 0;	
}

Ptype common_base(Pclass cl1, Pclass cl2)
/*
	do cl1 and cl2 have one common base class?
*/
{
	if (cl1 == 0 || cl2 == 0) return 0;
	if (cl1->baselist == 0 || cl2->baselist == 0) return 0;
	Pbase answer = 0;

	for (Pbcl b = cl1->baselist; b; b=b->next) {
		if (b->ppp!=PUBLIC && !same_class(cc->cot,cl1)
		    && 
		    (cc->nof==0 || cl1->has_friend(Pfct(cc->nof->tp))==0) 
		    && 
		    (cc->cot==0 || cl1->has_friend(cc->cot)==0)
		)
			continue;
		Nvis = 0;
		if (cl2->has_base(b->bclass)) {
			if (Nvis) continue;
			if (answer) return 0;
			Pname bn = new name(b->bclass->string);
			bn->tp = b->bclass;
			answer = new basetype(COBJ,bn);
		}
	}
	return answer;
}

static Pexpr
do_qualifiers( Ptype t, Pexpr mem )
/*
**	saw ( (t*) p )->mem
**	if mem has unprocessed qualifiers, look up qualifier names
*/
{
	// qualifiers after . or -> processed after type of base object is known
	Pname m = Pname(mem);
	if ( m->base != NAME && m->base != DTOR
	||   m->n_qualifier == 0
	||   m->n_qualifier->base != MEMQ )
		return mem;
	Pname cn = t ? t->is_cl_obj() : 0;
	Pname tn = 0;
	Pname q = m->n_qualifier;
	m->n_qualifier = 0;
	if ( q->string == 0 ) { // ->:: 
		//error("scope qualification syntax");
		do {
			Pname qx = q->n_list;
			delete q;
			q = qx;
		} while ( q && q->string == 0 );
		if ( q == 0 ) { m->n_initializer = sta_name; return mem; }
		tn = k_find_name(q->string,Gtbl,HIDDEN);
	} else if ( cn )
		tn = k_find_name(q->string,Pclass(cn->tp)->k_tbl,HIDDEN);
	else
		tn = k_find_name(q->string,Ctbl,HIDDEN);

/*	// check if first qualifier is a base class of cn
**	if ( cn && tn ) {
**		Pclass cl = Pclass(cn->tp);
**		Pname qn = tn->is_cl_obj();
**		if (qn && !same_class(Pclass(qn->tp),cl)) {	// really a base?
**			Pclass qcl = Pclass(qn->tp);
**			Pclass bcl = cl->is_base(qcl->string);
**			if (bcl == 0 || !same_class(bcl,qcl)) {
**				error("(%n*)->%n:: --%n is not aBC of%n",cn,qn,qn,cn);
**				return mem;
**			}
**		}
**	}
*/
	Pname cx = 0;
	for(;;) {
		if ( tn == 0 ) {
		    error("%n :: --TN%n not found",q,q);
		    return mem;
		}
		cn = tn->tp->is_cl_obj();
		if ( cn == 0 ) {
			// if at the final qualifier and mem is
			//    a dtor, mem could be a basic type dtor,
			//    in which case the qualifier doesn't have 
			//    to be a class name 
			if ( q->n_list==0 && m->base==DTOR )
				m->n_qualifier = tn;
			else
				error("%n :: --%n is not aCN",tn,tn);
			return mem;
		}
		if ( cx ) {  // cx::cn::
			if ( Pclass(cx->tp)->has_base(Pclass(cn->tp)) )
				error("%n ::%n :: --%n not aM of%n",cx,cn,cn,cx);
		}
		cx = cn;
		{ Pname qx = q->n_list; delete q; q = qx; }
		if ( q == 0 ) break;
		if ( q->string == 0 ) { // ->X:: ::
			error("scope qualification syntax");
			return mem;
		}
		tn = k_find_member(q->string,Pclass(cn->tp),HIDDEN);
	}
	m->n_qualifier = tn;
	return mem;
}

static Pexpr
do_dtor( Ptype t, Pexpr mem)
{
	Pname m = Pname(mem);
	// m->base == DTOR
	// represents 
	//            p->something::~something 
	//            something::~something 
	//            p->~something
	// where t == type of *p or 0
	// fields of m (see dummy_dtor() in gram.y):
	//     X::~Y	m->n_qualifier==X m->n_dtag==Y
	//     int::~int	m->tp2==int_type m->tpdef==int_type
	//     X::~int	m->n_qualifier==X m->tpdef==int_type
	//     int::~Y	m->tp2==int_type m->n_dtag==Y
	// etc
	Pname q = m->n_qualifier;
	Pname d = m->n_dtag;
	Pname cn = (t ? t->is_cl_obj() : 0);
	DB(if(Edebug>=1){
		error('d',"do_dtor: t%t b%k cn%n",t,m->base,cn);
		error('d',"         q%n d%n tp2%t tpdef%t",q,d,m->tp2,m->tpdef);
		error('d',"         qtp%t dtp%t",q->tp,d->tp);
	});
	if ( cn ) { // cp->something::~something -- look up qualifier again
	    // q already looked up in do_qualifiers()
	    if ( d && d->string ) {
		Pclass cl = (q && q->tp->is_cl_obj())
				? q->tp->skiptypedefs()->classtype()
				: Pclass(cn->tp);
		Pname dx = k_find_name(d->string,cl->k_tbl,HIDDEN);
		if (dx == 0) { error("TN%n not found in%t",d,t); dx = q; }
		d = m->n_dtag = dx;
	    }
	}
	DB(if(Edebug>=1){
		error('d',"     new q%n%t d%n%t",q,q?q->tp:0,d,d?d->tp:0);
	});
	if ( q ) {
	    if ( d && d->tp->check(q->tp,0) ) {
		error("bad syntax for destructor call:N andQr do not match");
		q = d;
	    } else if ( m->tpdef && m->tpdef->check(q->tp,0) )
		error("bad syntax for destructor call:N andQr do not match");
	    if ( d && d->tp->is_cl_obj() ) {
	    real_dtor:
		m->base = NAME;
////		m->n_dtag = 0;		// Keep n_dtag for find_name if needed.
		m->tp = m->tpdef = m->tp2 = 0;
		m->string = oper_name(DTOR);
		m->n_oper = DTOR;
	    } else {
		if ( m->tpdef ) m->tp2 = m->tpdef;
		else m->tp2 = m->tpdef = q->tp;
		m->n_qualifier = m->n_dtag = 0;
	    }
	}
	else { // int::~something or p->~something
	    if ( d && m->tp2==0 && d->tp->is_cl_obj() ) {
		goto real_dtor;
	    }
	    if ( d && m->tp2 && d->tp->check(m->tp2,0) )
		error("bad syntax for destructor call:N andQr do not match");
	    if ( d ) { m->tp2 = m->tpdef = d->tp; m->n_dtag = 0; }
	}
	DB(if(Edebug>=1)error('d',"do_dtor: returning%k%t",mem->base,mem->tp););

	// If "t" is provided, check that the type of the object referenced
	// agrees with the type associated with the DTOR or that the DTOR
	// is an accessible base class of that object.
	if ( t ) {
	    if ( mem->tp2 ) {
		// i.int::~int() or some basic type.

		if ( t->check(mem->tp2,0) ) {
		    error("T mismatch for simpleT destructor:XO ofT%t",mem->tp2);
		}  /* if */
	    }  else  {
		if (cn) {
		    // The object referenced is a class object; the DTOR must be for
		    // the same object or at least a base class.
		    if ( d && t->check(d->tp,0) ) {
			// This was not an identical match.  Check if a DTOR of a
			// base class before complaining of an error.
			Pname dn = ( d->tp ? d->tp->is_cl_obj() : 0 );
			if ( cn && dn ) {
			    if ( ! q && ! Pclass(cn->tp)->has_base(Pclass(dn->tp)) ) {
				// The class of the DTOR is not a
				// base class of the referenced object.  The
				// "X is not a base class of Y" message will
				// be generated by find_name later in
				// expr::typ().
				error("bad syntax for destructor call");
			    }  /* if */
			} else {
			    error("T mismatch for destructor:XO ofT%t", d->tp);
			}  /* if */
		    }  /* if */
		} else {
		    error("T mismatch for destructor:XO ofT%t", d->tp);
		}  /* if */
	    }  /* if */
        }  /* if */
	return mem;
}

#define nppromote(b)	t=np_promote(b,r1,r2,t1,t2,1)
#define npcheck(b)	(void)np_promote(b,r1,r2,t1,t2,0)

Pname Ntmp_flag_list;
bit in_quest;
con_dtor *pdlist;

Pexpr expr::typ(Ptable tbl)
/*
	find the type of "this" and place it in tp;
	return the typechecked version of the expression:
	"tbl" provides the scope for the names in "this"
*/
{
	Pname n;
	Ptype t = 0;
	Ptype t1, t2;
	TOK b = base;
	TOK r1, r2;
	if (tbl->base != TABLE) error('i',"expr::typ(%d)",tbl->base);

  	DB( if(Tdebug>=1) {
  		error('d',"%d->expr::typ(%d) %k %t",this,tbl,b,tp);
  		display_expr(this);
  	});
// error('d',"%k->typ %n tp: %t", b,b==NAME?this:0,tp);
// error('d',"     e1 %d %k e1 %d %k",e1,e1?e1->base:0,e2,e2?e2->base:0);

	if (tp) {
		switch (b) {
		case NAME:
			break;
		case MDOT:
			mem = mem->typ(tbl);
		}	;
		return this;
	}

	switch (b) {		// is it a basic type
	case MDOT:
		error('i',"mdot %s",string2);
	case DUMMY:
		error("emptyE");
		tp = any_type;
	case DTOR: // dummy type destructor
	{
		Pexpr e = do_dtor(0,this);
		return e->base==NAME ? e->typ(tbl) : e;
	}

	case ZERO:
		tp = zero_type;
		return this;

	case IVAL:
		tp = int_type;
		return this;

	case ICON:
		/*	is it long?
			explicit long?
			decimal larger than largest signed int
			octal or hexadecimal larger than largest unsigned int
		 */
	{	int ll = strlen(string);
		switch (string[ll-1]) {
		case 'l':
		case 'L':
			switch (string[ll-2]) {
			case 'u':
			case 'U':
				string[ll-2] = 0;
				tp = ulong_type;
				goto cast_n_save;
			}
// FIXME: long long
		lng:
			tp = long_type;
			goto save;
		case 'u':
		case 'U':			// 1u => unsigned(1)
			switch (string[ll-2]) {
			case 'l':
			case 'L':
				string[ll-2] = 0;
		ulng:
				tp = ulong_type;
				goto cast_n_save;
			default:
				string[ll-1] = 0;
		labuint:
				tp = uint_type;
				goto cast_n_save;
			}
		}

		// no suffix - see if we can figure it out
		if  (string[0] == '0') {	// assume 8 bits in byte
			register int index = 1;
			switch (string[1]) {
			case 'x':
			case 'X':
				{
				while(string[++index]=='0') ;
				ll -= index;
				int HSZ = SZ_INT+SZ_INT;
				if(ll < HSZ) goto nrm;
				if(ll == HSZ)
					if(string[2]>='8') goto labuint;
					else goto nrm;
				if(SZ_INT==SZ_LONG) break;
				HSZ = SZ_LONG+SZ_LONG;
				if(ll < HSZ) goto lng;
				if(ll == HSZ)
					if(string[2]>='8') goto ulng;
					else goto lng;
				break;
				}
			default:   // OCTAL
				{
				register int IBITS = BI_IN_BYTE*SZ_INT;
				while(string[index]=='0') index++;
				register char x = string[index];
				int lbt = x=='1' ? 1 :
					( x=='2' || x=='3' ? 2 : 3 );
				int nbits = (ll-index-1)*3 + lbt;
				if(nbits < IBITS) goto nrm;
				if(nbits == IBITS) goto labuint;
				if(nbits < BI_IN_BYTE*SZ_LONG) goto lng;
				}
			}
			goto ulng;
		}
		else {  // DECIMAL
			if (ll<chars_in_largest) {
		nrm:
				tp = int_type;
				goto save;
			}
			if (ll>chars_in_largest) {
				if(SZ_INT==SZ_LONG || ll>2*chars_in_largest)
					goto ulng;
				goto lng;
			}
			// ll == chars_in_largest
			char* p = string;
			char* q = LARGEST_INT;
			do if (*p>*q) {
				if(SZ_INT==SZ_LONG) goto ulng;
				goto lng; 
			} while (*p++==*q++ && *p);
		}

		goto nrm;
	}
	case CCON:
		tp = c_strlen(string)<5 ? char_type : int_type;	// stored as 'a'
		goto save;

	case FCON:
	{	int ll = strlen(string);
		int last = string[ll-1];
		tp = double_type;
		if (last=='F' || last=='f') {
			tp = float_type;
			if (!ansi_opt) {
				string[ll-1] = 0;
				goto cast_n_save;
			}
		}
		else if (last=='L' || last=='l') {
			if (ansi_opt == 0) string[ll-1] = 0;
			tp = ldouble_type;
		}
		goto save;
	}
	case STRING:			// type of "as\tdf" is char[6]
					// c_strlen counts the terminating '\0'
	{	Pvec v = new vec(char_type,0);
		v->size = c_strlen(string);
		tp = v;
	}
	save:
		if (const_save) {	// "as\tdf" needs 7 chars for storage
			char* p = new char[strlen(string)+1];
			strcpy(p,string);
			string = p;
		}

		return this;

	cast_n_save:
		if (const_save) {	// "as\tdf" needs 7 chars for storage
			char* p = new char[strlen(string)+1];
			strcpy(p,string);
			string = p;
		}
		
		return new cast(tp,this);

	case THIS:
		delete this;
		if (cc->c_this) {
			cc->c_this->use();
			return cc->c_this;
		}
		error("``this'' used in nonC context");
		n = new name("this");
		n->tp = any_type;
		return tbl->insert(n,0); 

	case NAME:
	{	
		Pname q = Pname(this)->n_qualifier; // suppress virtual iff x::
		Pexpr ee = find_name(Pname(this),cc->cot,tbl,0,cc->nof);
		if (q && (ee->base==REF || ee->base==DOT))
			ee->n_initializer = Pexpr(q);

		if(
			cc->nof
			&&
			Pfct(cc->nof->tp)->f_const
			&&
			(ee->base==REF||ee->base==DOT)
			&&
			ee->tp
			&&
			ee->tp->skiptypedefs()->base!=FCT
			&&
			ee->tp->skiptypedefs()->base!=OVERLOAD
		)
			ee->tp = ee->tp->mkconst();

//error('d',"ee %k %t %n",ee->base,ee->tp,ee->base==NAME?ee:ee->base==REF?ee->mem:0);
		if (ee->tp->is_ref()) return ee->contents();

		if (ee->base==NAME && Pname(ee)->n_xref) {
		// fudge to handle X(X&) args
			ee = new expr(DEREF,ee,0);
			ee->tp = ee->e1->tp;	// !!
		}

		return ee;
	}

	case ADDROF:
		if (e2->base == THIS) {
			error("&this");
			break;
		}
	case G_ADDROF:	// handle lookup for &s::i
		if (e2->base == NAME) e2 = find_name(Pname(e2),cc->cot,tbl,ADDROF,cc->nof);

		if(
			cc->nof
			&&
			Pfct(cc->nof->tp)->f_const
			&&
			(e2->base==REF||e2->base==DOT)
			&&
			e2->tp
			&&
			e2->tp->skiptypedefs()->base!=FCT
			&&
			e2->tp->skiptypedefs()->base!=OVERLOAD
		)
			e2->tp = e2->tp->mkconst();

		if (e2->base==NAME && Pname(e2)->n_xref) {
			// fudge to handle X(X&) args
			e2 = new expr(DEREF,e2,0);
			e2->tp = e2->e1->tp;	// !!
		}
		if (e2->base==DOT) {	// &f().x  = > &(tmp=f(),&tmp)->x
			switch (e2->e1->base) {
			case CALL:
			case G_CALL:
				{
					Pcall c=Pcall(e2->e1);
					if (c && c->e1) {
						c->e1=c->e1->typ(tbl);
						if (c->e1->tp && Pfct(c->e1->tp)->returns->base==RPTR) break;
					}
				}
			case VALUE:
				error("& non-lvalue");
			}
		}
		break;

	case SIZEOF:
		if (tp2) {
			tp2->dcl(tbl);
			switch (tp2->base) {
			case VOID:
				error("sizeof(void)");
				break;
			case CLASS:
			{
				Pclass cl = Pclass(tp2);
				if (cl->c_body==1
				&& (cl->defined&(DEFINED|SIMPLIFIED)) == 0) 
					error('s',"class defined within sizeof");
			}
			}
	
			if (e1 && e1!=dummy) {
				e1 = e1->typ(tbl);
				DEL(e1);
				e1 = dummy;
			}
			Pptr r = tp2->is_ref();
			if (r) tp2 = r->typ;	// sizeof(T&)==sizeof(T)
		}
		else if (e1 == dummy) {
			error("sizeof emptyE");
			tp = any_type;
			return this;
		}
		else {
			++processing_sizeof;
			e1 = e1->typ(tbl);
			--processing_sizeof;
			tp2 = e1->tp;
			if(tp2->base == VEC) tp2->permanent = 1;
			if (e1->base==ILIST)	// PtoM
				e1 = dummy;
			else if (tp2 == char_type)	// sizeof ('a')
				e1 = dummy;
		}
		(void) tp2->tsizeof();
		if (tp2->base==VOID) error("sizeof(void)");
		tp = size_t_type;
		return this;

	case CAST:
	case G_CAST:
	{	Pexpr ee = docast(tbl);
		return ee->tp->is_ref() ? ee->contents() : ee;
	}

	case VALUE:
		//SYM obsolete table hack removed 
		return dovalue(tbl);

	case NEW:
	case GNEW:
		return donew(tbl);

	case DELETE:	// delete e1 OR delete[e2] e1
	case GDELETE:
	{	int i;
		//if (e1->base == ADDROF) error('w',"delete &E");
		e1 = e1->typ(tbl);
		if (e1->tp->skiptypedefs()->base == COBJ) {
			Pexpr x = try_to_coerce(Pvoid_type,e1,"argument",tbl);
			if (x) e1 = x;
		}
		i = e1->tp->num_ptr(DELETE);
		if (i != 'P') error("nonP deleted");
		if (e2) {
			e2 = e2->typ(tbl);
			e2->tp->integral(DELETE);
		}
		tp = void_type;
		return this;
	}

	case ILIST:	/* an ILIST is pointer to an ELIST */
		e1 = e1->typ(tbl);
		tp = any_type;
		return this;

	case ELIST:
	{	Pexpr e;
		Pexpr ex;

		if (e1 == dummy && e2==0) {
			error("emptyIrL");
			tp = any_type;
			return this;
		}
				
		for (e=this; e; e=ex) {
			Pexpr ee = e->e1;
//error('d',"e %d %d ee %d %d",e,e?e->base:0,ee,ee?ee->base:0);
			if (e->base != ELIST) error('i',"elist%k",e->base);
			if (ex = e->e2) {	/* look ahead for end of list */
				if (ee == dummy) error("EX in EL");
				if (ex->e1 == dummy && ex->e2 == 0) {
					/* { ... , } */
					DEL(ex);
					e->e2 = ex = 0;
				}
			}
			e->e1 = ee->typ(tbl);
			t = e->e1->tp;
			if (t->base == FCT) {	// yuk!
				ee = new expr(G_ADDROF,0,e->e1);
				e->e1 = ee->typ(tbl);
				t = e->e1->tp;
			}
		}
		tp = t;
		return this;
	}

	case DOT:
	case REF:
	{	
		if (e2) { // .* or ->*
			if (b == REF) b = base = REFMUL;
			if (e2->base == NAME && Pname(e2)->permanent != 1) PERM(Pname(e2));
			break;
		}

		Pbase b;
		bit bcc;

		e1 = e1->typ(tbl);
		t = e1->tp->skiptypedefs(bcc);

		if (	base==DOT && 
			e1->base==DEREF && 
			e1->e1->base==NAME && 
			is_probably_temp(Pname(e1->e1)->string)
		) 
			bcc=0;

		// check that . (->) is applied to class object (pointer)
		if (base == REF) {
			switch (t->base) {
			case COBJ:
			{	
				Pname n = t->classtype()->has_oper(REF);
				if (n) {
					n->n_used += 2;
					e1 = new call(new ref(DOT,e1,new name(n->string)),0);
					return typ(tbl);
				}
				// no break;
			}
			default:
				error("nonP ->%n",mem);
				t = any_type;
				// no break;
			case ANY:
				goto qqq;
			case PTR:
			case VEC:
				b = Pbase(Pptr(t)->typ->skiptypedefs(bcc));
				mem = do_qualifiers(b,mem);
				if (mem->base == DTOR) mem = do_dtor(b,mem);
			}
		}
                else {  // base == DOT
		qqq:
			mem = do_qualifiers(t,mem);
			switch (t->base) {
			default:
				if (mem->base == DTOR) mem = do_dtor(t,mem);
				if ( mem->base == DTOR ) // i.int::~int(), etc
					break;
				error("nonO .%n",mem); t = any_type;
			case ANY:
				break;
			case COBJ:
				if (mem->base == DTOR) mem = do_dtor(t,mem);
			}
//error('d',"dot %k",e1->base);
			switch (e1->base) { 
			case QUEST:
			case ASSIGN:
			case INCR:
			case DECR:
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
			case G_CM:
				base = REF;
				e1 = e1->address();
				break;
                        case CALL:
                        case G_CALL:
//error('d'," f().  %n mem->tp %t",e1->fct_name,mem->tp);
#ifdef FDOTRIGHT
// this rewrite is only necessary on machines with broken C compilers
// there seems to be a lot of those
// and for inlines (a,b).c
                        if (e1->fct_name && Pfct(e1->fct_name->tp)->f_inline)
#endif
                        {       // f(). => (tmp=f(),&tmp)->
                                Pname tmp = make_tmp('Q',e1->tp,tbl);
//error('d',"fdot2 %k",e1->base);
                                e1 = init_tmp(tmp,e1,tbl);
                                Pexpr aa = tmp->address();
                                e1 = new expr(G_CM,e1,aa);
                                e1->tp = aa->tp;
                                base = REF;
                                break;
                        }
                        }
			b = Pbase(t);
		}

		switch (b->base) {
		default:
			if ( mem->base == DTOR ) { // i.int::~int(), etc.
//				// Error check moved into do_dtor().
//				if ( mem->tp2 && Ptype(b)->check(mem->tp2,0) )
//					error("T mismatch for simpleT destructor:XO ofT%t",mem->tp2);
				return mem;
			} else 
				error("(%t) before%k%n (%n not aM)",e1->tp,base,mem,mem);
		case ANY:
			tp = any_type;
			return this;
		case COBJ:
		{	
			Pclass cl = b->classtype();
			if (cl->c_body == 1) cl->dcl_print(0);
			break;
		}
		}	

/*
	x.m is not a const even if x is a const object, this case is handled
	by lval() rejecting it
*/

		if (mem->tp) {
			tp = mem->tp;
			for (Pexpr ee = mem; ee->base==REF; ee = ee->e1) {
				if (ee->e1 == cc->c_this) { // this-> => p->
					ee->e1 = e1;
					ee->base = base;
					if(
						bcc
						&&
						mem->tp
						&&
						mem->tp->skiptypedefs()->base!=FCT
						&&
						mem->tp->skiptypedefs()->base!=OVERLOAD
					)
						mem->tp = mem->tp->mkconst();
					return mem->tp->is_ref() ? mem->contents() : mem;
				}
			}

			return tp->is_ref() ? contents() : this;
		}

		
		Pname q = Pname(mem)->n_qualifier;
		// For a DTOR without a qualifier, ie. p->~A(), use the
		// n_dtag as if it were a qualifier to insure getting
		// the DTOR for the correct base class.  This will also
		// generate a "X is not a base class of Y" message to 
		// supplement a "bad syntax for destructor call" diagnostic
		// generated in do_dtor().
		if ( !q && (strcmp(Pname(mem)->string, oper_name(DTOR)) == 0)) {
			Pname(mem)->n_qualifier = Pname(mem)->n_dtag;
		}  /* if */
		Pexpr e = find_name(Pname(mem),b->classtype(),0,base,cc->nof);
		if (q && (e->base==REF || e->base==DOT)) {
			Pname(e)->n_initializer = Pexpr(q);
//error('d',"q%n e%k%t%n",q,e->base,e->tp,e->base==NAME?e:e->base==REF?e->mem:0);	
		}

		for (Pexpr ee = e; ee->base==REF; ee = ee->e1) {
			if (ee->e1 == cc->c_this) { // this-> => p->
				ee->e1 = e1;
				ee->base = base;
				if(
					bcc
					&&
					e->tp
					&&
					e->tp->skiptypedefs()->base!=FCT
					&&
					e->tp->skiptypedefs()->base!=OVERLOAD
				)
					e->tp = e->tp->mkconst();
				break;
			}
		}

		if (e->base == NAME) {
			switch (e->tp->base) {
			case FCT:
			case OVERLOAD:
				mem = e;
				tp = e->tp;
				e = this;
			}
		}
			
			// function or static member
		return e->tp->is_ref() ? e->contents() : e;
	}

	case CALL:	/* handle undefined function names */
//error('d',"call %k %t %k",e1->base,e1->tp,e2?e2->base:0);
		if (e1->base==NAME && e1->tp==0) {
			Pname q = Pname(e1)->n_qualifier;
			e1 = find_name(Pname(e1),cc->cot,tbl,CALL,cc->nof);
//error('d',"e1 %k %t %n",e1->base,e1->tp,e1->base==NAME?e1:e1->base==REF?e1->mem:0);
			if (q && (e1->base==REF ||e1->base==DOT)) // suppress virtual call
				e1->n_initializer = Pexpr(q);
		}

		if (e1->base==NAME && Pname(e1)->n_xref) {
			// fudge to handle X(X&) args
			e1 = new expr(DEREF,e1,0);
			e1->tp = e1->e1->tp;	// !!
		}

		switch (e1->base) {
		case DTOR: 
			// This is an explicit DTOR call without -> or .
			error('e', "O orP missing for explicit destructor call\n");
			break;
		case DOT:
		case REF:			// becomes ob.x::~x(0) or
			if (e2==0		// becomes p->x::~x(0)
			&& Pref(e1)->mem 
			&& Pref(e1)->mem->base==DTOR ) {
				Pref r = Pref(e1);
				if ( r->base == DOT ) {
					r->e1 = new expr(ADDROF,0,r->e1);
					r->base = REF;
				}
				Pname m = Pname( r->mem );
				bit qual = m->n_qualifier != 0;
				int tok = qual ? 0 : REF;
				// tok==REF => allow virtual call in call_dtor()
// error('d',"qual: %d, e1: %k tok: %d", qual, e1->base, tok );
				e1 = e1->typ(tbl);
				// The memory pointed to by the previous value of
				// "r->mem" or "m" has been deleted.
				r = Pref(e1);
				m = Pname(r->mem);
				if ( m->base == NAME ) { // "real" dtor
					Pexpr ee = call_dtor(e1->e1,r->mem,0,tok,one);
					ee->tp = void_type;
					return ee;
				}  /* if */
			}  /* if */
		}
		break;

	case QUEST:
		Ntmp = 0;
		Ntmp_flag = 0;
		if (in_quest < 1) {
			Ntmp_refd = 0;
			Ntmp_flag_list = 0;
		}
		cond = cond->typ(tbl);
		in_quest++;
                if (Ntmp_refd) {
			Pname n = Ntmp_refd; 
			Pname nn = Ntmp_flag_list; 
			for (; n && nn; n = n->n_list, nn = nn->n_list)	{
// error('d',"pdlist: n: %n nn: %n",n,nn);
				Pname nc = new name("");
				*nc = *n;
				nc->string = new char[strlen(n->string) + 1];
				strcpy(nc->string, n->string);
				Pname nnc = new name("");
				*nnc = *nn;
				nnc->string = new char[strlen(nn->string) + 1];
				strcpy(nnc->string, nn->string);
				con_dtor *t = new con_dtor(nc, nnc);
                        	if (pdlist) t->next = pdlist;
                        	pdlist = t;
			}
                        Ntmp_flag = 0;
                }
		break;

	case ANDAND:
	case OROR:
		Ntmp = 0;
		Ntmp_flag = 0;
		if (in_quest < 2) {
			Ntmp_refd = 0;
			Ntmp_flag_list = 0;
		}
	}

	if (e1) {
		e1 = e1->typ(tbl);
		if (e1->tp->is_ref()) e1 = e1->contents();
		t1 = e1->tp;
		if (Ntmp_flag)  {
                        if (b==QUEST || in_quest ) {
                                Pexpr e = new expr(ASSIGN,Ntmp_flag,one);
                                e->tp = int_type;
                                e1 = new expr(G_CM,e,e1);
                                e1->tp = t1;
                        }
                        Ntmp_flag = 0;
		}
	}
	else
		t1 = 0;

	if (e2) {
		e2 = e2->typ(tbl);
		if (e2->tp->is_ref()) e2 = e2->contents();
		t2 = e2->tp;
		if (Ntmp_flag)  {
                        if (b==QUEST || in_quest) {
                                Pexpr e = new expr(ASSIGN,Ntmp_flag,one);
                                e->tp = int_type;
				if (e2->base == ELIST) {
					Ptype t = e2->e1->tp;
                                	e2->e1 = new expr(G_CM,e,e2->e1);
					e2->e1->tp = t;
				} else {
                                	e2 = new expr(G_CM,e,e2);
                                	e2->tp = t2;
				}
                        }
                        Ntmp_flag = 0;
		}
	}
	else 
		t2 = 0;

	switch (b) {	// filter out non-overloadable operators
	default:
	{	
		Pexpr x = oper_overload(tbl);
		if (x) {
			if (x->tp != any_type
			    &&
			    (b==INCR || b==DECR)
			    &&
			    x->base != INCR && x->base != DECR
			) {
#if 0
				if (t2==0 && t1) { // postfix
					Pexpr ox = x;
					Pexpr fe;
					if (x->base == DEREF
					    &&
					    x->e1->tp->is_ref()
					) 
						x = x->e1;

					fe = x->e1->base==NAME?x->e1:x->e1->mem;

					if (fe->tp->base==OVERLOAD) 
						x = make_postfix(x);
					else x = check_postfix(x);
					if (
					    (warning_opt || strict_opt)
					    &&
					    x->e2==0
					)
						error(warning_opt?'w':0,"prefix ``%k'' used for postfix call (anachronism)",b);
					if (x != ox) {
						x->tp = 0;
						x = x->typ(tbl);
					}
				}
#endif
			}	
			return x;
		}
		if (t2 && t1==0 && t2!=e2->tp) t2 = e2->tp;
	}
	case DOT:
	case G_CM:
	case G_ADDROF:
	case G_CALL:
	case QUEST:
		break;
	}

	switch (b) {
	case QUEST:
		in_quest--;
	case ANDAND:
	case OROR:
		if (Ntmp && b != QUEST ) { 
			Ntmp_dtor = new expr(ELIST,this,Ntmp_dtor);
// error('d',"(e %d) temp ofC%n with dtor needed in%kE",this,Ntmp,b);
		}

                if (b==QUEST && Ntmp_refd) {
			Pname n = Ntmp_refd; 
			Pname nn = Ntmp_flag_list; 
			for (; n && nn; n = n->n_list, nn = nn->n_list)	{
// error('d',"pdlist: n: %n nn: %n",n,nn);
				Pname nc = new name("");
				*nc = *n;
				nc->string = new char[strlen(n->string) + 1];
				strcpy(nc->string, n->string);
				Pname nnc = new name("");
				*nnc = *nn;
				nnc->string = new char[strlen(nn->string) + 1];
				strcpy(nnc->string, nn->string);
				con_dtor *t = new con_dtor(nc, nnc);
                        	if (pdlist) t->next = pdlist;
                        	pdlist = t;
			}
                        Ntmp_flag = 0;
                }
	}

	t = (t1==0) ? t2 : (t2==0) ? t1 : 0;
	switch (b) {		/* are the operands of legal types */
	case REFMUL:
		base = REF;
		// no break;
	case DOT:
	{	// a .* p => &a MEMPTR p => appropriate indirection
			// to be considered: what happens if a .* expression
			// is used except in a call/=?
		Pexpr a = e1->typ(tbl);
		Ptype at = a->tp->skiptypedefs();
		if (base == DOT) {
			a = a->address();
			at = at->addrof();	// beware of n_xref
		}

		Pname cn = at->base==PTR ? Pptr(at)->typ->is_cl_obj() : 0;
		Pclass mm = cn ? Pclass(cn->tp) : 0;

		Pexpr p = e2->typ(tbl);
		Ptype pt = p->tp->skiptypedefs();
		Pname pcn = pt->is_cl_obj();

//error('d',"mm %t pt %t",mm,pt);
		if (pcn) {
			Pclass cl = Pclass(pcn->tp);
			Pname found = 0;
			for (Pname on=cl->conv; on; on=on->n_list) {
				Pfct f = Pfct(on->tp);
				Pptr frt = Pptr(f->returns->skiptypedefs());
				if (frt->base!=PTR || Pptr(frt)->memof==0) break;
				if (same_class(Pptr(frt)->memof,mm) || mm->has_base(Pptr(frt)->memof)) {
					if (found)
						error("two possible conversions forP toM: %n and %n",found,on);
					else
						found = on;
				}
			}
//error('d',"found %n",found);
			if (found) {
				p = new ref(DOT,p,found);
				p = new call(p,0);
				p = p->typ(tbl);
				pt = p->tp->skiptypedefs();
			}
	
		}
		if (pt->base!=PTR || Pptr(pt)->memof==0) {
			if (b==DOT)
				error("P toMX in .*E: %t",pt);
			else
				error("P toMX in ->*E: %t",pt);
			tp = any_type;
			base = DUMMY;
			return this;
		}
		Pclass pm = Pptr(pt)->memof;

		if ( !same_class(mm,pm) ) {
			if (mm->has_base(pm) == 0) {
				error("badOT in .*E: %t (%s*X)",a->tp,pm->string);
				tp = any_type;
			}
			a = new texpr(G_CAST,pm->this_type,a);
			e1 = a = a->typ(tbl);
		}

		Ptype tpx = Pptr(pt)->typ->skiptypedefs();

		if (tpx->base == FCT) {	// a.*p => (&a MEMPTR p)
			base = MEMPTR;
			tp2 = mm;	// keep the class for simpl.c
			e1 = a;
			e2 = p;
		}
		else {	// a .* p => *(typeof(p))((char*)&a + (int)p-1)
			a = new cast(Pchar_type,a);
			p = new cast(int_type,p);
			p = new expr(MINUS,p,one);
			p->tp = int_type;
			Pexpr pl = new expr(PLUS,a,p);
			pl->tp = Pchar_type;
			base = DEREF;
			pt = new ptr(PTR,tpx);	// need a T* not a T C::*
			Pptr(pt)->b_const = Pptr(tpx)->b_const;
			e1 = new cast(pt,pl);
			e2 = 0;
		}
		tp = tpx;
		return tp->is_ref() ? contents() : this;
	}

	case G_CALL:
	case CALL:
		tp = call_fct(tbl);	/* two calls of use() for e1's names */
		if (tp->is_ref()) return contents();
		return this;

	case DEREF:
		if (e1 == dummy) error("O missing before []\n");
		if (e2 == dummy) error("subscriptE missing");

		if (t) {	/*	*t	*/
			t = t->skiptypedefs();
			if (t->base==PTR && Pptr(t)->memof) error("P toM dereferenced");
			tp = t->deref();
		}
		else {					// e1[e2] that is *(e1+e2)
//error('d',"deref %t[%t]",t1,t2);
			if (t1->vec_type()) {		// e1[e2]
				switch (t2->base) {
				case CHAR:
				case SHORT:
				case INT:
				case LONG:
				case EOBJ:
					break;
				default:
				{	Pname cn = t2->is_cl_obj();
					if (cn)	// conversion to integral?
						e2 = check_cond(e2,DEREF,tbl);
					else
						t2->integral(DEREF);
				}
				}
				t1 = t1->skiptypedefs();
				if (t1->base==PTR && Pptr(t1)->memof) error("P toM dereferenced");
				tp = t1->deref();
				(void) tp->tsizeof();
			}
			else if (t2->vec_type()) {	// really e2[e1]
				t1->integral(DEREF);
				t2 = t2->skiptypedefs();
				if (t2->base==PTR && Pptr(t2)->memof) error("P toM dereferenced");
				tp = t2->deref();
				(void) tp->tsizeof();
			}
			else {
				error("[] applied to nonPT:%t[%t]",t1,t2);
				tp = any_type;
			}
		}

		if (tp->is_ref()) return contents();

		return this;

	case G_ADDROF:
	case ADDROF:
//error('d',"addrof(%d) %k %d",base,e2->base,e2->base);
		{
		switch (e2->base) {	// potential lvalues
		case G_CM:
			if (base==ADDROF && e2->e2->base==NAME) {
				// check for cfront generated result variable
				char* s = e2->e2->string;
				if (s[0]=='_' && s[1] && s[1]=='_') {
					if (s[2] && (s[2]=='R' || s[2]=='V')) {
						error("address of non-lvalue");
						break;
					}
				}
			}
		case CM:	// &(a,b) => (a,&b)
		{
			Pexpr ee = e2;
			ee->tp = 0;
			ee->e2 = new expr(base,0,ee->e2);
			delete this;
			return ee->typ(tbl);
		}
		case QUEST:	// & (a?b:c) => (a?&b:&c)
		{
			Pexpr ee = e2;
			ee->tp = 0;
			ee->e1 = new expr(base,0,ee->e1);
			ee->e2 = new expr(base,0,ee->e2);
			delete this;
			return ee->typ(tbl);
		}
		case INCR:
		case DECR:
			if (e2->e1) break;
		case ASSIGN:
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
			return e2->address();
		}

		int oerror_count = error_count;
		if (e2->lval(ADDROF) == 0) {
			if (error_count>oerror_count) {	// error_count == oerror_count
							// possible in case of & const
				tp = any_type;
				return this;
			}
		}

		tp = t->addrof();

		if (e2->tp->base == VEC) {
			if(e2->base != NAME) {
				base = G_CAST;
				tp2 = tp;
				e1 = e2;
				e2 = 0;
				return this;
			}
		}

		if (t->base==FCT) 
			Pptr(tp)->memof = Pfct(t)->memof;

//		if (t->tconst() && vec_const==0 && fct_const==0) Pptr(tp)->b_const = 1;
// ??? & (const T) is NOT T*const but const T*

		switch (e2->base) {
		case NAME:
		mname:					// check for &s::i
		{	Pname n2 = Pname(e2);
			Pname cn = (n2->n_table && n2->n_table!=gtbl) ? n2->n_table->t_name : 0;
// error('d',"n2 %k cn %n t %t",n2->base,cn,t);
			if (cn == 0) break;

			if (t->base==FCT && Pfct(t)->f_static) {
				Pptr(tp)->memof = 0;
				break;
			}

			switch (t->base) {
			case OVERLOAD:
				return e2;
			case FCT:
// error('d', "cn->tp: %d, %k", cn->tp, cn->tp->base);
				{
				Pclass cl = Pclass(cn->tp);
				Pptr(tp)->memof = cl; //SSS

				if (Pfct(t)->f_virtual)	{
					// { 0,vtbl index,0 }
					e1 = new ival(Pfct(t)->f_virtual);
					e1 = new expr(ELIST,zero,e1);
					e2 = zero;
					base = ILIST;
					return this;
				}		// use the pointer
				
				// { 0,-1,(int(*)())ptr }
				e1 = new ival(-1);
				e1 = new expr(ELIST,zero,e1);
				// e2 is the name
				if(
				    Pname(e2)->n_oper == ASSIGN
				    &&
				    cl->memtbl->look(e2->string,0)->tp->base
				    != OVERLOAD
				) {
				    Pname nmw = new name;
				    *nmw = *((Pname)e2);
				    e2 = new cast(Pfct_type,nmw);
				}
				else {
				    e2 = new cast(Pfct_type,e2);
				}
				base = ILIST;
				return this;
				}

			default:
				if (n2->n_stclass != STATIC) {	// offset + 1
					e1 = new ival(n2->n_offset+1);
					Pptr(tp)->memof = Pclass(cn->tp);
//error('d',"cl %d %s i %d",Pclass(cn->tp),Pclass(cn->tp)?Pclass(cn->tp)->string:"0",n2->n_offset);
				}
				else
					return this;
			}
//error('d',"int_type");
			e1->tp = int_type;
			e2 = 0;
			tp2 = tp;
			base = G_CAST;
			return this;
		}
		case DOT:
                case REF:
                {       
                        Pname m = Pname(e2->mem);
			while ( m->base == MDOT ) m = Pname(Pexpr(m)->mem);

			Pfct f = Pfct(m->tp);

			if (t->base==FCT && Pfct(t)->f_static) {
				Pptr(tp)->memof = 0;
				break;
			}

			bit all_stat = 1;
			if (f->base == OVERLOAD) {
				Plist gl = Pgen(m->tp)->fct_list;
				for(; gl; gl=gl->l) {
					if (Pfct(gl->f->tp)->f_static == 0) {
						all_stat = 0;
						break;
					}
				}
			}

			if (f->base == FCT || f->base == OVERLOAD) {  // &p->f
				Pexpr q = Pname(e2)->n_initializer;  // &p->x::f

				if (q && bound==0 && e2->e1==cc->c_this) {	
					// FUDGE: &this->x::f => &x::f
					DEL(e2);
					e2 = m;
					goto mname;
				}

				bound = 1;
				if (f->base==OVERLOAD && all_stat==0
				    ||
				    f->base==FCT && !f->f_static
				) {
					error(
					    "address of boundF (try using ``%s::*'' forPT and ``&%s::%s'' for address)",
					    m->n_table->t_name->string,
					    m->n_table->t_name->string,
					    m->string
					);
				}

				if (q || f->f_virtual==0) {
					// & x.f  =>  & f
					DEL(e2);
					e2 = m;
				}
			}
			break;

		}
		case MEMPTR:
			// &(p->*q)
			error("& .* E");
		}

		return this;
		}

	case UMINUS:
		t->numeric(UMINUS);
		tp = t;
		return this;

	case UPLUS:
		tp = t;
		if (t->num_ptr(UPLUS)=='P' || ansi_opt==0) {
			base = PLUS;
			e1 = zero;
		}
		return this;

	case NOT:
		e2 = check_cond(e2,NOT,tbl);
		tp = int_type;
		return this;

	case COMPL:
		t->integral(COMPL);
		tp = t;
		return this;

	case INCR:
	case DECR:
		{
// error('d',"incr-decr: e1: %k e2: %k", e1?e1->base:0, e2?e2->base:0);
		Pexpr e = e1?e1:e2;	// e1!=0 ==> e++ or e--
					// e1==0 ==> ++e or --e

		e->lval(b);

		switch(e->base) {
		case QUEST:
			e->tp=0;
			e->e1 = e1 ? new expr(base,e->e1,0) : new expr(base,0,e->e1);
			e->e2 = e1 ? new expr(base,e->e2,0) : new expr(base,0,e->e2);
			delete this;
			return e->typ(tbl);
		case CM:
		case G_CM:
			e->tp=0;
			e->e2 = e1 ? new expr(base,e->e2,0) : new expr(base,0,e->e2);
			delete this;
			return e->typ(tbl);
		case INCR:
		case DECR:
			if(e->e1)
				break;
			nin++;
			if(e->e2->not_simple())
				error('s',"overly complex %k of %k",b,e->base);
			nin--;
			e = new expr(G_CM,e,e->e2->typ(tbl));
			if (e1) e1=e; else e2=e;
			// e1?e1:e2 = e;
			return typ(tbl);
		case ASSIGN:
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
			nin++;
			if(e->e1->not_simple())
				error('s',"overly complex %k of %k",b,e->base);
			nin--;
			e = new expr(G_CM,e,e->e1->typ(tbl));
			if (e1) e1=e; else e2=e;
			return typ(tbl);
		}
		}

		r1 = t->num_ptr(b);
		if (r1=='P' && t->check(Pvoid_type,IGNORE_CONST)==0 && 
			ptr_is_template_formal(Pptr(t))==0) 
				error("%k of void*",b);
		tp = t;
		return this;
	
	}

	if (e1==dummy || e1==0) error("operand missing for%k",b);
	if (e2==dummy || e2==0) error("initializer missing for%k",b);

	switch (b) {
	case MUL:
	case DIV:
		r1 = t1->numeric(b);
		r2 = t2->numeric(b);
		nppromote(b);
		break;

	case PLUS:
		r2 = t2->num_ptr(PLUS);
		r1 = t1->num_ptr(PLUS);
		nppromote(PLUS);
		goto void_check;

	case MINUS:
		r2 = t2->num_ptr(MINUS);
		r1 = t1->num_ptr(MINUS);
		if (r2=='P' && r1!='P' && r1!='A') error("nonP - P");
		nppromote(MINUS);
	void_check:
// error('d',"t1: %t %k t2: %t %k", t1, t1->base, t2, t2->base);

		if ((r1=='P' && t1->check(Pvoid_type,IGNORE_CONST)==0 &&  
			ptr_is_template_formal(Pptr(t1))==0) 
		||  (r2=='P' && t2->check(Pvoid_type,IGNORE_CONST)==0 &&  
			ptr_is_template_formal(Pptr(t2))==0))
			error("%k of void*",b);
		tp = t;
		break;

	case LS:
	case RS:
	case AND:
	case OR:
	case ER:
		switch (e1->base) {
		case LT:
		case LE:
		case GT:
		case GE:
		case EQ:
		case NE:
			error('w',"%kE as operand for%k",e1->base,b);
		}
		switch (e2->base) {
		case LT:
		case LE:
		case GT:
		case GE:
		case EQ:
		case NE:
			error('w',"%kE as operand for%k",e2->base,b);
		}
	case MOD:
		r1 = t1->integral(b);
		r2 = t2->integral(b);
		nppromote(b);
		break;

	case LT:
	case LE:
	case GT:
	case GE:
	case EQ:
	case NE:
		r1 = t1->num_ptr(b);
		r2 = t2->num_ptr(b);
		if (r1=='P' && r2=='I') {	// allow things like:
						// if (p==2-2)
						// YUCK!
			Neval = 0;
			long i = e2->eval();
			if (Neval==0 && i==0) {
				DEL(e2);
				e2 = zero;
				r2 = 'Z';
			}
		}
		else if (r2=='P' && r1=='I') {
			Neval = 0;
			long i = e1->eval();
			if (Neval==0 && i==0) {
				DEL(e1);
				e1 = zero;
				r1 = 'Z';
			}
		}

		if (b!=EQ && b!=NE) {
			if (r1=='P' && r2=='Z') error("P%k 0",b);
			if (r2=='P' && r1=='Z') error("P%k 0",b);
		}

		// make sure functions are properly converted to pointers to
		// functions and make sure overloaded functions are rejected
		if (r1 == FCT) {
			e1 = ptof(0,e1,tbl);
			t1 = e1->tp;
			r1 = t1->num_ptr(b);
		}
		if (r2 == FCT){
			e2 = ptof(0,e2,tbl);
			t2 = e2->tp;
			r2 = t2->num_ptr(b);
		}

		npcheck(b);

		if (r1=='P') { // need cast for pointers to virtual and second bases
			Pptr p1 = t1->is_ptr();
			Pptr p2 = t2->is_ptr();
			Pname cn = p1?p1->typ->is_cl_obj():0;
			Pname cn2 = p2?p2->typ->is_cl_obj():0;

			if (cn && cn2) {
				Pclass cl = Pclass(cn->tp);
				Pclass cl2 = Pclass(cn2->tp);

				if (cl->has_base(cl2)) {
					e1 = cast_cptr(cl2,e1,tbl,base==CAST?1:0);
					e1 = new cast(p2,e1);
				}
				else if (cl2->has_base(cl)) {
					e2 = cast_cptr(cl,e2,tbl,base==CAST?1:0);
					e2 = new cast(p1,e2);
				}
			}
		}

		if (r1=='P' && t1->memptr()
		    ||
		    t1->base==FCT && Pfct(t1)->memof
		    ||
		    r2=='P' && t2->memptr()
		    ||
		    t2->base==FCT && Pfct(t2)->memof
		) {
			if (r1=='Z') {
				e1 = zero;
				if(e2->base == ILIST) e2 = e2->e1->e2;
				else e2 = make_dot(e2,tbl);
			}
			else if (r2=='Z') {
				e2 = zero;
				if(e1->base == ILIST) e1 = e1->e1->e2;
				else e1 = make_dot(e1,tbl);
			}
			else if (
			    r2=='P' && !t2->memptr()
			    || 
			    r1=='P' && !t1->memptr()
			) {
				;//error('s',"%t %k %t",t1,base,t2);
			}
			else {
				// ERROR: no check for sideeffects
				Pexpr i1;
				i1 = e1;
				while ( i1->base == G_CAST || i1->base == CAST )
				    i1 = i1->e1;
				if ( i1->base == ILIST ) {
				    e1 = i1;
				    i1 = make_dot(i1,tbl,"i");
				}
				else if (i1->base == ZERO) {
				    i1 = zero;
				    e1 = i1;
				}
				else {
				    i1 = make_dot(i1,tbl,"i");
				}

				Pexpr i2;
				i2 = e2;
				while ( i2->base == G_CAST  || i2->base == CAST)
				    i2 = i2->e1;
				if ( i2->base == ILIST ) {
				    e2 = i2;
				    i2 = make_dot(i2,tbl,"i");
				}
				else if (i2->base == ZERO) {
				    i2 = zero;
				    e2 = i2;
				}
				else {
				    i2 = make_dot(i2,tbl,"i");
				}

				Pexpr f1;
				if(e1->base == ILIST) {
					f1 = e1->e2;
				}
				else if (e1->base == ZERO) {
					f1 = zero;
				}
				else {
					f1 = make_dot(e1,tbl,"f");
				}

				Pexpr f2;
				if (e2->base == ILIST) {
					f2 = e2->e2;
				}
				else if (e2->base == ZERO) {
					f2 = zero;
				}
				else {
					f2 = make_dot(e2,tbl,"f");
					f2->tp = f1->tp;
				}

				if (b != EQ && b != NE) {
					f1 = new cast(Pvoid_type, f1);
					f2 = new cast(Pvoid_type, f2);
				}
				Pexpr ei = new expr(base,i1,i2);
				Pexpr fi = new expr(base,f1,f2);

				base = b==NE ? OROR : ANDAND;
				e1 = ei;
				e2 = fi;
			}
		}
		else if (b != EQ && b != NE) {
			bit doit = 0;
			if (r1 == 'P') {
				Ptype tt1 = t1;
				Ptype tt2;
				while (tt2 = tt1->is_ptr_or_ref())
					tt1 = Pptr(tt2)->typ;
				if (tt1->base == FCT)
					doit = 1;
			}
			if (!doit && r2 == 'P') {
				Ptype tt1 = t2;
				Ptype tt2;
				while (tt2 = tt1->is_ptr_or_ref())
					tt1 = Pptr(tt2)->typ;
				if (tt1->base == FCT)
					doit = 1;
			}
			if (doit) {
				e1 = new cast(Pvoid_type, e1);
				e2 = new cast(Pvoid_type, e2);
			}
		}
		t = int_type;
		break;

	case ANDAND:
	case OROR:
		e1 = check_cond(e1,b,tbl);
		e2 = check_cond(e2,b,tbl);

		if (Ntmp && b != QUEST ) { 
	       	   Ntmp_dtor = new expr(ELIST,this,Ntmp_dtor);
// error('d',"(e %d) temp ofC%n with dtor needed in%kE",this,Ntmp,b);
		}

		t = int_type;
		break;

	case QUEST:
		{
		Pname c1, c2;
		cond = check_cond(cond,b,tbl);

		// still doesn't do complete checking for possible conversions
		bit MPTR = 0; // local hack

		suppress_error++;
		r1 = t1->num_ptr(b);
		r2 = t2->num_ptr(b);
		suppress_error--;
		if (
		    (r1=='P' && t1->memptr() || r1==FCT && Pfct(t1)->memof && Pfct(t1)->f_static==0)
    		    && 
		    (r2=='P' && t2->memptr() || r2==FCT && Pfct(t2)->memof && Pfct(t2)->f_static==0)
		) {

			++MPTR; // prevent later sorry

			if(r1==FCT) {
				e1 = new expr(G_ADDROF,0,e1);
				e1->typ(tbl);
				t1 = e1->tp;
				r1 = t1->num_ptr(b);
			}
			if(r2==FCT) {
				e2 = new expr(G_ADDROF,0,e2);
				e2->typ(tbl);
				t2 = e2->tp;
				r2 = t2->num_ptr(b);
			}
	
			// watch for casts
			Pexpr tt = e1;
			while ( tt->base == CAST || tt->base == G_CAST ) 
				tt = tt->e1;
			if ( tt->base == ILIST ) e1 = tt; 
	
			tt = e2;
			while ( tt->base == CAST || tt->base == G_CAST ) 
				tt = tt->e1;
			if ( tt->base == ILIST ) e2 = tt; 

			// cannot have sides return (expr?{}:{})
			// reuse same temp for both sides ?:
			if (e1->base == ILIST || e2->base == ILIST) {
			    Pname temp = make_tmp( 'A', mptr_type, tbl );
	
			    if (e1->base == ILIST) {
				e1 = mptr_assign( temp, e1 );
				e1 = new expr( G_CM, e1, temp );
				e1->tp = temp->tp;
			    }

			    if (e2->base == ILIST) {
				e2 = mptr_assign( temp, e2 );
				e2 = new expr( G_CM, e2, temp );
				e2->tp = temp->tp;
			    }
			}
		}

		bit redo1 = 0;
		bit redo2 = 0;
		c1 = t1->is_cl_obj();
		Pname eo1 = eobj;
		c2 = t2->is_cl_obj();
		Pname eo2 = eobj;

		if (t1 == t2
		    ||
		    c1 && c2 && c1->tp==c2->tp
		    ||
		    eo1 && eo2 && eo1->tp==eo2->tp
		)
			t = t1;
		else {
			if (c1 && c2) {
				Ptype tt;
				Pclass cl1 = Pclass(c1->tp);
				Pclass cl2 = Pclass(c2->tp);
				if (same_class(cl1,cl2) || cl2->has_base(cl1)) {
					t = t1;
					redo2=1;
					goto caca;
				}
				else if (cl1->has_base(cl2)) {
					t = t2;
					redo1=1;
					goto caca;
				}
				else if (tt = common_base(cl1,cl2)) {
					redo1 = redo2 = 1;
					t = tt;
					goto caca;
				}
			}

			r1 = t1->num_ptr(b);
			r2 = t2->num_ptr(b);

			if (r1=='P' && r2=='P') {
				Pptr p1 = t1->is_ptr();
				Pptr p2 = t2->is_ptr();
				Ptype tt;
				if ((c1 = p1->typ->is_cl_obj())
				&& (c2 = p2->typ->is_cl_obj())) {
					Pclass cl1 = Pclass(c1->tp);
					Pclass cl2 = Pclass(c2->tp);
					if (same_class(cl1,cl2) || cl2->has_base(cl1)) {
						t = t1;
						goto caca;
					}
					else if (cl1->has_base(cl2)) {
						t = t2;
						goto caca;
					}
					else if (tt = common_base(cl1,cl2)) {
						t = tt->addrof();
						goto caca;
					}
				}
			}

			if (r1==FCT && r2==FCT) {
				if (t1->check(t2,ASSIGN))
					error("badTs in ?:E: %t and %t",t1,t2);
				t = t1;
			}
			else
				nppromote(b);
		caca:
			Pptr pt = t->is_ptr();
			bit t_ptr = pt && pt->base == PTR;
			Pname cn = !t_ptr ? 0 : pt->typ->is_cl_obj();

			if (t!=t1 && t->check(t1,0)) {
				PERM(t);
				if (redo1) {
					e1 = e1->address();
					e1 = new cast(t->addrof(),e1);
					e1 = e1->docast(tbl);
					e1 = new expr(DEREF,e1,0);
					e1->tp = t;
				}
				else  {
					e1 = new cast(t,e1);
					if (cn) e1->e1=cast_cptr(Pclass(cn->tp),e1->e1,tbl,e1->base==CAST?1:0);
				}
			}

			if (t!=t2 && t->check(t2,0)) {
				PERM(t);
				if (redo2) {
					e2 = e2->address();
					e2 = new cast(t->addrof(),e2);
					e2 = e2->docast(tbl);
					e2 = new expr(DEREF,e2,0);
					e2->tp = t;
				}
				else {
					e2 = new cast(t,e2);
					if (cn) e2->e1=cast_cptr(Pclass(cn->tp),e2->e1,tbl,e2->base==CAST?1:0);
				}
			}
			if (t_ptr && pt->memof && MPTR == 0)
				error('s',"conditionalE with%t",t);
		}
		}
		break;

	case ASPLUS:
		r1 = t1->num_ptr(ASPLUS);
		r2 = t2->num_ptr(ASPLUS);
		nppromote(ASPLUS);
		goto ass;

	case ASMINUS:
		r1 = t1->num_ptr(ASMINUS);
		r2 = t2->num_ptr(ASMINUS);
		if (r2=='P' && r1!='P' && r1!='A') error("P -= nonP");
		nppromote(ASMINUS);
		goto ass;

	case ASMUL:
	case ASDIV:
		r1 = t1->numeric(b);
		r2 = t2->numeric(b);
		nppromote(b);
		goto ass;

	case ASMOD:
		r1 = t1->integral(ASMOD);
		r2 = t2->integral(ASMOD);
		nppromote(ASMOD);
		goto ass;

	case ASAND:
	case ASOR:
	case ASER:
	case ASLS:
	case ASRS:
		r1 = t1->integral(b);
		r2 = t2->integral(b);
		npcheck(b);
		t = int_type;
		goto ass;
	ass:
		if (r1=='P' && t1->check(Pvoid_type,IGNORE_CONST)==0 &&  
			ptr_is_template_formal(Pptr(t1))==0) 
		 		error("%k of void*",b);
		tp = t;
		as_type = t;	/* the type of the rhs */
		t2 = t;

	case ASSIGN:
//error('d',"    e1 %d %k e1 %d %k",e1,e1->base,e2,e2->base);

		switch (e1->base) {
		case G_CM:
		case CM:	// (a,b)=c => *(a,&b)=c
		{
			e1->e2 = new expr(G_ADDROF,0,e1->e2);
			e1->tp = 0;
			e1 = new expr(DEREF,e1,0);
			return typ(tbl);
		}
		case QUEST:	// (a?b:c)=d => *(a?&b:&c)=c
		{
			e1->e1 = new expr(G_ADDROF,0,e1->e1);
			e1->e2 = new expr(G_ADDROF,0,e1->e2);
			e1->tp = 0;
			e1 = new expr(DEREF,e1,0);
			return typ(tbl);
		}
		case ASSIGN:	// (a*=b)=c => a*=b,a=c
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
		{
			base = G_CM;
			nin++;
			if (e1->e1->not_simple()) error('s',"lvalue %k too complicated",b);
			nin--;
			Pexpr aa = new expr(e1->base,e1->e1,e1->e2);
			Pexpr bb = new expr(b,e1->e1,e2);
			e1 = aa;
			e2 = bb;
			return typ(tbl);
		}
		case INCR:
		case DECR:	// ++a=b => ++a,a=b
		{
			if(!e1->e2) break ;
			base = G_CM;
			nin++;
			if (e1->e2->not_simple()) error('s',"lvalue %k too complicated",b);
			nin--;
			Pexpr aa = new expr(e1->base,0,e1->e2);
			Pexpr bb = new expr(b,e1->e2,e2);
			e1 = aa;
			e2 = bb;
			return typ(tbl);
		}
		case REF:
		{
			Pexpr r = e1;
			// hack to prevent
			//	f().i = j
			// transformed into
			//	((t=f()),&t)->i = j
			if (r->e1->base==G_CM
			&& r->e1->e2->base==G_ADDROF
			&& r->e1->e2->e2->base==NAME) {
				char* s = r->e1->e2->e2->string;
				if (s[0]=='_' && s[1]=='_')
					error("left hand side not lvalue");
			}
		}
		}

		if (e1->lval(b) == 0) {
			tp = any_type;
			return this;
		}

		t1 = t1->skiptypedefs();
		switch (t1->base) {
		case INT:
		case CHAR:
		case SHORT:
		{	Ptype t = e2->tp->skiptypedefs();
			switch (t->base) {
			case LONG:
			case FLOAT:
			case DOUBLE:
			case LDOUBLE:
				error('w',"%t assigned to %t",e2->tp,t1);
			}
		}
			// no break
		case LONG:
			if (b==ASSIGN
			&& Pbase(t1)->b_unsigned
			&& e2->base==UMINUS
			&& e2->e2->base==ICON)
				error('w',"negative assigned to unsigned");
			break;
		case PTR:
			if (b == ASSIGN) {
				e2 = ptr_init(Pptr(t1),e2,tbl);
				t2 = e2->tp;
//error('d',"pchecked %d",Pchecked);
				if (Pchecked) {
					tp = e1->tp;
					return this;
				}
			}
			break;
   		case COBJ:
		{	Pname c1 = t1->is_cl_obj();
				// test of c1->tp necessary for ``fake classes''
				// _Sdd generated for vector assignemnts
			if (c1
			&& c1->tp
		/*	&& Pclass(c1->tp)->memtbl->look("__as",0)==0*/) {
				Pname c2 = t2->is_cl_obj();
// error('d', "expr::typ: c1: %n c2: %n", c1, c2 );
				if (c1 != c2) {
/*
	consider:

	struct A { A(B&); };
	struct B : A {};

	A aa;
	B bb;

	aa = bb;	// aa.operator=(A(bb));
			// optimize to aa.A(bb) when possible
			// avoid temporary where aa = *(A*)&bb is legal
*/
// error('d',"expr::typ c1 %n %d c2 %n %d",c1,c1?c1->tp:0,c2,c2?c2->tp:0);
					if (c2
					&& c2->tp
					&& can_coerce(t1,t2)==0
					&& (vcllist->clear(),vcllist=0,1<is_unique_base(Pclass(c2->tp),c1->string,0))
					&& Pclass(c1->tp)->c_xref&(C_VBASE|C_VPTR|C_ASS)) {
// error('d',"aaa");
						if (make_assignment(c1)) return oper_overload(tbl);
					}
				// optimize
					else {
						e2 = new expr(ELIST,e2,0);
						e2 = new texpr(VALUE,t1,e2);
						if (Pclass(c1->tp)->has_dtor()==0 &&
							Pclass(c1->tp)->has_oper(ASSIGN)==0) {
				// optimize
// error('d',"bbb");
							e2->e2 = e1;
							e2 = e2->typ(tbl);
                                                	if (e2->base==DEREF && e2->e1->base==G_CALL ||
                                                    	e2->base==ASSIGN && e2->e1==e1) {
// error('d',"ccc");
                                                        	*this = *e2;
							}
							tp = t1;
							return this;
						}
						return typ(tbl);
					}
				}
					// test of c1->tp necessary for ``fake classes''
					// _Sdd generated for vector assignemnts
				else if (c1->tp && Pclass(c1->tp)->c_xref&(C_VBASE|C_VPTR|C_ASS)) {
					if (make_assignment(c1)) return oper_overload(tbl);
				}
			}
			(void) t1->tsizeof();
			break;
		}
		}

//error('d',"check(%t,%t) -> %d",e1->tp,t2,try_to_coerce(t1,e2,"assignment",tbl));
		{	
			Pexpr x = try_to_coerce(t1,e2,"assignment",tbl);

			if (t1->base==COBJ && t1->classtype() && t1->classtype()->has_const_mem())
				error("cannot assign to an object of class %t with const member(s)",t1->classtype());

			if (x)
				e2 = x;
			else if (t2->is_or_pts_to(OVERLOAD))
				error("bad assignmentT:%t =&overloaded function",e1->tp);
			else if (e1->tp->check(t2,ASSIGN)) {
				error('e',"bad assignmentT:%t =%t",e1->tp,t2);
				if (no_const) 
					error('c'," (no usable const conversion)\n");
				else error('c',"\n");
			}
			else if ((t1 = t1->is_ptr()) && t1->memptr()) {
				if (t2 == zero_type) {
					Pexpr ee = new expr(ELIST,zero,zero);
					e2 = new expr(ILIST,ee,zero);
				}
				else if (t2->base==PTR && t2->memptr()) {
					// do nothing: structure assignment
				}
				else {
					Pexpr x = ptr_init(Pptr(t1),e2,tbl);
					if (x != e2) e2 = x;
				}
			}
		}		
		t = e1->tp;	// the type of the lhs
		break;
	case CM:
	case G_CM:
		// cannot have sides return ({},{})
		if (t1->base == FCT) {
			e1 = new expr(G_ADDROF,0,e1);
			e1->typ(tbl);
			t1 = e1->tp;
		}
		if (t2->base == FCT){
			e2 = new expr(G_ADDROF,0,e2);
			e2->typ(tbl);
			t2 = e2->tp;
		}
		if (e1->base == ILIST) {
			Pname temp1 = make_tmp( 'A', mptr_type, tbl );
			e1 = mptr_assign( temp1, e1 );
			e1 = new expr( G_CM, e1, temp1 );
			e1->tp = temp1->tp;
		}

		if (e2->base == ILIST) {
			Pname temp2 = make_tmp( 'A', mptr_type, tbl );
			e2 = mptr_assign( temp2, e2 );
			e2 = new expr( G_CM, e2, temp2 );
			e2->tp = temp2->tp;
			//t2 = e2->tp;
		}

		t = t2;
		break;
	default:
		error('i',"unknown operator%k",b);
	}

	tp = t;
	return this;
}

bit 
expr::is_const_obj()
{
	Pexpr ee = this;
	if(ee == 0) return 0;

	while (ee && (ee->base==DOT || ee->base==REF)) {
		Pexpr m = ee->mem;
		if ( ee->base==REF && m->tp &&  m->tp->is_ptr())
			break;
		ee = ee->e1;
	}
	if (ee) {
		Pptr p;
		Ptype ttt = ee->tp;
		switch (base) {
		case REF:
			{
			p = ttt?ttt->is_ptr():0;
			if (p && p->typ->tconst())
				return 1;
			break;
			}
		case DOT:
			{
			p = ttt?ttt->is_ptr():0;
			int tc = ttt ? ttt->tconst() : 0;
			if (!p && ttt && tc && (!strict_opt || tc!=2))
				return 1;
			break;
			}
		}
	}
	return 0;
}
