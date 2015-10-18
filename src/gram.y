/*ident	"@(#)cls4:src/gram.y	1.29" */
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

gram.y:
	
	This is the C++ syntax analyser.

	Syntax extensions for error handling:
		nested functions
		any expression can be empty
		any expression can be a constant_expression

	A call to error() does not change the parser's state

***************************************************************************/

%{
#include "cfront.h"
#include "size.h"
#include "template.h"
#include <string.h>
#ifdef SVR42
#include <unistd.h>
#endif
// include tqueue.h after YYSTYPE is defined ...

int		in_friend = 0;
int		must_be_friend = 0;
int		dont_instantiate = 0;
static int explicit_template_definition = 0;
Pname righttname=0;


static struct parstate {
	Ptype intypedef;
	int   infriend;
	int   defercheck;
	Pname intag;
	int cid;
} pstate[BLMAX];
static int px;
extern int classid;
static void
SAVE_STATE()
{
//error('d',"save_state: in_typedef%t in_tag%n defer_check %d",in_typedef,in_tag,defer_check);
	if ( px >= BLMAX ) error('i',"parsing state stack overflow -- current table %s",Ctbl->whereami());
	pstate[px].intypedef = in_typedef;
	pstate[px].infriend = in_friend;
	in_typedef = 0;
	in_friend = 0;
	pstate[px].defercheck = defer_check;
	defer_check = 0;
	pstate[px].intag = in_tag;
	in_tag = 0;
	pstate[px].cid = classid;
	classid = 0;
	++px;
}
static void
RESTORE_STATE()
{
//error('d',"restore_state: in_typedef%t in_tag%n defer_check %d",in_typedef,in_tag,defer_check);
	if ( --px < 0 ) error('i',"parsing state stack underflow -- current table %s",Ctbl->whereami());
	in_typedef = pstate[px].intypedef;
	in_friend = pstate[px].infriend;
	defer_check = pstate[px].defercheck;
	in_tag = pstate[px].intag;
	classid = pstate[px].cid;
//error('d',"            -> in_typedef%t in_tag%n defer_check %d",in_typedef,in_tag,defer_check);
}


//SYM parsing symbol table management
inline void
PUSH_ARG_SCOPE()
{
DB(if(Kdebug>=1)error('d',"push arg table; %ctbl %s",'C',Ctbl->whereami()););
	Ctbl = new ktable( 0, Ctbl, 0 );
	Ctbl->k_id = ARG;
}
inline void
PUSH_CLASS_SCOPE( Pname n )
{
DB(if(Kdebug>=1)error('d',"pushC table%n; %ctbl %s",n,'C',Ctbl->whereami()););
	// table allocated in name::tname() to avoid problems with forward
	//     refs to class templates
	Pclass cl = n->tp->classtype();
	cl->k_tbl->k_next = Ctbl;
	Ctbl = cl->k_tbl;
	Ctbl->expand(CTBLSIZE);
}
inline void
PUSH_BLOCK_SCOPE()
{
DB(if(Kdebug>=1)error('d',"push block tbl; %ctbl %s in %s",'C',Ctbl->whereami(),Ctbl->k_next->whereami()););
	if ( Ctbl->k_id == ARG )
		Ctbl->expand(TBLSIZE);
	else {
		Ctbl = new ktable(TBLSIZE, Ctbl, Ctbl->k_name );
	}
	Ctbl->k_id = BLOCK;
	if ( Ctbl->k_next == Gtbl
	||   Ctbl->k_next->k_id == BLOCK
	||   Ctbl->k_next->k_id == CLASS )
		Ctbl->k_t->next = Ctbl->k_next->k_t;
}
inline void
PUSH_TEMPLATE_SCOPE()
{
DB(if(Kdebug>=1)error('d',"push template tbl; %ctbl %s",'C',Ctbl->whereami()););
	Ctbl = new ktable( 0, Ctbl, 0 );
	Ctbl->k_id = TEMPLATE;
}
inline void
POP_SCOPE( int deallocate = 0 )
{
DB(if(Kdebug>=1)error('d',"pop tbl %s",Ctbl->whereami()););
	if ( Ctbl == Gtbl ) error('i', "bad parsing table");
	Pktab b = Ctbl;
	Ctbl = Ctbl->k_next;
	if ( b->k_id == ARG || deallocate ) delete b;
}


/*SYM -- replaces set_scope()/curr_scope for switching between scopes
 *       in member defs
 * Temporarily reset scope in member defs:
 *     int X::f( T t = a ) { ... }
 *     int X::s[10] =        ... ,  i = 3;
 *            |----scope of X----|
 * ??? What about exprs "p->operator T()", etc ???
 */
// Although local member defs are illegal outside a local class, scopes
//    are stacked for error recovery / extensibility.
struct pcontext {
	Pktab ktbl;
	int   saved_template;
};
static pcontext scopestack[BLMAX];
static int scopex = 0;
Pname
SET_SCOPE( Pname n )
{
	Pktab ntb;
	if ( n == 0 ) ntb = 0;
	else if ( n == sta_name ) ntb = Gtbl;
	else if ( n->n_template_arg == template_type_formal ) {
		DB(if(Kdebug>=1)error('d',"set scope%n -- template formal -- currently in %s next %s",n,Ctbl->whereami(),Ctbl->k_next->whereami()););
		return n;
	}
	else if ( n->tp ) {
		Ptype t = n->tp->skiptypedefs();
		if ( t->base==COBJ ) ntb = t->classtype()->k_tbl;
		else ntb = 0;
	} else ntb = 0;
	DB(if(Kdebug>=1)error('d',"set scope%n ntb %s next %s, currently in %s",n,ntb->whereami(),ntb?ntb->k_next->whereami():"<nil>",Ctbl->whereami()););
	if (ntb == 0) return 0;//error('i',"scope set to null table(n==%n)!",n);
	if ( scopex >= BLMAX ) error('i',"set scope %s: parsing scope stack overflow -- current table %s",ntb->whereami(),Ctbl->whereami());
	Pktab tt = Ctbl;
	if ( Ctbl->k_id == TEMPLATE ) {
		// parsing template member function
		//    template<...> PT<...>::f() {}
		// be sure template params are in scope when parsing f
		// -- extract template scope from current scope and put it
		//    in new scope
		Ctbl = Ctbl->k_next;
		tt->k_next = ntb->k_next;
		ntb->k_next = tt;
		scopestack[scopex].saved_template = 1;
	}
	else if ( Ctbl->k_id == ARG && Ctbl->k_next->k_id == TEMPLATE ) {
		// parsing static data member template with declarator ()'s.
		//    template<...> T (PT<...>::d) = v;
		// interpose PT's table between ARG table and its parent
		Ctbl = Ctbl->k_next->k_next;
		tt->k_next->k_next = ntb->k_next;
		ntb->k_next = tt->k_next;
		tt->k_next = ntb;
		scopestack[scopex].saved_template = 1;
		ntb = tt;
	}
	scopestack[scopex++].ktbl = Ctbl;
	Ctbl = ntb;
	return n;
}
void
UNSET_SCOPE()
{
	// restore surrounding template scope, if appropriate
	Pktab tt = 0;
	if ( --scopex < 0 ) error('i',"parsing scope stack underflow -- current scope %s",Ctbl->whereami());
	if ( scopestack[scopex].saved_template ) {
		scopestack[scopex].saved_template = 0;
		if ( Ctbl->k_next==0 || Ctbl->k_next->k_id != TEMPLATE )
			error('i',"set scope failed restoring template table -- current table %s",Ctbl->whereami());
		tt = Ctbl->k_next;
		Ctbl->k_next = tt->k_next;
	}
	DB(if(Kdebug>=1)error('d',"unset scope %s -> %s tt %s",Ctbl->whereami(),scopestack[scopex].ktbl->whereami(),tt?tt->whereami():"<nil>"););
	Ctbl = scopestack[scopex].ktbl;
	if ( tt ) { tt->k_next = Ctbl; Ctbl = tt; }
}
inline Pktab
GET_XSCOPE()
{
	if ( scopex-1 < 0 ) error('i',"parsing scope stack underflow -- current table %s",Ctbl->whereami());
	return scopestack[scopex-1].ktbl;
}
inline void
SET_XSCOPE( Pktab tb )
{
	if ( scopex-1 < 0 ) error('i',"parsing scope stack underflow -- current table %s",Ctbl->whereami());
	scopestack[scopex-1].ktbl = tb;
}


// macros
#define copy_if_need_be(s)  ((templp->in_progress || templp->parameters_in_progress) ? strdup(s) : s) 
#define YYMAXDEPTH 600

#if 0
#define YYCLEAN {free(yys); free(yyv);}
#else
#define YYCLEAN
#endif

#ifdef DBG
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#endif

static int init_seen = 0;
static int cdi = 0;
static Pnlist cd = 0, cd_vec[BLMAX];
static char stmt_seen = 0, stmt_vec[BLMAX];
static Pnlist scd[BLMAX]; // keep track of cd list outside of switch
static int scdp = -1;

static Pname memptr_pn;
static TOK memptr_tok;

static Pname err_name = 0;

// support for template friend declarations within a class
static Pcons templ_friends;

// fcts put into norm2.c just to get them out of gram.y
void sig_name(Pname);	
Ptype tok_to_type(TOK);
void memptrdcl(Pname, Pname, Ptype, Pname);

static bit decl_with_init(Pnlist cd) 
/* do the declarations have an initializer
   or a class object with a constructor */
{
	for (Pname n=cd->head;n;n=n->n_list) {
		if (n->n_initializer) return 1;
		Pname cln=n->tp->is_cl_obj();
		if (cln && Pclass(cln->tp)->c_ctor)
			return 1;
	}
	return 0;
}


static char* 
get_classname(char* s)
{
// error('d',"get_classname(%s)",s);
	char* r = new char[strlen(s)+1];
	sprintf(r,s);
	s = r;

	char* s1 = s;
	while (*s) {
		for ( ; s[0] && (s[0] != '_' || s[1] && s[1] != '_'); s++) s1++;;
		if (*s) {
			if (strncmp(s,"___pt__",7)==0) {
				*s1 = 0;
				return r;
			}
			if (strncmp(s,"__pt__",6)==0) { // parameterized class
				*s1 = '\0';
				return r;
			}
		}
		return r;
	}
	return r;
}

static Pptr doptr(TOK p, TOK t)
{
	Pptr r = new ptr(p,0);
	switch (t) {
	case CONST:
		r->b_const = 1;
               // if (p == RPTR) error('w',"redundant `const' after &");
		break;
	case VOLATILE:
		error('w',"\"volatile\" not implemented (ignored)");
		break;
	default:
		error("syntax error: *%k",t);
	}
	return r;
}

static Pbcl dobase(TOK pr, Pname n, TOK v = 0)
{
// error('d',"dobase(%k %n %k)", pr,n,v);

	Pbcl b = new basecl(0,0);
	b->ppp = pr;	// save protection indicator

	if (n) {
		if (n->base != TNAME) {
			Pname nn = k_find_name(n->string,Ctbl,HIDDEN);
			if ( nn == 0 ) {
				error("BN%n not aTN",n);
				return 0;
			} else
				n = nn;
		}

		Pbase bt = Pbase(n->tp);
		while (bt->base == TYPE) bt = Pbase(bt->b_name->tp);

                if (bt->base != COBJ) {
                        // template <class B> class D : public B {};
                        if (templp->in_progress == true  && bt->base == ANY)
                            error('s',"formalTZ%n used asBC ofY",n);
                        else error("BN%n not aCN",n);
                        return 0;
                }

		if (v) {
			if (v != VIRTUAL) error("syntax error:%k inBCD",v);
			b->base = VIRTUAL;
		}
		else
			b->base = NAME;

		b->bclass = Pclass(bt->b_name->tp);
	}

	return b;
}


#define Ndata(a,b)	b->normalize(Pbase(a),0,0)
#define Ncast(a,b)	b->normalize(Pbase(a),0,1)
#define Nfct(a,b,c)	b->normalize(Pbase(a),Pblock(c),0)
//#define Ncopy(n)	(n->base==TNAME)?new name(n->string):n
inline Pname Ncopy(Pname n)
{
	Pname nn;
	if (n->base!=TNAME) {
		nn = n;
	} else {
		nn = new name(n->string);
		nn->n_template_arg = n->n_template_arg;
	}
	return nn;
}

#define Finit(p)	Pfct(p)->f_init
#define Fargdcl(p,q,r)	Pfct(p)->argdcl(q,r)
#define Freturns(p)	Pfct(p)->returns
#define Fbody(p)	Pfct(p)->body /*SYM*/
#define Vtype(v)	Pvec(v)->typ
#define Ptyp(p)		Pptr(p)->typ

		/* avoid redefinitions */
#undef EOFTOK
#undef ASM
#undef BREAK
#undef CASE
#undef CONTINUE
#undef DEFAULT
#undef DELETE
#undef DO
#undef ELSE
#undef ENUM
#undef FOR
#undef FORTRAN
#undef FRIEND
#undef GOTO
#undef IF
#undef NEW
#undef OPERATOR
#undef RETURN
#undef SIZEOF
#undef SWITCH
#undef THIS
#undef WHILE
#undef LP
#undef RP
#undef LB
#undef RB
#undef REF
#undef DOT
#undef NOT
#undef COMPL
#undef MUL
#undef AND
#undef PLUS
#undef MINUS
#undef ER
#undef OR
#undef ANDAND
#undef OROR
#undef QUEST
#undef COLON
#undef ASSIGN
#undef CM
#undef SM
#undef LC
#undef RC
#undef ID
#undef STRING
#undef ICON
#undef FCON
#undef CCON
#undef ZERO
#undef ASOP
#undef RELOP
#undef EQUOP
#undef DIVOP
#undef SHIFTOP
#undef ICOP
#undef TYPE

#undef CATCH
#undef THROW
#undef TRY

#undef TNAME
#undef EMPTY
#undef NO_ID
#undef NO_EXPR
#undef FDEF
#undef ELLIPSIS
#undef AGGR
#undef MEM
#undef MEMPTR
#undef PR
#undef MEMQ
#undef TSCOPE
#undef DECL_MARKER
#undef REFMUL
#undef LDOUBLE
#undef LINKAGE
#undef TEMPLATE

#undef XVIRT
#undef XNLIST
#undef XILINE
#undef XIA
#undef SM_PARAM
#undef PTNAME
#undef NEW_INIT_KLUDGE
%}

%union {
	char*	s;
	TOK	t;
	int	i;
	loc	l;
	Pname	pn;
	Ptype	pt;
	Pexpr	pe;
	Pstmt	ps;
	Pbase	pb;
	Pnlist	nl;
	Pslist	sl;
	Pelist	el;
	Pbcl	pbc;
	Pptr	pp;
	PP	p;	// fudge: pointer to all class node objects
	Plist	pl;
	toknode* q;	// token queue
}
%{
#include "tqueue.h"
extern YYSTYPE yylval, yyval;
extern int yyparse();

// in_typedef should allow for nested in_typedef
extern int	declTag;	 // !1: inline, virtual mod permitted
int		in_sizeof = 0;
int		in_new = 0;
Ptype 		in_typedef = 0;  // catch redefinition of TNAME
Pname		in_tag = 0;      // handle complex typedefs: int (*)()
extern int	defer_check;	 // redefinition typedef check delay

extern 	int must_be_id;	 // !0, TNAME => ID, i.e., int X
int	DECL_TYPE = 0; 	 // lalex() wants this set for global x(*fp)()
int	in_arg_list=0; 	 // !0 when parsing argument list, 1: in (), 2: in <>
static  int in_binit_list=0;
int	in_class_decl=0; // !0 when processing class definition
int	parsing_class_members=0; // !0 when parsing class def but not member function body
int	in_mem_fct=0;    // !0 when parsing member function definition
Ptempl_inst pti = 0; // explicit template class: class X<int> {};

#define yylex lalex
#define NEXTTOK() ( (yychar==-1) ? (yychar=yylex(),yychar) : yychar )
#define EXPECT_ID() must_be_id = 1
#define NOT_EXPECT_ID() must_be_id = 0

Pname syn()
{
ll:
	switch (yyparse()) {
	case 0:		return 0;	// EOF
	case 1:		goto ll;	// no action needed
	default:	return yyval.pn;
	}
}

%}
/*
	the token definitions are copied from token.h,
	and all %token replaced by %token
*/
			/* keywords in alphabetical order */
%token EOFTOK		0
%token ASM		1
%token AUTO		2
%token BREAK		3
%token CASE		4
%token CONTINUE		7
%token DEFAULT		8
%token DELETE		9
%token DO		10
%token ELSE		12
%token ENUM		13
%token FOR		16
%token FORTRAN		17 
%token FRIEND		18 
%token GOTO		19
%token IF		20
%token NEW		23
%token OPERATOR		24
%token RETURN		28
%token SIZEOF		30
%token SWITCH		33
%token THIS		34
%token WHILE		39

			/* operators in priority order (sort of) */
%token LP		40
%token RP		41
%token LB		42
%token RB		43
%token REF		44
%token DOT		45
%token NOT		46
%token COMPL		47
%token MUL		50
%token AND		52
%token PLUS		54
%token MINUS		55
%token LT		58
%token GT		60
%token ER		64
%token OR		65
%token ANDAND		66
%token OROR		67
%token QUEST		68
%token COLON		69
%token ASSIGN		70
%token CM		71
%token SM		72
%token LC		73
%token RC		74

    /* = constants etc. */
%token ID	   80
%token STRING	   81
%token ICON	   82
%token FCON	   83
%token CCON	   84
%token NAME	   85
%token ZERO	   86
    /* groups of tokens */
%token ASOP   90	/* op= */
%token RELOP  91	/* LE GE LT GT */
%token EQUOP  92	/* EQ NE */
%token DIVOP  93	/* DIV MOD */
%token SHIFTOP		94	/* LS RS */
%token ICOP		95	/* INCR DECR */
%token TYPE		97
    /* TYPE =	INT FLOAT CHAR DOUBLE REGISTER STATIC EXTERN AUTO
	LONG SHORT UNSIGNED INLINE FRIEND VIRTUAL */

%token CATCH		98
%token THROW		99
%token TRY		100

%token TNAME	  123
%token EMPTY	  124
%token NO_ID	  125
%token NO_EXPR	  126
%token FDEF	  127

%token ELLIPSIS	155
%token AGGR	156
%token MEM	160
%token MEMPTR	173
%token PR		175	/* PUBLIC PRIVATE PROTECTED */
%token MEMQ		176	/* TNAME :: */
%token TSCOPE		178	/* TNAME :: */
%token DECL_MARKER	179
%token REFMUL		180	/* ->*, .* */
%token LDOUBLE		181
%token LINKAGE		182	/* extern "asdf" */
%token TEMPLATE		185	/* local class */

/* "tokens" for aux data structures */

%token XVIRT	        200 /* class virt */
%token XNLIST		201 /* struct name_list */
%token XILINE		202
%token XIA		203
%token SM_PARAM         207 
%token PTNAME           209
%token NEW_INIT_KLUDGE  210
%token XDELETED_NODE    211
%token DUMMY_LAST_NODE  212

%type <p>	external_def fct_dcl fct_def att_fct_def arg_dcl_list 
		base_init init_list binit
		data_dcl ext_def vec ptr
		type tp enum_dcl moe_list moe 
		tag ttag enumtag class_head class_dcl cl_mem_list 
		cl_mem dl decl_list 
		fname decl initializer stmt_list
		caselab_stmt caselablist
		block statement simple ex_list elist e ee term prim
		term_elist
		cast_decl cast_type c_decl c_type c_tp
		arg_decl formal_decl at arg_type arg_list arg_type_list
		new_decl new_type
		new_type2 new_decl2 cm
		condition
		TSCOPE tscope MEMQ TNAME tn_list MEMPTR dtorspec
		scope_qualifiers /*SYM*/
		qualified_tname
		PTNAME tname ptname template_def
%type <sl>	handler_list
%type <ps>	handler
%type <l>	LC RC SWITCH CASE DEFAULT FOR IF DO WHILE GOTO RETURN DELETE
		BREAK CONTINUE
%type <t>	oper ellipsis_opt
		EQUOP DIVOP SHIFTOP ICOP RELOP GT LT ASOP
		ANDAND OROR PLUS MINUS MUL ASSIGN OR ER AND 
		LP LB NOT COMPL AGGR
		TYPE PR REFMUL
%type <s>	CCON ZERO ICON FCON STRING LINKAGE
%type <pn>	ID FDEF inline_fct_def identifier exception_type
%type <pbc>	base_list base_unit_list base_unit
%type <q>	EMPTY
%type <i>	fct_attributes
%type <pl>	arg_lp type_list
%type <pe>      temp_inst_parm
%type <el>      temp_inst_parms

%left	EMPTY
%left	NO_ID
%left	RC LC ID BREAK CONTINUE RETURN GOTO DELETE DO IF WHILE FOR CASE DEFAULT
	AGGR ENUM TYPE TNAME TSCOPE
%left	NO_EXPR

%left	CM
%right	ASOP ASSIGN
%right	QUEST COLON
%left	OROR
%left	ANDAND
%left	OR
%left	ER
%left	AND
%left	EQUOP
%left	RELOP GT LT
%left	SHIFTOP
%left	PLUS MINUS
%left	MUL DIVOP MEMPTR
%left   REFMUL
%right	NOT COMPL NEW
%right	ICOP SIZEOF
%left	LB LP DOT REF MEM

%start ext_def

%%
/*
	this parser handles declarations one by one,
	NOT a complete .c file
*/


/************** DECLARATIONS in the outermost scope: returns Pname (in yylval) ***/

ext_def		:  external_def		{	YYCLEAN;return 2; }
		|  SM			{	YYCLEAN;return 1; }
		|  EOFTOK		{	YYCLEAN;return 0; }
		|  LINKAGE LC
			{
				set_linkage($<s>1);
				bl_level--;
				YYCLEAN;return 1;
			}
		|  RC
			{
				set_linkage(0);
				bl_level++;
				YYCLEAN;return 1;
			}
 		|  template              {  YYCLEAN;return 1; } 
		;

template    :    TEMPLATE
                 {	
			PUSH_TEMPLATE_SCOPE();//SYM
// error('d',"template seen: in_class_decl %d", in_class_decl);
			if (in_class_decl) {
				must_be_friend = 1;
				if (templp->in_progress == true) // inside template
					templp->save_templ = new templ_state;
			}
			else must_be_friend = 0;
			templp->start() ;
			templp->formals_in_progress = true;
		 }
                 LT { in_arg_list = 2; } template_parm_list GT
                 {
			templp->enter_parameters(); 
			templp->formals_in_progress = false;
			in_arg_list = 0;
		 }
                 template_def
                 {
			templp->end($<pn>8);
			POP_SCOPE(); //SYM
			if (in_class_decl && templp->save_templ) {
				delete templp->save_templ;
				templp->save_templ = 0;
			}
			else { 
				templp->in_progress = false;
				bound_expr_tbl->reinit();
			}
                  //SYM -- goto mod removed
                 }
                ;

template_def    : data_dcl 
		| att_fct_def { goto mod; }
                | fct_def    { goto mod; }
                | fct_dcl
                | class_dcl SM
                  { 
			Pname pn = $<pb>1->aggr();
			/* basetype:aggr() does not return the name for a forward 
		    	 * declaration, so extract it directly */
                    	$$ = (pn ? pn : $<pb>1->b_name);
                    	DECL_TYPE = 0;
		  }
                 ;

identifier	: ID 
		|  qualified_tname
			{ $<pn>$ = Ncopy($<pn>1) ;}
		;

external_def	:  data_dcl
			{	
				//SYM -- tn stuff removed
				if ($<pn>1==0) $<i>$ = 1; 
			}
		|  att_fct_def  { goto mod; }
		|  fct_def
			{ mod: //SYM -- tn stuff removed
				Pname n = $<pn>1;
				if ( n && n->n_qualifier ) {//SYM
					if ( n->n_qualifier->n_template_arg != template_type_formal )
						UNSET_SCOPE();
					if ( n->n_qualifier == sta_name )
						n->n_qualifier = 0;
				}
			}
		|  fct_dcl
		|  ASM LP STRING RP SM
			{	Pname n = new name(make_name('A'));
				n->tp = new basetype(ASM,0);
				Pbase(n->tp)->b_name = Pname($<s>3);
				$$ = n;
			}
		;

fct_dcl		: decl ASSIGN initializer SM
			{
				err_name = $<pn>1;
				if(err_name) err_name->n_initializer = $<pe>3;
				goto fix;
			}
		| decl SM
			{
				Ptype t;
				err_name = $<pn>1;
			fix:
				if (err_name == 0) {
					error("syntax error:TX");
					$$ = Ndata(defa_type,err_name);
				}
				else if ((t=err_name->tp) == 0) {
					error("TX for%n",err_name);
					$$ = Ndata(defa_type,err_name);
				}
				else if (t->base==FCT) {
					if (Pfct(t)->returns==0)
						$$ = Nfct(defa_type,err_name,0);
					else
						$$ = Ndata(0,err_name);
				}
				else {
					error("syntax error:TX for%k%n",t->base,err_name);
					$$ = Ndata(defa_type,err_name);
				}
				if ( err_name && err_name->n_qualifier ) {
					if ( err_name->n_qualifier->n_template_arg != template_type_formal )
						UNSET_SCOPE();
					if ( err_name->n_qualifier == sta_name )
						err_name->n_qualifier = 0;
				}
			}
		;

att_fct_def	:  type decl arg_dcl_list check_inline
			{	if ( yychar == LC ) --bl_level;
				Pname n = Nfct($1,$<pn>2,dummy);
				Fargdcl(n->tp,name_unlist($<nl>3),n);
				arg_redec( $<pn>2 );
				$<pn>$ = n;
			 	if ( yychar == LC ) ++bl_level;
				Ctbl->k_name = n;
			}/*SYM*/
		   base_init block
			{	Pname n = $<pn>5;//SYM
				if ( !in_typedef ) {
					Fbody(n->tp) = Pblock($7);//SYM
					Finit(n->tp) = $<pn>6;
				}
				$$ = n;
				NOT_EXPECT_ID();
			}
		|  type decl arg_dcl_list check_inline EMPTY
			{
				Pname n = Nfct($1,$<pn>2,dummy);
				Fargdcl(n->tp,name_unlist($<nl>3),n);
				$<q>5->retval.pn = n;
				$$ = n;
				NOT_EXPECT_ID();
			}
		|  type decl arg_dcl_list check_inline NO_ID /*syntax error*/
			{
				if (!templp->in_progress)
					error(&$<pn>2->where,"syntax error -- did you forget a ';'?");
				Pname n = Nfct($1,$<pn>2,0);
				$$ = n;
				NOT_EXPECT_ID();
			}
		;

fct_def		:  decl arg_dcl_list check_inline
			{	if ( yychar == LC ) --bl_level;
			 	Pname n = Nfct(defa_type,$<pn>1,dummy);
				Fargdcl(n->tp,name_unlist($<nl>2),n);
				arg_redec( $<pn>1 );
				$<pn>$ = n;
			 	if ( yychar == LC ) ++bl_level;
				Ctbl->k_name = n;
			}/*SYM*/
		   base_init block
			{	Pname n = $<pn>4;//SYM
				Fbody(n->tp) = Pblock($6);//SYM
				if ( $<pn>5 && $<pn>5->n_list && 
     					ccl && ccl->csu == UNION )  
						error( "multiple initializers in unionK %s::%n", $<pn>1->string, $<pn>1 );
				Finit(n->tp) = $<pn>5;
				$$ = n;
				NOT_EXPECT_ID();
			}
		|  decl arg_dcl_list check_inline EMPTY
			{
				Pname n = Nfct(defa_type,$<pn>1,dummy);
				Fargdcl(n->tp,name_unlist($<nl>2),n);
				$<q>4->retval.pn = n;
				$$ = n;
				NOT_EXPECT_ID();
			}
		|  decl arg_dcl_list check_inline NO_ID /*syntax error*/
			{
				if (explicit_template_definition  == 0 )
					error(&$<pn>1->where,"badD of%n -- did you forget a ';'?",$<pn>1);
				else {
			             Pname n = pti->get_tname();
				     if ($<pn>1->n_oper == DTOR)
					error('s',"explicitYZL for destructor of specializedYC%n -- please drop the parameter list",n);
				     else
					error('i',"specialializedYC%n: declaration problem: %s",n,$<pn>1->string);
				     error('i', "cannot recover from previous error" );
				}
				Pname n = Nfct(defa_type,$<pn>1,0);
				$$ = n;
				NOT_EXPECT_ID();
			}
		;

inline_fct_def	:  FDEF 
			{	PUSH_ARG_SCOPE();//SYM
                        	arg_redec($<pn>1);
                        	Ctbl->k_name = $<pn>1;
                   	}
		   base_init block
			{
				Finit($1->tp) = $<pn>3;
				Pfct($1->tp)->body = Pblock($4);
				$$ = $1;
				NOT_EXPECT_ID();
			}
		;


check_inline	:  /* empty */
		   {
			// if parsing implicit inline def, save body
			//   of function for parsing after class def
			if ( Ctbl->k_id != ARG )
				error('i',"expectingA table in check_inline!");
			switch ( NEXTTOK() ) {
			case LC: case COLON:
				if ( in_class_decl ) {
					// mem or friend inline def
					// save text of mem_init & ftn
					la_backup(yychar,yylval);
					// yylval used as dummy...
					la_backup(FDEF, yylval);
					if ( yylval.q = save_text() ) {
						yychar = EMPTY;
						POP_SCOPE();//SYM
					} else { // syntax error
						// just parse in place
						yylex(); // FDEF
						yylex();
						yychar = NO_ID;
						hoist_al();//SYM
					}
				} // if in_class_decl
				//SYM -- else non-nested ftn def
				//SYM -- arg table will become block table
				break;
			default:
				la_backup(yychar,yylval);
				yychar = NO_ID; // 'graceful' recovery
				hoist_al();//SYM
				break;
			}
		   }
		;

base_init	:  COLON { ++in_binit_list; } init_list
			{	
				$$ = $3; 
				in_arg_list = 0;
				--in_binit_list;
			}
		|  %prec EMPTY
			{	$$ = 0; }
		;

init_list	:  binit
			{ $$ = $1; }
		|  init_list CM binit
			{ $<pn>$ = $<pn>3;  $<pn>$->n_list = $<pn>1; }
		;

binit		:  LP elist RP
			{
				$<pn>$ = new name;
				$<pn>$->n_initializer = $<pe>2;
			}
		|  ttag LP elist RP
			{
				Pname n = Ncopy($<pn>1);
				n->base = $<pn>1->base;
				n->tp = $<pn>1->tp;
				n->n_initializer = $<pe>3;
				$<pn>$ = n;
			}
		;




/*************** declarations: returns Pname ********************/

arg_dcl_list	:  arg_dcl_list data_dcl
			{	if ($<pn>2 == 0)
					error("badAD");
				else if ($<pn>2->tp->base == FCT)
					error("FD inAL (%n)",$<pn>2);
				else if ($1)
                                        $<nl>1->add_list($<pn>2);
				else
					$<nl>$ = new nlist($<pn>2);
			}
		|  %prec EMPTY
			{
				PUSH_ARG_SCOPE();//SYM
                                $$ = 0; 
			}
		;

dl		:  decl
		|  ID COLON 
                        {
                                if ( in_typedef ) {
                                        error("Tdef field");
                                        in_typedef = 0;
                                }
                        //      ENTER_NAME($<pn>1);
                        }
		    e		%prec CM
			{	$$ = $<pn>1;
				$<pn>$->tp = new basetype(FIELD,$<pn>4);
		 	}
		|  COLON e		%prec CM
			{	$$ = new name;
				$<pn>$->tp = new basetype(FIELD,$<pn>2);
                                if ( in_typedef ) {
                                        error("Tdef field");
                                        in_typedef = 0;
                                }
			}
                |  decl ASSIGN
                        {
                        //      ENTER_NAME($<pn>1);
                        }
                   initializer
                        {       Pexpr e = $<pe>4;
                                if (e == dummy) error("emptyIr");
                                $<pn>1->n_initializer = e;
                                init_seen = 0;
                        }
                ;

decl_list	:  dl
			{	Pname n = $<pn>1;
				if (n) {
				    $<nl>$ = new nlist(n);
				    if ( n->n_qualifier ) {//SYM
					if ( n->n_qualifier->n_template_arg != template_type_formal )
						UNSET_SCOPE();
					if ( n->n_qualifier == sta_name )
					    n->n_qualifier = 0;
				    }
				}
				if ( NEXTTOK() == CM && la_look() == TNAME ) 
					EXPECT_ID();
			}
		|  decl_list CM dl
			{	Pname n = $<pn>3;
				if ($1)
					if (n)
						$<nl>1->add(n);
					else
						error("DL syntax");
				else {
					if (n) $<nl>$ = new nlist(n);
					error("DL syntax");
				}
				if ( n ) {
				    if ( n->n_qualifier ) {//SYM
					if ( n->n_qualifier->n_template_arg != template_type_formal )
						UNSET_SCOPE();
					if ( n->n_qualifier == sta_name )
					    n->n_qualifier = 0;
				    }
				}
				if ( NEXTTOK() == CM && la_look() == TNAME ) 
					EXPECT_ID();
			}
		;

data_dcl	:  type decl_list SM	
			{ 
				extern int co_hack;
				co_hack = 1;
				/*$$ = Ndata($1,name_unlist($<nl>2));*/
				Pname n = Ndata($1,name_unlist($<nl>2)); 
//error('d',"data_dcl:type decl_list sm: %n%t in_typedef%t in_tag%n",n,n->tp,in_typedef,in_tag);
				//SYM redef check removed
				in_typedef = 0;
				in_friend = 0;
				in_tag = 0;
				co_hack = 0;
				DECL_TYPE = 0; 
				$$ = n;
			}
		|  type SM		
			{
				$$ = $<pb>1->aggr(); 
				in_typedef = 0;
				in_friend = 0;
				in_tag = 0;
				DECL_TYPE = 0; 
			}
		;

/* This is where parametrized types, and regular types come together. */

lt 		: LT
		  {
			templp->parameters_in_progress++;
			in_arg_list = 2;
			check_decl();
		  };
gt 		: GT
		  {
			templp->parameters_in_progress--;
			if (!templp->parameters_in_progress)
				in_arg_list = 0;
		  };

tname		: qualified_tname { $<pn>$ = templp->check_tname($<pn>1) ; }
                | qualified_tname lt temp_inst_parms gt
                  { 
			int sm = NEXTTOK()==SM;
			if (in_friend) in_friend += sm;
			bit flag=0;
			if (dtpt_opt && in_typedef &&  
				!templp->parameters_in_progress &&
				curloc.file == first_file)
				flag=1;
		    	$<pn>$ = parametrized_typename($<pn>1,
						  (expr_unlist($<el>3)),in_friend) ; 
			if (flag)
				righttname=$<pn>$;
		  }
                | NAME lt temp_inst_parms gt
                  { extern Pbase any_type;
		    error("%n was not aZizedT.", $<pn>$) ;
                    $<pn>$= $<pn>1->tdef() ;
                    $<pn>$->tp = any_type ; }
		;



tp		:  TYPE			
			{ 
				$$ = new basetype($<t>1,0); 
				if ( $<t>1 == TYPEDEF ) {
					in_typedef = $<pt>$;
// error('d',"typedef: ccl %t ", ccl, $<pn>1);
				}
				else if ( $<t>1 == FRIEND ) {
					in_friend = 1;
					must_be_friend = 0;
				}
				if (DECL_TYPE == -1) DECL_TYPE = 0;
			}
		|  LINKAGE		
			{	$$ = new basetype(EXTERN,0);
				$<pb>$->b_linkage = $<s>1;
				if (DECL_TYPE == -1) DECL_TYPE = 0;
			}
		|  tname %prec NO_ID
			{
				$$ = new basetype(TYPE,$<pn>1); 
				if (DECL_TYPE == -1) DECL_TYPE = 0;
			}
	/*XXX*/	|  tn_list DECL_MARKER
			{ // modified tn_list TNAME
				$$ = new basetype(TYPE,$<pn>2);
				//xxx qualifier currently ignored...
				if (DECL_TYPE == -1) DECL_TYPE = 0;
			}
		|  class_dcl 
		|  enum_dcl
		|  DECL_MARKER		
			{ 
				if (DECL_TYPE == TNAME)
					$$ = new basetype(TYPE,$<pn>1); 
		//	else if (DECL_TYPE == TSCOPE)
		//	$$ = 0;
				else
				if (DECL_TYPE == 0 &&
					$<p>1->base == TNAME)
						$$ = new basetype(TYPE,$<pn>1); 
				else
					$$ = new basetype($<t>1,0); 
				DECL_TYPE = -1;
			}
		;

type		:  tp
		|  type TYPE		
			{ 
				if ( DECL_TYPE != -1 ) {
					switch ($<pb>1->base) { 
					case COBJ: case EOBJ:
						Pbase bt;	
						bt = new basetype(0,0);
						*bt = *$<pb>1;
						DEL($<pb>1);
						$<pb>1 = bt;
					}
					$$ = $<pb>1->type_adj($<t>2); 
				}
				DECL_TYPE = 0;
			}
		|  type tname %prec NO_ID
			{
//error('d',"decl_type: %d  $1: %t  $2: %n",DECL_TYPE,$<pb>1,$<pn>2);
				if ( DECL_TYPE != -1 ) 
			 		$$ = $<pb>1->name_adj($<pn>2);
			/*XXX*/	else if($<pb>1==0) $$=new basetype(TYPE,$<pn>2);
				DECL_TYPE = 0;
			}
		|  type class_dcl	{ $$ = $<pb>1->base_adj($<pb>2); }
		|  type enum_dcl	{ $$ = $<pb>1->base_adj($<pb>2); }
		|  type DECL_MARKER		
			{ 
				if (DECL_TYPE == TYPE) {
					switch ($<pb>1->base) { 
					case COBJ: case EOBJ: 
						{
						Pbase bt;
						bt = new basetype(0,0);
						*bt = *$<pb>1;
						DEL($<pb>1);
						$<pb>1 = bt;
						}
					}
					$$ = $<pb>1->type_adj($<t>2);  
				}
			/*XXX*/	else if (DECL_TYPE == TSCOPE) {
			/*XXX*/		error('i',"T decl_marker(tscope)");
			/*XXX*/	//	$$ = $1;//ignore(?)
			/*XXX*/	}
				else
					$$ = $<pb>1->name_adj($<pn>2); 
				DECL_TYPE = -1;
			}
		;

cm		: CM {in_arg_list = 2; check_decl();}

temp_inst_parms : temp_inst_parms cm temp_inst_parm
                  {$<el>1->add(new expr(ELIST,$<pe>3,NULL)) ; }
                | temp_inst_parm
		  	{ in_arg_list=0; $<el>$ = new elist(new expr(ELIST,$<pe>1,NULL)); }
		;

new_decl2	: %prec NO_ID
			{
				$$ = new name;
			}
		| arg_lp new_decl2 RP
			{
				$$ = $2;
				in_arg_list = 0;
				hoist_al();
			}
		| ptr new_decl2  %prec MUL
			{
				Ptyp($1) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$1;
				$$ = $2;
				NOT_EXPECT_ID();
			}
		| new_decl2 arg_list
			{
				Freturns($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
		| new_decl2 vec %prec LB
			{
				Vtype($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
		;

new_type2	: type new_decl2	{ $$ = Ncast($1,$<pn>2); };

temp_inst_parm  : new_type2
		 	{
				$<pn>1->n_template_arg = template_actual_arg_dummy;
				$<pe>$ = $<pn>1; /* keep yacc happy */ 
			}
                | e %prec GT
                  	{ 
				if ($<pe>1 == dummy) error("emptyYZL");
				$<pe>$ = $<pe>1; 
			}
		;

/***************** aggregate: returns Pname *****************/

enumtag	:  tag { $$ = enumcheck( $<pn>1); }
	|  DECL_MARKER
		{
			if ( DECL_TYPE != TNAME ) {
				error("syntax error -- enum%k",$<t>1);
				$$ = 0;
			} else 
				$$ = enumcheck( $<pn>1);
		}
	|  tn_list DECL_MARKER 
		{
			if ( DECL_TYPE != TNAME ) {
				error("enum declaration syntax");
				$$ = 0;
			} else 
				$<pn>$ = enumcheck($<pn>2);
			if (in_typedef && in_typedef->base == 0)
				in_typedef->defined = TNAME_SEEN;
			//xxx qualifier currently ignored...
		}
	;

enum_dcl	:  ENUM LC { ++in_class_decl; } moe_list RC
			{ --in_class_decl; $$ = end_enum(0,$<nl>4); }
		|  ENUM enumtag LC { ++in_class_decl; } moe_list RC
			{ --in_class_decl; $$ = end_enum($<pn>2,$<nl>5); }
		|  ENUM enumtag			{ $<pb>$ = (Pbase)$<pn>2->tp; }
		;

moe_list	:  moe
			{	if ($1) $<nl>$ = new nlist($<pn>1); }
		|  moe_list CM moe
			{	if( $3)
					if ($1)
						$<nl>1->add($<pn>3);
					else
						$<nl>$ = new nlist($<pn>3);
			}
		;

template_parm_list : template_parm_list CM template_parm
                   | template_parm
                   | /* empty */
			{ 
				$<pn>$ = NULL;
				error("emptyYZL");
			}
		;

template_parm   : AGGR identifier
                  /* Build the name for the parameter
		  /* Check that AGGR is indeed CLASS */
                 { templp->collect($<t>1, $<pn>2) ; }
                 | type formal_decl
			{templp->collect(Ndata($1,$<pn>2)); }
		;	


/* these productions are a variant of the ones for arg_decl,
 * verify them against arg_decl for each release. */
formal_decl	:  ID
			{	$$ = $<pn>1; }
		|  ptr formal_decl		%prec MUL
			{	Ptyp($1) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$1;
				$$ = $2;
			}
		|  formal_decl vec		%prec LB
			{	Vtype($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
		|  formal_decl arg_list
			{	Freturns($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
		|  arg_lp formal_decl RP
			{
				$$ = $2;
				in_arg_list = 0;
				hoist_al();
			}
		;


moe		:  ID %prec NO_ID
			{
				if ( $<pn>1->n_oper != TNAME )
					insert_name($<pn>1,Ctbl);
				$$ = $<pn>1;
				$<pn>$->tp = moe_type;
			}
		|  ID
			{
				if ( $<pn>1->n_oper != TNAME )
					insert_name($<pn>1,Ctbl);
			}
		   ASSIGN e
			{	$$ = $<pn>1;
				$<pn>$->tp = moe_type;
				$<pn>$->n_initializer = $<pe>4;
			}
		|  /* empty: handle trailing CM: enum e { a,b, }; */
			{	$$ = 0; }
		;

class_dcl	:  class_head cl_mem_list RC
			{	parsing_class_members = 0;
				RESTORE_STATE();
				switch ( NEXTTOK() ) {
				case TYPE: case AGGR: case ENUM: case EOFTOK:
					error("`;' or declaratorX afterCD");
					la_backup(yychar,yylval);
					yychar = SM;
					break;
				}
				la_backup(yychar,yylval);
				yychar = -1;
				restore_text();
				++bl_level; // scope weirdness!
				++in_mem_fct;
			}
			inline_mem_defs
			{
				--in_mem_fct;
				--bl_level; // scope weirdness!
				if ( yychar == ID ) {
					// (yuk!) adjust lex level
					--yylval.pn->lex_level;
				}
				ccl->mem_list = name_unlist($<nl>2);
// error('d',"ccl: %t templ_friends: %d", ccl, templ_friends);
				ccl->templ_friends = templ_friends; templ_friends = 0;
				if ( --in_class_decl )  // nested class
					// continue to parse enclosing class
					parsing_class_members = 1;
				//SYM -- tn stuff removed
                        	POP_SCOPE();//SYM
				if ( Ctbl->k_id == TEMPLATE ) {
					// remove intermediate template table
					//    from scope of class
					ccl->k_tbl->k_next = Ctbl->k_next;
				}

				if (pti) {
					pti->explicit_inst();
					Pname nnn = $<pt>$->bname();
// error('d',"ccl: %s nnn: %s", ccl->string, nnn->string);
					nnn->string = ccl->string;
					explicit_template_definition = 0;
					pti = 0;
				}

				end_cl();
				declTag = 1;
			}
        	|  AGGR tag 
			{ aggrcheck:
				$<pb>$ = (Pbase)$<pn>2->tp; 
				if ( $$ == 0 ) {
					if (templp->parameters_in_progress)
						error("TX for%n -- did you misdeclare aY?",$<pn>2);
					else error("TX for %n",$<pn>2);
					error('i', "cannot recover from previous error" );
				}

				if ( $$->base == TYPE ) {
					Pname nx = $<pb>$->b_name;
					$<pb>$ = (Pbase)nx->tp;
					if ( $$->base != COBJ
					||   strcmp(nx->string,$<pn>2->string)
					)
						error("%n of type%t redeclared as%k.",$<pn>2,$<pb>$,$<t>1);
				} else if ( $$->base != COBJ )
					error("%n of type%t redeclared as%k",$<pn>2,$<pb>$,$<t>1);
				check_tag();
			}
                | AGGR qualified_tname lt temp_inst_parms gt
                  { 
                    /* don't place the template on the instantiation list if it is
                     * a friend declaration or a foward declaration of a specialization
                     * friend class x<int> and struct x<int>; */

                    dont_instantiate = NEXTTOK()==SM;
		    Pexpr e=0;
		    if (dont_instantiate && in_friend == 0) {
			// *** this code could be used for forward declaration
			Ptempl t = templp->is_template($<pn>2);

			// must have seen a definition of template class
			if (t == 0) {
				error("explicitC instance of a nonYC%n",$<pn>2);
				error('i', "cannot recover from previous error" );
			}

			// make sure the explicit arguments are ok
			e = expr_unlist($<el>4);
			t->check_actual_args(e);

			// watch out for redefinition -- if exists,
			// use forward declaration or create instance
			pti = t->get_match(e,0,false);
			if ( pti ) { 
				if (pti->get_class()->class_base == INSTANTIATED)
					error("ZC%n multiply instantiated",$<pn>2);
			}
			else pti = new templ_inst(e,t,$<t>1);
			pti->inst_formals = t->get_formals();

			error('s',"forwardD of a specialized version ofY%n",$<pn>2);
			error('C',"\tclass %n",pti->get_tname()); 
			error('c'," -- did you mean a general forward declaration of theY?\n");
			error('C',"\tif so, use:  template <formal-parameters> class %n;\n",$<pn>2);
	            }

		    if (!e)
		    {
		        e = expr_unlist($<el>4);
                    }
                    Pname p = parametrized_typename($<pn>2, e, in_friend);
                    dont_instantiate = 0;
                    $<pb>$ = (Pbase)p->tp;
                    check_tag();
                  }
        	|  AGGR DECL_MARKER 
			{
				goto aggrcheck;
			}
		;


inline_mem_defs	:  /* empty */
		|  inline_mem_defs inline_fct_def
		;

base_list	:  COLON base_unit_list		{ $$ = $2; }
		|  %prec EMPTY			{ $$ = 0; }
		;

base_unit_list	:  base_unit
		|  base_unit_list CM base_unit
			{	if ($3) { $$ = $3; $<pbc>$->next = $1; } }
		;

base_unit	:  ttag			{ $$ = dobase(0,$<pn>1); }
		|  PR ttag		{ $$ = dobase($<t>1,$<pn>2); }
		|  TYPE ttag		{ $$ = dobase(0,$<pn>2,$<t>1); }
		|  PR TYPE ttag		{ $$ = dobase($<t>1,$<pn>3,$<t>2); }
		|  TYPE PR ttag		{ $$ = dobase($<t>2,$<pn>3,$<t>1); }
		;

class_head	:  AGGR LC	
			{
				Pname n = start_cl($<t>1,0,0);
				PUSH_CLASS_SCOPE(n);//SYM
				ccl->k_tbl = Ctbl;//SYM
				$$ = Pbase(n->tp);
				parsing_class_members = 1;
				//SYM -- tn stuff removed
				in_class_decl++;
				SAVE_STATE();
			}

		|  AGGR tag base_list LC
			{
				Pname n = start_cl($<t>1,$<pn>2,$<pbc>3);
				PUSH_CLASS_SCOPE(n);//SYM
				ccl->k_tbl = Ctbl;//SYM
				$$ = Pbase(n->tp);
				parsing_class_members = 1;
				//SYM -- tn stuff removed
				in_class_decl++;
				SAVE_STATE();
			}
		|  AGGR qualified_tname lt temp_inst_parms gt base_list LC
                 	{
			// LC increments bl_level by 1
			if ( bl_level > 1 )
			     error('s', "specializedY%n not at global scope",$<pn>2);
			explicit_template_definition = 1;
			Ptempl t = templp->is_template($<pn>2);

			// must have seen a definition of template class
			if (t == 0 || !t->defined) {
				error("YC%n must be defined prior to an explicitC instance",$<pn>2);
				error('i', "cannot recover from previous error" );
			}

			// make sure the explicit arguments are ok
			Pexpr e = expr_unlist($<el>4);
			t->check_actual_args(e);

			// watch out for redefinition -- if exists,
			// use forward declaration or create instance
			pti = t->get_match(e,0,false);
			if ( pti ) { 
				if (pti->get_class()->class_base == INSTANTIATED)
					error("ZC%n multiply instantiated",$<pn>2);
			}
			else pti = new templ_inst(e,t,$<t>1);

			Pname n = start_cl($<t>1,pti->get_tname(),$<pbc>6);
                        Pbase(n->tp)->b_name->n_redefined = 1;
			PUSH_CLASS_SCOPE(n);//SYM
			ccl->k_tbl = Ctbl;//SYM
			$$ = Pbase(n->tp);
			parsing_class_members = 1;
			in_class_decl++;
			SAVE_STATE();
			}
		;

tag		:  ID	{ $$ = $1; }
		|  qualified_tname { $$=$1; }
		;

ttag		:  ID	{ $$ = $1; }
		|  tname { $$=$1; }
		;

cl_mem_list	:  cl_mem_list cl_mem
			{
				if ($2) {
					if ($1)
						$<nl>1->add_list($<pn>2);
					else
						$<nl>$ = new nlist($<pn>2);
				}
				in_friend = 0;
			}
		|  %prec EMPTY { $$ = 0; }
		|  cl_mem_list template
			{
// error('d', "ZizedTD must be atG, notC scope" );
				if (must_be_friend) {
					error("non-friend ZizedTD must be atG, notC scope" );
					error('i', "cannot recover from previous error" );
				}
  				templ_friends = new cons(templp->parsed_template,templ_friends);
				templp->parsed_template = 0;
			}
		;

cl_mem		:  data_dcl
		|  att_fct_def SM
			{
				fct_friend1:
				if (in_friend &&
				    $<pn>1 &&
				    $<pn>1->n_qualifier &&
				    $<pn>1->n_qualifier->n_template_arg != template_type_formal)
					UNSET_SCOPE();
			}
		|  fct_def SM
			{
				goto fct_friend1;
			}
		|  fct_def
			{
				fct_friend2:
				if (in_friend &&
				    $<pn>1 &&
				    $<pn>1->n_qualifier &&
				    $<pn>1->n_qualifier->n_template_arg != template_type_formal)
					UNSET_SCOPE();
			}
		|  att_fct_def
			{
				goto fct_friend2;
			}
		|  fct_dcl
		|  PR COLON
			{	$$ = new name;
				$<pn>$->base = $<t>1;
			}
	 	|  scope_qualifiers fname SM
			{	Pname n = Ncopy($<pn>2);
				if (n->n_oper == TYPE) {
					error('s',"visibilityD for conversion operator");
					// n->tp = Ptype(n->n_initializer);
					n->tp = Ptype(n->cond);
					n->cond = 0;
					// n->n_initializer = 0;
					n->n_oper = 0;
					sig_name(n);
				}
				n->n_qualifier = $<pn>1;
				n->base = PR;
				$$ = n;
				if ( $<pn>1 && $<pn>1->n_template_arg != template_type_formal )
					UNSET_SCOPE();//SYM
			}
		;

/************* declarators:	returns Pname **********************/
/*	a ``decl'' is used for function and data declarations,
		and for member declarations
		(it has a name)
	an ``arg_decl'' is used for argument declarations
		(it may or may not have a name)
	an ``cast_decl'' is used for casts
		(it does not have a name)
	a ``new_decl'' is used for type specifiers for the NEW operator
		(it does not have a name, and PtoF and PtoV cannot be expressed)
*/

fname		:  ID { $$ = $<pn>1; }
		|  COMPL TNAME  /* qualified_tname? */
			{	$$ = Ncopy($<pn>2);
				$<pn>$->n_oper = DTOR;
			}
		|  OPERATOR oper
			{	$$ = new name(oper_name($2));
				$<pn>$->n_oper = $<t>2;
			}
		|  OPERATOR c_type
			{	Pname n = $<pn>2;
				n->string = "_type";
				n->n_oper = TYPE;
				n->cond = Pexpr(n->tp);
				// n->n_initializer = Pexpr(n->tp);
				n->tp = 0;
				$$ = n;
			}
		;

oper		:  PLUS
		|  MINUS
		|  MUL
		|  AND
		|  OR
		|  ER
		|  SHIFTOP
		|  EQUOP
		|  DIVOP
		|  RELOP
		|  LT
		|  GT
		|  ANDAND
		|  OROR
		|  LP RP	{	$$ = CALL; }
		|  LB RB	{	$$ = DEREF; }
		|  NOT
		|  COMPL
		|  ICOP
		|  ASOP
		|  ASSIGN
		|  NEW		{	$$ = NEW; --in_new; }
		|  DELETE	{	$$ = DELETE; }
		|  REF		{	$$ = REF; }
		|  CM		{	$$ = CM; }
		|  REFMUL	{	$$ = REFMUL;
					if ($<t>1 == DOT) error(".* cannot be overloaded");
				}
		;

scope_qualifiers:  tn_list { $<pn>$ = SET_SCOPE($<pn>1); }/*SYM*/
		;

tn_list		:  tscope
		/*XXX*/	{   if ( $<pn>1 != sta_name ) {
				Pname n = $<pn>1;
				char *str = 0, *str2 = 0, *s = n->string;
				if (n->n_template_arg != template_type_formal) {
				    n = n->tp->is_cl_obj();
				    if ( n ) str = s = get_classname(n->string);
				}
				if ( n && NEXTTOK() == TNAME
				&& strcmp(s,str2=get_classname(yylval.pn->string))==0){
					// ctor -- change to ID to avoid
					//    parsing as type spec
					yychar = ID;
					yylval.pn = Ncopy(yylval.pn);
					yylval.pn->n_oper = TNAME;
				}
				if ( str ) delete [] str;
				if ( str2 ) delete [] str2;
			    }
			    $<pn>$ = $<pn>1;
			}
		|  tn_list tscope /*SYM*/
			{//SYM
			   $<pn>$ = $<pn>2;
			   if ( $<pn>2 == sta_name ) {
			   	error("scope qualifier syntax");
			   } else {
// error('d',"tn_list: tn_list tscope: pn2: %s", $<pn>2->string);
				Pname cn = $<pn>2;
				char *str = 0, *str2 = 0, *s = cn->string;
				if (cn->n_template_arg != template_type_formal){
				    cn = $<pn>2->tp->is_cl_obj();
				    if (cn) str = s = get_classname(cn->string);
				}
				if ( cn && NEXTTOK() == TNAME
				&& strcmp(s,str2=get_classname(yylval.pn->string))==0){
					// ctor -- change to ID to avoid
					//    parsing as type spec
					yychar = ID;
					yylval.pn = Ncopy(yylval.pn);
					yylval.pn->n_oper = TNAME;
				}
				if ( str ) delete [] str;
				if ( str2 ) delete [] str2;
				if ( $<pn>1 != sta_name
				&&   $<pn>1->n_template_arg != template_type_formal){
				    Pname cx = $<pn>1->tp->is_cl_obj();
				    if ( cx ) // cx::cn::
					if ( check_if_base(Pclass(cx->tp),Pclass(cn->tp)) )
						error("%n ::%n :: --%n not aM of%n",cx,cn,cn,cx);
				}
			    }
			}
		;

qualified_tname	: tn_list TNAME
			{	
				$<pn>$ = $<pn>2;
				if (in_typedef && in_typedef->base == 0)
					in_typedef->defined = TNAME_SEEN;
				//xxx qualifier currently ignored...
				// $<pn>$ = Ncopy( $<pn>2 );
				// $<pn>$->n_oper = TNAME;
				// $<pn>$->n_qualifier = $<pn>1;
			}
		| TNAME
			{	
				$<pn>$ = $<pn>1;
				if (in_typedef && in_typedef->base == 0)
					in_typedef->defined = TNAME_SEEN;
			//	$<pn>$ = Ncopy( $<pn>1 );
			//	$<pn>$->n_oper = TNAME;
			}
		;

fct_attributes	: /* empty */
			{ $$ = 0; }
		| fct_attributes TYPE
 			{ /* const/volatile function */
				switch ( $<t>2 ) {
				case VOLATILE:
					error('s',"volatile functions");
					break;
				case CONST:
					$$ = ($1 | 1);
					break;
				default:
					if ( NEXTTOK() != SM
					&&   yychar != COLON
					&&   yychar != LC ) {
						la_backup(yychar,yylval);
						yylval.t = $<t>2;
						la_backup(TYPE,yylval);
						yylval.t = SM;
						yychar = SM;
						error("syntax error: unexpected%k (did you forget a `;'?)",$<t>2);
					} else error("FD syntax: unexpected%k",$<t>2);
					break;
				}
 			}
		| fct_attributes THROW LP type_list RP
			{ $$ = $1; }
		;

type_list	:  tag  { $$ = 0; }
		|  type_list CM tag
			{ $$ = 0; }
		;

decl		:  decl arg_list
			{	Freturns($2) = $<pn>1->tp;
				$<pn>1->tp = $<pt>2;
			}
 		|  decl LP RP fct_attributes
 			{ /* function with no argument */
 				$<pn>1->tp = new fct($<pn>1->tp,0,1);
 				Pfct($<pn>1->tp)->f_const = ($<i>4 & 1);
 			}
		|  tname arg_list
			{	Pname n = $<pn>1;
				$$ = Ncopy(n);
				//??? what if tname is qualified ???
				//SYM -- change -- do not if ctor def...
				if ( !in_typedef
				&&  (ccl==0 || strcmp(n->string,ccl->string)) )
					n->hide();
				$<pn>$->n_oper = TNAME;
				Freturns($2) = $<pn>$->tp;
				$<pn>$->tp = $<pt>2;
			}
		|  decl arg_lp elist RP
			/*	may be class object initializer,
				class object vector initializer,
				if not elist will be a CM or an ID
			*/
			{	
				$<pn>1->tp = new fct($<pn>1->tp,$<pn>3,1); 
 				in_arg_list = 0;
 				//SYM end_al($2,0);
 				POP_SCOPE();//SYM
				//RESTORE_STATE();
			}
		|  tname LP MUL ID RP arg_list
			{
				Pptr p = new ptr( PTR, 0 );
				Ptyp(p) = new basetype(TYPE,$<pn>1);
				Freturns( $6 ) = Ptype(p);
				$<pn>4->tp = $<pt>6;
			//SYM -- insert in table if not done elsewhere...
				if ( $<pn>4->n_oper != TNAME && !in_typedef ) {
					insert_name($<pn>4,Ctbl);
				}
				$$ = $4;
				if (DECL_TYPE == -1) DECL_TYPE = 0;
			}
		|  tname LP AND ID RP arg_list
			{
				Pptr p = new ptr( RPTR, 0 );
				Ptyp(p) = new basetype(TYPE,$<pn>1);
				Freturns( $6 ) = Ptype(p);
				$<pn>4->tp = $<pt>6;
			//SYM -- insert in table if not done elsewhere...
				if ( $<pn>4->n_oper != TNAME && !in_typedef ) {
					insert_name($<pn>4,Ctbl);
				}
				$$ = $4;
				if (DECL_TYPE == -1) DECL_TYPE = 0;
			}
		|  tname LP elist RP
			{	Pname n = $<pn>1;
				$$ = Ncopy($<pn>1);
				//??? what about qualified tname?
				//SYM -- change -- do not if ctor def...
			//XXXXX defer until name::normalize()?
				if ( !in_typedef
				&&  (ccl==0 || strcmp(n->string,ccl->string)) )
					n->hide();
				$<pn>$->n_oper = TNAME;
				$<pn>$->tp = new fct(0,$<pn>3,1);
			}
 		|  tname LP RP fct_attributes
 			{ /* function with no argument */
				Pname n = $<pn>1;
				$$ = Ncopy($<pn>1);
				//SYM -- change -- do not if ctor def...
				if ( !in_typedef
				&&  (ccl==0 || strcmp(n->string,ccl->string)) )
					n->hide();
 				$<pn>$->n_oper = TNAME;
 				$<pn>$->tp = new fct(0,0,1);
 				Pfct($<pn>$->tp)->f_const = ($<i>4 & 1);
			}
		|  tname LP MEMPTR decl RP arg_list
			{	memptrdcl($<pn>3,$<pn>1,$<pt>6,$<pn>4);
				$$ = $4;
			}
		|  fname
			{//SYM -- insert in table if appropriate...
			 //SYM    n_oper == TNAME => tname already hidden
			 //SYM    other !=0 values of n_oper => op or dtor
			 //SYM -- enter non-oper names as well as TNAMEs
			 //SYM    in parsing table to handle "1.5 namespace"
			 //SYM    in all scopes, since dcl does not build
			 //SYM    the complete table until after functions
			 //SYM    / classdefs are parsed.
			 //SYM friends are processed in norm.c
			 //SYM    to handle 'friend x;' etc
				if ( $<pn>1->n_oper == 0
				&&  !in_typedef && !in_friend ) {
				    //SYM do not hide ctor name...
				    if ( ccl==0 || parsing_class_members==0
				    ||   strcmp(ccl->string,$<pn>1->string)) {
					Pname n = new name; *n = *$<pn>1;
					insert_name(n,Ctbl);
				    }
				}
			}
		|  ID DOT fname
			{	$$ = Ncopy($<pn>3);
				//$<pn>$->n_qualifier = $1;
				error("`.' used for qualification; please use `::'");
			}
		|  scope_qualifiers fname
			{	$$ = $2;
				//SYM if ( $<pn>1 != sta_name ) {
    				//SYM	set_scope($<pn>1); 
					if ( $<pn>1 == sta_name 
    					&&   $<pn>$->n_oper==DTOR )
						error("bad syntax for destructor ::%n",$<pn>$);
    					$<pn>$->n_qualifier = $<pn>1;
				//SYM }
			}
	/*XXX*/	|  scope_qualifiers ID DOT fname
			{	$$ = Ncopy($<pn>4);
				//$<pn>$->n_qualifier = $<pn>1;//SYM $2;
				error("`.' used for qualification; please use `::'");
				error("non-type qualifier%n",$<pn>2);
				//if ( $<pn>1 != sta_name ) {
    				//SYM	set_scope($<pn>1); 
    				//	$<pn>2->n_qualifier = $<pn>1;
				//}
			}
	 	|  ptr decl	%prec MUL
			{	Ptyp($1) = $<pn>2->tp;
				$<pn>2->tp = $<pt>1;
				$$ = $2;
			}
		|  ptr tname	%prec MUL
			{	$$ = Ncopy($<pn>2);
				//??? what about qualified tnames?
				$<pn>$->n_oper = TNAME;
				// cannot evaluate at this point: defer until data_dcl
				if ( !in_typedef ) $<pn>2->hide();//SYM
				else in_tag = $<pn>2;//SYM???
				$<pn>$->tp = $<pt>1;
			}
		|  tname vec	%prec LB
			{	$$ = Ncopy($<pn>1);
				//??? what about qualified tnames?
				$<pn>$->n_oper = TNAME;
				if ( !in_typedef ) $<pn>1->hide();//SYM
				else in_tag = $<pn>1;//SYM???
				$<pn>$->tp = $<pt>2;
			}
		|  decl vec	%prec LB	
			{	Vtype($2) = $<pn>1->tp;
				$<pn>1->tp = $<pt>2;
			}
/*
		|  LP decl RP arg_list
			{
				Freturns($4) = $<pn>2->tp;
				$<pn>2->tp = $<pt>4;
				$$ = $2;
			}
		|  LP decl RP vec
			{	Vtype($4) = $<pn>2->tp;
				$<pn>2->tp = $<pt>4;
				$$ = $2;
			}
*/
		|  arg_lp decl RP 
			{ 
				$$ = $2; 
				in_arg_list = 0;
				hoist_al();//SYM end_al($1,0);
				//RESTORE_STATE();
			}
		;

arg_decl	:  ID
			{//SYM -- insert in table if not done elsewhere...
				if ( $<pn>1->n_oper != TNAME ) {
					Pname n = new name; *n = *$<pn>1;
					insert_name(n,Ctbl);
				}
				$$ = $<pn>1;
			}
		|  ptr qualified_tname	%prec MUL
			{	$$ = Ncopy($<pn>2);
				$<pn>$->n_oper = TNAME;
				$<pn>2->hide();
				$<pn>$->tp = $<pt>1;
			}
		|  %prec NO_ID
			{	
				$$ = new name; 
				NOT_EXPECT_ID();
			}
		|  ptr arg_decl		%prec MUL
			{	Ptyp($1) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$1;
				$$ = $2;
			}
		|  arg_decl vec		%prec LB
			{	Vtype($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
		|  arg_decl arg_list
			{	Freturns($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
/*
		|  LP arg_decl RP arg_list
			{	Freturns($4) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$4;
				$$ = $2;
			}
		|  LP arg_decl RP vec
			{	Vtype($4) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$4;
				$$ = $2;
			}
*/
		|  arg_lp arg_decl RP	
			{ 
// error('d', "arg_lp arg_decl rp in_arg_list: %d", in_arg_list );
				$$ = $2; 
				in_arg_list = 0;
				hoist_al();//SYM end_al($1,0);
				//RESTORE_STATE();
			}
		;

new_decl	:  %prec NO_ID
			{	$$ = new name; }
		|  ptr new_decl		%prec MUL
			{	Ptyp($1) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$1;
				$$ = $2;
                                NOT_EXPECT_ID();
			}
		|  new_decl vec		%prec LB
			{	Vtype($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
		;

cast_decl	:  %prec NO_ID { $$ = new name; }
		|  ptr cast_decl			%prec MUL
			{	Ptyp($1) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$1;
				$$ = $2;
				NOT_EXPECT_ID();
			}
		|  cast_decl vec			%prec LB
			{	Vtype($2) = $<pn>1->tp;
				$<pn>1->tp = (Ptype)$2;
			}
		|  LP cast_decl RP arg_list
			{	Freturns($4) = $<pn>2->tp;
				$<pn>2->tp = $<pt>4;
				$$ = $2;
			}
		|  LP cast_decl RP vec
			{	Vtype($4) = $<pn>2->tp;
				$<pn>2->tp = $<pt>4;
				$$ = $2;
			}
		;

c_decl		:  %prec NO_ID
			{	$$ = new name; }
		|  ptr c_decl				%prec MUL
			{	Ptyp($1) = $<pn>2->tp;
				$<pn>2->tp = (Ptype)$1;
				$$ = $2;
			}
		;



/***************** statements: returns Pstmt *****************/
stmt_list	:  /* empty */
			{
				$$ = 0; 
			}
		|  stmt_list TEMPLATE
			{
				error( "ZizedTD must be atG, not local scope" );
				error('i', "cannot recover from previous error" );
			}
         	|  stmt_list caselab_stmt
			{	
				if ($2)
					if ($1)
						$<sl>1->add($<ps>2);
					else {
						$<sl>$ =  new slist($<ps>2);
						stmt_seen = 1;
					}
			}
		;
caselab_stmt	:  caselablist statement
			{
				$$ = $2;
				if ($2)	stmt_seen = 1;
			}
		;

caselablist	: /* empty */
			{
				$$ = 0;
				check_decl();
			}
		;

condition	:  LP e RP
			{	$$ = $2;
			/*	if ($<pe>$ == dummy) error("empty condition");*/
				stmt_seen = 1;
			}
		;

block		:  LC
			{	PUSH_BLOCK_SCOPE();//SYM
				cd_vec[cdi] = cd;
				stmt_vec[cdi] = stmt_seen;
				++cdi;
				cd = 0;
				stmt_seen = 0;
				//SYM -- tn stuff removed
			}
			stmt_list RC
			{	Pname n = name_unlist(cd);
				Pstmt ss = stmt_unlist($<sl>3);
				$$ = new block($<l>1,n,ss,$<l>4);
				//SYM -- tn stuff removed
				cd = cd_vec[--cdi];
				stmt_seen = stmt_vec[cdi];
				if (cdi < 0) error('i',"block level(%d)",cdi);
				NOT_EXPECT_ID();
				$<ps>$->k_tbl = Ctbl;//SYM
                        	POP_SCOPE();//SYM
			}
		|  LC RC
			{ $$ = new block($<l>1,0,0,$<l>2); NOT_EXPECT_ID();
			  if ( Ctbl->k_id == ARG ) POP_SCOPE();//SYM
			}
		|  LC error RC
			{ $$ = new block($<l>1,0,0,$<l>3); NOT_EXPECT_ID();
			  if ( Ctbl->k_id == ARG ) POP_SCOPE();//SYM
			}
		;

simple		:  ee
			{	$$ = new estmt(SM,curloc,$<pe>1,0);	}
		|  BREAK
			{	$$ = new stmt(BREAK,$<l>1,0); }
		|  CONTINUE
			{	$$ = new stmt(CONTINUE,$<l>1,0); }
		|  GOTO ID
			{	$$ = new lstmt(GOTO,$<l>1,$<pn>2,0); }
		|  DO { stmt_seen=1; } caselab_stmt WHILE condition
			{	$$ = new estmt(DO,$<l>1,$<pe>5,$<ps>3); }
		|  ASM LP STRING RP
			{	
				if (stmt_seen)
					$$ = new estmt(ASM,curloc,(Pexpr)$<s>3,0);
				else {
					Pname n = new name(make_name('A'));
					n->tp = new basetype(ASM,(Pname)$<s>3);
					if (cd)
						cd->add_list(n);
					else
						cd = new nlist(n);
					$$ = 0;
				}
			}
		;

sm		: {
			if ( NEXTTOK() != SM ) {
				error("`;' missing afterS");
				la_backup(yychar,yylval);
				yychar = SM;
			}
		  } SM
		;

statement	:  simple sm
		|  SM
			{	$$ = new estmt(SM,curloc,dummy,0); }
		|  RETURN e SM
			{	$$ = new estmt(RETURN,$<l>1,$<pe>2,0); }
		|  TYPE STRING block
			{
				error("local linkage specification");
				$$ = $<pn>3;
			}
		|  data_dcl
			{	Pname n = $<pn>1;
				if (n) {
//error('d',"adding local dcl of%n%t ll %d in_typedef%t",n,n->tp,n->lex_level,in_typedef);
					if (stmt_seen) {
						$$ = new block(n->where,n,0);
						$<ps>$->base = DCL;
						$<ps>$->k_tbl = Ctbl;//SYM
					}
					else {
						if (cd)
							cd->add_list(n);
						else
							cd = new nlist(n);
						$$ = 0;
					}
				} // if n
				else if (stmt_seen) {
					$$ = new block(curloc,0,0);
					$<ps>$->base = FDCL;
				}
			}
		|  att_fct_def
			{
				Pname n = $<pn>1;
				if (!templp->in_progress)
					error(&n->where,"%n's definition is nested (did you forget a ``}''?)",n);
				if (cd)
					cd->add_list(n);
				else
					cd = new nlist(n);
				$$ = 0;
			}
		|  block
		|  IF condition caselab_stmt
			{	$$ = new ifstmt($<l>1,$<pe>2,$<ps>3,0); }
		|  IF condition caselab_stmt ELSE caselab_stmt
			{	$$ = new ifstmt($<l>1,$<pe>2,$<ps>3,$<ps>5); }
		|  WHILE condition caselab_stmt
			{	$$ = new estmt(WHILE,$<l>1,$<pe>2,$<ps>3); }
		|  FOR LP { stmt_seen=1; } caselab_stmt e SM e RP caselab_stmt
			{	$$ = new forstmt($<l>1,$<ps>4,$<pe>5,$<pe>7,$<ps>9); }
		|  SWITCH  { scd[++scdp] = cd;} condition caselab_stmt
			{	
				--scdp;
				$$ = new estmt(SWITCH,$<l>1,$<pe>3,$<ps>4); 
			}
		|  ID COLON { $$ = $1; stmt_seen=1; } caselab_stmt
			{	Pname n = $<pn>3;
				$$ = new lstmt(LABEL,n->where,n,$<ps>4);
			}
		|  TNAME COLON { $$ = new name($<pn>1->string); stmt_seen=1; } caselab_stmt
			{	Pname n = $<pn>3;
				$$ = new lstmt(LABEL,n->where,n,$<ps>4);
			}
		|  CASE { stmt_seen=1; } e COLON caselab_stmt
			{	
				if (scdp>=0 && scd[scdp]!=cd && cd && decl_with_init(cd)) error("jump past initializer (did you forget a '{ }'?)");
				if ($<pe>3 == dummy) error("empty case label");
				$$ = new estmt(CASE,$<l>1,$<pe>3,$<ps>5);
			}
		|  DEFAULT COLON { stmt_seen=1; } caselab_stmt
			{	
				if (scdp>=0 && scd[scdp]!=cd && cd && $<pe>3 && decl_with_init(cd)) error("jump past initializer (did you forget a '{ }'?)");
				$$ = new stmt(DEFAULT,$<l>1,$<ps>4); 
			}
		|  TRY block handler_list
			{ $$ = new handler( $<ps>2, stmt_unlist($<sl>3) ); }
		;


handler_list	:  /* empty */
			{ $$ = 0; }
		|  handler_list handler
			{	
				if ($2)
					if ($1)
						$<sl>1->add($<ps>2);
					else {
						$<sl>$ =  new slist($<ps>2);
						stmt_seen = 1;
					}
			}
		;

handler		:  CATCH exception_type block
			{
			    if ( $2 ) {
				$2->n_list = $<ps>3->d;
				$<ps>3->d = $2;
			    }
			    $$ = $<ps>3;
			}
		;

/* enter arg scope and don't exit so block will absorb exception declaration */
exception_type	:  arg_lp type arg_decl RP
			{	in_arg_list = 0;
				$$ = Ndata($2,$<pn>3);
				if ( $$->string == 0 )
					$$ = 0;
				else
					$$->base = CATCH;
			}
		|  LP ELLIPSIS RP
			{ $$ = 0; }
		;



/********************* expressions: returns Pexpr **************/
elist		: ex_list
			{	Pexpr e = expr_unlist($<el>1);
				while (e && e->e1==dummy) {
					register Pexpr ee2 = e->e2;
					if (ee2) error("EX inEL");
					delete e;
					e = ee2;
				}
				$$ = e;
			}
		;

ex_list		:  initializer		%prec CM
			{	$<el>$ = new elist(new expr(ELIST,$<pe>1,0)); }
		|  ex_list CM initializer
			{	$<el>1->add(new expr(ELIST,$<pe>3,0)); }
		;

initializer	:  e				%prec CM
		|  LC elist RC
			{
  			  	if ( in_arg_list ) 
       			  		error( "syntax error: IrL not permitted in AL" );
				else if ( in_binit_list )
					error( "syntax error: IrL not permitted inMIr" );
  			  	else 
					init_seen = 1;
				Pexpr e;
				if ($2)
					e = $<pe>2;
				else
					e = new expr(ELIST,dummy,0);
				$$ = new expr(ILIST,e,0);
			}
		;

ee		:  ee ASSIGN ee
			{	bbinop:	$$ = new expr($<t>2,$<pe>1,$<pe>3); }
		|  ee PLUS ee	{	goto bbinop; }
		|  ee MINUS ee	{	goto bbinop; }
		|  ee MUL ee	{	goto bbinop; }
		|  ee AND ee	{	goto bbinop; }
		|  ee OR ee	{	goto bbinop; }
		|  ee ER ee	{	goto bbinop; }
		|  ee SHIFTOP ee	{ 	goto bbinop; }
		|  ee EQUOP ee	{	goto bbinop; }
		|  ee DIVOP ee	{	goto bbinop; }
		|  ee RELOP ee	{	goto bbinop; }
		|  ee GT ee	{	goto bbinop; }
		|  ee LT ee	{	goto bbinop; }
		|  ee ANDAND ee	{	goto bbinop; }
		|  ee OROR ee	{	goto bbinop; }
		|  ee ASOP ee	{	goto bbinop; }
		|  ee CM ee	{	goto bbinop; }
		|  ee QUEST ee COLON ee
			{	$$ = new qexpr($<pe>1,$<pe>3,$<pe>5); }
		|  ee REFMUL ee
			{	$$ = new expr($<t>2,$<pe>1,$<pe>3); }
		|  DELETE term 
			{ $$ = new expr(DELETE,$<pe>2,0); }
		|  DELETE LB e RB term
			{
				if($<pe>3 != dummy) {
					if ( warning_opt || strict_opt )
						error(strict_opt?0:'w',"v in `delete[v]' is redundant; use `delete[] instead (anachronism)");
				}
				$$ = new expr(DELETE,$<pe>5,$<pe>3);
			}
		|  MEM DELETE term 
			{	$$ = new expr(GDELETE,$<pe>3,0); }
		|  MEM DELETE LB e RB term
			{
				if($<pe>4 != dummy) { 
					if ( warning_opt || strict_opt )
						error(strict_opt?0:'w',"v in `::delete[v]' is redundant; use `::delete[] instead (anachronism)");
				}
				$$ = new expr(DELETE,$<pe>6,$<pe>4);
			}
		|  term
		|  THROW term
			{ $$ = dummy; }
		;

e		:  e ASSIGN e
			{	binop:	$$ = new expr($<t>2,$<pe>1,$<pe>3); }
		|  e PLUS e	{	goto binop; }
		|  e MINUS e	{	goto binop; }
		|  e MUL e	{	goto binop; }
		|  e AND e	{	goto binop; }
		|  e OR e	{	goto binop; }
		|  e ER e	{	goto binop; }
		|  e SHIFTOP e	{ 	goto binop; }
		|  e EQUOP e	{	goto binop; }
		|  e DIVOP e	{	goto binop; }
		|  e RELOP e	{	goto binop; }
		|  e LT e	{	goto binop; }
		|  e GT e	{	goto binop; }
		|  e ANDAND e	{	goto binop; }
		|  e OROR e	{	goto binop; }
		|  e ASOP e	{	goto binop; }
		|  e CM e	{	goto binop; }
		|  e QUEST e COLON e
			{	$$ = new qexpr($<pe>1,$<pe>3,$<pe>5); }
		|  e REFMUL e
			{	$$ = new expr($<t>2,$<pe>1,$<pe>3); }
		|  DELETE term 
			{	$$ = new expr(DELETE,$<pe>2,0); }
		|  DELETE LB e RB term
			{
				if($<pe>3 != dummy) {
					if ( warning_opt || strict_opt )
						error(strict_opt?0:'w',"v in `delete[v]' is redundant; use `delete[] instead (anachronism)");
				}
				$$ = new expr(DELETE,$<pe>5,$<pe>3);
			}
		|  MEM DELETE term 
			{	$$ = new expr(GDELETE,$<pe>3,0); }
		|  MEM DELETE LB e RB term
			{
				if($<pe>4 != dummy) {
					if ( warning_opt || strict_opt )
						error(strict_opt?0:'w',"v in `::delete[v]' is redundant; use `::delete[] instead (anachronism)");
				}
				$$ = new expr(DELETE,$<pe>6,$<pe>4);
			}
		|  term { 
			init_seen = 0; 
			} 
		|  THROW term
			{ $$ = dummy; }
		|  %prec NO_EXPR
			{	$$ = dummy; }
		; 

term		:  NEW cast_type	{ goto new1; }
		|  NEW new_type
			{	new1:
				Ptype t = $<pn>2->tp;
				$$ = new texpr(NEW,t,0);
				--in_new;
			}
 		|  MEM NEW cast_type	{ goto new3; }
		|  MEM NEW new_type
 			{	new3:
				Ptype t = $<pn>3->tp;
 				$$ = new texpr(GNEW,t,0);
				--in_new;
 			}
		|  term ICOP
			{	$$ = new expr($<t>2,$<pe>1,0); }
		|  cast_type term %prec ICOP
			{
				$$ = new texpr(CAST,$<pn>1->tp,$<pe>2);
			}
		|  MUL term
			{	$$ = new expr(DEREF,$<pe>2,0); }
		|  AND term
			{	$$ = new expr(ADDROF,0,$<pe>2); }
		|  MINUS term
			{	$$ = new expr(UMINUS,0,$<pe>2); }
		|  PLUS term
			{	$$ = new expr(UPLUS,0,$<pe>2); }
		|  NOT term
			{	$$ = new expr(NOT,0,$<pe>2); }
		|  COMPL term
			{	$$ = new expr(COMPL,0,$<pe>2); }
		|  ICOP term
			{	$$ = new expr($<t>1,0,$<pe>2); }
		|  SIZEOF term
			{	
				$$ = new texpr(SIZEOF,0,$<pe>2); 
				--in_sizeof;
			}
		|  SIZEOF cast_type %prec SIZEOF
			{	
				$$ = new texpr(SIZEOF,$<pn>2->tp,0); 
				--in_sizeof;
			}
		|  term LB e RB
			{	$$ = new expr(DEREF,$<pe>1,$<pe>3); }
		|  term REF prim
			{	$$ = new ref(REF,$<pe>1,$<pn>3); }
		|  term REF MEMQ prim
			{
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(REF,$<pe>1,$<pn>4);
			}
		|  term REF MEMQ TNAME
			{	$<pn>4 = new name($<pn>4->string);
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(REF,$<pe>1,$<pn>4);
			}
		|  term REF dtorspec
			{	$$ = new ref(REF,$<pe>1,$<pn>3); }
		|  term REF scope_qualifiers prim
			{ // kluge to handle parameterized qualifiers, since
			  //    they are not included in MEMQ
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(REF,$<pe>1,$<pn>4);
				if ( $<pn>3 && $<pn>3->n_template_arg != template_type_formal )
					UNSET_SCOPE();
			}
		|  term REF scope_qualifiers TNAME
			{ // kluge to handle parameterized qualifiers, since
			  //    they are not included in MEMQ
				$<pn>4 = new name($<pn>4->string);
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(REF,$<pe>1,$<pn>4);
				if ( $<pn>3 && $<pn>3->n_template_arg != template_type_formal )
					UNSET_SCOPE();
			}
		|  term DOT prim
			{	$$ = new ref(DOT,$<pe>1,$<pn>3); }
		|  term DOT MEMQ prim
			{
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(DOT,$<pe>1,$<pn>4);
			}
		|  term DOT MEMQ TNAME
			{	$<pn>4 = new name($<pn>4->string);
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(DOT,$<pe>1,$<pn>4);
			}
		|  term DOT dtorspec
			{	$$ = new ref(DOT,$<pe>1,$<pn>3); }
		|  term DOT scope_qualifiers prim
			{ // kluge to handle parameterized qualifiers, since
			  //    they are not included in MEMQ
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(DOT,$<pe>1,$<pn>4);
				if ( $<pn>3 && $<pn>3->n_template_arg != template_type_formal )
					UNSET_SCOPE();
			}
		|  term DOT scope_qualifiers TNAME
			{ // kluge to handle parameterized qualifiers, since
			  //    they are not included in MEMQ
				$<pn>4 = new name($<pn>4->string);
				$<pn>4->n_qualifier = $<pn>3;
				$$ = new ref(DOT,$<pe>1,$<pn>4);
				if ( $<pn>3 && $<pn>3->n_template_arg != template_type_formal )
					UNSET_SCOPE();
			}
		|  prim
		|  scope_qualifiers prim
			{ // set scope to parse 'C::operator N' where N is in C
			  // still does not handle 'p->operator N'
			  //     (requires either fancier structures or 
			  //     on-the-fly type checking)
				$$ = Ncopy($<pn>2);
				$<pn>$->n_qualifier = $<pn>1;
				if ( $<pn>1 && $<pn>1->n_template_arg != template_type_formal )
					UNSET_SCOPE();
			}
		|  tn_list COMPL tag	/* allow explicit call of destructor */
			{
				$$ = dummy_dtor();
				$<pn>$->n_qualifier = $<pn>1;
				$<pn>$->n_dtag = $<pn>3;
			}
		|  tn_list COMPL TYPE	/* explicit call to basic type dtor */
			{	
				$$ = dummy_dtor( $<t>3, $<t>3 );
				$<pn>$->n_qualifier = $<pn>1;
			}
		|  term_elist 
			{
			if ( init_seen )
     				error( "syntax error:IrL illegal within ()");
			}

		|  term_lp e RP
			{
				if ( $2 == dummy )
					error("syntax error: nullE");
				$$ = $2;
			}
		|  ZERO
			{	$$ = zero; }
		|  ICON
			{	$$ = new expr(ICON,0,0);
				$<pe>$->string = copy_if_need_be($<s>1);
			}
		|  FCON
			{	$$ = new expr(FCON,0,0);
				$<pe>$->string = copy_if_need_be($<s>1);
			}
		|  STRING
			{	$$ = new expr(STRING,0,0);
				$<pe>$->string = copy_if_need_be($<s>1);
			}
		|  CCON
			{	$$ = new expr(CCON,0,0);
				$<pe>$->string = copy_if_need_be($<s>1);
			}
		|  THIS
			{	$$ = new expr(THIS,0,0); }
		;

dtorspec	:  COMPL tag /* explicit, unqualified dtor call */
			{	
				$$ = dummy_dtor();
				$<pn>$->n_dtag = $<pn>2; // checked later
			}
		|  MEMQ COMPL tag /* explicit dtor call */
			{	
				$$ = dummy_dtor();
				$<pn>$->n_qualifier = $<pn>1; // checked later
				$<pn>$->n_dtag = $<pn>3; // checked later
			}
		|  TYPE MEM COMPL TYPE /* call of basic type dtor */
			{ $$ = dummy_dtor($<t>1,$<t>4); }
		|  COMPL TYPE /* call of basic type dtor */
			{ $$ = dummy_dtor($<t>2,$<t>2); }
		|  TYPE MEM COMPL tag	/* call to basic type dtor */
			{	
				$$ = dummy_dtor( $<t>1, $<t>1 );
				$<pn>$->n_dtag = $<pn>4;
			}
		|  MEMQ COMPL TYPE
			{
				$$ = dummy_dtor( $<t>3, $<t>3 );
				$<pn>$->n_qualifier = $<pn>1;
			}
		|  tn_list COMPL tag
			{ // kluge to parse parameterized qualifiers after ./->
				$$ = dummy_dtor();
				$<pn>$->n_qualifier = $<pn>1;
				$<pn>$->n_dtag = $<pn>3;
			}
		|  tn_list COMPL TYPE	/* explicit call to basic type dtor */
			{ // kluge to parse parameterized qualifiers after ./->
				$$ = dummy_dtor( $<t>3, $<t>3 );
				$<pn>$->n_qualifier = $<pn>1;
			}
		;

term_elist	: TYPE LP elist RP
			{ 	$$ = new texpr(VALUE,tok_to_type($<t>1),$<pe>3); }
/*
		|  qualified_tname LP elist RP
*/
		|  tname LP elist RP
			{	
				$$ = new texpr(VALUE,$<pn>1->tp,$<pe>3); 
				if ($<pn>1->is_template_arg() && $<pn>1->tp->base == ANY) {
					$<pe>$->tp2 = new basetype(TYPE,$<pn>1);
				}
			}
                |  NEW term_lp elist RP cast_type    { goto new2; }
                |  NEW term_lp elist RP new_type     /* allow separate allocation */
			{	new2:
				Ptype t = $<pn>5->tp;
				$$=new texpr(NEW,t,0);
				$<pe>$->e2 = $<pe>3;
				--in_new;
			}
		|  MEM NEW term_lp elist RP cast_type	{ goto new4; }
		|  MEM NEW term_lp elist RP new_type	/* allow separate allocation */
			{	new4:
				Ptype t = $<pn>6->tp;
				$$ = new texpr(GNEW,t,0);
				$<pe>$->e2 = $<pe>4;
				--in_new;
			}
		|  term LP elist RP
			{	
				Pexpr ee = $<pe>3;
				Pexpr e = $<pe>1;
				if (e->base==NEW || e->base==GNEW)
					e->e1 = ee;
				else
					$$ = new call(e,ee);
			}

		;

ptname         : PTNAME lt temp_inst_parms  gt
                 {
			$<pn>$ = parametrized_typename($<pn>1,(expr_unlist($<el>3))); 
	 	 }
               ;

tscope		:  TSCOPE { $<pn>$ = $<pn>1; }
		|  MEM { $<pn>$ = sta_name; }
		|  ptname TSCOPE { $<pn>$ = $<pn>1; }
		;


prim		:  ID
			{
				if ( in_arg_list && !in_binit_list ) {
				    Pktab tb = Ctbl;
				    for (; tb && tb->k_id==ARG; tb=tb->k_next) {
					if ( tb->look($<pn>1->string,0) ) {
						error("illegalR toA%n in defaultA",$<pn>1);
						$<pe>1 = dummy;
						break;
					}
				    }
				}
				$$ = $<pn>1;
			}
		|  OPERATOR oper
			{	$$ = new name(oper_name($2));
				$<pn>$->n_oper = $<t>2;
			}
		|  OPERATOR c_type
			{	$$ = $2;
				sig_name($<pn>$);
			}
		;



/****************** abstract types (return type Pname) *************/
cast_type	:  term_lp type cast_decl RP 
			{
				$$ = Ncast($2,$<pn>3);
			}
		;

term_lp		:  LP { check_cast(); }
		;

c_tp		:  TYPE	
			{
				TOK t = $<t>1;

				switch (t) {
				case FRIEND:
				case OVERLOAD:
				case REGISTER:
				case STATIC:
				case EXTERN:
				case AUTO:
				case VIRTUAL:
					error("%k in operatorT",t);
					t = INT;
					
				}

				$$ = new basetype(t,0);

			}
		|  tname	{ $$ = new basetype(TYPE,$<pn>1); }
		|  c_tp TYPE		
			{ 
				if ( DECL_TYPE != -1 ) {
					switch ($<pb>1->base) { 
					case COBJ: case EOBJ:
						{
						Pbase bt;
						bt = new basetype(0,0);
						*bt = *$<pb>1;
						DEL($<pb>1);
						$<pb>1 = bt;
						}
					}
					$$ = $<pb>1->type_adj($<t>2); 
				}
				DECL_TYPE = 0;
			}
		|  c_tp tname
			{ 
				if ( DECL_TYPE != -1 ) 
			 		$$ = $<pb>1->name_adj($<pn>2);
				DECL_TYPE = 0;
			}
		;

c_type		:  c_tp c_decl	{ $$ = Ncast($1,$<pn>2); }
		;

new_type	:  type new_decl	{ $$ = Ncast($1,$<pn>2); };

arg_type	:  type arg_decl
			{	
                         //      ENTER_NAME($<pn>2);
				$$ = Ndata($1,$<pn>2); 
			}
                |  type arg_decl ASSIGN
                        {
                        //      ENTER_NAME($<pn>2);
                        }
                   initializer
                        {       
				$$ = Ndata($1,$<pn>2);
                                $<pn>$->n_initializer = $<pe>5;
                        }
                ;

arg_lp		:  LP 
			{	PUSH_ARG_SCOPE();//SYM
				//SAVE_STATE();
				in_arg_list=1; 
				check_decl(); 
				$$ = 0;
				//SYM -- tn stuff removed
			}
		;

arg_list	:  arg_lp arg_type_list ellipsis_opt RP fct_attributes
		    {
			$$ = new fct(0,name_unlist($<nl>2),$<t>3); 
			if ( NEXTTOK() != COLON ) in_arg_list=0;
			//in_arg_list=0;
			Pfct($<pt>$)->f_const = ($<i>5 & 1);
			//SYM -- removed kluge
                        POP_SCOPE();//SYM
			//RESTORE_STATE();
		    }
		;

arg_type_list	:  arg_type_list CM at
			{
				if ($3)
					if ($1)
						$<nl>1->add($<pn>3);
					else {
						error("AD syntax");
						$<nl>$ = new nlist($<pn>3); 
					}
				else
					error("AD syntax");
			}
		|  at	%prec CM
			{
				if ($1) $<nl>$ = new nlist($<pn>1); 
			}
		;

at		:  arg_type
		|  %prec EMPTY	{	$$ = 0; }
		;

ellipsis_opt	:  /* empty */
		    {	$$ = 1; }
		|  ELLIPSIS
		    {	$$ = ELLIPSIS; }
		|  CM ELLIPSIS
		    {	$$ = ELLIPSIS; }
		;

ptr		:  MUL %prec NO_ID
			{
			$$ = new ptr(PTR,0); 
			EXPECT_ID();
			}
		|  AND %prec NO_ID
			{
			$$ = new ptr(RPTR,0); 
			EXPECT_ID();
			}
		|  MUL TYPE %prec NO_ID
			{	$$ = doptr(PTR,$<t>2); }
		|  ptr TYPE %prec NO_ID
			{	
				switch ( $<t>2 ) {
				case CONST:
                                     $<pp>1->b_const = 1; break;
				case VOLATILE:
				     error('w',"\"volatile\" not implemented (ignored)");
				     break;
				default:
				    error( "syntax error: *%k", $<t>2 );
				}
				$$ = $<pp>1; 
			}
		|  AND TYPE %prec NO_ID
			{	$$ = doptr(RPTR,$<t>2); }
		|  ptname MEMPTR %prec NO_ID
			{	
				memptr_pn = $<pn>1;
				memptr_tok = 0;
				goto memptr1;
			}
		|  MEMPTR %prec NO_ID
			{
			memptr_pn = $<pn>1;
			memptr_tok = 0;
			memptr1:
			if (memptr_tok)
				$$ = doptr(PTR,memptr_tok);
			else
				$$ = new ptr(PTR,0);
			Pname n = memptr_pn;
			if (n->is_template_arg()==0) {
				if(n->tp->skiptypedefs()->base != COBJ) {
				    $<pp>$->memof = 0;
				    error("P toM of nonCT");
				}
				else $<pp>$->memof = n->tp->skiptypedefs()->classtype();
			}
			else {
				$<pp>$->memof = 0;
				$<pp>$->ptname = n;
			}
			EXPECT_ID();
			}
		|  ptname MEMPTR TYPE %prec NO_ID
			{	
				memptr_pn = $<pn>1;
				memptr_tok = $<t>3;
				goto memptr1;
			}
		|  MEMPTR TYPE %prec NO_ID
			{	
				memptr_pn = $<pn>1;
				memptr_tok = $<t>2;
				goto memptr1;
			}
		;

vec		:  LB e RB		{ $$ = new vec(0,$<pe>2!=dummy?$<pe>2:0 ); }
		|  NOT %prec LB		{ $$ = new vec(0,0); }
		;

%%

static Pname
enumcheck( Pname n )
{
	Ptype tx = n->tp;
	if ( tx->base == TYPE ) {
		Pname bn = Pbase(tx)->b_name;
		tx = bn->tp;
		if ( tx->base != EOBJ
		||   strcmp(bn->string,n->string)
		)
			error("%n ofT%t redeclared as enum.",n,tx);
		n = bn;
	} else if ( tx->base != EOBJ )
		error("%n ofT%t redeclared as enum",n,tx);

// error('d',"enumtag: ccl %t tag: %n", ccl, n);
	return n;
}

static void
check_tag()
/*
        Allow the case of inline/virtual/overload as 
        modifiers of return type of form struct/class/union x foo() 
        SM, COLON, LC ==> real class declaration, not return type
*/
{
	switch ( NEXTTOK() ) {
	case SM: case COLON: case LC:
    		declTag = 1;
    		break;
	default:
    		declTag = 0;
    		break;
        }
}

static void
hoist_al()
/*SYM hoist names in arg table to next outer scope and pop scope
 * called upon discovering that an arg list isn't really being parsed
 *    -- i.e., 'arg_lp decl RP ...' -- arg_lp pushed an arg table
 */
{
	if ( Ctbl->k_id != ARG ) {
		// saw something like '( X::y )' while probably reduced
		//    as 'arg_lp decl RP' and pushed scope of X
		// no names should have been entered; discard table
		Pktab otbl = Ctbl;
		Ctbl = GET_XSCOPE();
		if ( Ctbl->k_id != ARG ) error('i',"hoist_al: noA table");
		POP_SCOPE();
		SET_XSCOPE(Ctbl);
		Ctbl = otbl;
		return;
	}
	if ( !in_typedef && !in_friend ) Ctbl->hoist();
	POP_SCOPE();
}
static void
arg_redec( Pname fn )
/* parsing restored member inline at end of class
 * redeclare args before entering function
 */
{
	if ( fn==0 || fn->tp->base != FCT )
		error('i',"bad inline rewrite! --%n %t",fn,fn?fn->tp:0);
	if ( Ctbl->k_id != ARG )
		error('i',"arg_redec(%n ) -- noA table",fn);
	//SYM -- reenter in arg table
	//SYM -- probably more efficient to keep arg table around...
	Pname al = Pfct(fn->tp)->argtype;
	Pname n = 0;
	for ( ;  al;  al = al->n_list ) {
		DB( if(Ydebug>=1)error('d',"arg_redec:  %n  %d",al,al->lex_level); );
		n = new name(al->string);
		insert_name(n,Ctbl);
		DB( if(Ydebug>=1)error('d',"   %n",n); );
	}
}

static Pname
dummy_dtor( TOK q, TOK d )
{
	if ( q != d ) {
		error("syntax error: inconsistent destructor notation");
		q = d;
	}
	Pname dt = new name("type destructor");
	dt->base = DTOR;
	dt->tp = new fct(void_type,0,1);
	dt->n_dcl_printed = 1; // suppress any code generation
	switch ( d ) {
	default:
		error("syntax error: illegal destructor notation");
		dt->tp2 = any_type;
		break;
	case CHAR:
		dt->tp2 = dt->tpdef = char_type;
		break;
	case SHORT:
		dt->tp2 = dt->tpdef = short_type;
		break;
	case SIGNED:
	case INT:
		dt->tp2 = dt->tpdef = int_type;
		break;
	case UNSIGNED:
		dt->tp2 = dt->tpdef = uint_type;
		break;
	case LONG:
		dt->tp2 = dt->tpdef = long_type;
		break;
	case VLONG:
		dt->tp2 = dt->tpdef = vlong_type;
		break;
	case FLOAT:
		dt->tp2 = dt->tpdef = float_type;
		break;
	case DOUBLE:
		dt->tp2 = dt->tpdef = double_type;
		break;
	case VOID:
		dt->tp2 = dt->tpdef = void_type;
		break;
	}
	return dt;
}
static Pname
dummy_dtor()
{
	Pname dt = new name("type destructor");
	dt->base = DTOR;
	dt->tp = new fct(void_type,0,1);
	dt->n_dcl_printed = 1; // suppress any code generation
	dt->tp2 = 0;
	dt->tpdef = 0;
	return dt;
}

static bit
check_if_base( Pclass c1, Pclass c2 )
{
	if ( same_class(c1,c2) ) return 1;
	for (Pbcl b = c1->baselist; b; b=b->next) {
		if ( same_class(b->bclass,c2) ) return 1;
		if (check_if_base(b->bclass,c2)) return 1;
	}
	return 0;
}
