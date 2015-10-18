/*ident	"@(#)cls4:src/template.c	1.72" */
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
/******************************************************************************
*    Copyright (c) 1989 by Object Design, Inc., Burlington, Mass.
*    All rights reserved.
*******************************************************************************/
/*******************************************************************
* template.c
*
* TBD
* 
* 1) The template copying process could probably be speeded up 
* substantially, by only placing "graph-like" nodes in the hash 
* table. The current implementation plays it safe, and places 
* all nodes in the hash table.
* 			
* 2) remove `$' in lex.c
* 							
* 3) Add something to lalex to handle x<y<int>>.  Currently parsed
* as a right shift rather than nested template instance.
*********************************************************************/
#include "tree_copy.h"
#include "cfront.h"
#include <string.h>
#include "template.h"
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include "hash.h"

extern int bound; // is not mentioned in the header file
bit tempdcl;
static Ptempl_inst dummyinst=0;
static Ptempl_inst curr_inst=0;
static Pfunct_inst fdummyinst=0;
static Pfunct_inst fcurr_inst=0;
static bit notinstflag=0;
//static bit matchflag=0;

Ptable bound_expr_tbl;
const int max_string_size = 1024;
const int default_copy_hash_size = 1000;

const int MAX_INST_DEPTH = 16; // maximum instantiations at one time

// static data member definitions
Pfunt templ_compilation::f_list=0;
Pbase_inst basic_inst::head=0;      
Ptempl templ_compilation::list=0;
Pcons templ_compilation::last_cons=0;
Pcons templ_compilation::last_friend_cons=0;
Pcons templ_compilation::templ_refs=0; 
Pcons templ_compilation::friend_templ_refs=0; 
Ptstate templ_compilation::save_templ=0;
Ptempl_base templ_compilation::parsed_template=0; 
Pfunt templ_compilation::f_owner=0;
Ptempl templ_compilation::owner=0;  
Plist templ_compilation::param_end=0;
Plist templ_compilation::params=0;
Pexpr templ_compilation::actuals=0;
bool templ_compilation::formals_in_progress=false;

// The canonical template compilation instance. 
templ_compilation *templp;
Ptable templ_compilation::templates=0;

// Save and restore global state around a template instantiation
void 
state::save() {
  	Cdcl = ::Cdcl;
  	Cstmt = ::Cstmt;
  	curloc = ::curloc;

  	curr_file = ::curr_file;
  	curr_expr = ::curr_expr;
  	curr_icall = ::curr_icall;
  	curr_loop = ::curr_loop;
  	curr_block = ::curr_block;
  	curr_switch = ::curr_switch;

  	bound = ::bound ;   
  	inline_restr = ::inline_restr;
  	last_line = ::last_line;
}

void 
state::restore() {
  	::Cdcl = Cdcl;
  	::Cstmt = Cstmt;
  	::curloc = curloc;

  	::curr_file = curr_file;
  	::curr_expr = curr_expr;
  	::curr_icall = curr_icall;
  	::curr_loop = curr_loop;
  	::curr_block = curr_block;
  	::curr_switch = curr_switch;

  	::bound = bound;   
  	::inline_restr = inline_restr;
  	::last_line = last_line;
}

void 
state::init() {
  	::bound = 0;
  	::inline_restr = 0;
  	// lastline  needs to be initialized probaly via a call to putline
}

#if 0
bit 
basetype::parametrized_class() { 
    	return ((base == COBJ) && 
		Ptclass(Pbase(this)->b_name->tp)->class_base==UNINSTANTIATED);
}

bit 
classdef::parametrized_class() { 
	return (class_base == UNINSTANTIATED);
}
#endif

Templ_type
get_class_base(Pbase b) {
	if (b->base != COBJ) 
		error('i',"::get_class_base: badAT(%k) cobjX",b->base);
  	return Ptclass(Pbase(b)->b_name->tp)->class_base ;
}

Templ_type
get_templ_base(Pbase b) {
        if (b->base != COBJ)
                error('i',"::get_templ_base: badAT(%k) cobjX",b->base);
        return Pclass(Pbase(b)->b_name->tp)->templ_base;
}

Ptclass 
get_template_class(Pbase b) {
	Templ_type t = get_class_base(b) ;
  	if (! ((t==INSTANTIATED) || (t==UNINSTANTIATED)))
  		error ('i', "C is not aYC");
  	return Ptclass(Pbase(b)->b_name->tp);
}

#if 0
Ptempl_inst 
get_templ_inst(Pbase b) {
	return (get_template_class(b))->inst;
}
#endif

static bit 
same_class_templ(Pclass c1, Pclass c2) 
{
// error('d',"same_class_templ: c1 %s c2 %s c1 %d c2 %d", c1->string, c2->string, c1->class_base, c2->class_base);
	if (c1 == c2) return true;

  	if ((c1->class_base == INSTANTIATED) &&
      		(c2->class_base == UNINSTANTIATED) &&
      		(Ptclass(c1)->inst->def== Ptclass(c2)->inst->def))
    			return true;

  	if ((c2->class_base == INSTANTIATED) &&
      		(c1->class_base == UNINSTANTIATED) &&
      		(Ptclass(c1)->inst->def== Ptclass(c2)->inst->def))
    			return true;

  	return false;
}

bit 
classdef::same_class(Pclass pc,int access)  
/* Predicate to determine whether two classes are indeed the same. 
 * cfront normally relies on pointer identity, however, this test 
 * is insufficient when parametrized class instantiationa are involved,
 * since there are potentially many instances of a COBJ and CLASS 
 * for a given instantiation.
 *
 * it would seem reasonable to always have the occurrence of 
 * an UNINSTANTIATED and INSTANTIATED instance be checked;
 * this occurs, for example, in
 *	template<class T> class Tag{};
 *	template<class T> struct Shrub {
 *      	struct Link { Link* next; };
 *      	int advance(Tag<T>&); // UNINSTANTIATED
 *	};
 *
 * 	void f() {
 *        	Shrub<short> s; Tag<short> t;
 *        	s.advance(t); } // INSTANTIATED -- same class!
 *
 * however, it seems that same_class also determines whether or not
 * an instantiation needs to be done??? and so currently making this
 * a blanket check causes certain classes under certain conditions
 * not to be instantiated -- if this is truly the case, this should
 * be broken into two functions
 */
{

// error('d',"%t->same_class(%t, %d)",this,pc,access);
	if (this == 0 || pc == 0 ) return false;
	if (this == pc) return true;

	if (class_base==VANILLA && pc->class_base==VANILLA)
	{
		if (local_sig && pc->local_sig)
		{
			return strcmp(local_sig, pc->local_sig)==0;
		}
		if (nested_sig && pc->nested_sig)
		{
			return strcmp(nested_sig, pc->nested_sig)==0;
		}
		if (local_sig==pc->local_sig && nested_sig==pc->nested_sig)
		{
			return strcmp(string, pc->string)==0;
		}
		return false;
	}
		
        Templ_type this_base = this->class_base;
        Templ_type pc_base = pc->class_base;

  	if ((this_base == CL_TEMPLATE) &&
      		(pc_base == INSTANTIATED) &&
      		(Ptclass(pc)->inst->def_basetype()->b_name->tp==this))
    			return true;

  	// The inverse symmetric test
  	if ((pc_base == CL_TEMPLATE) &&
      		(this_base == INSTANTIATED) &&
      		(Ptclass(this)->inst->def_basetype()->b_name->tp==pc))
    			return true;

  	// Check whether the templates were determined to be 
	// identical after instantiation. 
  	if ((pc_base == INSTANTIATED) &&
      		(this_base == INSTANTIATED) &&
		(strcmp(pc->string,string)==0 ||
      		(Ptclass(this)->inst->same(Ptclass(pc)->inst))))
    			return true;
       
        if (access == 0) return false;

        // change to treatment of friend instantiation
        // now makes this a possibility
        if (this_base == INSTANTIATED && 
	    pc_base == UNINSTANTIATED) {
		char *str = Ptclass(this)->inst->def->namep->string;
// error('d',"same_class: this: %s pc: %s", str, pc->string);
		return (strcmp(str,pc->string)==0);
        }

        if (pc_base == INSTANTIATED && 
	    this_base == UNINSTANTIATED) {
		char *str = Ptclass(pc)->inst->def->namep->string;
// error('d',"same_class: pc: %s this: %s", str, string);
		return (strcmp(str,string)==0);
        }
  	return false;
}

bool 
templ_inst::same(Ptempl_inst t) 
/* determine whether two instantiations are identical; 
 * test asumes that the templates have been instantiated. */
{
	return ((forward && (forward==t->forward)) 
		|| (forward == t) 
		|| (t->forward == this)) ? true : false;
}

Ptempl 
templ_compilation::is_template(Pname p)
/* Determine whether the name refers to the canonical
 * template class during syntax analysis */
{
// error('d',"is_template(%n)",p );
        if (p->tp && (p->tp->base==COBJ))
        {
                Templ_type tc = get_class_base(Pbase(p->tp));
                Templ_type sc = get_templ_base(Pbase(p->tp));
                
                if (tc == CL_TEMPLATE ||
                   (tc == INSTANTIATED && sc == CL_TEMPLATE)) {
                        Pname n = templates->look(p->string,0);
                        return (n ? Ptempl(n->tp) : 0);
                }
                else return 0;
        }
        return 0;
}

Ptempl 
templ_compilation::is_template(char *s) 
{ 
// error('d',"is_template(char *%s)",s );
	Pname n = templates->look(s,0);
	return (n ? Ptempl(n->tp) : 0);
}

Pfunt 
templ_compilation::is_template(char *s, TOK t) 
{ 
// error('d',"is_template(char *%s, %k)",s,t );
	Pname n = templates->look(s,t);
	return (n ? Pfunt(n->tp) : 0);
}

void 
templ_compilation::start() 
/* Set up the environment for parsing a template. 
 * This involves setting up a new nesting level into 
 * which the "type type" template parameters can be
 * entered so that the lexer can find them as TNAMEs. 
 * templ_compilation::collect adds new types.
 */
{ 
	templp->in_progress = true ;
  	params = param_end = 0; 
	owner = 0; f_owner = 0;
	parsed_template = 0;
	// has_expr_formals = 0;
  	// SYM modified_tn = 0;  
}

void 
templ_compilation::collect(TOK parm_type, Pname n)
/* Collect each parameter as it is parsed, adding 
 * it to the list of parms. Validate each parameter */
{
// ::error('d',"templ_compilation::collect(%k,%n)",parm_type,n);

	switch (parm_type) {
	case STRUCT:
  	case CLASS:
    		/* A "type type" parameter ... 
		 * tp: ``any_type'', a wildcard match 
		 * tdef: base==TNAME, inserts in ktbl, sets modified_tn
		 * lex_level: +1, so restore() can HIDE it after 
		*/
    		n->tp = ::any_type; 
    		n = n->tdef(); 
    		n->lex_level = bl_level+1; 
    		n->n_template_arg = template_type_formal;
    		break;
  	default:
		error("theZT for%n must beC or struct, not%k", n, parm_type);
		break;
  	}
  	append_parameter(n);
}

void 
templ_compilation::append_parameter(Pname n)
{ // append the "non-type" parameter to the end of the list
  // ::error('d',"templ_compilation::append(%n)",n);
	if (params) {
		param_end->l = new name_list(n,0);
    		param_end = param_end->l;
  	}
	else params = param_end = new name_list(n,0);
  	PERM(n); PERM(n->tp);
}

void 
templ_compilation::collect(Pname n) 
/* Collect non "type type" parameters. 
 * tp indicates type of the formal parameter */
{
// ::error('d',"templ_compilation::collect(%n)",n);
  	// grammar should be sufficient to protect against undesirable 
	// types. Any additional checks go here.
  	n->n_template_arg = template_expr_formal;
	extern int must_be_friend;
        if (must_be_friend == 0)
        	bound_expr_tbl->insert(n,0);
  	append_parameter(n) ;
}
  
static void 
check_non_type_formal(Pname n) 
{ // validate the type for a non-type formal 
// error('d',"check_non_type_formal: %n %k", n, n->tp->base);

	Ptype tp = n->tp->skiptypedefs();
  	switch (tp->base) {
		// case W_CHAR:
		// case W_STRING:
  		case FLOAT:
  		case DOUBLE:
		case LDOUBLE:
		    if (tp->base==LDOUBLE && ansi_opt==0) 
			error('w',"long double supported under ``+a1'' option only, generating ``double%n''",n);
		    error('s',"ZE ofT float, double, or long double");
		    return;

  		case ZTYPE:
  		case CHAR:
  		case SHORT:
  		case INT:
  		case LONG:
  		case FIELD:
  		case EOBJ:
  		case COBJ:
  		// case TYPE:
  		case ANY:
    		{ // a basetype node
      			TOK bad_base = 0;
      			if (Pbase(n->tp)->b_volatile) bad_base=VOLATILE;
			// ? ?? ??? ???? ?????
      			// if (Pbase(n->tp)->b_typedef) bad_base=TYPEDEF;
      			if (Pbase(n->tp)->b_inline) bad_base=INLINE;
      			if (Pbase(n->tp)->b_virtual) bad_base=VIRTUAL;
      			if (bad_base)
				error ("bad %k declarator forY formal %n", bad_base,n);
			goto hack;
      
    		}
  		case PTR:
    		{ 
			if (tp != n->tp) { 
		hack:
      			    Pbase b = new basetype(0,0);
      			    *b= *Pbase(n->tp);
			    // see const_formal_hack() for explanation
			    if (b->b_const == 0) b->b_const = 2;
      			    n->tp = b;
      			    break;
			}

			Pptr b = new ptr(0,0);
       			*b = *Pptr(n->tp);
			if (b->b_const == 0) b->b_const = 2;
      			n->tp = b;
      			break;
    		}
  		case RPTR:
  		case VEC:
    			break;		// constant by definition
  		default:
    			error("badZT%t for formalZ %n",tp,n);
  	}
  	return;
}

void 
templ_compilation::enter_parameters()
/* The template parameters, if any, have been parsed. 
 * Member function templates may choose to default their 
 *     template arguments to the class arguments -- if so,
 *     make the defaulting happen 
 */
{
	for (Plist list = params; list; list = list->l) {
    		Pname n = list->f;
    		switch(n->n_template_arg) {
    			case template_type_formal:
      				// SYM -- param should be in parse table...
				// Bring the names out of hiding
      				n->n_key = 0;
      				break;

    			case template_expr_formal:
      				check_non_type_formal(n);
      				n->tp->dcl(gtbl);
      				break;
    			default:
      				error ('i', "bad template formal" ) ;
    		}
  	}
  	// SYM -- tn stuff no longer needed 
  	// param_tn = modified_tn ;
  	// modified_tn = 0 ;
}

void 
templ::resolve_forward_decl(Plist params, Pclass c) 
/* Resolve the forward declaration of a template to its 
 * true definition. The template and class type data 
 * structures must be reused, since there may be 
 * outstanding references to them. */
{
  	check_formals(params); 
  	formals = params;
  	defined = true;
  	members = c->mem_list;
}

static bit reinstat = 0;
/* 3.1/4.0: merge there two */
void 
templ::instantiate_forward_decl() {
	for (Ptempl_inst i = insts ; i ; i = i->next)
    		if (Ptclass(Pbase(i->tname->tp)->b_name->tp)->class_base ==
			INSTANTIATED && ! i->forward)
      		{ // reinstantiate it
			reinstat = 1;
			i->instantiate(true) ;
			reinstat = 0;
      		}
}

void 
function_template::instantiate_forward_decl() {
	for (Pfunct_inst i = insts ; i ; i = i->next)
    		if (Ptfct(i->tname->tp)->fct_base == INSTANTIATED)
      		{ // reinstantiate it
			i->instantiate(true) ;
      		}
}

// verify that the qualifier used to decl the member function matches the
// template arguments in name, ie.
// template <class P, class Q, ..> c<P,Q,..>::member_function() {}
// match it's Ps and Qs.
bool 
templ_inst::check_qualifier(Plist formals)
{
  Pexpr actual = actuals ;
  for (Plist formal = formals ; formal && actual ; formal = formal->l,
       actual = actual->e2)
    
    switch (formal->f->n_template_arg) {
      
    case template_type_formal:
      { Pbase b = Pbase(actual->e1->tp) ;
	
	if (! ((b->base == TYPE) &&
	       (b->b_name->base == TNAME) &&
	       (strcmp (Pname(b->b_name)->string, formal->f->string) == 0)))
	  return false ;
	break ;
      }
      
    case template_expr_formal:
      if (! ((actual->e1->base == NAME) &&
	     (strcmp(Pname(actual->e1)->string, formal->f->string) == 0)))
	return false ;
      break ;
      
	default:
      		error ('i',"bad template formal") ;
    }

  return true ;
}

extern int add_first; 
void 
introduce_global_name( Pname ft ) 
{
// error( 'd', "introduce_global(%n)", ft );
	Pname n = gtbl->look(ft->string,0);
	if ( n == 0 ) {
		Pname nn = gtbl->insert(new name(ft->string),0);
		nn->n_template_fct = 1;
		if (ft->n_sto==FRIEND) {
			nn->n_scope = EXTERN;
		}
		nn->tp = ft->tp;
		PERM(nn);
	}
	else {
// error( 'd', "introduce_global(%n) n: %n %t", ft, n, n->tp );
		switch( n->tp->base ) {
		default: 
			error("YF%n renamed object ofT%t",ft,n->tp);
			break;
		case OVERLOAD: {
			Pname n2 = Pgen(n->tp)->add(ft);
			n2->n_template_fct++;
			n->n_template_fct++;
			}
			break;
		case FCT: {
			Pgen g = new gen;
			PERM(g);
			add_first = 1;
			(void) g->add(n);
			add_first = 0;
			Pname n2 = g->add(ft);
			n2->n_template_fct++;
			n->tp = g;
			n->n_template_fct++;
			}
			break;
		}
	}
}

void 
templ_compilation::introduce_class_templ(Pname namep) 
/* make the class template visible when compiling the 
 *      template class definition, 
 * so that it can be referenced while compiling the class body.  */
{
// error('d',"introduce_class_templ: %n", namep );
	/* this check is because introduce_call_templ is invoked in two places:
 	 * st_class() in norm.c for class definition, and templ_compilation::end()
 	 * call at end() is always for a forward declaration 
	*/
  	owner = is_template(namep);
  	if (!owner){
		check_formals_for_dups(params); // place for it??

		// create a template definition
    		owner = new templ(params, namep);
		Pname nn = new name(namep->string);
    		Pname lookup_name = templp->templates->insert(nn,0);
    		lookup_name->tp = Ptype(owner);  // lie, to permit use of the table
  	}
}
			    
void 
templ_compilation::introduce_funct_templ(Pname namep) { 
/* make the function template visible when compiling the 
 *      template function definition, 
 * so that it can be referenced while compiling the function body.
 */
// error('d',"introduce_funct_templ: %n", namep );
	Pfunt t = new function_template(params,namep);
	Pname n = templp->templates->look(namep->string,FCT);

	if ( n == 0 ) {
		Pname nn = new name(namep->string);
    		n = templp->templates->insert(nn,FCT);
	}
	else t->gen_list = Pfunt(n->tp);
	n->tp = Ptype(t);
	f_owner = t;
	introduce_global_name(namep);
}
			    
static int
has_formal_type(Ptclass pt_cl, Plist list)
{	
// error('d',"has_formal_type: %s", pt_cl->string);
	int has_formal = 0;
	for (Pexpr formals = pt_cl->inst->actuals; 
		formals; formals=formals->e2) 
	{
		Pexpr fe = formals->e1;
		if (fe->base != NAME || !fe->tp || fe->tp->base != TYPE) continue;
		Pname tn = fe->tp->bname();
		if (!tn->is_template_arg()) continue;
// error('d',"has_formal_type: found: %n", tn);
		list = list->l = new name_list(tn,0);
		++has_formal;
	}
	return has_formal;
}

static Pname Tmpl;

static int
has_formal_type(Pname nn, Plist list)
{	
// error('d',"has_formal_type: %n %t", nn, nn->tp);
	Pname bn;
	Ptype t, nt = nn->tp;

	while(t=nt->is_ptr()) nt = Pptr(t)->typ; 
	while(t=nt->is_ref()) nt = Pptr(t)->typ; 

	if(nt->base != TYPE) return 0;
	while (nt->base == TYPE) {
		bn = nt->bname();
		nt = bn->tp;
	}

	if (nt->base == COBJ) { // declaration:  X<T,...>
		Pclass c1 = nt->classtype();
		if (c1->is_templ_instance()) 
		   	return has_formal_type(Ptclass(c1),list);

		// encapsulate later ...
		if (c1->class_base == CL_TEMPLATE) {
// error('d',"class template: %n set to tmpl", bn);
			Tmpl = bn;
			return -1;
		}
	}

	if (bn->is_template_arg()) {
// error('d',"has_formal_type: found: %n", bn);
		list = list->l = new name_list(bn,0);
		return true;
	}
	return false;
}

static bit hbf = 0;

void
handle_bound_friend(Ptempl_base pb, Pname fn)
{ /* point of this is to extrapolate the template function 
   * declaration implicit within a declaration such as  
   *	friend min(X<T>,X<T>);
   */
// error('d',"handle_bound_friend (%n %n)", Ptempl(pb)->namep, fn);
	Plist formals, f_list = new name_list(0,0);
	int i = 0, formal_cnt = pb->get_formals_count();
	Pfct f = fn->fct_type();
	if (f->body) return; // no need to extrapolate

	struct bleh { Pname n; int used; }; 
	bleh *pbleh = new bleh[formal_cnt];

        for (formals = pb->get_formals(); formals; formals=formals->l) {
		pbleh[i].n = formals->f; pbleh[i++].used = 0;
	}

	int has_formal = 0;

	for (Pname n = f->argtype; n; n=n->n_list) 
		// build up f_list of formals in signature
		has_formal += has_formal_type(n,f_list); 

	if (!has_formal) return;

        // *** XXX : incomplete: what if f(pb<...>,T,...)
	if (has_formal == -1) { // f(pb<...>
		if (strcmp(Tmpl->string,Ptempl(pb)->namep->string))
			return;
		// pb is formal argument to function: all formals used
// error('d',"has_formal == -1, %n == %n",Tmpl,Ptempl(pb)->namep);
	}
	else 
        for (formals=f_list->l; formals; formals=formals->l) {
		Pname n = formals->f;
		if (!n) break; 
                for (i = 0; i < formal_cnt; ++i )
		    if (strcmp(n->string,pbleh[i].n->string)==0) {
			pbleh[i].used++;
// error('d',"match: %n used: %d", pbleh[i].n, pbleh[i].used);
			break;
		    }
        }

	templp->save_templ = new templ_state;
  	templp->params = templp->param_end = 0;  
	for (i = 0; i < formal_cnt; ++i ) {
    		if (pbleh[i].used == 0 && has_formal != -1) continue;
    		templp->append_parameter(pbleh[i].n);
// error('d',"i %d t_list: %n", i, pbleh[i].n);
	}

	// unnecessary when class and functions share owner
	templp->owner = 0; // restored by delete, below
        hbf = 1;
	templp->end(fn);
	f->fct_base = FCT_TEMPLATE;

	Ptempl_base fr = templp->parsed_template; // set to f_owner
	delete templp->save_templ; templp->save_templ = 0;

	Pclass cl = Ptempl(pb)->classtype();
        cl->templ_friends = new cons(fr,cl->templ_friends);
	fr->extrapolated = 1;
	if (hbf == 1) // set to 2 within templ_compilation::end()
		fr->templ_refs = templp->friend_templ_refs;
	hbf = 0;
// error('d',"handle_bound: cl %t is_extrap: %d templ_refs: %d", cl?cl:0, fr->is_extrapolated(), fr->templ_refs);
}

void check_templ_funct(Pname fn)
{
// error('d', "check_templ_funct( %n ): n_oper: %k",fn,fn->n_oper);
	Pfct f = Pfct(fn->tp);

	for (Pname al = f->argtype; al; al = al->n_list)
	{
		if (al->n_initializer)
		{
			error('s', "FYs do not support defaultAs");
			break;
		}
	}

	if (f->nargs_known != 1)
		 error('s',"ellipsis (...) inA list ofYF%n",fn);

        if ( fn->n_oper ) 
        {
	    switch (f->nargs) {
		case 1: case 2: break;
		default:
			if (fn->n_oper!=NEW)
				error("FY%n must take 1 or 2As",fn);
	    }
	
	    switch (fn->n_oper) {
	    case CALL:
	    case DEREF:
	    case REF:
	    case ASSIGN:
		error("YF operator%s must be aCM",keys[fn->n_oper]);
		return;
	    case DELETE:
		error("::operator %s may not be aYF",keys[fn->n_oper]);
		return;	
	    default:
		fn->check_oper(0);
		break;
	    }
	}
	
	if (strcmp("main",fn->string)==0)
		error("main() may not be aYF");
}

#if 0
static void 
display_templ_refs() 
{
  error('d',"display_templ_refs :: templp\n");

  	for (Pcons p = templp->templ_refs; p; p = p->cdr) {
		Ptempl_inst pt = Ptempl_inst(p->car);
		if ( pt == 0 ) { error('d',"templ_refs: empty (%d)",p->car); continue; }
  error('d',"\ntempl_refs: %d->car:\n",p->car); 
  error('d',"\ttname: %n namep: %n",pt->get_tname(),pt->get_namep());
  error('d',"\tdef: %n refp: %d\n", pt->def->namep, pt->refp);
	}
}

static void 
display_templ_refs( basic_template *bt ) 
{
  error('d',"display_templ_refs :: basic_template\n");

  	for (Pcons p = bt->templ_refs; p; p = p->cdr) {
		Ptempl_inst pt = Ptempl_inst(p->car);
		if ( pt == 0 ) { error('d',"templ_refs: empty (%d)",p->car); continue; }
  error('d',"\ntempl_refs: %d->car:\n",p->car); 
  error('d',"\ttname: %n namep: %n",pt->get_tname(),pt->get_namep());
  error('d',"\tdef: %n refp: %d\n", pt->def->namep, pt->refp);
	}
}
#endif

void 
templ_compilation::end(Pname p) 
/* The body of the template has been parsed. 
 * Finish the definition of the template class */
{
//error('d',"templ_compilation::end(%n) tp %k",p,p->tp?p->tp->base:0);

  	// Restore the name state to that prior to 
        // the processing of the template parameters

	//SYM -- need to restore tables... XXXXX ???
	// since the lex_level of these paramenters was lex_level+1
	// each will be hidden by restore() (n_key<==HIDDEN)
  	// modified_tn = param_tn;
  	// restore(); 
  	// modified_tn = 0;

	bool forward_definition = false;
	if (!p->tp) {
      		error ("aC,F,MF or static dataM definition wasX");
      		return;
    	}

    	switch(p->tp->base) {
	case COBJ:
		if ( p->n_sto == FRIEND ) {
			p = p->tp->bname();
// error('d',"p %n tp %k ", p, p->tp->base );
			// no break;
		}
		else { // should not happen: i.e., caught in grammar
			error("illegalY %n",p);  
			return; 
		}

    	case CLASS:
      	/* Create the template type to represent the parsed template, 
	 * entering it into the global table. This is achieved simply 
	 * by modifying the TNAME that was entered into ktbl to 
	 * represent the class definition. 
	 */
		{
   		// Pname namep = ktbl->look(p->string,0);
          	// Pname namep = k_find_name(p->string,Ctbl,0);//SYM
          	Pname namep = k_find_name(p->string,Gtbl,HIDDEN);//SYM
          	if ( namep == 0 || namep->base==NAME ) // namep = 0;
                {
			char* s = (p->string ? p->string : "");
			if (*s && (s[0] != '_' || s[1] != '_' || s[2] != 'C'))
				error("nestedYC %s", s);
			else {
				error("missingYN");
				return;
			}
                }

      		owner = is_template(namep);
      		if (owner) {
			Pclass c = owner->classtype();
			// ignore it, if it is a forward declaration 
			// following a real definition
			if (owner->defined && 
				(Pclass(p->tp)->mem_list != owner->members))
	  				error("YC %s multiply defined", p->string);
			forward_definition = bool((c->defined & DEF_SEEN) && (!owner->defined));
			if (forward_definition) 
				owner->resolve_forward_decl(params, c);

                        if (pt_opt && forward_definition)
                                fprintf(pt_file,"t %s %s\n",p->string,curr_filename());
      		}
		// a forward declaration
		else introduce_class_templ(namep);

		if (owner->defined) {
      		    for (Pname nn=Pclass(p->tp)->mem_list; nn; nn=nn->n_list)
		    { // look for implicit non-member template friend functions
			if (nn->base == NAME && 
				nn->n_sto == FRIEND &&
				nn->n_qualifier == 0)   
			{
	  			switch(nn->tp->base) {
	  			case FCT:
// error('d',"fct friend %s",nn->string);
					handle_bound_friend(owner,nn);
	      				break;
	    			case OVERLOAD:
// error('d',"overload friend %s",nn->string);
	      				break;
	  			}
			}
		}
}
      		if (templ_refs) owner->templ_refs = templ_refs;
		templp->clear_friend_ref_templ();
      		break;
		}
    
    		case FCT: 
		{ 
                        if (pt_opt && !p->fct_type()->body && !p->n_qualifier)
			    fprintf(pt_file,"f %s %s\n",p->string,curr_filename());

			Pname qual = p->n_qualifier;
			if (!qual) { // function template
				check_funct_formals(params,p);
				check_templ_funct(p);
				f_owner = is_template(p->string,FCT);
				if ( !f_owner )
					introduce_funct_templ(p);
				else {
					Pfunt tl = f_owner;
					Pname fn = 0;
					Pfct n_fct = p->fct_type();

					// is it an overloaded instance
					int error_cnt = 0;	
					for (fn=tl->fn; tl; tl=tl->gen_list,fn=tl?tl->fn:0) {
					    extern bit return_error; // set in type::check
					    if (n_fct->check(fn->tp,PT_OVERLOAD)==0) break;
					    if (error_cnt = return_error) break;
					}
					if (error_cnt)
					    error("FY%n: declared twice with different returnTs",p);

// error('d',"after gen_list: tl: %d", tl );
					if (tl==0) // overloaded instance
						introduce_funct_templ(p);
					else { // redefinition
						int f1, f2, extrap = 0;
						Pfct o_fct = fn->fct_type();
						if ((f1=n_fct->body!=0) && (f2=o_fct->body!=0))
							error("two definitions ofYF%n (%t %t)",fn,o_fct,n_fct);
						// replace entry in template table for a forward definition
						// or if original definition is extrapolated friend function
						extrap = f_owner->is_extrapolated();
						if ((forward_definition = (bool)(f1 && f2==0)) || extrap)
						{
							tl->formals = params;
							tl->fn = p;
							tl->templ_refs = templ_refs;
							f_owner = tl;
						}
                                                else if (hbf == 1) hbf = 2;
					}
				}
				if (f_owner->templ_refs == 0 && hbf != 2)
				    f_owner->templ_refs = templ_refs;
				break;

			}
	
			Pbase q = Pbase(qual->tp);
			if (q && (q->base == COBJ)) 
	  			switch (get_class_base(q)) {
	  			case UNINSTANTIATED:
	    				owner = Ptclass(q->b_name->tp)->inst->def;
	    				/* verify that the formals specified match the 
				 	 * template formals in name, note that the length 
				 	 * was already matched when the instantiation was 
				 	 * generated 
					 */
	    				if (!get_template_class(q)->inst->check_qualifier(params))
	      					error ("QrZs must match theY formalZs");
	  				break;
	  			case CL_TEMPLATE:
	    			// the template reference was without any of the formals
	      				error("Qr%n for%n must specifyYZs",qual,p);
	    				break;
	  			default:
                                        {
                                        Pclass cl = (Pclass)q->b_name->tp;
                                        if (cl && cl->in_class && cl->in_class->class_base)
                                                error('s',"out-of-line definition ofMF ofC nestedWinYC (%t::%n)",cl,p);
                                        else error ("Qr%n for%n wasn't aYC",qual,p);
                                        return;
                                        }
	  			}
			if (! p->fct_type()->body) 
				error ("QdN%n::%n inYFD",qual,p);
			if (owner)
			{
				Pfunt ft= owner->collect_function_member(p);
				ft->templ_refs = templ_refs;
				ft->formals = params;
				owner->check_formals(params);
			}
			else
				error("memberFY must beQd byCYN");
			break;
		}

    		default: 
		{ 
			Pname qual = p->n_qualifier;
			if ( !qual ) {
	    			error ("%n: only static dataCMs may beZized",p);
	    			return;
			}
			// *** SDM yes, this is currently duplicated from above
			Pbase q = Pbase(qual->tp);
			if (q && (q->base == COBJ))
	  			switch (get_class_base(q)) {
	  			case UNINSTANTIATED:
	    				owner = Ptclass(q->b_name->tp)->inst->def;
	    				if (!get_template_class(q)->inst->check_qualifier(params))
	      					error ("%n: QrZs must match theY formalZs",p);
	  				break;
	  			case CL_TEMPLATE:
	    			// the template reference was without any of the formals
	      				error("Qr%n for%n must specifyYZs",qual,p);
	    				break;
	  			default:
	    				error ("Qr%n for%n wasn't aYC",qual,p);
	    				return;
	  			}
			Pdata dat = owner->collect_data_member(p);
			dat->templ_refs = templ_refs;
			dat->formals = params;
			owner->check_formals(params);
			break;
		}
	}

  	// Note the template references from this definition
  	clear_ref_templ();
  	param_end = params = 0;  // Indicates the end of template processing.
  	if (forward_definition) {
/* XXX: 
 *	 make this one line of code based on
 *       owner being a basic_template pointer
 */
		if (owner) 
			owner->instantiate_forward_decl();
		else {
			f_owner->instantiate_forward_decl();
		}
	}	

	parsed_template = owner?(Ptempl_base)owner:(Ptempl_base)f_owner;
  	owner = 0;
	f_owner = 0;
}

void 
templ_compilation::clear_friend_ref_templ() {
  	for (Pcons p = friend_templ_refs; p; p = p->cdr)
    		Ptempl_inst(p->car)->friend_refp = false;
  	friend_templ_refs = 0; 
	last_friend_cons = 0;
}

void 
templ_compilation::clear_ref_templ() {
/* Clear list of templates referenced during syntax analysis of a top
 * level definition.  Note that since this list is produced during syntax
 * analysis, it does not recognize instantiations that may actually turn 
 * out to be identical at instantiation after the substitution of actual 
 * parameters.  Thus, the list may be longer than it would be after the 
 * substitution of the actual argument
 */
  	for (Pcons p = templ_refs; p; p = p->cdr)
    		Ptempl_inst(p->car)->refp = false;
  	templ_refs = 0; 
	last_cons = 0;
}

void 
templ_compilation::instantiate_ref_templ() 
/* Instantiate the class templates that were referenced 
 * by a non-template definition, after compleletion of
 * syntax analysis on that definition */
{
  	for (Pcons p = templ_refs; p; p = p->cdr) {
		if (p->cdr)
			notinstflag=1;
		else 
			notinstflag=0;
    		Ptempl_inst(p->car)->instantiate();
	}
  	clear_ref_templ();
}

static void 
data_copy_hook(void*, Pnode &node, node_class,
	       tree_node_action &action, int& never_see_again) 
{
// error('d',"data_copy_hook: %k",node->base);

	never_see_again = 1;
  	switch (node->base) {
  	case NAME:
      		if(node == sta_name) 
			{ action = tna_stop; return; }
  	default:
    		action = tna_continue;
   	 	return;
  	}
}

Pname 
templ_inst::data_copy(Pdata dat, Pcons &templ_refs)
{
	pointer_hash fcorr(*corr); // initialize with old hash table
  
    	{     
    	tree_copy_info info;
    	Pnode root = dat->dat_mem; 

     	for (Plist fformal = dat->formals, cformal = inst_formals;
  	     fformal; fformal = fformal->l, cformal = cformal->l) {
		fcorr[int(fformal->f)] = int(cformal->f);
		if (fcorr[int(fformal->f)] != int(cformal->f))
			error ('i', "templ_inst::fuction_copy: hash table bug");
      	}

      	info.node_hook = ::data_copy_hook;
      	info.hook_info = this;

      	templ_refs = ref_copy(fcorr,info,templ_refs);
      	if (fcorr[int(def->namep)] != int(tname))
  	  	error ('i', "Y to instantiationTN correspondence is missing");
    
      	copy_tree(root,info,&fcorr);
      	return Pname(root);
    	}
}

void 
templ_compilation::end_of_compilation() 
/* 
 * Compile all template member body instantiations. 
 * Set in motion the compilation of the graph of 
 * instantiation bodies. Note that compilation of
 * a body may in turn initiate the instantiation of 
 * templates that had not previously been instantiated.  */
{
	bool change = false;
  	do {
    		change = false;
    		for (Ptempl p = list; p; p = p->next)
      			change = ( change | p->instantiate_bodies() 
				? true : false);
  	} while (change);
}

bit
name::dinst_body()
{


	if (	
		data_flag==0 && 
		all_flag==0 && 
		alltc_flag==0 &&
		curr_inst==dummyinst &&
		curr_inst != 0
	) 
		return 0;


	if (	
		all_flag==0 &&
		curr_inst!=dummyinst &&
		curr_inst!=0
	) 
		return 0;

	bit instflag=0;

	for (int i=0;i<nodatainst;i++) 
		if (strcmp(instdata[i],string)==0)
			instflag=1;

	if (	instflag && 
		curr_inst!=0) 
		return 0;

	return 1;
}

bit
name::finst_body()
{
	if (	
		all_flag==0 && 
		ft_flag==0 && 
		fcurr_inst!=0 &&
		fct_type()->f_inline==0 
	) 
		return 0;

	if (	
		ft_flag && 
		fdummyinst!=fcurr_inst &&
		fcurr_inst!=0 &&
		fct_type()->f_inline==0 
	) 
		return 0;

	return 1;
}

bit 
name::inst_body()
{
	bit inst_flag=0;

	for (int i=0;i<noinst;i++) 
		if (strcmp(string,instfct[i])==0) 
			inst_flag=1; //instantiate it


//error('d',"string is %s inst_flag is %d curr_inst %d dummyinst %d",string,inst_flag,curr_inst,dummyinst);

	bit datainstflag=0;

	for (int j=0;j<nodatainst;j++) 
		if (strcmp(instdata[j],string)==0)
			datainstflag=1;

	Pfct ft=fct_type();

	if (	
		inst_flag==0 && 
		curr_inst != 0 &&
		curr_inst==dummyinst &&
		all_flag==0 &&
		alltc_flag==0 &&
		ft->f_virtual==0 && 
		ft->f_inline==0
	)  
		return 0;

	if (	datainstflag &&
		curr_inst !=0 &&
		curr_inst==dummyinst &&
		ft->f_virtual &&
		all_flag==0 &&
		alltc_flag==0
	)
		return 0;


	if (	
		curr_inst==dummyinst &&
		curr_inst != 0 && 
		ft->f_virtual && 
		ft->f_inline==0 &&
		data_flag==0 &&
		all_flag==0 &&
		alltc_flag==0
	) 
		return 0;

//error('d',"this is %n f_inline is %d ft->f_virtual is %d",this,ft->f_inline,ft->f_virtual);
//error('d',"this %n curr_inst %d dummyinst %d all_flag %d",this,curr_inst,dummyinst,all_flag);

	if (	
		all_flag==0 &&
		curr_inst!=dummyinst &&
		curr_inst!=0 && 
		ft->f_inline==0 &&
		ft->f_is_inline==0
	)
		return 0;

//error('d',"about to return 1");

	return 1;
}

bool 
templ::instantiate_bodies()
/* Instantiate each member function body. 
 * It assumes that the class declaration has been instantiated. 
 * The return value indicates whether an instantiation of bodies 
 *     actually took place. 
 * This function is only invoked at the end of a file compilation, 
 *     after all source text has been processed. 
 * fns: list of member function declarations for template
 * insts: list of instantiations for template */
{
    bool change = false;
    if (!fns && !data) return change; 

    for (Ptempl_inst inst = insts; inst; inst = inst->next)
// error('d',"instantiate_bodies: %n inst->status: %d", inst->get_tname(), inst->status );
   	if (!inst->forward && (inst->status==class_instantiated)) 
	{
      	    // Set up the environment for the declaration, 
	    // and subsequent compilation of function bodies
      	    inst->status = body_instantiated; 
	    change = true;
	    Pclass ic = inst->get_class(); 

      	    {  
	    int i;
	    // note the overriding definitions explicitly provided
	    for (Pname fn=ic->memtbl->get_mem(i=1); fn; NEXT_NAME(ic->memtbl,fn,i))
	   	if ((fn->base == NAME) && 
		    (fn->tp->base == FCT) && (fn->fct_type()->body))
	  		    fn->n_redefined = 1; // note overriding definitions 
       	    }

		cc->stack(); cc->cot=0; cc->not=0; cc->tot=0; cc->c_this=0;
		for (Pfunt fnt=fns; fnt; fnt=fnt->next) {
			Pcons templ_ref_copy = fnt->templ_refs;
			Pname fn = inst->function_copy(fnt,templ_ref_copy);
	
			// Change qualifier to name of the instantiated,
			// rather than parameterized class name
			fn->n_qualifier = inst->namep;

			// cond contains type information
			// shares space with n_table
			if (fn->n_oper != TYPE ) fn->n_table = 0; 
			fn->n_tbl_list = 0;
	
			// Note that the formals were bound to actuals
			// when the class decl was instantiated, 
			// so the binding is not redone.
	
			// Modify constructor and destructor names.
			if (!strcmp(fn->string, namep->string))
	  			fn->string = inst->namep->string;	
	
			{ 
			inst->save_state(fn);
	  			
			// Instantiate the parametrized types 
			// referenced by this template
	  		for (Pcons pc=templ_ref_copy; pc; pc=pc->cdr) 
	     			Ptempl_inst(pc->car)->instantiate();

			curr_inst=inst;

  			if (((fn=fn->dcl(gtbl,EXTERN))==0) || error_count) {
    				inst->restore_state();
   			 	continue; }

			curr_inst=inst;

			if (se_opt && fn->inst_body()==0) 
				suppress_error++;

  			fn->simpl();

			if (se_opt && fn->inst_body()==0) {
				suppress_error--;
			}

			if (dtpt_opt && fn->inst_body()==0)
				fn->fct_type()->body=0;

  			fn->dcl_print(0);
			curr_inst=0;
  			inst->restore_state();
			}
      		}
		cc->unstack();

      	    	inst->status = data_instantiated; 
		cc->stack(); cc->cot=0; cc->not=0; cc->tot=0; cc->c_this=0;
        	for (Pdata dat=data; dat; dat=dat->next) 
		{
			Pcons templ_ref_copy = dat->templ_refs;
			Pname dn = ic->memtbl->look(dat->dat_mem->string,0);
			/* do other checks here for explicit definitions */
			if (!dn || dn->n_stclass != STATIC)
				error("%n: only static dataMs can beZized",dat->dat_mem);
			if (dn->n_redefined) continue; // explicitly defined
			Pname n = inst->data_copy(dat,templ_ref_copy);
			n->n_qualifier = inst->namep;
			n->n_table = 0; n->n_tbl_list = 0; 
	
			inst->save_state(n);
  			for (Pcons pc=templ_ref_copy; pc; pc=pc->cdr)
     				Ptempl_inst(pc->car)->instantiate();

			curr_inst=inst;

  			if (((n=n->dcl(gtbl,EXTERN))==0) || 
			      error_count) { 
					inst->restore_state(); 
					continue; }
  			n->simpl();

			mk_zero_init_flag=0;

			if (n->n_stclass==STATIC && n->n_initializer==0 && n->n_evaluated==0)
				n->n_initializer=mk_zero_init(n->tp,n,n);

			if (	
				dtpt_opt && 
				n->dinst_body()==0
			) {
				n->n_initializer=0;
				mk_zero_init_flag=1;
			}

			if (n->n_stclass != STATIC || !mk_zero_init_flag || !strict_opt)
	  			n->dcl_print(0);
			curr_inst=0;
			mk_zero_init_flag=0;
			n->n_dcl_printed = 2; // suppress further printing
			inst->restore_state();
		}
		cc->unstack();
    	}
  	return change;
}

Pname 
templ_compilation::check_tname(Pname p) 
/* A predicate to validate that a tname without template parameters is legit
 * in the scope; i.e., that it does not need actual template arguments.
 * Currently, a tname without parameters is ok within the class definition,
 * but parameters are required within the member definition. They should not
 * be required within the member function either to be consonance with their
 * use in the class definition.  */
{
// error('d', "templ_compilation::check_tname(%n) t_arg: %d",p,p->n_template_arg);
	Ptempl t = is_template(p);
  	if (p->n_template_arg) p->n_used++;
  	if (!t) return p;
	extern int in_friend;
  	if (in_progress && in_friend == 0 &&
		((owner && (owner->namep == p)) ||
		(!owner && same_class(t->classtype(),ccl))))
     			return p;

	// explicit instance
	extern Ptempl_inst pti;
	if (pti && pti->def == t) 
		return p;

  	error ("%n needsY instantiationAs",p);
  	return p;
}

bool 
templ_compilation::current_template(Pname p, Pexpr actuals) 
/* 
 * Determine if parameters specified to a template are
 * redundant, and really refer to the current template 
 * class. Thus,
 *
 *      template c<class p1, class p2> c<p1,p2>::foo 
 *			{ ... };
 *
 * has the redundant template specification c<p1, p2> 
 * and can simply be a reference to a "c" instead 
 */
{
// error('d',"templ_compilation::current_template(%n) in_progress: %d",p,in_progress);

	Ptype t;
	if (Pbase(p->tp) && Pbase(p->tp)->b_name)
		t = Pbase(p->tp)->b_name->tp;
	else
		t = 0;
	if (in_progress &&
      		((owner && (owner->namep == p)) ||
       		(!owner && ((p->tp->base == COBJ) && (t == ccl)))))
    	{ // Check whether the formal and actual types are identical
      		Pexpr actual = actuals;
      		for (Plist formal = params; formal && actual; 
			formal = formal->l, actual = actual->e2)
		{
			if (formal->f->n_template_arg == template_type_formal)
			{
				if (
					formal->f->tp==::any_type
					&&
					actual->e1->tp
					&&
					actual->e1->tp->base==TYPE
					&&
					Pbase(actual->e1->tp)->b_name->tp==::any_type
					&&
					strcmp(Pbase(actual->e1->tp)->b_name->string,formal->f->string)==0
				)
					continue;
				else
					break;
			}
			else //(formal->f->n_template_arg == template_expr_formal)
			{
      				if (
					formal->f->base==NAME
					&&
					actual->e1->base==NAME
					&&
	     				strcmp(Pname(actual->e1)->string,formal->f->string)==0
				)
					continue;
				else
					break;
			}
		}
      		if (!formal && !actual) return true;
    	}
  	return false;
}
  
Pfunt 
templ::collect_function_member(Pname fname) 
{ // Add a new member function to list of template class functions
  	PERM(fname); PERM(fname->tp); 
	PERM(Pfct(fname->tp)->body);
  	return new function_template(*this,templp->params,fname);
}

Pdata 
templ::collect_data_member(Pname dname) 
{ // Add a new static data member to list of template class 
  	PERM(dname); PERM(dname->tp); 
  	return new data_template(*this,templp->params,dname);
}

void 
check_formals_for_dups(Plist formals) 
{ // template <class T, class T> class X;
	for (Plist fl1 = formals; fl1; fl1 = fl1->l) {
		Pname n1 = fl1->f;
		for (Plist fl2 = fl1->l; fl2; fl2 = fl2->l) {
			Pname n2 = fl2->f;
			if (strcmp(n1->string,n2->string)==0)
				error("YA%n redeclared inZ list",n1);
		}
	}
}

bit
contains_formal(Pname formal, Pclass cl) 
{
// error('d',"contains_formal formal: %n cl %t cl->class_base: %d", formal,cl,cl->class_base);
	if ( cl->class_base == VANILLA ) return 0;
	if ( cl->is_templ_instance() ) {
		Pexpr actuals = Ptclass(cl)->inst->actuals;
		for ( ; actuals; actuals = actuals->e2 ) {
			Pexpr e = actuals->e1;
			if (e->base != NAME || e->tp->base != TYPE) continue;
			Pname tn = e->tp->bname();
			if (strcmp(formal->string,tn->string) == 0)
				return 1;
		}
	} 
	else {
	// Queue<Type> in formal, Queue<T> in formal signature
		Ptempl t = templp->is_template(cl->string);
		if (!t) return 0;
		for ( Plist formals = t->get_formals(); formals; formals=formals->l ) {
			Pname n = formals->f;
    			if ( n->n_template_arg == template_type_formal ) {
				return 1;
      			}
		}
	}

	return 0;
}
		
void 
check_funct_formals(Plist formals, Pname namep) 
{ 
// error('d',"check_funct_formals: %n", namep);
	check_formals_for_dups(formals);
	for (Plist fl = formals; fl; fl = fl->l) 
	{
		Pname fn = fl->f;
		if (!fn->is_template_arg()) {
			error("FYZ%n must be aT",fn);
			continue;
		}

		for (Pname a = Pfct(namep->tp)->argtype; a; a = a->n_list )
		{
			Ptype t = a->tp;
			int found = 0;
			while (t->base == PTR || t->base == RPTR 
				|| t->base == VEC)
			{ 
				switch (t->base) {
					case PTR: 
					    if (Pptr(t)->ptname) { // T::*
						Pname n = Pptr(t)->ptname;
						if (strcmp(n->string,fn->string)==0) found = 1;
						t = n->tp; 
						break;
					     }
					case RPTR: t = Pptr(t)->typ; break;
					case VEC: t = Pvec(t)->typ; break;
				}
			}

			if (found) { found = 0; break; }

			if (t->base == COBJ ) { // min(X<T>
				if (contains_formal(fn,t->classtype())) {
					break;
				}
			} 
			else if (t->base != TYPE) continue;

			Pname tn = t->bname();			
			if (!tn->is_template_arg()) {
				t = tn->tp;
				if (t->base == COBJ ) { // min(X<T>
					if (contains_formal(fn,t->classtype())) {
						break;
					}
				}
				continue;
			}
			if (strcmp(tn->string,fn->string)==0)
				break;
		}
		if ( a == 0 )
			error("FYZ%n must be used in signature of %n",fn,namep);
	}
}
		
#if 0
templ_compilation::get_formals_count()
{
	int cnt = 0;
  	for ( Plist p = params; p; p=p->l, ++cnt);
	return cnt;
}
#endif

int basic_template::get_formals_count() 
{
	int cnt = 0;
  	for ( Plist f = formals; f; f=f->l, ++cnt);
	return cnt;
}

void 
basic_template::check_formals(Plist f2) 
/* Check the formals specified for a member function 
 * or a forward definition of a class 
 * against the formals for the class  */
{
  	for (Plist f1 = formals; f1 && f2; f1=f1->l, f2=f2->l)
    		if (f1->f->base != f2->f->base) 
      			switch (f1->f->n_template_arg) {
      			    case template_type_formal:
				error("Y formalZ mismatch, %n must be aT formalZ",f2->f);
				break;

      			    case template_expr_formal:
				error("formalZ mismatch, %n must be aZ ofT %t",f2->f,f2->f->tp);
				break;

      			    default:
				error("formalZ mismatch betweenC formal: %n andM formal: %n",f1->f,f2->f);
      			}
		else 
		if (f1->f->n_template_arg == template_expr_formal) 
		{
			// the types should be identical
			if (f1->f->tp->check(f2->f->tp, 0) == 1)
	  		    error("T mismatch betweenM formal %n andC formal %n",f2->f,f1->f);
      		}

  		if (f1)
    		    error("insufficient formalZs,CZ%n onwards are missing",f1->f);

  		if (f2)
    		    error ("excess formalZs,Z%n onwards not defined forC",f2->f);
  
}

static Ptype 
real_type (Ptype t)
{ // predicate to get past all the type nodes
  	while (t && t->base == TYPE) 
		t = Pbase(t)->b_name->tp;
  	return t;
}

static int expr_match(Pexpr a1, Pexpr a2);

static void
vec_eval(Ptype p)
{
	while (p && (p->base == VEC || p->base == PTR)) {
		if (p->base == VEC) {
			Pvec v = Pvec(p);
			if (v->dim && !v->size) {
				Neval = 0;
				v->size = (int)v->dim->eval();
				if (Neval)
					error("cannot evaluate constantE");
				DEL(v->dim);
				v->dim = 0;
			}
			p = v->typ;
		}
		else {
			p = Pptr(p)->typ;
		}
	}
}

bool 
templ::check_actual_args(Pexpr actual)
{ // check actual template arguments against formals
	for (Plist formal=formals; formal && actual; 
		formal=formal->l, actual=actual->e2)
	{
// error('d',"check_actual_args: formal %n %t", formal->f, formal->f->tp);

    		switch (formal->f->n_template_arg) {
    		case template_type_formal:
      		{
		/* A "type type" parameter, any actual type that
		 * was accepted by the parse is acceptable here, 
		 * just make sure that it is indeed a type.
		 * If it was parsed as a type, the grammar 
		 * should have created a name node, and 
		 * attached the type to it, having marked the 
		 * name as a template_actual_arg_dummy.  */

			vec_eval(real_type(actual->e1->tp));
			Pname n = Pname(actual->e1);
			if (!((n->base == NAME) &&
	    	     		(n->n_template_arg==template_actual_arg_dummy))) 
			{
	  			error ("Y %sA mismatch, theY formal:%n required aT actualZ", namep->string, formal->f);
	  			// recover from error with a safe expression
	  			n = new name("");
	  			n->tp = ::any_type;
	  			actual->e1 = n;
			}

			Ptype tp = n->tp->skiptypedefs();
			if (tp->base == COBJ) {
    				Pclass cl = tp->classtype();
				if (cl->local_sig)
					error('s', "localC%t as ZT to YC%n",cl,namep);
			}

			break;
      		} // case template_type_formal

    		case template_expr_formal:
      		// in general, checking can only be done at instantiation, 
 		// however, check 0 as actual for pointer
			{
			Ptype tp = formal->f->tp->skiptypedefs();;
			if (actual->e1->base==ZERO && tp->base==PTR) 
			{ 
				// ``2'' signifies user didn't set const
				if (tp->b_const == 2) 
					tp->b_const = 0;
				error("cannot instantiate 0 to formalZ%n(%t)",formal->f,tp); 
			}
      			break;
			}
    		default:
      			error ('i',"badY formal") ;
    		}
	} // for loop

  	// ??????????????? If we provide for optionals, 
 	// this is where the processing should get done.
  	if (formal)
    		error ("too fewAs supplied forY %s",namep->string);

  	if (actual && actual->e1) 
    		error ("too manyAs supplied forY %s", namep->string);

// error('d',"check_actual_args: formal: %d actual: %d", formal, actual);
  	return bool(~(formal || actual));
}

void 
templ_compilation::append_friend_ref(Ptempl_inst ref) 
{ // Append to the list of references.
// error('d',"append_friend_ref: %n",ref->get_tname());

	cons *p = new cons(ref,0);
  	if (last_friend_cons)
    		last_friend_cons->cdr = p;
  	else friend_templ_refs =  p;
  	last_friend_cons = p;
}

void 
templ_compilation::append_ref(Ptempl_inst ref) 
{ // Append to the list of references.
// error('d',"append_ref: %n",ref->get_tname());

	cons *p = new cons(ref,0);
  	if (last_cons)
    		last_cons->cdr = p;
  	else templ_refs =  p;
  	last_cons = p;
// display_templ_refs();
}

// pretty stupid to have both, but ...
static bit ref_in_friend = 0;
static bit ignore_ref = 0;

Ptempl_inst 
templ_inst::note_ref() 
/* Note the reference by a definition to the template, 
 * so that the template can be instantiated before the 
 * definition is processed.  */
{  
// error('d',"templ_inst::note_ref: %n", tname);
	if ( ref_in_friend && ignore_ref == 0) {
		friend_refp = true;
		templp->append_friend_ref(this);
	}

	if ((! refp)) {
    		refp = true;
    		templp->append_ref(this);
  	}
  	return this;
}
  
bool
templ_compilation::friend_template(Pexpr actuals) 
/* 
 * Determine if parameters specified to a friend template 
 * are redundant -- refer to the current template formals 
 * if so, do not note reference: not a case for instantiation */
{
	// Check whether the formal and actual types are identical
      	Pexpr actual = actuals;
      	for (Plist formal = params; formal && actual; 
		formal = formal->l, actual = actual->e2)
	{
		if ((formal->f->tp == actual->e1->tp) || 
    			((actual->e1->tp && 
			 (actual->e1->tp->base == TYPE)) &&
     			 (Pbase(actual->e1->tp)->b_name->tp == formal->f->tp)))
	  				continue;
		else break;
	}

 	if (!formal && !actual) 
	    return true;
  	return false;
}
  
extern int dont_instantiate;

Ptempl_inst 
templ::get_inst(Pexpr actuals, Ptempl_inst exclude)
/* 
 * Get an instantiation for the template with the given set of actuals. 
 * If one exists, return it, otherwise create a new one. 
 */
{
// error('d',"templ::get_inst: %n", namep);

	static int level = 0;
	++level;	// record recursion depth

	Ptempl_inst inst = get_match(actuals, exclude, false);
	Ptempl_inst ti;
	int bl;
	if (inst)
	{
		if ( !inst->refp && !dont_instantiate )
		{
			if (templp->in_progress || level>1)
			{
				inst->note_ref();
			}
			else
			{
				bl = bl_level;
				bl_level = 0;
				ti = inst->instantiate();
				bl_level = bl;
				--level;
				return ti;
			}
		}
		--level;
		return inst;
	}

	inst = new templ_inst(actuals, this);
	if ( dont_instantiate )
	{
		--level;
		return inst;
	}
        ignore_ref = templp->friend_template(actuals) == 0;
	// if (ref_in_friend==0 || ignore_ref)
 	if (ref_in_friend != 2)
	{
		if (templp->in_progress || level>1)
		{
			inst->note_ref();
		}
		else
		{
			bl = bl_level;
			bl_level = 0;
			ti = inst->instantiate();
			bl_level = bl;
			--level;
			return ti;
		}
	}
	--level;
	return inst;
}

Ptempl_inst 
templ::get_match(Pexpr actuals, Ptempl_inst exclude,  
	         bool match_instantiated_only)     
/* Find an instantiation that has the same set of actuals, 
 * exclude: don't match this one passed in from the set of candidates
 * match_instantiated_only: only instantiated templates match */
{  
  	for (Ptempl_inst p = insts ; p ; p = p->next)
	{
    		if ((p != exclude) &&
		    (p->actuals_match(actuals)) &&
		    (match_instantiated_only ?
	 		    (Pclass(Pbase(p->tname->tp)->b_name->tp)->class_base == INSTANTIATED)
	 		    : true))
      	            return (p->forward ? p->forward : p);
	}
  	return 0;
}

int template_hier; // interim solution: permit Y<t> to match X<t>

Pfunct_inst
function_template::get_match(Pexpr actuals, Pfunct_inst exclude, 
				bool match_instantiate_only)
{
	for (Pfunct_inst p = insts; p; p = p->next)
	{
		if ( (p != exclude) &&
			(p->actuals_match(actuals)) &&
			(match_instantiate_only ? 
				Ptfct(p->tname->tp)->fct_base == INSTANTIATED
				: true))
			return p;
	}
	return 0;
}

static void
check_actuals( Pexpr actuals, Pname fn )
{
// error('d',"check_actuals: fn: %n",fn);

    for (Pexpr ae=actuals; ae; ae = ae->e2) {
	Ptype t = ae->e1->tp->skiptypedefs();
	if ( t->base == COBJ ) t = t->classtype();
	if ( t->base != CLASS ) continue;
	if ( strncmp("__C",Pclass(t)->string,3) == 0 ) 
		error("YF%n instantiatedW unnamedC", fn); 
	if ( t->lex_level && t->local_sig ) 
		error('s',"YF%n instantiatedW localC %s", fn,Pclass(t)->string); 
    }

}

Pfunct_inst
function_template::get_inst(Pexpr actuals, Pfunct_inst exclude)
{
// error('d',"%n->get_inst()",fn);
	Pfunct_inst inst = get_match(actuals,exclude,false);
	if (inst == 0) {
		check_actuals(actuals,fn);
		inst = new funct_inst(actuals,this);
	}
	return inst;
}

#if 0
Pbase 
templ::inst_basetype(Pexpr actuals)
{ // provides the basetype created for a given set of actuals.
	return (check_actual_args(actuals) 
		? Pbase(get_inst(actuals)->tname->tp) 
		: basep);
}
#endif

Pname 
parametrized_typename(Pname p, Pexpr actuals, bit in_friend) 
/* Validate that the name denotes a parametrized type, 
 * and produce a TNAME for the instantiation.  */
{ 
// error('d',"parametrized_typename(%n (%d),%k) in_friend:%d",p,p,actuals->base,in_friend);
	Ptempl t = templp->is_template(p);

	// A template instantiation with redundant actuals
	// identical to the formals of the current template

  	if (templp->current_template(p, actuals) && in_friend == 0) 
		return p;

  	if (t) {
		ref_in_friend = in_friend;
    		Pname tname = t->typename(actuals);
		ref_in_friend = 0;
    		return (tname ? tname : p);
  	}
  	error ("%n hasTAs but is not aZizedC", p);
  	return p;
}

Pname 
templ::typename(Pexpr actuals) 
{ // obtain typename associated with an instantiation
  	return (check_actual_args(actuals) 
		? get_inst(actuals)->tname : 0);
}


// Check whether the actuals provided match the actuals for this instantiation.
// The actuals match the formals, iff they are same type or parametrized type. 
bool 
templ_inst::actuals_match(Pexpr check_actuals)
{ 
  Pexpr ae1, ae2 ;     // the cons cells
  Plist formal = def->formals ;
  
  // The lists should be the same length, since check_actuals will have taken
  // appropriate action.
  for (ae1=actuals, ae2=check_actuals ; formal && ae1 && ae2 ;
       ae1=ae1->e2, ae2=ae2->e2, formal = formal->l)

    switch (formal->f->n_template_arg) {
    
    case template_type_formal:
      { 
	Ptype t1 = ae1->e1->tp, t2 = ae2->e1->tp ;
	if (t1->check(t2,255) == 1)
	  return false ;
	break ;
      }
    case template_expr_formal:
      if (! expr_match(ae1->e1, ae2->e1)) return false ;
      break ;
      
    default:
      error ('i', "bad template formal") ;
    }
  return true ;
}

Ptype non_template_arg_type(Pbase);

bool
funct_inst::actuals_match(Pexpr check_actuals)
/*
  Try to match the actual arguments in a function call expression with
  the formal arguments of the template function pointed to by `this'.
*/
{
// error('d',"%n->actuals_match()",tname);

	Pname n = Ptfct(tname->tp)->argtype;	// formal arguments
	Pexpr e = check_actuals;		// actual arguments

	for ( ; n && e; n=n->n_list, e=e->e2)
	{
		Ptype t1 = n->tp;
		Ptype t2 = e->e1->tp;

// error('d',"actuals_match: t1: %t t2: %t", t1,t2);

		while ( t2->base == TYPE ) {
			Pname nnn = t2->bname();
			// instantiation inside instantiated function
			if (nnn->is_template_arg()) 
				t2 = t2->bname_type();
			else
				t2 = nnn->tp;
		}

		// first try to match with trivial conversions only
		int t_h = template_hier;
		template_hier = 0;      // turn off derived-->base conversions
		if (exact1(n, t2))
		{
			template_hier = t_h;
			continue;
		}
		template_hier = t_h;


		// for class C, allow for C -> C&
		if (t2->base==COBJ && t1->base==RPTR) {
			Ptype t = Pptr(t1)->typ->skiptypedefs();
			if (t->base==COBJ)
				t2 = new ptr(RPTR,t2);
		// for T[n], convert T[n] -> T*
		} else if (t2->base==VEC) {
			t2 = new ptr(PTR,Pvec(t2)->typ);
			if (t1->base==RPTR)
				t2 = new ptr(RPTR,t2);
		}

		// `template_hier' was set to 1 in `has_templ_instance()'
		// to allow for Derived --> Base conversions
		if (t1->check(t2,PT_OVERLOAD)) {
			// provided that const-ness is not violated, we are
			// matching with an allowed Derived --> Base conversion
			if (!const_problem || Pptr(t2)->typ->b_const)
				return false;
			else
				continue;
		}
	}
	return true;
}

extern char emode;
/* stradd:  set of overloaded fuctions used to 
 * accumulate name strings */


void 
stradd(char *&target, char *source, int numeric = 0) {
// error('d',"stradd: target: %s\t source: %s\tnumeric: %d",target,source,numeric);
  	while (*target = *source) {
		if ( *target == '-' && numeric ) 
			*target = 'n';
		target++; source++;
  	}
}
	
void 
stradd(char *&p, long i) {
  char s[64];
  char t[64];
  
  	if (!emode) { *p++ = 'L'; }
  	sprintf(s,"%ld", i);
	if (!emode) {
		int len = strlen(s);
		if (len >= 10)
  	    		sprintf(t,"%ld_%s", len,s);
        	else sprintf(t,"%ld%s", len,s);
  		stradd(p,t,1);
	}
  	else stradd(p,s,0);
}

void 
stradd(char *&p, Pname n) {
  	if (!emode){
    		char s[1024];
    
    		sprintf(s,"%d", strlen(n->string));
    		stradd(p,s);
  	}
  	stradd(p, n->string);
}

char* 
mangled_expr(char *p, Pexpr e, bool mangle_for_address = false)
/* produce a unique string suitable for use within a name; 
 * if in emode, i.e., printing in the context of an error, 
 * print a pretty name instead. */
{
  	static int mangle_address = 0;
  	if (e == 0) return p;

// error('d',"mangled_expr: p %s e: %k", p, e?e->base:0);
  
	switch (e->base) {
  	case ADDROF:
  	case G_ADDROF:
    		// relies on unary using e2 
    		++mangle_address;
		if (emode)
		{
			stradd(p, "&");
		}
    		p = mangled_expr(p,e->e2);
    		--mangle_address;
    		break;
    
  	case NAME:
    		if (mangle_address || mangle_for_address)
		{
			Pname n = Pname(e);
			if (n->n_scope == PUBLIC || n->n_scope == 0)
 			{  // static data or function members of some class
				Ptype t = n->tp;
				if ((n->n_stclass == STATIC && n->n_table->t_name) ||
				    (t->base == FCT && Pfct(t)->f_static))
				{ // &X::i ==> 5i__1X
					char xx[1024];
					char s[1024];
					char *st = t->base!=FCT
						? n->n_table->t_name->string 
						: Pfct(t)->memof->string;
					if (emode)
					{
						sprintf(s,"%s::%s",st,n->string);
					}
					else
					{
						sprintf(xx,"%s__%d%s",n->string,strlen(st),st);
						sprintf(s,"%d",strlen(xx));
						strcat(s,xx);
					}
//error('d',"mangled_expr: %s", s );
					stradd(p,s);
				}
				else stradd(p,Pname(e));
			}
			else stradd(p,Pname(e));
		}
    		else 
		if (Pname(e)->n_evaluated)
      			stradd(p,Pname(e)->n_val);
    		else 
		if (Pname(e)->n_initializer) {
			// see ``suitable_const_expr''
			Pexpr ee = Pname(e)->n_initializer;
			if (ee->base == NAME &&
			    Pname(ee)->n_evaluated) // const
				p = mangled_expr(p,ee);
			else p = mangled_expr(p,ee,true);
		}
		else 
		if (Pname(e)->tp->base == VEC || Pname(e)->tp->base == FCT)
			stradd(p,Pname(e));
    		else error('i',"mangled_expr: couldn't mangle actualYE %n",e);
    		break ;
      
  	case IVAL:
    		stradd(p, ((ival *)e)->i1);
    		break;

  	case CAST:
  	case G_CAST:
    	{ 
		// an IVAL || ICON hiding below the cast?
		// see ``suitable_const_expr''
      	  	if (e->e1->base == IVAL) {
		    stradd(p, ((ival *)e->e1)->i1);
		    break;
	  	}
	  	else 
      	  	if (e->e1->base == ZERO) {
    	    	    *p++ = '0';
		    break;
	  	}
	  	else 
      	  	if (e->e1->base != ICON && e->e1->base != CCON)
		{
			if (emode)
			{
				Neval=0;
				long e_eval = e->eval();
				if (!Neval)
				{
      					stradd(p,e_eval);
					break;
				}
			}
			error ('i', "mangled_expr: unexpected cast in YE");
			break;
	  	}
	  	else e = e->e1; // no break! -- compute ICON
    	}

  	// case FCON:  /* already filtered out */
  	case ICON:
  	case CCON:
	{
		if (!emode)
		{
			*p++ = 'L';
		}
		int len = strlen(e->string);
		char s[64];
		if (!emode)
		{
			if (len >= 10)
				sprintf(s,"%ld_%s", len,e->string);
			else
				sprintf(s,"%ld%s", len,e->string);
		}
		else
		{
			sprintf(s,"%s",e->string);
		}
		strcpy(p,s);
    	    	// ??? must there be a better encoding scheme ???
    	    	if (!emode)
      	    	    while (*p)
      		    	if (! (isalnum(*p)))
			    switch(*p) {
				case '+': 
	  			    *p++ = 'p';
	  			    break;
				case '-':
	  			    *p++ = 'n';
	  			    break;
				case '.':
	  			    *p++ = 'D';
	  			    break;
				case 'e':
	 			     *p++ = 'E';
	  			    break;
				default:
	  			    error ('i', "bad character in constant");
	  			    break;
			    } 
			else p++;
    		break ;
	}
  	case ZERO:
    	    	*p++ = '0'; 
		break; 
	case ILIST: // pointer to member constant
	{
	// literal encoding of pointer to member :: LM
		if (emode)
		{
			stradd(p, "&class::member");
			break;
		}
		Pexpr i1 = e->e1->e1; // delta
		Pexpr i2 = e->e1->e2; // index 
		Pexpr i3 = (e->e2->base==CAST||
			e->e2->base==G_CAST)?e->e2->e1:e->e2; // function address or 0
		*p++ = 'L'; *p++ = 'M';
		p = mangled_expr(p,i1); *p++ = '_';
		p = mangled_expr(p,i2); *p++ = '_';
		if (i3->base != NAME) //  
			p = mangled_expr(p,i3);   // virtual function
		else p = mangled_expr(p,i3,true); // address of name
	}
		break;
	case DOT: 
	case REF:
	    	if (mangle_address) {
		    while (e->base == DOT || e->base == REF)
		           e = Pexpr(e->mem);	
    	    	     p = mangled_expr(p,e->e2);
	    	     break;
	        }
		// no break
  	default:
		if (emode)
		{
			Neval=0;
			long e_eval = e->eval();
			if (!Neval)
			{
      				stradd(p,e_eval);
				break;
			}
		}
		error ('i', "can't mangle %k", e->base);
      	}

// error('d',"mangled_expr: %s", p);

  	return p;
}

#if 0
char*
mangled_expr(char *p, Pname n) 
{ // this function is invoked once at the top level

  	if (n->n_evaluated) {
   		stradd(p, n->n_val);
// error('d',"mangled_expr using stradd: %s", p);
    		return p;
	}
	return mangled_expr(p, n->n_initializer,
		(real_type(n->tp)->base == PTR ? true : false));
}
#endif

void
templ_inst::print_pretty_name()
{
/* Generate a template class instantiation name suitable for printing 
 * when it is presented to the user.
 * this depends on using a %t and passing it a classdef type --
 * passing it the %n, Pname pair will have it print out with encoding */

  	extern char emode;
  
  	/* 3.1/4.0: until next release when defs are merged */
	Pname n = Ptempl_inst(this)->def->namep;
  	error('c', " %s<", (n ? n->string : "?"));

  	Plist formal = inst_formals;
  	emode++;  
  
  	for ( Pexpr ae1=actuals; formal && ae1; 
              ae1=ae1->e2, formal = formal->l ) 
        {
    		if (ae1->e1->base == 0) break; // we are in an error mode in any case 
    		switch (formal->f->n_template_arg) {
    			case template_type_formal:
      				ae1->e1->tp->dcl_print(0);
      				break;
    			case template_expr_formal: { 
       				char buff[ 1024 ]; 
       				memset(buff,0,1024);
       				mangled_expr(buff, ae1->e1);
       				error('c',"%s", buff);
       				break;
      			}
    			default: error('i',"unexpected formalZ");
    		}

        	// this comma is unfortunately misplaced, 
        	// since it follows a space printed out by dcl_print
    		if (formal->l) error('c',", ");
  	}
  	error('c', ">");
  	emode --;
}

void
funct_inst::print_pretty_name()
/*
	Generate a template function instantiation name suitable
	for printing when it is presented to the user.
*/
{
	extern char emode;
	emode++;

	Pname n = Pfunct_inst(this)->def->fn;
	error('c', " %s(", n ? n->string : "?");

	n = Ptfct(tname->tp)->argtype;		// formal arguments
	for ( ; n; n=n->n_list)
	{
		Ptype t = n->tp;
		if (!t || t->base == 0)
			continue;		// in error mode in any case
		t->dcl_print(0);
		if (n->n_list)
			error('c', ", ");
	}

	error('c', ")");
	emode --;
}

static int
check_expr(TOK b)
{
	switch (b) {
		default:
			return 0;
		case ICON: case IVAL: case ZERO:
		case CCON: case CAST: case G_CAST:
		case EOBJ: case SIZEOF: case COMPL:
		case UMINUS: case UPLUS: case NOT:
		case PLUS: case MINUS: case MUL:
		case LS: case RS: case NE:
		case LT: case GT: case GE:
		case LE: case AND: case OR:
		case ER: case DIV: case MOD:
		case EQ: case ANDAND: case OROR:
		case CM: case G_CM: case QUEST:
			return 1;
	}
}

int 
check_for_const(Pexpr a1, Pexpr a2) 
{ // there are different representations for ICON based upon 
  // whether it has been evaluated. 
// error('d',"check_for_const a1: %k a2: %k",a1->base,a2->base);
// error('d',"check_for_const a1(tp): %t a2(tp): %t",a1->tp,a2->tp);

  	Neval = 0 ;
	if (a1->base != NAME && a2->base != NAME) {
		if (a1->tp && a2->tp && a1->tp->check(a2->tp, 255)) return false;
		while (a1->base == CAST || a1->base == G_CAST) {
			TOK b1 = a1->tp2->skiptypedefs()->base;
			if (b1 == CHAR || b1 == SHORT || b1 == INT ||
			    b1 == LONG || b1 == ENUM || b1 == EOBJ)
				break;
			a1 = a1->e1;
		}
		while (a2->base == CAST || a2->base == G_CAST) {
			TOK b2 = a2->tp2->skiptypedefs()->base;
			if (b2 == CHAR || b2 == SHORT || b2 == INT ||
			    b2 == LONG || b2 == ENUM || b2 == EOBJ)
				break;
			a2 = a2->e1;
		}
		long a1_eval = a1->eval();
		long a2_eval = a2->eval();
		if (Neval) return false;
		return a1_eval==a2_eval;
	}

  	if (a1->base == NAME && check_expr(a2->base)) {
		Pname n = Pname(a1);
		long a2_eval = a2->eval();
		return (!Neval && n->n_evaluated && (n->n_val == a2_eval));
    	}
        else
  	if (a2->base == NAME && check_expr(a1->base)) {
		Pname n = Pname(a2);
		long a1_eval = a1->eval();
		return (!Neval && n->n_evaluated && (n->n_val == a1_eval));
	}

	if (a1->base == NAME && (a2->base == ADDROF || a2->base == G_ADDROF) &&
	    a1 == a2->e2)
		return true;

	if (a2->base == NAME && (a1->base == ADDROF || a1->base == G_ADDROF) &&
	    a2 == a1->e2)
		return true;

	if (a1->base == NAME && a2->base == ILIST && a1->tp && a2->tp)
		return !a1->tp->check(a2->tp, 255) ? true : false;

	if (a2->base == NAME && a1->base == ILIST && a1->tp && a2->tp)
		return !a2->tp->check(a1->tp, 255) ? true : false;

  	return false;
}
  
// get past the template parameter names to get the the real expression
static Pexpr 
real_expression(Pexpr e) 
{
  while (e && (e->base == NAME) &&
	 (Pname(e)->n_template_arg == template_expr_formal) &&
	 (! Pname(e)->n_evaluated) && Pname(e)->n_initializer)
    e = Pname(e)->n_initializer ;
  return e ;
}

// determine whether two expressions supplied as actual arguments to
// a "template_expr_formal"  formal argument match. 
static int 
expr_match(Pexpr a1, Pexpr a2)
{
  static int addr_of = 0 ;

  a1 = (addr_of ? a1 : real_expression(a1)) ;
  a2 = (addr_of ? a2 : real_expression(a2)) ;
  
  if (a1 == a2) return true;

// error('d',"expr_match: a1 %k a2 %k",a1->base,a2->base);

  //
  if (a1->base != a2->base) return check_for_const(a1, a2) ;
  Neval = 0;
  switch (a1->base) {
  case QUEST:
    // a ternary operator
    return ( expr_match(a1->cond, a2->cond) &&
	     expr_match(a1->e1, a2->e1) &&
	     expr_match(a1->e2, a2->e2)) ;
  
  case PLUS: case MINUS: case MUL: case DIV: case MOD:
  case ER: case OR: case ANDAND: case OROR:
  case LS: case RS:
  case EQ: case NE: case LT: case LE: case GT: case GE:
    // binary operator
    return (expr_match(a1->e1, a2->e1) &&
	     expr_match(a1->e2, a2->e2)) ;
  case UMINUS: case UPLUS:
  case NOT: case COMPL:
    return (expr_match(a1->e2, a2->e2)) ;
  case ADDROF:
  case G_ADDROF:
    {
      // unary using e2
      addr_of ++ ;
      int result = (expr_match(a1->e2, a2->e2)) ;
      addr_of -- ;
      return result ;
    }
  case CAST:
  case G_CAST:
    {
    TOK b1 = a1->e1->base;
    TOK b2 = a2->e1->base;
    if ((b1 == ICON || b1 == IVAL || b1 == CCON || b1 == ZERO) &&
        (b2 == ICON || b2 == IVAL || b2 == CCON || b2 == ZERO))
	return check_for_const(a1, a2);
    else
	return (expr_match(a1->e1, a2->e1)) ;
    }
  case NAME:
    {
    Pname n1 = Pname(a1), n2 = Pname(a2) ;
    if (! addr_of) {
      if ((n1->n_evaluated) && n2->n_evaluated && (n1->n_val == n2->n_val))
	return true ;
      if (n1->n_initializer &&  (! n2->n_initializer))
	return check_for_const(n2, n1->n_initializer) ;
      if (n2->n_initializer && (! n1->n_initializer))
	return check_for_const(n1, n2->n_initializer) ;
      if (n1->n_initializer &&  n2->n_initializer)
	return expr_match(n1->n_initializer,  n2->n_initializer) ;
    }
    if (templp->formals_in_progress)
      return strcmp(n1->string,n2->string)==0;
    return false;
    }
  case DOT:
    return (expr_match(a1->e1, a2->e1)) ;
  case REF:
    return (expr_match(a1->e1, a2->e1)) ;
  case IVAL:
    return (ival *)a1->i1 == (ival *)a2->i1 ;
  case ICON:
  case CCON:
	{
		int i;
		i = a1->eval() == a2->eval();
		if (Neval)
			error("cannot evaluate constantE");
		return i;
	}
  case FCON:
  case STRING:
    return (strcmp (a1->string, a2->string) == 0) ;
  case ZERO:
    return true ;
  case SIZEOF:
    { long l1 = a1->eval(), l2 = a2->eval() ;
      if (Neval) return false;
      return (l1 == l2) ;
    }
  case ILIST:
    if (a1->tp && a2->tp && !a1->tp->check(a2->tp, 255))
	return true;
    else
	return false;
  }
  return false ;
}     

static char* 
non_type_argument_signature(Pexpr e, char *p) 
{ // template <int ix, double sal> ... 
  	p = e->tp->signature(p);
  	return mangled_expr(p,e);
}
  
/* check for excessive template class nesting levels */
static int
check_nest(char* s, int max)
{
	char* t;
	char* t2;
	int len;
	int n;
	int maxn;

	t = s;
	maxn = 0;
	while (*t && t < s + max) {
		if (t[0] == '_' && t[1] == '_' && t[2] == 'p' &&
		    t[3] == 't' && t[4] == '_' && t[5] == '_') {
			t2 = t + 6;
			len = 0;
			while (*t2 >= '0' && *t2 <= '9') {
				len = len * 10 + *t2 - '0';
				t2++;
			}
			n = 1 + check_nest(t2, len);
			if (n > maxn)
				maxn = n;
		}
		t++;
	}
	return maxn;
}
    
// Generate a mangled template instantiation name. The syntax of as template
// mangled class name is of the form:
//   original_name__<argument type signatures>__unique_id
// Each non-type argument is replaced by a unique id.
char*
templ_inst::mangled_name(char *ip)
{ 
// error('d',"templ_inst::mangled_name(%s)", ip);
	 
  char *start = ip ;
  ip = start ;
  strcpy(ip, (def->namep ? def->namep->string : "?")) ;
  ip= start + strlen(start) ;
  stradd(ip, "__pt__") ;

  {
    char  a [max_string_size], *p = a ;
    Plist formal = inst_formals ;
  
    for (Pexpr ae1=actuals ; ae1 ; ae1=ae1->e2, formal = formal->l)
      switch (formal->f->n_template_arg) {
      case template_expr_formal:
	*p++ = 'X' ;
	// the formal must have been bound
	p = non_type_argument_signature(formal->f, p) ;
	break ;
      
      case template_type_formal:  
	p = ae1->e1->tp->signature(p,1) ;
	break ;
      
      default:
	error ("bad template formal:%d", formal->f->base) ;
	break ;
      }
    
    *p = 0 ;
    sprintf(ip, "%d_", strlen(a)+1) ;
    ip = start + strlen(start) ;
    strcpy(ip,a) ;
  }
    
// error('d',"templ_inst::mangled_name(%s)", start);
  if (check_nest(start, 9999) > MAX_INST_DEPTH) {
	if (!error_count)
		error("infinite template instantiation sequence");
	else
		error('i', "infinite template instantiation sequence");
  }

  return start ;
}

char*
funct_inst::mangled_name(char *ip) 
{ /* generate mangled template instantiation name of the form
   * <function_name>__pt__F<size>_[<arg>_]... */

  	char *start = ip;
  	ip = start;
  	strcpy(ip, (def->fn ? def->fn->string : "?"));
  	ip = start + strlen(start);
  	stradd(ip, "__pt__");

    	char buf[max_string_size], *p = buf;

	Ptype t;
	if (Ptfct(tname->tp) && Ptfct(tname->tp)->argtype) {
		Pname n = Ptfct(tname->tp)->argtype;
		for ( ; n; n=n->n_list)
		{
			t = n->tp;
			if (t->base == ZTYPE) t = int_type;
			p = t->signature(p);
			*p++ = '_';
		}
	} else {
		for (Pexpr ae = actuals; ae; ae=ae->e2)
		{
			t = ae->e1->tp;
			if (t->base == ZTYPE) t = int_type;
			p = t->signature(p);
			*p++ = '_';
		}
	}

	if (actuals) --p;
	*p = 0;
    	sprintf(ip, "F%d_", strlen(buf)+1);
    	ip = start + strlen(start);
    	strcpy(ip,buf);

// error('d',"funct_inst::mangled_name: %n (%s)",def->fn,start);    
  	return start;
}

const char leader[]= "\t";

void 
basic_inst::print_error_loc(int newline) 
{
/* Explain the location of an instantiation in greater detail, 
 * since it may be far removed from it's textual definition. 
 */

  	if (! basic_inst::head) return; // No active instantiations

  	extern void print_loc();
  	state current_state;
  	char buffer[max_string_size];

  	for (int i = 0; i<max_string_size; i++) buffer[i] = 0;

	if (newline) putch('\n');
  	current_state.save();
  	print_loc();
  	error('c',"%serror detected during the instantiation of", leader);
  	print_pretty_name();
	putch('\n');

  	if (!basic_inst::head->next_active) {
    		// A more compact message for a single level of instantiation
    		context.restore();
    		print_loc();
    		error('c', "%sis the site of the instantiation\n", leader); 
  	}
	else {
    		// The instantiation chain is longer than one
		print_loc();
    		error('c',"%sthe instantiation path was:\n", leader);
    		for (Pbase_inst p = basic_inst::head ; p ; p = p->next_active) {
      			p->context.restore();
      			print_loc();
      			error('c',"%s template:", leader);
      			p->print_pretty_name();
			putch('\n');
    		}
  	}
  	current_state.restore();
}

char*
basic_inst::instantiation_string() 
{ /* generates name for class or function template */
  	char inst_name[max_string_size];
  	for (int i=0; i<max_string_size; ++i) inst_name[i]=0;
  	mangled_name(inst_name); // virtual call
  	return strdup(inst_name);
} 
    
void 
classdef::modify_inst_names(char *s)
{ /* Change the names for the class, constructors, 
   * and destructors to reflect the new class instantiation name.  */ 
// error('d',"%t->modify_inst_names(%s)",this,s);

  	char *old = string;
  	string = s;   // Change the class name
	if ( lex_level && (in_class==0 || lex_level!=in_class->lex_level) ) {
		error('i',"localCY");
		//Pktab tb = k_tbl->k_next;
		//while ( tb && tb->k_id!=BLOCK ) tb = tb->k_next;
		//if ( tb == 0 ) error('i',"missing block scope for localCY%t",this);
		//local_sig = make_local_name(this,tb->k_name);
	} else
  		c_strlen = ::strlen(s);

  	// Change the constructor names
   	for (Pname p=mem_list; p; p=p->n_list)
    		if (p->tp && (p->tp->base==FCT) && 
		   (!strcmp(old, p->string)))
      			p->string = s;
}

Ptype 
non_template_arg_type(Pbase t) 
{ // Get past the fake template argument name typename types
// error('d',"non_template_arg_type: %t", t );

  	if ((t->base == TYPE) &&
      	    (t->b_name->n_template_arg == template_type_formal))
    		return non_template_arg_type(Pbase(t->b_name->tp));
  	else return t;
}

// follow the chain until we hit a non
void 
non_template_arg_non_type(Pname n) {
  Pexpr i = n->n_initializer; 
  while (i &&
	 (i->base == NAME) &&
	 (Pname(i)->n_template_arg == template_expr_formal)) 
    {
      if (Pname(i)->n_initializer) {
	n->n_initializer = Pname(i)->n_initializer ;
	i = n->n_initializer ;
	continue ;
      }
      if (Pname(i)->n_evaluated) {
	n->n_evaluated = 1 ;
	n->n_val = Pname(i)->n_val ;
	return ;
      }
    }
  return ;
}

// Now that the actuals are truly resolved, ie. semantics is complete, and the
// template is about to be instantiated.
void 
forward_template_arg_types(Plist formal, Pexpr actuals)
{   
  for (Pexpr actual = actuals ; formal && actual ;
       formal = formal->l, actual = actual->e2)
    switch(formal->f->n_template_arg){
    case template_type_formal:
      actual->e1->tp = non_template_arg_type(Pbase(actual->e1->tp)) ;
      break ;
    case template_expr_formal:
      break ;
    default:
      error ('i', "bad template formal") ;
    }
}
     
static int  
suitable_const_expr(Pname n)
/* 
 * determine whether the supplied expression is suitable 
 * All expressions must be of the form constant integer expression, 
 * or the address of a variable, or an array, or function */
{
// error('d',"suitable_const_expr %n",n);

  	if (n->n_evaluated) return 1;
	Pexpr ee = n->n_initializer;
  	if (ee == 0) return 0;

// error('d',"init: %k e1 %k e2 %k",ee->base,ee->e1?ee->e1->base:0,ee->e2?ee->e2->base:0);
  	switch (ee->base) {
  	case CAST:
  	case G_CAST:
    	{ 
		Pexpr e = ee->e1;
		// if it is a cast of an integer value, it's fine.
      		if (e->base == IVAL || e->base == ICON || 
		    e->base == CCON || e->base == ZERO)
			return 1;
      		return -2; // to permit explicit sorry
    	}
  
  	// case FCON:  /* should be caught in bind_formals() */
  	case ZERO:
    		return 1;
  	case ADDROF:
  	case G_ADDROF:
    	{
		bit mbr = 0;
      		Pexpr e = ee->e2;
		while (e->base == DOT || e->base == REF)
		{ // &x.i, &px->i
			if (e->e1->base == CALL || 
			    e->e1->base == G_CALL)
				return 0;
                        // a sorry requested by directed instantiation
                        if ( e->mem->base != DOT && e->mem->base != REF )
                                error('s',"address of boundM (&%n%k%n) as actualYA",e->e1,e->base,e->mem);
			e = Pexpr(e->mem);	
			++mbr;
		}
		Pname an = Pname(e);
// error('d',"an: %k %n %t",an->base,an->base==NAME?an:0,an->tp);
      		if (an->base != NAME) return 0;
		if (an->n_sto == STATIC) return -3; // explicit static
      		if (an->n_stclass == STATIC || mbr) return 1;
		if (an->tp->base == FCT && 
			Pfct(an->tp)->f_static &&
			Pfct(an->tp)->memof ) 
				return 1; // T (x::*)()
      		return 0;
    	}
  	case NAME:
    	{
      		Pname an = Pname(n->n_initializer);
      		if (an->n_stclass == STATIC && 
 			(an->tp->base == VEC ||
		   	(an->tp->b_const && 
		            an->n_evaluated))) // const
				return 1;
      		return 0;
    	}
	case ILIST:
		// constant pointer to member: int (X::*)()
		if (ee->e2) return 1;
		return 0;
	case STRING:
		return -1; // simplest way to add the explicit sorry
  	default:
    		return 0;
  	}
}

static int 
const_formal_hack(Pname n) 
{ /* templ_inst::bind_formals sets b_const in expr parameters.
   * this break type-checking for binding of formals to insts.  
   * simply not setting it, though, is no answer since the
   * instantiated formal should be handled as a const
   * hence, this ``elegant'' hack ... */
// error('d',"const_formal_hack: %n %k %t const_problem: %d",n,n->tp->base,n->tp,const_problem);

	if (!const_problem) return 0;
  	switch (n->tp->base) {
		// case W_CHAR:
		// case W_STRING:
  		case RPTR: case VEC:
  		case FLOAT: case DOUBLE: case LDOUBLE:
		    return 0; // should not occur

  		case ZTYPE: case CHAR: case SHORT: case INT:
  		case LONG: case FIELD: case EOBJ: case COBJ:
  		case TYPE: case ANY:
    		{ // a basetype node
      			Pbase b = Pbase(n->tp);
			if (b->b_const == 2) return 1;
			return 0;
    		}
  		case PTR:
    		{ 
			Pptr b = Pptr(n->tp);
			if (b->b_const == 2) return 1;
			return 0;
    		}
  	}
  	return 0;
}

char *
make_formal_name(char *fns, char *ins)
{// create formal parameter name: ``fns__<length>ins''
    char s[1024];
    char t[6];
    strcpy(s,fns); // formal name string
    sprintf(t,"__%d",strlen(ins)); // instantiation name string
    strcat(s,t);
    strcat(s,ins);
// error('d',"make_formal_name: %s",s);
    char *result = new char[ strlen(s)+1 ];
    strcpy(result,s);
    return result;
}

void 
templ_inst::bind_formals()
/* Bind the formals to the types passed in as the actuals, for the
 * instantiations, bind the non-type names to their expressions. */
{ 
  	Pexpr actual;
  	Plist formal;
  
  	for (formal = inst_formals, actual = actuals;
       		formal && actual; 
		formal = formal->l, actual = actual->e2)
        {
    	switch (formal->f->n_template_arg) {
      		case template_type_formal:
		{
	  		Ptype t = non_template_arg_type(Pbase(actual->e1->tp));
			if (t->base == VEC) { 
 			        // handle X<char[]> 
      				Pvec v = Pvec(t);
      				if(v->dim == 0 && v->size == 0) 
				    error("actual vectorZ%t must include dimension",actual->e1->tp);
			}
	  		formal->f->tp = t;
	  		PERM(formal->f->tp);
	  		break;
		}
	
      		case template_expr_formal:
		{
	  		actual->e1 = actual->e1->typ(gtbl);

			// insure that T* == T[]
			Ptype t = actual->e1->tp;
			if (t->base == VEC && 
			    formal->f->tp->base == PTR)
				t=new ptr(PTR,Pvec(t)->typ);

	  		if (formal->f->tp->check(t,0)) {
				// const shouldn't be a problem for objects
				if (const_problem && 
					t->is_ptr_or_ref()==0 &&
					formal->f->tp->is_ptr_or_ref()==0)
						; 
				else 
				if (!const_formal_hack(formal->f))
				{	
	    				error("YA mismatch:X %t for formal %n, not %t",
		  		    	     formal->f->tp, formal->f, actual->e1->tp);
					error('i',"cannot recover from previous errors");
				}
			}

	  		// hide the global name around decl processing of the formal name
	  		Pname g = gtbl->look(formal->f->string, 0);
	  		if (g) g->n_key = HIDDEN;

	  		// bind the non type arguments to their expressions
	  		// parameters that are bound at syntax analysis, these parameters are
	  		// bound during dcl processing, so ensure that they can be found.
	  		formal->f->n_initializer = actual->e1;
	  		formal->f->simpl();
	  		formal->f = formal->f->dcl(gtbl, STATIC);
	  		formal->f->n_key = HIDDEN;
			formal->f->n_sto = STATIC;
	  		PERM(formal->f);

	  		non_template_arg_non_type(formal->f);
			int sorry = suitable_const_expr(formal->f);
			// yes, should have constants defined for case labels
			switch ( sorry ) {
			case 0:
	      			error("YA for formal: %sis not a suitable constant.", formal->f->string);
				break;
			case -1: // string literal
		    		error('s',"actualZE ofT string literal");
				error('i',"cannot recover from previous errors");
				break;
			case -2: // cast of non-integer constant
		    		error('s',"cast of non-integer constant");
				break;
			case -3: // address of explicit static name
		    		error("YA for formal %s: address of static identifier",formal->f->string);
				break;
			}
			
	  		if (g) g->n_key = 0;
	  		break;
		}
	
      		default: error ('i', "badY formal");
      		}
  
  	}

  	// now that the formals are bound, compute the instantiation string
  	tname->string = instantiation_string();

  	for (formal=inst_formals; formal; formal=formal->l)
    		if (formal->f->n_template_arg_string)
			error('i', "attempt to bind aYZ multiple times");
    		else 
      			formal->f->n_template_arg_string = make_formal_name(formal->f->string,tname->string);
}

void 
funct_inst::bind_formals()
{ /* bind formal parameters to the actual type arguments 
   * generate the mangled name for this instantiation */
// error('d',"funct_inst::bind_formals: %n status: %d", tname,status );
	int count = def->get_formals_count();

  	for (Plist formal=inst_formals; formal; formal=formal->l)
	{
	/* 3.1/4.0: redo is-ftempl_match to set binding in order of formals */
	/* this will make `bind_formal' faster ... */
// error('d',"\tfunct_inst::bind_formals: formal: %n", formal->f);
		Pbinding p = binding;
		for (int i=0; i<count; ++p,++i) 
		{
// error('d',"\tfunct_inst::bind_formals: p: %n", p->param );
			if (strcmp(formal->f->string,p->param->string)==0)
			{
// error('d',"\tfunct_inst::bind_formals: match !! tp: %t\n", p->typ);

				Ptype t = p->typ;
				if (t->base == ZTYPE) 
					t = int_type;

				if (t->b_const != 0) {
					if (t->base==PTR || t->base==RPTR) {
						Pptr pt = new ptr(t->base,0); 
						*pt = *(Pptr(t));
						pt->b_const = 0;
						t = pt;
					} else {
						Pbase bt = new basetype(t->base,0); 
						*bt = *(Pbase(t));
						bt->b_const = 0;
						t = bt;
					}
				}

				formal->f->tp = t;
	  			PERM(formal->f->tp);
			}
		}
	}
	
  	// now compute the instantiation string
  	tname->string = instantiation_string();

// error('d',"bind_formals: instantiation_string: %s", inst_name);

  	for (formal=inst_formals; formal; formal=formal->l)
	{
    		if (formal->f->n_template_arg_string)
			error('i',"attempt to bind aYP multiple times");
    		else
      			formal->f->n_template_arg_string = make_formal_name(formal->f->string,tname->string);
	}
}

void
templ_inst::explicit_inst() 
{
   	// copy the formals :: need to instantiate string
    	name_list dummy_formal(0,0); 
    	Plist last = &dummy_formal;
      
    	for (Plist formal=def->formals; formal; formal=formal->l) 
	{
      		Pname copy_name = new name("");
      		*copy_name = *formal->f;
      		copy_name->n_tbl_list = 0;
      		last = last->l = new name_list(copy_name,0);
    	}
    	inst_formals = dummy_formal.l;
  
  	bind_formals();
        Pclass cl = tname->tp->classtype();
	cl->class_base = INSTANTIATED;
	cl->templ_base = CL_TEMPLATE; // indicates specialized
        Pbase(tname->tp)->b_name->string = tname->string;
        // namep = insert_type(tname,Ctbl,TYPE);//SYM
        Pktab tb = cl->k_tbl->k_next;
        if (tb == 0)
                tb = Gtbl;
        else 
        if ( tb->k_id == TEMPLATE )
                tb = tb->k_next;
        namep =  insert_type(tname,tb,cl->csu);//SYM
	namep->tp = tname->tp;
        cl->modify_inst_names(tname->string);
        cl->k_tbl->k_name = tname;
        // don't understand why this is necessary ...
        if (cl->k_tbl->k_name->n_ktable == 0)
            cl->k_tbl->k_name->n_ktable = namep->n_ktable;
}

// Expose the non-type parameter names so that they are visible during decl
// processing. Conflicting global names are hidden, so that they are not
// found.
void 
basic_inst::expose_parameter_names() {
  if (hidden_globals)
    error ('i', "an expose without a hide of global names") ;
  
  for (Plist formal = inst_formals ; formal  ; formal = formal->l)
    if (formal->f->n_template_arg == template_expr_formal) {
      // Hide any visible globals
      Pname gname = gtbl->look(formal->f->string, 0) ;
      if (gname) {
	// an existing global name, hide it
	gname->n_key = HIDDEN ;
	// note them for future restoration
	hidden_globals = new name_list(gname,hidden_globals);
      }
      formal->f->n_key = 0 ;	// bring it out of hiding
      if (formal->f != gtbl->look(formal->f->string,0))
	error('i', "parameter could not be located in the global table") ;
    }
}


// Hide the non-type parameter names after an instantiation, and restore any
// globals that may have been hidden during the process.
void 
basic_inst::hide_parameter_names() {
// error('d',"hide_parameter_names()"); 
  for (Plist formal = inst_formals ; formal  ; formal = formal->l)
    if (formal->f->n_template_arg == template_expr_formal) {
      formal->f->n_key = HIDDEN ;
    }
  for (; hidden_globals; hidden_globals= hidden_globals->l)
    hidden_globals->f->n_key= 0 ;
  hidden_globals = 0 ;
}

// Primitives for saving and restoring the compilation state around a template
// instantiation. It also maintains the stack of template instantiations.
void 
basic_inst::save_state(Pname p)  {
  if (next_active) error ('i', "circular instantiation of a template") ;
  context.save() ;
  if (basic_inst::head)
    basic_inst::head->hide_parameter_names() ;
  next_active = basic_inst::head; 
  basic_inst::head = this;
  context.init() ;
  Cdcl = p; Cstmt = NULL;
  curr_file = (Cdcl) ? Cdcl->where.file : 0;
  expose_parameter_names() ;
} 

void 
basic_inst::restore_state() {
  context.restore() ;
  hide_parameter_names() ;
  basic_inst::head = next_active ; next_active = NULL ;
  if (basic_inst::head)
    basic_inst::head->expose_parameter_names() ;
} 

// Copy over the class definition subtree starting from COBJ down to the
// CLASSDEF node. This minimal subtree has to exist during syntax analysis,
// and already contains pointers into it.
void 
templ_inst::kludge_copy(Pbase pbc)
{
  // copy just the COBJ ->b_name NAME ->tp CLASS path for now, note that the
  // preceding path of the tree is pre-allocated, since syntax analysis needs
  // to generate pointers to these objects.
  Pbase pb = Pbase(tname->tp) ;
  Pname save_b_name = pb->b_name ;
  Ptype save_tp = pb->b_name->tp ;

  if ((pb->base != COBJ) || (pbc->base != COBJ))
    error('i',"templ_inst::kludge_copy:(pb %k,pbc %k) cobjX",pb->base,pbc->base);
  
  *pb = *pbc ;
  pb->b_name = save_b_name ;
  *pb->b_name = *pbc->b_name ;
  pb->b_name->tp = save_tp ;
  *Pclass(pb->b_name->tp) = *Pclass(pbc->b_name->tp) ;
  Pclass(pb->b_name->tp)->class_base = INSTANTIATED ;
// ::error('d',"kludge_copy: %n", pb->b_name); 
}


// these statics probably belong in templ_inst and shouldn't be dangling around
static Pbase cobj_node ;
static Pname  cname_node ;
static Pclass class_node ;
static Pfct fct_node;
	     
static void 
syntax_tree_copy_hook(void *,
			     Pnode &,
			     node_class,
			     tree_node_action &action,
			     int& never_see_again)
{
  never_see_again = 1;
  action = tna_continue;
  return;
}


// create a copy of the expression tree
static Pnode 
copy_syntax_tree(Pnode n, int no_types = 0) {
  pointer_hash       cht(default_copy_hash_size) ;
  tree_copy_info  info ;
  if(no_types) info.node_hook = syntax_tree_copy_hook;
  copy_tree (n, info, &cht);
  return n ;
}

bool 
templ_inst::copy_hook(Pnode &node)
{ // hook to perform the copying of the pre-allocated class subtree
  	switch (node->base) {
  		case COBJ:
    			if (node == cobj_node) return false;
    			if (node == def->namep->tp) {
      				*cobj_node =  *Pbase(node);
      				node = cobj_node;
    			}
    			break;

  		case NAME:
    			if (node == cname_node) return false;
    			if (node == sta_name) return false;

    			if (node == Pbase(def->namep->tp)->b_name) {
      				*cname_node= *Pname(node);
      				node = cname_node;		  
    			}
    			break;
  		
		case CLASS:
    			if (node == class_node) return false;
    			if (node == Pbase(def->namep->tp)->b_name->tp) {
				if (class_node==0) return false; 
      				*class_node = *Pclass(node);
      				node = class_node;
    			}
    			break;
  		}
  		return true;
}

/* This hook function used during a class copy.  */
static void 
copy_hook(void /* Ptempl_inst */ *p, Pnode &node,
		      node_class, tree_node_action &action,
		      int& never_see_again)
{
  	action = (Ptempl_inst(p)->copy_hook(node) ? tna_continue : tna_stop ) ;
  	never_see_again = (action != tna_stop);
  	return;
}      

static void 
f_copy_hook(void *p, Pnode &node, node_class, 
	tree_node_action &action, int& never_see_again)
{ // 3.1/4.0: should be merged with global copy_hook
// error('d',"f_copy_hook");

  	action = (Pfunct_inst(p)->f_copy_hook(node) ? tna_continue : tna_stop);
  	never_see_again = (action != tna_stop);
  	return;
}      

// hook to perform the copying of the pre-allocated class subtree
bool 
funct_inst::f_copy_hook(Pnode &node)
{
// error('d',"funct_inst::f_copy_hook: %k %d", node->base , node);
  	switch (node->base) {
  	case NAME:
      		if(node == sta_name) return false;
	  	break;
	case FCT:
    		if (node == fct_node) return false;
    		if (node == def->fn->tp) {
      			*fct_node = *Pfct(node);
      			node = fct_node;
    		}
    		break;
	}
	return true;
}

void 
establish_class_subtree_correspondence(pointer_hash &h, Pname key_tname,
					    Pname value_tname) 
{
  h[int(key_tname)] = int(value_tname) ;
  h[int(key_tname->tp)] = int(value_tname->tp) ;
  h[int(Pbase(key_tname->tp)->b_name)] =
    int(Pbase(value_tname->tp)->b_name) ;
  h[int(Pbase(key_tname->tp)->b_name->tp)] =
    int(Pbase(value_tname->tp)->b_name->tp) ;
}

Pcons 
make_ref_copy(pointer_hash &h, tree_copy_info &info, Pcons old_templ_refs)
{
  cons dummy(0,0), *last = &dummy ;
  for (Pcons pc = old_templ_refs ; pc ; pc = pc->cdr) {
    
    Ptempl_inst t = Ptempl_inst(pc->car) ;
    Pexpr   dummy = new expr(ELIST, 0, 0);
    elist  list(dummy);

// error('d',"make_ref_copy: %n", t->tname);

    // copy the trees corresponding to the actuals
    for (Pexpr actual = t->actuals ; actual ; actual = actual->e2) {
      Pnode root = actual->e1 ;
      copy_tree (root, info, &h);

      // make sure that references to enclosing formals are resolved
      root = Pexpr(root)->typ(gtbl);
      list.add(new expr(ELIST, Pexpr(root), 0)) ;
    }
    Pexpr new_actuals = list.head->e2 ;
    
    // get one if it exists, create one otherwise.
    Ptempl_inst  treal = t->def->get_inst(new_actuals, t) ;
    Pname new_tname = treal->tname ;
    
    last = last->cdr = new cons(treal,0);

    establish_class_subtree_correspondence(h, t->tname, new_tname) ;
  }

  return dummy.cdr ;
}


/*
   Remap the template references from within the body of the template. This
action is similar to the normal tree copy operation; it would normally have
been done during the syntax phase, that produced the tree, but since there
isn't one, for the instantiated body, it must be done here.

*/					     
Pcons 
basic_inst::ref_copy(pointer_hash &h, tree_copy_info &info, Pcons old_templ_refs)
{
  expose_parameter_names() ;
  Pcons new_refs = make_ref_copy(h,info,old_templ_refs) ;
  hide_parameter_names() ;
  return new_refs ;
}

#if 0
static bool 
is_forward_instantiation(Pbase b_base, Pbase f_base)
{
  return bool(b_base->b_name->tp->defined && f_base->b_name->tp->defined) ;
}
#endif

/*****************************************************************************
* 									     *
* If the template instantiation is found to be unique after the decl	     *
* processing of the actuals, create a copy of the post syntax graph for the  *
* class. The edges of the graph are determined by "type nodes" that have     *
* already been defined, and TNAME nodes that are in the global keyword	     *
* table. Special care is also taken to avoid copying nodes whose identity    *
* must be maintained, since cfront uses them for fast type checks, these     *
* nodes always have the "defined" flag turned on and so are never copied.    *
* 									     *
* Copying of the pre-allocated class sub-tree for the template: COBJ	     *
* ->b_name NAME ->tp CLASS						     *
* 									     *
* is handled by the class_copy hook above, that is invoked during the course *
* of the copy.								     *
* 									     *
* Template references from within the class need special handling, since     *
* each instantiation of the class, results in a potentially new template     *
* instantiation.							     *
* 									     *
*****************************************************************************/
Ptempl_inst  
templ_inst::class_copy(Pcons &templ_refs, bool recopy) 
{
		
  // associate the formals with their types, and their expressions
  if (recopy) {
    // remove the class def node from the table, so that it's attributes are
    // copied. 
    corr->del(int(Pbase(def->namep->tp)->b_name->tp)) ;
    corr->del(int(Pbase(def->namep->tp)->b_name)) ;
    corr->del(int(def->namep->tp)) ;
    
    corr->del(int(tname->tp)) ;
    corr->del(int(Pbase(tname->tp)->b_name)) ;
    corr->del(int(Pbase(tname->tp)->b_name->tp)) ;
  }else corr = new pointer_hash(default_copy_hash_size) ;
  
  { // copy the formals & install them in the correspondence table
    name_list dummy_formal(0,0) ; 
    Plist last = &dummy_formal ;
      
    for (Plist formal = def->formals ; formal ; formal = formal->l) {
      Pname copy_name = new name("") ;
      *copy_name = *formal->f ;
      copy_name->n_tbl_list = 0 ;
      last = last->l = new name_list(copy_name, 0) ;
      (*corr)[int(formal->f)] = (int)copy_name ;
    }
    inst_formals = dummy_formal.l ;
  }
  
  bind_formals() ;
  if ( ! recopy ) { 
    // Pname nnn = k_find_name(tname->string,Ctbl,0);
    Pname nnn = k_find_name(tname->string,Gtbl,HIDDEN);
    if ( nnn && nnn->base==TNAME ) {
    // formal binding may result in detecting identical instantiations
    Ptempl_inst ti = def->get_match(actuals, this, true) ;
    if (ti) return ti ;
    error('i', "generated template instantiation name %swas not unique",
	  tname->string) ;
     }
  }
  
  { 
    tree_copy_info  info ;
    info.node_hook = ::copy_hook ;
    info.hook_info = this ;
    
    (*corr)[int(def->namep)] = int(tname) ; // make the tnames correspond

    templ_refs = ref_copy(*corr, info, templ_refs) ;
    Pnode root = def->basep ;	// start the copy at the cobj node

    // deal with these nodes differently during the copy, ie. the nodes
    // themselves are not copied, but their attributes are.
    cobj_node = (Pbase)tname->tp ;
    cname_node = Pbase(tname->tp)->b_name ;
    class_node = Pclass(Pbase( tname->tp)->b_name->tp) ;

    copy_tree (root, info, corr);
  }
  // Perform name modifications for the class, so that it is an
  // instantiation-specific name.
  cname_node->string = tname->string ;
  if (!recopy) {
    // namep =  ktbl->insert(tname, 0) ;
    // namep =  insert_type(tname,Ctbl,TYPE) ;//SYM
    Pktab tb = class_node->k_tbl->k_next;
    if (tb == 0)
        tb = Gtbl;
    else 
    if ( tb->k_id == TEMPLATE )
        tb = tb->k_next;
    namep =  insert_type(tname,tb,class_node->csu);//SYM
    if (reinstat) class_node->k_tbl->k_name = tname;
  }
  else class_node->defined &= ~(DEFINED|SIMPLIFIED) ;
  namep->tp = cobj_node;
    
  class_node->modify_inst_names(cname_node->string) ;

   // don't understand why this is necessary ...
   if (class_node->k_tbl->k_name->n_ktable == 0) {
// error('d',"change k_name: %n",tname);
       class_node->k_tbl->k_name->n_ktable = namep->n_ktable;
    }

  return 0 ;
}

/*
This hook function is responsible for the replacement of references to
expression when copying function bodies
*/
static void 
function_copy_hook(void *current_templ_inst, Pnode &node, node_class,
			     tree_node_action &action, int& never_see_again)
{
  never_see_again = 1;

  switch (node->base) {
  case NAME:
    { 
      if(node == sta_name) {
	  action = tna_stop;
	  return;
      }
      char *s = Pname(node)->string ;
      Pname f =  0 ;
      if (s && (*s == '$') &&
	  (f = Ptempl_inst(current_templ_inst)->get_parameter(s+1))) {
	      if(Pname(node)->n_list) 
		  error ('i', "n_list set in tree template formal.");
	      node = copy_syntax_tree(Pname(f)->n_initializer) ;
	      action = tna_stop ;
	      never_see_again = 0;
	  }else action = tna_continue ;
      return ;
    }
  default:
    action = tna_continue ;
    return ;
  }
}

Pname 
templ_inst::function_copy(Pfunt fnt, Pcons &templ_refs)
/* 
 * Create a copy of a function member, as part
 * of the instantiation of a function body. 
 * The correspondence table is first initialized 
 * with the contents of the correspondence table 
 * used to instantiate the class. 
 * Copying is initiated in this context */
{
	pointer_hash fcorr(*corr); // initialize with old hash table
  
    	{     
    	tree_copy_info info;
    	Pnode root = fnt->fn; 

    /* establish a correspondence between the formals used 
     * for the class template, and the formals used for the 
     * function template, all references to the function 
     * template formals will be replaced by references to 
     * the instantiated class template formals after the 
     * copy has been completed */
    
     for (Plist fformal = fnt->formals, cformal = inst_formals;
	  fformal; fformal = fformal->l, cformal = cformal->l)
     {
	fcorr[int(fformal->f)] = int(cformal->f) ;
	if (fcorr[int(fformal->f)] != int(cformal->f))
		error ('i', "templ_inst::fuction_copy: hash table bug");
      }

      info.node_hook = ::function_copy_hook;
      info.hook_info = this;

      templ_refs = ref_copy(fcorr,info,templ_refs);
      if (fcorr[int(def->namep)] != int(tname))
    	  error ('i', "Y to instantiationTN correspondence is missing");
    
      copy_tree(root,info,&fcorr);
      return Pname(root);
    
    }
}

/*****************************************************************************
* 		       							     *
* A matching template was found at instantiation time, which was not	     *
* detected at syntax analysis time. This can happen, when an instantiation   *
* has as its arguments not real types but template arguments, so that	     *
* matches cannot be detected until the templates are bound. Note that it is  *
* also possible to match a template that is in the process of being	     *
* instantiated further up the instantiation call chain. In such cases, the   *
* kludge_copy operation will copy over an incomplete class subtree, which    *
* will be recopied with the completed one after the instantiation is	     *
* completed, in templ_inst::instantiate.				     *
* 									     *
*****************************************************************************/
Pclass current_instantiation = 0 ;

void 
templ_inst::instantiate_match(Ptempl_inst match) { 
	Pbase pb = Pbase(match->tname->tp) ;
  	kludge_copy(pb) ;
  	forward = match ; // Note that template was matched
}

void 
print_nested_typedef(Pname n, Pclass cl) 
{ /* need to print a nested typedef now because it serves as a parameter 
   * to a class being instantiated -- mark it as printed within its 
   * enclosing class so it is not subsequently printed a second time 
   */
	for (Pname nn = cl->mem_list; nn; nn = nn->n_list) {
		if ( nn->base != TNAME ) continue;
		if ( strcmp(nn->string,n->string) == 0 ) {
			if (nn->n_dcl_printed == 0)
			{
				nn->dcl_print(0);
				nn->n_dcl_printed = 2;
			}
			break;
		}
	}
}

Ptempl_inst
templ_inst::instantiate(bool reinstantiate) {
// ::error('d', "templ_inst::instantiate(%d) tname: %n namep: %n", reinstantiate, tname, namep);

	if ( 	(dtpt_opt && 
		curloc.file==first_file && 
//		dummyinst==0 && 
		notinstflag==0 && 
		(tname==righttname || tname && righttname && !strcmp(tname->string, righttname->string))) /*||
		matchflag==1*/
	) { 
		dummyinst=this;	
//		matchflag=0;
	}


  	Pcons templ_refs = def->templ_refs;
	Templ_type ct = Ptclass(Pbase(tname->tp)->b_name->tp)->class_base;
  	if (! reinstantiate) {
    		switch (ct) {
    			case INSTANTIATED: 
    				return this;
    			case UNINSTANTIATED:
      				break;
    			case VANILLA:
    			case CL_TEMPLATE:	// the canonical template class
    			case FCT_TEMPLATE:	// the canonical template class
			case BOUND_TEMPLATE:
    			default: 
    				error ('i',"attempt to instantiate a non-YC %n",namep);
    		}
  		status = class_instantiated;

    		// Check whether the template has already 
		// been instantiated.  if so, use it.
    		forward_template_arg_types(def->formals, actuals);
    		Ptempl_inst match = def->get_match(actuals, this, true);
    
    		if (match || (match = class_copy(templ_refs, false))) {
//			if (dummyinst==this) matchflag=1;
      			instantiate_match(match);
      			return this;
    		}
  	}
	else class_copy(templ_refs, true);

  	Pbase pb = Pbase(tname->tp);

	if (ansi_opt) {
		TOK csu = Ptclass(Pbase(tname->tp)->b_name->tp)->csu; 
		fprintf(out_file, "%s %s;\n", csu == UNION || csu == ANON ? "union" : "struct", tname->string);
	}

  	// Save the state around decl processing
  	{ 
	save_state(def->namep);
    
    	if (def->open_instantiations++ > MAX_INST_DEPTH) {
      		error ("an infinite instantiation sequence was initiated");
      		def->open_instantiations--;
      		return this;
    	}

    	// Mark the class as instantiated to avoid circular instantiations. 
    	Pclass(pb->b_name->tp)->class_base = INSTANTIATED;

    	// if it is a forward reference, rely on the usual compilation to
    	// provide an error message, if indeed it is an error, and not a
    	// benign forward reference such as: friend class foo<X,Y> 
    	if (def->basep->b_name->tp->defined) 
	{
		// need to reset the `where' of the instantiation to that of definitionf
		pb->b_name->where = def->basep->b_name->where;

      		/* Put out the typedefs for the template parameters 
		 * do this before the call to name::dcl below, 
		 * since dcl processing will emit c declarations
		 * that make use of the type
                 */
      		for (Plist formal=inst_formals; formal; formal=formal->l) {
			if (formal->f->tp->base == TYPE) {
				Pclass cl;
				Pname n = Pbase(formal->f->tp)->b_name;
				if (n->base == TNAME && n->tpdef &&
				   (cl = n->tpdef->in_class) && cl->c_body == 1) 
					print_nested_typedef(n,cl);
			}
			if (formal->f->n_template_arg == template_expr_formal)
			{
				formal->f->dcl_print(0);
			}
		}

      		// Instantiate parameterized types referenced by this template
      		for (Pcons pc = templ_refs ; pc ; pc = pc->cdr)
			Ptempl_inst(pc->car)->instantiate();

		tempdcl = 1;
		cc->stack(); cc->cot=0; cc->not=0; cc->tot=0; cc->c_this=0;
		curr_inst=this;
      		if (!((pb->b_name->dcl(gtbl,EXTERN) == 0) || error_count)) 
		{
			pb->b_name->simpl();
		        Ptype pt = pb->b_name->tp;	
			if (pt->base != CLASS)
	  			error('i',"templ_inst::instantiate(%k),CX",pt->base);
	
			Pclass cl = Pclass(pt);
			current_instantiation = cl;
			pb->b_name->dcl_print(0);
			if (cl->c_body == 3) cl->print_all_vtbls(cl);
			if (!(pt->defined & DEFINED))
	  			error ('i', "templ_inst::instantiate: dclC%t is not yet defined", pt);
			current_instantiation = 0;
      		}
		curr_inst=0;
		cc->unstack();
		tempdcl = 0;
    	}

    // bash every template instantiation class that has been forwarded to
    // it, with the decl processed version.
    for (Ptempl_inst clone = def->insts ; clone ; clone = clone->next)
      if (clone != this) {
	if (clone->forward == this)
	  clone->kludge_copy(Pbase(tname->tp)) ;
	else {
#if 0
	  // resolve references to forward declarations
	  if (this == def->get_match(clone->actuals, clone, true)) {
	    clone->kludge_copy(Pbase(tname->tp)) ;
	    clone->forward = this ;
	  }
#endif
	}
      }
      
    /* this does not fit in with cfront's lazy print strategy
    // dcl_print the member functions, so that they can be referenced
    int i = 0 ;
    for (Pname fn= Pclass(pb->b_name->tp)->memtbl->get_mem(i=1); fn;
	 NEXT_NAME(Pclass(pb->b_name->tp)->memtbl,fn,i))
      if ((fn->base == NAME) && (fn->tp->base == FCT))
	fn->dcl_print(0) ;
    */
    
    restore_state() ;
    def->open_instantiations-- ;
  }
  return this;
}

/* Template Constructors  */

templ_state::templ_state() {
// error('d',"templ_state::templ_state");
	param_end = templp->param_end; 
	params = templp->params; 
	templ_refs = templp->templ_refs; 
	friend_templ_refs = templp->friend_templ_refs; 
	last_cons = templp->last_cons; 
	owner = templp->owner;
	// has_expr_formals = templp->has_expr_formals;
}

templ_state::~templ_state() {
// error('d',"templ_state::~templ_state");
	templp->param_end = param_end; 
	templp->params = params; 
	templp->templ_refs = templ_refs; 
	templp->friend_templ_refs = friend_templ_refs; 
	templp->last_cons = last_cons; 
	templp->owner = owner;
	// templp->has_expr_formals = has_expr_formals;
}

templ::templ(Plist parms, Pname p) 
// template <class T> class X {T t; public: T foo() {return t;}};
{
// ::error('d',"templ::templ(%d %n)", parms, p );

  	formals = parms; // 'T'

	namep = p; // 'X'
  	basep = Pbase(namep->tp) ;

	if (basep->base != COBJ) 
            error("YC%n --%n already declared asTdef (%t) ",p,p,p->tp);

	Ptype t = basep->b_name->tp;
	Pclass cl = Pclass(t);

  	cl->class_base = CL_TEMPLATE;
  	defined = ((t->defined & DEF_SEEN) ? true : false);
  	if (defined) members = cl->mem_list;
  
  	PERM(namep); PERM(namep->tp);
  
	// Chain on to the list of templates for the compilation.
  	next = templp->list; templp->list = this;
}

templ_inst::templ_inst (Pexpr act, Ptempl owner)
/* 
 * Set up the basetype for the class, so that nodes that 
 * need to point to it during syntax processing can do so. 
 * These objects are merely place-holders during syntax 
 * analysis, and are actually filled in during
 * the copy phase of instantiation.
 */
{
// error('d',"templ_inst: %n", owner->namep);

	isa = CLASS;
  	def = owner;
  	tname = new name(def->namep->string);
  	tname->base = TNAME;
  	tname->tp = new basetype(COBJ,new name(def->namep->string));
	Pclass c = new templ_classdef(this);

	Pclass cl = owner->namep->tp->classtype(); // SYM
	if (cl->k_tbl) c->k_tbl = cl->k_tbl; // SYM

  	Pbase(tname->tp)->b_name->tp = c;
  	PERM(tname); PERM(tname->tp); 
	PERM(Pbase(tname->tp)->b_name);
	PERM(Pbase(tname->tp)->b_name->tp);

  	// initialize member list so set_scope can do the right thing
	// SYM archaic
  	c->mem_list = def->classtype()->mem_list;
  
  	actuals = act;
  	next = owner->insts;
  	owner->insts = this;
}

templ_inst::templ_inst (Pexpr act, Ptempl owner, TOK csu)
{ // explicit template class instance of already defined template
// error('d',"templ_inst: %n csu: %k", owner->namep, csu);
	isa = CLASS;
  	def = owner;
  	tname = new name(owner->namep->string);
  	tname->base = TNAME;
  	tname->tp = new basetype(COBJ,new name(owner->namep->string));
	Pclass c = new templ_classdef(this,csu);

	// XXXXX : need to supply some name here for k_name
	c->k_tbl = new ktable(0,0,tname);
        c->k_tbl->k_id = CLASS;

  	Pbase(tname->tp)->b_name->tp = c;
  	PERM(tname); PERM(tname->tp); 
	PERM(Pbase(tname->tp)->b_name);
	PERM(Pbase(tname->tp)->b_name->tp);

  	// initialize member list so set_scope can do the right thing
	// SYM archaic
        // ??????????????? XXXXX
  	// c->mem_list = def->classtype()->mem_list;
  
  	actuals = act;
  	next = owner->insts;
  	owner->insts = this;
}

funct_inst::funct_inst (Pexpr act, Pfunt owner)
/* 
 * mumble
 */
{
// error('d',"funct_inst( act: %d owner: %n", act, owner->fn);
	isa = FCT;
  	def = owner;
  	tname = new name(def->fn->string);
  	tname->tp = new templ_fct(this);
  	actuals = act;

  	next = owner->insts;
  	owner->insts = this;
  	PERM(tname); PERM(tname->tp); 
}

templ_classdef::templ_classdef(Ptempl_inst i)
	: classdef(CLASS) 
{
  inst = i;
  class_base = UNINSTANTIATED;
  string = unparametrized_tname()->string;
// error('d',"templ_classdef::templ_classdef: %s", string );
}

templ_classdef::templ_classdef(Ptempl_inst i, TOK csu)
        : classdef(csu)
{
        inst = i;
        class_base = INSTANTIATED;
        templ_base = CL_TEMPLATE;
// error('d',"templ_classdef::templ_classdef: %s csu %k", string,csu );
}

templ_fct::templ_fct(Pfunct_inst i) : fct(0,0,0) {
// error('d',"templ_fct::templ_fct: %n",i->tname);
  	inst = i;
  	fct_base = UNINSTANTIATED;
}

data_template::data_template(templ &owner,Plist params,Pname n)
{ // Create a new data template.
// ::error('d',"data_template(%n,%n)", owner.namep,n);
  	if (owner.data_end)
    		owner.data_end->next = this;
  	else owner.data = this;
  	owner.data_end = this;
  	formals = params;
  	dat_mem = n;
  	PERM(n); PERM(n->tp);
}

function_template::function_template(templ &owner,Plist params,Pname n)
{ // Create a new function template.
// ::error('d',"function_template(%n,%n)", owner.namep,n);
  	if (owner.fns_end)
    		owner.fns_end->next = this;
  	else owner.fns = this;
  	owner.fns_end = this;
  	formals = params;
  	fn = n;
  	PERM(n); PERM(n->tp);
}

function_template::function_template(Plist params,Pname n)
{ // Create a new function template.
// ::error('d',"function_template(%n)", n);
  	formals = params;
  	fn = n;
	Pfct(n->tp)->fct_base = FCT_TEMPLATE;
	next = templp->f_list; templp->f_list = this;
	templ_refs = 0;
  	PERM(n); PERM(n->tp);
}

Pname 
templ_inst::get_parameter(char *s) {
  	for (Plist formal=inst_formals; formal; formal=formal->l) 
    		if (strcmp(formal->f->string,s)== 0)
      			return formal->f;
  	return 0;
}

Pfunct_inst
funct_inst::tfct_copy(Pcons &templ_refs, bool recopy) 
{
// error('d',"%n->tfct_copy(recopy: %d)",tname,recopy);

  	// associate the formals with their types, and their expressions
  	if (recopy) {
    		// remove the function def node from the table, 
    		// so that it's attributes are copied. 
    		corr->del(int(def->fn));
    	 	corr->del(int(tname));
  	}
  	else corr = new pointer_hash(default_copy_hash_size) ;
  
  	// copy the formals & install them in the correspondence table
    	name_list dummy_formal(0,0); 
    	Plist last = &dummy_formal;
      
    	for (Plist formal=def->formals; formal; formal=formal->l) {
      		Pname copy_name = new name("");
      		*copy_name = *formal->f;
      		copy_name->n_tbl_list = 0;
      		last = last->l = new name_list(copy_name, 0);
      		(*corr)[int(formal->f)] = (int)copy_name;
    	}
    	inst_formals = dummy_formal.l;
  
  	bind_formals();
  	if ( !recopy && gtbl->look(tname->string, 0)) {    
    		// formal binding may result in detecting identical instantiations
    		Pfunct_inst ti = def->get_match(actuals, this, true);
    		if (ti) return ti;
	    	error('i',"generatedY instanceN %s not unique",tname->string);
  	}
  
 	tree_copy_info info;
      	info.node_hook = ::f_copy_hook;
      	info.hook_info = this;

  	Pnode root = def->fn->tp; 
	fct_node = Pfct(tname->tp);

 	// make the tnames correspond ???
 	(*corr)[int(def->fn)] = int(tname); 
	templ_refs = ref_copy(*corr,info,templ_refs);
    	copy_tree(root,info,corr);
	return 0;
}

Pfct current_fct_instantiation;
Pfunct_inst fct_instantiation;

void
funct_inst::instantiate(bool reinstantiate) {
// ::error('d', "funct_inst::instantiate(%d) tname: %n namep: %n",reinstantiate,tname,namep);

  	Pcons templ_refs = def->templ_refs;
	Templ_type ft = Ptfct(tname->tp)->fct_base;
  	if (!reinstantiate) {
    		switch (ft) {
    			case INSTANTIATED: 
				if (	dtpt_opt && 
					fdummyinst==0 && 
					curloc.file==first_file && 
					tempdcl==0
				)
				{
					fdummyinst=this;
					fcurr_inst=this;
					current_fct_instantiation=tname->fct_type();
					if (tname->finst_body()) tname->dcl_print(0);
					current_fct_instantiation=0;
				}
    				return;
    			case UNINSTANTIATED: 
      				break;
    			case VANILLA:
    			case CL_TEMPLATE:	
    			case FCT_TEMPLATE:	
			case BOUND_TEMPLATE:
    			default: 
    				error ('i',"attempt to instantiate a non-YF %n",namep);
    		}
  		status = function_instantiated;

		if (dtpt_opt && fdummyinst==0 && curloc.file==first_file && tempdcl==0)
			fdummyinst=this;


		// class template calls forward_template_arg_type()
		// functions however only take ``type type'' 
		for (Pexpr e = actuals; e; e = e->e2) 
			e->e1->tp = non_template_arg_type(Pbase(e->e1->tp));

		Pfunct_inst dup;
    		if (dup=tfct_copy(templ_refs, false))
		{
			// don't believe it should happen -- let's check
			error('i',"FT %n already instantiated", namep);
			return; 
		}
  	}
	else tfct_copy(templ_refs, true);


	// need to propagate certain fields of the pure template instance
	// tname's ``where'' is at point of instantiation but the
	// statements of the function are at the point of definition
        tname->n_oper = def->fn->n_oper;
	tname->where = def->fn->where; 

	save_state(def->fn); // save state around decl processing
    	if (def->open_instantiations++ > MAX_INST_DEPTH) {
      		error ("%n: an infinite instantiation sequence was initiated",namep);
      		def->open_instantiations--;
      		return;
    	}

    	// Mark the function as instantiated to avoid circular instantiations. 
  	Ptfct ptf = Ptfct(tname->tp);
    	ptf->fct_base = INSTANTIATED;

	// Instantiate parameterized types referenced by this template
	for (Pcons pc=templ_refs; pc; pc=pc->cdr) {
		Ptempl_inst(pc->car)->instantiate();;
	}

	if (reinstantiate && tname->n_table) 
		tname->n_table = 0;

        if (fct_instantiation == 0) 
		fct_instantiation = this;

	fcurr_inst=this;

	cc->stack(); cc->cot = 0; cc->not = 0; cc->tot = 0; cc->c_this = 0;
	if (!((tname = tname->dcl(gtbl, EXTERN)) == 0) || error_count) 
	{
		fcurr_inst=this;
		if (se_opt && tname->finst_body()==0)
			suppress_error++;
		tname->simpl();
		Ptype pt = tname->tp;
		if (pt->base != FCT)
	  		error('i',"funct_inst::instantiate(%k),FX",pt->base);
		if (fct_instantiation != this &&
		    fct_instantiation->namep == namep) {
// error('d',"fct_instantiation: %n",fct_instantiation->tname);
			Pfct f = fct_instantiation->tname->fct_type(); 
			Pblock b = f->body;
			f->body = 0;
			current_fct_instantiation = f;
			Pblock b1;
			if (dtpt_opt && tname->finst_body()==0)
			{
				b1=tname->fct_type()->body;
				tname->fct_type()->body=0;
			}
			fct_instantiation->tname->dcl_print(0);
			if (dtpt_opt && tname->finst_body()==0)
				tname->fct_type()->body=b1;
			current_fct_instantiation = 0;
			f->body = b;
		}	
		current_fct_instantiation = Pfct(pt);

		Pblock b;


		if (se_opt && tname->finst_body()==0)
			suppress_error--;

		if (dtpt_opt && tname->finst_body()==0) { 
			b=tname->fct_type()->body;
			tname->fct_type()->body=0;
		}

		tname->dcl_print(0);

		if (dtpt_opt && tname->finst_body()==0)
			tname->fct_type()->body=b;

		fcurr_inst=0;
		current_fct_instantiation = 0;
    	}

	fcurr_inst=0;
	cc->unstack();

    	restore_state() ;
    	def->open_instantiations--;
	if (fct_instantiation == this)
		fct_instantiation = 0;
}

static int
has_templ_arg( Pclass cl1, Ptclass cl2, Pbinding p, int& ni)
{ /* should be able to meld these two instances of 
   * has_templ_arg -- this is expediency for users
   * cl1: min(X<T1,T2>), cl2: min(X<int,double>)
   * need to bind T1 == int, T2 == double */

	Ptempl t = templp->is_template(cl1->string);
	if (t == 0) error('i',"has_templ_arg: unable to retrieveCY%t",cl1);

	Plist formals = t->get_formals();
	Pexpr actuals = cl2->inst->actuals;

	for (; formals && actuals; formals=formals->l, actuals=actuals->e2) 
	{
		// get the associated actual type
		Ptype at = actuals->e1->tp;
		while (at->base == TYPE) at = at->bname_type();

		// if not bound yet, bind Type to type
		// if already bound, check for consistency
		int i = -1;
		while ( ++i < ni ) {
			if (p[i].param == formals->f) break;
		}

		if ( i < ni ) { // previously bound
                        if(p[i].typ->check(at,0)) {
                                if (!const_problem) 
					return 0;
                        }
			continue;
		}

		p[ni].param = formals->f;
		p[ni++].typ = at;

	} 

	return 1;
}

static int
has_templ_arg( Ptclass cl1, Ptclass cl2, Pbinding p, int& ni)
{ /* cl1: min(X<T1,T2>), cl2: min(X<int,double>)
   * need to bind T1 == int, T2 == double */

	Pexpr formals = cl1->inst->actuals;
	Pexpr actuals = cl2->inst->actuals;

	for (; formals && actuals; formals=formals->e2, actuals=actuals->e2) 
	{
		// find any formal Type
		Pexpr fe = formals->e1;
		if (fe->base != NAME || fe->tp->base != TYPE) continue;
		Pname ftn = fe->tp->bname();
		if (!ftn->is_template_arg()) continue;

		// get the associated actual type
		Ptype at = actuals->e1->tp;
		while (at->base == TYPE) at = at->bname_type();

		// if not bound yet, bind Type to type
		// if already bound, check for consistency
		int i = -1;
		while ( ++i < ni ) {
			if (p[i].param == ftn) break;
		}

		if ( i < ni ) { // previously bound
                        if(p[i].typ->check(at,0)) {
                                if (!const_problem) 
					return 0;
                        }
			continue;
		}

		p[ni].param = ftn;
		p[ni++].typ = at;

	} // end: for (formals && actuals)

	return 1;
}

static bit
formal_not_const(Pname nn) 
/* called by is_ftempl-match if type::check sets const_problem: 
 * ok if problem is that formal is const 
 * yes: could be made recursive if arg is nn->tp ... */
{
// error('d',"formal_not_const %n %t", nn, nn->tp);
   
    Ptype t = nn->tp;
    bit cnst = t->tconst();

loop:
    if (cnst == 0) 
        // parallels type::check's behavior
	if (t->base == PTR || t->base == RPTR) {
		t = Pptr(t)->typ;
		cnst = t->tconst();
		goto loop;
	} 
	return cnst == 0; 
}

static void 
check_valid_formal_type(Ptype et) {
	et = et->skiptypedefs();
	switch( et->base ) {
		case FIELD:
			error('s',"formalYZ of type bit field" );
			break;
		case VOID:
			error("formalYZ of type void illegal");
			break;
	}
}

Pslot*
is_ftempl_match(Pexpr actuals, Pfunt ft) 
{
// error('d',"is_ftempl_match: %n", ft->fn);
	int count = ft->get_formals_count();
	Pslot* parray = new Pslot[count];
	int ni=0;

	Pfct f = ft->fn->fct_type();	
	Pexpr e = actuals;
	Pname nn = f->argtype;

	// turn on special handling of base/derived
// error('d',"template_hier : %d fn: %n", template_hier, ft->fn);
	for( ; e; e=e->e2, nn=nn->n_list) {
		if (nn == 0) { 
			if (f->nargs_known==ELLIPSIS) 
				{return parray;}
			delete parray;
			return 0;
		}

		Pexpr a = e->e1;
		Ptype et = a->tp;

		check_valid_formal_type(et);

		Ptype t = nn->tp;
		if (t->is_ref()) {
			t = Pptr(t->skiptypedefs())->typ;
			if (et->is_ref())
				et = Pptr(et->skiptypedefs())->typ;
		}

		if (t->check(et,OVERLOAD)) {
		    if (const_problem) {
			// ok: f(T,int); const int actual;
      			// ok: f(const T*,const int*); int *actual;
		        if ( t->is_ptr_or_ref() != 0 &&
			     et->is_ptr_or_ref() != 0 &&
			     formal_not_const(nn)) 
			{
				delete parray;
				return 0;
			}
		    } else {
			delete parray;
			return 0;
		        }
		}

		// handle things like formal: Type& actual: char* 
		// *** this should be recursive
		int ptr_count = 0, ref_count = 0;
		Ptype nt = nn->tp;
		Pname ptm = 0;

		if (nt->is_ref()) {
			nt = Pptr(nt->skiptypedefs())->typ;
		}
		while(t=nt->is_ptr()) { 
			++ptr_count; 
			Pptr p = Pptr(t);
			if (p->ptname) { ptm = p->ptname; break; }
			nt = Pptr(t)->typ; 
		}

		while(ptr_count-- && (t=et->is_ptr())) {
			Pptr p = Pptr(t);
			if (p->memof) { et = p->memof; break; }
			et = Pptr(t)->typ;
		}

		Pname bn = ptm;
		if (bn == 0) {
			while(t=nt->is_ref()) { ++ref_count; nt = Pptr(t)->typ; }
			while(ref_count-- && (t=et->is_ref())) et = Pptr(t)->typ; 

			if(nt->base != TYPE) continue;
			while (nt->base == TYPE) {
				bn = nt->bname();
				if (bn->is_template_arg()) 
				    break;
				else bn = 0;
				nt = nt->bname_type();
			}
		}

		while ( et->base == TYPE ) et = et->bname_type();
		if (bn == 0) {
		    if (nt->base == COBJ && et->base == COBJ) 
		    { // declaration:  min(X<T>), call:  min(X<int>)
			Pclass c1 = nt->classtype();
			Pclass c2 = et->classtype();
	
			// formal argument is not a template class -- skip it
			if ( !c1->class_base ) continue;

			// are the two different states of the same class
   			// or is the second class a public base of the first?
			// no?  then skip it....
			if ( c1->class_base == CL_TEMPLATE ) {
			     if (same_class(c1,c2,1) == 0 &&
		                 c2->is_base(c1->string) == 0 ) continue;
			}
			else if (same_class_templ(c1,c2) == 0 &&
		             (c2=c2->is_base(c1->string)) == 0 ) continue;

			// two possibilities -- formal argument is either
			// an abstract template or uninstantiated ...
			int bound_formals = 0;
			if (c1->class_base == CL_TEMPLATE)
			    bound_formals = has_templ_arg(c1,Ptclass(c2),parray,ni);
			else bound_formals= has_templ_arg(Ptclass(c1),Ptclass(c2),parray,ni);

			if ( !bound_formals ) { delete parray; return 0; }
			continue;
		    }
		    else continue;
		}

		Pptr p = 0;
		if (et->base==PTR && Pptr(et)->typ->base == FCT) {
			Pfct f = Pfct(Pptr(et)->typ);
			if (f->fct_base == FCT_TEMPLATE) {
				error("actual argument toFY%n is an uninstantiatedFY",ft->fn);
				delete parray;
				return 0;
			}
				
			if (f->body) {
				Pfct ff = new fct(0,0,0);
				*ff = *f;
				ff->body = 0;
				p = new ptr(PTR,ff);
				p->memof = ((Pptr)et)->memof;
				p->ptname = ((Pptr)et)->ptname;
			}
		}
		if (et->base==VEC) p = new ptr(PTR,Pvec(et)->typ);

		int i = -1;
		while(++i < ni) {
			if(parray[i].param == bn) break;
		}

		if(i < ni) {
			if(parray[i].typ->check(p==0?et:p,0)) {
				if (const_problem)
					continue;

				delete parray;
				return 0;
			}
			continue;
		}

		if ( et->base == OVERLOAD ) {
			delete parray;
			return 0;
		}

		parray[ni].param = bn;
		parray[ni++].typ = p == 0 ? et : p;
	}
	if (nn && !nn->n_initializer) {
		delete parray;
		return 0;
	}
	if (ni < count) return 0;
	return parray;
}

Pname
has_templ_instance(Pname fn, Pexpr arg, bit no_err)
{ /* 
   * invoked by a use of this function, fn: expr::call_fct, ptof
   * args: the actual arguments of the use
   * if matches template function, return instantiated function 
   */
// error('d',"has_templ_instance(%n)", fn);

	if ( fn->is_template_fct() == 0 ) return 0;

	Pfunt ft = templp->is_template(fn->string,FCT);
	if (ft==0) 
		error('i',"%n flagged asYF but not entered inY table",fn);

	if (ft->gen_list && fn->tp->base != OVERLOAD) 
		error('i',"%n gtbl: non-overloaded,Y table: overloaded",fn);

	Pbinding pb = 0;
	Pfunt instance = 0;
	// account for derived->base conversions when matching fnc. templates
	bit used_conv = 0;	// did matching template use conversion ?
	int conv_reqd = 0;	// number of matches requiring conversion
	int no_conv_reqd = 0;	// number of exact matches

	for (Pfunt p=ft; p; p=p->gen_list) { // find matching template
		Pbinding b;
		template_hier = 1;
		Nvirt = 0; // set by classdef::is_base() if conversion used
// error('d',"template_hier : %d fn: %n", template_hier, ft->fn);
		if (b = is_ftempl_match(arg,p)) {
			if (!instance)
			{
				pb = b;
				instance = p;
				used_conv = Nvirt==0 ? 0 : 1;
			}
			// earlier match req'd conversion, this one doesn't
			else if (used_conv && Nvirt==0)
			{
				pb = b;
				instance = p;
				used_conv = 0;
			}
			Nvirt==0 ? no_conv_reqd++ : conv_reqd++;
		}
	}

	if (!instance) {
		if (fn->tp->base == FCT && !no_err) {
			error("use ofYF%n does not match any of itsY definitions", fn);
		}
		template_hier = 0;
		return 0;
	}
	else if ((no_conv_reqd>1) || (no_conv_reqd==0 && conv_reqd>1))
	{
		error("use ofYF%n matches multiple instances",fn);
		// use `instance' for rest of compilation
	}

	// get (or generate) correct instantiation of template
// error('d',"has_templ_instance: instance: %n",instance->fn);

	template_hier = conv_reqd;
	Pfunct_inst fctmpl = instance->get_inst(arg);
	template_hier = 0;
	fctmpl->binding = pb;
	fctmpl->instantiate();
	Pname fct_inst = fctmpl->get_tname();

// error('d',"has_templ_instance: instantiated function %n", fct_inst);
	return fct_inst;
}

void basic_template::dummy()
{
	abort();
}

void basic_inst::dummy()
{
	abort();
}
