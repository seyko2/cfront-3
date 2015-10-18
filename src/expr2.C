/*ident	"@(#)cls4:src/expr2.c	1.29" */
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

expr2.c:

	type check expressions

************************************************************************/

#include "cfront.h"
#include "size.h"
#include "overload.h"
#include "template.h"

static int const_obj1,const_obj2;
static void opov_error(Ptype, Ptype, TOK);
static Pname compare_builtin(Ptype, Ptype, Pname, Pname, Pname, int);
extern int oper_okay(Ptype, TOK);
extern Pname best_conv(const Block(Pname)&, Pname&, int&, bit);

Pname really_dominate(Pname on1, Pname on2, bit tc)
{

	Pname best = hier_dominates(on1,on2);
	if (best) return best;

	Pfct f1 = on1->get_fct();
	Pfct f2 = on2->get_fct();

	Ptype t1=f1->returns->skiptypedefs();
	Ptype t2=f2->returns->skiptypedefs();

	if (t1->is_ref()) t1 = Pptr(t1)->typ;
	if (t2->is_ref()) t2 = Pptr(t2)->typ;

	if (!t1 || !t2 || (t1->check(t2,OVERLOAD) && !const_problem)
	    ||
	    (
	      t1->is_ref() && t2->is_ref()
		&&
	      Pptr(t1)->typ->check(Pptr(t2)->typ,OVERLOAD) && !const_problem
	    )
	    ||
	    (
	      t1->is_ptr() && t2->is_ptr()
		&&
	      Pptr(t1)->typ->check(Pptr(t2)->typ,OVERLOAD) && !const_problem
	    )
	) {
		return 0;
	}

	// const check
	int c1 = f1->f_const;
	int c2 = f2->f_const;

	if(c1 == c2) ;
	else if(c1 && !c2) return tc ? on1 : on2;
	else if(c2 && !c1) return tc ? on2 : on1;

	return 0;
}

void name::assign()
{
	if (n_assigned_to++ == 0) {
		switch (n_scope) {
		case FCT:
			if (n_used && n_addr_taken==0)  {
				Ptype t = tp->skiptypedefs();
				switch (t->base) {
				case VEC:
					break;
				default:
					if (curr_loop)
						error('w',&where,"%n may have been used before set",this);
					else
						error('w',&where,"%n used before set",this);
				}
			}
		}
	}
}

void name::take_addr()
{
// error('d', "%n->take_addr tp: %t", this, tp?tp:0 );
// error('d', "%n->take_addr tp: %d %d", this, tp?tp:0, tp?tp->base:0 );
	// if ( (warning_opt) && (! n_addr_taken) && (tp) && (tp->base==FCT) && Pfct (tp)->f_inline)
	if ( (warning_opt) && (! n_addr_taken) && (tp) && (tp->base==FCT) && fct_type()->f_inline)
		error('w',"can't take address of inlineF%n,%n not inlined", this, this);  
	n_addr_taken++; 
	if ( n_sto == EXTERN && tp ) {
		Ptype t = tp->skiptypedefs();
		switch ( t->base ) {
			case COBJ:
				t = Pbase(t)->b_name->tp; // no break
			case CLASS: {
				Pclass cl = Pclass(t);
				if ( cl->c_body == 1 )
					cl->dcl_print(0);
			}
		}
	}
}

int ignore_const;	// for use by ref_init
static int is_dataMemPtr(Pexpr);

int expr::lval(TOK oper)
{
	register Pexpr ee = this;
	register Pname n;
	int deref = 0;
	char* es;

//error('d',"%k expr::lval %k",base,oper);

	switch (oper) {
	case ADDROF:
	case G_ADDROF:	es = "address of";	break;
	case DEREF:	es = "dereference of";	break;
	case INCR:	es = "increment of";	goto def;
	case DECR:	es = "decrement of";	goto def;
	default:	es = "assignment to";
	def:
		if (ignore_const==0 && tp->tconst()) {
			if (oper) {
				char *ms = vec_const?"array":fct_const?"function":"const type";
				if (base == NAME) {
					if (vec_const && Pname(this)->n_scope==ARG) break;
					error("%s%s%n",es,ms,this);
				}
				else
					error("%s%s",es,ms);
			}
			return 0;
		}
	}

	for(;;) {
//error('d',"loop %k",ee->base);
		switch (ee->base) {
	//	case G_CALL:
	//	case CALL:
		default:
		defa:
			if (deref == 0) {
				if (oper) error("%s%k (not an lvalue)",es,ee->base);
				return 0;
			}
			return 1;
		case ZERO:
		case CCON:
		case ICON:
		case FCON:
			if (oper) error("%s numeric constant",es);
			return 0;
		case STRING:
			if (oper) error('w',"%s string constant",es);
			return 1;
		case CAST:
		case G_CAST:
		switch( oper ) {
		case 0:
		case ADDROF:
		case G_ADDROF:
		case DEREF:
     			goto defa;
		default:
			if ( ee->tp->base == PTR 
			     && is_dataMemPtr(ee) ) 
			{ // check for const class object
    				Pexpr te;
				te = ee->e1->e1->e1;	 
       				if ( te->base == G_ADDROF )
					te = te->e2;
       				if ( te->base == NAME ) {
					Ptype pt = te->tp;
					if ( pt->base == PTR )
						pt = Pptr(pt)->typ;
					if ( pt->tconst() ) 
						error("%sCMP of const%n",es,te);
					return 0;
				}
			}
			goto defa;
		}

		case DEREF:
		{	
			Pexpr ee1 = ee->e1;
// error( 'd', "ee1: %k", ee1->base );
			switch (ee1->base) {	// *& vanishes
			case ADDROF:	// *&
				return 1;
			case G_CM:
			case CM:	// look for *(a,&b)
				if (ee1->e2->base==G_ADDROF
				|| ee1->e2->base==ADDROF)
					return 1;
				goto defaa;
			case QUEST:	// look for *(q?&a:&b)
				if ((ee1->e1->base==G_ADDROF
					|| ee1->e1->base==ADDROF)
				&& (ee1->e2->base==G_ADDROF
					|| ee1->e2->base==ADDROF))
					return 1;
				// no break
			default:
			defaa:
				ee = ee1;
				deref = 1;
			}
			break;
		}

		case QUEST:
		{	int x1 = ee->e1->lval(deref?0:oper);
			int x2 = ee->e2->lval(deref?0:oper);
			if (ee->e1->tp->check(ee->e2->tp,0)) return 0;
			if (deref) return 1;
			return x1 && x2;
		}

		case INCR:
		case DECR:
			if (e1) goto defa;	// postfix does not preserve lval
		case ASSIGN:
		case ASPLUS:
		case ASMINUS:
		case ASMUL:
		case ASDIV:
		case ASMOD:
		case ASAND:
		case ASOR:
		case ASER:
		case ASLS:
		case ASRS:
			return 1;

		case CM:
		case G_CM:
			if (ee->e2->lval(deref?0:oper)==0) return deref;
			return 1;

		case MEMPTR:
			ee = ee->e2;
			break;

		case MDOT:
			ee = ee->mem;
			break;

		case DOT:
		{
// error('d',"dot %k oper %k",ee->e1->base,oper);
			Pexpr e = 0;
			int e_const = 0; // to catch x.y.val = 1, where x is const

			switch (ee->e1->base) {		// update use counts, etc.
			case NAME:
				switch (oper) {
				case ADDROF:
				case G_ADDROF:	Pname(ee->e1)->take_addr();
				case 0:		break;
				case ASSIGN:	Pname(ee->e1)->n_used--;
				default:	Pname(ee->e1)->assign(); // asop
				}
				break;
			case DOT:
				e = ee->e1;
				do e=e->e1; while (e->base==DOT);
				if (e->base == NAME) {
					e_const = e->tp->tconst();
					switch (oper) {
					case ADDROF:
					case G_ADDROF:	Pname(e)->take_addr();
					case 0:		break;
					case ASSIGN:	Pname(e)->n_used--;
					default:	Pname(e)->assign(); // asop
					}
				}
			}

			n = Pname(ee->mem);
			while (n->base == MDOT) n = Pname(Pref(n)->mem);

			if (deref==0 && 
				(ee->e1->tp->tconst() || e_const)) {

				switch (oper) {
				case 0:
				case ADDROF:
				case G_ADDROF:
				case DEREF:
					break;
				default:
					error("%sM%n of%t",es,n,e_const?e->tp:ee->e1->tp);
				}
				return 0;
			}
		}
		goto xx;

		case REF:
			n = Pname(ee->mem);
			while (n->base == MDOT) n = Pname(Pref(n)->mem);

			if (deref==0 && ee->e1) {  //BR
				Ptype p = ee->e1->tp->skiptypedefs();

				switch (p->base) {
				case PTR:
				case VEC:	break;
				case ANY:	return 0;
				default:	error('i',"expr::lval%t ->%n",p,n);
				}
				if (ignore_const==0 && Pptr(p)->typ->tconst()) {
					switch (oper) {
					case 0:
					case ADDROF:
					case G_ADDROF:
					case DEREF:
						break;
					default:
						error("%sM%n of%t",es,n,Pptr(p)->typ);
					}
					return 0;
				}
			}
			goto xx;

		case NAME:
			n = Pname(ee);
		xx:
// error('d',"name xx: %n oper %d lex_level: %d",n,oper,n->lex_level);
			if (deref) return 1;
			if (oper==0) return n->n_stclass != ENUM ;

			if (n->tp->base==FIELD && Pbase(n->tp)->b_bits==0) {
				error("%s 0-length field%n",es,n);
				return 0;
			}

			switch (oper) {
			case ADDROF:
			case G_ADDROF:
			{
				if (n->n_sto == REGISTER) {
					if (warning_opt) error('w',"& register%n",n);
				//	return 0;
					n->n_sto = 0;
					n->n_stclass = AUTO;
				}

				if (n->tp == 0) {
					error("& label%n",n);
					return 0;
				}

				if (n->n_stclass == ENUM) {
					error("& enumerator%n",n);
					return 0;
				}

				if (n->tp->base == FIELD) {
					error("& field%n",n);
					return 0;
				}

				n->n_used--;
				if (n->n_qualifier) { // oops, not the real one
					Pname tn = Pclass(n->n_table->t_name->tp)->memtbl->look(n->string,0);
					n = tn ? tn : n;
				}
				n->take_addr();

				// suppress hoisting of local consts
				int statmem = n->n_scope==0 || n->n_scope==PUBLIC || n->n_scope == FCT;
				if (n->n_evaluated && n->n_scope!=ARG) { // &const
					if (statmem == 0 && n->n_dcl_printed==0) {
						n->n_initializer = new ival(n->n_val);
						n->dcl_print(0);
					}
				}
				else if (n->tp->base==FCT && n->n_dcl_printed==0) {
					Pfct f = Pfct(n->tp);
					if (f->fct_base == INSTANTIATED)
						current_fct_instantiation = f;
					if (!f->fct_base || f->fct_base == INSTANTIATED)
						n->dcl_print(0);
					if (f->fct_base == INSTANTIATED)
						current_fct_instantiation = 0;
				}
				break;
			}

			case ASSIGN:
//error('d',"ass %n %d",n,n->n_used);
				n->n_used--;
				n->assign();
				break;
			case INCR:
			case DECR:
				if (n->tp->skiptypedefs()->base == EOBJ) {
					error("%s enum",es);
					return 0;
				}
				// no break
			default:	/* incr ops, and asops */
				if (cc->tot && n==cc->c_this) {
					error("%n%k",n,oper);
					return 0;
				}
				n->assign();
			}
			return 1;
		}
	}
}

int char_to_int(char* s)
/*	assume s points to a string:
		'c'
	or	'\c'
	or	'\0'
	or	'\ddd'
	or multi-character versions of the above
	(hex constants have been converted to octal by the parser)
*/
{
	register int i = 0;
	register char c, d, e;

	switch (*s) {
	default:
		error('i',"char constant store corrupted");
	case '`':
		error("bcd constant");
		return 0;
	case '\'':
		break;
	}

	for(;;)			/* also handle multi-character constants */
	switch (c = *++s) {
	case '\'':
		return i;
	case '\\':			/* special character */
		switch (c = *++s) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7':	/* octal representation */
			c -= '0';
			switch (d = *++s) {		/* try for 2 */
				
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7':
				d -= '0';
				switch (e = *++s) {	/* try for 3 */
					
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7':
					c = c*64+d*8+e-'0';
					break;
				default:
					c = c*8+d;
					s--;
				}
				break;
			default:
				s--;
			}
			break;
		case 'a':
			c = '\a';
			break;
		case 'b':
			c = '\b';
			break;
		case 'f':
			c = '\f';
			break;
		case 'n':
			c = '\n';
			break;
		case 'r':
			c = '\r';
			break;
		case 't':
			c = '\t';
			break;
		case 'v':
			c = '\v';
			break;
		case '\\':
			c = '\\';
			break;
		case '\'':
			c = '\'';
			break;
		}
		/* no break */
	default:
		if (i) i <<= BI_IN_BYTE;
		i += c;
	}
}

const int A10 = 'A'-10;
const int a10 = 'a'-10;

long str_to_long(register const char* p)
{
	register int c,j;
	register int dotflag=0;
	register int dotcount=0;
	register long exp=0;
	register unsigned long i= 0;
	const char* pp = p;

// error( 'd', "str_to_long: %s", p );

	if ((c=*p++) == '0') {
		switch (c = *p++) {
		case 0:
			return 0;

		case 'l':
		case 'L':	/* long zero */
			return 0;

		case 'x':
		case 'X':	/* hexadecimal */
			while (c=*p++) {
				switch (c) {
				case 'l':
				case 'L':
				case 'U':
				case 'u':
					return i;
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
					i = i*16 + c-A10;
					break;
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					i = i*16 + c-a10;
					break;
				default:
					i = i*16 + c-'0';
				}
			}
			return i;

		default:	/* octal */
			do 
				switch (c) {
				case 'l':
				case 'L':
				case 'U':
				case 'u':
					return i;
				default:
					i = i*8 + c-'0';
				}
			while (c=*p++);
			return i;
		}
	}	
				/* decimal */
	i = c-'0';
	while (c=*p++)
		switch (c) {
		case 'l':
		case 'L':
		case 'U':
		case 'u':
			return i;
		case '.':
			dotflag=1;
			break;
		case 'e':
		case 'E':	/* scientific notation */
		{	int negative = 1;
			if (*p == '+' || *p == '-')
				negative = (*p++ == '-' ? -1 : 1);
			exp=str_to_long(p)*negative;
			exp=exp-dotcount;
			if (exp >= 1) {
				unsigned long ii = i;
				for (j=0;exp>j;j++, ii=i) {
				    i = i*10;
				    if (i<ii) {
					error('w',"integral conversion of %s is out of range",pp);
					return i;
				    }
				}
			} else
				for (j=0;exp<j;j--) i=i*(0.1);
			return i;
		}
		default:
		{       unsigned long ii = i;
			i = i*10 + c-'0';
			if (dotflag) dotcount++;
			if (i<ii) goto bad;
		}
		}
	return i;
bad:
	error("integer constant %s larger than the largest long",pp);
	return i;
}

static void
ftp_normalize(char*& str)
/*
   strip off leading '0' in a scientific floating point string 
   so that str_to_long() can process it correctly.
*/
{
	char* p = str;
	char* pp = str;
	char  c;
	register int adjust = 0;
	register int dotflag = 0;
	register int dotcnt = 0;

	while (c = *p) {
		switch (c) {
		case '+': 
		case '-': 
				pp++;
				break;
		case '0': 
				if (dotflag) dotcnt++;
				break;
		case '.': 
				dotflag = 1;
				break;
		default:  {			// non-zero digits
				if (p == pp) return;
				*pp++ = *p++;
				if (dotflag == 0) {
					while (*p) *pp++ = *p++;
					*pp = 0;
					return;
				} else {
					*pp++ = '.';
					while (*p != 'E' && *p != 'e') 
						if (*p == '.') p++;
						else *pp++ = *p++;
				}
				adjust = 1;
			  }
		case 'E':
		case 'e': {
				if (adjust == 0) {	// 0 significant value
					*pp++='0';
					*pp = 0;
					return;
				}
				// have to adjust exponent
				*pp++ = *p++;
				int sign = 1;
				if (*p == '+' || *p == '-') 
					sign = (*p++ == '-' ? -1 : 1);
				long i = sign * str_to_long(p) - dotcnt - 1;
				char tmp[40]; // enough for 128-bit doubles
				sprintf(tmp,"%-d",i);
				if ((strlen(p)+(p-pp)) < strlen(tmp)) {
					char *newstr = new char[
						strlen(tmp)+(pp-str)+1];
					strcpy(newstr, str);
					int offset = (pp - str);
					str = newstr;
					pp = str + offset;
				}
				sprintf(pp,"%-d",i);
				return;
			  }
		}
		p++;
	}
}


bit type::is_unsigned()
{
	Ptype t = skiptypedefs();
	if (t->base == PTR) return 0;
	return Pbase(t)->b_unsigned;
}

char* Neval;
bit binary_val;

unsigned long expr::ueval(long x1, long x2)
{
	unsigned long i1 = (unsigned long) x1;	
	unsigned long i2 = (unsigned long) x2;
//error('d',"ueval %k %ld %ld",base,x1,x2);
	switch (base) {
	case UMINUS:	return -i2;
	case UPLUS:	return i2;
	case NOT:	return !i2;
	case COMPL:	return ~i2;
	case CAST:
	case G_CAST:	
			return i1;
	case PLUS:	return i1+i2;
	case MINUS:	return i1-i2;
	case MUL:	return i1*i2;
	case LS:	return i1<<i2;
	case RS:	return i1>>i2;
	case NE:	return i1!=i2;
	case EQ:	return i1==i2;
	case LT:	return i1<i2;
	case LE:	return i1<=i2;
	case GT:	return i1>i2;
	case GE:	return i1>=i2;
	case AND:	return i1&i2;
 	case ANDAND:	return i1&&i2;
	case OR:	return i1|i2;
	case OROR:	return i1||i2;
	case ER:	return i1^i2;
	case MOD:	if (i2 == 0) {
				if (Neval == 0) { 
					Neval = "mod zero";
					error("mod zero");
				}
				return 1;
			}
			return i1%i2;
	case QUEST:	return (cond->eval()) ? i1 : i2;
	case DIV:	if (i2 == 0) {
				if (Neval == 0) { 
					Neval = "divide by zero";
					error("divide by zero");
				}
				return 1;
			}
			return i1/i2;
	case CM:
	case G_CM:
		return i2;
	}

	Neval = "unsigned expression";
	return 0;
}

long expr::eval()
{
	if (Neval) return 1;
// error('d',"eval %k",base);
	Ptype tt;

	switch (base) {
	case ZERO:	return 0;
	case IVAL:	return i1;
	case ICON:	return str_to_long(string);
	case CCON:	return char_to_int(string);
	case FCON:	Neval = "float in constant expression"; return 1;
	case STRING:	Neval = "string in constant expression"; return 1;
	case EOBJ:	return Pname(this)->n_val;
	case SIZEOF:
	{
		extern int no_sizeof;
		if (no_sizeof) Neval = "sizeof";
		char *cese = "cannot evaluate sizeof expression";
		if (!tp2) {
			Neval = cese;
			return 1;
		} else {
			tt = tp2;
			while (tt && tt->vec_type()) {	// ANY,VEC,PTR,RPTR
				tt = tt->skiptypedefs();
				if (tt->base==ANY) {
					Neval = cese;
					return 1;
				}
				tt = (tt->base==VEC) ?
						Pvec(tt)->typ
						:
						Pptr(tt)->typ;
			}
		}
		return tp2->tsizeof();
	}
		
	case NAME:
	{	Pname n = Pname(this);
// error('d',"eval %n eval %d %d",n,n->n_evaluated,n->n_val);
// error('d',"eval tp->tconst() %d, n->n_initializer: %k", n->tp->tconst(), n->n_initializer?n->n_initializer->base:0 );
		if (n->n_evaluated && n->n_scope!=ARG) return n->n_val;
		if (binary_val && strcmp(string,"_result")==0) return 8888;
		Neval = "cannot evaluate constant";
		return 1;
	}
	case ICALL:
		if (e1) {
			il->i_next = curr_icall;
			curr_icall = il;
			long i = e1->eval();
			curr_icall = il->i_next;
			return i;
		}
		Neval = "void inlineF";
		return 1;
	case ANAME:
	{	Pname n = (Pname)this;

		Pin il;
		for (il=curr_icall; il; il=il->i_next)
			if (il->i_table == n->n_table) goto aok;
		goto bok;
	aok:
		if (il->i_args[n->argno].local) {
	bok:
			Neval = "inlineF call too complicated for constant expression";
			return 1;
		}
		Pexpr aa = il->i_args[n->argno].arg;
		return aa->eval();
	}
	case CAST:
	case G_CAST:
	{	if (tp2->base!=FLOAT && tp2->base!=DOUBLE && (e1->base==FCON 
		|| (e1->base==UMINUS || e1->base==UPLUS) && e1->e2->base==FCON)) {
			char*& str = (e1->base==FCON ? e1->string : e1->e2->string);
			char* p = str;
			while (*p && *p!='.' && *p!='E' && *p!='e') p++;
			if (*p) {
			    if ((strchr(p,'E')==0) && (strchr(p,'e')==0)) {
				if (p==str) *p++ = '0';
				*p = 0;
			    } else {
				ftp_normalize(str);
			    }
			}
			if (e1->base == FCON)
				e1->base = ICON;
			else
				e1->e2->base = ICON;
		}		
		long i = e1->eval();
		tt = tp2->skiptypedefs();

		switch (tt->base) {
		default:
			Neval = "cast to non-integral type in constant expression";
			break;
		case ENUM:
		case EOBJ:
		case LONG:
		case INT:
		case CHAR:
		case SHORT:
		     {
			long j = (((~(unsigned long)0)<<(BI_IN_BYTE*(tp2->tsizeof()-1)))<<BI_IN_BYTE);
			i &= ~j;
			if (tt->is_unsigned()==0)
			   if (long((i<<(BI_IN_BYTE*(sizeof(long)-tp2->tsizeof())))) < 0)
				i |= j;
			break;
		     }
		}
		return i;
	}
	case UMINUS:
	case UPLUS:
	case NOT:
	case COMPL:
	case PLUS:
	case MINUS:
	case MUL:
	case LS:
	case RS:
	case NE:
	case LT:
	case LE:
	case GT:
	case GE:
	case AND:
	case OR:
	case ER:
	case DIV:
	case MOD:
	case QUEST:
	case EQ:
	case ANDAND:
		break;
	case OROR:
		if (binary_val) {	// a||b, don't evaluate b if a!=0
			long i1 = (e1) ? e1->eval() : 0;
			if (Neval==0 && i1 && e1->tp->is_unsigned()==0) return i1;
		}
		break;
	case CM:
	case G_CM:
		Neval = "comma operator in constant expression";
		return 1;
	case G_ADDROF:
	case ADDROF:
		if (binary_val) {	// beware of &*(T*)0
			switch (e2->base) {
			case NAME:
			case DOT:
			case REF:	return 9999;
			}
		}
	default:
		Neval = "bad operator in constant expression";
		return 1;
	}

	long i1 = (e1) ? e1->eval() : 0;
	long i2 = (e2) ? e2->eval() : 0;

	if (binary_val && i1==9999 && i2==9999) {
		Neval = "";
		return 1;
	}
	
	if (Neval==0
	    && (
		e1&&e1->tp&&e1->tp->is_unsigned()
		 || 
		e2&&e2->tp&&e2->tp->is_unsigned()
	    )
	)
		return (long) ueval(i1,i2);
	
	switch (base) {
	case UMINUS:	return -i2;
	case UPLUS:	return i2;
	case NOT:	return !i2;
	case COMPL:	return ~i2;
	case CAST:
	case G_CAST:	
			return i1;
	case PLUS:	return i1+i2;
	case MINUS:	return i1-i2;
	case MUL:	return i1*i2;
	case LS:	return i1<<i2;
	case RS:	return i1>>i2;
	case NE:	return i1!=i2;
	case EQ:	return i1==i2;
	case LT:	return i1<i2;
	case LE:	return i1<=i2;
	case GT:	return i1>i2;
	case GE:	return i1>=i2;
	case AND:	return i1&i2;
 	case ANDAND:	return i1&&i2;
	case OR:	return i1|i2;
	case OROR:	return i1||i2;
	case ER:	return i1^i2;
	case MOD:	if (i2 == 0) {
				if (Neval == 0) { 
					Neval = "mod zero";
					error("mod zero");
				}
				return 1;
			}
			return i1%i2;
	case QUEST:	return (cond->eval()) ? i1 : i2;
	case DIV:	if (i2 == 0) {
				if (Neval == 0) { 
					Neval = "divide by zero";
					error("divide by zero");
				}
				return 1;
			}
			return i1/i2;
	case CM:
	case G_CM:
		return i2;
	}

	error('i', "fall off end of expr::eval()");
	return 0;
}
#if 0
bit classdef::baseof(Pname f)
/*
	is ``this'' class a public base class of "f"'s class
	or its immediate base class
*/
{
	Ptable ctbl = f->n_table;
	Pname b = ctbl->t_name;

	if (b == 0) return 0;
	Pclass cl = Pclass(b->tp);
	if (cl == 0) return 0;
	if (::same_class(cl,this)) return 1;
	ppbase = PUBLIC;
	Pclass bcl = is_base(cl->string);
	return (bcl && ppbase==PUBLIC);
}
#endif

bit classdef::baseof(Pclass cl)
/*
	is ``this'' class a public base class of "cl"
*/
{
	if (cl == 0) return 0;
	if (::same_class(cl,this)) return 1;
	ppbase = PUBLIC;
	Pclass bcl = is_base(cl->string);
	return (bcl && ppbase==PUBLIC);
}

static int mem_match(Pfct f1, Pfct f2)
/*
	check class membership.

	For some reason checking f_this==0 works and f_static doesn't
*/
{
//	if (f1->memof) return f2->f_this ?f2->memof==f1->memof : 0;
//	if (f1 && f1->memof) return f2->f_this?f2->memof==f1->memof : 0;
//	return f2->f_this==0;
	if (f1==0 || f2==0) return 0;
	if (f1->memof && f2->f_this && !same_class(f2->memof,f1->memof)) return 0;
	if (f2->f_this) return 0;
	if (f1->memof && f2->f_static) return 0;
	if (f1->check(f2,ASSIGN)) return 0;
	return 1;
}

int Pchecked;

static Pexpr
return_elist(Pname args)
{
/*
	make a list of expressions out of a function argument list
*/
	Pexpr head=0, tail=0;

	for (Pname n=args; n; n=n->n_list)
	{
		Pexpr e = new expr(ELIST,n,0);
		if (head==0)
			head = e;
		if (tail)
			tail->e2 = e;
		tail = e;
	}

	return head;
}

Pexpr 
ptof(Pfct ef, Pexpr e, Ptable tbl)
/*
	a kludge: initialize/assign-to pointer to function
*/
{
	Pfct f;
	Pname n = 0;
eee:
//error('d',"ptof %t %t %k",ef,e->tp,e->base);
	switch (e->base) {
	case QUEST:
		e->e1 = ptof(ef,e->e1,tbl);
		e->e2 = ptof(ef,e->e2,tbl);
		return e;
	case CM:
	case G_CM:
		e->e2 = ptof(ef,e->e2,tbl);
		if (e->tp && e->tp->base == FCT) e->tp = e->e2->tp;
		return e;
	case NAME:
		f = Pfct(e->tp);
		n = Pname(e);

		switch (f->base) {
		case OVERLOAD:
			e = Pgen(f)->find(ef,0);

                        if (e == 0 && n->is_template_fct()) {
                                Pexpr arg = return_elist(ef->argtype);
                                e = has_templ_instance(n,arg);
                        }

			if (e == 0) {
				error("cannot deduceT for &overloaded%n",n);
				return e;
			}
			// no break
		case FCT:
			// if (f->base == FCT && f->is_templ()) {
			if (f->base == FCT && f->fct_base == FCT_TEMPLATE) {
                                Pexpr arg = return_elist(ef->argtype);
                                e = has_templ_instance(n,arg);
                                if (e==0) return e; // error already reported
                        }

			Pchecked = mem_match(ef,Pfct(e->tp));
			e = new expr(G_ADDROF,0,e);
			return e->typ(tbl);	// handle &B::f
			//e->tp = f;
		}
		goto ad;

	case ZERO:
		if (ef->memof) {
			e = new expr(ELIST,zero,zero);
			e = new expr(ILIST,e,zero);
			e->tp = zero_type;
			return e; 
		}
		break;

	case MDOT:
		// ?? error('s',"P toM of not firstB");
		do e = e->mem; while (e->base == MDOT);
		goto eee;

	case DOT:
	case REF:
		f = Pfct(e->mem->tp);

		switch (f->base) {
		case OVERLOAD:
			n = Pgen(f)->find(ef,0);
			if (n == 0) error("cannot deduceT for &overloaded%n",e->mem);
			else e = n;
			// no break
		case FCT:
			Pchecked = mem_match(ef,Pfct(e->tp));
			e = new expr(G_ADDROF,0,e);
			return e->typ(tbl);	// handle &B::f
		//	n = Pname(e->mem);
		//	e = n->address();
		}
		goto ad;

	case ADDROF:
	case G_ADDROF:
		f = Pfct(e->e2->tp);
		n = Pname(e->e2);
// error('d',"ptof: address_of!!! f: %t n: %n ", f,n);
	ad:
		if (f->base == OVERLOAD) {
			Pname nn = Pgen(f)->find(ef,0);

			if (nn == 0 && n->is_template_fct()) {
				Pexpr arg = return_elist(ef->argtype);
				nn = has_templ_instance(n,arg);
			}
				
			if (nn == 0) {
				error("cannot deduceT for &overloaded %s()",Pgen(f)->fct_list->f->string);
				return nn;
			}

			n = nn;
			Pfct nf = n->fct_type();
			Pchecked = mem_match(ef,nf);
			if (nf->f_static) nf->memof = 0;
			e->e2 = n;
			e->tp = n->tp;
		}

		// if (f->base == FCT && f->is_templ()) {
		if (f->base == FCT && f->fct_base == FCT_TEMPLATE) {
			Pexpr arg = return_elist(ef->argtype);
			Pname nn = has_templ_instance(n,arg);
			if (nn==0) return nn; // error already reported
			n = nn;
			Pchecked = mem_match(ef,n->fct_type());
			e->e2 = n;
			e->tp = n->tp;
		}
		if (n) n->lval(ADDROF);
		break;

	case CAST:
	case G_CAST:
	{
		Pexpr te = e->e1;
		if (e->e1->base == G_ADDROF) te = e->e1->e2;
		(void) ptof(ef,te,tbl);
	}
	}
	return e;
}

Pexpr ptr_init(Pptr p, Pexpr init, Ptable tbl)
/*
	check for silly initializers

	char* p = 0L;	 ??	fudge to allow redundant and incorrect `L'
	char* p = 2 - 2; ??	worse
*/
{
// error('d',"ptr_init: p=%t init->tp=%t init->base %k",p,init->tp,init->base);

	Pchecked = 0;

	Ptype it = init->tp->skiptypedefs();

	switch (it->base) {
	case ZTYPE:
		break;
	case EOBJ:
	case INT:
	case CHAR:
	case SHORT:
	case LONG:
	{	Neval = 0;
		long i = init->eval();
		if (Neval)
			error("badPIr: %s",Neval);
		else
		if (i)
			error("badPIr value %d",i);
		else {
			DEL(init);
			init = zero;
		}
		break;
	}		
	}

	Pclass c1 = p->memof;

	if (c1) {
		if (init==zero)
			;
		else {
			Pclass c2;
// error('d',"it %t %d",it,it->base);
			switch (it->base) {
			case FCT:
				c2 = Pfct(it)->memof;
				break;
			case OVERLOAD:
				c2 = Pfct(Pgen(it)->fct_list->f->tp)->memof;
				break;
			case PTR:
			case RPTR:
				c2 = Pptr(it)->memof;
				break;
			default:
			    c2 = 0;
			    if ( init->base == QUEST) {
				// cannot have sides return (expr?{}:{})
				// reuse same temp for both sides ?:
				Pname temp = make_tmp( 'A', mptr_type, tbl );

				init->e1 = mptr_assign( temp, init->e1 );
				init->e1 = new expr( G_CM, init->e1, temp );
				init->e1->tp = temp->tp;
		
				init->e2 = mptr_assign( temp, init->e2 );
				init->e2 = new expr( G_CM, init->e2, temp );
				init->tp = p;
			    }
			}

			if (c2 != 0 && !same_class(c1,c2)) {
                                Nptr = 0;
				Noffset = 0;
				vcllist->clear();
				vcllist=0;
				int u1 = is_unique_base(c1,c2->string,0);
//error('d',"c1 %t c2 %t u1 %d off %d",c1,c2,u1,Noffset);
                                if (u1 && (Nptr || Noffset)) {
					// requires offset manipulation
					int bad = 0;
        				if (u1 == 1 && !Nptr) {
						if (init->base==ILIST) {
							// d = d+Noffset;
							switch (init->e1->e1->base) {
								case IVAL:
									init->e1->e1->i1 += Noffset;
									break;
								case ZERO:
									init->e1->e1 = new ival(Noffset);
									break;
								default:
									bad = 1;
								}

							// if (i<0) f = vptroffset
							switch (init->e1->e2->base) {
								case IVAL:
									if (0<init->e1->e2->i1) {
									// extern Ptype Pfct_type;
									// store vptr offset
									//  init->e2=new cast(Pfct_type,zero);
									}
									else
										break;
								default:
									bad = 1;
								}	
						} // end if (init->base==ILIST)
						else
							bad = 1;
					} // end if (u1 == 1 ...
					else
						bad = 1;

				if (bad) error('s',"%t assigned to%t (too complicated)",init->tp,p);
				} // end if (u1 && ...

                                Nptr = 0;
				Noffset = 0;
				vcllist->clear();
				vcllist=0;
				int u2 = is_unique_base(c2,c1->string,0);
//error('d',"c1 %t c2 %t u2 %d off %d",c1,c2,u2,Noffset);
                                if (u2 && (Nptr || Noffset)) {
					// requires offset manipulation
					error('s',"%t assigned to%t",init->tp,p);
				}
			} // end if (c1 != c2
		} // end else
	} // end if (c1)

	Ptype pit = p->typ->skiptypedefs();
// error('d',"p %t pit %t",p,pit);
	switch (pit->base) {
	case FCT:
		return ptof(Pfct(pit),init,tbl);
	case COBJ:
	{	
		Pptr r = it->is_ptr_or_ref();
		Pexpr x = 0;
		Pname old_Ncoerce = Ncoerce;
		Ncoerce = 0;
		if ( r == 0 ) {
			suppress_error++;
			 x = try_to_coerce(p,init,"initializer",tbl);;
			suppress_error--;
		}

// error('d',"cobj: ptr %t, ref %t",it->is_ptr(),it->is_ref());
		if (r != 0 || (x && Ncoerce && (r = x->tp->is_ptr_or_ref()))) 
		{
			if (x && Ncoerce) init = x;
			Pchecked = 1;
			TOK b = p->base;		// could be REF
			TOK bb = r->base;
			if (b==RPTR) p->base = PTR;
			if (bb==RPTR) r->base = PTR;
			if (p->check(r,ASSIGN)) {
				if ( cc && cc->nof && 
					Pfct(cc->nof->tp)->f_const &&
        				cc->c_this == init )
						error("%n const: assignment of%n (%t) to%t",cc->nof,init,init->tp,p);
				else {
					p->base=b;
					if (init->base==G_ADDROF)
						error("no standard conversion of%t to%t",init->e2->tp,p);
					else 
						error("no standard conversion of%t to%t",init->tp,p);
				}
			}
			p->base = b;
			r->base = bb;
			Pexpr cp = cast_cptr(Pclass(Pbase(pit)->b_name->tp),init,tbl,0);
			if (cp != init) {
				PERM(p);	// or else it will be deleted twice!
				Ncoerce = old_Ncoerce;
				return new cast(p,cp);
			}
		}
		Ncoerce = old_Ncoerce;
		// no break
	}
	default:
		return init;
	}
}

static Pname Lcoerce, Rcoerce;

int try_to_demote(TOK oper, Ptype t1, Ptype t2)
/*
	look at t1 and t2 and see if there are ``demotions'' of t1 
	and/or t2 so that ``t1 oper t2'' can be made legal

	return	< 0 if there is not
		  1 if there is exactly one way
		> 1 if there is more than one way
*/
{
//error('d',"try_to_demote(%k : %t : %t)",oper,t1,t2);

	Pname n1 = t1 ? t1->is_cl_obj() : 0;
	Pclass c1 = n1 ? Pclass(n1->tp) : 0;
	Pname n2 = t2 ? t2->is_cl_obj() : 0;
	Pclass c2 = n2 ? Pclass(n2->tp) : 0;

	Ptype lt = t1;
	Ptype rt = t2;
	int not_const1, not_const2;

	Lcoerce = Rcoerce = 0;

	Block(Pname) rconv;
	Block(Pname) lconv;
	int rfound = 0;
	int lfound = 0;

	if (c1)
		switch (oper) {
		case ASSIGN:
		case ASPLUS:
		case ASMINUS:
		case ASMUL:
		case ASDIV:
		case ASMOD:
		case ASAND:
		case ASOR:
		case ASER:
		case ASLS:
		case ASRS:	// don't coerce left hand side of assignment
			if (c1->memtbl->look("__as",0)) return 0;
		}
	else
		switch (oper) {
		case ADDROF:
		case INCR:
		case DECR:	// don't coerce unary requiring an lval
			return 0;
		}

	if (c1) {
		not_const1 = 0;
		// find best conversions for c1
		for (Pname on1 = c1->conv; on1; on1=on1->n_list) {
			Pfct f = on1->get_fct();
			lt = f->returns;
			if (lt->is_cl_obj()) continue;
			Pptr rn = lt->is_ref();
			if(rn && rn->typ->is_cl_obj()) continue;
			// this has to be here because in np_promote
			// it screws up type checking
			if (lt->skiptypedefs()->base==EOBJ
			    && oper >= ASPLUS && oper <= ASRS) continue;
			if (t1->tconst() && !f->f_const) {
				not_const1 = 1;
				continue;
			}
			lconv.reserve(lfound+1);
			lconv[lfound++] = on1;
		}

		if (lfound == 0) return not_const1 ? -1 : 0;

		Bits b(0,lfound);
		if (lfound == 1) b = ~b;
		else b = best_conv(lconv,lfound,const_obj1);

		// for each best conversion in c1, look at right side
		while (b.count()) {
			int i = b.signif() - 1;
			b.reset(i);
			Pfct f = lconv[i]->get_fct();
			lt = f->returns;
			Pptr rr = lt->is_ref();
			if(rr) lt = rr->typ;
			if(lt->base==OVERLOAD)
			    lt = Pgen(lt)->fct_list->f->tp;
			int r1 = lt->kind(oper,0,0);

			if (c2) {
				not_const2 = 0;
				rfound = 0;
				for(Pname on2=c2->conv; on2; on2=on2->n_list) {
					Pfct f = on2->get_fct();
					rt = f->returns;
					Pptr rn = rt->is_ref();
					if(rn) rt = rn->typ;
					if(rt->is_cl_obj()) continue;
					if(rt->base==OVERLOAD)
					    rt = Pgen(rt)->fct_list->f->tp;
					int r2 = rt->kind(oper,0,0);
					if (np_promote(oper,r1,r2,lt,rt,1,0)
					    !=
					    any_type
					) {
						if(t2->tconst() && !f->f_const) {
							not_const2 = 1;
							continue;
						}
						rconv.reserve(rfound+1);
						rconv[rfound++] = on2;
					}
				}
				// find Rcoerce
				if(rfound == 0) {
					//if(not_const2) return -2;
					//return 0;
					continue;
				}
				if(rfound == 1) {
					if(Lcoerce) return 2;
					Lcoerce=lconv[i]; Rcoerce=rconv[0];
					continue;
				}
				Bits b2 = best_conv(rconv,rfound,const_obj2);
				if(rfound==1) {
					if(Lcoerce) return 2;
					Lcoerce=lconv[i];
					Rcoerce=rconv[b2.signif()-1];
				}
				//else if (b.count() == 0) {
				else {
					if(lfound > 1) return 4;
					return 3;
				}
			}
			else if (rt) {
				if(rt->base==OVERLOAD)
					rt = Pgen(rt)->fct_list->f->tp;
				int r2 = rt->kind(oper,0,0);
				if (np_promote(oper,r1,r2,lt,rt,1,0)
				    !=
				    any_type
				) {
					if(Lcoerce) return 2;
					Lcoerce = lconv[i];
					if (b.count() == 0) return 1;
				}
			}
			else {
				if(oper==MUL && r1 != 'P') continue;
				if (oper==CALL && !lt->is_or_pts_to(FCT))
					continue;
				if (lt->skiptypedefs()->base == EOBJ
				    &&
				    (oper==INCR || oper==DECR)
				)
					continue;

				if(Lcoerce) return 2;
				Lcoerce = lconv[i];
				if (b.count() == 0) return 1;
			}
		}
		//if(lfound>1) return 2;
		return (Lcoerce || Rcoerce);
	}

	// otherwise, !c1 && c2
	not_const2 = 0;
	for (Pname on = c2->conv; on; on=on->n_list) {
		Pfct f = on->get_fct();
		rt = f->returns;
		Pptr rn = rt->is_ref();
		if(rn) rt = rn->typ;
		if(rt->is_cl_obj()) continue;
		if(t2->tconst() && !f->f_const) {
			not_const2 = 1;
			continue;
		}
		if(rt->base==OVERLOAD)
		    rt = Pgen(rt)->fct_list->f->tp;
		int r2 = rt->kind(oper,0,0);

		if( lt ) {
			if(lt->base==OVERLOAD)
			    lt = Pgen(lt)->fct_list->f->tp;
			int r1 = lt->kind(oper,0,0);

			if (np_promote(oper,r1,r2,lt,rt,1,0)!=any_type) {
				rconv.reserve(rfound+1);
				rconv[rfound++] = on;
			}
		}
		else {
			if( oper_okay(rt,oper)) {
				rconv.reserve(rfound+1);
				rconv[rfound++] = on;
			}
		}
	}
	if(rfound==0 && not_const2) return -2;
	if(rfound==1) Rcoerce = rconv[0];
	if(rfound>1) {
		Bits b = best_conv(rconv,rfound,const_obj2);
		Rcoerce = rconv[b.signif() - 1];
	}
	if(rfound>1) return 3;

	return (Lcoerce || Rcoerce);
}

void opov_error(Ptype t1, Ptype t2, TOK op)
{
	if (t1 && t2) {
		error('e',"ambiguous call of operator%k:%t%k%t",op,t1,op,t2);
	}
	else {
		Ptype tmp = t1 ? t1 : t2;
		error('e',"ambiguous call of operator%k:%k%t",op,op,tmp);
	}
	error('c'," (conflicts with built-in operator%k)\n",op);
}

int non_const;

static int id_match(Pexpr th, Pfct f, Pclass cl, int anac)
{
	Pname n = f->argtype;
	int ok = 0;

	if (!anac && th->e1 && ((cl && n && !n->n_list) || (!cl && n && n->n_list)))
		ok = 1;

	if (anac && th->e1 && ((cl && !n) || (!cl && n && !n->n_list)))
		ok = 1;

	if (th->e2 && ((cl && !n) || (!cl && n && !n->n_list)))
		ok = 1;

	if (ok && !cl) {
		Ptype t;
		if (th->e1)
			t = th->e1->tp;
		else if (th->e2->base==ELIST && !th->e2->tp)
			t = th->e2->e1->tp;
		else
			t = th->e2->tp;
		ok = can_coerce(n->tp,t);
	}

	return ok;
}

static Pexpr id_overload(Pexpr th, Pclass cl, int anac)
{
	char* on = oper_name(th->base);
	Pexpr e;

	if (cl)
		e = cl->find_name(on, 0);
	else
		e = gtbl->look(on, 0);

	if (!e || !e->tp)
		return 0;

	if (e->tp->base == FCT) {
		if (id_match(th, Pfct(e->tp), cl, anac))
			return e;
	}
	else {
		for (Plist gl = Pgen(e->tp)->fct_list; gl; gl = gl->l)
			if (id_match(th, Pfct(gl->f->tp), cl, anac))
				return gl->f;
	}

	return 0;
}

Pexpr expr::oper_overload(Ptable tbl)
{
	Pname n1 = 0;
	Ptype t1 = 0;
	const_obj1 = 0;
	const_obj2 = 0;
	int const_obj = 0;
	bit already_ambig = 0;

	if (e1) {
		t1 = e1->tp;
		Ptype tpx = t1->skiptypedefs();
		n1 = tpx->is_cl_obj();
		const_obj1 = t1->tconst() ? 1 : e1->is_const_obj();
	}

	TOK bb = base;
	switch (bb) {
	case DEREF:
		if (e2 == 0) bb = MUL;
		// no break;
	case CALL:
	case G_CALL:
		if (n1 == 0) return 0;	// ignore type of argument list
	}

	Pname n2 = 0;
	Ptype t2 = 0;

	if (e2 && e2->base!=ELIST) {
		t2 = e2->tp;
		Ptype tpx = t2->skiptypedefs();
		n2 = tpx->is_cl_obj();
		const_obj2 = t2->tconst() ? 1 : e2->is_const_obj();
	}

	if (n1==0 && n2==0) return 0;
	if (n1 && n1->tp == 0) return 0;	// make_assign() fudge

	Pname Ggn=0;
	Pexpr oe2 = e2;
	Pexpr ee2 = (e2 && e2->base!=ELIST) ? e2 = new expr(ELIST,e2,0) : 0;
	Pexpr ee1 = e1 ? new expr(ELIST,e1,e2) : ee2;
	char* obb = oper_name(bb);

	if (bb == INCR || bb == DECR) {
		Pclass cl = Pclass(e1 ? n1->tp : n2->tp);
		char* on = oper_name(bb);
		bit ismem = 1;
		Pexpr e;
		Pexpr ee;
		bit anac = 0;

		e = id_overload(this, cl, 0);
		ee = id_overload(this, 0, 0);

		if (e && ee)
			error("ambiguous call of operator%k: %a and %a", bb, e, ee);
		if (!e && !ee && e1 && !strict_opt) {
			e = id_overload(this, cl, 1);
			ee = id_overload(this, 0, 1);
			if (e || ee) {
				error('w', "prefix ++/-- used as postfix (anachronism)");
				anac = 1;
			}
		}
		if (!e)
			ismem = 0, e = ee;
		if (!e) {
			if (try_to_demote(bb, t1 ? t1 : t2, 0) == 1) {
				Pname xx = new name(Lcoerce->string);
				Pref r = new ref(DOT, e1 ? e1 : oe2, xx);
				if (e1)
					e1 = new expr(G_CALL, r, 0);
				else
					e2 = new expr(G_CALL, r, 0);
				return typ(tbl);
			}
			else {
				error("bad operand for%k:%t", bb, t1 ? t1 : t2);
				return this;
			}
		}
		base = (ismem ? CALL : G_CALL);
		if (!ismem) {
			Pexpr eee = 0;
			if (e1 && !anac)
				eee = new expr(ELIST, zero, 0);
			eee = new expr(ELIST, e1 ? e1 : oe2, eee);
			e1 = new name(on);
			Pname(e1)->n_qualifier = sta_name;
			e2 = eee;
		}
		else {
			Pexpr eee = e1;
			e1 = new ref(DOT, e1 ? e1 : oe2, new name(on));
			e2 = (eee && !anac ? new expr(ELIST, zero, 0) : 0);;
		}
		e = typ(tbl);
		if (!e)
			return e;
#if 0
		if (e->base == DEREF && e->e1->tp->is_ref()) {
			e = e->e1;
			e->tp = 0;
			e = e->typ(tbl);
		}
#endif
		return e;
	}

	// first try for non-member function
	Pname gname = tbl->look(obb,0);
	int go;
	if(!gname) go = 0;
	else if(gname->tp->base == FCT) go = matchable(gname,ee1,0);
	else {
		suppress_error++;
		Ggn = gname;
		Pgen g = Pgen(gname->tp);
		gname = ee1->e2 ? g->multArgMatch(ee1,0)
			: g->oneArgMatch(ee1,0);
		if(ambig) already_ambig = 1;
		go = gname ? matchable(gname,ee1,0) : 0;
		suppress_error--;
	}

	non_const = 0;

	if (n1) {
		if (bb == ASSIGN) {
			Pclass c1 = Pclass(n1->tp);
			if (c1->memtbl->look(obb,0)==0) {
				Pclass bcl = c1->baselist?c1->baselist->bclass:0;
				// if legal, a=1 can be optimized to a.ctor(1)
				if ( n2==0 
				     || 
				     !same_class(Pclass(n2->tp),c1)
				     &&
				     Pclass(n2->tp)->has_base(c1)==0
				) {
					//take out next line when no non-
					// member op=
					if (c1->c_xref==C_REFM) c1->c_xref=0;
					if (go > STD) goto glob;
					return 0;
				}

				// make operator=() if no base, different 
				// (smaller) sized base, or two bases
				if (
				    bcl && c1->obj_size!=bcl->obj_size
				    && bcl->memtbl->look(obb,0)
				    ||
				    c1->c_xref&(C_VBASE|C_VPTR|C_ASS|C_REFM)
				) {
					//take out next line when no non-
					// member op=
					if (go > STD) goto glob;
					return make_assignment(n1) ? 
						oper_overload(tbl) : 0;
				}
				return 0;
			}
			// now take care of other assignments, 
		}

		Pclass ccl = Pclass(n1->tp);
  		if(strcmp(obb,"__as")) {
			tcl = ccl; // ugh!!! 
		}
		Pexpr mn = ccl->find_name(obb,0);

		Pname mname = Pname(mn);
		if (mname == 0) goto glob;

		while (mname->base == REF || mname->base == MDOT) {
			mname = Pname(Pexpr(mname)->mem);
		}

		int mo = 0;
		if (const_obj1) const_obj = 1;
		if(mname->tp->base == FCT) 
			mo = matchable(mname,e2,const_obj1);
		else if(mname->tp->base == OVERLOAD) {
			suppress_error++;
			Pgen g = Pgen(mname->tp);
			if(!e2) {
				mname = g->exactMatch(0,const_obj1);
			}
			else if(e2->e2) {
			        mname = g->multArgMatch(e2,const_obj1);
			}
			else {
				mname = g->oneArgMatch(e2,const_obj1);
			}
			suppress_error--;
			if(ambig) already_ambig += 2;
			mo = mname ? matchable(mname,e2,const_obj1) : 0;
		}

		if (mo==0) goto glob;
		if (
		     (mo != EXACT || Pclass(n1->tp)->memtbl->look(obb,0)==0) 
		     && 
		     go != EXACT
		) {
		    if(
			Ggn && Pgen(Ggn->tp)->has_templ()
			||
			gname && gname->is_template_fct()
		    ) {
			Pname f_inst=has_templ_instance(Ggn?Ggn:gname,ee1,1);
			if( f_inst ) {
				gname = f_inst;
				go = EXACT;
				goto glob;
			}
		    }
		}

		if (mo && go) {
			Pfct mfct = mname->fct_type();
			Ptype mt1 = mfct->f_this->tp;
			mt1 = Pptr(mt1)->typ;
			Ptype mt2 = mfct->argtype->tp;
			Pname mm = new name(gname->string);
			mm->n_oper = gname->n_oper;
			Pname a1 = new name;
			a1->tp = mt1;
			Pname a2 = new name;
			a2->tp = mt2;
			a1->n_list = a2;
			mm->tp = new fct(mfct->returns,a1,2);

			Pname aa = gname->fct_type()->argtype;
			Ptype savep = 0;
			Pptr p;
			if (p = aa->tp->is_ref()) {
				savep = p;
				aa->tp = p->typ;
			}
			
			Pgen tgen = new gen;
			name_list nl(gname,0);
			tgen->fct_list = new name_list(mm,&nl);
			Pname found = tgen->multArgMatch(ee1,0);
			if(ambig) already_ambig = 1;

			if(savep) aa->tp = p;
			delete a1,a2;
			DEL( mm->tp );

			delete tgen;
			if(found!=mm) {		// non-member best
				delete mm;
				if(already_ambig==2) already_ambig=0;
				goto glob;
			}

			// member better or as good
			if(!ambig && already_ambig==1) already_ambig=0;
			delete mm;
		}

		if (
		    (mo == EXACT && Pclass(n1->tp)->memtbl->look(obb,0))
		    ||
		    try_to_demote(bb,t1,t2)<=0
		    ||
		    already_ambig > 0
		    ||
		    compare_builtin(t1,t2,n1,n2,mname,mo)
		) {
		    base = G_CALL;			// e1.op(e2) or e1.op()
		    Pname xx = new name(mname->string);	// do another lookup
							// . suppresses virtual
		    e1 = new ref(DOT,e1,xx);
		    if (ee1) delete ee1;
		    return typ(tbl);
		}
	}
	
	if (n2 && e1==0) {			/* look for unary operator */
		if (const_obj2) const_obj = 1;
		suppress_error++;
		Pexpr mn = Pclass(n2->tp)->find_name(obb,0);
		suppress_error--;
		Pname mname = Pname(mn);
		if (mname == 0) goto glob;

		while (mname->base==REF || mname->base==MDOT) {
			mname = Pname(Pexpr(mname)->mem);
		}	

		if (mname->n_scope != 0 && mname->n_scope != PUBLIC) {
			goto glob;
		}
		
		int mo = 0;
		if(mname->tp->base == FCT) 
			mo = matchable(mname,0,const_obj2);
		else if(mname->tp->base == OVERLOAD) {
			suppress_error++;
			Pgen g = Pgen(mname->tp);
			mname = g->exactMatch(0,const_obj2);
			if(ambig) already_ambig += 2;
			suppress_error--;
			mo = mname ? matchable(mname,0,const_obj2) : 0;
		}

		if (mo==0) {
			goto glob;
		}
		if (
		     (mo != EXACT || Pclass(n2->tp)->memtbl->look(obb,0)==0) 
		     && 
		     go != EXACT
		) {
		    if(
			Ggn && Pgen(Ggn->tp)->has_templ()
			||
			gname && gname->is_template_fct()
		    ) {
			Pname f_inst=has_templ_instance(Ggn?Ggn:gname,ee1,1);
			if( f_inst ) {
				gname = f_inst;
				go = EXACT;
				goto glob;
			}
		    }
		}
		if(mo && go) {
			Ptype mt1 = mname->fct_type()->f_this->tp;
			mt1 = Pptr(mt1)->typ;
			Pname marg = new name;
			marg->tp = mt1;
			Pname garg = gname->fct_type()->argtype;
			Pname nn = bestOfPair(marg,garg,e2->e1->tp);

			if(nn==garg) {
				if(already_ambig==2) already_ambig=0;
				delete marg; 
				goto glob;
			}
			else if(nn==marg) {
				if(already_ambig == 1) already_ambig=0;
			}
			// member better or as good
			else if(!nn) {
				Block(Pname) errblock(3);
				errblock[0] = mname;
				errblock[1] = gname;
				fmError(1,errblock,e2,const_obj2);
				already_ambig = 1;
			}
			delete marg;
		}
		base = G_CALL;				// e2.op()
		Pname xx = new name(mname->string);	// do another lookup
							// . suppresses virtual
		e1 = new ref(DOT,oe2,xx);
		e2 = 0;
		if (ee2) delete ee2;
		if (ee1 && ee1!=ee2) delete ee1;
		return typ(tbl);
	}
	
glob:
	if (go != EXACT) {
		if(
			Ggn && Pgen(Ggn->tp)->has_templ()
			||
			gname && gname->is_template_fct()
		) {
			Pname f_inst = has_templ_instance(Ggn?Ggn:gname,ee1,1);
			if( f_inst ) {
				gname = f_inst;
				go = EXACT;
			}
		}
	}
	if (go) {

		if (
		    go == EXACT			    // UDO is an exact match
		    ||
		    try_to_demote(bb,t1,t2)<=0	    // can use a built_in
		    ||
		    already_ambig > 0		    // already ambiguous
		    ||
		    compare_builtin(t1,t2,n1,n2,gname,go) // builtin not better
		) {
			base = gname->n_table == gtbl ? G_CALL : CALL;
			e1 = new name(gname->string);
			// if global scope, look only for globals
			if(gname->n_table == gtbl) 
				Pname(e1)->n_qualifier = sta_name;
			e2 = ee1;
			return typ(tbl);
		}
	}

	if (ee2) delete ee2;
	if (ee1 && ee1!=ee2) delete ee1;
	e2 = oe2;

	switch (bb) {
	case CM:
	case G_CM:
	case G_ADDROF:
		return 0;
	case ASSIGN:
		if( n1 && n2
		    && 
		    (
			n1->tp==n2->tp
			||
			Pclass(n2->tp)->has_base(Pclass(n1->tp))
		    )
		) {
			if (!const_obj1 && make_assignment(n1))
				return oper_overload(tbl);
			else
				return 0;
		}
	case DEREF:
	case CALL:
		if (n1 == 0) break;

	default:	/* look for conversions to basic types */
		if( n1
		    && 
		    Pclass(n1->tp)->conv
		    && 
		    (bb==ANDAND || bb==OROR)
		) {
			e1 = check_cond(e1,bb,tbl);
			return 0;
		}

		if( n2 && Pclass(n2->tp)->conv
		    && 
		    (
			bb==ANDAND || bb==OROR || bb==NOT || 
			bb==UMINUS || bb==UPLUS || bb==COMPL
		    )
		) {
			Pexpr te = check_cond(e2,bb,tbl);
			if (te == e2) {
				tp = any_type;
				return this;
			}
			e2 = te;
			return 0;
		}

		int ttd;
		switch ((ttd=try_to_demote(bb,t1,t2))) {
		case -2:
			error("no usable const conversion for%n",n2);
			break;
		case -1:
			error("no usable const conversion for%n",n1);
			break;
		case 0:
			break;
		case 2:
			error("ambiguous conversion of%n",n1);
			break;
		case 3:
			error("ambiguous conversion of%n",n2);
			break;
		case 4:
			error("ambiguous conversion of%n and%n",n1,n2);
			break;
		case 1:
			if ( Lcoerce ) {
				Pname xx = new name(Lcoerce->string);
				Pref r = new ref(DOT,e1,xx);
				e1 = new expr(G_CALL,r,0);
			}

			if (Rcoerce) {
				Pname xx = new name(Rcoerce->string);
				Pref r = new ref(DOT,e2,xx);
				e2 = new expr(G_CALL,r,0);
			}
			return typ(tbl);
		}

		switch (bb) {
		case CM:
		case ADDROF:	// has legal built-in meaning
			return 0;
		}

		if (t1 && t2)
			error('e',"bad operands for%k:%t%k%t",bb,t1,bb,t2);
		else
			error('e',"bad operand for%k:%t",bb,t1?t1:t2);

		if (const_obj && ttd <= 0 && non_const) 
			 error('c'," (no usable const operator%k)\n",bb);
		else error('c',"\n");

		tp = any_type;
		return this;
	}

	return 0;
}

Pexpr cast_cptr(Pclass ccl, Pexpr ee, Ptable tbl, int real_cast)
/*
	"ee" is being cast to pointer object of class "ccl"
	if necessary modify "ee"
*/
{

	Ptype etp = ee->tp->is_ptr_or_ref();
	if (etp == 0) return ee;

	Pname on = Pptr(etp)->typ->is_cl_obj();
	if (on == 0) return ee;

	Pclass ocl = Pclass(on->tp);
	if (ccl==0 || ocl==0 || same_class(ocl,ccl)) return ee;

// error('d',"cast_cptr %t(%t) real %d",ccl,ocl,real_cast);
	int oo = 0;

	if (ocl->baselist
	&& (!same_class(ocl->baselist->bclass,ccl) || ocl->baselist->base!=NAME)) {
		// casting derived to second or virtual base?
		Nptr = 0;
		Nvis = 0; 
		Nalloc_base = 0;
		vcllist->clear();
		vcllist=0;
		int x = is_unique_base(ocl,ccl->string,0);
		if (Nvis) {
                	if (real_cast==0)
                        	error("cast:%n* ->B%t*;%kBC",on,ccl,Nvis);
                        else if (warning_opt)
                                error('w',"cast:%n* ->B%t*;%kBC",on,ccl,Nvis);
                        real_cast = 1;  // suppress further error mesages
                        Nvis = 0;

		}

		switch (x) {
		default:
			error("cast:%n* ->B%t*;%t isB more than once",on,ccl,ccl);
		case 0:		// unrelated;
			break;
		case 1:
			oo = Noffset;
			break;
		}

		if (Nptr) {	// => ee?Nptr:0
                        if (ocl->c_body==1) ocl->dcl_print(0);
                        Nptr->mem = ee; // ee->Pbase_class
			if ( Nalloc_base ) {
// error('d', "cast_cptr: nalloc_base: %s", Nalloc_base);
				Nptr->i1 = 5;
				Nptr->string4 = new char[strlen(Nalloc_base)+1];
				strcpy(Nptr->string4,Nalloc_base);
				Nalloc_base = 0;
			}
			else Nptr->i1 = 3;

			extern Pexpr this_handler;
                        if (ee->base==ADDROF || ee->base==G_ADDROF)   
                                	ee = Nptr;
                        else if (this_handler && ee->base == NAME &&
			    	strcmp(ee->string,"this") == 0)
                                	ee = this_handler;
                        else {
				Pexpr p = new expr(QUEST,Nptr,zero);
				nin = 1;
				if (ee->not_simple()) {	// need temp
					Ptype t = unconst_type(ee->tp);
					Pname pp = make_tmp('N',t,tbl);
					Pname(pp)->n_assigned_to = 1;
					ee = new expr(ASSIGN,pp,ee);
					ee->tp = t;
					Nptr->mem = pp;
				}
				nin = 0;
				p->cond = ee;
				p->tp = ee->tp;
                                ee = p;
			}
		}			
	}

	if (ccl->baselist
	&& (!same_class(ccl->baselist->bclass,ocl) || ccl->baselist->base!=NAME)) {
		// casting second or virtual base to derived?
		Nptr = 0;
		vcllist->clear();
		vcllist=0;
		int x = is_unique_base(ccl,ocl->string,0);
		switch (x) {
		default:
			error("cast:%n* ->derived%t*;%n isB more than once",on,ccl,on);
		case 0:		// unrelated;
			break;
		case 1:
			oo = -Noffset;
                        if (Nptr)
                                error("cast:%n* ->derived%t*;%n is virtualB",on,ccl,on);
                        break;
		}
		Nvis = 0;	// visibility no concern when converting from base to derived
	}
// error('d',"oo %d ee %k",oo,ee->base);
	if (oo) {	// => ee?ee+offset:0
                if (ee->base==ADDROF || ee->base==G_ADDROF || (ee->base==NAME && ee->tp->base==RPTR))
                        ee = rptr(ee->tp,ee,oo);
                else {
			Pexpr p;
			nin = 1;
			if (ee->not_simple()) {	// need temp
				Ptype t = ee->base==MDOT?ee->mem->tp:ee->tp;
				Pname pp = make_tmp('M',t,tbl);
				if (pp->tp->base==VEC)
					pp->tp = new ptr(PTR,Pvec(pp->tp)->typ);
				Pname(pp)->n_assigned_to = 1;
				ee = new expr(ASSIGN,pp,ee);
				ee->tp = t;
				p = rptr(t,pp,oo);
			}
			else
				p = rptr(ee->base==MDOT?ee->mem->tp:ee->tp,ee,oo);
			nin = 0;
			Pexpr pp = new expr(QUEST,p,zero);
			pp->tp = ee->tp;
			pp->cond = ee;
                        ee = pp;
		}
	}

	Nvis = 0; // Nvis set by has_base()
	if (ocl->has_base(ccl) && Nvis) {
		if (real_cast==0)
			error("cast:%n* ->B%t*;%kBC",on,ccl,Nvis);
		//else // why only warn? if (warning_opt)
                        //error('w',"cast:%n* ->B%t*;%kBC",on,ccl,Nvis);
                        //error("cast:%n* ->B%t*;%kBC",on,ccl,Nvis);
		Nvis = 0;
	}

// error('d',"return %d %k %t",ee,ee->base,ee->tp);
	return ee;
}

Pexpr expr::donew(Ptable tbl)
{
	Ptype tt = tp2;
	Ptype tpx = tt;
	bit v = 0;
	bit old = new_type;
	int init = 0;	// non-constructor initialization
	new_type = 1;

	tt->dcl(tbl);
	new_type = old;
// error('d',"donew %d %d (%k) tt %t",e1,e2,e2?e2->base:0,tt);
	if (e1) e1 = e1->typ(tbl);
	if (e2) e2 = e2->typ(tbl);
ll:
//error('d',"ll %d",tt->base);
	switch (tt->base) {
	default:
		if ( e1) {
			if (v) {
				error("Ir for array created using \"new\"");
				break;
			}
			init = 1;
		} 
	//	if (e1) {
	//		error("Ir for nonCO created using \"new\"");
	//		e1 = 0;
	//	}
		break;
	case VEC:
		if (v && Pvec(tt)->dim) error("only 1st array dimension can be non-constant");
		if (Pvec(tt)->size==0 && Pvec(tt)->dim==0) error("array dimension missing in `new'");
	//	if (Pvec(tt)->dim==zero) {
	//		Pvec(tt)->size = 0;
	//		Pvec(tt)->dim = 0;
	//	}
		v++;
		tt = Pvec(tt)->typ;
		goto ll;
	case TYPE:
		tt = Pbase(tt)->b_name->tp;
		goto ll;
	case VOID:
		error("badT for `new': void");
		break;
	case COBJ:
	{	Pname cn = Pbase(tt)->b_name;
		Pclass cl = Pclass(cn->tp);
		Pname icn = 0;

		if ( e1 ) { // arguments
			if ( e1->e2 == 0 && e1->base == ELIST ) {
				e1 = e1->e1;
				e1 = e1->typ(tbl);
			}
			icn = (e1->base!=ELIST)?e1->tp->is_cl_obj():0;
		}
		//Pname icn = (e1 && e1->base!=ELIST)?e1->tp->is_cl_obj() : 0;

		Pclass icl = icn ? Pclass(icn->tp) : 0;

		if (cl->c_abstract) {
			error("`new' of abstractC%t",cl);
			error('C',"%a is a pure virtualF ofC%t\n",cl->c_abstract,cl);
			break;
		}

		if (v && e1) {
			error("Ir for array ofCO created using \"new\"");
			break;
		}

		if ((cl->defined&(DEFINED|SIMPLIFIED)) == 0) {
			error("new%n;%n isU",cn,cn);
			break;
		}

		Pname ctor = cl->has_ctor();

		if (ctor) {
			if (v) {
				Pname ic;
				if ((ic = cl->has_ictor())==0) {
					error("array ofC%n that does not have aK taking noAs",cn);
					break;
				}
					
				if (Pfct(ic->tp)->nargs) {
					if (!cl->has_vtor()) cl->make_vec_ctor(ic);
					break;
				}
			}

			if (icl
			&& cl->has_itor()==0	// incomplete:
						// what if X(Y&) exists
						// for class Y : X ?
			&& (same_class(icl,cl) || icl->has_base(cl))) {
				init = 1;
				break;
			}
			// If link compatability is broken change the
			// way ctors are called.  Create an expression
			// that will create space for the object then
			// call the ctor.  This way you can remove the
			// check of this==0 from ctors.
                        if (perf_opt && !v) {
                                // First check that this is not a case
                                // where the NEW is a default argument.
                                // The rest of cfront cannot handle this
                                // so kick out the case right now.
				// Set in dargs (dcl3.c) function
				// This is the only way to tell if
				// a new is in the arg list

				extern int New_in_arg_list;
                                if (New_in_arg_list)
                                        error('s',"optimization option does not allow new as a default argument\n");
                                // create space for object then call
                                // constructor.
                                Pexpr p;
				Pexpr args = e2;
                                Pexpr sz = new texpr(SIZEOF,tt,0);
                                (void) tt->tsizeof();
                                sz->tp = uint_type;
                                Pexpr ee = new expr(ELIST,sz,args);
                                char *s = oper_name(NEW);
                                Pname n = new name(s);
                                if (base == GNEW)
                                        p = gtbl->look(s,0);
                                else
                                        p = find_name(n,cl,cl->memtbl,CALL,cc->nof);
                                ee = new call(p,ee);
				overFound = 0; //set in call_fct
                                (void) ee->call_fct(cl->memtbl);
				if (overFound && overFound->n_scope != EXTERN
            			&& overFound->n_scope != STATIC) 
                			check_visibility(overFound,0,Pfct(overFound->tp)->memof,cc->ftbl,cc->nof);
        			overFound=0;
                                Ptype cobj_ptr = new ptr(PTR, tt);
				Pname cobj_tmp;
                                Pname tn = tbl->t_name;
				// Create temp at global scope, for sti
				// because make_tmp doesn't expect to
				// see a temp in an sti
                                if (tn && tn->tp) {
                                	cobj_tmp = make_tmp('B', cobj_ptr, gtbl);
				}
				else
                                	cobj_tmp = make_tmp('B', cobj_ptr, tbl);
				cobj_tmp->n_assigned_to = 1;
				cobj_tmp->n_used = 1;
                                Pexpr ctor_expr = call_ctor(tbl,cobj_tmp,ctor,e1);
                                ee = new expr(ASSIGN,cobj_tmp,ee);
                                ee = new expr(ANDAND,ee,ctor_expr);
				ee = new expr(G_CM,ee,cobj_tmp);
                                e1 = ee;
                                e1->tp = cobj_ptr;
				return e1;
                        }
                        else {
				e1 = call_ctor(tbl,0,ctor,e1);
			}
		}
		else if (e1) {
			if (same_class(icl,cl) || icl->has_base(cl)) 
				init = 1;
			else
				error("new%n(As );%n does not have aK",cn,cn);
		}
	}
	}

	if (init) {
		Pname tmp = make_tmp('N',tt->addrof(),tbl);
		e1 = e1->typ(tbl);
		if (tt->check(e1->tp,ASSIGN))
			error("badIrT%t for new operator (%t X)",e1->tp,tt);
		if (e1->base == ILIST ||
		    (e1->base == ELIST && e1->e1 && e1->e1->base == ILIST)) {
			if (e1->base != ILIST)
				e1 = e1->e1;
			Pname tmp2 = make_tmp('A', mptr_type, tbl);
			Ptype t = e1->tp;
			e1 = mptr_assign(tmp2, e1);
			e1 = new expr(G_CM, e1, tmp2);
			e1->tp = t;
		}
		e1 = new expr(0,tmp,e1);
		tmp->assign();
		if (ansi_opt && tmp->tp) {
			Ptype t = tmp->tp;
			if (t->is_ptr_or_ref())
				t = Pptr(t)->typ;
			t->ansi_const = 1;
		}
	}

//	tp = (v) ? tpx : tpx->addrof();
	switch (v) {
		case 0:
			tp = tpx->addrof();
			break;
		case 1:
			tp = tpx;
			break;
		default:
			tp = tpx;
	}
        return this;
}

static int is_dataMemPtr( Pexpr ee ) 
/* this is utterly implementation dependent 
 * called by expr::lval to determine 
 * const objects bounds to pointers to data members 
 */
{
	Pexpr te = ee->e1;
	if ( te == 0 ) return 0;
	if ( te->base != PLUS ) return 0;
	if ( (te = te->e2) == 0 ) return 0;
	if ( te->base != MINUS ) return 0;
	if ( (te = te->e1) == 0 ) return 0;
	if ( te->base != CAST && te->base != G_CAST) return 0;
	if ( (te = te->e1) == 0 ) return 0;
	if ( te->tp->base != PTR ) return 0;
        if ( Pptr(te->tp)->memof == 0 ) return 0;
	return 1;
}

inline bit
exact123(Pname nn, Ptype tt)
{
	if ( exact1(nn,tt) || exact2(nn,tt) || exact3(nn,tt))
		return 1;
	else return 0;
}

Pname
compare_builtin(Ptype t1, Ptype t2, Pname n1, Pname n2, Pname fname, int fo)
/* 
    routine to compare user-defined operator to built-in 
    version. returns fname if user-defined operator is 
    preferred, and 0 otherwise.
*/
{
	if (fo == EXACT) return fname;
	bit binary = t1 && t2 ? 1 : 0;
	Pfct ff = fname->fct_type();
	TOK oper = fname->n_oper;

	// operators which must be members are
	// always preferred over built-in
	// also ASOPs since can't assign to GCALL
	if(
	    oper==DEREF || oper==CALL
	    ||
	    oper==ASSIGN || oper==REF
	    ||
	    oper==ASPLUS || oper==ASMINUS
	    || 
	    oper==ASMUL || oper==ASDIV
	    || 
	    oper==ASMOD || oper==ASAND
	    || 
	    oper==ASOR ||  oper==ASER
	    || 
	    oper==ASLS || oper==ASRS
	) 
		return fname;

	int t1eobj = (t1 && t1->skiptypedefs()->base == EOBJ);
	if (t1eobj) {
		Pname n1 = t2 ? t2->is_cl_obj() : 0;
		Pclass c1 = n1 ? Pclass(n1->tp) : 0;
		if (c1) {
			for (Pname on1 = c1->conv; on1; on1 = on1->n_list) {
				Pfct f = on1->get_fct();
				Ptype ret = f ? f->returns : 0;
				if (ret && ret->skiptypedefs()->base == EOBJ &&
				    !ret->check(t1->skiptypedefs(), ASSIGN)) {
					t1eobj = 0;
					break;
				}
			}
		}
		else {
			t1eobj = 0;
		}
	}
	int t2eobj = (t2 && t2->skiptypedefs()->base == EOBJ);
	if (t2eobj) {
		Pname n1 = t1 ? t1->is_cl_obj() : 0;
		Pclass c1 = n1 ? Pclass(n1->tp) : 0;
		if (c1) {
			for (Pname on1 = c1->conv; on1; on1 = on1->n_list) {
				Pfct f = on1->get_fct();
				Ptype ret = f ? f->returns : 0;
				if (ret && ret->skiptypedefs()->base == EOBJ &&
				    !ret->check(t2->skiptypedefs(), ASSIGN)) {
					t2eobj = 0;
					break;
				}
			}
		}
		else {
			t2eobj = 0;
		}
	}
	int teobj = t1eobj || t2eobj;

	if (ff->memof) {
		// unary member always better
		if ( !binary || n2) return fname;

		Pname nn = ff->argtype;

		if ((!teobj && exact1(nn,t2)) || (teobj && exact2(nn,t2)))
			return fname;

		opov_error(t1,t2,oper);
		return fname;
	}

	// non-member
	if (!binary) {			// unary
		if (fo==UDC) {
			opov_error(t1,t2,oper);
			return fname;
		}
	}
	else {				// binary
		if (n1 && n2) {
			if (fo < UDC) return fname;
			Pname nn1 = ff->argtype;
			Pname nn2 = nn1->n_list;
			if ( exact123(nn1,t1) || exact123(nn2,t2)) {
				return fname; 
			}
			else {
				opov_error(t1,t2,oper);
				return fname; 
			}
		}
		else {
			Pname carg, oarg;
			Ptype ct,ot;
			if (n1) {
				carg = ff->argtype;
				ct = t1;
				oarg = carg->n_list;
				ot = t2;
			}
			else {
				oarg = ff->argtype;
				ot = t1;
				carg = oarg->n_list;
				ct = t2;
			}

			if ( exact123(carg,ct)) {
				if ((!teobj && !exact1(oarg,ot)) ||
				   (teobj && !exact2(oarg,ot))) 
					opov_error(t1,t2,oper);
				return fname;
			}
			else {
				if ((!teobj && exact1(oarg,ot)) ||
				    (teobj && exact2(oarg,ot))) {
					opov_error(t1,t2,oper);
					return fname;
				}
				else return 0;
			}
		}
	}

	error('i', "fall off end of compare_builtin()");
	return 0;
}
