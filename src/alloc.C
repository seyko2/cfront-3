/*ident	"@(#)cls4:src/alloc.c	1.4" */
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
#include "memory.h"
#include "template.h"

const int NBITE = (CHUNK-8)/sizeof(name)-1;
const int EBITE = (CHUNK-8)/sizeof(expr)-1;
const int SBITE = (CHUNK-8)/sizeof(stmt)-1;
const int TBITE = (CHUNK-8)/sizeof(table)-1;
const int KBITE = (CHUNK-8)/sizeof(ktable)-1;
const int VBITE = (CHUNK-8)/sizeof(vec)-1;
const int FBITE = (CHUNK-8)/sizeof(fct)-1;
const int PBITE = (CHUNK-8)/sizeof(ptr)-1;
const int PTBIT = (CHUNK-8)/sizeof(templ_fct)-1;

// define static _free members of classes
Pptr   ptr::ptr_free = 0;
Pfct   fct::fct_free = 0;
Pvec   vec::vec_free = 0;
Pktab  ktable::table_free = 0;
Ptable table::table_free = 0;
Pname  name::name_free = 0;
Pstmt  stmt::stmt_free = 0;
Pexpr  expr::expr_free = 0;
Ptfct  templ_fct::ptfct_free = 0;

#define  mzero(p,l)  memset(p,0,l)

void* chunk(int i)	// get memory that is not to be freed
{
	register char* cp = (char*)malloc(i*CHUNK-8);
	if (cp == 0) {			// no space
		free((char*)gtbl);	// get space for error message
		error('i',"free store exhausted");
	}
	return cp;
}

/*
 * plan 9 has size_t
 * #ifdef __HAVE_SIZE_T
 */
#include <new.h>
#define NEW_SIZE size_t
/*
 * plan 9 #else
 * #define NEW_SIZE unsigned 
 * #endif
 */

void* operator new(NEW_SIZE sz)	// get memory that might be freed
{

#ifdef _AIX
	if (sz == 0) sz = 4; // most stringent alignment criteria
#endif

	char* p = (char*)calloc((unsigned)sz,1);

//fprintf(stderr,"alloc(%d)->%d\n",sz,p);

	if (p == 0 && sz!=0) {			// no space
		free((char*)gtbl);	// get space for error message
		error('i',"free store exhausted");
	}
	return p;
}

void operator delete (void* p)
{
	if (p == 0) return;

//fprintf(stderr,"free(%d) %d\n",p,((int*)p)[-1]-(int)p-1+sizeof(int*));

	free((char*)p);
}

// class new and delete operators

void*
expr::operator new(size_t sz)
{
	register Pexpr p;

        if ( (p=expr_free) == 0 ) {
                register Pexpr q = (Pexpr) chunk(1);
                for (p=expr_free=&q[EBITE-1]; q<p; p--) {
                        p->e1 = p-1;
                        DB(p->node::allocated=0);
		}
		(p+1)->e1 = 0;
		DB(p->node::allocated=0);
	}
	else
		expr_free = p->e1;

	mzero(p,sz);
	return p;
}

void
expr::operator delete(void* vp, size_t)
{
	Pexpr p = (Pexpr)vp;

	DB( if(!p->node::allocated) error('i',"deleting unallocated expr:%k! -- id==%d",p->base,p->node::id);
	    p->node::allocated = 0;
	);
	p->e1 = expr_free;
	expr_free = p;
	vp = 0;
}

void*
stmt::operator new(size_t sz)
{
	register Pstmt p;
	if ( (p=stmt_free) == 0 ) {
		register Pstmt q = (Pstmt) chunk(1);
		for (p=stmt_free=&q[SBITE-1]; q<p; p--) {
			p->s_list = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->s_list = 0;
		DB(p->node::allocated=0);
	}
	else
		stmt_free = p->s_list;

	mzero(p,sz);
	return p;
}

void
stmt::operator delete(void* vp,size_t)
{
	Pstmt p = (Pstmt)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated stmt:%k! -- id==%d",p->base,p->node::id);
	    p->node::allocated = 0;
	);
	p->s_list = stmt_free;
	stmt_free = p;
	vp = 0;
}

void*
name::operator new(size_t sz)
{
	register Pname p;

	if ( (p=name_free) == 0 ) {
		register Pname q = (Pname) chunk(1);
		for (p=name_free=&q[NBITE-1]; q<p; p--) {
			p->n_tbl_list = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->n_tbl_list = 0;
		DB(p->node::allocated=0);
	}
	else
		name_free = p->n_tbl_list;

	mzero(p,sz);
	return p;
}

void
name::operator delete(void* vp,size_t)
{
	Pname p = (Pname)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated name %s! -- id==%d",p->string?p->string:"???",p->node::id);
	    p->node::allocated = 0;
	);
	p->n_tbl_list = name_free;
	name_free = p;
	vp = 0;
}

void*
table::operator new(size_t sz)
{
	register Ptable p;

	if ( (p=table_free) == 0 ) {
		register Ptable q = (Ptable) chunk(1);
		for (p=table_free=&q[TBITE-1]; q<p; p--) {
			p->next = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->next = 0;
		DB(p->node::allocated=0);
	}
	else
		table_free = p->next;

	mzero(p, sz);
	return p;
}
void*
ktable::operator new(size_t sz)
{
	register Pktab p;

	if ( (p=table_free) == 0 ) {
		register Pktab q = (Pktab) chunk(1);
		for (p=table_free=&q[KBITE-1]; q<p; p--) {
			p->k_next = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->k_next = 0;
		DB(p->node::allocated=0);
	}
	else
		table_free = p->k_next;

	mzero(p, sz);
	return p;
}

void table::operator delete(void* vp,size_t)
{
	Ptable p = (Ptable)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated table %d! -- id==%d",p->base,p->node::id);
	    p->node::allocated = 0;
	);
	p->next = table_free;
	table_free = p;
	vp = 0;
}
void ktable::operator delete(void* vp,size_t)
{
	Pktab p = (Pktab)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated ktable %d! -- id==%d",p->base,p->node::id); p->node::allocated = 0;);
	p->k_next = table_free;
	table_free = p;
	vp = 0;
}

void*
vec::operator new(size_t sz)
{
	register Pvec p;

	if ( (p=vec_free) == 0 ) {
		register Pvec q = (Pvec) chunk(1);
		for (p=vec_free=&q[VBITE-1]; q<p; p--) {
			p->tlist = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->tlist = 0;
		DB(p->node::allocated=0);
	}
	else
		vec_free = (Pvec) p->tlist;

	mzero(p, sz);
	return p;
}

void
vec::operator delete(void* vp,size_t)
{
	Pvec p = (Pvec)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated vec %d! -- id==%d",p->base,p->node::id);
	    p->node::allocated = 0;
	);
	p->tlist = vec_free;
	vec_free = p;
	vp = 0;
}

void*
fct::operator new(size_t sz)
{
	register Pfct p;

	if ( (p=fct_free) == 0 ) {
		register Pfct q = (Pfct) chunk(1);
		for (p=fct_free=&q[FBITE-1]; q<p; p--) {
			p->tlist = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->tlist = 0;
		DB(p->node::allocated=0);
	}
	else
		fct_free = (Pfct) p->tlist;

	mzero(p, sz);
	return p;
}

void
fct::operator delete(void* vp,size_t)
{
	Pfct p = (Pfct)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated fct %d! -- id==%d",p->base,p->node::id);
	    p->node::allocated = 0;
	);
	p->tlist = fct_free;
	fct_free = p;
	vp = 0;
}

void*
ptr::operator new(size_t sz)
{
	register Pptr p;

//error('d',"ptr new");
	if ( (p=ptr_free) == 0 ) {
		register Pptr q = (Pptr) chunk(1);
		for (p=ptr_free=&q[PBITE-1]; q<p; p--) {
			p->tlist = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->tlist = 0;
		DB(p->node::allocated=0);
	}
	else 
		ptr_free = (Pptr) p->tlist;

	mzero(p, sz);
	return p;
}

void
ptr::operator delete(void* vp,size_t)
{
	Pptr p = (Pptr)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated ptr %d! -- id==%d",p->base,p->node::id);
	    p->node::allocated = 0;
	);
	p->tlist = ptr_free;
	ptr_free = p;
	vp = 0;
}

void*
templ_fct::operator new(size_t sz)
{
	register Ptfct p;

	if ( (p=ptfct_free) == 0 ) {
		register Ptfct q = (Ptfct) chunk(1);
		for (p=ptfct_free=&q[PTBIT-1]; q<p; p--) {
			p->tlist = p-1;
			DB(p->node::allocated=0);
		}
		(p+1)->tlist = 0;
		DB(p->node::allocated=0);
	}
	else
		ptfct_free = (Ptfct) p->tlist;

	mzero(p, sz);
	return p;
}

void
templ_fct::operator delete(void* vp,size_t)
{
	Ptfct p = (Ptfct)vp;
	DB( if(!p->node::allocated) error('i',"deleting unallocated templ_fct %d! -- id==%d",p->base,p->node::id);
	    p->node::allocated = 0;
	);
	p->tlist = ptfct_free;
	ptfct_free = p;
	vp = 0;
}

