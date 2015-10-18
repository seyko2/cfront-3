/*ident	"@(#)cls4:src/lalex.c	1.21" */
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

lalex.c:

	lookahead 

*****************************************************************************/
#include <stdio.h>

#include "cfront.h"
#include "yystype.h"
#include "tqueue.h"
#include "template.h"

#ifdef DBG
#define LDB(val,a) { if(Ldebug>=val) {a;} }
#else
#define LDB(val,a) /**/
#endif

// static data members definition
int templ_compilation::parameters_in_progress=0;
toknode* toknode::free_toks=0;

#ifdef DBG
extern "C" char*
image( int t )
{
	if(keys[t]) return keys[t];
	else { static char b[20];
		sprintf(b,"token(%d)",t);
		return b;
	}
}
extern "C" void
printok2( TOK tk, YYSTYPE rv, loc wh )
{
	switch(tk) {
	default:
		fprintf(stderr,"\t%s",image(tk));
		break;
	case ID: case ICON: case CCON: case FCON: case STRING:
		fprintf(stderr,"\tID '%s'",rv.s);
		break;
	case TNAME:
		fprintf(stderr,"\tTNAME '%s'",rv.pn->string);
		break;
	case NAME:
		fprintf(stderr,"\tID '%s'",rv.pn->string);
		break;
	case PTNAME:
		fprintf(stderr,"\tPTNAME '%s'",rv.pn->string);
		break;
	case AGGR:
		fprintf(stderr,"\tAGGR '%s'",image(rv.t));
		break;
	case TYPE:
		fprintf(stderr,"\tTYPE '%s'",image(rv.t));
		break;
	case TSCOPE:
		fprintf(stderr,"\tTSCOPE '%s'::",rv.pn->string);
		break;
	case MEMPTR:
		fprintf(stderr,"\tMEMPTR '%s'::*",rv.pn->string);
		break;
	}
	putc(' ',stderr);
	wh.put(stderr);
	putc('\n',stderr);
	fflush(stderr);
}
extern "C" void
printok( toknode* t, char* id = 0 )
{
	if ( id ) fprintf(stderr,"%s:",id);
	if ( t == 0 ) {
		fprintf(stderr,"	NULL TOKNODE!\n");
		fflush(stderr);
	} else
		printok2( t->tok, t->retval, t->place );
}
extern "C" void
showQ( char* where )
/*
	display token Q
*/
{
	fprintf(stderr,"TOKEN Q (%s):\n",where);
	for (register toknode* t = front; t; t = t->next) printok(t);
	putc('\n',stderr);
	fflush(stderr);
}
#endif

int bl_level;

static int  laexpr( TOK );
static int  latype( TOK );
static int  la_decl( int );
static TOK  lookahead();

/* make this a toknode! */
static int lasttk = 0;		// one token history
static YYSTYPE lastval; 	// yylval lasttk value 

static int must_be_expr = 0;	// handle redundant parentheses
int must_be_id = 0;		// !0, TNAME => ID, i.e., int X

loc curloc;
int curr_file;

static toknode* latok;		// current lookahead token
toknode* front = 0;
static toknode* rear  = 0;

const int TQCHUNK = 16;

void*
toknode::operator new(size_t)
{
	register toknode* p;

	if ((p=free_toks) == 0) {
		register toknode* q;
		free_toks = q = (toknode*)new char[TQCHUNK * sizeof(toknode)];
		p = free_toks;
		for (; q != &p[TQCHUNK-1]; q->next = q+1, ++q);
		q->next = 0;
	}
	free_toks = p->next;
	return p;
}

toknode::toknode(TOK t, YYSTYPE r, loc tloc)
{
	tok = t;
	used = 0;
	retval = r;
	place = tloc;
	next = last = 0;
}

void
toknode::operator delete(void* vp,size_t)
{
	register toknode* p = (toknode*)vp;
	p->next = free_toks;
	free_toks = p;
	vp = 0;
}

static void
add_tokens()
/*
    extend lookahead token queue when depleted
*/
{
    TOK tk = tlex();
    if ( tk != ID )
	return;

    while (tk == ID || tk == MEM || tk == DOT )  
	tk = tlex(); 
}

#define USE_TOKEN(T,W) \
	LDB(2,error('d',&(T)->place,"use_token('%k','%s')",(T)->tok,W);); \
	if ( !(T)->used ) use_token(T);

//SYM -- removed Ptype return_nstd_local_type(Pname,TOK&)

//SYM -- removed Pname local_nested_kludge( Pname n, Pname tn )

enum { one_back, two_back };
	
static TOK last_tokens[2];  // TSCOPE not reduced at this point
static Pname last_tname;    // tname :: id, where id is nested class
static void
use_token( toknode* T )
/*
	lookup TNAMEs here instead of in tlex()
	maintain block level
*/
{
	T->used = 1;
	static bit aggr=0;
	if (T->tok == AGGR || T->tok == ENUM) 
		aggr=1;
	else if ((T->tok != MEM) && (T->tok != ID)) 
		aggr=0;

	DB(if(Ldebug>=1) {
		error('d',&T->place,"\n*** use_token(%k )",T->tok);
		printok(T);
		error('D',&T->place,"    lasttk%k last_tname%n last tokens%k%k",lasttk,last_tname,last_tokens[one_back],last_tokens[two_back]);
	});

	switch ( T->tok ) {
	case REF: case DOT:
	{	toknode* t = T;
		Pname q = 0, r = 0;
		for(;;) {
			if ( t->next == 0 ) add_tokens();
			t = t->next;
			if ( t->tok == ID && t->next->tok == MEM ) {
				Pname n = new name( t->retval.s );
				n->base = MEMQ;
				if ( q == 0 ) q = r = n;
				else { r->n_list = n; r = n; }
				t = t->next;
			} else if ( t->tok == MEM ) {
				Pname n = new name();
				n->base = MEMQ;
				if ( q == 0 ) q = r = n;
				else { r->n_list = n; r = n; }
			} else
				break;
		}
		if ( q ) {
			toknode *x = T->next, *xx = x->next;
			x->tok = MEMQ;
			x->retval.pn = q;
			x->used = 1;
			x->next = t;
			t->last->next = 0;
			t->last = x;
			for ( ;  xx;  xx = x ) { x = xx->next; delete xx; }
		}
		break;
	}
	case ID:
		if ( last_tokens[one_back] == MEMQ ) break;
		{
		Pname n = 0;
		TOK sc = T->next&&T->next->tok==MEM || aggr ? HIDDEN : 0;
// error('d', &T->place, "use_token: %s", T->retval.s );
		// look up in correct table
		if ( last_tokens[one_back]==MEM || (last_tokens[one_back]==TSCOPE && !templp->in_progress) ) {
		    if ( last_tokens[two_back]==TNAME || (last_tokens[two_back]==GT && last_tokens[one_back]!=MEM) ) {
			if ( last_tokens[two_back] == GT && last_tokens[one_back] == TSCOPE )
			{
				extern YYSTYPE yyval;
				last_tname = yyval.pn;
			}
			// TNAME :: ID
			Pname tn = last_tname;
			if ( tn == 0 ) error('i',&T->place,"last_tname not set for tname::%s",T->retval.s);
			while (tn->tp && tn->tp->base==TYPE)
				tn=Pbase(tn->tp)->b_name;
			if ( strcmp(T->retval.s,tn->string)==0 ) {
			    // X::X or X::~X -- leave as TNAME here
			    n = tn;
			} else if ( tn->tp && tn->tp->base == COBJ ) {
			    Pclass cl = Pclass( Pbase(tn->tp)->b_name->tp );
			    // X::X or X::~X -- leave as TNAME here
			    if (
				cl->is_templ_instance()
				&&
				strcmp(T->retval.s,Ptclass(cl)->unparametrized_tname()->string)==0
			    ) {
				n = tn;
			    }
                            else
			    if ( (cl->defined & (DEF_SEEN|DEFINED)) == 0 )
				error(&T->place,"%n:: %s -- %tU",last_tname,T->retval.s,cl);
			    else {
				n = k_find_member(T->retval.s, cl, sc);
				if (n && n->n_ktable == Gtbl)
					n = 0;
			    }
			} else {
  			    if (tn->tp->base != ANY) // don't flag Template formal
  			    	error(&T->place,"%n:: %s --%n not aCN",tn,T->retval.s,tn);
			    n = k_find_name(T->retval.s,Ctbl,sc);
			}
		    } else if ( last_tokens[two_back] != ID ) {
			// :: ID
			n = k_find_name(T->retval.s,Gtbl,sc);
		    }
		} else // look in current scope
		    n = k_find_name(T->retval.s,Ctbl,sc);
		T->idname = n;
		if ( n && n->base == TNAME ) {
		    T->tok = TNAME;
		    T->retval.pn = n;
		    DB(if(Ldebug>=1)error('d',&T->place,"use_token: tname%n",T->retval.pn););
		}
		//SYM -- remove	nn = local_nested_kludge(nn,ntd==NESTED?n:0);
#ifdef DBG
		else if(Ldebug>=1)
			error('d',&T->place,"use_token: id %s",T->retval.s);
#endif
		break;
		}
	case LC: ++bl_level; break;
	case RC: --bl_level; break;
	}
	
	if (T->tok != COMPL || last_tokens[one_back] != MEM) {
		last_tokens[two_back] = last_tokens[one_back];
 		last_tokens[one_back] = T->tok;	
		if (T->tok == TNAME) last_tname = T->retval.pn;
	}
}

static void
la_reset( toknode* T, Pname qual )
{
	// just failed a lookahead for fptr declarator after qualified TNAME
	//	TNAME::TNAME ( ... )
	// probably a member ftn declaration
	// "unuse" tokens after T so names will be searched in correct scope
	// T should == '('
	last_tokens[one_back] = MEM;
	last_tokens[two_back] = TNAME;
	last_tname = qual;
	while ( T && T->used ) {
		T->used = 0;
		switch ( T->tok ) {
		case TNAME:
			T->tok = ID;
			T->retval.s = T->retval.pn->string;
			break;
		case LC: --bl_level; break; // backtrack
		case RC: ++bl_level; break; // backtrack
		}
		T = T->next;
	}
}

void
addtok(TOK t, YYSTYPE r, loc tloc)
{
	toknode* T = new toknode(t,r,tloc);
	if (front == 0)
		front = rear = T;
	else {
		rear->next = T;
		T->last = rear;
		rear = T;
	}
//error('d',&tloc,"addtok: %k '%s'",t,t==ID?r.s:"");
//showQ("addtok");
}

static Pname idname;
extern TOK
deltok( int noset = 0 )
{
	register toknode* T = front;
	USE_TOKEN(T,"deltok");
	register TOK tk = T->tok;
	if ( !noset ) { yylval = T->retval; curloc = T->place; }
	curr_file = curloc.file;
	if (front = front->next)
		front->last = 0;
	else
		latok = rear = 0;
	idname = T->idname;
	delete T;
	return tk;
}

static void
del_tokens( toknode* marker )
/*
    delete tokens from marker to latok, not inclusive
*/
{
    if ( marker == 0 || marker == latok || marker->next == 0 )
	error('i', "bad token queue");

    LDB(2,fprintf(stderr,"del_tokens: %s..%s\n",image(marker->tok),image(latok->tok)));

    register toknode* tt = marker->next;
    if ( tt == latok ) return;
    marker->next = latok;
    latok->last->next = 0;
    latok->last = marker;
    register toknode* tx = tt;
    do {
	LDB(3,fprintf(stderr,"	deleting %s\n",image(tt->tok)));
	tx = tx->next;
	delete tt;
	tt = tx;
    } while ( tx );
}

static void
del_1( toknode* t )
// delete t from the queue
{
	if ( t->last ) t->last->next = t->next;
	else front = t->next;
	if ( latok == t ) latok = t->last ? t->last : front;
	if ( t->next ) t->next->last = t->last;
	else rear = t->last;
	delete t;
}

extern TOK
la_look()
/*
	peek at head of token queue
*/
{
    LDB(1,fprintf(stderr,"\n*** la_look()\n"));
    if ( front == 0 )
	add_tokens();

    latok = front;
    USE_TOKEN(latok,"la_look");
    LDB(1,fprintf(stderr," -- %s\n",image(latok->tok)));
    return latok->tok;
}

#define NEXTTOK() ( (yychar==-1) ? (yychar=lalex(),yychar) : yychar )
void
check_decl()
/*
	Lookahead to direct parsing of local/arg type declarations
	la_decl() returns 1 if lookahead sees a declaration.
*/
{
	TOK tk2;
	switch( NEXTTOK() ) {
	default: break;
	case TSCOPE: //XXX
DB(if(Ldebug>=1)error('d',"check_decl() tscope%n...",yylval.pn););
		tk2 = la_look();
		while ( tk2 == TSCOPE ) tk2 = lookahead();
		if ( tk2 == TNAME ) {
		    toknode* t = latok;
		    if(t->tok!=TNAME)
                   	error('i',&t->place,"check_decl() token scan");
		    tk2 = lookahead();
		    if ( tk2 == LP && la_decl(in_arg_list) ) {
			t->tok = DECL_MARKER; //TNAME
		    }
		}
DB(if(Ldebug>=1)error('d',"%k",DECL_MARKER););
		break;
	case DECL_MARKER:
		if ( in_arg_list == 0 ) break;
		// gag! -- re-scan for declaration in case previous call
		//         to la_decl() came before reduction of arg_lp
		//         (occurs if la_decl() were called from lalex())
		yychar = DECL_TYPE;
		DECL_TYPE = 0;
		if ( yychar != TYPE && yychar != TNAME ) error('i',"check_decl() failed rescanning arg decl; yychar==%d",yychar);
		// no break;
	case TYPE: case TNAME:
DB(if(Ldebug>=1)error('d',"check_decl() %s",yychar==TYPE?"TYPE":"TNAME"););
	    if ( la_look() == LP && la_decl(in_arg_list) ) {
		must_be_id = 0;
		DECL_TYPE=yychar;
		yychar = DECL_MARKER;
DB(if(Ldebug>=1)error('d',"%k",DECL_MARKER););
	    }
	}
}

void
check_cast()
/*
	Lookahead to direct parsing of cast
	la_cast() returns 1 if lookahead sees an ambiguous old-style C cast.
*/
{
	TOK tk2;
	switch( NEXTTOK() ) {
	case TSCOPE: //XXX
		tk2 = la_look();
		while ( tk2 == TSCOPE ) tk2 = lookahead();
		if ( tk2 == TNAME ) {
		    toknode* t = latok;
		    if(t->tok!=TNAME)
                         error('i',&t->place,"check_cast() token scan");
		    tk2 = lookahead();
		    if ( tk2 == LP && la_decl(in_arg_list) ) {
			t->tok = DECL_MARKER;//TNAME
		    }
		}
		break;
	case TYPE: case TNAME:
	    if ( la_look() == LP && la_cast() ) {
		must_be_id = 0;
		DECL_TYPE = yychar;
		yychar = DECL_MARKER;
	    }
	}
}


static int
latype( TOK t )
{
	switch ( t ) {
	default: // includes friend, typedef, storage classes, etc.
		return 0;
	case CHAR: case SHORT: case INT: case LONG:
	case FLOAT: case DOUBLE:
        case UNSIGNED:
		return 1;
	}
}

static int
laexpr( TOK t )
{
	switch ( t ) {
	default: 
		return 0;
	case RETURN: case NEW: case AND: case ANDAND: case OR: case OROR:
	case SIZEOF: case NOT: case COMPL: case MUL: case PLUS: case MINUS: 
	case ER: case ASSIGN: case ASOP: case RELOP: case EQUOP: case DIVOP: 
	case SHIFTOP: case ICOP:
		return 1;
	}
}

static toknode *
get_next_token(toknode *t)
{
  if (! t->next)
    add_tokens() ;
  
  return t->next ;
}


static int
template_tscope(Pname tn, toknode *lt) 
/* provide the looakhead for determining TSCOPE tokens when the name is a
 * parametrized type name; the lookahead here is non-trivial, because it
 * involves stepping over the template arguments. 
 */
{
  int nest = 0 ; // the LT has been fetched

  if (lt->tok != LT) error ('i', "a `<' token was expected") ;  

  // assume the worst, ensure that name strings are consed in the heap
  templp->parameters_in_progress++ ;
  
  for (toknode *t = lt;; t = get_next_token(t))    
    switch (t->tok) {
      
    case LT:
      	++nest;
      	continue;
    case GT:
	// ***************
	// need to fold in awareness of x::y::z
      	if (--nest == 0) {
		t = get_next_token(t);
		if (t->tok==MEM || t->tok==TSCOPE) {
	  	// determine whether it is a memptr
	  		if (t->next == 0) add_tokens();
	  		if (t->next->tok == MUL) {
	    			t->tok = MEMPTR;
	    			del_1(t->next);
	  		} else {
				t->tok = TSCOPE ;
				// handle X<T>::Y ...
	  			if (t->next->tok == ID && templp->in_progress) {
				    Pname cn = tn->tp->is_cl_obj();
				    if ( cn ) {
					int hh = t->next->next->tok==MEM;
	  				Pname tn2 = k_find_member(t->next->retval.s, Pclass(cn->tp), hh );
					if ( tn2 && tn2->base == TNAME ) {
					    t->next->tok = TNAME;
					    t->next->retval.pn = tn2;
					}
	  			    }
	  			}
			}
			t->retval.pn  = tn ;
	  		--templp->parameters_in_progress;
	  		return 1;
		} else {
	  		--templp->parameters_in_progress;	
			return 0;
		}
      }
      continue;

    case SM: case LC: case RC: // a quick exit in case of error
    case EOFTOK:
      	--templp->parameters_in_progress;
      	return 0 ;
      
    default:
      	continue;
    }
}

static TOK
lookahead()
/*
    advance lookahead pointer, lexing at end of Q
    handle occurrences of TNAME and TSCOPE
    (should be kept up to date with lalex())
*/
{
    TOK tk;
    TOK tk2;
    TOK prev_tk = 0;
    YYSTYPE lastval;

    if ( latok == rear ) {
	add_tokens();
	if ( latok )
	    latok = latok->next;
	else
	    latok = front;
    }
    else
	latok = latok->next;

    if ( latok->last ) {
	prev_tk = latok->last->tok;
	lastval = latok->last->retval;
    }

nexttok:
    USE_TOKEN(latok,"lookahead1");
    tk = latok->tok;
    if ( tk == ID || tk == TNAME ) 
    {
	if (latok->next == 0) add_tokens();
	USE_TOKEN(latok->next,"lookahead2");
	/* TOK */ tk2 = latok->next->tok;
	if ( tk == TNAME ) {
	   if (tk2 == LT) {
	      // a parametrized type name -- differentiate from TNAME 
	      // so that it can be dealt with in the grammar. 
	      if (template_tscope(latok->retval.pn, latok->next)) tk = PTNAME;
	   }
           else  
	   if ( tk2 == MEM || tk2 == DOT ) {
tscope:	
		tk = TSCOPE;
// error('d',"lookahead: tk: %k tk2: %k", tk, tk2 );
// XXX -- should be modified to loop and do lookup
		latok = latok->next;
		if (latok->next == 0) add_tokens();
		USE_TOKEN(latok->next,"lookahead3");
		tk2 = latok->next->tok;
		if ( tk2 == MUL ) {
			tk = MEMPTR;
			latok = latok->next;
		}
	    }
	    else if (( prev_tk == MUL && tk2 != RP )
			|| prev_tk == AND )
		{
		tk = ID;
		latok->retval.pn->hide();
		latok->tok = ID;
		latok->retval.s = latok->retval.pn->string;
		}
	}
	else if ( tk2 == MEM ) {
	    	// ID ::
	//XXX	latok = latok->next->next;
	//XXX	goto nexttok;
		goto tscope; // treat as tscope
	}

	if ( tk == ID &&
   		( tk2 == ID ||
		( prev_tk == ID && ( tk2 == COLON || tk2 == LC )))) {
	    		// ID ID
	    		latok = latok->next;
	    		goto nexttok;
			}
    }
//??? check_for_nested()
    return tk;
}

extern int in_sizeof;
extern int in_friend;
extern int in_new;

static int
type_is_redefined( Pname n )
{
	Pktab tb = Ctbl;
	while ( tb->k_id == ARG ) tb = tb->k_next;
	return n->n_ktable == tb;
}

static Pname
do_nl_type( Pname n, int lex_level, TOK tecsu )
/*
 * replaces do_local_class() and do_nested_type()
 * define a new type
 * context is either "AGGR ID" or "ENUM ID" at local or nested scope
 * NOTE: typedefs now processed in name::tdef()
 */
{
    Pname nn = n;
    if ( ccl && in_mem_fct==0 && strcmp(ccl->string, n->string)==0) { // class x { typedef T x;
	error( "nested%k%n redefines immediately enclosing class",tecsu==TYPE?TPDEF:tecsu,n);
	error( 'i', "cannot recover from previous errors" );
    }
    switch ( tecsu ) {
    case CLASS: case STRUCT: case UNION: case ENUM:
	// check for redef at local scope...
	if ( n->base == TNAME // previous def exists
	&&   n->lex_level == lex_level // same block level
	&&   type_is_redefined(n)
	) {
	    if ( n->tp==0 || ( n->tp->base!=COBJ && n->tp->base!=EOBJ )) {
		//error("two definitions of%n",n);
		//error('i',"cannot recover from earlier errors");
		// typedef T ... class T{}, etc.
		// error caught later
		return n;
	    }
	    // catch some redefs here to avoid internal errors later
	    if ( n->tp->base == EOBJ && tecsu != ENUM 
	    ||   n->tp->base == COBJ && tecsu == ENUM ) {
		error("%n defined asC and enum",n);
		error('i',"cannot recover");
	    }
	    // class C{}; ... class C{};
	    // enum E{}; ... enum E{};
	    // etc. -- also an error,but requires name to be placed on
	    //    local_class so error can be detected later during dcl phase
	    if ( n->tp->base == COBJ 
	    &&  (n->tp->classtype()->defined & (DEFINED|DEF_SEEN)) == 0 )
		// class X; class  X{}; ...
		return n;
	}
	nn = new name(n->string);
	nn->lex_level = lex_level;
	nn = nn->tname(tecsu);
	if ( nn->n_ktable == 0 ) error('i',"TN insert failed for%n",nn);
	// if local, put on list for use in del.c
	if ( tecsu!=ENUM && nn->n_ktable->k_id==BLOCK )
		local_class = new name_list( nn, local_class );
	break;
    default:
	error('i',&n->where,"bad tecsu%k in do_nl_type()",tecsu);
    }
    return nn;
}

//SYM -- removed int is_empty( Pclass cl, bit const_chk )
  /* 	for nested class check, empty means *no* members
   *    for const object check, means no *data* members
   */
//SYM -- Since size isn't calculated until dcl() and since
//SYM    local/nested classes aren't type checked until the
//SYM    end of a function/class, is_empty should not be used by the parser.

//SYM -- removed int is_empty( Penum en )

//SYM -- Pname check_nested_type(Pname) removed

//SYM -- removed int in_local_class( Pclass cl )

//SYM ??? static Pname dtor_seen;
//SYM ??? static int in_expr;

extern TOK
lalex()
/*  return next token to grammar  */
{
    register TOK tk;
    if ( front == 0 )
	add_tokens();		// extend lookahead queue
    LDB(1,fprintf(stderr,"\n*** lalex()\n");showQ("before"));

gettok:
    tk = deltok();
    Pname n = idname;
// error('d',&curloc,"lalex: just got %k '%s' in_typedef: %d ccl: %t",tk,tk==ID||tk==TNAME?n->string:"", in_typedef,ccl);

    if ( tk == ID || tk == TNAME ) 
    {
	TOK tk2 = la_look();
	int lex_level = bl_level - in_class_decl - (tk2 == LC);

	if ( tk == TNAME )
	{
//error('d', "lalex tname %n;   lasttk: %k tk2: %k", yylval.pn, lasttk, tk2);
//error('d', "    must_be_id: %d must_be_expr %d  decl_type %d",must_be_id,must_be_expr,DECL_TYPE);
//error('d', "    bl_level: %d parsing_members %d",bl_level,parsing_class_members);
	    if ( tk2 == LP && lasttk != TSCOPE && lasttk != MEM
		&& (lasttk==TYPE || bl_level == 0 || parsing_class_members)
		&& ( laexpr(lasttk) == 0 ) 
		&& must_be_expr == 0
		&& DECL_TYPE == 0 ) {
			if (la_decl(in_arg_list)) {
				must_be_id = 0;
				DECL_TYPE = tk;
				tk = DECL_MARKER;
				goto ret;
			}
	    }

	    //SYM -- change
	    if ( lasttk == AGGR || lasttk == ENUM ) {
		if ( tk2 == LC || tk2 == COLON
		||  (tk2==SM && !in_new && !in_friend) ) {
			// tag def
			//XXXXX currently enter all unqualified
			//XXXXX    friends in Gtbl
			//XXXXX Should eventually enter in enclosing
			//XXXXX    scope, however back end currently
			//XXXXX    doesn't support this, due to lack
			//XXXXX    of context info.
			//XXXXX Commented code below will enter name
			//XXXXX    correctly when back end is brought
			//XXXXX    up to date.
			//Pktab otbl = Ctbl;
			//Pclass occl = ccl;
			if ( in_friend ) {
			    if ( Ctbl->k_id != CLASS )
				error("friend %s not inC",yylval.pn->string);
			//    else {
			//	--in_class_decl;
			//	ccl = ccl->in_class;
			//    }
			//    if ( Ctbl->k_id == CLASS || Ctbl->k_id == TEMPLATE ) {
			//	Ctbl = Ctbl->k_next;
			//	if (Ctbl->k_id == TEMPLATE) Ctbl = Ctbl->k_next;
			//    }
			}
			else // remove this line when friend name entry is fixed
			if ( tk2 != SM || type_is_redefined(yylval.pn)==0 ) {
			    if ( lex_level 
			    &&  (in_class_decl==0 || in_mem_fct) )
				yylval.pn = do_nl_type( yylval.pn, lex_level, lastval.t );
			    else if ( in_class_decl && ccl )
				yylval.pn = do_nl_type( yylval.pn, lex_level, lastval.t );
			}
			//Ctbl = otbl;
			//if ( ccl != occl ) {
			//	ccl = occl;
			//	++in_class_decl;
			//}
		}
	    }

           if (tk2 == LT && template_tscope(yylval.pn,latok)) {
		tk = PTNAME;	// a parameterized type name
	   } else  
	   if ( tk2==MEM || (tk2==DOT && lasttk!=REF && lasttk!=DOT && lasttk !=REFMUL )) {  
		if (tk2==DOT)
			error("``.'' used for qualification; please use ``::''");

		if (yylval.pn->tp->base == COBJ) { 
			Pclass cl = yylval.pn->tp->classtype();
			Pclass cl2 = ccl;
			while (cl2 && cl2->in_class) cl2=cl2->in_class;
			if (cl && cl->class_base && (cl2==0 || cl2->templ_base==CL_TEMPLATE || same_class(cl,cl2)==0))
				error("YC%n must be qualifiedWZL of instantiations",yylval.pn);
		}

		tk = TSCOPE;
		{//XXX -- should be modified to do lookup and del at each ::
		  int n = 0;
		  while ( (tk2 = lookahead()) == TSCOPE ) n += 2;
		  if ( tk2 == TNAME ) {
			Pname cn = latok->retval.pn;
			toknode* t = latok;
			tk2 = lookahead();
			if ( tk2 == LP 
			&& (bl_level == 0 || parsing_class_members) 
			&& ( laexpr(lasttk) == 0 ) 
			&& must_be_expr == 0
			&& DECL_TYPE == 0 ) {
				if (la_decl(in_arg_list)) {
					must_be_id = 0;
//					t->tok = DECL_MARKER;
					DECL_TYPE = TNAME;
//					tk2 = deltok(1); // ::
					n++;	// ::
					n++;	// TNAME
					tk = DECL_MARKER;
					while (n-- > 0)
						deltok();
					goto ret;
				}
				la_reset(t->next,cn);
			}
		  }
		}
		tk2 = deltok(1);
		tk2 = la_look();
		if ( tk2 == MUL ) {
			tk = MEMPTR;
			tk2 = deltok(1);
		}
	    } // if tk2==MEM
	    // Have a TNAME.  Check to be sure.
	    else if ( must_be_id ){
DB(if(Ldebug>=2)error('d',"lalex: must_be_id: <tname %n> <%k>",yylval.pn,tk2););
		if ( in_class_decl 
			&& lasttk == TYPE 
			&& tk2 == LP
			&& strcmp(yylval.pn->string,ccl->string) == 0 )
			error("%nK with returnT", yylval.pn);

		else if ( lasttk == TYPE && lastval.t == OVERLOAD
			  && ( tk2 == SM || tk2 == LP ) )
		        { 
			tk = ID;
			yylval.pn->hide();
			yylval.pn = new name( yylval.pn->string );
			yylval.pn->n_oper = TNAME;
		}
		else if ( lasttk == OPERATOR )
		//SYM -- remove || in_typedef && yylval.pn->n_key == NESTED)
				must_be_id = 0;
		else if ( lasttk != TSCOPE // watch out for X::X
		     || lastval.pn != yylval.pn 
		     || (in_typedef && 
				in_typedef->check( yylval.pn->tp,0) == 0 )) 
		{
DB(if(Ldebug>=2)error('d',"lalex:  -- tname -> id; lasttk%k",lasttk));
			tk = ID;
			if ( in_typedef && (lasttk == MUL || lasttk == REF)) {
     				defer_check = 1;
     				in_tag = yylval.pn;
			}

			if ( lasttk == MEM && yylval.pn->lex_level ) {
				//SYM -- change
				//SYM ??? this code looks suspicious
				Pname nn = k_find_name(yylval.pn->string,Gtbl,0);
				if ( nn==0 || nn->base == NAME )
    					error( "%k%s undeclared", lasttk, yylval.pn->string);
				else
    					yylval.pn = nn;
			}
			else {
				if (lasttk!=DOT && lasttk!=REF
				&&  lasttk!=TSCOPE && lasttk != GOTO ) {
				    // handle typedefs in basetype::check
				    //    when type is available
				    if (!in_typedef || in_arg_list) {
DB(if(Ldebug>=2)error('d',"\"%s\" line %d: hiding%n",__FILE__,__LINE__,yylval.pn));
					yylval.pn->hide();
					yylval.pn = new name(yylval.pn->string);
				    } else if ( yylval.pn->base == TNAME ) {
					yylval.pn = new name(yylval.pn->string);
				    }
				    // else name already copied in do_nl_type()
				    // NOTE: copying name should preserve tpdef
				    yylval.pn->n_oper = TNAME;
				} else {
//error('d',"tname%n -> id lasttk%k tk2%k",yylval.pn,lasttk,tk2);
				    yylval.pn = new name(yylval.pn->string);
				}
DB(if(Ldebug>=2)error('d'," -- %n%k",yylval.pn,yylval.pn->n_oper));
			}
			if ( defer_check ) defer_check = 0;
		}
	    } // must_be_id

DB(if(Ldebug>=2)error('d',"lalex -- end of if tname --%k%n",tk,yylval.pn););
	    // if we still have a TNAME, make sure have the right TNAME
	    // possibility of ananchronistic reference to nested type
	    //SYM -- transitional stuff removed
	    //Ptype nbt = yylval.pn->tp;
	    //if (tk == TNAME && curr_scope == 0 && nbt && // Y y; not X::Y y;
	    //	(nbt->base == EOBJ || nbt->base == COBJ))
	    //{ ... }
	}
	else 
	{ // tk == ID 
    		char *s = yylval.s;
		if ( n ) n = n->n_hidden;
		//SYM Pname nstd = ktbl->look( s, NESTED );

		//SYM -- removed nstd weirdness

		if (tk2 == MEM) {
	            // ID :: -- see use_token()
		    error( "%s:: -- %sis not aTN", s, s ); 
		    tk2 = deltok(1);
	    	    goto gettok;
		} 
		//SYM transitional kludge deleted
		// Have an ID. Check last token to be sure.
		else if (lasttk==ENUM || lasttk==AGGR && (in_arg_list != 2
                           	// template <class id, class id>
                           	|| (tk2 != GT && tk2 != CM)))
		{
			// "class X {}" "class X :" enters tag in current scope 
			// "class X;" (but not "new class X;")
			// enters tag in current scope if it isn't already there
			//   (note: could be defined in enclosing scope)
			// "friend class X;" enters in scope enclosing ccl
			int tagdef = tk2==LC || tk2==COLON || (tk2==SM && !in_new && !in_friend);
			tk = TNAME;
			// new tag, define it
//error("n%n ll %d bl %d tk2%k",n,n?n->lex_level:0,bl_level,tk2);
//error('d',"n%n %s ctbl %s",n,n?n->n_ktable->whereami():"???",Ctbl->whereami());
//error('d',"n%n tagdef %d tk2%k  rdf %d",n,tagdef,tk2,n?type_is_redefined(n):0);
			if ( n==0 // no hidden type name ...
			||   (n->n_template_arg == template_type_formal)
			||   tagdef
			) {
//error('d', "ccl%t ll %d bl %d in_class_decl %d", ccl, ccl?ccl->lex_level:0, bl_level, in_class_decl);
			//XXXXX currently enter all unqualified
			//XXXXX    friends in Gtbl
			//XXXXX Should eventually enter in enclosing
			//XXXXX    scope, however back end currently
			//XXXXX    doesn't support this, due to lack
			//XXXXX    of context info.
			//XXXXX Commented code below will enter name
			//XXXXX    correctly when back end is brought
			//XXXXX    up to date.
			    //Pktab otbl = Ctbl;
			    //Pclass occl = ccl;
			    if ( in_friend && tagdef ) {
				if ( Ctbl->k_id != CLASS )
					error("friend %s not inC",s);
				//else {
				//	--in_class_decl;
				//	ccl = ccl->in_class;
				//}
				//if ( Ctbl->k_id == CLASS || Ctbl->k_id == TEMPLATE ) {
				//	Ctbl = Ctbl->k_next;
				//	if (Ctbl->k_id == TEMPLATE) Ctbl = Ctbl->k_next;
				//}
			    }
			    else // remove this line when friend name entry is fixed
			    if ( n==0 || tk2!=SM || !type_is_redefined(n) ) {
				Pname nn = new name( s );
				if ( !tagdef )  // struct X*, etc.
					nn->lex_level=0; 
				else
					nn->lex_level=lex_level;

				if ( ccl && tagdef && 
					in_class_decl && 
			(bl_level == ccl->lex_level + in_class_decl + (tk2==LC))) {
					if ( n ) { DEL(nn); nn=n; }
					n = do_nl_type(nn,lex_level,lastval.t);//SYM
				}
				else
				if ( nn->lex_level ) {
					int ll = nn->lex_level;
					if ( n ) { DEL(nn); nn=n; }
					n = do_nl_type( nn, ll, lastval.t );
				}
				else {
				    // either global def "class n { ... }"
				    //    or global or non-global ref "class n"
				    //    (class or enum)
				    //SYM XXXXX currently simulates 2.1 behavior
				    //SYM XXXXX by entering undefined tags in
				    //SYM XXXXX Gtbl
				    //NOTE:  By accident, this code also
				    //    correctly enters template class
				    //    defs into global scope instead
				    //    of the current template table.
				    //    This should be tested for explicitly
				    //    if the code changes.
				    if ( n == 0 ) {
					Pktab otbl = Ctbl;//SYM
					Pclass occl = ccl;
					Ctbl = Gtbl;//SYM
					ccl = 0;
					n = nn->tname( lastval.t );
					Ctbl = otbl;//SYM
					ccl = occl;
				    } else DEL(nn);
				    //SYM removed statStat stuff
				}
			    }
			    //if ( Ctbl != otbl ) {
				//Ctbl = otbl;
				//if ( ccl != occl ) {
				    //ccl = occl;
				    //++in_class_decl;
				//}
			    //}
			}
			else {
				if (n->tp->base!=COBJ && n->tp->base!=EOBJ) {
					// call error twice to avoid omitting
					//    the message if there were
					//    previous errors
					error( "hidden%n:%t",n,n->tp );
					error('i',"cannot recover");
				}

				//SYM removed statStat stuff
			}
			yylval.pn = n; //SYM nstd?nstd:n; 
		}
		else {
	    		tk = ID;
			yylval.pn = new name( s );
		}

		if ( tk == ID ) 
		{
		    switch ( tk2 ) {
		    case ID: case TNAME: case AGGR: case ENUM:
		    {
			Pname n = 0; 
			//SYM removed nstd stuff...
			n = k_find_name( s, Ctbl, HIDDEN );
			if ( n ) {
				char* x = (tk2==ID||tk2==TNAME) ? front->retval.s : keys[tk2];
				// if n->n_key != HIDDEN, then lasttk
				//    was probably TSCOPE (C::)
				// Otherwise n would have been found.
				switch ( n->tp->base ) {
				default:
					error("%s%s:Tdef %sis %sin this scope", s, x, s, n->n_key==HIDDEN?"hidden":"undefined" );
					break;
				case COBJ:
					error("%sis %sin this scope: useC %s%s",s,n->n_key==HIDDEN?"hidden":"undefined",s,x);
					break;
				case EOBJ:
					error("%sis %sin this scope: use enum %s%s",s,n->n_key==HIDDEN?"hidden":"undefined",s,x);
					break;
				}
				tk = TNAME;
				yylval.pn = n;
			} // if n
			//SYM -- else nested lookup removed
			else { // probably a typo
			    if ( tk2 == ID ) {
				// x<parameters>::y z, where y is a nested class
				if (lasttk == TSCOPE && lastval.pn->base == TNAME) {
					if (lastval.pn->tp->base == ANY && 
					    lastval.pn->n_template_arg == template_type_formal) 
							error('s',"use of %n::%sW formalYTZ",lastval.pn,s);
					else 
					if (lastval.pn->tp->base == COBJ &&
					    lastval.pn->tp->classtype()->class_base != VANILLA)
						error('s',"Zized nestedC access: %n<...>::%s",lastval.pn,s);
					else error("%s%s: %sis not aTN", s,front->retval.s,s);
					error( 'i', "cannot recover from previous errors" );
				}
				else error("%s%s: %sis not aTN", s,front->retval.s,s);
			    }
			    else if ( tk2 == TNAME )
  				error("%s%s: %sis not aTN", s,front->retval.pn->string,s);
			    else 
  				error("%s%k: %sis not aTN", s,front->retval.t,s); 
				goto gettok;
			}
    			 break;
		    }
		    case DOT: case REF:
    			 break;
		    default:
			if ( lasttk == TNAME && tk2 == LC )  
			{
	    			error("T%s %k: %s is unexpected", s, tk2, s );
	    			goto gettok;
			}
			// have an ID.  lets just make sure it should not be a TNAME
			//SYM deleted nstd weirdness
    			break;
		    } // end: switch tk2
		} // end: if (tk == ID)
	}

// error('d',"testing for in_expr: in_expr: %d tk: %k", in_expr, tk );
// error('d',"testing for in_expr: tk2: %k lasttk: %k", tk2, lasttk );
	    //SYM???if (lex_level && tk==ID && tk2==LP &&
		//SYM???	(lasttk==LC || lasttk==RC || lasttk==RP ||
		//SYM???	lasttk == ASSIGN || lasttk == SM))
		//SYM???		in_expr = 1;
	    //SYM???else in_expr = 0;
    }
    if ( tk == TNAME || ( tk == TYPE && latype(yylval.t) )
	|| tk == REF || tk == DOT || tk == GOTO 
	|| tk == MEMPTR )
	// TNAME cannot immediately follow a type name,
	// scope operator, right curly, selection, or goto
		must_be_id = 1;
    else
	must_be_id = 0;

    switch ( tk ) {
    case SM:
	//SYM???in_expr = 0;
    case RP: case RC: must_be_expr = 0; break;
    case COLON: 
	if  (lasttk == RP || 
	    (lasttk == TYPE && lastval.t == CONST)) 
 	        must_be_expr = 1;
	break;
    case SIZEOF:
	++in_sizeof;
	break;
    case NEW:
	++in_new;
	break;
    }
ret:
    //SYM???if ( tk == COMPL && lasttk == TSCOPE )
	//SYM??? dtor_seen = lastval.pn;
    //SYM???else dtor_seen = 0;

    	lasttk = tk;
    	lastval = yylval;
    	LDB(1,showQ("after");
		fprintf(stderr,"lalex returning ");
		printok2(tk==ID?NAME:tk,yylval,curloc);
    	);
// error('d',"returning tk: %k dtor_seen: %n", tk,dtor_seen );
    	return tk;
}

extern void
la_backup( TOK t, YYSTYPE r )
/*
    called by parser to push token back on front of queue
*/
{
    LDB(1,fprintf(stderr,"\n*** la_backup( '%s', ...)\n",image(t)));
    switch ( t ) {
    case ID: { Pname n = r.pn; r.s = n->string; DEL(n); break; }
    case LC: --bl_level; break;
    case RC: ++bl_level; break;
    }
    register toknode* T = new toknode(t,r,curloc);
    if (front) {
	front->last = T;
	T->next = front;
	T->last = 0;
	front = T;
    } else
	front = rear = T;
    lasttk = 0;
}
static int
la_sctype( TOK t )
{
//error('d',&latok->place,"la_sctype(%k ) -- latok ==%k",t,latok->tok);
	if ( t != latok->tok && t != TSCOPE && t != MEMPTR )
		error( 'i', &latok->place, "la_sctype t%k, latok->tok%k", t, latok->tok );

	switch( latok->retval.t ) {
		case TYPEDEF:
		case EXTERN:
		case STATIC:
		case AUTO:
		case REGISTER:
		case OVERLOAD:
		case INLINE:
		case FRIEND:
		case CONST:
		case VOLATILE:
			return 1; 
		default:
			return 0; 
	}
}

static TOK
ptname_tscope(toknode *lt) 
{
  	int nest = 0; // the LT has been fetched
  	if (lt->tok != LT) error ('i', "ptname_tscope: a `<' token was expected");  

	TOK tk;
  	for (toknode *t = lt;; t = get_next_token(t)) {
// error('d',"ptname_tscope: t: %k t->next: %k",t->tok, t->next?t->next->tok:(add_tokens(),t->next->tok));
    	switch (t->tok) {
    		case LT: ++nest; continue;
    		case GT:
      			if (--nest == 0) {
				t = get_next_token(t);
				if (t->tok == MEM) {
	  				if (t->next == 0) add_tokens();
	  				if (t->next->tok == MUL)  {
						t = t->next;
	    					tk = MEMPTR;
					}
	  				else  tk = TSCOPE;
					latok = t;
					return tk;
				}
				else if (t->tok == MEMPTR || t->tok == TSCOPE) {
					latok = t;
	  				return t->tok;
				} else {
					latok = t;
					return PTNAME;
				}
      			}
      			continue;

    		case SM: case LC: case RC: case EOFTOK:
      			latok = t;
      			return PTNAME;
    		default: continue;
    	}}
}

extern int
la_cast()
/*
	called in reduction of term_lp to check for ambiguous prefix-style cast
	if result is 1, caller inserts DECL_MARKER to force reduction of cast
*/
{
	// yychar already designates TYPE or TNAME
	// LP must start the lookahead queue!
	LDB(1,fprintf(stderr,"\n*** la_cast()\n"););
	int tk, tk2 = latok->tok;

	for ( ; ; ) {
	    tk = tk2;
	    if ( tk == PTNAME ) {
		if (latok->tok==TNAME) latok->tok=PTNAME;
		if (latok->next == 0) add_tokens();
		tk2=ptname_tscope(latok->next);
	    } else
		tk2 = lookahead();

	    switch( tk ) {
	    case LP:
		if( tk2 == MUL || 
		    tk2 == AND ||
		    tk2 == MEMPTR ||
		    tk2 == PTNAME ) 
		    continue;
		else
			// T ( expr )
			return 0;
	    case MUL: case AND: case MEMPTR:
		while ( tk2==TYPE && la_sctype( tk2 ) )
		    // T ( * const ...
		    // T ( * volatile ...
		    tk2 = lookahead();
		continue;
	    case PTNAME:
		// T ( C<...> ...
		if ( tk2 == MEMPTR ) continue;
		return 0;
	    case RP: case LB:
		// T (*) ...
		// T (*[ ...
		return 1;
	    default:
		return 0;
	    } // switch tk
	} // for(;;)
}

// undo SET_SCOPE() on qualified declarator immediately before returning
// RESET_SCOPE() could be called several times, but only the first would
//    have an effect
#define RESET_SCOPE(s) if(s>0) do UNSET_SCOPE(); while(--s);

static int
la_decl( int arg_decl )
/*
	handles ambiguities
		type (*x) ()
		type (*x) []
	at start of arg list / statement
	return val == 1 if lookahead finds a declaration
		(used for error messages only)
	if declaration is "ambiguous" (i.e., can't be recognized with
		1-symbol lookahead), insert DECL_MARKER to force reduction
		of "type"
*/
{
	// LP must start the lookahead queue!
	LDB(1,fprintf(stderr,"\n*** la_decl( %d)\n",arg_decl););
	int tk, tk2 = latok->tok;
	int paren = 0;
	int ptr = 0;
	static int scopesets = 0;

	if ( tk2 != LP ) error('d',&latok->place,"la_decl(): latok ==%k -- '(' expected",tk2);

	for ( ; ; ) {

	    tk = tk2;
	    if ( tk == PTNAME ) {
		if (latok->tok==TNAME) latok->tok=PTNAME;
		if (latok->next == 0) add_tokens();
		tk2=ptname_tscope(latok->next);
	    } else
		tk2 = lookahead();
// fprintf(stderr,"\nla_decl:tk:%d %s tk2: %d %s", tk, keys[tk], tk2, keys[tk2]); fflush(stderr);
	    switch( tk ) {
	    case LP:
		if ( tk2 == RP ) {
		    RESET_SCOPE(scopesets);
		    return 0;
		}
		if ( paren && ptr==0 && arg_decl ) {
		    // redundant parens in arg decl
		    RESET_SCOPE(scopesets);
		    return 0;
		}
		++paren;
		ptr = 0;
		continue;
	    case MUL: case AND:
		ptr = 1;
		if ( tk2==TYPE && la_sctype( tk2 )) {
		    // T ( * const ...
		    // T ( * volatile ...
		    RESET_SCOPE(scopesets);
		    return 1;
		} else {
		    continue;
		}
	    case MEMPTR:
		// T ( C :: * ...
		RESET_SCOPE(scopesets);
		return 1;
	    case TSCOPE:
		if ( (tk2 == ID || tk2 == OPERATOR) && !arg_decl ) {
		    // T ( * C :: id ...
		    if ( latok->last->last->tok == TNAME
		    &&   SET_SCOPE(latok->last->last->retval.pn) )
			++scopesets;
		    continue;
		} else {
		    // error
		    RESET_SCOPE(scopesets);
		    return 0;
		}
	    case PTNAME:
		if ( tk2 == TSCOPE && !arg_decl ) {
		  toknode* t = latok;
		  tk = tk2; tk2 = lookahead();
		  if ( tk2 == ID || tk2 == OPERATOR ) {
		    // T ( * C<int> :: id ...
		    if ( SET_SCOPE(t->retval.pn) )
			++scopesets;
		    continue;
		  } else {
		    // error
		    RESET_SCOPE(scopesets);
		    return 0;
		  }
		} else {
		  RESET_SCOPE(scopesets);
		  return tk2==MEMPTR;
		}
	    }

	    break;
	}

	if ( tk == RP || tk == LB ) {
	    // T (*)()
	    // T (*[])()
	    RESET_SCOPE(scopesets);
	    return 1;
	}

	if ( tk != ID && tk != OPERATOR ) {
	    // T ( exp )
	    RESET_SCOPE(scopesets);
	    return 0;
	}

	if (tk == ID && tk2 == RP && arg_decl && !ptr) {
		TOK nt = lookahead();
		latok = latok->last;
		if (nt == LP) {
			RESET_SCOPE(scopesets);
			return 1;
		}
	}

	if ( ptr == 0 && arg_decl ) {
	    // possible redundant parens in arg decl: T ( id ...
	    RESET_SCOPE(scopesets);
	    return 0;
	}

	if ( tk == OPERATOR ) {
	    switch ( tk2 ) {
	    case PLUS: case MINUS: case MUL: case REFMUL:
            case AND: case OR: case ER: case SHIFTOP: case EQUOP: 
	    case DIVOP: case RELOP: case ANDAND: case OROR: 
	    case NOT: case COMPL: case ICOP: case ASSIGN: 
            case ASOP: case NEW: case GNEW: case DELETE:
		// OPERATOR oper
		tk2 = lookahead();
		break;
	    case LP:
		// OPERATOR ()
		tk2 = lookahead();
		if ( tk2 == RP ) {
		    tk2 = lookahead();
		    break;
		} else {
		    RESET_SCOPE(scopesets);
		    return 0;
		}
	    case LB:
		// OPERATOR []
		tk2 = lookahead();
		if ( tk2 == RB ) {
		    tk2 = lookahead();
		    break;
		} else {
		    RESET_SCOPE(scopesets);
		    return 0;
		}
	    default:	// illegal operator
		RESET_SCOPE(scopesets);
		return 0;
	    }
	} // if OPERATOR

	int allow_lp = 1;
	int allow_rp = 1;
	int pd = paren;
	for ( ; ; ) {
	    tk = tk2;
	    if (tk == LP || tk == LB || tk == RP)
		tk2 = lookahead();
//error('d',&latok->place,"ad: tk%k tk2%k alp %d arp %d",tk,tk2,allow_lp,allow_rp);

// fprintf(stderr,"\nla_decl2:tk:%d %s tk2: %d %s", tk, keys[tk], tk2, keys[tk2]);
	    switch( tk ) {
	    case LP:
		if ( !allow_lp ) {
		    // T ( * id [ exp ] ( ...
		    RESET_SCOPE(scopesets);
		    return 0;
		}

		// Current lookahead will be a decl if
		// the next lookahead is an arg decl
		if ( tk2 == RP && paren ) {
		    tk2 = lookahead();
		    allow_lp = 0;
		    allow_rp = 1;
		    continue;
		}
		if ( tk2 == RP || tk2 == ENUM || tk2==AGGR 
 			|| tk2==ELLIPSIS || tk2==TYPE && la_sctype( tk2 )) {
		    // T ( * id ()
		    // T ( * id ) ()
		    RESET_SCOPE(scopesets);
		    return 1;
		}

		if ( tk2 == TSCOPE ) {
		ts: do { //latok = latok->next; // ::
		         tk2 = lookahead();
		    } while ( tk2 == TSCOPE );
		    if ( tk2 == TNAME ) {
			toknode* T = latok;
			if ( lookahead() == LP && !la_decl(1) ) {
				RESET_SCOPE(scopesets);
				return 0;
			}
			la_reset(T->next,T->retval.pn);
			RESET_SCOPE(scopesets);
			return 1;
		    }
		    RESET_SCOPE(scopesets);
		    return 0;
		}
		if ( tk2 == PTNAME ) {
			if (latok->tok==TNAME) latok->tok=PTNAME;
			if (latok->next == 0) add_tokens();
			tk2=ptname_tscope(latok->next);
			switch ( tk2 ) {
			case TSCOPE:
				goto ts;
			case TNAME:
				break;
			default:
		    		RESET_SCOPE(scopesets);
		    		return 0;
			}
		}
		if ( tk2 == TYPE || tk2 == TNAME ) {
		    // T ( * id ) ( T2 ...
		    if ( lookahead() == LP && !la_decl(1) ) {
			RESET_SCOPE(scopesets);
			return 0;
		    }
		    RESET_SCOPE(scopesets);
		    return 1;
		}

		RESET_SCOPE(scopesets);
		return 0;
	    case LB:
		if ( paren == 0 ) {
		    RESET_SCOPE(scopesets);
		    return 1;
		}
                if ( tk2 == RB ) {
		    // T ( * id [] ...
		    RESET_SCOPE(scopesets);
		    return 1;
		}
		else {
		    // T ( * id [ exp ] ...
		    allow_lp = 0;
		    allow_rp = 1;
		    //XXXXX should balance []!
		    while ( lookahead() != RB && latok->tok!=EOFTOK );
		    tk2 = lookahead();
		    continue;
		}
	    case RP:
// error ('d', "rp: allow_rp: %d paren: %d", allow_rp, paren );
		if ( !allow_rp || !paren ) {
		    // T ( * id ) )
		    RESET_SCOPE(scopesets);
		    return 0;
		}
// permit redundant parentheses
		else 
		if ( tk2 == SM || tk2 == CM || tk2 == ASSIGN ) {
			RESET_SCOPE(scopesets);
			// if at local scope, interpret
			//    T ( id ); as ctor call and
			//    T ( * id ); as declaration
			if (!arg_decl && (pd==1 && !ptr) && !strict_opt)
				error('w', "T(id) (anachronism; will be declaration in future)");
			return !arg_decl && (pd>(strict_opt?0:1) || ptr);
		}
		else
		if ( tk2 == RP && (bl_level-in_class_decl == 0)) {
			RESET_SCOPE(scopesets);
			return !arg_decl;
		} else
		{
		    // T ( * id ) ...
		    --paren;
		    allow_lp = 1;
		    allow_rp = (paren>0);
		    continue;
		}
	    default:
		RESET_SCOPE(scopesets);
		return 0;
	    }
	}
}



/*
**	PROCESSING OF INLINE MEMBER FUNCTIONS
*/
static int la_snarf();

extern toknode*
save_text()
/*
	save text of inline def on q of class
*/
{
	// Q should contain at least the tokens < FDEF, X ... >
	// where X is either LC or COLON (start of ftn)
	LDB(2,fprintf(stderr,"save_text()"));
	LDB(3,fprintf(stderr,"front: %s",image(front->tok)));
	LDB(3,fprintf(stderr,"front->next: %s",image(front->next->tok)));
	latok = front->next;
	if ( la_snarf() ) {
		// append this set of tokens to
		// inline tokenq for class
		toknode* t = front; // FDEF
		if ( ccl->c_funqf == 0 )
			ccl->c_funqf = front;
		else {
			ccl->c_funqr->next = front;
			front->last = ccl->c_funqr;
		}
		ccl->c_funqr = latok;
		front = latok->next;
		latok->next = 0;
		if (front)  front->last = 0;
		return t;
	}
	return 0;
}


extern void
restore_text()
/*
	restore tokens for member inlines onto token q
*/
{
	LDB(2,fprintf(stderr,"restore_text()"));
	if (ccl->c_funqf == 0)	// no inlines on Q
		return;
	LDB(3,fprintf(stderr,"	Q present: %d,%d",ccl->c_funqf,ccl->c_funqr));
	LDB(3,fprintf(stderr,"	front==%s",image(ccl->c_funqf->tok)));
	LDB(3,fprintf(stderr,"	rear ==%s",image(ccl->c_funqr->tok)));
	ccl->c_funqr->next = front;
	if (front)  front->last = ccl->c_funqr;
	front = ccl->c_funqf;
	ccl->c_funqf = ccl->c_funqr = 0;
}


static int
la_snarf()
/*
	scan function def without processing declarations
*/
{
	LDB(2,fprintf(stderr,"la_snarf()"));
	loc *L = &latok->place;
	//DBPLACE(1,L.l,L.f);
	int level;
	int parens = 0;
	int paren_error = 0;
	toknode* marker = latok;
	switch ( latok->tok ) {
	default:
		error('i', L, "bad token Q snarfing function: %d", latok->tok);
	case COLON:
		level = 0;
		break;
	case LC:
		level = 1;
		goto eatf;
	}
	LDB(2,fprintf(stderr,"\"eat\" member initializers"));
	for (;;) {
		if (latok->next == 0) add_tokens();
		switch ( (latok=latok->next)->tok ) {
		case LP:
			++parens;
		default:
			LDB(3,fprintf(stderr,"...%s",image(latok->tok)));
			continue;
		case RP:
			if ( (--parens < 0) && (paren_error++ == 0) )
				error(0,&latok->place,"unbalanced ()");
			continue;
		case LC:
			++level;
			if ( parens <= 0 )
				goto eatf;
			continue;
		case RC:
			if ( --level < 0 ) {
				error(&latok->place,"unexpected '}'");
				goto bad;
			}
			if ( parens <= 0 )
				goto eatf;
			continue;
		case SM:
			if ( parens <= 0 ) {
				error(0, L, "illegal bit field");
				del_tokens( front );
				delete front;
				front = latok;
				front->last = 0;
				return 0;
			}
			continue;
		case EOFTOK:
			error('i',&latok->place,"unexpected end of file");
		} // switch
	} // for

	eatf:
	for (;;) {
		if (latok->next == 0) add_tokens();
		switch ( (latok=latok->next)->tok ) {
		case LC:
			++level;
		default:
			LDB(3,fprintf(stderr,"...%s",image(latok->tok)));
			continue;
		case RC:
			LDB(3,fprintf(stderr,"...RC"));
			if (--level <= 0) {
				if (level < 0) {
					error(0,&latok->place,"unexpected '}'");
					goto bad;
				}
				return 1;
			}
			break;
		case EOFTOK:
			error('e', &latok->place, "unbalanced {}");
			goto bad;
		} // switch
	} // for
	bad:
		del_tokens( marker );
		marker->tok = SM;
		return 0;
}
