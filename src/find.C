/*ident	"@(#)cls4:src/find.c	1.22" */
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

find.c:

	name lookup and visibility checks

*******************************************************************/

#include "cfront.h"
#include "template.h"

Pname undef(Pname n, Ptable tbl, TOK f)
{
	switch (f) {
	case CCON:
		error("illegalF call: explicit call ofK %s()",n->string);
		break;
	case 0:		error("%nU",n);			break;
	case CALL:	error("UF%n called",n);		break;
	case REF:
	case DOT:	error("M%nU",n);		break;
	case ADDROF:	error("address ofU%n",n);	break;
	}

	if (tbl == gtbl) {
		Pname nn = tbl->insert(n,0);
		if (f == CALL) {
			nn->tp = new fct(defa_type,0,0);
			nn->n_sto = nn->n_scope = EXTERN;
		}
		else
			nn->tp = any_type;
		delete n;
		return nn;
	}
	
	n->n_table = tbl;
	n->tp = any_type;
	return n;
}



static int mptr;	// &C::m
static Pname me;	// name of fct requesting access to name using find_name()
static Pfct mef;	// fct requesting access to name using find_name()
Pclass tcl;		// class of original ``this''
Pclass mec;	 	// class requesting access to name using find_name()
int mex; // 0 suppresses access error messages 


static void
check_local_nested_ref( Pname nn, TOK f, loc where )
// check for illegal refs to automatics, etc., in surrounding scope 
{
	if ( processing_sizeof ) return;
//error('d',&where,"check_local_nested_ref nn%n f%k me%n mec%t",nn,f,me,mec);
//error('d',&where,"   nntp%t scope%k sto%k stclass%k",nn->tp,nn->n_scope,nn->n_sto,nn->n_stclass);
	switch ( nn->n_scope ) {
	case ARG:
		error(&where,"automatic variable%n referenced in localC",nn);
		break;
	case FCT:
		if ( nn->n_sto != STATIC 
		&&   nn->n_sto != EXTERN 
		&&   nn->n_stclass != ENUM 
		) {
			switch ( nn->tp->skiptypedefs()->base ) {
			case FCT: case OVERLOAD:
				break;
			default:
				if ( !nn->tp->is_const_object() )
					error(&where,"automatic variable%n referenced in localC",nn);
				else if ( f == ADDROF )
					error(&where,"address of local const%n in localC",nn);
			}
		}
		break;
	case 0: case PUBLIC: // ref to member of enclosing class
		// shouldn't happen ...
		// this function should only be called for non-member refs...
		break;
	} // switch n_scope
}

Pexpr find_name(Pname n, Pclass cl, Ptable tbl, int f, Pname m)
/*
	in function ``m'' find the true name for "n",
	implicitly define if undefined

	f==CALL:	n()		cl == cc->cot
	f==REF:		p->n		cl == class of *p
	f==DOT:		obj.n		cl == class of obj
	f==ADDROF:	&n		cl == cc->cot
	f==0		n   		(none of the above)

	"tbl" defines local scope (block or global)

*/
{
	if ( n == 0 ) 
	    error('i',"find_name(n==0,cl==%t,tbl==%d,f==%k,m==%n)",cl,tbl,f,m);
	Pname q = n->n_qualifier;
	char* s = n->string;
	Pfct mf = m ? m->fct_type() : 0;
	Pname tnrv;

	if (mf && mf->nrv && !q 
	    && 
	    f != DOT && f != REF
	    &&
	    strcmp(mf->nrv->string,s)==0
	    &&
	    ((tnrv=tbl->look(s,0))==0 || tnrv->n_table == gtbl)
	)
	     return mf->f_result->contents();

	Pexpr ee;
	DB( if(Ddebug>=3) {
		error('d',&n->where,"find_name(n%n, cl%t, ..., f %d, m%n)",n,cl,f,m);
		error('d',&n->where," -- q %d %k %t %s",q,q?q->base:0, q?q->tp:0,tbl->whatami());
		if ( Ddebug>=4) tbl->dump();
	});

	tcl = cl;

	mex = 1;
	if (me = m) {
		mef = Pfct(me->tp);
                if (mef->base!=FCT) 
                    error('i',"mef %d %k",mef,mef->base);
		if ( cc->cot && cc->cot->lex_level > m->lex_level )
			// cc->cot is a class local to me
			mec = cc->cot;
		else mec = mef->memof;
	}
	else 
	{
		mef = 0;
		if ( !tbl && (f==DOT || f==REF)) 
			mec = cc->cot;
		else {
			if (q==0 && cl==0 && tbl && tbl!=gtbl && tbl->t_name) {
				// hack: processing static data mem def at 
				//       global scope -- name::dcl() sets tbl 
				//       to class's memtbl, but doesn't push 
				//       class context
				// Ideally, dcl() should push a new context, 
				//       but due to other ideosyncrasies, 
				//       this doesn't work.
				cl = (Pclass)tbl->t_name->tp;
				if ( cl->base != CLASS ) error('i',&n->where,"find_name(%n, 0, %d,%k, 0 ) --CTX for table",n,tbl,f);
			}
			mec = cl;
		}
	}

	if (n->base == MDOT) error('i',"find (mdot%n)",n);

	if (n->n_table) { me = 0; return n; }

	if (me == 0 && (tbl == gtbl || cc->nof==0)) 
		me = dummy_fct; // for error message

//error('d',"q%n%t f%k",q,q?q->tp:0,f);
	if (q) {			// qualified name:	q::s

		if (q == sta_name) {	// explicitly global:	::s
			Pname nn = gtbl->look(s,0);
			if (nn == 0) { me = 0; return undef(n,gtbl,f); }
			nn->use();
			delete n;
			me = 0;
			return nn;
		}

		{
		  Pname aq = q; // actual q
		  while ( aq->tp->base == TYPE ) aq = Pbase(aq->tp)->b_name;
		  if (aq->tp->base != COBJ) {
		    if (aq->n_template_arg == template_type_formal)
 			error('s',"use of %n::%sW formalYTZ",aq,s);
		    else error("Qr%n not aCN",q);
		    me = 0;
		    return undef(n,gtbl,f);
		  }
		  q = aq;
		}
		Pclass qcl = Pclass(Pbase(q->tp)->b_name->tp);

		Pclass bcl = cl;

		if (cl==0 || f==ADDROF)
			bcl = cl = qcl; // Pclass(Pbase(q->tp)->b_name->tp);
		else {
			if (!same_class(qcl,cl)) {	// really a base?
				bcl = cl->is_base(qcl->string);
				if (bcl == 0 || !same_class(bcl,qcl)) {
					if (f==REF || f==DOT) {
						error("%t is not aBC of%t",qcl,cl);
						me = 0;
						return undef(n,cl->memtbl,7);
					}
					goto sss;
				}
				// else try in base or for static
			}
		}

		if (f == ADDROF) mptr = 1;		// &C::m
		ee = cl->find_name(s,same_class(bcl,cl)?0:bcl);	// really a member?

		if (ee && ee->base == NAME && Pname(ee)->n_stclass != STATIC
		    && 
		    ee->tp->is_ref()
		)
			error("P toRM: %s::%s",cl->string,Pname(ee)->string);

		mptr = 0;

		if (ee == 0) {
		sss:
//error('d',"sss %k",f);
			Pname nn=qcl->memtbl->look(s,0);

			if ((f==0) && (nn && nn->tp && nn->tp->base != EOBJ && nn->tp->base!=FCT && nn->tp->base!=OVERLOAD) && (nn->n_stclass != STATIC) && (q && q->tp && q->tp->base!=FCT && q->tp->base != OVERLOAD))
			{
				error("O missing for%n",q);
				if (n) delete n;
				me = 0;
				return q;
			}
			
			if (f!=REF && f!=DOT) {
				// try for static member of other class:
				Pclass qcl = Pclass(Pbase(q->tp)->b_name->tp);
				mptr = 1;
				if ( cl && cl->csu == ANON ) {
					mec = (cc-1)->cot;
					ee = qcl->find_name(s,qcl);
				} else
					ee = qcl->find_name(s,qcl);
				mptr = 0;
				if (ee && ee->base==NAME) {
						DEL(n);
						me = 0;
						return ee;
				}

				/* better error message; excuse the goto
					template <class t> class z {
					public: enum y { a, b }; };

					main() { int i = z::a; }
				*/
				if (ee == 0 && qcl->class_base == CL_TEMPLATE) {
					error("YCM %t::%n requires %t<instance>::%n",qcl,n,qcl,n);
					goto finishing_up;
				}
			}
			error("QdN%n::%n not found in %t",q,n,cl);
finishing_up:
			me = 0;
			return undef(n,bcl?bcl->memtbl:cl->memtbl,7);
		}

		if (ee->base==REF && ee->e1==0) {	// &C::f, no ``this''
//error('d',"ee %k %d f %k",ee->base,ee->e1,f);
			switch (f) {
			case 0:
			case CALL:	//SSS
				{	Pexpr mm = ee->mem;
					while (mm->base==REF || mm->base==MDOT) mm = mm->mem;
					if (mm->base==NAME)
						switch (mm->tp->base) {
						case FCT:
						case OVERLOAD:
							goto addrof;
						default:
							if (Pname(mm)->n_stclass == STATIC) goto addrof;
						}
				}
				error("O orOP missing forM%n",n);
					
			case ADDROF:
			addrof:
				{
				Pexpr x = ee;
				ee = ee->mem;
				delete x;
				}
			case REF:
			case DOT:
				break;
			default:
				error("QdN%n::%n used in nonC context",q,n);
			}
		}

		delete n;
		me = 0;
		return ee;
	} // if q

	if (f!=DOT && f!=REF) {	// not .s or ->s: look for local, global, and member
	    /* ref to name without ::, . or ->
	     * tbl == current table (global, class or block)
	     * mec == cc->cot == nearest enclosing class
	     * cl == tcl == referencing class -- usually == mec, except for
	     *     some calls from simpl.c/simpl2.c for refs to op new/delete
	     *     In this case, search through cl to global scope, not
	     *     through context of calling function (me) or enclosing class 
	     *     (mec).
	     * me == m == nearest enclosing function
	     * cc->c_this == nearest enclosing 'this' pointer
	     *    - used in classdef::find_name() to construct 'this->n'
	     * if tbl is block scope, tbl->look() will find names
	     *    local to the ftn containing the block, or global,
	     *    bypassing intervening function/class scopes
	     *    Therefore, intervening contexts must be searched explicitly.
	     */

	    Pname save_this = cc->c_this;
	    Pname fn = me;
	    Pfct  ft = mef;
	    Pclass dc = ft ? ft->def_context : 0;
	    Pname global_nn = 0;
	    int new_context = 0; // set when looking in enclosing contexts
	    if ( cl && !same_class(cl,mec) ) {
//error('d',"n%n cl%t fn%n mec%t",n,cl,fn,mec);
		for (Pclass cx = cl; cx && cx->c_context==0; cx = cx->in_class);
		if ( cx ) { fn = cx->in_fct; ft = (Pfct)fn->tp; }
		else { fn = 0; ft = 0; }
		tbl = cl->memtbl;
		cc->c_this = 0;
//error('d',"    cl%t fn%n",cl,fn);
	    }
	    while ( mec && mec->csu == ANON ) mec = mec->in_class;
	    for (;;) {
		Pclass cx;
	    	Pname oth = cc->c_this;
		Pexpr ee;
		if ( cl ) {
			if ( tbl == cl->memtbl ) {
				// remove c_this so "object missing"
				//    missing msg will be printed if
				//    if n is found in nested scope
				cc->c_this = 0;
			} else if ( ft==0 || !same_class(cl,ft->memof) ) {
//error('d',&n->where,"find_name%n: confused context cl%t fn%n tbl %s",n,cl,fn,tbl->whatami());
//error('d',&n->where,"   m%n me%n mec%t",m,me,mec);
				error('i',&n->where,"find_name%n: confused context cl%t fn%n tbl %s",n,cl,fn,tbl->whatami());
			} else { // check local scope first
				Pname nn = tbl->look(s,0);
				if (nn && nn->base==TNAME) nn = 0;
				if ( nn ) { // local or global name nn
					if ( nn->n_table != gtbl ) {
						if ( m == 0 ) error('i',&n->where,"find_name%n: local scope but missing m",n);
//error('d',&n->where,"found%n m%n fn%n mec%t",nn,m,fn,mec);
						// this should only be 
						// executed once for initial
						// local scope -- so no need 
						// to execute
						// if ( new context ) 
						//	check_local_nested_ref(nn,f,n->where);
						nn->use();
						DEL(n);
						me = 0;
						cc->c_this = save_this;
						return nn;
					}
					// nn was found in gtbl, but
					//    intervening scopes still exist
					// try them before returning nn
					global_nn = nn;
				} // if nn
			} // else check local scope
			if ( dc ) { // defining context for friend function
				// class A { friend B::f() { ... } };
				// dc==A, cl==B
//error('d',&n->where,"%n dc%t cl%t m%n",n,dc,cl,m);
				cc->c_this = 0; // for "object missing" error
				cx = dc;
				ee = dc->find_name(s,0);
				while ( ee==0 ) {
					if ( cx->c_context ) break;
					cx = cx->in_class;
					if ( cx == 0 ) break;
					if ( same_class(cx,cl) ) break;
					ee = cx->find_name(s,0);
				} // while ee == 0
				cc->c_this = oth;
				dc = 0;
				if ( ee ) goto eee;
			}
			cx = cl;
			ee = cl->find_name(s,0);
			while ( ee==0 ) {
				if ( cx->c_context ) break;
				cx = cx->in_class;
				if ( cx == 0 ) break;
				// remove c_this so "object missing"
				//    missing msg will be printed if
				//    if n is found in nested scope
				cc->c_this = 0;
				ee = cx->find_name(s,0);
			} // while ee == 0
			cc->c_this = oth;
			if ( ee ) {  // class member name
			eee:	if ( new_context ) {
					// class A { // defines op new/delete
					//    void f() {
					//       class B { B(){} ~B(){} };
					//    }
					// };
					// mec==B cl==A
					// cfront currently wants to 
					//    use global op, but issue warning
					Pname nx = Pname(ee);
					Pname nn = global_nn;
					if ( (nn || (nn=gtbl->look(s,0))!=0)
					&&   nn->n_oper ) {
						while(nx->base==REF)
							nx = (Pname)nx->mem;
						error('w',"%n andG%n are both visible within%t -- usingG%n", nx, nn, mec, nn );
						nn->use();
						DEL(n);
						me = 0;
						cc->c_this = save_this;
						return nn;
					}
				}
				if (ee->base==REF && ee->e1==0 ) {
					Pexpr mm = ee->mem;
					while (mm->base==REF || mm->base==MDOT)
						mm = mm->mem;
					if (mm->base==NAME)
			    		switch (mm->tp->base) {
			    		default:
						if (Pname(mm)->n_stclass != STATIC)
							error("O orOP missing for%n",mm);
			    		case FCT: case OVERLOAD:
						DEL(n);
						me = 0;
						cc->c_this = save_this;
						return mm;
			    		}
				}
				DEL(n);
				me = 0;
				cc->c_this = save_this;
				return ee;
			}  // if ee
			// name not found in cl or any enclosing
			//    class within current context
			// cx should == outermost class in current context
			tbl = cx ? cx->c_context : gtbl;
			// now tbl must be either gtbl or a block table in fn
			if ( tbl == 0 ) error('i',"missing context table for cl%t/cx%t",cl,cx);
			if ( tbl == gtbl && global_nn && !dc ) {
				// name found in previous iteration
				global_nn->use();
				DEL(n);
				me = 0;
				cc->c_this = save_this;
				return global_nn;
			}
			new_context = 1;
		} // if cl
		// tbl should be local or global
		Pname nn = tbl->look(s,0);
		if (nn && nn->base==TNAME) nn = 0;
		if ( nn ) { // local or global name nn
			if ( nn->n_table != gtbl ) {
				if ( m == 0 ) error('i',&n->where,"find_name%n: local scope but missing m",n);
				if ( new_context ) {
					// nn is a local name in
					// an enclosing context
					// print appropriate err
//error('d',&n->where,"found%n m%n fn%n mec%t",nn,m,fn,mec);
					check_local_nested_ref(nn,f,n->where);
				}
			} else if ( ft && (ft->memof || dc) ) {
				// nn was found in gtbl, but
				//    intervening scopes still exist
				// try them before returning nn
				global_nn = nn;
				goto nxt;
			}
			nn->use();
			DEL(n);
			me = 0;
			cc->c_this = save_this;
			return nn;
		} // if nn
	    nxt:
		if ( dc ) { // defining context for friend
			// see also if(cl) above
			cc->c_this = 0; // for "object missing" error
			cx = dc;
			ee = dc->find_name(s,0);
			while ( ee==0 ) {
				if ( cx->c_context ) break;
				cx = cx->in_class;
				if ( cx == 0 ) break;
				ee = cx->find_name(s,0);
			} // while ee == 0
			cc->c_this = oth;
			if ( ee ) goto eee;
		}
		new_context = 1;
		cl = ft ? ft->memof : 0;
		if ( cl == 0 ) {
		    if ( global_nn ) {
			global_nn->use();
			DEL(n);
			me = 0;
			cc->c_this = save_this;
			return global_nn;
		    }
		    break;
		}
		// jump to next outer context
		tbl = cl->memtbl;
		fn = cx ? cx->in_fct : 0;
		if ( fn ) { ft = (Pfct)fn->tp; cc->c_this = ft->f_this; }
		else { ft = 0; cc->c_this = 0; }
		dc = ft ? ft->def_context : 0;
	    } // for(;;)
	    me = 0;
	    cc->c_this = save_this;
	    return undef(n,gtbl,f);
	} // if not . or ->

	if (ee = cl->find_name(s,cl)) {	// .s or ->s
		DEL(n);
		me = 0;
		return ee;
	}

	if(!strcmp(s,cl->string)) {
		me = 0;
		return undef(n,gtbl,CCON);
	}

	me = 0;
	return undef(n,gtbl,f);
}

int classdef::check_dup(Pclass cl, TOK bb)
/*
	 see if cl is a base of this; return 0 if no clash
*/
{
	for (Pbcl b = baselist; b; b=b->next) {
		if (::same_class(cl,b->bclass)) {
			if (bb!=VIRTUAL) {
				if (b->base==VIRTUAL)
					error('w',"%t inaccessible because of virtual%t in%t",cl,cl,this);
				else
					error('w',"%t inaccessible because of%t in%t",cl,cl,this);
				return 1;
			}
			else if (b->base!=VIRTUAL) {
				error('w',"virtual %t inaccessible because of%t in%t",cl,cl,this);
				return 1;
			}
		}
		if (b->bclass->check_dup(cl,bb)) return 1;
	}
	return 0;
}


TOK Nvis;
TOK Nvirt;
TOK ppbase;

Pclass classdef::is_base(char* s, TOK* pptr, int level)
/*
	is "s" a public base class of this?
*/
{
// error('d',"%s->is_base(%s) %k",string,s,ppbase);
	TOK pp = ppbase;
	TOK ppaccum;
//	Pclass res = 0;

	for (Pbcl b = baselist; b; b=b->next) {
		if (b->promoted)
			continue;
		char *str = 0;
		if (b->bclass->class_base == INSTANTIATED) 
			str = ((Ptclass)b->bclass)->unparametrized_tname()->string;
// error('d',"base: %s str: %s", b->bclass->string, str);

		if (strcmp(s,b->bclass->string) == 0 ||  
		          (str && strcmp(s,str) == 0)) 
		{
			ppaccum = PUBLIC;
			if (b->ppp!=PUBLIC && 
				!::same_class(cc->cot,this) && 
				(cc->nof==0 || this->has_friend(Pfct(cc->nof->tp))==0)) 
					ppaccum = b->ppp;
			// Nvirt is now used with template functions to recognize B<T>, D<T>
			// that is, if set, then a standard conversion is being used
			Nvirt = b->base;
// error('d',"is_base: level: %d, b->base: %k res: %d ppbase: %k",level,b->base,res,ppbase);
//			if (level==0 && res && b->base==VIRTUAL) 
//				ppbase = b->ppp>ppbase?b->ppp:ppbase;
			if (pptr)
				*pptr = ppaccum;
			if (level==0)
				ppbase = ppaccum;
			return b->bclass;
		}
		else {
//			if (b->ppp!=PUBLIC && 
//				!::same_class(cc->cot,this) && 
//				(cc->nof==0 || this->has_friend(Pfct(cc->nof->tp))==0))
//					ppbase = b->ppp>ppbase?b->ppp:ppbase;// PUBLIC<PROTECTED<PRIVATE
			TOK prot;
			Pclass bc = b->bclass->is_base(s,&prot,level+1);
			if (bc) {
				ppaccum = prot;
				if (b->ppp!=PUBLIC && 
					!::same_class(cc->cot,this) && 
					(cc->nof==0 || this->has_friend(Pfct(cc->nof->tp))==0))
						ppaccum = b->ppp>ppaccum?b->ppp:ppaccum;
				// guaranteed to have an instance in baselist of VIRTUAL
				// its protection level indicates actual protection of class 
// error('d',"is_base: level: %d, b->base: %k bc: %s ppbase: %k",level,b->base,bc->string,ppbase);
				if (ppaccum == PUBLIC) {
					if (pptr)
						*pptr = ppaccum;
					if (level==0)
						ppbase = ppaccum;
					return bc;
				}
//				if (level==0 && Nvirt == VIRTUAL) 
//					{ res=bc; continue; }
//				return bc;
			}
		}
	}
	ppbase = pp;
	return 0;
}

bit classdef::has_base(Pclass cl, int level, int ccflag)
/*
	is "cl" a base of this?
*/
{
// error('d', "%t->has_base( %t ) cc->cot: %t", this, cl, cc->cot?cc->cot:0 );
	if (this == 0) return 0;
	static int found = 0;
	if (level == 0) found = 0;

	for (Pbcl b = baselist; b; b=b->next) {
// error('d', "has_base: b: %t ppp: %k found: %d", b->bclass, b->ppp, found);
		if (::same_class(b->bclass,cl,1)) {
			if (b->ppp!=PUBLIC
			&& (ccflag || !::same_class(cc->cot,this))
			&& (cc->nof==0 ||
				this->has_friend(Pfct(cc->nof->tp))==0) 
			&& (cc->cot==0 ||
				this->has_friend(cc->cot)==0))
					Nvis = b->ppp;	// no standard coercion
			Nvirt = b->base;
// error('d',"has_base: nvirt: %k nvis: %d", Nvirt,Nvis);
			if (level==0 && b->base==VIRTUAL && found==1) {
				// this version of Nvis wins out!
				if (b->ppp != Nvis) 
					Nvis = b->ppp==PUBLIC?0:b->ppp;
			}
			return 1;
		}
		if (b->bclass->has_base(cl,level+1,ccflag)) { 
			// force it to find instance within derived baselist -- at end
			if (level==0 && Nvirt==VIRTUAL) { 
				found=1; continue; }
			return 1;
			}
	}
	return 0;
}

int Noffset;
Pexpr Nptr;
char *Nalloc_base;
clist* vcllist;

int clist::onlist(Pclass c)
{
	for (clist* p = this; p; p = p->next)
		if (same_class(p->cl,c)) return 1;
	return 0;
}

void clist::clear()
{
	if (this == 0) return;
	clist* p = next;
	while (p) {
		clist* q = p->next;
		delete p;
		p = q;
	};
	delete this;
}

Pbcl Nvbc_alloc;
int is_unique_base(Pclass cl, char* s, int offset, int in_base,
		   Pclass priSeen )
/*
	is "s" a unique base class of this?
*/
{
	int i = 0;
// error('d',"is_unique_base(cl: %t, s: %s,%d,%d)",cl,s,offset,in_base);
	for (Pbcl b = cl->baselist; b; b=b->next) {
		int no = 0;
		if (b->base!=VIRTUAL)
			no = offset + b->obj_offset;
		else if (in_base)
			continue;
		if (strcmp(s,b->bclass->string) == 0) {
			Noffset = no;
			i++;
			if ((b->ppp!=PUBLIC || priSeen )
				&& (!same_class(cc->cot,cl) || !same_class(cc->cot,priSeen) )
				&& (cc->nof==0 || cl->has_friend(Pfct(cc->nof->tp))==0)
			      		|| (priSeen && priSeen->has_friend(Pfct(cc->nof->tp))==0))
						Nvis = b->ppp;	// no standard coercion

			if (b->base==VIRTUAL) {
				Nptr = new mdot(s,0);
				// if (b->allocated == 0) {
				if (b->allocated != 1) {
					Nvbc_alloc = 0;
					Nalloc_base = cl->has_allocated_base(s);
				}
			}
		}
		else {
			if (b->base==VIRTUAL) {
				if (vcllist->onlist(b->bclass) )continue;
				vcllist = new clist(b->bclass,vcllist);
			}

			Pclass clscope = 0;
			if ( cc && cc->c_this ) {
				Ptype t = Pptr(cc->c_this->tp)->typ;
				clscope = Pclass(Pbase(t)->b_name->tp);
			}
// error('d', "cl: %t %d clscope: %t %d", cl, cl, clscope, clscope);
			Pclass	new_priSeen = priSeen;
			if (b->ppp != PUBLIC && 
				!same_class(cl,clscope) && priSeen == 0 ) new_priSeen = cl;

			int ii = is_unique_base(b->bclass,s,no,1,new_priSeen);
// error('d',"base %t i %d ii %d",b->bclass,i,ii);
// error('d',"base %t %k allocated: %d", b->bclass, b->base, b->allocated);
			i += ii;
			if (ii==1 && b->base==VIRTUAL) {
			 	Nptr = new mdot(b->bclass->string,0);
				// if (b->allocated == 0) {
				if (b->allocated != 1) {
					Nvbc_alloc = 0;
					Nalloc_base = cl->has_allocated_base(b->bclass->string);
				}
			}
		}
	}

	return i;
}

/*
int classdef::has_allocated_base(Pclass bcl)

	search the list of !first base classes for this virtual base
	space will have been allocated in !first bases for virtual bases
	declared in !first classes

	in addition bcl may bave been specified explicitly as a base

{
	int off;
	for (Pbcl l = baselist; l; l=l->next) {
		if (l->base == VIRTUAL) continue; // another non-allocated virtual base
		if (l==baselist) continue;	// first base

		Pclass bc = l->bclass;
		off = l->obj_offset;

		for (Pbcl ll = bc->baselist; ll; ll=ll->next) {
			// cannot share non-virtual base
			if (ll->base != VIRTUAL) continue;
			if (same_class(ll->bclass,bcl)) return off + ll->obj_offset;
		}
	}
	return 0;
}
*/

int link_compat_hack = 0;
int classdef::has_allocated_base(Pclass bcl, bit first)
/*
	search the list of base classes for this virtual base
	space will be allocated in first virtual version found.
	return offset.

	A virtual base cannot have offset 0 (its pointer at least is ahead)
*/
{
// error('d',"%t->has_allocated_base(%t,%d) ",this,bcl,first);
	for (Pbcl l = baselist; l; l=l->next) {
//		if (l->base==VIRTUAL && l->bclass==bcl && l->obj_offset) return l->obj_offset;
		if (l->base==VIRTUAL && ::same_class(l->bclass,bcl))
		{
		 	if (l->obj_offset && first == 0)
				return l->obj_offset;
			
			for (Pbcl ll = baselist; ll != l; ll=ll->next) {
				if (ll->base==VIRTUAL) continue;
				int i = ll->bclass->has_allocated_base(bcl,ll==baselist);
				// if (i) return i;
				if (i) {
					link_compat_hack = 1;
// error('d',"%t->has_allocated_base(%t %d) link_compat_hack set",this,bcl,first);
					return 0;
				}
			}
		}


		if (l->base==VIRTUAL || l!=baselist) {
			// allocated as an object,
			// not unravelled as a set of members
			int i = l->bclass->has_allocated_base(bcl);
			if (i) return l->obj_offset + i;
		}
	}
	return 0;
}

extern bit Vvtab;
extern bit Vvbc_inher;
extern bit Vvbc_alloc;

char *
classdef::has_allocated_base(char *str) 
/*
 *	str is an unallocated virtual base class of this
 *	return the name of the second or subsequent base class 
 *		containing the member ``struct str *P<str>''
 */
{
// error('d',"%t::has_allocated_base(%s %d) baselist: %t",this,str,baselist->bclass);
	for (Pbcl l = baselist; l; l=l->next) {
		if (l->base == VIRTUAL) {
			// link_compat_hack sets allocated to 2 
			// if ( l->allocated ) 
			if ( l->allocated == 1 ) 
				Nvbc_alloc = l; 
			else
			if (strcmp(str,l->bclass->string) == 0) {
// error('d',"has_allocated_base(%s): found itself as virtual",str);
				Vvbc_inher = 1;
				return 0;
			}
			continue; 
		}

		Pclass bc = l->bclass;
		for (Pbcl ll = bc->baselist; ll; ll=ll->next) {
			if (ll->base != VIRTUAL) continue;
			// if (ll->allocated && 
			if (ll->allocated == 1 && 
			    strcmp( str, ll->bclass->string) == 0 ) { 
				if (::same_class(bc,baselist->bclass)) { 
				    	Vvtab = ll->bclass->has_vvtab;
				    	return 0; 
				}
				return bc->string;
			}
		}
	}
	if (Nvbc_alloc == 0) Vvbc_alloc = 1;
	return 0;
}

/*
int allocated_base(Pclass cl,Pclass bcl)
{
static second;
int s2 = second;
	for (Pbcl l = cl->baselist; l; l=l->next) {
		if (l->base==VIRTUAL
		&& same_class(l->bclass,bcl)
		&& l->obj_offset
		&& (second || l!=cl->baselist)) return (second=s2,1);
		int i = allocated_base(l->bclass,bcl);
		if (i) return (second=s2,1);
		second = 1;
	}
	second = s2;
	return 0;
}
*/
Pname vfct(Pclass cl, char* s)
/*
	Called for each name "s" in a vtbl for "cl"
	Find the "s" to go in the vtbl.
	The "s" that caused the vtbl entry to be created
		is found if nothing else is
*/
{
	Pname n = cl->memtbl->look(s,0);
	if (n) return n;

	for (Pbcl b = cl->baselist; b; b=b->next) {
		Pname nn = vfct(b->bclass,s);
		if (nn) {
//error('d',"nn%n",nn);
			if (n && n!=nn) {
				Pclass ncl = Pclass(n->n_table->t_name->tp);
				Pclass nncl = Pclass(nn->n_table->t_name->tp);
//error('d',"ncl %t nncl %t",ncl,nncl);
				if (nncl->is_base(ncl->string))
					n = nn;		// use nn
			}
			else
				n = nn;
		}
	}

	return n;
}

Pexpr rptr(Ptype t, Pexpr p, int offset)
/*
	return rvalue of offset off pointer:
	(t)((char*)p+offset)
*/
{
	if ( t == 0 ) error( 'i', "rptr(), t==0 (type passed for cast)" );
	Pexpr pp = p;
//error('d',"rptr %t %d",t,offset);
	if (offset) {
		Pexpr i = new ival(offset);
	//	pp = new texpr(G_CAST,Pchar_type,pp);
		pp = new cast(Pchar_type,pp);
		pp = new expr(PLUS,pp,i);
	}
	pp = new cast(t,pp);
	return pp;
}
/*
Pexpr lptr(Ptype t, Pexpr p, int offset)

	return lvalue of offset off pointer:
	*(t*)((char*)p+offset)

{
	Pexpr pp = p;
	if (offset) {
		Pexpr i = new ival(offset);
	//	pp = new texpr(G_CAST,Pchar_type,pp);
		pp = new cast(Pchar_type,pp);
		pp = new expr(PLUS,pp,i);
	}
	pp = new cast(t->addrof(),pp);
	pp = new expr(DEREF,pp,0);
	pp->tp = t;
	return pp;
}
*/
int friend_check(Pclass start,Pclass stop, Pfct f)
/*
	is `f' a friend of a class between `start' and `stop'
	in a class DAG?
*/
{
//error('d',"friend_check(%t,%t)",start,stop);
	
	if (start->has_friend(f)) return 1;
	if (stop == start) return 0;
	for (Pbcl b = start->baselist; b; b = b->next) {
		if (b->bclass->has_friend(f)) return 1;
		if (friend_check(b->bclass,stop,f)) return 1;
	}
	return 0;
}

int function_template::has_friend(Pname ifn)
{
// error('d',"function_template::has_friend(%n %k)",ifn,ifn->tp->base);
	Ptype t = ifn->tp;

	switch (t->base) {
	case FCT:
		{
		Pfct f = Pfct(t);			
		Pname ftn = (f->fct_base==VANILLA) ? ifn : Ptfct(f)->unparametrized_tname();
// error('d',"function_template::has_friend() ftn %d%n fn %d%n",ftn,ftn,fn,fn);
// error('d',"function_template::has_friend() ftn %s fn %s",ftn->string,fn->string);
		// *** XXX: incomplete -- too inclusive
		return (strcmp(ftn->string,fn->string) == 0);
		}
/*
	case GEN: 
		// ??? shouldn't happen ?
*/
	}

	return 0;
}

int templ::has_friend(Pname cn)
{
// error('d',"templ::has_friend(%n %k)",cn,cn->tp->base);
	if (cn->tp->base!=FCT)
	{
		Pclass cl = cn->tp->classtype();
		Pname ptn = Ptclass(cl)->unparametrized_tname();
// error('d',"templ::has_friend() ptn %d%n namep %d%n",ptn,ptn,namep,namep);
// error('d',"templ::has_friend() ptn %s namep %s",ptn->string,namep->string);
		return (strcmp(ptn->string, namep->string)==0);
	}
	return 0;
}

int data_template::has_friend(Pname)
{
	return 0;
}

bit classdef::has_friend(Pfct f)
/*
	does this class have function "f" as its friend?
*/
{
// error('d',"%t->has_friend(%t) %k",this,f,f->base);
// error('d',"%t->has_friend(%t) me%n mef:%t",this,f,me,mef);

	if ( ::same_class(f->memof,this) ) return 1; // could be hit in recursive call 
	for (Plist l=friend_list; l; l=l->l) {
		Pname fr = l->f;
		Ptype frt = fr->tp;
// error('d',"frt %t %d %k",frt,frt,frt->base);
		switch (frt->base) {
		case FCT:
			if (f == frt) return 1;
			break;
		case OVERLOAD:
			l->f = fr = Pgen(frt)->fct_list->f;	// first fct
			if (fr->tp == f) return 1;
		case CLASS:
			break;
		default:
			error('i',"bad friend %k",fr->tp->base);
		}
	}

	Pname fn = me ? me : ((cc->nof && cc->nof->tp==f) ? cc->nof : 0);

	if (fn)
	{
// error('d',"fn:%n%t", fn, fn->tp );
		for (Pcons p = templ_friends; p; p = p->cdr)
		{
			Ptempl_base ptb = Ptempl_base(p->car);
			if (ptb && ptb->has_friend(fn)) return 1;
		}
	}

	if ( f->memof ) return has_friend(f->memof);
	return 0;
}

bit classdef::has_friend(Pclass cl)
/*
	does this class have class "cl" as its friend?
*/
{
// error('d',"%t->has_friend(%t) ",this,cl);

	for (Plist l=friend_list; l; l=l->l) {
		Pname fr = l->f;
		Ptype frt = fr->tp;
		switch (frt->base) {
		case CLASS:
// error('d',"class friend: %t", frt);
			if (::same_class(cl,Pclass(frt),1)) 
				return 1;
		case FCT:
		case OVERLOAD:
			break;
		default:
			{error('i',"bad friend %k",frt->base);}
		}
	}

	if ( cl->is_templ_instance() ) {
		Pname tn = cl->k_tbl->find_cn(cl->string);
// error('d',"tn:%n%t", tn, tn->tp );
		for (Pcons p = templ_friends; p; p = p->cdr ) {
			Ptempl_base ptb = Ptempl_base(p->car);
			if (ptb && ptb->has_friend(tn)) return 1;
		}
	}

	if ( cl->in_fct ) return has_friend( (Pfct)cl->in_fct->tp );
	if ( cl->in_class ) return has_friend(cl->in_class);
	return 0;
}

Pname find_virtual(Pclass cl, Pname s)
/*
	does ``cl'' have a virtual function ``s'' in some base class
*/
{
	for (Pbcl b = cl->baselist; b; b = b->next) {
		Pclass bcl = b->bclass;
		Pname n;
		if (n = bcl->memtbl->look(s->string,0)) {
// error('d', "find_virtual: n: %d base: %k", n, n->base );
			if ( n->base == PUBLIC ) // x::foo;
				continue;
			Pfct f = Pfct(n->tp);
			if (f->base == OVERLOAD) {
				for (Plist gl=Pgen(f)->fct_list; gl; gl=gl->l) {
					n = gl->f;
					// if (f != s->tp) continue;
					// Replaced by:
					if (n->tp->check(s->tp,VIRTUAL)) continue;
					if (Pfct(n->tp)->f_virtual) return n;
				}
			}
			// else if (f->f_virtual)
			else if (f->f_virtual && n->tp->check(s->tp,VIRTUAL)==0)
				return n;
		}
		else if (n = find_virtual(bcl,s))
			return n;
	}
	return 0;
}

Pname dummy_fct;

static int 
is_accessible(Pname n, Pclass this_class, bit noCdcl = 0)
// 0 means n is not accessible to this_class
{
// error('d',"is_accessible(%n,%t)", n, this_class);
// error('d',"%n:%k me%n mef%t",n,n->n_protect,me, mef);
// error('d',"		this_class %d %t",this_class,this_class);
// error('d',"		mec  %d %t",mec,mec);
// error('d',"		tcl  %d %t",tcl,tcl);
// error('d',"mec->has_base(this_class) %d",mec?mec->has_base(this_class):0);
// error('d',"tcl->has_base(mec) %d",tcl?tcl->has_base(mec):0);
// error('d',"tcl->has_base(this_class) %d",tcl?tcl->has_base(this_class):0);
// error('d',"tcl->has_friend(mef) %d",tcl?tcl->has_friend(mef):0);

	if (same_class(this_class,mec) || (mec && this_class->has_friend(mec)) )
		return 1;
	if (mef && (mec==0 || same_class(mec,mef->memof)) && this_class->has_friend(mef))
		return 1;
	
	if ( n->n_protect && tcl ) {
		if ( mec ) 
		{
			if ( tcl->has_friend(mec) ||
				( mec->has_base(this_class) && 
					(same_class(tcl,mec) || tcl->has_base(mec))))
						return 1;
		}	

		if ( mef && (mec==0 || same_class(mec,mef->memof)) ) 
		{
			if ( tcl->has_base(this_class) &&
				// && tcl->has_friend(mef))
				friend_check(tcl,this_class,mef))
					return 1;
		}
	}

	// call from check_visibility
	if ( noCdcl ) return 0; 
	
	if ( Cdcl && Cdcl->base == NAME && 
		Cdcl->n_stclass == STATIC && 
//		Cdcl->n_initializer && 
		(!Cdcl->tp || Cdcl->tp->skiptypedefs()->base != FCT) &&
		Cdcl->n_qualifier ) 
	{
		Pbase bn = Pbase(Cdcl->n_qualifier->tp);
		Pclass cl = Pclass(bn->b_name->tp);

		if ( same_class(cl,this_class) ||
			(n->n_protect && cl->has_base(this_class)) || 
			this_class->has_friend(cl))
				return 1;
	}

	return 0;
}

Pexpr classdef::find_name(char* s, Pclass cl, int access_only, int newflag)
/*
	look for "s" in "this" class and its base classes
	if (cl)
		accept only a member of "cl" or "cl"'s base classes
		(that is cl::s was seen)
	access_only => not a reference -- find and check visibility only 
	complicated by access rules: private, protected, friend
*/
{
// error('d',"%s->find_name(%s,%s) memtbl %d",string,s,cl?cl->string:"",memtbl);
	Pname n = memtbl->look(s,0);
	if ( n == 0 ) { // swann's way? the guermantes' way? there must be a better way!
		if (in_class && templ_base == VANILLA && 
		    in_class->class_base == INSTANTIATED) {
			Pname nn = in_class->memtbl->look(string,CLASS);
			if (nn) {
				Pclass cl = Pclass(nn->tp);
				n = cl->memtbl->look(s,0);
			}
		}
	}

	if (n) {
		if (n->tp && !access_only) {
			switch (n->tp->base) {
			case OVERLOAD:
				break;
			case FCT:
				if (Pfct(n->tp)->f_virtual==0) {
					if (n->n_dcl_printed==0) n->dcl_print(0);
					break;
				}
			default:
				if (class_base == INSTANTIATED)
				{
					current_instantiation = this;
				}
				dcl_print(0);
				if (class_base == INSTANTIATED)
				{
					current_instantiation = 0;
				}
			}
		}

		if (n->base == PUBLIC) {
			if (mex && n->n_scope==0) {
				if ( is_accessible(n,this) == 0 ) 
				{
					if ( me == dummy_fct ) {
					    if ( mec )
						error("%t cannot access%n: %sM",mec,n,n->n_protect?"protected":"private");
					    else
						error("G scope cannot access%n: %sM",n,n->n_protect?"protected":"private");
					} else if (mec && (mef==0 || !::same_class(mec,mef->memof)))
						error("%t cannot access%n: %sM",mec,n,n->n_protect?"protected":"private");
					else
						error("%n cannot access%n: %sM",me,n,n->n_protect?"protected":"private");
					mex = 0;	// suppress further error messages
				}
			}
			mex = 0; // don't have find_in_base complain about n
			return find_in_base(s, cl, access_only, newflag);
		}

		if (cl==0 || ::same_class(cl,this)) {
			if (mptr==0
			&& n->n_stclass!=STATIC
			&& n->n_stclass!=ENUM) {
				Ptype t = n->tp;

// error('d',"%t->find_name cl %d this %d",this,cl,this);

				if (mex && n->n_scope==0 && n->tp->base!=OVERLOAD) {
					if ( is_accessible(n,this)==0 )
					{
						if ( me == dummy_fct ) {
						    if ( mec )
							error("%t cannot access%n: %sM",mec,n,n->n_protect?"protected":"private");
						    else
							error("G scope cannot access%n: %sM",n,n->n_protect?"protected":"private");
						} else if (mec && (mef==0 || !::same_class(mec,mef->memof)))
							error("%t cannot access%n: %sM",mec,n,n->n_protect?"protected":"private");
						else
							error("%n cannot access%n: %sM",me,n,n->n_protect?"protected":"private");
						mex = 0;	// suppress further error messages
					}
				}

				if ( n->base == TNAME ) return 0;
				if ( access_only ) return n;

				Pname th = cc->c_this;
				Pexpr r = new ref(REF,th,n);
				if (th) th->use();
				n->use();
				r->tp = t;
				return r;
			}

			if (mex && n->n_scope==0 && n->tp->base!=OVERLOAD) {
				if ( is_accessible(n,this)==0 )
				{
					if ( me == dummy_fct ) {
					    if ( mec )
						error("%t cannot access%n: %sM",mec,n,n->n_protect?"protected":"private");
					    else
						error("G scope cannot access%n: %sM",n,n->n_protect?"protected":"private");
					} else if (mec && (mef==0 || !::same_class(mec,mef->memof)))
						error("%t cannot access%n: %sM",mec,n,n->n_protect?"protected":"private");
					else
						error("%n cannot access%n: %sM",me,n,n->n_protect?"protected":"private");
					mex = 0;	// suppress further error messages
				}
			}
			n->use();
			return n;
		}
	} // if n 

	if ((cl==0 || ::same_class(cl,this))	// not qualified to a base class
	&& csu!=UNION
	&& csu!=ANON
	&& strcmp(s,"__as")==0) { // assignment is special: you cannot inherit it
		if ( baselist==0
		||   baselist->bclass->obj_size!=obj_size ) {
			Pname cn = this->k_tbl->find_cn(string);//SYM
			if ( cn == 0 ) error('i',"CN %s missing inCdef::find_name(%s)",string,s);//SYM
			if (cn->tp->base == COBJ) cn = Pbase(cn->tp)->b_name;
			Pname x = gtbl->look("__as",0);
			if (x) {	// what if there is an (illegal) global assignment operation?
					// hack hack don't declare global assignment operations!
				Pfct f = Pfct(x->tp);
				if (f->base == FCT) {
					Pptr r = f->argtype->tp->is_ref();
					if (r) {
						Pname cnn = r->typ->is_cl_obj();
						if (cnn && cn==cnn) return 0;
					}
				} else {
					for (Plist gl = Pgen(f)->fct_list; gl; gl=gl->l) {
						Pptr r = Pfct(gl->f)->argtype->tp->is_ref();
						if (r) {
							Pname cnn = r->typ->is_cl_obj();
							if (cnn &&cn==cnn) return 0;
						}
					}
				}
			}
			return make_assignment(cn) ? find_name(s,cl) : 0;
		}
	}

	return find_in_base(s, cl, access_only, newflag);
}

static Pclass rootClass;
static Pbcl pubVClass;
static struct PendingMessage { 
    Pbcl bc;
    Pname mf;
    char *nm;
} *pM;

Pexpr classdef::find_in_base(char* s, Pclass cl, int access_only, int newflag)
{
	static  Pbcl bc_found_in = 0;
	int statflag = 0; // is data member static
	Pbcl e_bc = 0, bc = 0;
	Pexpr e = 0;

// error('d',"%s->find_in_base(%s %s)",string,s,cl?cl->string:"");

	if ( me == 0 ) mef = 0;

	if (rootClass == 0) {
		rootClass = this;
		bc_found_in = 0;
	}

	for (Pbcl b=baselist; b; b=b->next) {
		Pclass ccl = ::same_class(cl,this)?0:cl;
// error('d',"try %t %s %k",b->bclass,b->bclass?b->bclass->string:"?", b->base);
		Pexpr ee = b->bclass->find_name(s,ccl,access_only,newflag);

		if (ee) {
			if (!access_only) {	// look for first use (through this)
				if (b!=baselist || b->base==VIRTUAL)
					dcl_print(0);
				else {
					Pexpr ex = ee;
					while ((ex->base==MDOT && ex->i1==1)
					|| (ex->base==REF && ex->e1==cc->c_this)) ex = ex->mem;
					switch (ex->tp->base) {
					case OVERLOAD:
						break;
					case FCT:
						if (Pfct(ex->tp)->f_virtual==0) break;
					default:
						dcl_print(0);
					}
				}
			}

			if (e) {
// error( 'd', "find_in_base: b( %s ): %k %k ",b->bclass->string,b->base,b->ppp);
				Pexpr ex = e;
				int evb = 0; 	// number of vbase indirections

// note that this does not catch enum members
				while (ex->base == MDOT
				|| (ex->base==REF && ex->e1==cc->c_this)) {
					if (ex->base==MDOT) evb += int(ex->i1);
					ex = ex->mem;
				}

				Pexpr eex = ee;
				int eevb = b->base==VIRTUAL; 	// number of vbase indirections (incl. possibly this one)

				while (eex->base == MDOT
				|| (eex->base==REF && eex->e1==cc->c_this)) {
					if (eex->base==MDOT) eevb += int(eex->i1);
					eex = eex->mem;
				}

// error('d', "find_in_base: ex: %k eex: %k, eevb: %d evb: %d", ex->base, eex->base, eevb, evb );
// relying on simple counts for sub-object identification isn't good enough

				if (ex != eex)	{
// error('d',"ex %k tp %k eex %k tp %k", ex->base, ex->tp->base, eex->base, eex->tp->base );
				//	if (!mqua) {
						Pclass ocl, ncl;
						if ( ex->tp->base == FCT ) 
     							ocl = Pfct(ex->tp)->memof;
						else 
						if ( ex->tp->base == OVERLOAD )
							ocl = Pfct(Pgen(ex->tp)->fct_list->f->tp)->memof;
						else ocl = Pclass(ex->n_table->t_name->tp);
//						else ocl = 0;

						if ( eex->tp->base == FCT ) 
     							ncl = Pfct(eex->tp)->memof;
						else 
						if ( eex->tp->base == OVERLOAD )
							ncl = Pfct(Pgen(eex->tp)->fct_list->f->tp)->memof;
						 else ncl = Pclass(eex->n_table->t_name->tp);
//						else ncl = 0;

//						Pclass ocl = Pfct(ex->tp)->memof;
//						Pclass ncl = Pfct(eex->tp)->memof;
						int eb = ocl?ocl->has_base(ncl):0;
						int eeb = ncl?ncl->has_base(ocl):0;

// error('d',"eb %d eeb %d evb %d eevb %d",eb,eeb,evb,eevb);
						if (eb==0 && eeb==0 && !newflag) {
							// different
							error("ambiguous%n and%n",ex,eex);
							break;
						}
						else if (eb) {	// ex dominates
							if (eevb<evb && !newflag) 
								error("ambiguous%n and%n (different sub-objects)",ex,eex);
							else
							if (eevb && ::same_class(rootClass,this) && 
								b->ppp==PUBLIC && pM && 
								(strcmp(b->bclass->string,pM->bc->bclass->string)==0)) {
									delete pM;
									pM=0;
									pubVClass = bc; 
								} // previous private member becomes public
						}
						else {	// eex dominates
							e = ee;
							bc = b;
							if (evb<eevb && !newflag) error("ambiguous%n and%n (different sub-objects)",ex,eex);
						}
						if (evb==0 && eevb==0 && !newflag) {
							error("ambiguous%n and%n (different sub-objects)",ex,eex);
						}
				}
				else if (ex->base==NAME
					&&
					(Pname(ex)->n_evaluated
					||
					Pname(ex)->n_sto==EXTERN
					||
						// static data member ?
					Pname(ex)->n_stclass==STATIC
					||
						// static function member ?
					(ex->tp && ex->tp->base==FCT && Pfct(ex->tp)->f_static))
				) {
				}
				else if (evb==0 && eevb==0) {
//error('d',"e %k",e->base);
					// no virtual base => different
					error("ambiguous%n and%n (no virtualB)",ex,eex);
					break;
				}
				else if ((evb && eevb==0) || (eevb && evb==0)) {
					// only one virtual base => different
					error("ambiguous%n and%n (one not in virtualB)",ex,eex);
					break;
				}
				else if (e_bc && bc_found_in &&
					::same_class(e_bc->bclass,bc_found_in->bclass) &&
					 e_bc->base != bc_found_in->base) {
					    	error("ambiguous%n: (%t both %s and %s)",ex,e_bc->bclass,e_bc->base==VIRTUAL?"virtual":"nonvirtual", bc_found_in->base==VIRTUAL?"virtual":"nonvirtual");
						e_bc = 0; // don't let it repeat
				         }

				if (eevb &&
				    ::same_class(rootClass, this) &&
				    b->ppp == PUBLIC &&
				    pM &&
				    !strcmp(b->bclass->string, pM->bc->bclass->string)) {
					delete pM;
					pM = 0;
					pubVClass = bc;
				}
			}
			else {
				e = ee;
				bc = b;
				if (bc_found_in == 0) bc_found_in = b;
				if (::same_class(rootClass,this)) {
					e_bc = bc_found_in;
					bc_found_in = 0;
				}
			}

			if ((ee->tp->base==FCT && Pfct(ee->tp)->f_static && cl && Pfct(ee->tp)->memof && strcmp(cl->string,Pfct(ee->tp)->memof->string)==0) || (ee->base==NAME && Pname(ee)->n_stclass==STATIC)) 
				statflag = 1;
		}
	}

	if ( ::same_class(rootClass,this) ) {
    		if ( pM ) {
			// deferred until all base classes of ``this'' examined
			Pbcl b = pM->bc;
			char *str = b->bclass->string;
			error("%n cannot access %s: %s is a%kBC",pM->mf,name_oper(pM->nm),str,b->ppp);
        		delete pM; pM=0;
    		}

		if (e == 0) {
    			rootClass = 0;
    		   	pubVClass = 0;
		}
	}

	if (e == 0) return 0;

	if (mex) {
		if ( bc->ppp==PRIVATE || bc->ppp==PROTECTED) {	
			if (::same_class(this,mec) || 
                           (mec && has_friend(mec)) || 
 			   (mef && has_friend(mef)) ||
			   (mec && bc->ppp==PROTECTED && 
				mec->is_base(string) && ppbase != PRIVATE))
				;
			else 
			if ( bc->base == VIRTUAL &&
   			     !::same_class(rootClass,this) ) { 
				// if one instance is public, it dominates
				if ( pubVClass == 0 ||
                                     strcmp(pubVClass->bclass->string,bc->bclass->string)) {
					pM = (PendingMessage *) new char[sizeof(*pM)];
					pM->bc = bc; pM->nm = s;
					if ( me == dummy_fct )
						pM->mf = mec ? mec->k_tbl->k_name : me;
					else if ( mec && (mef==0 || !::same_class(mec,mef->memof)) )
						pM->mf = mec->k_tbl->k_name;
					else
						pM->mf = me;
				}
			}
			else {
				if ( me == dummy_fct ) {
				    if ( mec )
					error("%t cannot access %s: %s is a %k BC",mec,name_oper(s),bc->bclass->string,bc->ppp);
				    else
					error("G scope cannot access %s: %s is a %k BC",name_oper(s),bc->bclass->string,bc->ppp);
				} else if (statflag==0 || strcmp(s,"__nw")==0 || strcmp(s,"__dl")==0) {
				    if ( mec && (mef==0 || !::same_class(mec,mef->memof)) )
					error("%t cannot access %s: %s is a %k BC",mec,name_oper(s),bc->bclass->string,bc->ppp);
				    else
					error("%n cannot access %s: %s is a %k BC",me,name_oper(s),bc->bclass->string,bc->ppp);
				}
				mex = 0;
			}
		}
		else { // public base class
		if ( bc->base == VIRTUAL ) {
			if ( pM && 
			     strcmp(bc->bclass->string,pM->bc->bclass->string)==0 ) {
				delete pM;
				pM=0;
				pubVClass = bc; 
				} // previous private member becomes public
			else pubVClass = bc; // ignore subsequent private instances
	 		}
		}
	}

	if ( ::same_class(rootClass,this) ) {
    		rootClass = 0;
    		pubVClass = 0;
	}

	if (e->base==NAME
	&& Pname(e)->n_stclass==STATIC) {	// static member
		Pname(e)->use();
		return e;
	}
	if (e->base != NAME)
	if (bc->base == VIRTUAL) {	// this->mem => this->Pbclass->mem
		e->mem = new mdot(bc->bclass->string,e->mem);
		e->mem->i1 = 1;
		e->mem->tp = e->mem->mem->tp;
	}
	else if (bc!=baselist) {	// not first base
		if (e->e1 == cc->c_this) {	// this->mem => this->Obcl.mem
			e->mem = new mdot(bc->bclass->string,e->mem);
			e->mem->tp = e->mem->mem->tp;
		}
		else {	// this->p->mem => this->Obcl.p->mem
			Pexpr ee = e;
			while (ee->e1->base == REF) ee = ee->e1;
			ee->mem = new mdot(bc->bclass->string,ee->mem);
			ee->mem->tp = ee->mem->mem->tp;	
		}
	}

	return e;
}

int has_virt(Pclass cl)
{
	if (cl->virt_count) return 1;
	for (Pbcl b = cl->baselist; b; b = b->next)
		if (b->bclass->virt_count || has_virt(b->bclass)) return 1;
	return 0;
}

Pname find_vptr(Pclass cl)
/*
	find virtual function table
	in memtbl or memtbl of ``first bases''
*/
{
//error('d',"find_vptr %t",cl);
	while (cl) {
		Pname vp = cl->memtbl->look("__vptr",0);
		if (vp) return vp;
		Pbcl b = cl->baselist;
		cl = 0;
		for (; b; b = b->next)
			if (b->base == NAME) {
				cl = b->bclass;
				break;
			}
	}
//error('d',"return 0");
	return 0;
}

void make_dummy()
/* a function with no special privileges */
{
        Pname x = new name(".." /*"__static_initializer"*/);
        x->tp = new fct(Pvoid_type,0,1);
        dummy_fct = x->dcl(gtbl,EXTERN);
//	dummy_fct->string = "";
        delete x;
}

void check_visibility(Pname n, Pname q, Pclass cl, Ptable tbl, Pname fn)
/*
	"fn" calls "n" a member function of "cl"
	fn can be zero (for functions called in arguments to static constructors) 
*/
{
// error('d',"check_visibility(%n,%t, %d,%n)",n,cl,tbl,fn);
	if (fn==0) {
                if (dummy_fct == 0) make_dummy();
                fn = dummy_fct;
	}
	Pname nn = new name;
        char* s = n->n_gen_fct_name;    // overloaded name
	nn->string = s?s:n->string;
	nn->n_qualifier = q;
	Pname nx = Pname(find_name(nn,cl,tbl,REF,fn)); // nn deleted by find_name
	if (nx->tp->base != OVERLOAD) return;

	// can we get here?

	// overloaded not checked by find_name()
	// (since it looks for NAMEs not functions)

	for (Plist gl=Pgen(nx->tp)->fct_list; gl; gl=gl->l) {
		Pname nn = gl->f;
		if (n == nn) {
			if (nn->n_scope) return;	// public member

			Pname ome = me;
			Pfct omef = mef;
			Pclass omec = mec;
			Pclass otcl = tcl;

			Pclass ncl = Pclass(nn->n_table->t_name->tp);
			Pname fncn = fn->n_table->t_name;

			me = fn;
			mec = fncn?Pclass(fncn->tp):0; 
			if ( cc->cot && cc->cot->lex_level > fn->lex_level )
				mec = cc->cot;
			Pfct f = mef = Pfct(fn->tp);
			tcl = cl;

			int ok = is_accessible( n, ncl, 1 );

			// restore global values
			mef = omef; mec = omec; tcl = otcl; me = ome;

			if ( ok ) return;

			if ( Cdcl && Cdcl->base == NAME && 
				Cdcl->n_stclass == STATIC && 
				Cdcl->n_initializer && 
				Cdcl->n_qualifier )
			{
				Pbase bn = Pbase(Cdcl->n_qualifier->tp);
				Pclass ccl = Pclass(bn->b_name->tp);

				if ( same_class(ccl,cl) || ccl->has_friend(f) ||
          				(n->n_protect && ccl->has_base(ncl)))
						return;
			}

			if ( fn == dummy_fct ) {
			    if ( mec )
				error("%t cannot access%a: %sM",mec,nn,nn->n_protect?"protected":"private");
			    else
				error("G scope cannot access%n: %sM",nn,nn->n_protect?"protected":"private");
			} else if ( mec && (mef==0 || !same_class(mec,mef->memof)) )
				error("%t cannot access%a: %sM",mec,nn,nn->n_protect?"protected":"private");
			else
				error("%a cannot access%a: %sM",fn,nn,nn->n_protect?"protected":"private");
			return;
		}
	}
	error('i',"visibility check failed");
}
