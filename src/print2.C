/*ident	"@(#)cls4:src/print2.c	1.36" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992  AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

print2.c:

	print names and declarations

****************************************************************************/

#include "cfront.h"
#include "template.h"

bit TCast;
bit Cast;
int last_ll = 1;
Pin curr_icall;
char emode;
int ntok;
bit mk_zero_init_flag=0;

static int MAIN;	// fudge to get _main() called by main()

#define eprint(e) if (e) Eprint(e)

#ifdef DENSE
void chop(char*);
#endif

static Ptable mem_table;
static Ptable perm_table;
static int mem_depth;
extern int dem_opt_mem;
extern int dem_opt_local;

int ispt_data(Pname n)
{
	if (!n->tp || !n->n_table || !n->n_table->t_name || !n->n_table->t_name->tp)
		return 0;
	char* s = Pclass(n->n_table->t_name->tp)->string;
	while (*s) {
		if (!strncmp(s, "__pt__", 6))
			return 1;
		s++;
	}
	return 0;
}

static void begin_mem()
{
	if (!dem_opt_mem)
		return;
	if (!mem_depth++)
		mem_table = new table(16, 0, 0);
}

static void end_mem()
{
	if (!dem_opt_mem)
		return;
	if (!--mem_depth) {
		delete mem_table;
		mem_table = 0;
	}
}

static int dem_mem(char* cn, char* mn)
{
	char buf[1024];
	Pname n;

	if (!cn || !mn || !*cn || !*mn)
		error('i', "invalidA to dem_mem()");

	sprintf(buf, "%s__%s", cn, mn);

	if (mem_table) {
		if (mem_table->look(mn, 0)) {
			if (!perm_table)
				perm_table = new table(16, 0, 0);
			n = new name(buf);
			perm_table->insert(n, 0);
			delete n;
			return 0;
		}
		else {
			n = new name(mn);
			mem_table->insert(n, 0);
			delete n;
			return 1;
		}
	}
	else {
		return !perm_table || !perm_table->look(buf, 0);
	}
}

void puttok(TOK t)
/*
	print the output representation of "t"
*/
{
	putstring(keys[t]);

	if (12<ntok++) {
		ntok = 0;
		if (emode==0) last_line.putline();
	}
	else if (t == SM) {
		ntok = 0;
		putch('\n');
		if (last_ll) last_line.line++;
	}
	else
		putch(' ');
}

#define MX	20
#define NTBUF	10
static class dcl_buf {
	/*
		buffer for assembling declaration (or cast)
		left contains CONST_PTR	=> *CONST
			     CONST_RPTR => &CONST
				PTR	=> *
				RPTR	=> &
				LP	=> (
		right contains	RP	=> )
				VEC	=> [ rnode ]
				FCT	=> ( rnode )
				FIELD	=> : rnode
	*/
	Pbase b;
	Pname n;
	TOK left[MX], right[MX];
	Pnode	rnode[MX];
	Pclass	lnode[MX];
	int li, ri;
	bit cf;
public:
	void	init(Pname nn)		{ b=0; n=nn; cf=li=ri=0; }
	void	base(Pbase bb, bit const_flag = 0)	{ b = bb; cf = const_flag;}
	void	front(TOK t)		{ left[++li] = t; }
	void	front(Pclass c)		{ left[++li] = MEMPTR; lnode[li] = c; }
	void	back(TOK t, Pnode nod)	{ right[++ri] = t; rnode[ri] = nod; }
	void	paran() 		{ front(LP); back(RP,0); }
	void	put();
} *tbufvec[NTBUF] = {0}, *tbuf = 0;

static int freetbuf = 0;

void dcl_buf::put()
{
	int i;
	Pfct ff = 0;

	if (MX<=li || MX<=ri) error('i',"T buffer overflow");

	if (n && n->n_sto && n->n_sto!=REGISTER && n->n_sto!=AUTO) puttok(n->n_sto);

	if (b == 0)
#ifdef DBG
	    if ( emode ) {
		fprintf(out_file,"?b==0?");
		return;
	    }
	    else
#endif
		error('i',"noBT%s",Cast?" in cast":"");

	else {
	    bit ac_save;
	    if (cf) {
		ac_save = b->ansi_const;
		b->ansi_const = 1;
	    }
	    b->dcl_print();
	    if (cf)
		b->ansi_const = ac_save;
	}
	
	for( ; li; li--) {
		switch (left[li]) {
		case LP:
			putch('(');
			break;
		case PTR:
			putch('*');
			break;
		case RPTR:
			if (emode)
				putch('&');
			else
				putch('*');
			break;
		case CONST_PTR:
			if (emode||ansi_opt && Cast==0)
				putstring("*const ");
			else
				putch('*');
			break;
		case CONST_RPTR:
			if (emode)
				putstring("&const ");
			else if (ansi_opt && Cast==0)
				putstring("*const ");
			else
				putch('*');
			break;
		case MEMPTR:
			if (lnode[li]) fprintf(out_file,"%s::",lnode[li]->string);
		}
	}

	if (n) n->print();

	for(i=1; i<=ri; i++) {
		switch (right[i]) {
		case RP:
			putch(')');
			break;
		case VEC:
			putch('[');
			{	Pvec v = (Pvec) rnode[i];
				Pexpr d = v->dim;
				int s = v->size;
				if (d) d->print();
				if (s) fprintf(out_file,"%d",s);
			}
			putch(']');
			break;
		case FCT:	// beware of function returning pointer to
				// function expressed witout typedef
		{	Pfct f = Pfct(rnode[i]);
			if (f->body) ff = f;
			f->dcl_print();
			break;
		}
		case FIELD:
		{
			Pbase f = (Pbase) rnode[i];
			Pexpr d = (Pexpr)f->b_name;
			int s = f->b_bits;
			putch(':');
			if (d)
				d->print();
			else if (s)
				fprintf(out_file,"%d",s);
			else
				puttok(ZERO);
			break;
		}
		}
	}
void print_body(Pfct);
	if (ff && emode==0) print_body(ff);
}

Pexpr mk_zero_init( Ptype tt, Pname obname, Pname currname )
/*
	creates 0 initializer for defined objects
	side effect: generates right nesting level for {}
*/
{
	if (dtpt_opt && mk_zero_init_flag)
		return 0;

	tt = tt->skiptypedefs();

	switch (tt->base) {
	case VEC: // type a[size]; => type a[size] = {0};
	{
		if ( obname == currname && !ispt_data(obname)) {
		//xxx initializing vectors blows up the size of some programs
			return 0;
		}
		else {
			Pexpr i = mk_zero_init(Pvec(tt)->typ,obname,currname);
			if ( i == 0 ) return 0;
			return new expr( ILIST, i, 0 );
		}
	}
	case COBJ: // "X a;" == "X a = {0};"
	{
		Pclass cl = Pclass(Pbase(tt)->b_name->tp);
		if ( !ansi_opt && (cl->csu == ANON || cl->csu == UNION) ) {
			if ( warning_opt ) {
			// ridiculous handstands to avoid printing
			// compiler generated names in warning
				Ptype tn = obname->tp;
				for(;;)
					if(tn->base==VEC)
						tn=Pvec(tn)->typ;
					else
						break;
				if ( obname == currname && tt == tn ) {
					if ( cl->string[0]=='_' && cl->string[1]=='_' )
						error('w',&obname->where,"cannot generate zeroIr for%n ofT union; toI, compile with +a1 or insert non-union object as first member",obname);
					else
						error('w',&obname->where,"cannot generate zeroIr for%n ofT%t; toI, compile with +a1 or insert non-union object as first member",obname,obname->tp);
				}
				else {
					Pclass cx;
					if (
						tn->base == COBJ
						&&
						(cx=(Pclass)Pbase(tn)->b_name->tp)
						&&
						cx->string[0]=='_'
						&&
						cx->string[1]=='_'
					)
						error('w',&obname->where,"cannot generate zeroIr for%n (union or aggregate with union as first element); toI, compile with +a1 or insert non-union object as first member",obname);
					else
						error('w',&obname->where,"cannot generate zeroIr for%n ofT%t (union or aggregate with union as first element); toI, compile with +a1 or insert non-union object as first member",obname,obname->tp);
				}
			} // if warning_opt
			return 0;
		}
		Pbcl l = cl->baselist;
		while ( l ) {
			if ( l->base == NAME ) cl = l->bclass;
			else return new expr(ILIST,zero,0);
			l = cl->baselist;
		}
		int i = 1;
		Pname nn = cl->memtbl->get_mem(i);
		for ( ;  nn;  NEXT_NAME(cl->memtbl,nn,i) ) {
			if ( nn->base == TNAME || nn->tp==0 ) continue;
			Ptype tx = nn->tp->skiptypedefs();
			if (nn->base==NAME
			&& nn->n_anon==0
			&& tx->base!=FCT
			&& tx->base!=OVERLOAD
			&& tx->base!=CLASS
			&& tx->base!=ENUM
			&& nn->n_stclass != STATIC
			&& nn->n_stclass != ENUM) {
				Pexpr i = mk_zero_init(tx,obname,nn);
				if ( i == 0 ) return 0;
				return new expr(ILIST,i,0);
			}
		}
		return 0;
	}
	case PTR:
		if (tt->memptr()) {
			Pexpr i = new expr(ELIST,zero,zero);
			return new expr(ILIST,i,zero);
		}
		// no break
	case RPTR:
	case CHAR:
	case SHORT:
	case INT:
	case EOBJ:
	case LONG:
	case VLONG:
	case FLOAT:
	case DOUBLE:
	case LDOUBLE:	// "int a;" == "int a = 0;"
	case FIELD:
	case ANY:
		return zero;
	}
	return 0;
}

void fct::arg_print()
/* print function arguments and type of function */
{
	putch('(');
	for (Pname nn=argtype; nn;) {
		nn->tp->dcl_print(0);
		if (nn=nn->n_list)
			puttok(CM);
		else
			break;
	}
	switch (nargs_known) {
	case 0:		//	putst("?"); break;
	case ELLIPSIS:	puttok(ELLIPSIS); break;
	}

	putch(')');

	if (f_const)
		puttok(CONST);	
	if (f_static)
		puttok(STATIC);	// wrong place for ``static''
}


void name::dcl_print(TOK list)
/*
	Print the declaration for a name (list==0) or a name list (list!=0):
		For each name
		(1) print storage class
		(2) print base type
		(3) print the name with its declarators
	Avoid (illegal) repetition of basetypes which are class or enum declarations
	(A name list may contain names with different base types)
	list == SM :	terminator SM
	list == 0:	single declaration with terminator SM
	list == CM :	separator CM
*/
{
	if (error_count) return;

//error('d',"name::dcl_print this is %n",this);

	if (n_stclass==STATIC)
		tp->ansi_const=1;

	for (Pname n=this; n; n=n->n_list) {
		Ptype t = n->tp;
		int sm = 0;

		if (t == 0) error('i',"N::dcl_print(%n)T missing",n);

		if (n->n_stclass==ENUM) if (list) continue; else return;

		if (n->where.line!=last_line.line || n->where.file!=last_line.file)
			if (last_ll = n->where.line)
				n->where.putline();
			else
				last_line.putline();
	
		bit tc = 0;
		Ptype tt = t->skiptypedefs(tc);

		switch (t->base) {
		case CLASS:
			if (n->base != TNAME) {
				Pclass(t)->dcl_print(n);
			}
			sm = 1;
			break;

		case ENUM:
			Penum(t)->dcl_print(0);
			sm = 1;
			break;

		case FCT:
		{	
			Pfct f = Pfct(t);




			if (	dtpt_opt && 
				all_flag==0 && 
				curloc.file!=first_file && 
				f->f_inline==0 && 
				f->f_is_inline==0 && 
				!(n->n_sto==STATIC && !n->n_stclass) &&
				none_flag==0) 
					f->body=0;

			if (t->is_templ_instance() && 
                		current_fct_instantiation != f) 
			{
				if (n->n_addr_taken == 1 &&
					current_fct_instantiation == 0 &&
				       	(f->f_inline || f->last_expanded)) 
					// special case of inline needing to printed
					// permit it to continue ...
						;
				else return;
			}

			if (n->base == TNAME) puttok(TYPEDEF);

			if (
				n->n_dcl_printed==2	// definition already printed
				||
				n->n_dcl_printed==1 && f->body==0	// declaration already printed
			) {
				// don't print again
				sm = 1;	// no SM
				break;
			}

			if (f->f_result == 0) make_res(f);

			if (f->body && n->n_sto==EXTERN) n->n_sto = 0;

			if (
				f->f_inline
				&&
				(n->n_table!=gtbl || strcmp(n->string,"main"))
			) {
				if (debug_opt)  {
					if (
						f->defined & DEFINED
						&&
						f->defined & SIMPLIFIED
						&&
						f->f_inline != ITOR
					)
						goto prnt_def;
					else if (n->n_dcl_printed == 0)
						goto prnt_dcl;
					else {
						sm = 1;
						break;
					}
				}
				if (f->f_virtual || n->n_addr_taken) {
				prnt_dcl:
					TOK st = n->n_sto;
					Pblock b = f->body;
					f->body = 0;
					t->dcl_print(n);
					n->n_dcl_printed = 1;
					n->n_sto = st;
					f->body = b;
					break;
				}
				else
					sm = 1;	// no SM
			}
			else if (
				(f->defined&DEFINED)==0
				||
				(f->defined&SIMPLIFIED)==0
			)
				goto prnt_dcl;
			else if (
				n->n_table==gtbl
				&&
				strcmp(n->string,"main")==0
			) {
				if (f->f_inline) {
					error(&n->where,"main() cannot be declared inline");
					f->f_inline=0;
					n->n_sto=0;
				}
				if (n->n_sto==STATIC) {
					error(&n->where,"main() cannot be declared static");
					n->n_sto=0;
				}
				MAIN = 1;
				gtbl->look("main",0)->use();
				f->f_signature = 0;
				t->dcl_print(n);
				n->n_dcl_printed = f->body?2:1;
				MAIN = 0;
			}
			else {
			prnt_def:
				if (n->n_oper==CTOR || n->n_oper==DTOR) {
					Pclass cl = Pclass(n->n_table->t_name->tp);
					if (cl->c_body == 3) cl->print_all_vtbls(cl);
				}

				if (
					n->n_sto == STATIC
					&& 
					pdef_name == 0
					&&
					def_name == 0
				) {
					pdef_name = n;
					if (last_ll = n->where.line)
						n->where.putline();
					else
						last_line.putline();
				}
				t->dcl_print(n);
				n->n_dcl_printed = f->body?2:1;
			}
			if (f->body) sm = 1;
			break;
		}

		case OVERLOAD:
		{
			for (Plist gl=Pgen(t)->fct_list; gl; gl=gl->l) {
				Pname nn = gl->f;
				Pfct f=(Pfct) nn->tp;
				if (f->defined) nn->dcl_print(0);
				sm = 1;
			}
			break;
		}

		case ASM:
			fprintf(out_file,"asm(\"%s\")\n",(char*)Pbase(t)->b_name);
			break;

		case INT:
		case EOBJ:
		case CHAR:
		case LONG:
		case SHORT:
		tcx:
			// do not allocate space for constants unless necessary


			if (
				tc
				&&
				n->n_sto!=EXTERN	// extern const one;
							// const one = 1;
							// allocates storage
				&&
				(
					n->n_scope==EXTERN	// FUDGE const one = 1;
								// is treated as static
								// need loader support
					||
					n->n_scope==STATIC
					||
					n->n_scope==FCT
				)
			) {
				if (n->n_evaluated && n->n_addr_taken==0) {
					sm = 1;	/* no ; */
					break;
				}
			}
			tc = 0;

			// no break;

		default:
		{


			Pexpr saveinit=n->n_initializer;
			TOK savesto=n->n_sto;

			Pexpr i = n->n_initializer;
			if (n->base == TNAME && n->tpdef ) i = 0;

			if (	dtpt_opt && 
				all_flag==0 && 
				n->n_scope!=STATIC && 
				none_flag==0 &&
				curloc.file != first_file
			) 
			{
				if (n->base==TNAME && n->tpdef)
					; // do nothing
				else {
					n->n_initializer=i=0;
					mk_zero_init_flag=1;
					if (n->n_scope==EXTERN && Pbase(t)->b_const==0)
						n->n_sto=EXTERN;
				}
			} 

			if (tc) {
				switch (tt->base) {
				case CHAR:
				case SHORT:
				case INT:
				case LONG:
				case EOBJ:
					goto tcx;
				}
			}

			if (n->base == TNAME) {
// error('d',"%s->dcl_print %k lex_level: %d", n->string, n->base, n->lex_level );
       			/* Always print template formals, even when they have the same 
			 * formal name, since the instantiation name is different. This 
			 * fix should not be required when the copy mechanism is in place. */
          			if (! n_template_arg) {
				//  Pname tn=k_find_name(n->string,Ctbl,HIDDEN);
     				//  if (tn && tn->lex_level && t==tn->tp)
				//	return;
				    if ( n->lex_level ) return;
				}
				puttok(TYPEDEF);
			}

			if (n->n_stclass == REGISTER) {
				// (imperfect) check against member functions
				// register s a; a.f() illegal
				Pname cln = n->tp->is_cl_obj();
				if (cln) {
					Pclass cl = Pclass(cln->tp);
					if (
						cl->csu!=CLASS
						&&
						cl->baselist==0
						&&
						cl->has_itor()==0
						&&
						cl->virt_count==0
						&&
						!n->n_addr_taken
					)
						puttok(REGISTER);
				}
				else if (!n->n_addr_taken)
					puttok(REGISTER);
			}

			if (i) {
				if (n->n_sto==EXTERN && n->n_stclass==STATIC) {
					n->n_initializer = 0;
					t->dcl_print(n);
					puttok(SM);
					n->n_initializer = i;
					n->n_sto = 0;
					t->dcl_print(n);
					n->n_sto = EXTERN;
				}
				else
					t->dcl_print(n);
				if(n->n_table) {
					Pname nn = n->n_table->look(n->string,0);
					if(nn)
						nn->n_dcl_printed = 1;
					else
						n->n_dcl_printed = 1;
				}
				else
					n->n_dcl_printed = 1;
			}
			else if (n->n_evaluated && Pbase(t)->b_const) {
				if (n->n_sto==EXTERN && n->n_stclass==STATIC) {
					int v = n->n_evaluated;
					n->n_evaluated = 0;
					t->dcl_print(n);
					puttok(SM);
					if (n->where.line!=last_line.line || n->where.file!=last_line.file)
						if (last_ll = n->where.line)
							n->where.putline();
						else
							last_line.putline();
					n->n_evaluated = v;
					n->n_sto = 0;
					t->dcl_print(n);
					n->n_sto = EXTERN;
				}
				else
					t->dcl_print(n);
				if(n->n_table) {
					Pname nn = n->n_table->look(n->string,0);
					if(nn)
						nn->n_dcl_printed = 1;
					else
						n->n_dcl_printed = 1;
				}
				else
					n->n_dcl_printed = 1;
			}
			else {
 				Ptype tempt=t->skiptypedefs();
 				if (tempt->base==COBJ) {
 					Pname cln=n->tp->is_cl_obj();
 					if (cln) {
 						Pclass cl= (Pclass) cln->tp;
 						Pname ctor= cl->has_ctor();
 						if (
							n->n_table!=gtbl
 							&&
							n->n_sto!=EXTERN
 							&&
							n->n_evaluated==0
 							&&
							n_stclass==STATIC
 							&&
							ctor
						)
 							n->n_initializer = i =mk_zero_init(t,n,n);
 					}
 				}
				if (
					(
						n->n_sto==0
						||
						n->n_val && n->n_evaluated==0
					)
					&&
					n_stclass==STATIC
					&&
					n->n_sto!=STATIC
					&&
					(n->n_table==gtbl || ispt_data(n))
				) {
					if (n->n_val && n->n_evaluated==0) { 
						// extern x = f();
						// generate int x = 0;       
						// plus dynamic initialization
						n->n_sto = 0;
					}

					n->n_initializer = i = mk_zero_init(t,n,n);
				}
				t->dcl_print(n);
				if(n->n_table) {
					Pname nn = n->n_table->look(n->string,0);
					if(nn)
						nn->n_dcl_printed = 1;
					else
						n->n_dcl_printed = 1;
				}
				else
					n->n_dcl_printed = 1;
			}

			if (n->n_scope!=ARG) {
				if (i) {
					puttok(ASSIGN);

					Pexpr i2 = i;
					while (i2->base == CAST || i2->base == G_CAST) 
						i2 = i2->e1;
					if (i2->base == ILIST) i = i2;

					if (
						t != i->tp
						&&
						i->base != ZERO
						&&
						i->base != ILIST
					) {
						Ptype t1 = n->tp->skiptypedefs();

						switch (t1->base) {
						default:
							i->print();
							break;
						case VEC:
							if (Pvec(t1)->typ->base==CHAR) {
								i->print();
								break;
							}
							// no break
						case PTR:
						case RPTR:
							if (i->tp==0 || 
								(!ansi_opt 
					  			&& 
					  			Pptr(t)->typ
					  			&& 
					  			Pptr(t)->typ->skiptypedefs()->base==VEC
					  			&& 
					  			i->base != G_CAST 
					  			&& 
					  			i->base != CAST
								)
								||
								n->tp->check(i->tp,0)) {
								putch('(');
								bit oc = Cast;
								Cast = 1;
								t->print();
								Cast = oc;
								putch(')');
							}
							eprint(i);
						}
					}
					else {
						if (i==zero) {
							while (t->base == TYPE) t = Pbase(t)->b_name->tp;
						}
						eprint(i);
					}
				}
				else if (n->n_evaluated) {
					puttok(ASSIGN);
					if (n->tp->base!=INT || n->tp->is_unsigned()) {
						putstring("((");
						bit oc = Cast;
						Cast = 1;
						n->tp->print();
						Cast = oc;
						fprintf(out_file,")%d)",n->n_val);
					}
					else
						fprintf(out_file,"%d",n->n_val);
				}
			}

			n->n_initializer=saveinit;
			n->n_sto=savesto;

		}
		}

		switch (list) {
		case SM:
			if (sm==0) puttok(SM);
			break;
		case 0:
			if (sm==0) puttok(SM);
			return;
		case CM:
			if (n->n_list) puttok(CM);
			break;
		}
	}
} 

char *local_sign( Ptype pt )
{ // get function signature for local class
	char buf[1024];
	char* bb = pt->signature(buf);
	int ll = bb-buf;
	if (1023 < ll) error('i',"local class N buffer overflow");
	char *p = new char[ll+1];
	strcpy(p,buf);
	return p;
}

void enumdef::dcl_print(Pname cln)
{
	// note: ***** modify to handle local enums 
	char* s = nested_sig?nested_sig:(cln ? cln->string:0);
	if ( nested_sig )
		fprintf(out_file,"enum __%s { ",nested_sig);
	else
		fprintf(out_file,"enum %s { ",local_sig?local_sig:string);

	for (Pname px, p=mem; p; p=px) {
		px = p->n_list;
		if (s) {
			if (p->n_evaluated)
				fprintf(out_file,"%s__%s = %d",p->string,s,p->n_val);
			else
				fprintf(out_file,"%s__%s",p->string,s);
		}
		else {
			if (p->n_evaluated)
				fprintf(out_file,"%s = %d",p->string,p->n_val);
			else
				fprintf(out_file,"%s",p->string);
		}
		if (px) puttok(CM);
		p->n_initializer = 0;
		
		// nested enum within a class template
		if (in_class == 0 || 
			in_class->class_base != INSTANTIATED )
				delete p;
	}
	if (in_class == 0 || 
		in_class->class_base != INSTANTIATED )
			mem = 0;
	else e_body = 3; // already printed
	puttok(RC);
	puttok(SM);
}

void name::print(bit fullprint)
{ // print just the name itself
	if (this == 0) error('i',"0->N::print()");
	if (string == 0) {
		if (emode) putch('?');
		return;
	}

	switch (base) {
	case TNAME:
		//SYM -- NESTED stuff removed
		if ( tpdef == 0 ) {
			switch ( tp->base ) {
			default:
				error('i',"missing tpdef forTdefN %s",string);
			case COBJ:
			case EOBJ:
				Pbase(tp)->b_name->print();
			}
		} else if (tpdef->nested_sig) {
			if ( emode == 0 ) {
				putstring( " __" );
				putst(tpdef->nested_sig);
			}
			else {
				Pbase(tpdef->in_class->k_tbl->k_name->tp)->b_name->print();
				putstring( "::" );
				putstring(string);
			}
		} else if ( emode )
			putstring(string);
		else
			putst(n_template_arg_string ? n_template_arg_string : string);
		return;

	case MDOT:
		Pexpr(this)->print();
		return;
	}

	if (emode) {
		Ptable tbl = 0;
		bit f = 0;
		if (tp) {
			switch (tp->base) {
			case OVERLOAD:
			case FCT:{
				f = 1;
				Pfct ft;
				if (tp->base==FCT)
					ft=fct_type();
				else
					ft=Pfct(Pgen(tp)->fct_list->f->tp);
				if (fullprint && ft->returns && ft->returns->base!=VOID)
					ft->returns->print();
			}
			default:
				if (tbl=n_table) {
					if (tbl == gtbl) {
						if (f == 0) putstring("::");
					}
					else {
						if (tbl->t_name) {
							tbl->t_name->print();
							fprintf(out_file,"::") ;
						}
					}
				}

				if (n_scope==ARG && strcmp(string,"this")==0) {
					// tell which "this" it is
					Ptype tt = Pptr(tp)->typ;
					Pname cn = Pbase(tt)->b_name;
					fprintf(out_file,"%s::",cn->string);
				}
				break;

			case CLASS:
			case ENUM:
				if ( tp->local_sig ) {
					fprintf(out_file,"%s(local to ",string);
					tp->in_fct->print();
					fprintf(out_file,")");
					return;
				} else {
					if ( tp->nested_sig ) {
						Pbase(tp->in_class->k_tbl->k_name->tp)->b_name->print();
						fprintf(out_file,"::");
					}
					if ( tp->base == CLASS ) {
						Ptclass pc = Ptclass(tp);
						if (
							pc->class_base == INSTANTIATED
						)
							pc->inst->print_pretty_name();
						else
							putstring(string);
					} else
						putstring(string);
					return;
				}
			}
		nop:
			switch (n_oper) {
			case TYPE:
				putstring("operator ");
				if (tp && Pfct(tp)->returns)	Pfct(tp)->returns->dcl_print(0);
				break;
			case 0:
				putstring(string);
				break;
			case DTOR:
				if ( tbl && tbl->t_name ) {
					putch('~');
					putstring(tbl->t_name->string);
				} else {
					if (string)
						fprintf(out_file,"%s", n_template_arg_string 
							? n_template_arg_string : "destructor");
					else
						putstring("destructor");
					f = 0;
				}
				break;
			case CTOR:
				if ( tbl && tbl->t_name )
					putstring(tbl->t_name->string);
				else {
					if (string)
						fprintf(out_file,"%s", n_template_arg_string 
							? n_template_arg_string : "constructor");
					else
						putstring("constructor");
					f = 0;
				}
				break;
			case TNAME:
				putstring(string);
				break;
			default:
				putstring("operator ");
				putstring(keys[n_oper]);
				break;
			}

			if (f) 
				if (fullprint)
					Pfct(tp)->arg_print();
				else putstring("()");
		}
		else {
			if (n_oper) goto nop;
			if (string) 
 				putstring(n_template_arg_string 
					? n_template_arg_string : string);
		}
		return;
	}

	char* sig = 0;
	char* templ_fct_name = 0;
	Pclass cl = 0;
	char* asig = n_anon;

	if (tp) {
		Ptable tbl;

		switch (tp->base) {
		default:
			if (tbl=n_table) {	// global or member
				Pname tn;
				if (tbl == gtbl) {
					if ( asig ) {
						fprintf(out_file,"%s.",asig);
					}
					break;
				}

				if (tn=tbl->t_name) {
					cl = Pclass(tn->tp);
//error('d',"%n %t cl%t",this,tp,cl);
					if (asig) { 
						if (
							cl->string[0]=='_'	
							&&
							cl->string[1]=='_'
							&&
							cl->string[2]=='C'
						) {
							fprintf(out_file,"%s.",asig);
						}
						else {
							char* cn;
							int f;
							char buf[1024];
							if (cl->nested_sig)
								cn = cl->nested_sig;
							else if (cl->lex_level)
								cn = cl->local_sig;
							else
								cn = cl->string;
							f = dem_opt_mem && dem_mem(cn, asig);
							fprintf(out_file, "%s", asig);
							if ( cl->nested_sig ) { 
								sprintf(buf, "__%s", cl->nested_sig);
							}
							else if ( cl->lex_level ) {
								sprintf(buf, "__%d%s", cl->c_strlen, cl->local_sig);
							}
							else {
								sprintf(buf, "__%d%s", cl->c_strlen, cl->string);
							}
							if (!f)
								fprintf(out_file, "%s", buf);
							fprintf(out_file, ".");
						}
						cl = 0;
					}
					else if (
						cl->string[0]=='_'
						&&
						cl->string[1]=='_'
						&&
						cl->string[2]=='C'
						&&
						n_stclass != STATIC
					) {
//error('d',"  inline%d vtbl_opt %d ansi_opt %d n_sto%k",Pfct(tp)->f_inline,vtbl_opt,ansi_opt,n_sto);
					    if ( tp->base == FCT
					    &&  (Pfct(tp)->f_is_inline && vtbl_opt)
						|| (ansi_opt && n_sto==STATIC) )
						;
					    else
						cl = 0;
					}
					break;
				}
			}

			switch (n_stclass) {	// local variable
			case STATIC:
			case EXTERN:
				if (asig)
					fprintf(out_file,"%s.",asig);
				else if (n_sto==STATIC && tp->base!=FCT) {
					if (lex_level == 0)
						putstring("__S");
					else if (!dem_opt_local || gtbl->look(string, 0))
						fprintf(out_file,"__%d",lex_level);
				}
				break;
			default:
				// encode with lexical level UNLESS ``special''
				// e.g. __builtin
				if (string[0]!='_' || string[1]!='_' || string[2] != 'C' ) {
					if (asig) {
						if (!dem_opt_local || gtbl->look(asig, 0))
							fprintf(out_file, "__%d", lex_level);
						fprintf(out_file,"%s.", asig);
					}
					else if (!dem_opt_local || gtbl->look(string, 0)) {
						fprintf(out_file,"__%d",lex_level);
					}
				}
			}
			break;

		case CLASS:
			if ( tp->nested_sig )
				fprintf(out_file,"__%s", tp->nested_sig);
			else if ( tp->local_sig )
				fprintf(out_file,"%s", tp->local_sig);
			else
				fprintf(out_file,"%s", Pclass(tp)->string);
			return;
		case ENUM:
			if ( tp->nested_sig )
				fprintf(out_file,"__%s", tp->nested_sig);
			else if ( tp->local_sig )
				fprintf(out_file,"%s", tp->local_sig);
			else
				fprintf(out_file,"%s", Penum(tp)->string);
			return;
		}

		if (tp->base==FCT) {
			sig = Pfct(tp)->f_signature;
			if (sig && sig[0]==0)
				sig = 0;
			if (sig && Pfct(tp)->fct_base == INSTANTIATED)
				templ_fct_name = Ptfct(tp)->unparametrized_tname()->string;
		}
	}

	if (string) {
#ifdef DENSE
		int i = strlen(templ_fct_name?templ_fct_name:string);
		if (cl)
			i += cl->c_strlen+4;	// __dd<class name>
		if (sig) {
			if (cl == 0) i += 2;
			i += strlen(sig);
		}

		if (31<i) {
			char buf[1024];
			if (cl && sig)
				sprintf(buf,"%s__%d%s%s",string,cl->c_strlen,cl->string,sig);
			else if (cl)
				sprintf(buf,"%s__%d%s",string,cl->c_strlen,cl->string);
			else if (sig)
				sprintf(buf,"%s__%s",templ_fct_name?templ_fct_name:string,sig);
			else 
				sprintf(buf,"%s",string);
			chop(buf);
			fprintf(out_file,"%s ",buf);
			return;
		}
#endif

		char* mn;
		{
		char *s = n_template_arg_string ? n_template_arg_string 
				: (templ_fct_name ? templ_fct_name : string);
		putstring(s); 
		mn = s;
		}

		if ( cl ) {
			int f = 0;
			if (tp->base != FCT && n_stclass != STATIC && dem_opt_mem) {
				char* cn;
				if (cl->nested_sig)
					cn = cl->nested_sig;
				else if (cl->lex_level)
					cn = cl->local_sig;
				else
					cn = cl->string;
				f = dem_mem(cn, mn);
			}
			if (!dem_opt_mem || !f) {
				if ( cl->nested_sig )
					fprintf(out_file,"__%s",cl->nested_sig);
				else if ( cl->lex_level )
					fprintf(out_file,"__%d%s",cl->c_strlen,cl->local_sig);
				else
					fprintf(out_file,"__%d%s",cl->c_strlen,cl->string);
			}
		}

		if (sig) {
			if (cl == 0)
				putstring("__");
			putstring(sig);
		}
		putch(' ');
	}
}

#ifdef DENSE
void chop(char* buf)
{
	static char alpha[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const asz = sizeof(alpha)-1;
	int hash = 0;
	char* p = &buf[29];

	if (strlen(buf) <= 30)
		return;
			
	while (*p) {
		hash <<= 1;
		if (hash & (1<<12)) {
			hash &= ~(1<<12);
			hash++;
		}
		hash ^= *p++;
	}

	buf[29] = alpha[(int)(hash%asz)];
	buf[30] = alpha[(int)((hash/asz)%asz)];
	buf[31] = 0;
}
#endif

void type::print()
{
	switch (base) {
	case PTR:
	case RPTR:
	case VEC:
		Pptr(this)->dcl_print(0);
		break;
	case FCT:
		Pfct(this)->dcl_print();
		break;
	case CLASS:
	case ENUM:
		if (emode)
			if (base==CLASS)
				putstring("class ");
			else putstring("enum ");
		else if (base==CLASS) {
			if (Pclass(this)->nested_sig)
				fprintf(out_file,"struct __%s *",Pclass(this)->nested_sig);
			else
				fprintf(out_file,"struct %s *",Pclass(this)->local_sig ? Pclass(this)->local_sig : Pclass(this)->string);
		}
		else 
			fprintf(out_file,"enum %s *",Penum(this)->string);
		break;
	case TYPE:
		if (Cast || Pbase(this)->b_name->lex_level) {
			Pbase(this)->b_name->tp->print();
			break;
		}
		// no break
	default:
		Pbase(this)->dcl_print();
	}
}

char* type::signature(register char* p, int ptflag)
/*
	take a signature suitable for argument types for overloaded
	function names
*/
{
	Ptype t = this;
	int pp = 0;	// pointer to

xx:
//error('d',"xx: (%d) %d %k %t",this,t,t->base,t);

	// first unroll typedefs and handle derived types:

	switch (t->base) {
	case TYPE:
		if (Pbase(t)->b_const)
			*p++ = 'C';
		t = Pbase(t)->b_name->tp;
		goto xx;

	case VEC:
		if ((pp || ptflag) && Pvec(t)->size) {		// A<size>_
			*p++ = 'A';
			sprintf(p,"%d\0",Pvec(t)->size); // don't trust
							// sprintf return value
			while (*++p);
			*p++ = '_';
		}
		else
			*p++ = 'P';
		t = Pvec(t)->typ;
		pp = 1;
		goto xx;

	case PTR:
		if (Pptr(t)->b_const)
			*p++ = 'C';			// *const
		register char* s;
		int d;
		s = 0;
		d = 0;
		if (!Pptr(t)->memof && Pptr(t)->ptname && Pptr(t)->ptname->tp) {
			s = Pbase(Pptr(t)->ptname->tp)->b_name->string;
			fprintf(stderr, "s=%lx name string\n", (long)s);
			d = strlen(s);
		}
		if (Pptr(t)->memof || s) {
			*p++ = 'M';
			if (!s) {		// M<size><classname>
				Pclass cl = Pptr(t)->memof;
				char* ns = cl->nested_sig;
				char* ls = cl->local_sig;
				if ( ns ){ s = ns;
				}
				else {
					if ( ls ){ s = ls;
					}else{ s = cl->string;
					}
					d = cl->c_strlen;
				}
			}
		        if (d >= 100)
				*p++ = '0' + d/100;
			if (d >= 10)
				*p++ = '0' + (d % 100) / 10;
			if (d)
				*p++ = '0'+ d%10;

			if(s)
				while (*p++ = *s++);
			--p;				// not the '\0'
		}
		else
			*p++ = 'P';
		t = Pptr(t)->typ;
		pp = 1;
		goto xx;

	case RPTR:
		*p++ = 'R';
		t = Pptr(t)->typ;
		pp = 1;
		goto xx;

	case FCT:
	{	Pfct f = Pfct(t);
		Pname n = f->argtype;

		if (f->f_const)
			*p++ = 'C';	// constant member function
		if (f->f_static)
			*p++ = 'S';	// static member function
		*p++ = 'F';

		if (n == 0)
			*p++ = 'v';	// VOID, that is f() == f(void)
		else
			for ( ; n; n=n->n_list) {	// print argument encoding
							// check if argtype is the same
							// as previously seen argtype
				int i = 0;
				for (Pname nn=f->argtype; n!=nn; nn=nn->n_list) {
					i++;
					if (nn->tp==n->tp || nn->tp->check(n->tp,0)==0) {
						// typeof (n) == typeof(arg i)
						int x = 1;	// try for a run after n
						Pname nnn = n;
						while ((nnn=nnn->n_list) && x<9) {
							if (
								nnn->tp==n->tp
								||
								nnn->tp->check(n->tp,0)==0
							) {
								x++;
								n = nnn;
							}
							else
								break;
						}
		
						if (x == 1)		// Ti
							*p++ = 'T';
						else {			// Nxi
							*p++ = 'N';
							*p++ = '0'+x;
						}
			
						// assume <100 arguments
						if (9<i)
							*p++ = '0'+i/10;
						*p++ = '0'+i%10;
						goto zk;
					}	
				}
		
				// ``normal'' case print argument type signature
				p = n->tp->signature(p);
				zk:;
			}

		if (f->nargs_known == ELLIPSIS)
			*p++ = 'e';

		if (pp) {		// '_' result type
			*p++ = '_';
			p = f->returns->signature(p);
		}

		*p = 0;
		return p;
	}
	}

	// base type modifiers:

	if ( Pbase(t)->b_const )	*p++ = 'C';
//	if ( Pbase(t)->b_signed )	*p++ = 'S';
	if ( Pbase(t)->b_unsigned )	*p++ = 'U';
//	if ( Pbase(t)->b_volatile )	*p++ = 'V';


	// now base types:

	register char *s, *ns = 0;;
	char* ls;
	int d;
	Pclass cl;
	switch (t->base) {
	case ANY:			break;
	case ZTYPE:			break;
	case VOID:	*p++ = 'v';	break;
	case CHAR:	*p++ = 'c';	break;
	case SHORT:	*p++ = 's';	break;
	case INT:	*p++ = 'i';	break;
	case LONG:	*p++ = 'l';	break;
	case VLONG:	*p++ = 'V';	break;
	case FLOAT:	*p++ = 'f';	break;
	case DOUBLE:	*p++ = 'd';	break;
	case LDOUBLE:	*p++ = 'r';	break;
	case EOBJ:
	{	Penum en = Penum(Pbase(t)->b_name->tp);
		ls = en->local_sig;
		ns = en->nested_sig; 
		s = en->string;
		d = en->e_strlen;
		goto pppp;
	}

	case COBJ:
	{	cl = Pclass(Pbase(t)->b_name->tp);
		ls = cl->local_sig;
		ns = cl->nested_sig;
		s = cl->string;
		d = cl->c_strlen;
	pppp:
		if ( ns ) s = ns;
		else {
			if ( ls ) s = ls;
			if (d >= 100)
				*p++ = '0' +d/100;
			if (d >= 10)
				*p++ = '0' + (d % 100) / 10;
			*p++ = '0' + d%10;
		}
		while (*p++ = *s++);
		--p;
		break;
	}
	case FIELD:
	default:
		error('i',"signature of%k",t->base);
	}

	*p = 0;
	return p;
}

void basetype::dcl_print()
{
	Pname nn;
	Pclass cl;

	if (emode) {
		if (b_virtual)
			puttok(VIRTUAL);
		if (b_inline)
			puttok(INLINE);
	}
	if((ansi_opt && !ansi_const) || emode)
		if (b_const)
			puttok(CONST);
	if (b_unsigned)
		puttok(UNSIGNED);

	switch (base) {
	case ANY:
		if (emode)
			putstring("any ");
		else
			putstring("int ");
		break;

	case ZTYPE:
		if (emode)
			putstring("zero ");
		else
			putstring("int ");
		break;

	case VOID:
		if (emode==0 && ansi_opt==0) {
			// silly trick to bypass BSD C compiler bug
			// void* (*)() dosn't work there
			// note simpl.c knows that VOID -> CHAR grep for VCVC
			puttok(CHAR);
			break;
		} 
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case VLONG:
	case FLOAT:
	case DOUBLE:
	case LDOUBLE:
		puttok(base);
		break;

	case EOBJ:
		nn = b_name;
	eob:
		if (emode == 0 && Penum(nn->tp)->e_type) {
			Penum(nn->tp)->e_type->dcl_print();
		} else {
			char* s = nn->string;
			puttok(ENUM);
			if ( *s!='_' || s[1]!='_' || s[2]!='E' )
				nn->print();
		}
		break;

	case COBJ:
		nn = b_name;
	cob:
		cl = Pclass(nn->tp);
		if ( emode && cl && cl->base == CLASS ) {
			char* s = nn->string;
			puttok(cl->csu);
			if ( *s!='_' || s[1]!='_' || s[2]!='C' )
				nn->print();
			break;
		}
		if (cl && (cl->csu==UNION || cl->csu==ANON))
			puttok(UNION);
		else
			puttok(STRUCT);
		if ( cl && cl->nested_sig )
			fprintf(out_file," __%s ",cl->nested_sig);
		else {
			char* s = 0;
			if ( cl && cl->lex_level && cl->local_sig )
				s = cl->local_sig;
			putst(s?s:nn->string);
		}
		break;

	case TYPE:
		if (emode == 0) {
			switch (b_name->tp->base) {
			case COBJ:
				nn = Pbase(b_name->tp)->b_name;
				goto cob;
			case EOBJ:
				nn = Pbase(b_name->tp)->b_name;
				goto eob;
			}
		}
		b_name->print();
		break;

	default:
		if (emode) {
			if (0<base && base<=MAXTOK && keys[base])
				fprintf(out_file," %s",keys[base]);
			else
				putch('?');
		}
		else
			error('i',"%p->BT::dcl_print(%d)",this,base);
	}
}

void type::dcl_print(Pname n)
/*
	"this" type is the type of "n". Print the declaration
*/
{
	Ptype t = this;
	Pptr p;
	TOK pre = 0;

	if (t == 0)
		error('i',"0->dcl_print()");
	if (n && n->tp!=t)
		error('i',"not %n'sT (%p)",n,t);

	if (base == OVERLOAD) {
		for (Plist gl=Pgen(this)->fct_list; gl; gl=gl->l) {
			Pname nn = gl->f;
			nn->tp->dcl_print(nn);
			if (gl->l) puttok(SM);
		}
		return;
	}

	tbuf = tbufvec[freetbuf];
	if (tbuf == 0) {
		if (freetbuf == NTBUF-1) error('i',"AT nesting overflow");
		tbufvec[freetbuf] = tbuf = new class dcl_buf;
	}
	freetbuf++;
	if ( base==FIELD && Pbase(this)->b_bits == 0 )
		tbuf->init(0);// some compilers can't handle mangled names here
	else
		tbuf->init(n);
	if (n && n->n_xref)
		tbuf->front(PTR);

	bit vec_const_flag = 0;
	bit prev_const = 0;
	while (t) {
		TOK k;
		switch (t->base) {
		case PTR:
			p = Pptr(t);
			k = (p->b_const&&!p->ansi_const) ? CONST_PTR : PTR;
			goto ppp;
		case RPTR:
			p = Pptr(t);
			k = (p->b_const&&!p->ansi_const) ? CONST_RPTR : RPTR;
		ppp:
			if (p->memof) {
				if (emode) {
					tbuf->front(k);
					tbuf->front(p->memof);
				}
				else {
					t = p->typ->skiptypedefs();
					if (t->base == FCT) {
						tbuf->base(mptr_type);
						goto zaq;
					}
					else
						tbuf->front(k);
				}
			}
			else
				tbuf->front(k);
			pre = PTR;
			t = p->typ;
			break;
		case VEC:
		{	Pvec v = Pvec(t);
			if (Cast && pre != PTR && pre != VEC) {	// for Macintosh: ptr to array uses [] notation
				tbuf->front(PTR);
				pre = PTR;
			}
			else {
				if (pre == PTR)
					tbuf->paran();
				tbuf->back(VEC,v);
				pre = VEC;
			}
			if (ansi_opt) {
				Ptype t = Pvec(v)->typ;
				while (t && t->skiptypedefs()->base == VEC)
					t = Pvec(t)->typ;
				switch (t ? t->skiptypedefs()->base : 0) {
					case PTR:
					case RPTR:
					case CHAR:
					case SHORT:
					case INT:
					case EOBJ:
					case LONG:
					case VLONG:
					case FLOAT:
					case DOUBLE:
					case LDOUBLE:
					case FIELD:
					case ANY:
						break;
					default:
						vec_const_flag = 1;
						break;
				}
			}
			t = v->typ;
			break;
		}

		case FCT:
		{
			Pfct f = Pfct(t);
			if (pre == PTR)
				tbuf->paran();
			else if (emode && f->memof && n==0)
				tbuf->front(f->memof);
			tbuf->back(FCT,f);
			pre = FCT;
			t = (f->s_returns) ? f->s_returns : f->returns;
			break;
		}

		case FIELD:
			tbuf->back(FIELD,t);
			tbuf->base( Pbase(Pbase(t)->b_fieldtype) );
			t = 0;
			break;

		case 0:
			if(!emode)error('i',"noBT(B=0)");
			goto dobase;

		case TYPE:
			if ((Cast && TCast)||Pbase(t)->b_name->lex_level) { // unravel type in case it contains vectors
				Ptype tt = t;
				bit ok = 0;
				while (tt && tt->base == TYPE) {
					if (Pbase(tt)->b_const)
						ok = 1;
					if (Pbase(tt)->ansi_const) {
						ok = 0;
						break;
					}
					tt = tt->bname_type();
				}
				if (ok && tt && tt->is_ptr_or_ref())
					ok = 0;
				t = t->skiptypedefs();
				if (	(emode || ansi_opt)
					&&
					ok
					&&
					!prev_const
				) {
					putstring("const ");
					prev_const = 1;
				}
				break;
			}

		default: // the base has been reached
		dobase:
			if (emode) {
				char* s;
				Ptype tt = t->skiptypedefs();

				switch (tt->base) {
				case CLASS:
					s = Pclass(tt)->string;
					if (
						Ptclass(tt)->class_base == INSTANTIATED
						||
						Ptclass(tt)->class_base == UNINSTANTIATED
						||
						!(s[0]=='_' && s[1]=='_' && s[2]=='C')
					) {
						Pbase(Pclass(tt)->k_tbl->k_name->tp)->b_name->print();
						--freetbuf;
						return;
					}
					s="class";
					goto fret;
				case ENUM:
					s = "enum";
					goto fret;
				case OVERLOAD:
					s = "overloaded";
				fret:
					putstring(s);
					freetbuf--;
					return;
				}
			}

			tbuf->base( Pbase(t), vec_const_flag );
			goto zaq;
		} // switch
	} // while
zaq:
	tbuf->put();
	freetbuf--;
}

void fct::dcl_print()
{

        Pname nn;
        if (is_templ() && // insure only print once
                current_fct_instantiation != this) return;

	if (emode) {
		arg_print();
		return;
	}

	Pname at = f_args;
	putch('(');

	if (ansi_opt) {
		// print typed arguments:
		at = (f_this) ? f_this : (f_result) ? f_result : argtype; 
		// WNG -- note:  at = f_args had 0 value with ansi_opt set
		//	mystery fix added here
		if (at == 0) {
			if (nargs_known == ELLIPSIS) {
				putch(')');
				return;
			}
			puttok(VOID);
		}
		else if (body && Cast==0) {
			if (at == argtype)
				at = f_args;
			at->dcl_print(CM);	// print argument type and name
		}
		else {
			for (Pname a = argtype; a; a=a->n_list) {
				Pname cln = a->tp->is_cl_obj();
				if (cln && Pclass(cln->tp)->has_itor())
						a->n_xref=1;
			}
			for (nn=at; nn;) {
				nn->tp->dcl_print(nn);	// print argument type
							// (there may not be a name)
				if (nn=nn->n_list)
					puttok(CM);
				else
					break;
			}
		}
		if (nargs_known == ELLIPSIS)
			putstring(",...");
		putch(')');
	}
	else {
		// print argument names followed by argument type declarations:
		if (body && Cast==0) {
			for (nn=at; nn;) {
				nn->print();
				if (nn=nn->n_list)
					puttok(CM);
				else
					break;
			}
#if mips || hp9000s800
                        if (nargs_known == ELLIPSIS)
                            if ( at )
                                putstring(", va_alist");
                            else
                                putstring(" va_alist");
#endif
#ifdef sparc
                        if (nargs_known == ELLIPSIS)
                            if ( at )
                                putstring(", __builtin_va_alist");
                            else
                                putstring(" __builtin_va_alist");
#endif
                        putch(')');
#ifdef hp9000s800
                        if (nargs_known == ELLIPSIS)
                                putstring("long va_alist;");
#endif

		}
		else
			putch(')');
	}
}

void print_body(Pfct f)
{


	if (Cast==0) {
		if (ansi_opt==0 && f->f_args) {
			f->f_args->dcl_print(SM);
			if ( last_ll==0 && (last_ll = f->body->where.line) )
				f->body->where.putline();
		}

		if (MAIN) {	// call constructors
//
// 			putstring("{ _main(); ");
// On Plan9 ape uses _main so c++ must use __main
//
			if(ansi_opt)
				putstring("{ void __main(void); __main(); ");
			else
				putstring("{ __main(); ");
			f->body->print();
			puttok(RC);
		}
		else f->body->print();
	}
}

void classdef::print_members()
{
	int i;
	
	Pbcl l = baselist;
	if (l) {
		if (l->base == NAME) {
			l->bclass->print_members();	// first base only
			// pad to ensure alignment:
			int boff = l->bclass->real_size;
			int ba = l->bclass->align();
			int xtra = boff%ba;
			int waste = (xtra) ? ba-xtra : 0;	// padding
			if (waste) {
				// waste it to protect against structure
				// assignments to the base class
				char* s = make_name('W');
				fprintf(out_file,"char %s[%d];\n",s,waste);
				delete s;
			}
			l = l->next;
		}

		for (; l; l=l->next)
		/*	for second base etc. one must allocate as an object
			(rather than a list of members) to ensure proper alignment
			for shared base allocate a pointer
			size, alignment, & offset handled in cassdef::dcl()
		*/
			if (l->base == NAME) {
				Pclass bcl = l->bclass;
				char *str =  0;
				char *cs = bcl->nested_sig?bcl->nested_sig:bcl->string;
				if (bcl->lex_level && !bcl->nested_sig)
					str=bcl->local_sig;
				puttok(STRUCT);
				if (bcl->nested_sig) putstring("__");
				putst(str?str:cs);
				putcat('O',bcl->string); // leave as unencoded
				puttok(SM);
			}
	}

    	for (Pname nn=memtbl->get_mem(i=1); nn; NEXT_NAME(memtbl,nn,i)) {
		if ( nn->base == TNAME ) continue;
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
			nn->n_stclass != STATIC
		) {
			Pexpr i = nn->n_initializer;
			nn->n_initializer = 0;
			DB(if(Pdebug>=1) error('d',&nn->where,"  printingM%n base%k tbase%k n_key%k",nn,nn->base,nn->tp->base,nn->n_key););
			nn->dcl_print(0);
			nn->n_initializer = i;
		}
	}

	for (l=baselist; l; l=l->next)
		if (l->base==VIRTUAL && l->ptr_offset) {
			Pclass bcl = l->bclass;
       			char* str =  0;
			char *cs = bcl->nested_sig?bcl->nested_sig:bcl->string;

			if (bcl->lex_level && !bcl->nested_sig)
				str=bcl->local_sig;

			puttok(STRUCT);
			if (bcl->nested_sig) putstring("__");
			putst(str?str:cs);
			putch('*');
			putcat('P',bcl->string); // leave unencoded
			puttok(SM);
		}
}

vl* vlist;

void really_really_print(Pclass cl, Pvirt vtab, char* s, char* ss);

int p2(Pname nn, Ptype t, Pclass cl, Pvirt vtab, char* s)
{
	int init;

	if (t->base == FCT) {
		Pfct f = Pfct(t);

		if (
			nn->n_initializer
			||
			nn->n_sto==STATIC
			||
			f->f_inline
			||
			f->f_imeasure
			||
			f->f_virtual==0
		)
			return 0;
		init = f->body!=0;
	}
	else
		init = nn->n_initializer!=0;

	int oo = vtbl_opt;
	vtbl_opt = 1;	// make sure the name is universal
	char *cs = cl->nested_sig?cl->nested_sig:cl->string;
	char* sstr = (cl->lex_level&&!cl->nested_sig)? cl->local_sig : 0;
	char* ss = vtbl_name(vtab->string,sstr?sstr:cs);

	if (init) {	// unique definition here
		really_really_print(cl,vtab,ss,s);
	}
	else {	// unique definition elsewhere
		char *vstr = 0;
		if ( vtab && vtab->string ) {
			vstr = new char[strlen(vtab->string)+cl->c_strlen+1];
			strcpy( vstr, vtab->string );
			strcat( vstr, cl->string );
		}
		Pname nn;
		if (nn=ptbl->look(vstr?vstr:cl->string,0)) { // use of ptbl in file
			fprintf(out_file,"extern struct __mptr %s[];\n",ss);
			s[2] = 'p';
			ptbl_add_pair(s, ss);
			nn->n_key=HIDDEN; 
		}
		delete vstr;
	}
	vtbl_opt = oo;

	delete ss;

	return 1;
}

void classdef::really_print(Pvirt vtab)
{
	int oo = vtbl_opt;	// make `simulated static' name
	vtbl_opt = -1;
	char *cs = nested_sig?nested_sig:string;
	char* str = (lex_level&&!nested_sig) ? this->local_sig : 0;
	char* s = vtbl_name(vtab->string,str?str:cs);
	vtbl_opt = oo;

	// see if needed
	int i;
	for (Pname nn=memtbl->get_mem(i=1); nn; NEXT_NAME(memtbl,nn,i) ) {
		if ( nn->base == TNAME || nn->tp==0 ) continue;
		Ptype t = nn->tp->skiptypedefs();

		if (t)
		switch (t->base) {
		case FCT:
			if (p2(nn,t,this,vtab,s))
				goto xyzzy;
			break;

		case OVERLOAD:
		{
			for (Plist gl=Pgen(t)->fct_list; gl; gl=gl->l)
				if (p2(gl->f,gl->f->tp,this,vtab,s))
					goto xyzzy;
		}
		}
	}

	{ // must be initialized var in block to permit goto
	char* ss = vtbl_name(vtab->string,str?str:cs);

	if (vtbl_opt)
		really_really_print(this,vtab,ss,s);
	else {
		char *vstr = 0;
		if ( vtab && vtab->string ) {
			vstr = new char[::strlen(vtab->string)+c_strlen+1];
			strcpy( vstr, vtab->string );
			strcat( vstr, string );
		}
		Pname nn;
		if (nn=ptbl->look(vstr?vstr:string,0)) { // use of ptbl in file
			fprintf(out_file,"extern struct __mptr %s[];\n",ss);
			s[2] = 'p';
			ptbl_add_pair(s, ss);
			nn->n_key=HIDDEN; 
		}
		delete vstr;
	}
	delete ss;
	}
xyzzy:
	delete s;
}

void really_really_print(Pclass cl, Pvirt vtab, char* s, char* ss)
{
	// make sure function is declared before using
	// it in vtbl initializer
	Pname nn;
	int i;
	for (i=0; nn = vtab->virt_init[i].n; i++) {
		if (nn->tp->base == OVERLOAD) {
			nn = Pgen(nn->tp)->fct_list->f;
		}
		Pfct f = Pfct(nn->tp);
		if (nn->n_initializer) {       // pure virtual
			static int pv;
			if (pv == 0) {	// VCVC void->char assumed
				if(ansi_opt)	// plan 9: ansi version
					fprintf(out_file,"void __pure_virtual_called(void);\n");
				else
					fprintf(out_file,"char __pure_virtual_called();\n");
				pv = 1;
			}
			continue;
		}
		if (f->base != FCT)
			error('i',"vtbl %n",nn);

		if (nn->n_dcl_printed==0) {
			if ((f->f_inline && vtbl_opt) || (ansi_opt && nn->n_sto==STATIC))
				puttok(STATIC);
			if (f->f_result == 0)
				make_res(f);
			Ptype r = f->s_returns ? f->s_returns : f->returns;
			r->print();
			nn->print();
			if (ansi_opt)
				f->dcl_print();
			else putstring("()");
			puttok(SM);
			nn->n_dcl_printed = 1;
		}
	}

	fprintf(out_file,"struct __mptr %s[] = {0,0,0,\n",s);

	Pname n;
	for (i=0; n=vtab->virt_init[i].n; i++) {
		if (n->tp->base == OVERLOAD) {
			n = Pgen(n->tp)->fct_list->f;
		}
		if (n->n_initializer)
			putstring("0,0,(__vptp)__pure_virtual_called,\n");
		else {
			fprintf(out_file,"%d,0,(__vptp)",-vtab->virt_init[i].offset);
			n->print();
			n->n_addr_taken = 1;
			putstring(",\n");
		}
	}
	putstring("0,0,0};\n");

	ss[2] = 'p';
	s[2] = 'v';
	ptbl_add_pair(ss,s);

	Pname nm;
	char *cstr = 0;
	char *vstr = 0;
	char *cs = cl->nested_sig?cl->nested_sig:cl->string;
	if (cl->lex_level && !cl->nested_sig)
		cstr = cl->local_sig;
	if ( vtab && vtab->string ) {
		vstr = new char[strlen(vtab->string)+(cstr?strlen(cstr):strlen(cs))+1];
		strcpy( vstr, vtab->string );
		strcat( vstr, cstr?cstr:cs );
	}

	if ( vstr == 0 )
		vstr = cstr?cstr:(cs?cs:cl->string);
	if ( nm = ptbl->look(vstr,0) )  {
		nm->n_key = HIDDEN;
		if ( vstr != cstr && vstr != cs && vstr != cl->string ) delete vstr;
	} else if ( ptbl->look(vstr,HIDDEN) == 0 )
		ptbl->insert(new name(vstr),HIDDEN);
	else
		if ( vstr != cstr && vstr != cl->string && vstr != cs ) delete vstr;
}

#include <ctype.h>
char* vtbl_name(char* s1, char* s2)
{
	char* s3 = (vtbl_opt == -1 && *src_file_name) ? src_file_name : 0;
		// if vtbl_opt == -1 fake a static (there are no portable
		// way of doing a forward declaration of a static in C)
	int ll = s1 ? strlen(s1) : 0;
	int ll2 = strlen(s2);
	int ll3 = s3 ? strlen(s3) : 0;
	int sz = (ll+ll2+ll3+20)/32+1;  // avoid fragmentation

	sz *= 32;
	char* buf = new char[sz];
	if (s3) {
		if (s1)
			sprintf(buf,"__vtbl__%d%s__%d%s__%s",ll,s1,ll2,s2,s3);
		else
			sprintf(buf,"__vtbl__%d%s__%s",ll2,s2,s3);
	}
	else if (s1)
		sprintf(buf,"__vtbl__%d%s__%d%s",ll,s1,ll2,s2);
	else
		sprintf(buf,"__vtbl__%d%s",ll2,s2);

	if (vtbl_opt == -1) {
		for (char* p = buf+ll2+11; *p; p++)
			if (!isalpha(*p) && !isdigit(*p)) *p = '_';
	}
#ifdef DENSE
	chop(buf);
#endif
	return buf;
}

void classdef::print_all_vtbls(Pclass bcl)
{
	for (Pvirt blist = bcl->virt_list; blist; blist = blist->next) {
		if (!::same_class(this,blist->vclass))
			continue;
		if (blist->printed)
			continue;
		vlist = new vl(this,blist,vlist);
		blist->printed = 1;
	}

	for (Pbcl b = bcl->baselist; b; b = b->next)
		print_all_vtbls(b->bclass);

	if (::same_class(this,bcl))
		c_body = 0;
}

void classdef::dcl_print(Pname n)
{ 
	defined |= REF_SEEN;
// ensure template instantiations are printed exactly once.
//	if (class_base != VANILLA && !::same_class(current_instantiation,this) && 
//		(n==0 || n->n_redefined != 1)) 
//			return;

	DB(if(Pdebug>=1) error('d',"%t::dcl_print(%n)   c_body %d defined%o",this,n,c_body,defined););
	if (c_body==0 || c_body==3 || (defined&DEFINED)==0)
		return;
	c_body = 3;

	int i;
	for (Pname nn=memtbl->get_mem(i=1); nn; NEXT_NAME(memtbl,nn,i) ) {
		if ( nn->base == TNAME ) continue;
		if ( nn->base==NAME &&
			nn->n_anon==0 &&
			nn->tp->base==CLASS &&
			Pclass(nn->tp)->c_body==1)
				Pclass(nn->tp)->dcl_print(nn);
		else 
		if (nn->base == TNAME && nn->tp->base != COBJ)
			nn->dcl_print(0);
		else 
		if (nn->tp && 
			nn->n_anon == 0 &&
			nn->tp->base == ENUM && 
			Penum(nn->tp)->e_body != 3) 
				Penum(nn->tp)->dcl_print(nn);
	}

	// note: reusing arg n
	n = k_find_name(string,Ctbl,HIDDEN);//SYM

	if (n) {
		if (
			n->where.line!=last_line.line
			||
			n->where.file!=last_line.file
		)
			if (last_ll = n->where.line)
				n->where.putline();
			else
				last_line.putline();
	}

	TOK c = csu==CLASS ? STRUCT : csu;
	puttok(c);

	char *str = 0;
//error('d',"this:%t nested_sig %s ll %d",this,nested_sig,lex_level);
	if ( lex_level && !nested_sig)
		str = this->local_sig;
	if ( nested_sig )
		putstring( " __" );
	putst(str?str:(nested_sig?nested_sig:string));

	int sz = tsizeof();
	int dvirt = 0;

	if ( nested_sig )
		fprintf(out_file,"{\t/* sizeof __%s == %d */\n",nested_sig,obj_size);
	else
		fprintf(out_file,"{\t/* sizeof %s == %d */\n",str?str:string,obj_size);
	if ( last_ll )
		last_line.line++;

	begin_mem();
	print_members();
	end_mem();
	for (Pbcl b = baselist; b; b = b->next) {	
		if (b->base != VIRTUAL)
			continue;
		Pclass bcl = b->bclass;
		dvirt += bcl->virt_count;
		if (b->allocated==0)
			continue;
		char* str = 0;
		char* cs = bcl->nested_sig?bcl->nested_sig:bcl->string;
		if (bcl->lex_level && !bcl->nested_sig)
			str = bcl->local_sig;
		puttok(STRUCT);			// struct bcl Obcl;
		if (bcl->nested_sig) putstring("__");
		putst(str?str:cs);
		putcat('O',bcl->string); // leave unencoded
		puttok(SM);
	}
	putstring("};\n");
	if ( last_ll )
		last_line.line++;

	for (nn=memtbl->get_mem(i=1); nn; NEXT_NAME(memtbl,nn,i) ) {
		if ( nn->base == TNAME ) continue;
		if (nn->base==NAME && nn->n_anon==0) {
			Ptype t = nn->tp;
			switch (t->base) {
			case FCT:
			case OVERLOAD:
				break;
			default:
				if (nn->n_stclass == STATIC) {
					TOK b = nn->n_sto;
					nn->n_sto = (nn->n_evaluated) ? STATIC : b;
					nn->dcl_print(0);
					nn->n_sto = b;
				}
			}
		}
	}
	if (vtbl_opt != -1)
		print_all_vtbls(this);	// force declaration
}


char *
make_local_name( Ptype tt, Pname fn )
{
	char *buf; 
	int tlen;
	char* tstring;
	switch ( tt->base ) {
	case CLASS:
		tlen = Pclass(tt)->c_strlen;
		tstring = Pclass(tt)->string;
		break;
	case ENUM:
		tlen = Penum(tt)->e_strlen;
		tstring = Penum(tt)->string;
		break;
	default:
		error('i',"make_local_name(%t,%n )",tt,fn);
	}
	char* lcl = make_name('L');
	if ( fn == 0 || fn->tp == 0 || fn->tp->base != FCT )
		error( 'i', "localC %smissing or badFN%n", tstring, fn ); 
	tt->in_fct = fn;
	char *fsig = Pfct(fn->tp)->f_signature;
	if ( fsig == 0 )
		fsig = local_sign( fn->tp );
	char *fs = fn->string;
	int name_len=tlen+strlen(fsig)+strlen(fs)+strlen(lcl)+4;
	int sz = (name_len+20)/32+1; // from vtbl_name()

	if ( Pfct(fn->tp)->memof == 0 ) {
		sz *= 32;
		buf = new char[ sz ];

		sprintf(buf, "%s__%s__%s%s", tstring, fs, fsig, lcl);
	}
	else {
		char *cs = Pclass(Pfct(fn->tp)->memof)->string;
		int len = Pclass(Pfct(fn->tp)->memof)->c_strlen;
		if ( len < 10 )
			++name_len;
		else if ( len > 99 )
			name_len += 3;
		else
			name_len += 2;
		name_len += len;
		sz = (name_len+20)/32+1; 
		sz *= 32;
		buf = new char[ sz ];

		sprintf(buf, "%s__%s__%d%s%s%s",tstring,fs,len,cs,fsig,lcl);
	}

#ifdef DENSE
	chop( buf );
#endif

	if ( tt->base == CLASS )
		Pclass(tt)->c_strlen = name_len;
	else
		Penum(tt)->e_strlen = name_len;
	return buf;
}
