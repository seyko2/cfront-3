/*ident	"@(#)cls4:src/error.c	1.6" */
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

 error.c :

	write error messages

	Until scan_started != 0 no context can be assumed

***************************************************************************/

#ifdef __cplusplus
#include <stdlib.h>
#endif

#include "cfront.h"
#include "size.h"
#include "template.h"

int error_count;
//static int no_of_warnings;
char scan_started;

#define ERRTRACE    20

static char* abbrev_tbl[] = {
	" argument",
	" base",
	" class",
	" declaration",
	" expression",
	" function",
	" global",
	"H",
	" initialize",
#if EBCDIC
	"", "", "", "", "", "", "", // fill out hole in ebcdic character set
	
#endif
	"J",
	" constructor",	// 'K' !
	" list",
	" member",
	" name",
	" object",
	" pointer",
	" qualifie",
	" reference",
#if EBCDIC
	"", "", "", "", "", "", "", // fill out hole in ebcdic character set
	
#endif
	" statement",
	" type",
	" undefined",
	" variable",
	" with",
	" expected", // 'X'
	" template", // 'Y'???
	" parameter", // 'Z'???
};

ea* ea0;

//ea::ea(const void* pp) {
//	p=pp;
//}

void error_init()
{
	static char errbuf[BUFSIZ];
	setbuf(stderr,errbuf);
	ea0 = new ea;
}

#define INTERNAL 127
#ifdef TEST_SUITE
#define INTERNAL2 126
#define SORRY 125
#endif

void ext(int n)
{
	int useit=n; // to avoid n not used warning during build
// for testing only
//	if (n == INTERNAL)
//	abort();
#ifdef TEST_SUITE
	if (n == INTERNAL)
		error_count = 99;
	else if (n == SORRY)
		error_count = 100;
#endif
	fflush(out_file);
	fflush(stderr);
	exit(error_count?error_count:1);
}

/* static */ 
void print_loc()
{
	loc* sl = (Cstmt) ? &Cstmt->where : 0;
	loc* dl = (Cdcl && (Cdcl->base==NAME || Cdcl->base==TNAME)) ? &Cdcl->where : 0;
	if (sl && dl && sl->file==dl->file) {	// Cstmt and Cdcl in same file
		if (sl->line<=dl->line) {
			if (curloc.file==dl->file && curloc.line<dl->line)
				// hack to compensate for YACC's
				// bad manners in the use of line numbers
				sl->put(out_file);
			else
				dl->put(out_file);
		}
		else
			sl->put(out_file);
	}
	else if (sl && sl->file==curr_file)	// Cstmt in current file
		sl->put(out_file);
	else if (dl && dl->file==curr_file)	// Cdcl in current file
		dl->put(out_file);
	else
		curloc.put(out_file);
}

static void print_context()
{
	putc('\n',out_file);
}

static char in_error = 0;
static loc dummy_loc;

void yyerror(const char* s)
{
	if(s && *s) {
#ifdef SVR42
		// Convert any upper-case chars to lower-case. The yyerror()
		// calls in SVR4.2 contain upper-case chars which would get
		// translated in error() otherwise.
		char* s2 = new char[(strlen(s)+1)];
		const char* beg;
		for(beg = strcpy(s2,s); *s2; ++s2){
			if(*s2 >= 'A' && *s2 <= 'Z')
				*s2 += ' ';
		}
		error(beg);
		delete (char*)beg;
#else
		error(s);
#endif
	} else {
		error("yyerror() called with null argument");
	}
}

int error(const char* s)
{
	return error(0,s);
}

int error(int t, const char* s)
{
	return error(t,&dummy_loc,s,*ea0,*ea0,*ea0,*ea0);
}

int error(const char* s, const ea& a0, const ea& a1, const ea& a2, const ea& a3)
{
	return error(0,&dummy_loc,s,a0,a1,a2,a3);
}

int error(loc* lc, const char* s, const ea& a0, const ea& a1, const ea& a2, const ea& a3)
{
	return error(0,lc,s,a0,a1,a2,a3);
}

int error(int t, const char* s, const ea& a0, const ea& a1, const ea& a2, const ea& a3)
{
	return error(t,&dummy_loc,s,a0,a1,a2,a3);
}

int suppress_error;

int error(int t, loc* lc, const char* s, const ea& a0, const ea& a1, const ea& a2, const ea& a3)
/*
	"int" not "void" because of "pch" in lex.c

	legal error types are:

	not counted in error count:

		'w'		warning
		'd'		debug
		'D'		debug -- no prefix
		'C'		text -- line no. and no newline
		'c'		text -- no line no. and no newline

	counted in error count:

		's'		"not implemented" message
		'l'		"compiler limit exceeded" message
    		0		error 
    		'e'		error -- no newline
    		'i'		internal error (causes abort)
		't'		error while printing error message
*/
{
	if (suppress_error && t!='i' && t!='d') return 0;

	if (in_error++)
		if (t == 't')
			t = 'i';
		else if (4 < in_error) {
			fprintf(stderr,"\nOops!, error while handling error\n");
			ext(13);
		}

	FILE * of = out_file;
	out_file = stderr;

	if (!scan_started || t=='t')
		putch('\n');
	else if (lc != &dummy_loc) {
		if(t != 'D' && t != 'c') lc->put(out_file);
	} else {
		if(t != 'D' && t != 'c') print_loc();
	}

	int user_error = 0;

	switch (t) {
        case 'C':
        case 'c':
		break;
    	case 'e':
		user_error = 1;
		// no break
    	case 0:
		putstring("error: ");
		user_error += 1;
		break;
        case 'd':
		putstring("DEBUG: ");
        case 'D':
		break;
        case 'w':
//		no_of_warnings++;
		putstring("warning: ");
		break;
        case 'l':
		putstring("compiler limit exceeded: ");
		break;
        case 's':
		putstring("sorry, not implemented: ");
		user_error = 1;
		break;
        case 'i':
		if (error_count++) {
			fprintf(out_file,"%s: cannot recover\n", prog_name);
			out_file = of; // restore for fflush()
#ifdef TEST_SUITE
			ext(INTERNAL2);
#else
			ext(INTERNAL);
#endif
		}
		else
			fprintf(out_file,"%s: internal error - ",prog_name);
        }

	ea argv[4];
	ea* a = argv;
	argv[0] = a0;
	argv[1] = a1;
	argv[2] = a2;
	argv[3] = a3;

	int c;

	while (c = *s++) {
		if ('A'<=c && c<='Z')
			putstring(abbrev_tbl[c-'A']);
		else if (c == '%') {
			switch (c = *s++) {
			case 'k':	// TOK assumed passed as an int
			{	TOK x = TOK(a->i);
				if (0<x && x<=MAXTOK && keys[x])
					fprintf(out_file," %s",keys[x]);
				else
					fprintf(out_file," token(%d)",x);
				break;
			}
			case 't':	// Ptype 
			{	Ptype tt = Ptype(a->p);
				if (tt == 0) break;

				putch(' ');
			
				int nt = ntok;
				emode = 1;
				tt->dcl_print(0);
				emode = 0;
				ntok = nt;
				break;
			}
			case 'n':	// Pname
			{	Pname nn = Pname(a->p);
				if (nn && nn->string) {
					// suppress generated class names:
					if (nn->string[0]=='_'
					&& nn->string[1]=='_'
					&& nn->string[2]=='C') break;
					emode = 1;
					putch(' ');
					nn->print();
					emode = 0;
				}
				else
					putstring(" ?");
				break;
			}
			case 'p':	// pointer
			{	char* f = sizeof(char*)==sizeof(int)?" %d":" %ld";
				fprintf(out_file,f,a->p);
				break;
			}
			case 'a':	// fully qualified function 
			{	Pname nn = Pname(a->p);
				if (nn->tp->base!=FCT && nn->tp->base!=OVERLOAD)
					error('i',"%n not function",nn);
				if (nn && nn->string) {
					// suppress generated class names:
					if (nn->string[0]=='_'
					&& nn->string[1]=='_'
					&& nn->string[2]=='C') break;
					emode = 1;
					putch(' ');
					nn->print(1);
					emode = 0;
				}
				else
					putstring(" ?");
				break;
			}
			case 'c':	// char assumed passed as an int
				putch((int)a->i);
				break;

			case 'd':	// int
				fprintf(out_file," %d",a->i);
				break;

			case 'o':	// int
				fprintf(out_file," 0%o",a->i);
				break;

			case 's':	// char*
				{
				char *s = ((char *)a->p);
				if ( s ) putst((char*)a->p);
				break;
				}
			}
			a++;
		}
		else
			putch(c);
	}

/*
	switch (t) {
	case 'd':
	case 't':
	case 'w':
		putch('\n');
		break;
	default:
*/
		if (t != 'c' && t != 'e' && t != 'C')
			print_context();
/*
	}
*/

	if (user_error) 
		basic_inst::head->print_error_loc(user_error==2);

	out_file = of; // restore before ext() for fflush()
	if (!scan_started && t!='d' && t!='w') ext(4);

        // now we may want to carry on 
	switch (t) {
	case 't':
		if (--in_error) {
			fflush(stderr);
			//fflush(out_file);
			return 0;
		}
	case 'i': 
		ext(INTERNAL);
	case 0:
	case 'e':
	case 'l':
	case 's':
#ifdef TEST_SUITE
		if (t == 's')
			ext(SORRY);
#endif
		if (MAXERR<++error_count) {
			fprintf(stderr,"%s: too many errors, giving up\n", prog_name);
			ext(7);
		}
	}

	in_error = 0;
	fflush(stderr);
	//fflush(out_file);
	return 0;
}



#ifdef DBG
#define OPEREP(v) ((v)>MAXTOK || (v)<=0 ? 0 : keys[v])
void
display_type( Ptype t )
{
	if ( t ) { putc(' ',stderr);
		FILE * of = out_file;
		out_file = stderr;
		extern int ntok; int nt = ntok;
		emode=1; (t)->dcl_print(0); emode=0;
		if(!t->allocated)fprintf(stderr," UNALLOCATED!");
		ntok = nt;
		out_file = of;
	} else fprintf(stderr," <null type>");
	fflush(stderr);
}

#define INDENT(in) { for ( int i = in;  i > 0;  --i ) fprintf(stderr,"  "); }
static indent = 0;

void
display_context( dcl_context* p, char* label, int oneline )
{
	int nt = ntok;
	if ( label && *label ) fprintf(stderr,"%s: ",label);
	fprintf(stderr,"dcl_context %d {\n",p);
	++indent; INDENT(indent);
	fprintf(stderr,"cot:"); display_type(p->cot);
	fprintf(stderr,"\n");
	display_expr(p->not,"not",oneline);
	INDENT(indent); putstring("tot:"); display_type(p->tot);
	fprintf(stderr,"\n");
	display_expr(p->nof,"nof",oneline);
	display_expr(p->c_this,"c_this",oneline);
	INDENT(indent); fprintf(stderr,"ftbl %d\n",p->ftbl);
	--indent;
	fprintf(stderr,"}\n");
	fflush(stderr);
}

void
display_expr( Pexpr ptr, char* label, int oneline )
{
	INDENT(indent);
	if ( label ) fprintf(stderr, "%s:", label);
	if ( ptr == 0 ) {
		fprintf(stderr, "NULL EXPR\n" );
		fflush(stderr);
		return;
	}
	fprintf(stderr,"%d",ptr->node::id);
	if(!ptr->allocated)fprintf(stderr," UNALLOCATED!");
	putc(':',stderr);
	char* s = OPEREP(ptr->base);
	if ( s == 0 )
		fprintf(stderr, "token(%d)", ptr->base );
	else
		fprintf(stderr,"%s",s);
	if ( ptr->displayed ) { // recursion!!!
		switch ( ptr->base ) {
		case NAME: case TNAME: case DTOR: case STRING:
		case ICON: case FCON: case CCON:
			fprintf(stderr," '%s'",(ptr->string)?ptr->string:"<0>");
			break;
		case IVAL:
			fprintf(stderr, " i1==%d", ptr->i1);
			break;
		}
		fprintf(stderr,"   RECURSION!!!\n");
		ptr->displayed = 0;
		fflush(stderr);
		return;
	}
	ptr->displayed = 1;
	switch ( ptr->base ) {
	case QUEST:
		display_type(ptr->tp);
		putc('\n',stderr);
		if ( !oneline ) {
			++indent;
			display_expr( ptr->cond, "cond" ); 
			display_expr( ptr->e1, "e1" ); 
			display_expr( ptr->e2, "e2" ); 
			--indent;
		}
		break;
	case REF: case DOT:
		display_type(ptr->tp);
		putc('\n',stderr);
		if ( !oneline ) {
			++indent;
			display_expr( ptr->e1, "e1" );
			display_expr( ptr->mem, "mem" );
			display_expr( ptr->n_initializer, "n_initializer" );
			--indent;
		}
		break;
	case MDOT:
		display_type(ptr->tp);
		fprintf(stderr," string2:'%s'\n",ptr->string2?ptr->string2:"");
		if ( !oneline ) {
			++indent;
			display_expr( ptr->mem, "mem" );
			--indent;
		}
		break;
	case ICALL:
		fprintf(stderr," fn=='%s'",ptr->il->fct_name->string);
		display_type(ptr->tp);
		putc('\n',stderr);
		if ( !oneline ) {
			++indent;
			for ( int i = 0;  i < ptr->il->i_slots;  ++i ) {
				ia *aa = &ptr->il->i_args[i];
				INDENT(indent);
				fprintf(stderr,"arg:'%s'",aa->local&&aa->local->string?aa->local->string:"");
				display_type(aa->tp);
				putc('\n',stderr);
				++indent;
				display_expr( aa->arg, "actual" );
				--indent;
			}
			display_expr( ptr->e1, "e1" );
			display_expr( ptr->e2, "e2" );
			--indent;
		}
		break;
	case SIZEOF:
		if ( ptr->tp2 ) {
			putc('(',stderr);
			display_type(ptr->tp2);
			putc(')',stderr);
		}
		display_type(ptr->tp);
		putc('\n',stderr);
		if ( !oneline ) {
		    if ( ptr->e1 ) {
			++indent;
			display_expr(ptr->e1,"e1");
			--indent;
		    }
		    if ( ptr->e2 ) {
			++indent;
			display_expr(ptr->e2,"e2");
			--indent;
		    }
		}
		break;
	case ZERO:
		display_type(ptr->tp);
		putc('\n',stderr);
		break;
	case DTOR:
		fprintf(stderr," '%s'",(ptr->string)?ptr->string:"<0>");
		display_type(ptr->tp);
		if ( ptr->permanent ) fprintf(stderr, " (permanent)");
		putc('\n',stderr);
		if ( !oneline ) {
			Pname n = Pname(ptr);
			++indent;
			INDENT(indent);
			fprintf(stderr,"tp2"); display_type(ptr->tp2);
			fprintf(stderr,"tpdef"); display_type(ptr->tpdef);
			putc('\n',stderr);
			INDENT(indent);
			display_expr(n->n_qualifier,"n_qualifier" );
			display_expr(n->n_dtag,"n_dtag" );
			--indent;
		}
		break;
	case NAME: case TNAME: case STRING:
	case ICON: case FCON: case CCON:
	case IVAL:
		if ( ptr->base == IVAL ) fprintf(stderr, " i1==%d", ptr->i1);
		else fprintf(stderr," '%s'",(ptr->string)?ptr->string:"<0>");
		display_type(ptr->tp);
		if(ptr->string2)fprintf(stderr," string2=='%s'",ptr->string2);
		if ( ptr->permanent ) fprintf(stderr, " (permanent)");
		putc('\n',stderr);
		if ( !oneline && (ptr->base == NAME || ptr->base == TNAME) ) {
			Pname n = Pname(ptr);
			++indent;
			INDENT(indent);
			fprintf(stderr, "n_key==%d", n->n_key );
			fprintf(stderr, " n_sto==%d", n->n_sto );
			fprintf(stderr, " n_stclass==%d",n->n_stclass);
			fprintf(stderr, " n_scope==%d",n->n_scope);
			fprintf(stderr, " n_protected==%d\n", n->n_protect );
			INDENT(indent);
			fprintf(stderr, "n_oper=='%s'", (s=OPEREP(n->base))?s:"0");
			fprintf(stderr, " n_val==%d", n->n_val );
			fprintf(stderr, " n_xref==%d",n->n_xref);
			fprintf(stderr, " lex_level==%d\n", n->lex_level );
			INDENT(indent);
			fprintf(stderr, " table==%d%d",n->n_table,n->n_table?n->n_table->t_name:0);
			fprintf(stderr, " ktable==%d%d\n",n->n_ktable,n->n_ktable?n->n_ktable->k_name:0);
			INDENT(indent);
			fprintf(stderr, "n_used==%d",n->n_used);
			fprintf(stderr, " n_assigned_to==%d",n->n_assigned_to);
			fprintf(stderr, " n_addr_taken==%d\n",n->n_addr_taken );
			INDENT(indent);
			fprintf(stderr, "n_anon==%s", n->n_anon);
			fprintf(stderr, " n_list=='%s'", n->n_list?n->n_list->string:"<0>" );
			fprintf(stderr, " n_qualifier=='%s'", n->n_qualifier?n->n_qualifier->string:"<0>" ); 
			if ( n->n_initializer ) {
				fprintf(stderr, " n_initializer:\n" );
				++indent;
				display_expr( n->n_initializer );
				--indent;
			} else fprintf(stderr, " n_initializer==<0>\n");
			--indent;
		}
		break;
	case BLOCK:
		((Pstmt)ptr)->where.put(stderr); putc(' ',stderr);
		putc('\n',stderr);
		break;
	default:
		display_type(ptr->tp);
		putc('\n',stderr);
		if ( !oneline && ptr->base > 0
		&&   (ptr->base<165 || ptr->base==MEMPTR) ) {
			++indent;
			display_expr( ptr->e1, "e1" ); 
			display_expr( ptr->e2, "e2" );
			--indent;
		}
		break;
	}
	ptr->displayed = 0;
	fflush(stderr);
	return;
}
void
display_stmt( Pstmt ptr, char* label, int oneline )
{
	INDENT(indent);
	if ( label ) fprintf(stderr, "%s:", label);
	if ( ptr == 0 ) {
		fprintf(stderr, "NULL STMT\n" );
		fflush(stderr);
		return;
	}
	fprintf(stderr,"%d",ptr->node::id);
	if(!ptr->allocated)fprintf(stderr," UNALLOCATED!");
	putc(':',stderr);
	char* s = OPEREP(ptr->base);
	if ( s == 0 )
		fprintf(stderr, "token(%d)", ptr->base );
	else
		fprintf(stderr,"%s",s);
	if ( ptr->displayed ) { // recursion!!!
		if ( ptr->base == NAME )
			fprintf(stderr," '%s'",Pname(ptr)->string);
		fprintf(stderr,"   RECURSION!!!\n");
		ptr->displayed = 0;
		fflush(stderr);
		return;
	}
	putc(' ',stderr);
	ptr->where.put(stderr);
	if ( oneline ) {
		putc('\n',stderr);
		fflush(stderr);
		return;
	}
	ptr->displayed = 1;
	switch ( ptr->base ) {
	case BLOCK:
		fprintf(stderr," .. ");
		ptr->where2.put(stderr);
		putc('\n',stderr);
		++indent;
		{ for( Pstmt st = ptr->s;  st;  st = st->s_list )
			display_stmt( ptr->s, "s" ); 
		}
		--indent;
		break;
	case IF:
		putc('\n',stderr);
		++indent;
		display_expr(ptr->e,"cond");
		display_stmt(ptr->s,"if-clause");
		--indent;
		if ( ptr->else_stmt ) {
			INDENT(indent);
			fprintf(stderr,"else\n");
			++indent;
			display_stmt(ptr->else_stmt,"else-clause");
			--indent;
		}
		break;
	case DO:
		putc('\n',stderr);
		++indent;
		display_stmt(ptr->s,"do-stmt");
		display_expr(ptr->e,"cond");
		--indent;
		break;
	case WHILE:
		putc('\n',stderr);
		++indent;
		display_expr(ptr->e,"cond");
		display_stmt(ptr->s,"while-stmt");
		--indent;
		break;
	case FOR:
		putc('\n',stderr);
		++indent;
		display_stmt(ptr->for_init,"init");
		display_expr(ptr->e,"cond");
		display_expr(ptr->e2,"incr");
		display_stmt(ptr->s,"stmt");
		--indent;
		break;
	case RETURN:
		putc('\n',stderr);
		++indent;
		display_expr(ptr->e,"e");
		--indent;
		break;
	case SM:
		putc('\n',stderr);
		++indent;
		display_expr(ptr->e,"e");
		--indent;
		break;
	default:
		putc('\n',stderr);
		break;
	}
	ptr->displayed = 0;
	fflush(stderr);
	return;
}

void
display_namelist( Plist nl, char* s, int verbose )
{
	error('d',"namelist:    %s",s?s:"");
	++indent;
	for ( Plist l = nl;  l;  l = l->l ) {
		Pname n = l->f;
		INDENT(indent);
		fprintf(stderr,"	%d",n);
		error('D'," %n %k n_key %d",n,n?n->tp->base:0,n?n->n_key:0);
		if ( verbose ) {
			++indent;
			display_expr(n);
			--indent;
		}
	}
	--indent;
}
#endif
