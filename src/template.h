/*ident	"@(#)cls4:src/template.h	1.12" */
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

enum bool { false, true };
extern Pclass current_instantiation;
extern Pfct current_fct_instantiation;

// typedef unsigned char uchar; // sun's types.h: u_char
typedef class templ *Ptempl;
typedef class funct_inst *Pfunct_inst;
typedef class templ_inst *Ptempl_inst;
typedef class basic_inst *Pbase_inst;
typedef class basic_template *Ptempl_base;
typedef class function_template *Pfunt;
typedef class data_template *Pdata;
typedef class templ_state *Ptstate;
typedef class templ_classdef *Ptclass;
typedef class templ_fct *Ptfct;
typedef class Pslot *Pbinding;
typedef class cons *Pcons;

class cons {
/* A Lisp style cons cell to help build lists. 
 * The parameterized type facility should obviate 
 * the need for this type-unsafe nonsense. */
public:
  	void *car;
  	cons *cdr;
  	cons(void *pcar, cons *pcdr) { car=pcar; cdr=pcdr; };
};

/*****************************************************************************
* 									     *
* The class template_compilation holds the state, and the associated methods *
* used during template compilation. There is exactly one instance of the     *
* type, it is mainly a device used to enforce modularity.  In reality, it    *
* would never need to be instantiated since all it's members are static.     *
* However, since the type::mem for a reference is not supported as	     *
* yet(cfront 1.2), we need an instantiation to get to the members via	     *
* pointer syntax instead.						     *
* 									     *
*	***** should change to pure static class *****			     *
* 									     *
* A templ_compilation holds the state relevant to the syntax analysis of a   *
* class or member function template definition.				     *
* 									     *
*****************************************************************************/

/*****************************************************************************
* will merge class and function handling by having owner be a basic template *
* pointer -- first, let's get functions up and running.  then do it right    *
*****************************************************************************/

class templ_state {
/* save relevant information when parsing friend 
 * template within a class: currently, only instance
 * where a nested template can occur */
public:
	templ_state(); // save
	~templ_state(); // restore
private:
  	Plist param_end;  
  	Plist params;    
  	Pcons templ_refs; 
  	Pcons friend_templ_refs; 
  	Pcons last_cons;
  	Pcons last_friend_cons;
	Ptempl owner;
};

class templ_compilation {
/* A templ_compilation holds the state relevant to the syntax 
 * analysis of a class or member function template definition.				     *
 * There is exactly one instance of the type.  It mainly serves
 * as a device used to enforce modularity. */
public:
  	static Plist param_end;   // make append easier
  	static Ptempl  list;      // list of class templates for compilation
  	static Pfunt   f_list;    // list of function templates for compilation
  	static Pfunt   f_owner;   // template when compiling a function
  	static Ptempl  owner;     // template when compiling a member function
  	static Ptempl_base parsed_template; // template immediately parsed

  	static Plist   params;    // list of parameters to the template
  	static Pexpr   actuals;   // list of actuals for explicit class template
  	static Ptable  templates; // templates defined during compilation

  	static bool    in_progress; 
  	static Ptstate save_templ;

  	/* instantiation parameter parsing in progress. Used in the lexer 
         * to ensure that name string are consed in the heap, rather than 
         * being retained as pointers into the lex buffer. */
  	static int     parameters_in_progress;
  	static bool    formals_in_progress;

  	// the list of templates referenced by the top level definition being compiled.
  	static Pcons   templ_refs; 
  	static Pcons   friend_templ_refs; 
  	static Pcons   last_cons;
  	static Pcons   last_friend_cons;
  
  	void start(); // initialize templ_compilation state
  	void end(Pname class_name);
  	void end_of_compilation(); // Done with compilation, instantiate bodies

  	Pname forward_declaration(Pname class_name);
  	void introduce_class_templ(Pname cnamep);
  	void introduce_funct_templ(Pname fnamep);

  	void append_ref(Ptempl_inst ref); // add to templ_refs
  	void append_friend_ref(Ptempl_inst ref); // add to friend_templ_refs
  	void instantiate_ref_templ(); // instantiate templ_refs 
  	void clear_ref_templ(); // zero-out templ_refs
  	void clear_friend_ref_templ(); // zero-out friend_templ_refs

  	void collect(TOK parm_type, Pname namep); // collect type parameter
  	void collect(Pname); // collect expression parameter
  	void append_parameter(Pname); // place them in params
  	void enter_parameters();
  	int get_formals_count(); // how many formals

  	Pname check_tname(Pname p);
 	bool  current_template(Pname p, Pexpr actuals);
	bool  friend_template(Pexpr actuals);
  	Ptempl is_template(Pname);
  	Ptempl is_template(char*);
  	Pfunt  is_template(char*,TOK);
};

/* 
 * The canonical template_compilation instance. 
 * templ_compilation exists as a class simply to provide 
 * a code and data packaging mechanism. 
 * There is exactly one generated instance of it. 
 */
extern templ_compilation *templp;

// should actually be static member functions of templ_compilation
Pname parametrized_typename (Pname p, Pexpr actuals, bit=0) ;
Pbase parametrized_basetype (Pname p, Pexpr actuals) ;

void check_formals_for_dups(Plist formals);
void check_funct_formals(Plist formals, Pname namep);

class basic_template {
/* the basis for class and member function templates */
  	friend class templ_compilation;
	friend void display_templ_refs( basic_template* );
	friend void handle_bound_friend( Ptempl_base, Pname );
protected:
  	Plist       formals;        // The formal arguments to the template
  	Pcons       templ_refs;     // The templates referenced by this template

         /* Use these state variables to set up the correct state for error processing. 
	 * They are used by the "error" routines for statement numbers.  */
  	Pname       Cdcl;
  	Pstmt       Cstmt;

  	// used to detect loops during instantiation; a count greater than two is
  	// indicative of a non-terminating instantiation sequence
  	int 	    open_instantiations;
	int         extrapolated; // extrapolated friend declaration

  	// ensure that use of formals is consistent across, class, member and
  	// forward declarations
  	void check_formals(Plist formals) ;

public:
	virtual void dummy(); // for optimal vtbl laydown
	virtual int has_friend(Pname)=0;
	Plist get_formals() { return formals; }	
  	int get_formals_count(); // should probably define a member
	bool is_extrapolated() { return extrapolated ? true : false; }
};

class templ  : public basic_template { 
/* the template class representation */
  	friend class templ_inst;
  	friend class function_template;
  	friend class data_template;
	friend class classdef;

private:
  	Ptempl_inst insts;	// template instantiations 
  	Pbase 	basep;     	// template COBJ basetype 
  	Pfunt	fns;           	// member function declarations
  	Pfunt	fns_end;        
	Pdata  	data;		// static data member declarations
	Pdata  	data_end;
public:
  	Ptempl_inst get_match(Pexpr,Ptempl_inst,bool);
  	bool check_actual_args(Pexpr);
  	Ptempl      next; 	// connects all the class templates in the comp
  	Pname       namep;      // the TNAME for the template class
  	bool        defined;    // actual definition seen
  	Pname       members;    // note the members to catch redefinition errors

  	Ptempl_inst get_inst(Pexpr,Ptempl_inst=0);
  	templ(Plist,Pname);
  	void resolve_forward_decl(Plist,Pclass);
  	void instantiate_forward_decl();
  
  	Pbase basetype() {return basep; } // uninstantiated base type
  	Pbase inst_basetype(Pexpr actuals); // specific instantiation basetype 
  	Pclass classtype() { return Pclass(basep->b_name->tp); }
  	Pname typename(Pexpr actuals);
 	Pfunt collect_function_member(Pname); 
 	Pdata collect_data_member(Pname);
  	bool instantiate_bodies(); 
	int has_friend(Pname);
};

// Member function templates
class function_template : public basic_template {
/* SBL: go through these */
  friend Pbinding is_ftempl_match(Pexpr,Pfunt);
  friend Pname has_templ_instance(Pname,Pexpr,bit=0);
  friend class templ_compilation;
  friend class templ;
  friend class templ_inst;
  friend class funct_inst;
  friend class basic_inst;
  friend class templ_fct;

  	Pname fn;             // The name of the member function
  	Pfunt next;           // connects the list of functions
  	Pfunt gen_list;       // connects overloaded instances 
public:
  	Pfunct_inst insts;    // instantiations of the template
  	function_template(templ&,Plist,Pname); 	// member functions
  	function_template(Plist,Pname); 	// function templates
	Pname func_name() { return fn; }

  	void instantiate_forward_decl();
  	Pfunct_inst get_match(Pexpr,Pfunct_inst,bool); 
  	Pfunct_inst get_inst(Pexpr,Pfunct_inst=0);
	int has_friend(Pname);
};

class data_template : public basic_template 
{ // explicitly initialized static data members 
	friend class templ;
	friend class basic_inst;
	friend class templ_inst;
	Pname 	dat_mem;
	Pdata	next;
public:
  	data_template(templ&,Plist,Pname); 	
	int has_friend(Pname); // need override pure virtual
};

// Global state variables that must be saved around an instantiation. The
// saving of state was required in the implementstion that interspersed decl
// processing and instantiation, rather than the current strategy, which only
// forces instantiations at the top level outside of any dcl-processing
// context. It is retained in case we ever go back to the "interspersed" style
// of instantiation. 
class state {
public:
  Pname       Cdcl ;      // the global variables used by the error routines
  Pstmt       Cstmt ;
  Pname       dcl_list ;  // Holds the list of typedef names that are hidden
  Loc         curloc ;
  
  int         curr_file ;
  Pexpr       curr_expr ;
  Pin         curr_icall ;
  Pstmt       curr_loop;
  Pblock      curr_block;
  Pstmt       curr_switch;

  int         bound  ;   
  int         inline_restr ;
  Loc         last_line ;

  state() {} ; // prevent used before set warnings.
  void save() ;
  
  void init() ; 
  void restore() ; 
} ;


class pointer_hash ; 
class tree_copy_info ;

// A template starts out being uninstantiated, and is class_instantiated when
// there is a refrence to it with actual arguments. It is body_instantiated at
// the end of compilation, when all its function members are instantiated.
enum inst_status { uninstantiated, 
			function_instantiated, data_instantiated,
			class_instantiated, body_instantiated };

// templ_inst captures the arguments used in the instantiation of a template.
// These instantiations are rooted in the templ object. 
class basic_inst {
/* SBL: go through these */
	friend class template_instantiation;
	friend class templ;
	friend class function_template;
	friend class templ_classdef;
	friend Pcons make_ref_copy(pointer_hash&,tree_copy_info&,Pcons);
public:
  Pname get_tname() { return tname; }
  Pname get_namep() { return namep; }
  void print_error_loc(int=0);           // Wants to be a static function
  virtual void dummy();			 // for optimal vtbl laydown
  virtual void print_pretty_name() = 0;  // for nice error reporting
  virtual char *mangled_name(char*) = 0; // mangle template class/function names
  Pexpr      actuals;       	// instantiation arguments, chained using ELIST
			     	// as an expression "cons" node, e1 is the car
			     	// and e2 the cdr. The car points to a name node.
  Plist      inst_formals; 	// the instantiation's copy of the formals
  static Pbase_inst head;	// head of list of active instantiations.
protected:
  TOK	     isa;		// for the moment: CLASS,FCT
  Pname      tname;         	// name of instantiation 
  Pname      namep;         	// version of name in ktbl (class) or gtbl(funct)
  state      context;      	// the context of this instantiation
  Plist      hidden_globals; 	// list of globals hidden during instantiation
  // Plist      inst_formals; 	// the instantiation's copy of the formals
  inst_status status;
  Pbase_inst  next_active;  // list of currently active instantiations.

  char *instantiation_string();

  // The class correspondence table. This table is initialized 
  // when the class definition is instantiated. Subsequently, it is used to
  // initial member correspondence tables before copy process is initiated.
  pointer_hash *corr;

  Pcons ref_copy(pointer_hash&,tree_copy_info&,Pcons);

  // save and restore state around the template instantiation
  void save_state(Pname p) ;
  void restore_state() ;

  void expose_parameter_names();
  void hide_parameter_names();
  
  TOK isA() { return isa; }
};

struct Pslot {
	Pname param;
	Ptype typ;
};

class funct_inst : public basic_inst {
  	friend class template_instantiation;
	friend class function_template;
  	friend class templ_fct;
private:
  	Pfunct_inst next;         // list of template instantiations 
	Pfunct_inst tfct_copy(Pcons&,bool);
        void bind_formals();
public:
	funct_inst( Pexpr act, Pfunt owner );
	void instantiate( bool reinstantiate = false );
	void print_pretty_name();  // virtual
	char *mangled_name(char*); // virtual
	bool actuals_match(Pexpr);

  	Pfunt def;   // template definition; this is an instantiation.
  	bool refp;   // notes template references during a C++ definition
  	bool friend_refp;   // notes template references during a C++ definition
	Pbinding binding; // actual types binding to formals	
	bool f_copy_hook(Pnode&);
};

class templ_inst : public basic_inst  {
  	friend class template_instantiation;
  	friend class templ;
  	friend class classdef;
  	friend class templ_classdef;
  	friend Pcons make_ref_copy(pointer_hash&,tree_copy_info&,Pcons);
private:
  	Ptempl_inst next;     // linked list of template instantiations 
  	Ptempl_inst forward;  // this instantiation same as `forward'
public: 
  templ_inst (Pexpr act,  Ptempl owner);
  templ_inst (Pexpr act,  Ptempl owner, TOK csu);
  bool actuals_match(Pexpr check_actuals); /* merge this and function's */
  void instantiate_match(Ptempl_inst match) ;
  void kludge_copy(Pbase source_base) ;

  // create a copy of the class type subtree preparatory to the ensuing
  // instantiation. Return a non-zero value, only if there was no need to
  // create a copy, ie. an identical instantiation already existed.
  Ptempl_inst class_copy(Pcons &templ_refs, bool recopy) ;

  // Used to collect references to this template by a definition
  Ptempl_inst note_ref() ;
public:
  Ptempl        def;         // The template definition, for which this is an
			     // instantiation.
  bool         refp ;        // flag to note template references during a C++ 
			     // definition

  bool         friend_refp ;        // flag to note template references during a C++ 
	void explicit_inst(); 
  // Bind the formals before an instantiation
  void bind_formals() ;

  Ptempl_inst canonical_instantiation() {
    return ( forward ? forward : this ) ;
  }

  // get the class associated with this instantiation
  Pclass get_class() { return Pclass(Pbase(tname->tp)->b_name->tp) ;}
			      
  Ptempl_inst instantiate(bool reinstantiate = false) ;
  void print_pretty_name();		// virtual
  char *mangled_name(char *buffer);	// virtual
  // The uninstantiated basetype
  Pbase def_basetype() { return def->basep ; } ;

  // A general way of determining whether two template instantiations are
  // the same
  bool  same(Ptempl_inst t) ;

  bool copy_hook(Pnode&) ;
 	// return a copy of the function tree starting with it's name
  	Pname function_copy(Pfunt,Pcons&);
  	Pname data_copy(Pdata,Pcons&);

  // special check for instantiations used in qualifiers for template function
  // member declarations. 
  bool check_qualifier(Plist formals) ;
  Pname get_parameter(char *s) ;
} ;

// Experimental debugging toggle ???
extern int zdebug;

// The class node used for template classes.
// Rep invariant:
// class_base == UNINSTANTIATED ||
// class_base  == INSTANTIATED

class templ_classdef : public classdef {
public:
   	// a pointer to the instantiation; the instantiation 
	// also points back to it via tname->cobj->name->class 
   	Ptempl_inst inst; 
  	templ_classdef(Ptempl_inst i);
  	templ_classdef(Ptempl_inst i, TOK csu);
  	Pname unparametrized_tname() {return inst->def->namep;} 
};

class templ_fct : public fct {
public:
  	Pfunct_inst inst; // pointer to the instantiation
  	templ_fct(Pfunct_inst i);
  	Pname unparametrized_tname() {return inst->def->fn;} 
	void *operator new(size_t);
	void operator delete(void*,size_t);
	static Ptfct ptfct_free;
};

// Safe accessor functions for navigating through COBJ base classes
extern Templ_type get_class_base (Pbase b);
extern Ptclass get_template_class (Pbase b);
extern Ptempl_inst get_templ_inst(Pbase b);

// functions and data structures needed for directed instantiation

#include "Block.h"
Blockdeclare(Pchar);
extern Block(Pchar) instfct;
extern int noinst;
extern Block(Pchar) instdata;
extern int nodatainst;
extern bit tempdcl;
extern bit mk_zero_init_flag;
extern int first_file;
extern bit all_flag,alltc_flag,data_flag,ft_flag,none_flag;
extern void set_flags();
extern Pname righttname;
