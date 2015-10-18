/*ident	"@(#)cls4:src/cfront.h	1.28" */
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

*	Here are all the class definitions for cfront, and most of the externs
*******************************************************************************/

#ifndef _CFRONT_H
#define _CFRONT_H

#include "token.h"
#include "typedef.h"

#ifndef GRAM
extern char* prog_name;		// compiler name and version
extern int inline_restr;	// inline expansion restrictions 
#endif

extern TOK	tlex();
extern Pname	syn();

extern void	ext(int);

extern Ptype unconst_type(Ptype);

extern char* 	make_name(TOK);
extern char* 	make_nested_name(char*,Pclass); 
extern void 	make_dummy();
extern Pname	dummy_fct;
extern Pname    really_dominate(Pname, Pname, bit);
extern int      exact1(Pname, Ptype);
extern int      exact2(Pname, Ptype);
extern int      exact3(Pname, Ptype);
extern char*	strdup(const char*);

extern int friend_check(Pclass start,Pclass stop, Pfct f);

struct loc		// a source file location
{
	short	file;	// index into file_name[], or zero
	short	line;
#ifndef GRAM
	void	put(FILE*);
	void	putline();
#endif
};

extern Loc curloc;
extern int curr_file;
extern char* curr_filename();
extern char* src_file_name;
extern loc noloc; // dummy null location
extern bit binary_val;
extern bit stmtno;
extern Ptable tmp_tbl;
extern Ptable bound_expr_tbl;

struct ea {	// fudge portable printf-like formatting for error()
	union {
		const void* p;
		long i;
	};

	ea(const void* pp) {p = pp;} 
	ea(long ii)   { i = ii; }
	ea() {}
};

extern ea* ea0;

int error(const char*);
int error(const char*, const ea&, const ea& = *ea0, const ea& = *ea0, const ea& = *ea0);
int error(int, const char*);
int error(int, const char*, const ea&, const ea& = *ea0, const ea& = *ea0, const ea& = *ea0);
int error(loc*, const char*, const ea& = *ea0, const ea& = *ea0, const ea& = *ea0, const ea& = *ea0);
int error(int, loc*, const char*, const ea& = *ea0, const ea& = *ea0, const ea& = *ea0, const ea& = *ea0);
extern char emode;

extern int error_count;
extern int vtbl_opt;
extern int debug_opt;
extern int warning_opt;
extern int ansi_opt;
extern int pt_opt;
extern int perf_opt;
extern int dtpt_opt;
extern int se_opt;
extern int strict_opt;
extern FILE* out_file;
extern FILE* pt_file;
extern FILE* dtpt_file;
extern char scan_started;

extern int bl_level;

//SYM Tables replacing the ktbl
extern Pktab Gtbl;	//SYM global parsing table
extern Pktab Ctbl;	//SYM current parsing table
	//SYM Ctbl is also set during dcl for accurate TNAME lookup.
	//SYM This is ok since parsing and dcl are disjoint operations.
extern Pname k_find_name( char*, Pktab, TOK );
extern Pname k_find_member( char*, Pclass, TOK );
extern Pname insert_type( Pname, Pktab, TOK );
extern Pname insert_name( Pname, Pktab );

extern Ptable gtbl;		// global names
extern Ptable ptbl;		
extern char* oper_name(TOK);
extern char* name_oper(char *);
extern int is_probably_temp(char*);
extern Pname def_name;
extern Pname pdef_name;
extern Pclass ccl;
extern Pbase defa_type;
extern Pbase moe_type;
extern Pbase mptr_type;

#ifndef GRAM
extern Pstmt Cstmt;		// current statement, or 0
extern Pname Cdcl;		// name currently being declared, or 0

extern int largest_int;
#endif

extern Pbase any_type;
extern Pbase int_type;
extern Pbase char_type;
extern Pbase short_type;
extern Pbase long_type;
extern Pbase vlong_type;
extern Pbase uint_type;
extern Pbase float_type;
extern Pbase double_type;
extern Pbase ldouble_type;
extern Pbase void_type;

#ifndef GRAM
extern Pbase uchar_type;
extern Pbase ushort_type;
extern Pbase ulong_type;
extern Pbase uvlong_type;
extern Ptype Pchar_type;
extern Ptype Pint_type;
extern Ptype Pvptr_type;
extern Ptype Pfctvec_type;
extern Ptype Pvoid_type;
extern Pbase zero_type;
extern Ptype size_t_type;

extern int byte_offset;
extern int bit_offset;
extern int max_align;
extern int const_save;
extern bit const_problem;
#endif

extern Pexpr dummy;	/* the empty expression */
extern Pexpr zero;
extern Pexpr one;
extern Pname sta_name;	/* qualifier for unary :: */

#define DEL(p) if (p && (p->permanent==0)) p->del()
#define PERM(p) p->permanent=1
#define UNPERM(p) p->permanent=0

struct node {
	TOK	base;
	bit	permanent;
        bit     baseclass; // base classes have NAME in their base ...
#ifdef DBG
	bit	displayed;	// avoid infinite recursion in display functions
	bit	allocated;	// set when not on free list
	long	id;
#endif
};
#ifdef DBG
extern long node_id;
extern int Adebug;
#define DBID() { node::id = ++::node_id; node::allocated=1; displayed=0; \
	if(Adebug>=1)fprintf(stderr,"\n*** allocated %d base %d\n",id,base); }
#else
#define DBID() /**/
#endif

struct table : public node {
/*	a table is a node only to give it a "base" for debugging */
	bit	init_stat;	/* ==0 if block(s) of table not simplified,
				   ==1 if simplified but had no initializers,
				   ==2 if simplified and had initializers.
				*/
	TOK	t_realbase; // is this part of a ktable?
	short	size;
	short	hashsize;
	short	free_slot;	/* next free slot in entries */
	Pname*	entries;
	short*	hashtbl;
	Pstmt	real_block;	/* the last block the user wrote,
				   not one of the ones cfront created
				*/
	Ptable	next;		/* table for enclosing scope */
	Pname	t_name;		/* name of the table */

	static Ptable table_free;
	void* operator new(size_t);
	void operator delete(void*,size_t);

	table(short, Ptable, Pname);
	~table();

	Pname	look(char*, TOK);
	Pname	insert(Pname, TOK);
	inline Pname	insert_copy(Pname, TOK);
	void	reinit();
#ifndef GRAM
	void	grow(int);
	void	set_name(Pname n)	{ t_name = n; };
	int	max()			{ return free_slot-1; };
	void	dcl_print(TOK,TOK);
#endif
	// return a pointer to the i'th entry, or 0 if it does not exist
	Pname get_mem(int i) { return (i<=0 || free_slot<=i) ? 0 : entries[i]; }
	void	del();
	char* whatami();
#ifdef DBG 
	void dump( int verbose = 1 );
#endif 
};
#define NEXT_NAME(tbl,n,i) (n = (n->n_tbl_list ? n->n_tbl_list : tbl->get_mem(++i)))
struct ktable : public node { //SYM parsing table
/*	a ktable is a node only to give it a "base" for debugging */
	union {
	Ptable k_t; // hash table
	Pname  k_n; // list of names for tiny tables such as arg lists
	};
	short k_size;   // if k_tiny, number of names in small table
	bit k_tiny;     // set when table is small (not hash table)
	TOK k_id;       // ARG CLASS BLOCK TEMPLATE 0

	Pktab	k_next; // table for enclosing scope 
	Pname   k_name; // name of table
	ktable(int, Pktab, Pname);
	~ktable();
	static Pktab table_free;
	void* operator new(size_t);
	void operator delete(void*,size_t);

	Pname	look(char*, TOK);
	Pname	insert(Pname, TOK);
	Pname	find_cn(char*); // find name of this class table
	void	expand(int);// expand tiny table to hash table
	void	hoist();    // hoist names in this table to enclosing table
	Pname get_mem(int);
	void	del();
	char* whereami(); // return name or description of table
};

#ifndef GRAM
extern bit Nold;
extern bit vec_const, fct_const;
#endif

//SYM -- tn stuff removed

// local class
extern Plist local_class; //SYM -- preserve for use in del.c
extern char *make_local_name(Ptype, Pname);

// nested type
extern Pname curr_fct; 

extern Pname start_cl(TOK, Pname, Pbcl);
extern void end_cl();
extern Pbase end_enum(Pname, nlist*);

/************ types : basic types, aggregates, declarators ************/

#ifndef GRAM
extern bit new_type;
extern Pname cl_obj_vec;
extern Pname eobj;
#endif


#define DEFINED 01	/* definition fed through ?::dcl() */
#define SIMPLIFIED 02	/* in ?::simpl() */
#define DEF_SEEN 04	/* definition seen, but not processed */
#define REF_SEEN 010	/* reference seen (classdef) */
			/*   used for class members in norm.c */
#define IN_ERROR 020
#define TNAME_SEEN 040  /* used in typedef TNAME name */
#define COPIED 0100	/* used by copy_walker in tree_copy.c */

enum Templ_type { VANILLA, FCT_TEMPLATE, CL_TEMPLATE, BOUND_TEMPLATE,
                	INSTANTIATED, UNINSTANTIATED };

struct type : public node {
	bit	defined;	/* flags DEF_SEEN, DEFINED, SIMPLIFIED, IN_ERROR
					not used systematically yet */
	bit	lex_level;
	Templ_type templ_base;  // CL_TEMPLATE(specialization) or BOUND_TEMPLATE
	Pclass  in_class; 	// nested type
	Pname	in_fct;
	char	*nested_sig;
	char    *local_sig;
	bit	b_const;
	bit	ansi_const;	// turns off constness when print ansi C

	char	*signature(char*,int=0);
	Ptype	tlist;
	bit	check(Ptype, TOK, bit=0);
	Ptype	mkconst();

	// encapsulate casts
	Pclass classtype(); 
	inline Penum enumtype(); 
	inline Ptype bname_type();
	inline Pname bname();
        inline bit is_templ_instance();

	Ptype	skiptypedefs();			// skip over typedef's
	Ptype	skiptypedefs(bit &isconst);	// same, but `OR' b_const's in isconst
#ifndef GRAM
	void	print();
	void	dcl_print(Pname);
	void	base_print();

	Pptr	is_ref();
	Pptr	is_ptr();
	Pptr	is_ptr_or_ref();
	bit	is_unsigned();
	void	dcl(Ptable);
	int	tsizeof(int = 0);
	bit	tconst();
	bit	is_const_object();
	TOK	set_const(bit);
	int	align();
	TOK	kind(TOK,TOK,bit=1);
	TOK	integral(TOK oo)	{ return kind(oo,'I'); };
	TOK	numeric(TOK oo)		{ return kind(oo,'N'); };
	TOK	num_ptr(TOK oo)		{ return kind(oo,'P'); };
	bit	vec_type();
	Ptype	deref();
	inline	Pptr addrof();
	inline	bit is_or_pts_to(TOK);
	Pfct	memptr();
#endif
	Pname	is_cl_obj();	/* sets cl_obj_vec */
	void	del();
};

struct enumdef : public type {	/* ENUM */
	bit	e_body;
	short	no_of_enumerators;
	unsigned short e_strlen;	// strlen(string) or strlen(local_sig)
	char*	string;		// name of enum
	Pname	mem;
	Pbase	e_type;		// type representing the enum
		enumdef(Pname n)	{ base=ENUM; mem=n; };
#ifndef GRAM
	void	print();
	void	dcl_print(Pname);
	void	dcl(Pname, Ptable);
	void	simpl();
#endif
};

struct velem {
	Pname	n;
	int	offset;
};

struct virt : public node {
	Pvirt	next;
	int     n_init;
	velem*	virt_init;	// vector of vtbl initializers (zero-terminated)
	Pclass	vclass;		// for class vclass
	char*	string;
	bit	is_vbase;	// vtable for virtual base
	bit	printed;
 	virt(Pclass cl, velem* v, char* s, bit flag, int ni) {base = XVIRT; vclass=cl; virt_init=v; string=s; is_vbase=flag; next=0; n_init = ni;}
};

enum { C_VPTR=1, C_XREF=2, C_ASS=4, C_VBASE=8, C_REFM=16 };

typedef class cons *Pcons;
typedef class basic_template *Ptempl_base;

struct classdef : public type {	/* CLASS */
        Templ_type  class_base; /* recognize template classes */
	bit	c_body;		/* print definition only once */
	TOK	csu;		/* CLASS, STRUCT, UNION, or ANON */
	bit	obj_align;
	bit	c_xref;
		// 1 set:	has vptr(s) 
		// 2 set:	X(X&) exists
		// 4 set:	operator=(X&) exists
		// 8 set:	has vbaseptr(s)
		// 16 set:	has reference data member(s)
		
	short	virt_count;	// number of virtual functions
				// starting at max base class virt_count in
	bit	virt_merge;	// set when no virtual functions, but
				// need to merge virtual base classes
	bit	has_vvtab;	// set if class has vtable from virtual base
	unsigned short c_strlen;// strlen(string) or strlen(local_sig)
	Pbcl	baselist;	// list of base classes
	char*	string;		/* name of class */
	Pname	c_abstract;	// abstract class: don't instantiate: virt func name 
	Pname	mem_list;
	Ptable	memtbl;
	Pktab	k_tbl; //SYM parsing table
	Ptable	c_context;
	int	obj_size;
	int	real_size;	/* obj_size - alignment waste */
 	Pcons	templ_friends;  /* list of template friends: template <class T> friend ... */
	Plist	friend_list;
	Pname	pubdef;
	Ptype	this_type;
	Pvirt	virt_list;	// vtbl initializers
	Pname	c_ctor;		// constuctor: possibly overloaded
	Pname	c_dtor;		// destructor
	Pname	c_itor;		/* constructor X(X&) */
	Pname	c_vtor;		/* x::x() ctor for vec_new */
	Pname	conv;		/* operator T() chain */
	struct toknode *c_funqf, *c_funqr; // token Q for parsing function defs after class def

	classdef(TOK);
	~classdef();
	TOK	is_simple()	{ return (csu==CLASS)?0:csu; };
        bit 	same_class(Pclass p,int=0);
#ifndef GRAM
	void	print();
	void	dcl_print(Pname);
	void	simpl();

	void	print_members();
	void	dcl(Pname, Ptable);
	void	make_vec_ctor(Pname);

//	bit	has_friend(Pname);
	bit	has_friend(Pclass);
	bit	has_friend(Pfct);

	bit	has_base(Pclass cl,int level=0,int ccflag=0);
	bit	baseof(Pname);
	bit	baseof(Pclass);
	Pclass	is_base(char*,TOK* =0,int level=0);

	Pname	has_oper(TOK);
	Pname	has_ctor()	{ return c_ctor; }
	Pname	has_dtor()	{ return c_dtor; }
	Pname	has_itor()	{ return c_itor; }
	Pname	has_vtor()	{ return c_vtor; }
	Pname	has_ictor();
	Pname	make_itor(int);

	Pexpr	find_name(char*, Pclass, int=0, int=0);
	int	do_virtuals(Pvirt, char*, int, bit);
	int	all_virt(Pclass, char*, int, bit);
	void	add_vtbl(velem*, char*, bit, int);
	void	print_all_vtbls(Pclass);
	void	print_vtbl(Pvirt);
	void	really_print(Pvirt);
	int	check_dup(Pclass, TOK);
	int	has_allocated_base(Pclass,bit=0);
	char 	*has_allocated_base(char*);
	int	get_offset(char*,bit=0);
	Pbcl	get_base(char*);
	Pexpr	get_vptr_exp(char*);
	Pexpr	find_in_base(char*, Pclass, int, int=0);
        void    modify_inst_names(char *s); // Adjust ctor names for instantiation
        bit     parametrized_class();
	bit	has_const_mem();
#endif
};

inline int same_class (Pclass c1, Pclass c2, int dummy=0) {
  // this function does the trick of comparing two classes in a
  // templatized scenerio, so, any usage of comparing two classes should
  // call this function, instead of doing " c1 == c2 " .

  return (c1==c2 ? 1 : c1->same_class(c2, dummy) );
}

#ifndef GRAM

class clist {
	Pclass cl;
	clist* next;
public:
	clist(Pclass c, clist* n) { cl=c; next=n; }
	int onlist(Pclass);
	void clear();
};
	
extern clist * vcllist;

struct vl {
	struct vl* next;
	Pvirt vt;
	classdef* cl;

	vl(classdef* c, Pvirt v, struct vl* n)
		{ cl = c; vt = v; next = n; }
};

extern vl* vlist;

extern int nin;
extern int Noffset;
extern TOK Nvis;
extern TOK Nvirt;
extern Pexpr Nptr;
extern Pbcl Nvbc_alloc;
extern char *Nalloc_base;
extern Pexpr rptr(Ptype,Pexpr,int);
extern Pexpr vbase_args(Pfct, Pname);
extern Pexpr cdvec(Pname,Pexpr,Pclass,Pname,int,Pexpr,Pexpr=0);
extern Pexpr mk_zero_init(Ptype,Pname,Pname);

extern Pexpr find_name(Pname, Pclass, Ptable, int, Pname);
extern Pname find_virtual(Pclass,Pname);
extern Pname vfct(Pclass, char*);
extern int Vcheckerror;
extern int ignore_const;	

extern int mex;
extern Pclass mec;
extern Pclass tcl;
extern int processing_sizeof; // suppresscertain referencing errors for args to sizeof 
#endif

struct basetype : public type
	/*	ZTYPE CHAR SHORT INT LONG FLOAT DOUBLE
		FIELD EOBJ COBJ TYPE ANY
	*/
	/*	used for gathering all the attributes
		for a list of declarators

		ZTYPE is the (generic) type of ZERO
		ANY is the generic type of an undeclared name
	*/
{
	bit	b_unsigned;
	bit	b_signed;
	bit	b_volatile;
	bit	b_typedef;
	bit	b_inline;
	bit	b_virtual;
	bit	b_short;
	bit	b_long;
	bit	b_vlong;
	bit	b_bits;		/* number of bits in field */
	bit	b_offset;	// bit offset of field
	TOK	b_sto;		/* AUTO STATIC EXTERN REGISTER 0 */
	Pname	b_name;		/* name of non-basic type */
	Ptable	b_table;	/* memtbl for b_name, or 0 */
	Pname	b_xname;	/* extra name */
	union {
	Ptype	b_fieldtype;
	char*	b_linkage;
	};

	basetype(TOK, Pname);

	Pbase	type_adj(TOK);
	Pbase	base_adj(Pbase);
	Pbase	name_adj(Pname);
	Pname   aggr();
	Pbase	check(Pname);
#ifndef GRAM
	void	dcl_print();
 	Pbase	arit_conv(Pbase);
        bit     parametrized_class();
#endif
	int     discriminator(int); // union discriminator fcn 
};

enum Linkage { linkage_default, linkage_C, linkage_Cplusplus };
extern Linkage linkage;
void set_linkage(char*);

struct fct : public type {		// FCT
	Templ_type fct_base;  	// recognize template function
	TOK	nargs;		// count of arguments
	TOK	nargs_known;	// 0 if unknown, 1 if known, or ELLIPSIS
	bit  	last_stmt;
	bit	f_vdef;		// 1 if this is the first virtual definition
				// of this function
	bit	f_inline;	// 1 if inline, 2 if being expanded, else 0
	bit	f_is_inline;	// because f_inline is set to 0
				//   before laying down static version of inline
	bit	f_const;	// one if member function that may be called for
				// a const object, else 0
	bit	f_static;	// 1 if static member function, else 0
	short	f_virtual;	// index in virtual table, or 0 meaning non-virtual
	short	f_imeasure;	// some measure of the size of an inline function
	Ptype	returns;
	Pname	argtype;
	Ptype	s_returns;
	Pname	f_this;
	Pclass	memof;		// member of class memof
	Pclass	def_context;	// defined in def_context (as for friends)
	Pblock	body;
	Pname	f_init;		// base and member initializers
	Pexpr	f_expr;		// body expanded into an expression
	Pexpr	last_expanded;
	Pname	nrv;		// named return value name
	Pname	f_result;	// extra second argument of type X&
	Pname	f_args;		// argument list including args added by cfront
	Linkage	f_linkage;
	char*	f_signature;	// character encoding of function type
	Plist local_class;	// list of local classes //SYM -- preserve for use in del.c
	static Pfct fct_free;
	void*	operator new(size_t);
	void	operator delete(void*,size_t);

	fct(Ptype, Pname, TOK);
	void	argdcl(Pname,Pname);
#ifndef GRAM
	Ptype	normalize(Ptype);
	void 	arg_print();
	void	dcl_print();
	void	dcl(Pname);
	Pexpr	base_init(Pclass, Pexpr, Ptable, int);
	Pexpr	mem_init(Pname, Pexpr, Ptable);
	void	init_bases(Pclass, Pexpr);
	bit	declared() { return nargs_known; };
	void	simpl();
	int	ctor_simpl(Pclass, Pexpr);
	Pstmt	dtor_simpl(Pclass, Pexpr);
	Pexpr	expand(Pname,Ptable,Pexpr);
	void	sign();
        int     is_templ() { return fct_base != VANILLA; }
#endif
	int     discriminator(int); // union discriminator fcn 
};

struct name_list : public node {
	Pname	f;
	Plist	l;
        name_list(Pname ff, Plist ll);
};

#ifndef GRAM
enum gen_types { no_templ, some_templ, all_templ };

struct gen : public type {		// OVERLOAD
	Plist	fct_list;
        gen_types holds_templ;    // does fct_list hold a template?

	gen() { base = OVERLOAD; }
	Pname	add(Pname);
	Pname  	find(Pfct, bit);
	Pname	match(Pname, Pfct, bit);
	Pname	exactMatch(Pexpr, int);
	Pname	oneArgMatch(Pexpr, int);
	Pname	multArgMatch(Pexpr, int);
        int     has_templ() { return holds_templ != no_templ; }
        int     pure_templ() { return holds_templ == all_templ; }
};
#endif

struct pvtyp : public type {
	Ptype typ;
};

struct vec : public pvtyp		// VEC
				// typ [ dim ]
{
	Pexpr	dim;
	int	size;	

	static Pvec vec_free;
	void*	operator new(size_t);
	void	operator delete(void*,size_t);

	vec(Ptype t, Pexpr e) { base=VEC; typ=t; dim=e; DBID(); }
#ifndef GRAM
	Ptype	normalize(Ptype);
#endif
};

struct ptr : public pvtyp		// PTR, RPTR i.e. reference
{
	Pclass	memof;		// pointer to member of memof: memof::*
	Pname   ptname;		// template class: T::*, adjusted in type::dcl

	static Pptr ptr_free;
	void*	operator new(size_t);
	void	operator delete(void*,size_t);

	ptr(TOK b, Ptype t) { base=b; typ=t; DBID(); }
#ifndef GRAM
	Ptype	normalize(Ptype);
#endif
};

#ifndef GRAM
inline Pptr type::addrof() { return new ptr(PTR,this); }

inline bit
type::is_or_pts_to(TOK k)
{
	Ptype t = skiptypedefs();
	if (t->base == PTR) t = Pptr(t)->typ->skiptypedefs();
	return (t->base == k) ? 1 : 0;
}
#endif


/****************************** constants ********************************/

		/* STRING ZERO ICON FCON CCON ID */
		/* IVAL FVAL LVAL */

/***************************** expressions ********************************/

#ifndef GRAM
extern Pexpr next_elem();
extern void new_list(Pexpr);
extern void list_check(Pname, Ptype, Pexpr, Ptable=0);
extern Pexpr ref_init(Pptr,Pexpr,Ptable);
extern Pexpr class_init(Pexpr,Ptype,Pexpr,Ptable);
extern Pexpr check_cond(Pexpr, TOK, Ptable);
extern Pexpr ptof(Pfct,Pexpr,Ptable);
extern void dosimpl(Pexpr, Pname);
extern int ref_initializer;
extern int ntok;

extern void ptbl_init(int);
extern void ptbl_add_pair(char*,char*);
extern char *ptbl_lookup(char*);
extern char *st_name(char*);
#endif

struct expr : public node	/* PLUS, MINUS, etc. */
	/* IMPORTANT:	all expressions are of sizeof(expr) */
	/*	DEREF		=>	*e1 (e2==0) OR e1[e2]
		UMINUS		=>	-e2
		INCR (e1==0)	=>	++e2
		INCR (e2==0)	=>	e1++
		CM		=>	e1 , e2
		ILIST		=>	LC e1 RC   (an initializer list)
		a Pexpr may denote a name
	*/
{
	union {
		Ptype	tp;
		char 	*string4;
	};
	union {
		Pexpr	e1;
		long	i1;
		char*	string;
	};
	union {
		Pexpr	e2;
		int	i2;
		char*	string2;
		Pexpr	n_initializer;
		Ptype	tpdef;  // local and nested typedef info
	};
	union {			/* used by the derived classes */
		Ptype	tp2;    
		Pname	fct_name; // of a call expr
		Pexpr	cond;
		Pexpr	mem;
		Ptype	as_type;
		Ptable	n_table;
		Pin	il;
		Pname	query_this; 
	};

	static Pexpr expr_free;
	void* operator new(size_t);
	void operator delete(void*,size_t);
	expr(TOK, Pexpr, Pexpr);
	void	del();
#ifndef GRAM
	void	print();
        Pexpr   typ0(Ptable);
	Pexpr	typ(Ptable);
	long	eval();
	unsigned long ueval(long,long);
	int	lval(TOK);
	Ptype	call_fct(Ptable);
	Pexpr	address();
	Pexpr	contents();
	void	simpl();
	Pexpr	expand();
	bit	not_simple(int inflag=0);
	bit	is_const_obj();
	Pexpr	oper_overload(Ptable);
	Pexpr	docast(Ptable);
	Pexpr	dovalue(Ptable);
	Pexpr	donew(Ptable);
	void	simpl_new();
	void	simpl_delete();
#endif
	int     discriminator(int); // union discriminator fcn 
};

struct texpr : public expr {		// G_CAST, CAST NEW VALUE (also ICALL)
	texpr(TOK bb, Ptype tt, Pexpr ee) : expr (bb,ee,0) { tp2=tt; }
};

struct cast : public expr {		// G_CAST
	cast(Ptype tt, Pexpr ee) : expr (G_CAST,ee,0) { tp=tp2=tt; }
};

struct ival : public expr {		// IVAL
	ival(long ii) : expr (IVAL,0,0) { i1 = ii;}
};

struct call : public expr {		// CALL
	call(Pexpr aa, Pexpr bb) : expr (CALL,aa,bb) { }
#ifndef GRAM
	void	simpl();
	Pexpr	expand(Ptable);
#endif
};

struct qexpr : public expr {		// QUEST	cond ? e1 : e2
	qexpr(Pexpr ee, Pexpr ee1, Pexpr ee2) : expr (QUEST,ee1,ee2) { cond=ee; }
};

struct ref : public expr {		// REF DOT	e1->mem OR e1.mem
	ref(TOK ba, Pexpr a, Pexpr b) : expr (ba,a,0) { mem=b; }
};

struct mdot : public expr {		// MDOT		a.b
	mdot(char* a, Pexpr b) : expr (MDOT,0,0) { string2=a; mem=b; }
};

struct text_expr : public expr	{	// TEXT (vtbl_name)
	text_expr(char* a, char* b) : expr (TEXT,0,0)
	{ string=a; string2=b; }
};
extern char* vtbl_name(char*,char*);
/************************* names (are expressions) ****************************/

struct basecl : public node {	// NAME		=> base class
			// VIRTUAL	=> virtual base class
	TOK	ppp;		// private / public / protected
	bit	allocated;	// allocated virtual base
        bit     promoted;       // non-explicit, promoted virtual base
	Pclass	bclass;
	Pexpr	init;		// base class initializers for ctors
	int	ptr_offset;	// pointer's relative position in derived class
	int	obj_offset;	// object's relative position in derived class
	Pname*	virt_init;	// vector of vtbl table initializers
	basecl*	next;

        basecl(Pclass cl, basecl* n) { baseclass=1; bclass=cl; next=n; promoted=0; init=0;}
};

enum template_formal_types { 
	template_type_formal =1 , template_expr_formal,
        template_actual_arg_dummy // used during the parse
};

extern TOK ppbase;

struct name : public expr {	// NAME TNAME DTOR and the lexical keywords
	TOK	n_oper;		// name of operator or 0
	TOK	n_sto;		// EXTERN STATIC AUTO REGISTER ENUM 0
	TOK	n_stclass;	// STATIC AUTO REGISTER 0
	TOK	n_scope;	// EXTERN STATIC FCT ARG PUBLIC 0 
	TOK	n_key;	/* for names in table: class */
	bit	n_evaluated;	// 0 or n_val holds the value
	bit	n_xref;		// argument of type X(X&)
	unsigned char	lex_level;
	TOK	n_protect;	// PROTECTED (<=>n_scope==0) or 0
	bit	n_dcl_printed;	// 1: declaration printed
				// 2: definition printed
				// 0: declaration not printed

        // if this is set it implies that n_template_arg == template_type_formal
	char    n_template_arg; // One of template_formal_types for template arguments
	bit 	n_template_fct; // non-class template function
        bit     n_redefined ;   // set only for PT function names && classes 
				// where an explict definition was provided.
	short	n_addr_taken;
	short	n_used;
	short	n_assigned_to;
	Loc where;
	int	n_offset;	// byte offset in frame or struct
	char*	n_anon;	// 0, or anon union object name 
	union {//SYM ???
	Pname	n_list;   // next name in arg or other list 
	Pname	n_hidden; //SYM type name hidden by this name in parse table
	Pname	n_dtag;   // base==DTOR -- name after ~ or 0 for basic type dtor
	};
	Pname	n_tbl_list;
	char    *n_gen_fct_name; // used to be punned with n_tbl_list.
        char    *n_template_arg_string ; // the mangled string name
	Pktab	n_ktable;//SYM parsing table in which name is entered
	union {
	/* 
	 * n_qualifier: name of containing class
	 * n_realscope: for labels (always entered in function table) 
         *		the table for the actual scope in which label occurred.
	 * syn_class: lex table only
	 */
		Pname	n_qualifier;	
                Ptable  n_realscope;    
		int     syn_class; 
        };

	/* n_val: the value of n_initializer */
	long	n_val;		
	/* for inlines, the number of the argument when base == ANAME */
	int argno;
	static Pname name_free;
	void* operator new(size_t);
	void operator delete(void*,size_t);
	name(char* =0);

	Pname	normalize(Pbase, Pblock, bit);
	Pname	tdef();
	Pname	tname(TOK);
	void	hide();
#ifndef GRAM
	Pname	dcl(Ptable,TOK);
	int	no_of_names();
	void	use()		{ n_used++; };
	void	assign();
	void	take_addr();
	void	check_oper(Pname);
	void	simpl();
	void	print(bit fullprint=0);
	void	dcl_print(TOK);
	void	field_align();
	Pname	dofct(Ptable,TOK);
	inline	Pfct get_fct();
        bit     is_template_fct() { return n_template_fct; }
	bit	inst_body();
	bit	finst_body();
	bit	dinst_body();
#endif
        // this works only for function templates which require type formals
        bit     is_template_arg() { return n_template_arg == template_type_formal; }
	void	del();
	inline Pfct fct_type();  
	int     discriminator(int); // union discriminator fcn 
};

extern int friend_in_class;
extern int in_class_dcl;

// from parser
extern int in_class_decl;
extern int parsing_class_members;
extern int in_mem_fct;
extern int in_arg_list;
extern Ptype in_typedef;
extern int	defer_check;	 // redefinition typedef check delay
extern int	declTag;	 // !1: inline, virtual mod permitted
extern Pname in_tag;
extern int DECL_TYPE;

/******************** statements *********************************/

struct stmt : public node {	/* BREAK CONTINUE DEFAULT */
/*	IMPORTANT: all statement nodes have sizeof(stmt) */
	Pstmt	s;
	Pstmt	s_list;
	Pstmt	gt;
	Loc	where;
	union {
		Pname	d; // goto/block -- destination
		Pexpr	e2; // for iteration
		Pstmt	has_default; // switch statement default
		int	case_value; 
		Ptype	ret_tp; // pair
	};
	union {
		Pexpr	e;
		bit	own_tbl;
		Pstmt	s2;
	};
	Ptable	memtbl;
	Pktab	k_tbl; //SYM parsing table
	union {
		Pstmt	for_init;
		Pstmt	else_stmt;
		Pstmt	case_list;
		Loc	where2; // location of } at end of block
				// in DO loops, location of conditional
	};

	static Pstmt stmt_free;
	void* operator new(size_t);
	void operator delete(void*,size_t);
	stmt(TOK, loc, Pstmt);
	void	del();
#ifndef GRAM
	void	print();
	void	dcl(int forflag=0);
        void    dcl1(Pstmt&);
	void	reached();
	Pstmt	simpl();
	Pstmt	expand();
	Pstmt	copy();
	// PAIR and BLOCK pun on `e': if (s->e) not safe
	bit	has_expr() { return (base != BLOCK && base != PAIR)?e!=0:0; }
#endif
	int     discriminator(int); // union discriminator fcn 
};

#ifndef GRAM
extern char* Neval;
extern Ptable scope;
extern Ptable expand_tbl;
extern Pname expand_fn;
#endif

struct estmt : public stmt	/* SM WHILE DO SWITCH RETURN CASE */
	/*	SM (e!=0)	=>	e;
		in particular assignments and function calls
		SM (e==0)	=>	;	(the null statement)
		CASE		=>	case e : s ;
		SM_PARAM        => e is the template_statement_tree_formal name
	*/
{
	estmt(TOK t, loc ll, Pexpr ee, Pstmt ss) : stmt (t,ll,ss) { e=ee;}
};

struct ifstmt : public stmt	/* IF */
	// else_stmt==0 =>	if (e) s
	// else_stmt!=0 =>	if (e) s else else_stmt
{
	ifstmt(loc ll, Pexpr ee, Pstmt ss1, Pstmt ss2)
		: stmt (IF,ll,ss1) { e=ee; else_stmt=ss2; };
};

struct lstmt : public stmt	/* LABEL GOTO */
	/*
		d : s
		goto d
	*/
{
	lstmt(TOK bb, loc ll, Pname nn, Pstmt ss) : stmt (bb,ll,ss) { d=nn; }
};

struct forstmt : public stmt {	// FOR
	forstmt(loc ll, Pstmt fss, Pexpr ee1, Pexpr ee2, Pstmt ss)
		: stmt (FOR,ll,ss) { for_init=fss; e=ee1; e2=ee2; }
};

struct block : public stmt {	// BLOCK	{ d s }
	block(loc ll, Pname nn, Pstmt ss, loc rr = noloc ) : stmt (BLOCK,ll,ss)
		{ d=nn; where2=rr; }
#ifndef GRAM
	void	dcl(Ptable);
	Pstmt	simpl();
#endif
};

struct handler : public stmt {	// not implemented -- currently set to BLOCK
	handler( Pstmt tt, Pstmt hh ) : stmt (BLOCK,tt?tt->where:curloc,tt)
		{ d=0; s->s_list=hh; where2=curloc; }
};

#ifndef GRAM
struct pair : public stmt {	// PAIR
	pair(loc ll, Pstmt a, Pstmt b) : stmt (PAIR,ll,a) { s2 = b; }
};
#endif

struct nlist {
	Pname	head;
	Pname	tail;
		nlist(Pname);
	void	add(Pname n)	{ tail->n_list = n; tail = n; };
	void	add_list(Pname);
};

extern Pname name_unlist(nlist*);

struct slist {
	Pstmt	head;
	Pstmt	tail;
		slist(Pstmt s)	{ /*Nl++;*/ head = tail = s; };
	void	add(Pstmt s)	{ tail->s_list = s; tail = s; };
};

extern Pstmt stmt_unlist(slist*);

struct elist {
	Pexpr	head;
	Pexpr	tail;
		elist(Pexpr e)	{ /*Nl++;*/ head = tail = e; };
	void	add(Pexpr e)	{ tail->e2 = e; tail = e; };
};

extern Pexpr expr_unlist(elist*);

#ifndef GRAM 
extern class dcl_context * cc;

#define MAXCONT	100
extern dcl_context ccvec[/*plan9 MACONT*/];

struct dcl_context {
	Pname	c_this;	/* current fct's "this" */
	Ptype	tot;	/* type of "this" or 0 */
	Pname	not;	/* name of "this"'s class or 0 */
	Pclass	cot;	/* the definition of "this"'s class */
	Ptable	ftbl;	/* current fct's symbol table */
	Pname	nof;	/* current fct's name */

	void	stack()		{
					cc++;
					cc >= &ccvec[MAXCONT] ?
					    error('i', "ccvec buffer overflow")
					    :
					    0;
					*cc = *(cc-1);
				};
	void	unstack()	{ cc--; };
};
#endif 

extern void yyerror(const char*);


#ifndef GRAM
extern char* line_format;

extern Plist stat_mem_list;
extern Plist isf_list;
extern Pstmt st_ilist;
extern Pstmt st_dlist;
extern Ptable sti_tbl;
extern Ptable std_tbl;
extern int need_sti( Pexpr e, Ptable tbl = 0, bit is_static_ok = 0 ); 
Pexpr try_to_coerce(Ptype, Pexpr, char*, Ptable);
extern bit can_coerce(Ptype, Ptype);
extern Ptype np_promote(TOK, TOK, TOK, Ptype, Ptype, TOK, bit=1);
extern bit enum_promote;
extern int suppress_error;
extern void delete_local();

extern int over_call(Pname, Pexpr);
extern Pname overFound;
extern Pname Nover;
extern Pname Ncoerce;
extern int Nover_coerce;
struct ia : public node {
	Pname	local;	// local variable for argument
	Pexpr	arg;	// actual arguments for call
	Ptype	tp;	// type of formal argument
        ia() { base = XIA; };
};

struct iline : public node {
	Pname	fct_name;	/* fct called */
	Pin	i_next;
	Ptable	i_table;
	int	i_slots;	// no of arg slots pointer to by i_args
	ia*	i_args;
        iline() { base = XILINE; };
};

struct con_dtor {
    Pname tn;
    Pexpr condition;
    con_dtor *next;
    con_dtor(Pname n, Pexpr e)
        : tn(n), condition(e), next(0) {}
};

extern con_dtor *pdlist;
extern Pexpr curr_expr;
extern Pin curr_icall;
#define FUDGE111 11111
#define VTOK 22222
#define ITOR 77

extern Pstmt curr_loop;
extern Pblock curr_block;
extern Pstmt curr_switch;
extern loc last_line;  // last #line + number of '\n's output since
extern int last_ll;    // 0 or line of current stmt/dcl being printed
extern FILE* out_file; // output file descriptor
extern bit Cast;
extern bit TCast;
extern bit simpl_friend; // are we simplifying a friend function
extern bit in_return; // are we calling ref_init with a RETURN
extern loc no_where;

extern long str_to_long(const char*);
extern int c_strlen(const char* s);
#endif

#ifndef GRAM
extern Pname vec_new_fct;
extern Pname new_fct;
extern Pname del_fct;
extern Pname vec_del_fct;

extern int Nstd; // standard coercion used (derived* =>base* or int=>long or ...)

extern int stcount;	// number of names generated using make_name()

Pexpr replace_temp(Pexpr,Pexpr);
void make_res(Pfct);

extern int Pchecked;
Pexpr ptr_init(Pptr,Pexpr,Ptable);
Pexpr call_ctor(Ptable, Pexpr p, Pexpr ctor, Pexpr args, int d = REF, Pexpr vb_args = 0);
Pexpr call_dtor(Pexpr p, Pexpr dtor, Pexpr arg = 0, int d = DOT, Pexpr vb_args = 0);
void check_visibility(Pname, Pname, Pclass, Ptable, Pname);

int make_assignment(Pname);

extern Pname make_tmp(char, Ptype, Ptable);
Pexpr init_tmp(Pname, Pexpr, Ptable);
extern Pname Ntmp;
extern Pname Ntmp_refd;
extern Pname Ntmp_flag;
extern Pexpr Ntmp_dtor;

extern int is_unique_base(Pclass, char*, int, int = 0, Pclass priSeen = 0);
extern Pexpr rptr(Ptype, Pexpr, int);

extern int read_align(char*);
extern void new_init();

extern void Eprint(Pexpr);
extern Pexpr cast_cptr(Pclass ccl, Pexpr ee, Ptable tbl, int real_cast);
extern Pexpr mptr_assign(Pexpr,Pexpr);
extern Pclass Mptr;

#endif

extern bit fake_sizeof;	// suppress error message for ``int v[];''

extern TOK lalex();

// encapsulate casts
inline Pname type::bname() { return Pbase(this)->b_name; }
inline Ptype type::bname_type() { return Pbase(this)->b_name->tp; }
//move to typ.c because 2.0 bug
/*
inline Pclass type::classtype() { 
    return (base==COBJ)?Pclass(Pbase(this)->b_name->tp)
	: (error('i',"T::classtype(): %k cobjX",base),0); 
}
*/

inline Penum type::enumtype() { 
    return (base==EOBJ)?Penum(Pbase(this)->b_name->tp)
	: (error('i',"T::enumtype(): %k eobjX",base),Penum(0)); 
}

inline Pfct name::fct_type() { 
    return (tp->base==FCT) ? Pfct(tp)
	: (error('i',"N::fct_type():%n is %k notF",this,tp->base),Pfct(0)); 
}

inline bit type::is_templ_instance() {
        Templ_type t = (base==FCT)
                ? Pfct(this)->fct_base
                : (base==CLASS)
                ? Pclass(this)->class_base : (Templ_type)0;
        return (t==INSTANTIATED || t==UNINSTANTIATED);
}

#ifndef GRAM
inline Pfct name::get_fct() { 
    return (tp->base==FCT) ? Pfct(tp)
	: Pfct(Pgen(tp)->fct_list->f->tp);
}

inline Pname
table::insert_copy(Pname n, TOK k)
{
	Pname nn = new name; *nn = *n;
	nn->n_table = 0; nn->n_tbl_list = 0;
	return insert(nn,k);
}
#endif

#ifdef DBG
extern "C" {
extern void display_expr( Pexpr, char* = 0, int = 0 );
extern void display_stmt( Pstmt, char* = 0, int = 0 );
extern void display_type( Ptype );
extern void display_namelist( Plist, char* = 0, int = 0 );
#ifndef GRAM
extern void display_context( dcl_context*, char* = "", int = 0 );
#endif
extern void process_debug_flags( char* );
}
#define DB(a) if(scan_started){a;}
extern int Adebug; // allocation (ctor/del) debugging
extern int Bdebug; // table.c debugging
extern char* Bdebarg; // arg for B flag
extern int Ddebug; // dcl debugging
extern int Edebug; // expr debugging
extern int Kdebug; // parsing table debugging
extern int Ldebug; // lex/lalex debugging
extern int Mdebug; // trace function matching
extern int Ndebug; // norm debugging
extern int Pdebug; // print debugging
extern int Rdebug; // run() debugging
extern int Sdebug; // simpl debugging
extern int Tdebug; // typ debugging
#define Ydebug yydebug
extern int Ydebug; // yacc debugging

#else

#define DB(a) /**/

#endif

#endif
/* end */
