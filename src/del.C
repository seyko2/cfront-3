/*ident	"@(#)cls4:src/del.c	1.6" */
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

del.c:

	walk the trees to reclaim storage

**************************************************************/

#include "cfront.h"

void name::del()
{
	DB( if(Adebug>=1) {
		fprintf(stderr,"\n*** '%s'->del()",string?string:"???");
		display_type(tp); putc(' ',stderr);
		where.put(stderr); putc('\n',stderr);
	    }
	    if ( base!=NAME && base!=TNAME ) error('i',"name::del() of%k -- id==%ld",base,node::id);
	);
	Pexpr i = n_initializer;

// error('d', "\nname::del: %d %s", this, string?string:"no name" );
//	NFn++;
	DEL(tp);
	if(i && i!=(Pexpr)1) DEL(i);
	delete this;
}

void type::del()
{
	DB( if(Adebug>=1) {
		fprintf(stderr,"\n*** '");
		display_type(this);
		fprintf(stderr," '->del()\n");
	    }
	);
//fprintf(stderr,"DEL(type=%d %d)\n",this,base);
	permanent = 3;	/* do not delete twice */
	switch (base) {
	case TNAME:
	case NAME:
		error('i',"%d->T::del():N %s %d",this,Pname(this)->string,base);
	case FCT:
	{	Pfct f = (Pfct) this;
		DEL(f->returns);
		delete f;
		break;
	}
	case VEC:
	{	Pvec v = (Pvec) this;
		DEL(v->dim);
		DEL(v->typ);
		delete v;
		break;
	}
	case PTR:
	case RPTR:
	{	Pptr p = (Pptr) this;
		DEL(p->typ);
		delete p;
		break;
	}
	}
}

void expr::del()
{
	DB( if(Adebug>=1) {
		fprintf(stderr,"\n*** expr::del():  ");
		display_expr(this,0,1);
		if(Adebug>=2) display_expr(this);
	});
//fprintf(stderr,"DEL(expr=%d: %d %d %d)\n",this,base,e1,e2); fflush(stderr);
	permanent = 3;
	switch (base) {
	case IVAL:
		if (this == one) return;
	//case FVAL:
	case THIS:
	case ICON:
	case FCON:
	case CCON:
	case STRING:
	case TEXT:
		goto dd;
	case DUMMY: // null or error
	case DTOR:  // dummy type destructor
	case ZERO:
	case NAME:
		return;
	case CAST:
	case G_CAST:
	case SIZEOF:
	case NEW:
	case GNEW:
	case VALUE:
		DEL(tp2);
		break;
	case REF:
	case DOT:
		DEL(e1);
		if (e2) DEL(e2);
	case MDOT:
		if (mem && mem->base!=NAME) DEL(mem);
		if ( i1 == 5 ) delete string4;
		goto dd;
	case QUEST:
		DEL(cond);
		break;
	case ICALL:
		delete il->i_args;
		delete il;
		goto dd;
	case ELIST: { // limit recursion
		Pexpr tp = e2;
		while ( tp && tp->e2 && tp->e2->base == ELIST ) {
			Pexpr x = tp;
			if ( x->permanent ) break;
			x->permanent = 3;
			tp = tp->e2;
    			if ( x->e1 ) DEL(x->e1);
			delete x;
		}
		e2 = tp;
		break;
		}
	}

	DEL(e1);
	DEL(e2);
dd:
	delete this;
}

void stmt::del()
{
	DB( if(Adebug>=1) {
		fprintf(stderr,"\n*** stmt::del():  ");
		display_stmt(this,0,1);
		if(Adebug>=2) display_stmt(this);
	});
//fprintf(stderr,"DEL(stmt %d %s)\n",this,keys[base]); fflush(stderr);
	permanent = 3;
	switch (base) {
	case SM:
	case WHILE:
	case DO:
	case RETURN:
	case CASE:
	case SWITCH:
		DEL(e);
		break;
	case PAIR:
		DEL(s2);
		break;
	case BLOCK:
		DEL(d);
		DEL(s);
		if (own_tbl) DEL(memtbl);
		DEL(s_list);
		goto dd;
	case FOR:
		DEL(e);
		DEL(e2);
		DEL(for_init);
		break;
	case IF:
		DEL(e);
		DEL(else_stmt);
		break;
	}

	DEL(s);
	DEL(s_list);
dd:
	delete this;
}

void table::del()
{
// error('d',"\n*** %d::del() -- '%s'\n",this, t_name?t_name->string:"???");

	for (register int i=1; i<free_slot; i++) {
		Pname n = entries[i];
		if (n==0) error('i',"table.del(0)");
		DB( if(Adebug>=2) { fprintf(stderr,"  name: '%s'",n->string);
			display_type(n->tp); putc('\n',stderr);
		});

// error( 'd',"\ntable::del: %s n_scope: %d, n_stclass: %d", n->string, n->n_scope, n->n_stclass );

		if (n->n_stclass == STATIC) continue;
		switch (n->n_scope) {
		case ARG:
		case ARGT:
			break;
		default:
		{	char* s = n->string;
			if (s && (s[0]!='_' || s[1]!='_' || s[2]!='X')) delete s;
			/* delete n; */
			n->del();
		}
		}
	}
	//delete entries;
	//delete hashtbl;
	delete this;
}
#if 0
void ktable::del()
{
	delete this;
}
#endif

// local class
void delete_local() 
{
	DB( if(Adebug>=1) {
		fprintf( stderr, "delete_local: vlist: %d", vlist );
	});
// error( 'd', "delete_local: vlist: %d", vlist );
    do {
	for (vl* v = vlist; v; v = v->next) v->cl->really_print(v->vt);
	vlist = 0;

	for (Plist l=isf_list; l; l=l->l) {
		Pname n = l->f;
		Pfct f = Pfct(n->tp);
// error('d',"isf %n f %d",n,f);
		if ( f == 0 ) { error('d', "delete_local: f == 0" );break;} 
		if (f->base == OVERLOAD) {
			n = Pgen(f)->fct_list->f;	// first fct
			f = Pfct(n->tp);
		}

		if (debug_opt==0 && n->n_addr_taken) {
			f->f_inline = 0;
			if (n->n_dcl_printed<2) {
				if (warning_opt)
					error('w',"out-of-line copy of %n created",n);
				n->dcl_print(0);
			}
		}
	}
	//isf_list = 0;
    } while (vlist);

    for ( Plist l = local_class; l; l = l->l )
    {
          Pname n = l->f;
// error( 'd' , "delete_local() %d %n %t", n, n, n->tp );
	Pname nn = Pbase(n->tp)->b_name;
	Pclass cl = Pclass(nn->tp);
	for (Pname px, p=cl->mem_list; p; p=px) {
		px = p->n_list;
		if (p->tp)
		switch (p->tp->base) {
			case FCT:
			{	Pfct f = (Pfct)p->tp;
				if (f->body) {
 				    if (f->f_inline==0
				    && f->f_imeasure==0) {

					if (ansi_opt && f->f_this) {
						f->f_this->n_table = 0;
						for (Pname n=f->f_this->n_list; n; n=n->n_list) 
							n->n_table = 0;	
					}

					DEL(f->body);
					f->body = 0;
					}
				}
			}
			case COBJ:
			case EOBJ:
				DEL(p);
				break;
			case CLASS:
			case ENUM:
				break;
			default:
				delete p;
			} // end switch
		else delete p;
	} // end for mem
        DEL(cl->memtbl);
	cl->mem_list = 0;
	cl->permanent = 3;
        nn->permanent = 0;
	DEL(nn);
	n->permanent = 0;
	//SYM table_delete removed
	DEL(n);
    }
}
