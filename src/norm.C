/*ident	"@(#)cls4:src/norm.c	1.12" */
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

norm.c:

	"normalization" handles problems which could have been handled
	by the syntax analyser; but has not been done. The idea is
	to simplify the grammar and the actions accociated with it,
	and to get a more robust error handling

****************************************************************************/
#include "cfront.h"
#include "size.h"
#include "template.h"

//SYM -- table defs
Pktab Gtbl;
Pktab Ctbl;

Pname sta_name = 0;
static Ptype generic_tpdef = 0;

void syn_init()
{
	generic_tpdef = new type;
	generic_tpdef->base = TPDEF;
	PERM(generic_tpdef);
	any_type = new basetype(ANY,0);
	PERM(any_type); any_type->defined = DEFINED;
	dummy = new expr(DUMMY,0,0);
	PERM(dummy);
	dummy->tp = any_type;
	zero = new expr(ZERO,0,0);
	PERM(zero);
	sta_name = new name;
	PERM(sta_name);
//SYM -- init new tables
	Gtbl = new ktable(GTBLSIZE,0,0); //SYM
	Ctbl = Gtbl; //SYM
}

int stcount;

char* make_name(TOK c)
{
	char* s = new char[8];	// as it happens: fits in two words

	if (10000 <= ++stcount) error('i',"too many generatedNs");

	sprintf(s,"__%c%d",c,stcount);
	return s;
}

Pbase basetype::type_adj(TOK t)
{
DB(if(Ndebug>=1)error('d',"'%k'->type_adj(%k) --%t b_xname%n",base,t,this,b_xname););
	if (b_xname) {
		if (base)
			error("badBT:%n%k",b_xname,t);
		else {
			base = TYPE;
			b_name = b_xname;
		}
		b_xname = 0;
	}

	switch (t) {
	case TYPEDEF:
			if(b_typedef)
				error('w',"two typedefs");
			else if(b_inline) {
				error("badBT:%k and%k",INLINE,TYPEDEF);
				b_typedef=0;
			}
			else
				b_typedef = 1;
			break;
	case INLINE:
			if(b_inline)
				error('w',"two inlines");
			else if(b_typedef) {
				error("badBT:%k and%k",TYPEDEF,INLINE);
				b_inline=0;
			}
			else
				b_inline = 1;
			break;
	case VIRTUAL:	b_virtual = 1;	break;
	case CONST:	if (b_const) error('w',"two const declarators");
			b_const = 1;	break;
	case UNSIGNED:	b_unsigned = 1;	break;
	case SHORT:	b_short = 1;	break;
	case LONG:
#ifdef VL
			if (b_long)
				base = VLONG;
			else
#endif
			if (base == DOUBLE)
				base = LDOUBLE;
			else
				b_long = 1;
			break;
	case FRIEND:
	case OVERLOAD:
	case EXTERN:
	case STATIC:
	case AUTO:
	case REGISTER:
		if (b_sto)
			error("badBT:%k%k",b_sto,t);
		else
			b_sto = t;
		break;
	case DOUBLE:
		if (b_long) {
			t = LDOUBLE;
			b_long = 0;
		}
		// no break
	case VOID:
	case CHAR:
	case INT:
	case FLOAT:
		if (base)
			error("badBT:%k%k",base,t);
		else
			base = t; 
		break;
	case SIGNED:
	case VOLATILE:
		error('w',"\"%k\" not implemented (ignored)",t);
		break;
	default:
		error('i',"BT::type_adj(%k)",t);
	}
	return this;
}

Pbase basetype::name_adj(Pname n)
{
DB(if(Ndebug>=1)error('d',"'%k'->name_adj(%n) --%t b_xname%n",base,n,this,b_xname));
	if (b_xname) {
		if (base)
			error("badBT:%n%n",b_xname,n);
		else {
			base = TYPE;
			b_name = b_xname;
		}
		b_xname = 0;
	}

	if ( base==0 
 		&& n->base == TNAME 
 		&& ( n->tp->base!=COBJ //SYM || in_arg_list )) {
 				|| Ctbl->k_id==ARG )) {
 			base = TYPE;
 			b_name = n;
	}
	else
		b_xname = n;

	return this;
}

static TOK type_set( Pbase b )
{
	TOK t = 0;

	if ( b->b_vlong ) t = VLONG;
	else if ( b->b_long ) t = LONG;
	else if ( b->b_short ) t = SHORT;
	else if ( b->b_unsigned ) t = UNSIGNED;
	else if ( b->b_inline ) t = INLINE;
	else if ( b->b_virtual ) t = VIRTUAL;
	else if ( b->b_sto == OVERLOAD ) t = OVERLOAD;
	return t;
}

int declTag = 1;

Pbase basetype::base_adj(Pbase b)
{
DB(if(Ndebug>=1)error('d',"'%k'->base_adj(%t) --%t b_xname%n",base,b,this,b_xname));
	Pname bn = b->b_name;

	switch (base) {
	case COBJ:
	case EOBJ:
		error("NX after%k%n",base,b_name);
		return this;
	}

	TOK t;
	if (base) {
		if (b_name)
			error("badBT:%k%n%k%n",base,b_name,b->base,bn);
		else
			error("badBT:%k%k%n",base,b->base,bn);
	}
	else if ( t = type_set(this)) {
		if (b_name)
			error("badBT:%k%n%k%n",t,b_name,b->base,bn);
		else {
                        if ( declTag++ && !(t==INLINE && b->base==EOBJ)) error("badBT:%k%k%n",t,b->base,bn);
			base=b->base; b_name = bn;
// error('d',"base_adj: t: %k", t );
		}
	}
	else {
		base = b->base;
		b_name = bn;
		b_table = b->b_table;
	}

	return this;
}

Pbase basetype::check(Pname n)
/*
	"n" is the first name to be declared using "this"
	check the consistency of "this"
	and use "b_xname" for "n->string" if possible and needed
*/
{
	b_inline = 0;
	b_virtual = 0;
//error('d',"basetype::check(%n) base %k b_xname %n",n,base,b_xname);

	if (b_xname && (n->tp || n->string)) {
		if (base)
			error("badBT:%k%n",base,b_xname);
		else {
			base = TYPE;
			b_name = b_xname;
		}
		b_xname = 0;
	}

	if (b_xname) {
		if (n->string)
			error("twoNs inD:%n%n",b_xname,n);
		else {
			n->string = b_xname->string;
			b_xname->hide(); //SYM?
		}
		b_xname = 0;
	}

	if (ccl==0
	&& n
	&& n->n_oper==TNAME
	&& !in_typedef
	&& n->n_qualifier==0
	&& n->string) {	// hide type name
		Pname nx = k_find_name(n->string,Ctbl,0); //SYM
		if (nx && nx->base==TNAME) nx->hide();
	}

	int defa = 0;
	switch (base) {
	case 0:
		defa = 1;
		base = INT;
		break;
	case EOBJ:
	case COBJ:
		if (b_name->base == TNAME) error('i',"TN%n inCO %p",b_name,this);
	}

	if (b_long || b_short || b_vlong) {
// FIXME: long long
		TOK vsl;

		if (b_long) vsl = LONG;
		if (b_short) vsl = SHORT;
		if (b_vlong) vsl = VLONG;
		if ((b_long && b_short) || (b_long && b_vlong) || (b_short && b_vlong))
			error("badBT:long short%k%n",base,n);
#ifndef VL
		if (base != INT)
			error("badBT:%k%k%n",vsl,base,n);
		else
#endif
			base = vsl;
		b_short = b_long = b_vlong = 0;
	}

	if (b_typedef && b_sto) error("badBT:Tdef%k%n",b_sto,n);
	b_typedef = b_sto = 0; 

	if (b_linkage) {
		if (1 <= bl_level) error("local linkage directive");
	}

	if (Pfctvec_type == 0) return this;

	if (b_const) {
		if (b_unsigned) {
			switch (base) {
			default:
				error("badBT: unsigned const %k%n",base,n);
				b_unsigned = 0;
			case VLONG:
			case LONG:
			case SHORT:
			case INT:
			case CHAR:
				return this;
			}
		}
		return this;
	}
	else if (b_unsigned) {
		switch (base) {
		case VLONG:
			delete this;
			return uvlong_type;
		case LONG:
			delete this;
			return ulong_type;
		case SHORT:
			delete this;
			return ushort_type;
		case INT:
			delete this;
			return uint_type;
		case CHAR:
			delete this;
			return uchar_type;
		default:
			error("badBT: unsigned%k%n",base,n);
			b_unsigned = 0;
			return this;
		}
	}
	else {
		switch (base) {
		case VLONG:
			delete this;
			return vlong_type;
		case LONG:
			delete this;
			return long_type;
		case SHORT:
			delete this;
			return short_type;
		case INT:
                        if (this==int_type || this==defa_type) return this;
                        delete this;
                        if (defa) return defa_type;
                        return int_type;
		case CHAR:
			delete this;
			return char_type;
		case VOID:
			delete this;
			return void_type;
		case TYPE:
			/* use a single base saved in the keyword */
			if (b_name->n_qualifier) {
				Pbase rv = Pbase(b_name->n_qualifier);
				delete this;
				return rv;
			}
			else {
				PERM(this);
				b_name->n_qualifier = (Pname)this;
				return this;
			}
		default:
			return this;
		}
	}
}

static int
check_redef( Pname on, Pname nn )
{
	// nn is a new typedef
	// on was returned from insert_type()
	// check for conflicts
	// return 1 for duplicate def, else 0
	if (Nold) { //SYM type name was already in symbol table
//error('d',"'%n'->tdef() -- redef%t,%t",on,on->tp,nn->tp);
//error('d'," -- nn->base%k, on->base%k, nn->n_oper%k, on->n_key%k",nn->base,on->base,nn->n_oper,on->n_key);
		if ( on->tp->check(nn->tp,0) == 0 ) { // duplicate typedef
			nn->base = TNAME;
			return 1; // avoid infinite loop for typedef I I;
		}
		if ( nn->tpdef->in_class && nn->tpdef->in_class->csu == ANON ) {
			if ( nn->tpdef->defined == 0 )
				error(&nn->where,"twoDs of %s (one in anonymous union)",nn->string);
		} else if ( on->tpdef )
			error(&nn->where,"%n redefined:Tdef%t andTdef%t",on,on->tp,nn->tp);
		else
			error(&nn->where,"%n redefined:%t andTdef%t",on,on->tp,nn->tp);
		// kluge: avoid printing error twice for nested anon
		nn->tpdef->defined = 1;
		Pname nw = new name;
		Pname x = on; x->n_key = VOID; // anything to get rid of it
		*nw = *nn;
		nw->n_tbl_list = 0;
		nw->n_ktable = 0;
		nw->n_table = 0;
		on = insert_type(nw,Ctbl,TYPE);
		if ( Nold ) error('i',&nn->where,"cannot recover");
		on->n_hidden = x;
	}
	if ( on->n_key == HIDDEN ) {
		if ( nn->tpdef->in_class && nn->tpdef->in_class->csu == ANON ) {
			if ( nn->tpdef->defined == 0 )
				error(&nn->where,"twoDs of %s (one in anonymous union)",nn->string);
		} else
			error(&nn->where,"%n redefined: identifier andTdef",on);
		// kluge: avoid printing error twice for nested anon
		nn->tpdef->defined = 1;
	}
	return 0;
}

Pname basetype::aggr()
/*
	"type SM" seen e.g.	struct s {};
				class x;
				enum e;
				int tname;
				friend cname;
				friend class x;
				int;
				
				typedef int i; // where i is tname

	convert
		union { ... };
	into
		union name { ... } name ;
*/
{
DB(if(Ndebug>=1)error('d',"'%k'->aggr() --%t b_xname%n ccl%t",base,this,b_xname,ccl));
	if (b_xname) {
		if (base) {
			Pname n = new name(b_xname->string);
                        //b_xname->hide();//SYM?
                        b_xname = 0;
			return n->normalize(this,0,0);
		}
		else {
			base = TYPE;
			b_name = b_xname;
			b_xname = 0;
		}
	}

	switch (base) {
	case COBJ:
	{	
		Pclass cl = Pclass(b_name->tp);
		char* s = cl->string;
  	/*SYM?*/if (cl->class_base != CL_TEMPLATE && 
			(cl->in_class == 0 || cl->in_class->class_base != CL_TEMPLATE) &&
			b_name->base == TNAME) 
				error('i',"TN%n inCO",b_name);
		if (b_const) error("const%k%n",cl->csu,b_name);

		if (cl->c_body == 2) {	/* body seen */
			if (s[0]=='_' && s[1]=='_' && s[2]=='C') {
				char* ss = new char[8];	// max size of generated name is 7 chars, see make_name()
				Pname obj = new name(ss);
				strcpy(ss,s);
				if (cl->csu==UNION && cl->class_base!=CL_TEMPLATE) {
					ss[2] = 'O';
					cl->csu = ANON;
					Pname un = obj->normalize(this,0,0);
					//SYM -- export anon union names
					//SYM    to current table
					int i = 1;
					Pname nn=cl->k_tbl->get_mem(i);
					for (; nn; NEXT_NAME(cl->k_tbl,nn,i)) {
					    if ( nn->string==0 ) continue;
					    if ( nn->base == NAME ) {
						insert_name(new name(nn->string),Ctbl);
					    } else if ( nn->tp == 0 ) {
						error('i',"BT::aggr(): nullT for%n in anon union",nn);
					    } else {
						Pname nx = new name(nn->string);
						nx->tp = nn->tp;
						nx->tpdef = nn->tpdef;
						nx->where = nn->where;
						if ( nn->tpdef ) {
						  nx=insert_type(nx,Ctbl,TYPE);
						  check_redef(nx,nn);
						} else {
						switch( nx->tp->base ) {
						case COBJ:
						{ Pclass mcl = (Pclass)Pbase(nn->tp)->b_name->tp;
						  if ( mcl->csu != ANON 
						  &&  !(   mcl->string[0]=='_'
						  	&& mcl->string[1]=='_'
						  	&& mcl->string[2]=='C')
						  ) {
						    nx=insert_type(nx,Ctbl,mcl->csu);
						    if ( Nold ) {
							Pname x = nx->tp->is_cl_obj();
							Pclass ocl = x?Pclass(x->tp):0;
							if ( ocl==0 
							||   strcmp(ocl->string,mcl->string)
							||   (ocl->defined&(DEF_SEEN|DEFINED))
							      && (mcl->defined&(DEF_SEEN|DEFINED))
							)
							  error(&nn->where,"twoDs of %s (one in anonymous union)",nn->string);
							else if ( mcl->defined&(DEF_SEEN|DEFINED) )
							  nx->tp = nn->tp;
						    }
						  }
						}
						  break;
						case EOBJ:
						  if ( nn->string[0] == '_'
						  &&   nn->string[1] == '_'
						  &&   nn->string[2] == 'E' )
						    break;
						  nx=insert_type(nx,Ctbl,ENUM);
						  if ( Nold ) {
						    Ptype t = nx->tp->skiptypedefs();
						    Penum oe = t->base==EOBJ ? (Penum)Pbase(t)->b_name->tp : 0;
						    t = nn->tp->skiptypedefs();
						    Penum ne = t->base==EOBJ ? (Penum)Pbase(t)->b_name->tp : 0;
						    if ( oe==0 
						    ||   strcmp(oe->string,ne->string)
						    ||   (oe->defined&(DEF_SEEN|DEFINED))
						          && (ne->defined&(DEF_SEEN|DEFINED))
						    )
							error(&nn->where,"twoDs of %s (one in anonymous union)",nn->string);
						    else if ( ne->defined&(DEF_SEEN|DEFINED) )
							  nx->tp = nn->tp;
						  }
						  break;
						default:
						  error('i',&nn->where,"missing tpdef forTdefN %s",nx->string);
						} // switch
						} // else tag
//error('d',&nn->where,"exportingTN %n%t",nx,nx->tp);
					    } // else TNAME
					} // for nn
					return un;
				}
				if (cl->class_base!=CL_TEMPLATE)
					error('w',"unusable%k ignored",cl->csu);
			}
			if ( b_sto == FRIEND )
  				error("friend%k%n{...} -- may only declare a friendC",cl->csu,b_name);
			cl->c_body = 1;
			return b_name;
		}
		else {	/* really a typedef for cfront only: class x; */
			if (b_sto == FRIEND) goto frr;
			Pname nn = new name(cl->string);
			nn->tp = b_name->tp;
			nn->n_key = REF;
			return nn;
		}
	}

	case EOBJ:
	{	Penum en = Penum(b_name->tp);
	/*SYM?*/if (b_name->base == TNAME) error('i',"TN%n in enumO",b_name);
		if (b_const) error("const enum%n",b_name);
		if (en->e_body == 2) {
			en->e_body = 1;
			return b_name;
		}
		else {
			error("forwardD of enum%n", b_name);
			en->e_type = int_type;
		}
		return 0;
	}

	case 0:
	{	Pname n = new name(make_name('D'));
		n->tp = defa_type;
		error("NX inDL");
		return n;
	}	
	default:
		if (b_typedef) error('w',"illegalTdef ignored");

		if (b_sto == FRIEND && b_name ) {
		frr:    // see also name::normalize(), stc == FRIEND
			Pname fr = k_find_name(b_name->string,Ctbl,0); //SYM
			if (fr == 0 || fr->base!=TNAME) error('i',"cannot find friend%n",b_name);
			Pname n = new name(b_name->string);
			n->n_sto = FRIEND;
			// If it is a parameterized type, use the instantiation 
			// type, not the general type.
			if ((fr->tp->base == COBJ) && 
			    (Pclass(Pbase(fr->tp)->b_name->tp)->class_base == CL_TEMPLATE)) {
			    if (base == COBJ) 
				n->tp = this;
			    else 
			    if ((base == TYPE) &&
			        (Pbase(this)->b_name->base == TNAME) &&
			        (Pbase(this)->b_name->tp->base == COBJ))
			      		n->tp = Pbase(this)->b_name->tp;
			    else 
				error('i', "basetype wasn't a COBJ");
		        }
			else n->tp = fr->tp;
			return n;
		}
		else {
			Pname n = new name(make_name('D'));
			n->tp = defa_type;
			error("NX inDL");
			return n;
		}
	}
}

//SYM -- removed void local_name()

//SYM void local_restore() removed

//SYM -- local_hide() removed

//SYM void nested_restore() removed

//SYM nested_hide(Pname) removed

//SYM static void nested_hide( Plist l ) removed

int defer_check = 0;
//SYM removed Pname statStat = 0;

void name::hide()
/*
	hide "this": that is, "this" should not be a keyword in this scope
*/
{
	if (base != TNAME) return;
//error('d',"'%n '->hide() -- %t lex_level %d bl_level %d",this,tp,lex_level,::bl_level);
	if (n_key == 0) {
                if (lex_level == bl_level && in_arg_list == 0) {
			if (tp->base != COBJ) { 
//error('d',"%n::hide() -- in_typedef base%k %t %t",this,in_typedef->base,tp,in_typedef);
				    if ( in_typedef && in_typedef->base 
				    &&   tp->base != type_set(Pbase(in_typedef)) 
				    &&   in_typedef->check(tp,0) ) {
							if ( defer_check == 0 )
     								error("%n redefined: previous: %t now: %t", this, tp, in_typedef);
				    }
			} 
			else {
//error('d',"in_typedef%t %d tp%t %d",in_typedef,in_typedef,tp,tp);
//error('d',"in_typedef%k tp%k",in_typedef->base,tp->base);
				if ( in_typedef && in_typedef->base
				&&   in_typedef->check(tp, 0) ) {
					if ( defer_check == 0 )
						error( "%n redefined: previous: %t now: %t", this, tp, in_typedef);
				}
				else {
					Pname nn = Pbase(tp)->b_name;
					Pclass cl = Pclass( nn->tp );
					// check for 'typedef class X X;'
					//       and 'typedef X X;'
					if ( in_typedef )
					    while ( in_typedef->base == TYPE )
						in_typedef = Pbase(in_typedef)->b_name->tp;
					if ( in_typedef
					&&   in_typedef->base==COBJ
					&&   same_class(Pclass(Pbase(in_typedef)->b_name->tp),cl) )
						in_typedef = tp;
					else if ( cl->has_ctor() )
						error( "%n redefined: both aCNWK and %s", this, in_typedef?"a type name":"an identifier" );
				}
			}

                }
// error( 'd', "%n::hide", this );
		if ( n_table == 0 ) {
// error('d',"templp: in progress: %d", templp->in_progress);
		 	Pclass cl = 0; 	
			if ( tp->base == COBJ ) cl = tp->classtype();
			if ( n_template_arg == template_type_formal &&
				templp->in_progress )
					error('s',"reuse of formalYZ%n",this);
			else if ( cl && (cl->class_base == INSTANTIATED || 
					 cl->class_base == UNINSTANTIATED ))
						 error("illegal use of instantiatedYC%t", cl);
			else error('i',"%n->hide() -- no n_table",this);
		}
		insert_name( new name(string), Ctbl );
	}
}

//SYM removed Ntncheck, notReally
//SYM void set_scope(Pname tn) removed

//SYM void restore() removed

int classid; // index of generated class names 
	// saved and reset by grammer at start of each class def
	// so nested tag indices will be relative
	// -- ensures link compatibility for static member anon unions 

Pname start_cl(TOK t, Pname c, Pbcl b)
{
        int mk_local = 0;
	DB( if(Ndebug>=1)
		error('d',"start_cl(%k,%n,%d)  ll %d",t,c,b,c?c->lex_level:0);
	);
	if (c == 0) {
	    int save = stcount; stcount = classid++;
	    c = new name(make_name('C'));
	    stcount = save;
	    c->lex_level -= in_class_decl + 1;
	    if ( in_typedef && c->lex_level ) 
		 mk_local = 1;
	    //SYM else c->lex_level = 0;
	}
	else if ( ccl && ccl->lex_level == c->lex_level
	     &&   strcmp(ccl->string,c->string) == 0) {
		error("%k %s redefines enclosing class",t,c->string);
		error('i', "can't recover from previous errors");
        }
	Pname n = c->tname(t);			/* t ignored */
	n->where = curloc;
	Pbase bt = Pbase(n->tp);		/* COBJ */
	if (bt->base != COBJ) {
		error("twoDs of%n:%t andC",n,bt);
		error('i', "can't recover from previous errors");
	}
	else {
		if (strcmp(n->string,Pbase(n->tp)->b_name->string)!=0)
			error("twoDs of %n:Tdef andC",n);
		if (n->tp->classtype()->csu!=t && (t==UNION || n->tp->classtype()->csu==UNION))
			error("twoDs of%n:%k and%k",n,t,n->tp->classtype()->csu);
		bt->b_name->where = curloc; // line#s in dcl()
	}

	//SYM for ( Pclass tc = ccl; tc; tc = tc->in_class ) {
	//SYM     if ( tc->lex_level == c->lex_level // c not local to mem ftn of tc
	//SYM     &&   strcmp( tc->string, c->string) == 0) {
	//SYM 	error( "C %s redefined", c->string );
	//SYM 	error('i', "can't recover from previous errors");
        
	//SYM }

// error('d',"ccl: %t bl: %d in_class_decl: %d ccl->lex_level: %d",ccl,bl_level,in_class_decl,ccl?ccl->lex_level:0);
        // if (templp->in_progress && (c->lex_level == 0))
        if (templp->in_progress && (c->lex_level == 0) &&
		(ccl == 0 || (!ccl->in_class && ccl->class_base!=CL_TEMPLATE) ||
		  bl_level != ccl->lex_level + in_class_decl + 1 ))
		// bring the template in scope
        	templp->introduce_class_templ(n);

// error('d',"'%n'->start_cl() ll %d",c, c->lex_level);
// error('d',"   templ %d in_cl %d in_memf %d",templp->in_progress, in_class_decl, in_mem_fct);

	if ( templp->in_progress && c->lex_level ) {
		if ( in_class_decl == 0 || in_mem_fct ) 
			if (mk_local==0)
			    error('s',"localC%nWinYF", c); 
			else error('s',"localCWinYF"); 
	}

	// typedef struct {} x;
	if ( mk_local ) {
	    local_class = new name_list( n, local_class );
	    //SYM -- tn stuff removed
	}

	Pclass occl = ccl;
	ccl = Pclass(bt->b_name->tp);		/* CLASS */
	if (ccl->defined) ccl->defined |= IN_ERROR;
	ccl->defined |= DEF_SEEN;

// error('d', "start_cl: %n ccl->in_class: %t lex_level: %d", n, ccl->in_class, n->lex_level );

	ccl->string = n->string;
	ccl->csu = t;

// error('d',"typedef: ccl %t %n", ccl, n);
	if (occl==0)
	{
		// if specialization, ccl->templ_base set within explicit_inst()
		ccl->templ_base = templp->in_progress ? BOUND_TEMPLATE : ccl->templ_base;
	}

//error('d',"ccl%t %d ll %d %s",ccl,ccl,ccl->lex_level,ccl->k_tbl->whereami());
	if (b) {	// list of base classes
		for (Pbcl bx, bb=b, l=0; bb; bb = bx) {
			bx = bb->next;
			bb->next = 0;

			if ( bb->bclass
			//&&   strcmp(ccl->string,bb->bclass->string)==0 
            		&& ( ( bb->bclass->nested_sig
                   		&& ccl->nested_sig
                   		&& strcmp(ccl->nested_sig,
                             		bb->bclass->nested_sig)==0 )
                		|| ( !bb->bclass->nested_sig
                      		&& !ccl->nested_sig
                      		&& strcmp(ccl->string,
                                		bb->bclass->string)==0 ) ) )

				error(&n->where,"%n derived from itself",n);
			else if (l == 0)
				l = bb;
			else {	// append and check for duplicates
				for (Pbcl ll = l;;) {
					 if (bb->bclass && same_class(ll->bclass,bb->bclass)) {
						error("%s has %s asB twice",ccl->string,bb->bclass->string);
						break;
					}

					if (ll->next)
						ll = ll->next;
					else {
						bb->next = l;
						l = bb;
						break;
					}
				}
			}
		}

		ccl->baselist = l;
		//SYM notReally++; set_scope(n); notReally--;
	}
	return n;	
}

void end_cl()
{
if ( templp->parameters_in_progress )  
	error("definition ofC %tWinY parameterL : %d",ccl,templp->parameters_in_progress);

	ccl->c_body = 2;
        // ccl->templ_base may be set within explicit_inst()
	ccl->templ_base = templp->in_progress
			?
			BOUND_TEMPLATE
			:
			ccl->in_class ? ccl->in_class->templ_base : ccl->templ_base;
	ccl = ccl->in_class;
	//SYM -- lots of stuff removed
}

Pbase end_enum(Pname n, nlist* b)
{
// error( 'd', "end_enum: %n ccl: %t", n , ccl ); 
	bit anon = 0;
	if (n == 0) {
		anon = 1;
		n = new name(make_name('E'));
		n->lex_level = bl_level - in_class_decl; //SYM
	}

// error('d',"'%n'->end_enum() ll %d",n, n->lex_level);
// error('d',"   templ %d in_cl %d in_memf %d",templp->in_progress, in_class_decl, in_mem_fct);

  	if ( templp->parameters_in_progress )  
  		error( "definition of enum %sWinY parameterL",anon?"":n->string);

	if ( templp->in_progress && n->lex_level ) {
		if ( in_class_decl == 0 || in_mem_fct ) 
			if (anon==0)
			    error('s',"local enum%nWinYF", n); 
			else error('s',"local enumWinYF"); 
	}

	n = n->tname(ENUM);
	Pbase bt = (Pbase)n->tp;
	if (bt->base != EOBJ) {
		error("twoDs of%n:%t and enum",n,bt);
		error('i', "can't recover from previous errors");
	}
	bt->b_name->where = curloc;
	Penum en = (Penum)bt->b_name->tp;
	en->e_body = 2;
	en->mem = name_unlist(b);
	if (en->defined) {
		// shouldn't be necessary anymore with nested types
		// if ( in_class_decl )
		//	error("%n redefined, enum tag not local to class", n);
		en->defined |= IN_ERROR;
        }
	en->defined |= DEF_SEEN;
	en->templ_base = templp->in_progress
			?
			BOUND_TEMPLATE
			:
			en->in_class ? en->in_class->templ_base : en->templ_base;
	return bt;
}

Pname name::tdef()
/*
	typedef "this"
*/
{
// error('d',&where,"'%n'->tdef()%t in_typedef %d",this,tp,in_typedef);
// error('d',&where,"   lex_level %d tpdef%d",lex_level,tpdef);
// error('d',&where,"   templ %d in_cl %d in_memf %d",templp->in_progress, in_class_decl, in_mem_fct);


        //SYM anon_cl removed
        if (n_qualifier) {
                error("QdN in typedef",this);
                n_qualifier = 0;
        }
	if (tp == 0) error('i',"Tdef%n tp==0",this);

	lex_level = bl_level - in_class_decl;

	if ( ccl && ccl->lex_level == lex_level ) {
//error('d',&where,"Tdef%n ccl%t ll %d",this,ccl,lex_level);
		tpdef = new type;
		tpdef->base = TPDEF;
		PERM(tpdef);
		tpdef->nested_sig = make_nested_name(string,ccl);
		if ( strcmp(ccl->string,string) == 0 ) {
			error(&where,"nestedTdef%n redefines immediately enclosingC",this);
			//SYM lookup mechanism in front end will ignore 
			//SYM    members with same name as class 
		}
		tpdef->in_class = ccl;
	} else
		tpdef = generic_tpdef;

	if ( templp->in_progress && lex_level ) {
		if ( in_class_decl == 0 || in_mem_fct ) 
			error('s',"localTdef%nWinYF", this); 
	}

	Pname n;
	n = insert_type(this,Ctbl,TYPE);//SYM
	if ( tpdef->in_class ) { // nested typedef
// error('d', "*****%s->tdef: %d ccl: %t", string, tpdef, ccl );
		//SYM fields copied by insert()
		if ( tp!=n->tp || tpdef !=n->tpdef ) // error detected below...
			;
		else {
			n->tpdef->templ_base = templp->in_progress
						?
						BOUND_TEMPLATE
						:
						tpdef->in_class->templ_base;
		}
// error('d',"tdef: templ_base: %d", n->tpdef->templ_base);
	}
	// else {
		//SYM removed check for clash w/nested type
	// }

	if ( check_redef(n,this) ) return n;
	n->base = base = TNAME;
	PERM(n);
	PERM(tp);
	if (tp->base == PTR) PERM(Pptr(tp)->typ);

	if (tp->base == COBJ || tp->base == EOBJ ) 
	{	// typedef struct/enum { } s; => struct/enum s {};
		Pname b = Pbase(tp)->b_name;
		if (b->string[0] == '_' && b->string[1] == '_' ) 
		    	switch ( tp->base ) {
				case COBJ: {
		             		if (b->string[2] == 'C') {
						Pclass cl = Pclass(b->tp);
						b->string = n->string;
						cl->string = n->string;
						if (!cl->local_sig)
							cl->c_strlen = strlen(cl->string);
			      		}
			      		break;
				}
				case EOBJ: {
		             		if (b->string[2] == 'E') {
						Penum en = Penum(b->tp);
						b->string = n->string;
						en->string = n->string;
						if (!en->local_sig )
							en->e_strlen = strlen(en->string);
					}	
		    		}
	        	}
	}

	DB(if(Ndebug>=1) {
	   error('d',&where,">>'%n'->tdef()%t returning",this,tp);
	   error('d',&where,"   lex_level %d tpdef%t",lex_level,tpdef);
	});
	return n;
}

Pname name::tname(TOK csu)
/*
	"csu" "this" seen, return typedef'd name for "this"
	return	(TNAME,x)-tp->(COBJ/EOBJ,y)-b_name->(NAME,z)-tp->(csue)
*/
{
//error('d',"'%n'::tname(%k)",this,csu);
	char* s = 0;
	bit nt = 0;
	switch (base) {
	case TNAME:
		return this;
	case NAME:
	{	Pname tn = insert_type(this,Ctbl,csu);//SYM
		Pname on = new name;
		tn->base = TNAME;
		tn->lex_level = lex_level;
		//SYM tn->n_list = n_list = 0;
		n_list = 0;
		string = tn->string;
		*on = *this;
		switch (csu) {
		case ENUM:
			tn->tp = new basetype(EOBJ,on);
			on->tp = new enumdef(0);
			Penum(on->tp)->string = tn->string;
			Penum(on->tp)->lex_level = tn->lex_level;
			Penum(on->tp)->in_class = ccl;
			Penum(on->tp)->e_strlen = strlen(tn->string);
			break;
		case CLASS:
		case STRUCT:
		case UNION:
			on->tp =  new classdef(csu);
			s = Pclass(on->tp)->string = tn->string;
			Pclass(on->tp)->lex_level = lex_level;
			//SYM allocate k_tbl here instead of in PUSH_CLAA_SCOPE
			//SYM to avoid problems with forward refs to template
			//SYM class defs
			Pclass(on->tp)->k_tbl = new ktable(0,0,tn);
			Pclass(on->tp)->k_tbl->k_id = CLASS;
			Pclass(on->tp)->in_class = ccl;
			tn->tp = new basetype(COBJ,on);
			Pbase(tn->tp)->b_table = Pclass(on->tp)->memtbl;
			Pclass(on->tp)->c_strlen = strlen(tn->string);
			break;
		default:
			error('i',&where,"illegal csu%k for%n in name::tname()",csu,this);
		}
		if ( ccl && lex_level == ccl->lex_level ) {
			on->tp->nested_sig = make_nested_name(string,ccl);
			if (s)
				s = on->tp->nested_sig, nt = 1;
		} else if ( lex_level ) {
			on->tp->local_sig = make_local_name(on->tp,Ctbl->k_name);
			if (s)
				s = on->tp->local_sig;
		}
		PERM(tn);
		PERM(tn->tp);
		PERM(on);
		PERM(on->tp);

		if (s && ansi_opt && !templp->in_progress)
			fprintf(out_file, "%s %s%s;\n", csu == UNION || csu == ANON ? "union" : "struct", nt ? "__" : "", s);
		return tn;
	}
	default:
		error('i',"tname(%s %d %k)",string,this,base);
		return 0;
	} 
}

int co_hack;
Pname name::normalize(Pbase b, Pblock bl, bit Cast)
/*
	if (bl) : a function definition (check that it really is a type

	if (Cast) : no name string

	for each name on the name list
	invert the declarator list(s) and attatch basetype
	watch out for class object initializers

	convert
		struct s { int a; } a;
	into
		struct s { int a; }; struct s a;
*/
{
	Pname n;
	Pname nn;
	TOK stc;
	bit tpdf;
	bit inli;
	bit virt;
	char * lnkg;
	DB( if(Ndebug>=1) {
	    error('d',"'%n'::normalize(b%t, bl %d, cast %d)",this,b,bl,Cast);
	    error('d',"    tp%k - lex_level %d - bl_level %d",tp?tp->base:0,lex_level,bl_level);
	});
	if (b) {
		stc = b->b_sto;
		tpdf = b->b_typedef;
		inli = b->b_inline;
		virt = b->b_virtual;
		lnkg = b->b_linkage;
	}
	else {
		stc = 0;
		tpdf = 0;
		inli = 0;
		virt = 0;
		lnkg = 0;
	}

	if (inli && stc==EXTERN)  {
		error("both extern and inline");
		inli = 0;
	}

	if ( stc==STATIC && tp && 
  		tp->base == FCT 
		&& Pfct(tp)->f_const ) 
     			error( "%n staticMF cannot be const", this ); 

	if ( stc == FRIEND ) {
//error('d',"norm friend%n tp%t b%t",this,tp,b);
	    if ( tp || (b && (b->base || b->b_name || b->b_xname)) ) {
		// friend dec with type mods -- enter as non type name
  		if ( Ctbl->k_id != CLASS && Ctbl->k_id != TEMPLATE) 
  			error("friend%n not inC",this);
		else if ( n_qualifier == 0 ) {
			//XXXXX currently enter all unqualified
			//XXXXX    friends in Gtbl
			//XXXXX Should eventually enter in enclosing
			//XXXXX    scope, however back end currently
			//XXXXX    doesn't support this, due to lack
			//XXXXX    of context info.
			//XXXXX Commented code below will enter name
			//XXXXX    correctly when back end is brought
			//XXXXX    up to date.
			//Pktab tb = Ctbl->k_next;
			//if ( tb->k_id == TEMPLATE ) tb = tb->k_next;
			//--lex_level;
			//insert_name(this,tb);
			lex_level = 0;
			insert_name(this,Gtbl);
		}
	    } else {
		/*	friend x;
			must be handled during syntax analysis to cope with
				class x { friend y; y* p; };
			"y" is not local to "x":
				class x { friend y; ... }; y* p;
			is legal

			examples:
				
				typedef void	SIG_TYP(int);
				class x {
					friend class y;
					friend z;
					friend x;		// dumb
					friend int i;		// error
					friend SIG_TYP sigFunc;	// subtle
					friend int f();
					friend g(int);
				};
		*/
		if (n_list) {
			error("L of friends");
			n_list = 0;
		}

//error( 'd', "%n ll: %d", ccl, ccl->lex_level );
		lex_level = ccl->lex_level;
		//SYM k_find_name should have failed for visible tname...
		Pname nx = k_find_name(string,Ctbl->k_next,0);//SYM
		if ( nx && nx->base==NAME ) {
			error(&where,"illegal friendD:F signature required for nonTN%n",nx);
			nx = nx->n_hidden;//SYM
		}
		if ( nx == 0 ) {//SYM
			//XXXXX currently enter all unqualified
			//XXXXX    friends in Gtbl
			//XXXXX Should eventually enter in enclosing
			//XXXXX    scope, however back end currently
			//XXXXX    doesn't support this, due to lack
			//XXXXX    of context info.
			//XXXXX Commented code below will enter name
			//XXXXX    correctly when back end is brought
			//XXXXX    up to date.
			Pktab otbl = Ctbl;
			Pclass occl = ccl;
			if ( Ctbl->k_id != CLASS && Ctbl->k_id != TEMPLATE )
				error("friend%n not inC",this);
			Ctbl = Gtbl;
			lex_level = 0;
			ccl = 0;
			//if ( Ctbl->k_id == CLASS || Ctbl->k_id == TEMPLATE ) {
				// enter in enclosing scope
				//Ctbl = Ctbl->k_next;
				//if (Ctbl->k_id == TEMPLATE) Ctbl = Ctbl->k_next;
				//lex_level = ccl->lex_level;
				//ccl = ccl->in_class;
			//}
			nx = tname(CLASS);
			Ctbl = otbl;
			ccl = occl;
		}
		if ( nx->n_key == HIDDEN ) //SYM
			error('w',"friend%n referring to hiddenTN",this);//SYM
//error('d',"friend%n --%k%t tbl %s",nx,nx->base,nx->tp,nx->n_ktable->whereami());
//error('d',"   tbase%k",nx->tp->base);
		n_sto = FRIEND;
		tp = nx->tp;
		return this;
	    }
	} // if friend
	if (tp  			// FUDGE: fix the bad grammar
	&& tp->base==FCT
	&& (n_oper==TNAME || Pfct(tp)->returns)) {
		Pfct f = Pfct(tp);
		Pfct f2 = Pfct(f->returns);

		if (f2) {
			Ptype pt;
			Ptype t = f2;
		lxlx:
			switch (t->base) {
			case RPTR:
			case PTR:	// x(* p)(args) ?
			case VEC:	// x(* p[10])(args) ?
				if (pt = Pptr(t)->typ) {
					if (pt->base == TYPE) {
						Pptr(t)->typ = 0;
						b = Pbase(pt);
					}
					else {
						t = pt;
						goto lxlx;
					}
				}
				goto zse1;
			case FCT:
			{//	Pexpr e = f2->argtype;
				Pexpr e = Pfct(f)->argtype;
				if  (e && e->base==ELIST) { // get the real name; fix its type
					if (e->e2 || e->e1->base!=DEREF) goto zse1;
					Pexpr ee = e->e1;
					Ptype t = 0;
					Ptype tpx;
				ldld:
					switch (ee->base) {
					case DEREF:
					{	Ptype tt = (ee->e2) ? Ptype(new vec(0,ee->e2)) : Ptype (new ptr(PTR,0));
						if (t)
							Pptr(t)->typ = tt;
						else
							tpx = tt;
						t = tt;
						ee = ee->e1;
						goto ldld;
					}
					case NAME:
					{	Pname rn = Pname(ee);
						{ Pname btn = k_find_name(string,Ctbl,0);//SYM
						  if(btn&&btn->base==NAME)btn=0;
						  b=new basetype(TYPE,btn);//SYM
						}
						f->returns = tpx;
						n_oper = 0;
						string = rn->string;
						base = NAME;
					}
					}
				}
			}
			}
		}
	}

zse1:
	if (b == 0) {
		error("BTX for %s",string);
		b = Pbase(defa_type);
	}
	
	if (Cast) string = "";
	b = b->check(this);

	switch (b->base) {	//	separate class definitions
				//	from object and function type declarations
	case COBJ:
		nn = b->b_name;
		if ( !Cast && Pclass(nn->tp)->c_body==2) {
			/* first occurrence */
//error('d',"%n   c_body==2",nn);
			if ( stc == FRIEND ) {
			    Pclass cl = Pclass(nn->tp);
			    if ( cl->csu == ANON )
				error( &nn->where, "friend anonymous union");
			    else
				error( &nn->where, "%k%n defined in friendD",cl->csu,nn);
			}
			if (tp && tp->base==FCT && co_hack == 0) {
				error(&this->where,"%k%n defined as returnT for%n (did you forget a ';' after '}' ?)",Pclass(nn->tp)->csu,nn,this);
				nn = this;
				break;
			}
			nn->n_list = this;
			Pclass(nn->tp)->c_body = 1;	/* other occurences */
		}
		else
			nn = this;
		break;
	case EOBJ:
		nn = b->b_name;
		if (Penum(nn->tp)->e_body==2) {
			if (tp && tp->base==FCT) {
				error(&this->where,"enum%n defined as returnT for%n (did you forget a ';'?)",nn,this);
				nn = this;
				break;
			}
			nn->n_list = this;
			Penum(nn->tp)->e_body = 1;
		}
		else {
			Penum en = Penum(nn->tp);
			if ( en->defined == 0 ) 
				error( "forwardD of enum%n", nn );
			en->e_type = int_type; 
			nn = this;
		}
		break;
	default:
		nn = this;
	}

//error('d',&where,"name::normalize: nn%n ll %d   nn %d this %d",nn,nn->lex_level,nn,this);
	Pname nx; 
	Ptype btyp;

	if(b->base==TYPE && b->b_const)
		btyp = b->mkconst();
	else
		btyp = b;

	for (n=this; n; n=nx) {
		Ptype t = n->tp;
		nx = n->n_list;
		n->n_sto = stc;

		if (n->base == TNAME) error('i',"redefinition ofTN%n",n);

		if (t == 0) {
			if (bl == 0) {
				n->tp = t = btyp;
				goto skipp;
			}
			else {
				if ( n->base == NAME && n->n_oper )
					error(&n->where,"illegalD of%n",n);
				else
					error(&n->where,"body of nonF%n",n);
				t = new fct(0,0,0);
			}
		}

		switch (t->base) {
		case PTR:
		case RPTR:
			n->tp = Pptr(t)->normalize(btyp);
			break;
		case VEC:
			n->tp = Pvec(t)->normalize(btyp);
			break;
		case FCT:
			n->tp = Pfct(t)->normalize(btyp);
			break;
		case FIELD:
			{
			if (n->string == 0) n->string = make_name('F');
			n->tp = t;

			bit cft = 0;
			Ptype tb = btyp->skiptypedefs(cft);

			switch (tb->base) {
			case CHAR:
			case SHORT:
			case LONG:
			case VLONG:
			case EOBJ:
			case INT:
//  typedef const unsigned cu_int;
//     struct x { x(); cu_int b1: 2; }

				Pbase(t)->b_fieldtype = Pbase(tb)->b_unsigned ? uint_type : int_type;
				Pbase(t)->b_unsigned = Pbase(tb)->b_unsigned;
				Pbase(t)->b_const = cft;
				break;
 			case ANY:
// error('d',"templp->in_progress %d", templp->in_progress);
				if (templp->in_progress)  // T t : 13;
  					Pbase(t)->b_fieldtype = int_type;
 				break;
			default:
				error("non-int field");
				n->tp = defa_type;
			}
			break;
			}
		}
skipp:

		Pfct f = Pfct(n->tp);

		if (f->base != FCT) {
			if (bl) {
				error("body for nonF%n",n);
				n->tp = f = new fct(defa_type,0,0);
				continue;
			}
			if (inli) error("inline nonF%n",n);
			if (virt) error("virtual nonF%n",n);
				
			if (tpdf) {
// error('d', "%n->normalize: ccl: %t", this, ccl );
				if (in_arg_list) {
					error("Tdef inA list");
					continue;
				}

				if (n->n_initializer) {
					error("Ir forTdefN%n",n);
					n->n_initializer = 0;
				}
				n->tdef();
				// because do_nl_type() can't call tdef()
				//SYM removed stuff
			}
			continue;
		}

		if ( lnkg ) set_linkage(lnkg);
		f->f_linkage = linkage;
		if ( lnkg ) set_linkage(0);
		// wait and call f->sign() after args are checked

		extern int inline_opt;
		if (inline_opt)
			f->f_inline = (bl && strcmp(n->string, "main") ? 1 : 0);
 		else
 			f->f_inline = inli;
//		f->f_inline = inli;
		f->f_is_inline = f->f_inline;
		extern int vcounter;
                f->f_virtual = virt?(vcounter++,VTOK):0;

		if (tpdf) {
			if (f->body = bl) {
				error("Tdef%n { ... }",n);
				f->body = bl = 0;
			}
			if (n->n_qualifier) {
				// typedef T x::f(args);
				// a pointer to member fucntion:
				// equivalent to typedef T x::(f)(args);
				f->memof = Pclass(Pbase(n->n_qualifier->tp)->b_name->tp);
				n->n_qualifier = 0;
			}
			n->tdef();
			// because do_nl_type() can't call tdef()
			//SYM removed stuff
			continue;
		}

		if (f->body = bl) continue;

		/*
			Check function declarations.
			Look for class object instantiations
			The real ambiguity:		; class x fo();
				is interpreted as an extern function
				declaration NOT a class object with an
				empty initializer
		*/
		{	Pname cn = f->returns->is_cl_obj();
                        Ptype template_formal_type;
			bit clob = (cn || cl_obj_vec);

			if (f->argtype) { /* check argument/initializer list */
				Pname nn;

				for (nn=f->argtype; nn; nn=nn->n_list) {
					if (nn->base != NAME) {
						if (!clob) {
							if ((f->returns->base == TYPE) &&
                                                  	    (Pbase(f->returns)->b_name->n_template_arg == template_type_formal))                                    
                                                {
                                                		template_formal_type = f->returns;
                                                		goto is_obj;
                                                        }
							//error(&n->where,"ATX for%n",n);
							goto zzz;
						}
						goto is_obj;
					}
					if (nn->tp) goto ok;
				}
				if (!clob) {
				//	error("FALX");
					goto zzz;
				}
		is_obj:
				/* it was an initializer: expand to constructor */
				n->tp = f->returns;
				if (f->argtype->base != ELIST) f->f_args = f->argtype = (Pname)new expr(ELIST,(Pexpr)f->argtype,0);
				if ( n->n_initializer ) {
					error(&n->where,"twoIrs for%n",n);
					DEL( ((Pexpr)f->argtype) );
					f->argtype = 0;
				} else {
                                        n->n_initializer = new texpr(VALUE, cn ? cn->tp : template_formal_type, (Pexpr)f->argtype);
				}
				goto ok;
			zzz:
				// ctor-style ilitializer for non class object
				if (f->argtype) {
					n->tp = f->returns;
					if (f->argtype->base == ELIST) {
					    if ( f->argtype->e2 == 0 ) {
						// int i(x)
						f->argtype = (Pname)f->argtype->e1;
					    } else { // int i( x, y )
						error("more than oneA for basicTIr");
						for ( Pexpr e = f->argtype;  e->e2->e2;  e = e->e2 )
							e->base = CM;
						e->base = CM;
						e->e2 = e->e2->e1;
					    }
					}
					if ( n->n_initializer ) {
						error(&n->where,"twoIrs for%n",n);
						DEL( ((Pexpr)f->argtype) );
						f->argtype = 0;
					} else {
                                        	n->n_initializer = (Pexpr)f->argtype;
					}
					goto ok;
				}
			}
			else {	/* T a(); => function declaration */
/*
				if (clob) {
					DEL(n->tp);
					n->tp = f->returns;
				}
*/
			}
		ok:
			;
		}
	}
	return nn;
}

Ptype vec::normalize(Ptype vecof)
{
	Ptype t = typ;
	typ = vecof;

	while(vecof->base == TYPE)
		vecof = Pbase(vecof)->b_name->tp;
		
	switch (vecof->base) {
	case RPTR:
			error("array ofRs");
			break;
	case FCT:
			error("array ofFs");
			break;
	case VOID:
			error("array of void");
			break;
	default:
			break;
	}

	if (t == 0) return this;

	switch (t->base) {
	case PTR:
	case RPTR:	return Pptr(t)->normalize(this);
	case VEC:	return Pvec(t)->normalize(this);
	case FCT:	return Pfct(t)->normalize(this);
	default:	error('i',"bad arrayT(%d)",t->base); return 0;
	}

}

Ptype ptr::normalize(Ptype ptrto)
{
//	if (this == 0) error('i',"0->ptr.normalize()");
	Ptype t = typ;
	typ = ptrto;

	int bc = 0;
	while (ptrto->base == TYPE) {
		bc += Pbase(ptrto)->b_const;
		ptrto = Pbase(ptrto)->b_name->tp;
	}

	switch (ptrto->base) {
	case FCT:
		if (memof)
			if (Pfct(ptrto)->memof) {
				if (!same_class(memof,Pfct(ptrto)->memof)) error("P toMF mismatch: %s and %s",memof->string, Pfct(ptrto)->memof->string);
			}
			else
				Pfct(ptrto)->memof = memof;
		else
			memof = Pfct(ptrto)->memof;
		break;
	case RPTR:
		switch (base) {
		case PTR:	error("P toR"); break;
		case RPTR:	error("R toR"); break;
		}
		break;
	case VOID:
		if(base==RPTR)
			error("R to void");
		break;
	}

	if (t == 0) {
		Pbase b = Pbase(ptrto);
		if (Pfctvec_type
		&& b_const==0
		&& b->b_unsigned==0
		&& b->b_const==0
		&& bc == 0
		&& memof==0 && ptname==0
		&& base==PTR) {
			switch (b->base) {
			case INT:	delete this;	return Pint_type;
			case CHAR:	delete this;	return Pchar_type;
			case VOID:	delete this;	return Pvoid_type;
			}
		}
		return this;
	}

	switch (t->base) {
	case PTR:
	case RPTR:	return Pptr(t)->normalize(this);
	case VEC:	return Pvec(t)->normalize(this);
	case FCT:	return Pfct(t)->normalize(this);
	default:	error('i',"badPT(%k)",t->base); return 0;
	}
}

Ptype fct::normalize(Ptype ret)
/*
	normalize return type
*/
{
//error('d',"%d%t()->norm(%d%t)",returns,returns,ret,ret);
	register Ptype t = returns;
	returns = ret;

	if (argtype && argtype->base==NAME && argtype->n_qualifier) {
		error("syntax: ANX");
		argtype = 0;
		nargs = 0;
		nargs_known = 0;
	}

	while(ret->base == TYPE)
		ret = Pbase(ret)->b_name->tp;

	switch(ret->base) {
	case VEC:
		error("F returning array");
		break;
	case FCT:
		error("F returningF");
//error('d',"t %d%k%t",t,t->base,t);
		returns = ret = any_type;
		break;
	default:
		break;
	}

	if (t == 0) return this;

	switch (t->base) {
	case PTR:
	case RPTR:	return Pptr(t)->normalize(this);
	case VEC:	return Pvec(t)->normalize(this);
	case FCT:	return Pfct(t)->normalize(this);
	default:	error('i',"badFT:%k",t->base); return 0;
	}
}

void fct::argdcl(Pname dcl, Pname fn)
/*
	sort out the argument types for old syntax:
			f(a,b) int a; char b; { ... }
	beware of
			f(a) struct s { int a; }; struct s a;
*/
{
	Pname n;
/*fprintf(stderr,"%d argtype %d %d dcl %d %d\n",this, argtype, argtype?argtype->base:0, dcl, dcl?dcl->base:0); fflush(stderr);*/
	switch (base) {
	case FCT:	break;
	case ANY:	return;
	default:	error('i',"fct::argdcl(%d)",base);
	}

	if (argtype) {
		switch (argtype->base) {
		case NAME:
			if (dcl) error("badF definition syntax");
			for (n=argtype; n; n=n->n_list) {
				if (n->string == 0) n->string = make_name('A');
			}
			return;
		case ELIST:	// expression list:	f(a,b,c) int a; ... { ... }
				// scan the elist and build a NAME list
		{
			Pname tail = 0;
			n = 0;

			error(strict_opt?0:'w',&fn->where,"old style definition of%n (anachronism)",fn);

			for (Pexpr e=Pexpr(argtype); e; e=e->e2) {
				Pexpr id = e->e1;
				if (id->base != NAME) {
					error("NX inAL");
					argtype = 0;
					dcl = 0;
					break;
				}
				Pname nn = new name(id->string);
				if (n)
					tail = tail->n_list = nn;
				else
					tail = n = nn;
			}
			f_args = argtype = n;
			break;
		}
		default:
			error("ALX(%d)",argtype->base);
			argtype = 0;
			dcl = 0;
		}
	}
	else if(nargs_known == ELLIPSIS) {
		return;
	}
	else {
		nargs_known = 1;
		nargs = 0;
		if (dcl) error("ADL forFWoutAs");
		return;
	}

//	nargs_known = 0;

	if (dcl) {
		Pname d;
		Pname dx;
		/*	for each  argument name see if its type is specified
			in the declaration list otherwise give it the default type
		*/

		for (n=argtype; n; n=n->n_list) {
			char* s = n->string;
			if (s == 0) {
				error("AN missing inF definition");
				n->string = s = make_name('A');
			}
			else if (n->tp) error("twoTs forA %s",n->string);

			for (d=dcl; d; d=d->n_list) {
				if (strcmp(s,d->string) == 0) {
					if (d->tp && d->tp->base == VOID) {
						error("voidA%n",d);
						d->tp = any_type;
					}
					n->tp = d->tp;
					n->n_sto = d->n_sto;
					d->tp = 0;	// now merged into argtype
					goto xx;
				}
			}
			n->tp = defa_type;
		xx:;
			if (n->tp == 0) error("noT for %s",n->string);
		}
	
		/*	now scan the declaration list for "unused declarations"
			and delete it
		*/
		for (d=dcl; d; d=dx) {
			dx = d->n_list;
			if (d->tp) {	/* not merged with argtype list */
				/*if (d->base == TNAME)  ??? */
				switch (d->tp->base) {
				case CLASS:
				case ENUM:
					/* WARNING: this will reverse the order of
					   class and enum declarations
					*/
					d->n_list = argtype;
					f_args = argtype = d;
					break;
				default:
					 error("%n inADL not inAL",d);
				}
			}
		}
	}

	/* add default argument types if necessary */
	for (n=argtype; n; n=n->n_list) {
		if (n->tp == 0) n->tp = defa_type;
		nargs++;
	}
}

Pname cl_obj_vec;	/* set if is_cl_obj() found a array of class objects */
Pname eobj;		/* set if is_cl_obj() found an enum */

Pname type::is_cl_obj()
/*
	returns this->b_name if this is a class object
	returns 0 and sets cl_obj_vec to this->b_name
		if this is a array of class objects
	returns 0 and sets eobj to this->b_name
		if this is an enum object
	else returns 0
*/
{
	bit v = 0;
	register Ptype t = this;

	if (t == 0) return 0;
	eobj = 0;
	cl_obj_vec = 0;
xx:
	switch (t->base) {
	case TYPE:
		t = Pbase(t)->b_name->tp;
		goto xx;

	case COBJ:
		if (v) {
			cl_obj_vec = Pbase(t)->b_name;
			return 0;
		}
		else
			return Pbase(t)->b_name;

	case VEC:
		t = Pvec(t)->typ;
		v=1;
		goto xx;

	case EOBJ:
		eobj = Pbase(t)->b_name;
	default:	
		return 0;
	}
}





/* //SYM
 * parsing table lookup
 */
static int
k_has_base( Pclass c1, Pclass c2, TOK& t )
/* true if c2 is an ancestor of c1
 * t indicates whether derivation is virtual
 */
{
	if  ( c1 == 0 || c2 == 0 ) error('i',"k_has_base(%d,%d,...)",c1,c2);
	for ( Pbcl b = c1->baselist;  b;  b = b->next ) {
		TOK v;
		t = b->base;
		if ( same_class(b->bclass,c2) ) return 1;
		if ( k_has_base(b->bclass,c2,v) ) {
			if ( v==VIRTUAL ) t = VIRTUAL;
			return 1;
		}
	}
	t = 0;
	return 0;
}

static Pname
k_find_in_base( char* s, Pclass cl, Pclass &acl, TOK k )
/* find s in class cl or one of its ancestors
 * (for consistency, may convert to using stuff in find.c...)
 */
{
DB(if(Kdebug>=3)error('d',"k_find_in_base( %s, %t, ...,%k)",s,cl,k););
	// look in base classes; check for dominance
	Pname n = 0;
	Pname n2 = 0;
	Pclass ncl = 0, n2cl = 0; // init to avoid warnings
	for (Pbcl b=cl->baselist; b; b=b->next) {
		if ( strcmp(s,b->bclass->string) == 0 ) {
		    n = b->bclass->k_tbl->find_cn(s);
		    ncl = b->bclass;
		}
		else if ( (n = b->bclass->k_tbl->look(s,0)) != 0 )
		    ncl = b->bclass;
		else
		    n = k_find_in_base( s, b->bclass, ncl, k );
		if ( n == 0 ) continue;
		if ( n2 ) {
		    // multiple defs from base classes
		    // Note: error diagnostics for ambiguous
		    //        refs are printed by find.c
		    // To print them here would require knowledge of
		    // type/storage class for each name, in addition
		    // to virtual derivation.
		    // This code is only used to select the right name
		    // for parsing purposes and should match the name
		    // selection in find.c.
		    //XXXXX find.c should also be extended to handle member
		    //XXXXX types
		    TOK t;
		    if ( !same_class(ncl,n2cl) ) {
			// check for dominance
			//if ( k_has_base(ncl,n2cl,t) ) {
			//    ;// n dominates n2; save n
			//} else
			if ( k_has_base(n2cl,ncl,t) ) {
			    // n2 dominates n; discard n
			    n = n2;
			    ncl = n2cl;
			}
		    }
		}
		n2 = n;
		n2cl = ncl;
	}
	if ( n ) {
		acl = ncl;
		return n;
	}
	acl = n2cl;
	return n2;
}

Pname
k_find_name(char* s, Pktab tbl, TOK k )
/* parse table lookup
 */
{
recurse:
DB(if(Kdebug>=3)error('d',"k_find_name( %s, %s,%k)",s,tbl->whereami(),k););
	if ( tbl == 0 ) return 0;
	Pname n;
	if ( tbl->k_id == CLASS ) {
		if ( tbl->k_name == 0 ) error('i',"C table with noN");
		if ( strcmp(s,tbl->k_name->string) == 0 )
			n = tbl->find_cn(s);
		else {
			Pclass cl = tbl->k_name->tp->classtype();
			n = cl->k_tbl->look(s,0);
		}
	} else
		n = tbl->look( s, 0 );
	if ( n && k == HIDDEN && n->base != TNAME ) n = n->n_hidden;
DB(if(Kdebug>=3) {
if(n) error('d',"found%n base%k tp%t in %s",n,n->base,n->tp,n->n_ktable->whereami());
else error('d',"%s not found in %s",s,tbl->whereami());
});
	if ( n ) return n;

	// look in enclosing scope
	// optimization: table::look already looked through containing
	//    blocks and global or first enclosing class scope
	while ( tbl && tbl->k_id == BLOCK ) tbl = tbl->k_next;
	if ( tbl == Gtbl ) return 0;
	if ( tbl->k_id == CLASS ) {
		if ( tbl->k_name == 0 ) error('i',"C table with noN");
		Pclass cl = tbl->k_name->tp->classtype(), acl;
		n = k_find_in_base(s,cl,acl,k);
	} else // template/arg ???
		n = tbl->look(s,0);
	if ( n && k == HIDDEN && n->base != TNAME ) n = n->n_hidden;
	if ( n ) return n;

	// still failed -- look in next enclosing scope
	tbl = tbl->k_next;
	goto recurse; //return tbl ? k_find_name(s,tbl,k) : 0;
}
Pname
k_find_member(char* s, Pclass cl, TOK k )
/* parse table lookup
 */
{
DB(if(Kdebug>=3)error('d',"k_find_member( %s, %s,%k)",s,cl->string,k););
	Pktab tbl = cl->k_tbl;
	Pname n;
	if ( tbl->k_id != CLASS ) error('i',"C tableX for%t!",cl);
	if ( strcmp(s,cl->string) == 0 )
		n = tbl->find_cn(s);
	else
		n = tbl->look(s,0);
	if ( n && k == HIDDEN && n->base != TNAME ) n = n->n_hidden;
DB(if(Kdebug>=3) {
if(n) error('d',"found%n base%k tp%t in %s",n,n->base,n->tp,n->n_ktable->whereami());
else error('d',"%s not found in %s",s,tbl->whereami());
});
	if ( n ) return n;
	// look in base classes
	Pclass acl;
	n = k_find_in_base(s,cl,acl,k);
	if ( n && k == HIDDEN && n->base != TNAME ) n = n->n_hidden;
DB(if(Kdebug>=3) {
if(n) error('d',"found%n base%k tp%t in %s",n,n->base,n->tp,n->n_ktable->whereami());
else error('d',"%s not found in bases of %s",s,tbl->whereami());
});
	return n;
}



//SYM -- table insertion routines...
Pname
insert_name( Pname nn, Pktab tbl )
/* insert non type name into parsing table
 */
{
DB(if(Kdebug>=2)error('d',"insert_name( nn%n, tbl(%d) %s )",nn,tbl,tbl->whereami()););
	Pname nx = k_find_name(nn->string,tbl,0);
	if ( nx && nx->n_ktable == tbl ) {
	    if ( nx->base == TNAME ) {
DB(if(Kdebug>=2)error('d'," -- hiding%n tp%t",nx,nx->tp););
		if ( nx->tp
		&&   nx->tp->base != COBJ
		&&   nx->tp->base != EOBJ ) {
		    DB(if(Kdebug>=2)error('d',"hidingTdef%n",nx));
		}
		nx->n_key = HIDDEN;
		Pname ny = tbl->insert(nn,0);
		if ( Nold ) error("hiddenTN%n inserted twice",nx);
		else ny->n_hidden = nx;
		nx = ny;
	    } else { // non-type name nx already exists in tbl
DB(if(Kdebug>=2)error('d'," -- nonTN%n already exists",nx););
		; // error handled later
	    }
	} else { // nn not in current table
	    Pname ny = tbl->insert(nn,0);
	    if ( nx && nx->base != TNAME )
		nx = nx->n_hidden;
	    ny->n_hidden = nx;
	    nx = ny;
	}
	return nx;
}
Pname
insert_type( Pname nn, Pktab tbl, TOK tcsue )
/* insert new type name into tbl
 * if another type name tn already exists, error
 * if a non type name already exists, enter class/enum as hidden
 */
{
	// type names defined in arg lists are entered in surrounding scope
	// friends are entered in scope enclosing class
DB(if(Kdebug>=2)error('d',"insert_type( nn%n, tbl(%d) %s, tcsue%k )",nn,tbl,tbl->whereami(),tcsue););
	if ( tbl->k_id == ARG ) {
		// types defined in arg lists are entered in surrounding scope
		do tbl = tbl->k_next; while ( tbl->k_id == ARG );
		if ( tbl->k_id == TEMPLATE ) tbl = tbl->k_next;
	}
	if ( tbl->k_id == CLASS ) {
	    if ( ccl == 0 ) error('i',&nn->where,"insert_type(%n) -- no ccl for class table!",nn);
	}
DB(if(Kdebug>=2)error('d'," -- real tbl %d %s",tbl,tbl->whereami()););

	Pname nx = k_find_name( nn->string, tbl, 0 );
	if ( nx && nx->n_ktable == tbl ) {
	    Nold = 0;
	    if ( nx->base == TNAME ) {
		Nold = 1;
DB(if(Kdebug>=2)error('d',"TN%n redefined in same scope",nx););
	    } else { // nx is a non-type name in current table -- hide new type
		if ( tcsue == TYPE ) {
			;
DB(if(Kdebug>=2)error('d',"%n redefined asTdef",nx););
		}
		Pname ny = tbl->insert(nn,HIDDEN);
		if ( Nold ) { // previous hidden tname
		    ;
DB(if(Kdebug>=2)error('d',"hiddenTN%n redefined in same scope",nx););
		} else { ny->n_hidden = nx->n_hidden; nx->n_hidden = ny;
DB(if(Kdebug>=2)error('d',"new, hiddenTN%n",nx););
		}
		nx = ny;
	    }
	} else { // nn not in current table
	    Pname ny = tbl->insert(nn,0);
	    ny->n_hidden = nx;
	    nx = ny;
DB(if(Kdebug>=2)error('d',"new, unhiddenTN%n",nx););
	}
	nx->base = TNAME;
	return nx;
}

char*
ktable::whereami()
{
	if ( this == 0 ) return "NULL TABLE!!!";
	else if ( k_id==ARG ) return "arg scope";
	else if ( this==Gtbl ) return "global scope";
	else if ( k_id==BLOCK ) return "block scope";
	else if ( k_id==TEMPLATE ) return "template scope";
	else if ( k_name ) return k_name->string;
	else return "??? scope";
}
