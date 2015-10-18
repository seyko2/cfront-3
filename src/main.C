/*ident	"@(#)cls4:src/main.c	1.26" */
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

main.c:

	Initialize global environment
	Read argument line
	Start compilation
	Clean up and exit

**************************************************************************/

#ifdef __cplusplus
#include <stdlib.h>
#endif

#include <ctype.h>
#include <string.h>
#include <libc.h>
#include <osfcn.h>
#include "size.h"
#include "cfront.h"
#include "template.h"
#ifdef LICENSE_MAN
#include "sdelicense.h"
#include "sdetoolnum.h"
#endif


char* prog_name = "<<AT&T C++ Language System <3.0.3> 05/05/94>>";
static char* prog_vers = "__cfront_version_303_xxxxxxxx";
char* src_file_name = "";
char* line_format = "\n# %d \"%s\"\n";

Block(Pchar) instfct;
Block(Pchar) instdata;
bit all_flag,alltc_flag,data_flag,ft_flag,none_flag;
int noinst,nodatainst;

#ifdef unix
#include <signal.h>

#ifdef TEST_SUITE
#define INTERNAL2 126
#endif

static void core_dump(int = 0)
{
	if (error_count)
		fprintf(stderr,"%s: sorry, cannot recover from previous error\n", prog_name);
	else 
		error('i',"bus error (or something nasty like that)");
#ifdef TEST_SUITE
	ext(INTERNAL2);
#else
	ext(99);
#endif
}
#endif

Plist isf_list;
Pstmt st_ilist;
Pstmt st_dlist;
Ptable sti_tbl;
Ptable std_tbl;
Plist stat_mem_list;

int vtbl_opt = -1;	// how to deal with vtbls:
			// -1 static and defined
			// 0 external and supposed to be defined elsewhere
			// 1 external and defined
int debug_opt;
static int gplus_opt;	// -g to cc, overrides print optimization 
int ansi_opt;
int strict_opt;		// disallow features from the anachronism section
			// of the manual
int warning_opt;	// produce more warnings if set
int dem_opt_local = 0;
int dem_opt_mem = 0;
int perf_opt=0;         // Break link compatibility performance option
			// ** For now there is no option to turn this on
int pt_opt;		// it need pt instantiation mechanism
int dtpt_opt=0;		// if need pt instantiation mechanism
int se_opt=0;		// silent on errors if not instantiating
bit stmtno = 0;

int inline_opt;

extern void simpl_init();
extern void typ_init();
extern void syn_init();
extern void lex_init();
extern void error_init();

char *st_name(char*);	// generates names of static ctor, dtor callers, ptbl_vec
Pname def_name;		// first definition in file
Pname pdef_name;	// used with ptbl_vec if function is first definition

int syn_count = 0; // to set conditional breakpoints to find particular expressions

void run()
/*
	run the appropriate stages
*/
{
	Pname n;
	templp = new templ_compilation;		// canonical instance

	while (n=syn()) {
		int oerror_count=error_count;
		if (n == Pname(1))
			continue;
		if (n->n_list)
			PERM(n->tp);

		DB( if(Rdebug>=1) error('d',"run: syn():%n%t base%k ll %d",n,n->tp,n->base,n->lex_level););
		templp->instantiate_ref_templ();

		for (Pname nx, nn=n; nn; nn=nx) {
			Pname rr;
			++syn_count;
			nx = nn->n_list;
			nn->n_list = 0;
			DB( if(Rdebug>=1) error('d',"run dcl:%n%t base%k ll %d",nn,nn->tp,nn->base,nn->lex_level);
			    if(Rdebug>=2)display_expr(nn,"nn");
			);

			if (
				(rr=nn->dcl(gtbl,EXTERN))==0
				||
				nn->base==0
				||
				(error_count>oerror_count)
			)
				continue;
			DB( if(Rdebug>=1)
				if(rr)error('d',"run rr:%n%t base%k ll %d",rr,rr->tp,rr->base,rr->lex_level);
				else error('d',"run rr: 0");
			    if(Rdebug>=2)display_expr(rr,"rr");
			);

			DB( if(Rdebug>=1) error('d',"run simpl:%n%t base%k ll %d",nn,nn->tp,nn->base,nn->lex_level););
			nn->simpl();

			if (error_count) {
				continue;
			}

			DB( if(Rdebug>=1) error('d',"run:print():%n (%k) tp%t",nn,nn->base,nn->tp););

			if (nn->base != TNAME) {
				Ptype t = nn->tp->skiptypedefs();

				switch (t->base) {
				case CLASS:
// error('d',"main:C:%n; n_redefined: %d", nn, nn->n_redefined);
 					DB( if(Rdebug>=1) fprintf(stderr," -- class\n"););
					if (vtbl_opt==1 || gplus_opt || nn->n_redefined == 1)
						nn->dcl_print(0);
					break;
				case ENUM:
 					DB( if(Rdebug>=1) fprintf(stderr," -- enum\n"););
					Penum(nn->tp)->dcl_print(0);
					break;
				case FCT:
					DB( if(Rdebug>=1) error('d'," -- fct%n inline %d body %d",nn,Pfct(t)->f_inline,Pfct(t)->body););
					if (
						Pfct(t)->body==0
						||
						(
							debug_opt==0
							&&
							Pfct(t)->f_inline
							&&
							(
								n->n_table!=gtbl
								||
								strcmp(nn->string,"main")
							)
						)
						||
						Pfct(t)->f_imeasure
						&&
						Pfct(t)->f_inline==0
					)
						break;
					DB( if(Rdebug>=1) error('d',"rr%n inline %d body %d",rr,Pfct(rr->tp)->f_inline,Pfct(rr->tp)->body););
					rr->dcl_print(0);
					break;
	
				default:
				{
					// print class def, if appropriate
					// can't use tsizeof() since this may
					// print an error if class isn't defined

					DB( if(Rdebug>=1) error('d'," --%n tpbase:%k",nn,nn->tp->base););

					for(Ptype tx = t;;) {
						switch ( tx->base ) {
						case VEC:
							tx=Pvec(tx)->typ;
							continue;
						case TYPE:
						case COBJ:
							tx=Pbase(tx)->b_name->tp;
							continue;
						case CLASS:
						{
							Pclass cl = Pclass(tx);
							if (
								(cl->defined & (DEFINED|SIMPLIFIED))
								&&
								cl->c_body==1
							)
								cl->dcl_print(0);
							break;
						}
						}
						break;
					}
					nn->dcl_print(0);
				}
				}
			}
			else
				nn->dcl_print(0);

			if (error_count)
				continue;

			DB( if(Rdebug>=1) error('d',"run: cleanup"););
			if (nn->base==TNAME) continue;
			switch (nn->tp->base) {			// clean up
			default:
			{	
				Pexpr i = nn->n_initializer;
				DB( if(Rdebug>=1) error('d',"default nn%n i %d",nn,i););
				if (i && i!=Pexpr(1)) {
					DEL(i);
					nn->n_initializer = 0;
					if (def_name==0 && rr->n_scope != STATIC) { 
						def_name = rr;
					}
				}
				break;
			}

			case FCT:
			{	
				if (nn->n_oper == CTOR) break; // at least for now, need the f_this
				Pfct f = Pfct(nn->tp);
				DB( if(Rdebug>=1) error('d', "function nn:%n local_class: %d %d", nn, local_class, f->local_class ););
				if (f->body && f->f_inline==0 && f->f_imeasure==0) {
					if ( local_class = f->local_class ) { 
						delete_local();
						local_class = 0;
					}

					if (ansi_opt && f->f_this) {
						f->f_this->n_table = 0;
						for (Pname n=f->f_this->n_list; n; n=n->n_list)
							n->n_table = 0;
					}

					DEL(f->body);
					if (def_name==0) { 
						def_name = rr;
					}
				}
				break;
			}

			case CLASS:
			{
				Pclass cl = Pclass(nn->tp);
				for (Pname px, p=cl->mem_list; p; p=px) {
					px = p->n_list;
					p->n_list = 0;
					if (p->tp) {
						switch (p->tp->base) {
						case FCT:
						{
							Pfct f = (Pfct)p->tp;
							if (f->body && f->f_inline==0 && f->f_imeasure==0) {
								if ( local_class = f->local_class ) {
									delete_local();
									local_class = 0;
								}
								DEL(f->body);
								f->body = 0;
							}
						}
						case CLASS:
						case ENUM:
						case TYPE:
							break;
						case COBJ:
						case EOBJ:
							DEL(p);
							break;
						default:
							delete p;
						}
					}
					else {
						delete p;
					}
				}	// for
				cl->mem_list = 0;
				cl->permanent = 3;
				break;
			}
			}
			DEL(nn);
			Ntmp_dtor = 0;
		}
		lex_clear();
	}

	templp->end_of_compilation() ;

	if (error_count)
		return;

	int i = 1;
	for (Pname m=gtbl->get_mem(i); m; NEXT_NAME(gtbl,m,i)) {
		if (
			m->base==TNAME
			||
			m->n_sto==EXTERN
			||
			m->n_stclass == ENUM
		)
			continue;

		Ptype t = m->tp;
		if (t == 0)
			continue;

		switch (t->base) {
		case CLASS:
		case ENUM:
		case OVERLOAD:
			continue;
		case COBJ:
		case VEC:
			break;
		case FCT:
			if (Pfct(t)->f_inline || Pfct(t)->body==0)
				continue;
			break;
		}

//error('d',&m->where,"%n %d %d %s",m,m->n_addr_taken,m->n_used,m->n_sto==STATIC?"static":"");
//xxx doesn't check anon union members as their use bits are set differently
		if (
			m->n_addr_taken==0
			&&
			m->n_used==0
			&&
			m->n_sto == STATIC
		) {				// (static or anon?)
			for(;;) {
//error('d',&m->where,"  %t tconst(): %d",t,t->tconst());
				if(t->base==TYPE) {
					if(t->tconst())
						break;
					t=Pbase(t)->b_name->tp;
				}
				else if(t->base==VEC) {
					t=Pvec(t)->typ;
				}
				else {
					if ( t->tconst() == 0 ) {
//error('d',"m%n tp%t t%t",m,m->tp,t);
						if ( t->base != COBJ )
							error('w',&m->where,"%n defined but not used",m);
						else {
							Pclass cl = Pclass(Pbase(t)->b_name->tp);
							if ( cl->has_ctor()==0 && cl->csu != ANON )
								error('w',&m->where,"%n defined but not used",m);
						}
					}	// if const
					break;
				}
			}	// for
		} // if static and not used
	}

	Pname ctor = 0;
	Pname dtor = 0;
	--curloc.line;

	if (st_ilist) {		// make an "init" function;
				// it calls all constructors for static objects
		DB( if(Rdebug>=1) error('d',"make sti"); );
		Pname n = new name( st_name("__sti__") );
		Pfct f = new fct(void_type,0,1);
		n->tp = f;
		f->body = new block(st_ilist->where,0,st_ilist);
		f->body->memtbl = sti_tbl;
		n->n_sto = EXTERN;
		ignore_const++;			// assignments here are really initializations:
		f->f_linkage = linkage_C;
		f->sign();
		(void) n->dcl(gtbl,EXTERN);
		ignore_const--;
		n->simpl();
		n->dcl_print(0);
		ctor = n;
	}

	if (st_dlist) {		// make a "done" function;
				// it calls all destructors for static objects
		DB( if(Rdebug>=1) error('d',"make std"); );
		Pname n = new name( st_name("__std__") );
		Pfct f = new fct(void_type,0,1);
		n->tp = f;
		f->body = new block(st_dlist->where,0,st_dlist);
		f->body->memtbl = std_tbl;
		n->n_sto = EXTERN;
		f->f_linkage = linkage_C;
		f->sign();
		(void) n->dcl(gtbl,EXTERN);
		n->simpl();
		n->dcl_print(0);
		dtor = n;
	}

		/*For fast load: make a static "__link" */
 		/*
 		 * plan 9:
		 * always do it, since it doesn't interfere with munch
		 * add arguments so it will type check
		 */
	if (ctor || dtor) {
		printf("static struct __linkl { struct __linkl * next;\n");
		if ( ansi_opt )
			printf("void (*ctor)(void); void (*dtor)(void); } __link = \n");
		else
			printf("char (*ctor)(); char (*dtor)(); } __link = \n");
		putstring("{ (struct __linkl *)0, ");
		if (ctor)
			ctor->print();
		else
			putch('0');
		putch(',');
		if (dtor)
			dtor->print();
		else
			putch('0');
		putstring("};\n");
	}

	 DB( if(Rdebug>=1) error( 'd', "run: vlist: %d", vlist ); );
	 do {
		for (vl* v = vlist; v; v = v->next)
				v->cl->really_print(v->vt);
		vlist = 0;

		for (Plist l=isf_list; l; l=l->l) {
			Pname n = l->f;
			Pfct f = Pfct(n->tp);
	DB( if(Rdebug>=1)error('d',"isf%n%t f %d addr %d",n,f,f,n->n_addr_taken); );
			if (f->base == OVERLOAD) {
				n = Pgen(f)->fct_list->f;	// first fct
				f = Pfct(n->tp);
			}

			if (debug_opt==0 && n->n_addr_taken) {
				f->f_inline = 0;
				if (n->n_dcl_printed<2) {
					if (f->fct_base == INSTANTIATED)
						current_fct_instantiation = f;
					if (warning_opt)
						error('w',"out-of-line copy of%n created",n);
					n->dcl_print(0);
					if (f->fct_base == INSTANTIATED)
						current_fct_instantiation = 0;
				}
			}
		}
	}	while (vlist);

	if (strict_opt == 0) {		// define static members 
					// patch for SysV VAX -g linkage botch
		for (Plist l=stat_mem_list; l; l=l->l) {
			extern int ispt_data(Pname);
			Pname n = l->f;
			if (ispt_data(n))
				continue;
// error('d',"stat mem%n %d %d ",n,n->n_initializer,n->n_evaluated);
			if (n->n_initializer==0 && n->n_evaluated==0) {
				Ptype t = n->tp;
				n->n_sto = 0;
				Pname cn = t->is_cl_obj();
				if (cn==0)
					cn = cl_obj_vec;

				if (cn && ((Pclass(cn->tp)->defined&(DEFINED|SIMPLIFIED)))
) { 
					fake_sizeof=1;
					Loc oldloc=curloc;
					curloc=n->where;
					(void) t->tsizeof();	// be sure to print class
					curloc=oldloc;
				
					fake_sizeof=0;
				}

//error('d',"t->is_ref() is %d",t->is_ref());
//error('d',"pclass(cn->tp)->has_ctor() is %d",Pclass(cn->tp)->has_ctor());

				if (
					cn
					&&
					( Pclass(cn->tp)->has_ctor()
					||
					t->is_ref()
					||
					((Pclass(cn->tp)->defined&(DEFINED|SIMPLIFIED))==0)
					)
				)
					;	// force explicit definition
				else {
					if (n->tp->base==VEC && Pvec(n->tp)->size==0)
						; // force explicit def
					else {
						Loc oldloc=curloc;
						curloc=n->where;
						fake_sizeof=1;
						n->dcl_print(0);
						fake_sizeof=0;
						curloc=oldloc;
					}
				}


			}
		}
	}

	i = 1;
	for (Pname nm=ptbl->get_mem(i); nm; NEXT_NAME(ptbl,nm,i)) {
		DB( if(Rdebug>=1) error('d',"ptbl anme %s string2 %s key:%k",nm->string==0?"???":nm->string2,nm->string2==0?"???":nm->string2,nm->n_key); );
		if ( nm->n_key == 0 ) {		// ptbl used in file, generate definition
			int str1 = (*src_file_name) ? strlen(src_file_name)+2 : 0; // +2 for __
			int str2 = strlen(nm->string2) - str1;
			char *ps = new char[ str2 + 1 ];
			strncpy( ps, nm->string2, str2 ); // grab vtbl name
			ps[str2]='\0';
			ps[2] = 'v';
			fprintf(out_file,"extern struct __mptr %s[];\n",ps);
			ptbl_add_pair(nm->string2,ps);
			delete ps;
		}
	}

	ptbl_init(1);
	curloc.putline();
	fprintf(out_file,"\n/* the end */\n");
}

void set_flags() 
{
	all_flag=0;
	alltc_flag=0;
	data_flag=0;
	ft_flag=0;
	none_flag=0;
	for (int i=0;i<noinst;i++) {
		if (strcmp("@all",instfct[i])==0) 
			all_flag=1;
		if (strcmp("@alltc",instfct[i])==0) 
			alltc_flag=1;
		if (strcmp("@data",instfct[i])==0) 
			data_flag=1;
		if (strcmp("@ft",instfct[i])==0)
			ft_flag=1;
		if (strcmp("@none",instfct[i])==0)
			none_flag=1;
	}
}


#ifdef DBG
int Adebug = 0;
int Bdebug = 0;
char* Bdebarg = 0;
int Ddebug = 0;
int Edebug = 0;
int Kdebug = 0;
int Ldebug = 0;
int Mdebug = 0;
int Ndebug = 0;
int Pdebug = 0;
int Rdebug = 0;
int Sdebug = 0;
int Tdebug = 0;
int Ydebug = yydebug;

void
process_debug_flags( char* p )
{
	// arg to +Dxxx or debugging comments //@!xxx
	// format of xxx == sequence of any combination of
	//	+flags  --  increment flags (default)
	//	-flags  --  decrement flags
	//	0flags  --  unset flags
	// where flags are single chars
	int incr = 1;
	char c;
	fprintf(stderr,"\n*** processing debugging flags '%s'\n",p);
	while ( c = *p++ ) {
		switch ( c ) {
		case '+': incr =  1; break;
		case '-': incr = -1; break;
		case '0': incr =  0; break;
		case ' ': break;
		// flags...
		case 'A': if(incr==0) Adebug=0; else Adebug+=incr; break;
		case 'B':
			if(incr==0) Bdebug=0; else Bdebug+=incr;
			if ( *p == ':' ) {
			    if ( *++p ) {
				Bdebarg = new char[strlen(p)+1];
				strcpy(Bdebarg,p);
			    } else Bdebarg = 0;
			    p = "";
			}
			break;
		case 'D': if(incr==0) Ddebug=0; else Ddebug+=incr; break;
		case 'E': if(incr==0) Edebug=0; else Edebug+=incr; break;
		case 'K': if(incr==0) Kdebug=0; else Kdebug+=incr; break;
		case 'L': if(incr==0) Ldebug=0; else Ldebug+=incr; break;
		case 'M': if(incr==0) Mdebug=0; else Mdebug+=incr; break;
		case 'N': if(incr==0) Ndebug=0; else Ndebug+=incr; break;
		case 'P': if(incr==0) Pdebug=0; else Pdebug+=incr; break;
		case 'R': if(incr==0) Rdebug=0; else Rdebug+=incr; break;
		case 'S': if(incr==0) Sdebug=0; else Sdebug+=incr; break;
		case 'T': if(incr==0) Tdebug=0; else Tdebug+=incr; break;
		case 'Y': if(incr==0) Ydebug=0; else Ydebug+=incr; break;
		default: error('w',"unknown debugging flag '%c'",c);
		}
	}
}
#endif /*DBG*/

main(int argc, char* argv[])
/*
	read options, initialize, and run
*/
{
	register char * cp;
	char* afile = "";

	prog_name = argv[0];
#ifdef unix

#ifdef COMPLETE_SIG_PF
	signal(SIGILL,core_dump);
	signal(SIGIOT,core_dump);
	signal(SIGEMT,core_dump);
	signal(SIGFPE,core_dump);
	signal(SIGBUS,core_dump);
	signal(SIGSEGV,core_dump);
#else
	typedef void (*ST)(int ...);	// trick to circumvent problems with old
	ST sick = ST(&signal);		// (or C) versions <signal.h>
	(*sick)(SIGILL,core_dump);
	(*sick)(SIGIOT,core_dump);
	(*sick)(SIGEMT,core_dump);
	(*sick)(SIGFPE,core_dump);
	(*sick)(SIGBUS,core_dump);
	(*sick)(SIGSEGV,core_dump);

#endif
#endif

	error_init();
#ifdef LICENSE_MAN
        sde_check_license("cfront",CFRONTNUM, 0);
#endif

	for (int i=1; i<argc; ++i) {
		switch (*(cp=argv[i])) {
		case '+':
			while (*++cp) {
				switch(*cp) {
				case 'U':
					dem_opt_local = 1;
					dem_opt_mem = 1;	
					break;
				case 'I':
					inline_opt = 1;
					break;
				case 'J':
					extern int size_complexity_measure;
					size_complexity_measure = atoi(cp + 1);
					while (*++cp)
						;
					cp--;
					break;
				case 'f':
					src_file_name = cp+1;
					goto xx;
				case 'x':	// read cross compilation table
					if (read_align(afile = cp+1)) {
						fprintf(stderr,"%s: %s bad size-table (option +x)\n",
							prog_name, afile);
						exit(11);
					}
					goto xx;
				case 'e':
					switch (*++cp) {
					case '0':
					case '1': 
						vtbl_opt = *cp-'0';
						break;
					default:
						fprintf(stderr,"%s: bad +e option\n", prog_name);
						exit(11);
					}
					break;
				case 'd':
					debug_opt = 1;
					break;
				case 'g':
					gplus_opt = 1;
					break;
				case 'D':
#ifdef DBG
					process_debug_flags(++cp);
#else
					error('w',"cfront not compiled for debugging -- +D%s ignored",++cp);
#endif
					goto xx;
				case 'w':
					warning_opt = 1;
					break;
				case 'a':
					switch (*++cp) {
					case '0':
					case '1': 
						ansi_opt = *cp-'0';
						break;
					default:
						fprintf(stderr,"%s: bad +a option\n", prog_name);
						exit(11);
					}
					break;
				case 'p':
					strict_opt = 1;
					break;
				case 'L':
					line_format = "\n#line %d \"%s\"\n";
					break;
				case 's':
					se_opt=1;
					break;
				case 't':
					pt_opt = 1;
					pt_file = fopen (cp+1, "w");
					if (pt_file == NULL) {
						fprintf(stderr, "%s: %s - open failed\n", prog_name, cp+1);
						exit(11);
					}
					goto xx;
				case 'T': 
					{
					dtpt_opt=1;
					dtpt_file = fopen(cp+1,"r");
					if (dtpt_file == NULL) {
						fprintf(stderr,"%s: %s - open failed\n", prog_name,cp+1);
						exit(11);
					}
					char tempstring[1024];
					noinst=0;
					while (fscanf(dtpt_file,"%s",tempstring)!=EOF) {
						instfct.reserve(noinst);
						instfct[noinst]=new char[strlen(tempstring)+1];
						strcpy(instfct[noinst],tempstring);
						noinst++;
						if (strcmp(tempstring,"@data")==0) {
							bit EOLflag=0;
							char c;
							nodatainst=0;
							int index=0;
							char ntempstring[1024];
							while (EOLflag==0 && fscanf(dtpt_file,"%c",&c)!=EOF)  
{
								switch (c) {
									case '\n': 
										EOLflag = 1;
									case ' ': 
									case '\t': 
										ntempstring[index]='\0';
										if (index!=0) {
										instdata[nodatainst]=new char [strlen(ntempstring)+1];
										strcpy(instdata[nodatainst],ntempstring);
										nodatainst++;
								}
										index=0;	
										break;
									default: 
										if (index==0) instdata.reserve(nodatainst);
										ntempstring[index]=c;
										index++;
										break;
								} /* switch */
							}
} /* if @data */
					}; 
					set_flags();
					}
					goto xx;
				default:
					fprintf(stderr,"%s: +%c bad option, ignored\n",prog_name, *cp);

				}
			}
		xx:
			break;
		default:
			fprintf(stderr,"%s: '%s' - bad argument\n",prog_name, cp);
			exit(11);
		}
	}


	fprintf(out_file,line_format+1,1,src_file_name); // strips leading \n
	fprintf(out_file,"\n/* %s */\n",prog_name);
	fprintf(out_file,"char %s;\n",prog_vers);
	if (*src_file_name)
		fprintf(out_file,"/* < %s > */\n",src_file_name);

	fflush(stderr);
	otbl_init();
	lex_init();
	syn_init();
	typ_init();
	simpl_init();
	ptbl_init(0);
	scan_started = 1;
	curloc.putline();
	if ((BI_IN_BYTE==0) && (SZ_INT==0) && (SZ_WPTR==0))
		error("no size/alignment values - use +x of provide #ifdef in size.h");
	run();

	exit( (0<=error_count && error_count<127) ? error_count : 127);
}

char* st_name(char* pref)
/*
	make name "pref|source_file_name|_" or "pref|source_file_name|_"
	where non alphanumeric characters are replaced with '_'
	and add def_name at end to ensure uniqueness
*/
{
	int prefl = strlen(pref);
	int strl = prefl + 2;	// trailing '_' and 0
	if (*src_file_name)
		strl += strlen(src_file_name);
	char* defs;
	int defl;
	if (def_name) {
		defs = def_name->string;
		defl = strlen(defs)+1;			// '_'
	}
	else {
		defs = 0;
		defl = 0;
	}
	char* name = new char[strl+defl];
	strcpy(name,pref);
	if (*src_file_name)
		strcpy(name+prefl,src_file_name);
	name[strl-2] = '_';
	name[strl-1] = 0;
	for (char* p = name; *p; p++)
		if (!isalpha(*p) && !isdigit(*p))
			*p = '_';
	if (defs) {
		strcpy(name+strl-1,defs);	// after the '_'
		name[strl+defl-2] = '_';
		name[strl+defl-1] = 0;
	}
#ifdef DENSE
	void chop(char*);
	chop(name);
#endif

	return name;
}
