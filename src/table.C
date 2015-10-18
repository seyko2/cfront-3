/*ident	"@(#)cls4:src/table.c	1.3" */
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

#include "cfront.h"
#include "size.h"

#ifdef DBG
extern long node_id;
#define DBCHECK() if(node::allocated) error('i',"allocated node (id %d, base%k) on free list! (src: \"%s\", %d",node::id,node::base,__FILE__,__LINE__);
#else
#define DBCHECK() /**/
#endif

table::table(short sz, Ptable nx, Pname n)
/*
	create a symbol table with "size" entries
	the scope of table is enclosed in the scope of "nx"

	both the vector of class name pointers and the hash table
	are initialized containing all zeroes
	
	to simplify hashed lookup entries[0] is never used
	so the size of "entries" must be "size+1" to hold "size" entries
*/
{
	DBCHECK();
	base = TABLE;
	t_name = n;
	size = sz = (sz<=0) ? 2 : sz+1;
//fprintf(stderr,"table::table %d %s %d (%d %d)\n", this, (n)?n->string:"?", sz,(sz*3)/2);
	entries = new Pname[sz];
	hashsize = sz = (sz*3)/2;
	hashtbl = new short[sz];
	next = nx;
	free_slot = 1;
	DBID();
}

table::~table()
{
	delete entries;
	delete hashtbl;
}

Pname table::look(char* s, TOK k)
/*
	look for "s" in table, ignore entries which are not of "k" type
	look and insert MUST be the same lookup algorithm
*/
{
	Ptable t;
	register char * p;
	register char * q;
	register int i;
	Pname n;
	int rr;

	DB( if(Bdebug>=2 && (Bdebarg==0 || strcmp(Bdebarg,s)==0)) {
		error('d',"%s %d->table::look( s %s, k%k )",t_realbase==KTABLE?"KTABLE":"",this,s,k);
	});

//	if (s == 0) error('i',"%d->look(0)",this);
//	if (this == 0) error('i',"0->look(%s)",s);
//	if (base != TABLE) error('i',"(%d,%d)->look(%s)",this,base,s);

	/* use simple hashing with linear search for overflow */

	p = s;
	i = 0;
	while (*p) i += (i + *p++); /* i<<1 ^ *p++ better?*/
	rr = (0<=i) ? i : -i;

	for (t=this; t; t=t->next) {	
		/* in this and all enclosing scopes look for name "s" */
		Pname* np = t->entries;
		int mx = t->hashsize;
		short* hash = t->hashtbl;
		int firsti = i = rr%mx;

		do {			
			if (hash[i] == 0) goto not_found;
			n = np[hash[i]];
			if (n == 0) error('i',"hashed lookup");
			p = n->string;		/* strcmp(n->n_string,s) */
			q = s;
			while (*p && *q)
				if (*p++ != *q++) goto nxt;
			if (*p == *q) goto found;
		nxt:
			if (mx <= ++i) i = 0;		/* wrap around */
		} while (i != firsti);

	found:
		for (; n; n=n->n_tbl_list){	/* for  all name "s"s look for a key match */
			DB(if(Bdebug>=2 && (Bdebarg==0||strcmp(Bdebarg,s)==0)){
				error('d',"    found%n%t n_key%k t %d",n,n->tp,n->n_key,t);
			});
			if (n->n_key == k) return n;
		}

	not_found:;
	}

	DB( if(Bdebug>=2 && (Bdebarg==0 || strcmp(Bdebarg,s)==0)) {
		error('d',"%s k%k not found",s,k);
	});
	return 0;	/* not found && no enclosing scope */
}

bit Nold;	/* non-zero if last insert() failed */

Pname table::insert(Pname nx, TOK k)
/*
	the lookup algorithm MUST be the same as look
	if nx is found return the older entry otherwise a copy of nx;
	Nold = (nx found) ? 1 : 0;
*/
{
	register char * p;
	register int i;
	Pname n;
	Pname* np = entries;
	Pname* link;
	int firsti;
	int mx = hashsize;
	short* hash = hashtbl;
	char* s = nx->string;

	DB( if(Bdebug>=1 && (Bdebarg==0 || strcmp(Bdebarg,nx->string)==0)) {
		error('d',"%d->table::insert( nx%n%t, k%k )",this,nx,nx?nx->tp:0,k);
		error('d',"   %s t_name %s size %d free_slot %d",t_realbase==KTABLE?"KTABLE":"",this==gtbl?"global table":t_name?t_name->string:"?",size,free_slot);
		if ( Bdebug>=2 ) display_expr(nx,"tbl::insert()");
	});

	if (s==0) error('i',"%p->insert(0,%k)",this,k);
	nx->n_key = k;
	if (nx->n_tbl_list || nx->n_table) error('i',"%n in two tables (n_tbl_list %d, n_table %d%n)",nx,nx->n_tbl_list,nx->n_table,nx->n_table?nx->n_table->t_name:0);
	/* use simple hashing with linear search for overflow */

	p = s;
	i = 0;
	while (*p) i += (i + *p++);
	if (i<0) i = -i;
	firsti = i = i%mx;

	do {	/* look for name "s" */
		if (hash[i] == 0) {
			hash[i] = free_slot;
			goto add_np;
		}
		n = np[hash[i]];
		if (n == 0) error('i',"hashed lookup");
		if (strcmp(n->string,s) == 0) goto found;
/*
		p = n->string;
		q = s;
		while (*p && *q) if (*p++ != *q++) goto nxt;
		if (*p == *q) goto found;
	nxt:
*/
		if (mx <= ++i) i = 0;	/* wrap around */
	} while (i != firsti);

	error("N table full");

found:	


	for(;;) {
		//SYM -- NESTED stuff removed
		if ( n->n_key==k) { Nold = 1; return n; }

		if (n->n_tbl_list)
			n = n->n_tbl_list;
		else {
			link = &(n->n_tbl_list);
			goto re_allocate;
		}
	}

add_np:
	if (size <= free_slot) {
		grow(2*size);
		return insert(nx,k);
	}

	link = &(np[free_slot++]);

re_allocate:
	{	
		Pname nw = new name;
		*nw = *nx;
		char* ps = new char[strlen(s)+1]; // copy string to safer store
		strcpy(ps,s);
//		Nstr++;
		nw->string = ps;
		nw->n_table = this;
		*link = nw;
		Nold = 0;
//		Nname++;
		return nw;
	}
}

void table::grow(int g)
{
	short* hash;
	register int j;
	int mx; 
	register Pname* np;
	Pname n;

	if (g <= free_slot) error('i',"table.grow(%d,%d)",g,free_slot);
	if (g <= size) return;
//error('d',"grow %d %s %d->%d", this, (t_name)?t_name->string:"?", size, g+1);
	size = mx = g+1;

	np = new Pname[mx];
	for (j=0; j<free_slot; j++) np[j] = entries[j];
	delete entries;
	entries = np;

	delete hashtbl;
	hashsize = mx = (g*3)/2;
	hash = hashtbl = new short[mx];

	for (j=1; j<free_slot; j++) {	/* rehash(np[j]); */
		char * s = np[j]->string;
		register char * p;
		char * q;
		register int i;
		int firsti;

		p = s;
		i = 0;
		while (*p) i += (i + *p++);
		if (i<0) i = -i;
		firsti = i = i%mx;

		do {	/* look for name "s" */
			if (hash[i] == 0) {
				hash[i] = j;
				goto add_np;
			}
			n = np[hash[i]];
			if (n == 0) error('i',"hashed lookup");
			p = n->string;	/* strcmp(n->n_string,s) */
			q = s;
			while (*p && *q) if (*p++ != *q++) goto nxt;
			if (*p == *q) goto found;
		nxt:
			if (mx <= ++i) i = 0;	/* wrap around */
		} while (i != firsti);

		error('i',"rehash??");

	found:
		error('i',"rehash failed");

	add_np:;
	}
}

void table::reinit() 
{ // reuse table for stmt::simpl
	for (int i = 1; i<free_slot; i++) entries[i] = 0;
	for (i=0; i<hashsize; i++) hashtbl[i] = 0;
	free_slot = 1;
}


char*
table::whatami()
{
	if ( this == 0 ) return "<null table>";
	if ( this == gtbl ) return "global";
	if ( t_name ) return t_name->string ? t_name->string : "???";
	return "block";
}
#ifdef DBG
void
table::dump( int v )
{
	if ( this == 0 ) {
		fprintf(stderr,"null table!!!");
		return;
	}
	fprintf(stderr,"table %d '%s'  free_slot %d  size %d  hashsz %d\n",this,t_name?t_name->string:"", free_slot, size, hashsize);
	for ( long i = 1;  i <= free_slot;  ++i )
		display_expr(entries[i],"",v==0);
}
#endif


//SYM ktable routines
#define STRCMP(a,b) ( *a==*b ? strcmp(a,b) : -1 )

ktable::ktable(int sz, ktable* nx, Pname n)
{
	DBCHECK(); base = KTABLE;
	if ( sz == 0 ) { k_tiny = 1; k_n = 0; k_size = 0; }
	else { k_tiny = 0; k_t = new table(sz,0,n); k_t->t_realbase = KTABLE; }
	k_next = nx; k_name = n;
	DBID();
}
ktable::~ktable()
{
    if ( k_tiny ) {
	Pname nx;
	for ( Pname n = k_n;  n;  n = nx ) {
		nx = n->n_tbl_list;
		DEL(n);
	}
	k_n = 0;
    } else delete k_t;
}
Pname ktable::look(char* s, TOK k)
{
    if ( k_tiny ) {
	for ( Pname n = k_n;  n;  n = n->n_tbl_list )
		if ( STRCMP(n->string,s) == 0 && n->n_key == k ) return n;
	return 0;
    } else return k_t->look(s,k);
}
Pname
ktable::find_cn( char* s )
{
	if ( k_id != CLASS ) error('i',"find_cn( %s) on nonC table",s);
	Pclass cl = k_name->tp->classtype();
	if ( cl->class_base == VANILLA ||
		// specialization of template: class X<int>, for example
		(cl->class_base == INSTANTIATED && cl->templ_base == CL_TEMPLATE))
			return k_name;
	// template or instantiation -- be sure to get the right copy
	Pname n = k_name->n_ktable->look(s,0);
	return n && n->base == NAME ? n->n_hidden : n;
}

Pname ktable::insert(Pname nn, TOK k)
{
    if ( nn->n_ktable ) error('i',"%n inserted twice",nn);
    if ( k_tiny ) {
	Nold = 0;
	if ( k_n == 0 ) {
	    k_n = nn;
	} else {
	    for ( Pname n = k_n;  n;  n = n->n_tbl_list ) {
		if ( STRCMP(n->string,nn->string) == 0 && n->n_key == k )
			{ Nold = 1; return n; }
		if ( n->n_tbl_list == 0 ) {
		    n->n_tbl_list = nn;
		    break;
		}
	    }
	}
	nn->n_key = k;
	nn->n_ktable = this;
	nn->n_tbl_list = 0;
	PERM(nn); //XXXXX
	return nn;
    } else {
	nn = k_t->insert(nn,k);
	if ( !Nold ) nn->n_ktable = this;
	return nn;
    }
}
// return a pointer to the i'th entry, or 0 if it does not exist
Pname ktable::get_mem(int i)
{
    if ( !k_tiny ) return k_t->get_mem(i);
    else {
	for ( Pname n = k_n;  n && i>1;  --i, n = n->n_tbl_list ) ;
	return n;
    }
}
void
ktable::expand( int sz )
{
	if ( !k_tiny ) return;
	k_tiny = 0;
	Pname names = k_n;
	k_t = new table(sz,0,k_name);
	for ( Pname nx, n = names;  n;  n = nx ) {
		nx = n->n_tbl_list;
		n->n_tbl_list = 0; n->n_table = 0;
		k_t->insert(n,n->n_key);
	}
}
void
ktable::hoist()
{
//XXXXX should be extended to handle any table
	if ( !k_tiny ) error('i',"hoisting wrong table");
	Pname names = k_n;
	k_n = 0;
	for ( Pname nx, n = names; n; n = nx ) {
		nx = n->n_tbl_list;
		n->n_tbl_list = 0; n->n_table = 0; n->n_ktable = 0;
		insert_name(n,k_next);
	}
}

//SYM -- void table_delete(char*, TOK, int ) removed
