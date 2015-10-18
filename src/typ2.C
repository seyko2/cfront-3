/*ident	"@(#)cls4:src/typ2.c	1.6" */
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
#include "template.h"

extern int chars_in_largest;
int largest_int;

void typ_init()
{	
	chars_in_largest = strlen(LARGEST_INT);
	largest_int = int(str_to_long(LARGEST_INT));

	defa_type = new basetype(INT,0);	// note defa_type!=int_type
	int_type = new basetype(INT,0);		// but they both represent `int'

	PERM(int_type); int_type->defined = DEFINED ;
	PERM(defa_type); defa_type->defined = DEFINED ;

	moe_type = new basetype(INT,0);
	PERM(moe_type); moe_type->defined = DEFINED ;
	moe_type->b_const = 1;
	moe_type->check(0);

	uint_type = new basetype(INT,0);
	PERM(uint_type); uint_type->defined = DEFINED ;
	uint_type->type_adj(UNSIGNED);
	uint_type->check(0);

	long_type = new basetype(LONG,0);
	PERM(long_type); long_type->defined = DEFINED ;
	long_type->check(0);

	ulong_type = new basetype(LONG,0);
	PERM(ulong_type); ulong_type->defined = DEFINED ;
	ulong_type->type_adj(UNSIGNED);
	ulong_type->check(0);

	vlong_type = new basetype(VLONG,0);
	PERM(vlong_type); vlong_type->defined = DEFINED ;
	vlong_type->check(0);

	uvlong_type = new basetype(VLONG,0);
	PERM(uvlong_type); uvlong_type->defined = DEFINED ;
	uvlong_type->type_adj(UNSIGNED);
	uvlong_type->check(0);

	short_type = new basetype(SHORT,0);
	PERM(short_type); short_type->defined = DEFINED ;
	short_type->check(0);

	ushort_type = new basetype(SHORT,0);
	PERM(ushort_type); ushort_type->defined = DEFINED ;
	ushort_type->type_adj(UNSIGNED);
	ushort_type->check(0);

	float_type = new basetype(FLOAT,0);
	PERM(float_type); float_type->defined = DEFINED ;

	double_type = new basetype(DOUBLE,0);
	PERM(double_type); double_type->defined = DEFINED ;

	ldouble_type = new basetype(LDOUBLE,0);
	PERM(ldouble_type); ldouble_type->defined = DEFINED ;

	zero_type = new basetype(ZTYPE,0);
	PERM(zero_type); zero_type->defined = DEFINED ;
	zero->tp = zero_type;

	void_type = new basetype(VOID,0);
	PERM(void_type); void_type->defined = DEFINED ;

	char_type = new basetype(CHAR,0);
	PERM(char_type); char_type->defined = DEFINED ;

	uchar_type = new basetype(CHAR,0);
	PERM(uchar_type); uchar_type->defined = DEFINED ;
	uchar_type->type_adj(UNSIGNED);
	uchar_type->check(0);

	Pchar_type = char_type->addrof();
	PERM(Pchar_type); Pchar_type->defined = DEFINED ;

	Pint_type = int_type->addrof();
	PERM(Pint_type); Pint_type->defined = DEFINED ;

	Pvoid_type = void_type->addrof();
	PERM(Pvoid_type); Pvoid_type->defined = DEFINED ;

	Pfctvec_type = new fct(int_type,0,0);	// must be last, see basetype::normalize()
	Pfctvec_type = Pfctvec_type->addrof();
	Pfctvec_type = Pfctvec_type->addrof();
	PERM(Pfctvec_type); Pfctvec_type->defined = DEFINED ;

	gtbl = new table(GTBLSIZE,0,0);
	gtbl->t_name = new name("global");
	ptbl = new table(CTBLSIZE,0,0);
	ptbl->t_name = new name("ptbl");
//	tmp_tbl = new table(CTBLSIZE,0,0);
	templ_compilation::templates = new table(128,0,0);
	bound_expr_tbl = new table(12,0,0);
}

Pbase basetype::arit_conv(Pbase t)
/*
	perform the "usual arithmetic conversions" C ref Manual 6.6
	on "this" op "t"
	"this" and "t" are integral or floating
	"t" may be 0
*/
{
// error('d', "arit_conv: this: %k %d %t %d", base, base, this, this );

	bit l;
	bit u;
	bit f;
	bit v;
	bit l1 = (base == LONG);
	bit v1 = (base == VLONG);
	bit u1 = b_unsigned;
	bit f1 = (base==FLOAT || base==DOUBLE || base==LDOUBLE);
	if (t) {
		t = (Pbase)t->skiptypedefs();
// error('d', "arit_conv: t: %k %d %t %d", t->base, t->base, t, t );

		bit l2 = (t->base == LONG);
		bit v2 = (t->base == VLONG);
		bit u2 = t->b_unsigned;
		bit f2 = (t->base==FLOAT || t->base==DOUBLE || t->base==LDOUBLE);
		l = l1 || l2;
		v = v1 || v2;
		u = u1 || u2;
		f = f1 || f2;
	}
	else {
		l = l1;
		v = v1;
		u = u1;
		f = f1;
	}

	if (f) {
		if (base==LDOUBLE || (t && t->base==LDOUBLE)) return ldouble_type;
		if (base==DOUBLE || (t && t->base==DOUBLE)) return double_type;
		return float_type;
	}
	if (l & u) return ulong_type;
	if (l & !u) return long_type;
	if (v & u) return uvlong_type;
	if (v & !u) return vlong_type;
	if (u) {
		if (base==INT || (t && t->base==INT)) return uint_type;
		if (SZ_SHORT==SZ_INT) // ANSIism
			if (base==SHORT || (t && t->base==SHORT)) return uint_type;
		return int_type;
	}

	return int_type;
}

bit vec_const = 0;
bit fct_const = 0;

bit type::tconst()
/*
	is this type a constant
*/
{
	Ptype t = this;
	vec_const = 0;
	fct_const = 0;
//error('d',"tconst %t",t);
xxx:
	switch (t->base) {
	case TYPE:
		if (Pbase(t)->b_const) return 1;
		t = Pbase(t)->b_name->tp;
		goto xxx;
	case VEC:
		vec_const = 1;
		return 1;
	case PTR:
	case RPTR:
		return Pptr(t)->b_const;
	case FCT:
	case OVERLOAD:
		fct_const = 1;
		return 1;
	default:
		return Pbase(t)->b_const;
	}
}

bit
type::is_const_object()
{
	int cc = this->tconst();
	Ptype tt = this->skiptypedefs();
	while ( cc && tt->base == VEC ) {
		tt = Pvec(tt)->typ;
		cc = tt->tconst();
		tt = tt->skiptypedefs();
	}
	return cc;
}

TOK type::set_const(bit mode)
/*
	make someting a constant or variable, return old status
*/
{
	Ptype t = this;
	int m;
xxx:
	switch (t->base) {
	case TYPE:
		m = Pbase(t)->b_const;
		Pbase(t)->b_const = mode;
		t = Pbase(t)->b_name->tp;
		goto xxx;
	case ANY:
	case RPTR:
	case VEC:
		return t->base;		// constant by definition
	case PTR:
		m = Pptr(t)->b_const;
		Pptr(t)->b_const = mode;
		return m;
	default:
		m = Pbase(t)->b_const;
		Pbase(t)->b_const = mode;
		return m;
	}
}

Pptr type::is_ref()
{
	Ptype t = skiptypedefs();

	switch (t->base) {
	case RPTR:	return Pptr(t);
	default:	return 0;
	}
}

Pclass Mptr;

Pptr type::is_ptr()
{
	Ptype t = skiptypedefs();

	switch (t->base) {
	case PTR:
	case VEC:	Mptr = Pptr(t)->memof;
			return Pptr(t);
	default:	return 0;
	}
}

Pptr type::is_ptr_or_ref()
{
	Ptype t = skiptypedefs();

	switch (t->base) {
	case PTR:
	case RPTR:
	case VEC:	Mptr = Pptr(t)->memof;
			return Pptr(t);
	default:	return 0;
	}
}

int type::align()
{
	Ptype t = skiptypedefs();

/*fprintf(stderr,"align %d %d\n",t,t->base);*/
	switch (t->base) {
	case COBJ:	return t->bname_type()->align();
	case VEC:	return Pvec(t)->typ->align();
	case ANY:	return 1;
	case CHAR:	return AL_CHAR;
	case SHORT:	return AL_SHORT;
	case INT:	return AL_INT;
	case LONG:	return AL_LONG;
	case VLONG:	return AL_VLONG;
	case FLOAT:	return AL_FLOAT;
	case DOUBLE:	return AL_DOUBLE;
	case LDOUBLE:	return AL_LDOUBLE;
	case PTR:
	case RPTR:	return AL_WPTR;
	case CLASS:	return Pclass(t)->obj_align;
	case ENUM:
	case EOBJ:	return AL_INT;
	case VOID:	error("illegal use of void"); return AL_INT;
	default:	error('i',"(%d,%k)->type::align",t,t->base); return 0;
	}
}

bit fake_sizeof;

int type::tsizeof(int ptmc)
/*
	the sizeof type operator
	return the size in bytes of the types representation
*/
{
	Ptype t = skiptypedefs();

//error('d',"zx %t %d",t,t->base);
	if (t == 0) error('i',"typ.tsizeof(t==0)");
	switch (t->base) {
	case COBJ:
		t = t->bname_type();
		if (t == 0) return 0;	// ``fake'' generated classes: _Sdd
		return t->tsizeof(ptmc);

	case ANY:	return 1;
	case VOID:	return 0;
	case ZTYPE:	return SZ_WPTR;	/* assume pointer */
	case CHAR:	return SZ_CHAR;
	case SHORT:	return SZ_SHORT;
	case INT:	return SZ_INT;
	case LONG:	return SZ_LONG;
	case VLONG:	return SZ_VLONG;
	case FLOAT:	return SZ_FLOAT;
	case DOUBLE:	return SZ_DOUBLE;
	case LDOUBLE:	return SZ_LDOUBLE;

	case VEC:
		{	Pvec v = Pvec(t);
			if (v->size == 0) {
				if (fake_sizeof == 0) error("sizeof array with undeclared dimension");
				return SZ_WPTR;	// vector argument has sizeof ptr
			}
			return v->size * v->typ->tsizeof();
		}
	case RPTR:	
	case PTR:
	{	
		int k = (Pptr(t)->memof && !ptmc)?sizeof(short)+sizeof(short):0;
		t = Pptr(t)->typ->skiptypedefs();

		switch (t->base) {
		default:	return SZ_WPTR;
		case CHAR:	return SZ_BPTR;
		case FCT:	return SZ_WPTR+k;
		}
	}
	case FIELD:
		error("sizeof(field)");
		return int(Pbase(t)->b_bits)/BI_IN_BYTE+1;
	case OVERLOAD:
	case FCT:
		error("sizeof(function)");
		return 0;

	case CLASS:	
	{
                Pclass cl = Pclass(t);
		if ((cl->defined&(DEFINED|SIMPLIFIED)) == 0) {
			if ( cl->class_base == CL_TEMPLATE || 
			     cl->class_base == UNINSTANTIATED )
				error("YC%t not yet instantiated; please add an explicit instantiation (Tdef%t<Ts>)",cl,cl);
			else error("%tU, size not known",cl);
			return SZ_INT;
		}
	 	if (cl->c_body == 1)	// detect first allocation or sizeof
	 		cl->dcl_print(0);
		return cl->obj_size;
	}

	case EOBJ:
		if ((t->enumtype()->defined&(DEFINED|SIMPLIFIED)) == 0)
			error("%tU, size not known",t->enumtype());
	case ENUM:	return SZ_INT;

	default:	return 0;	// deref can be called for any type
			//error('i',"sizeof(%d)",t->base);
	}
}

bit type::vec_type()
{
	Ptype t = skiptypedefs();

	switch (t->base) {
	case ANY:
	case VEC:
	case PTR:
	case RPTR:	return 1;
	default:	return 0;
	}
}

int ref_initializer;

Ptype type::deref()
/*	index==1:	*p
	index==0:	p[expr]
*/
{
//error('d',"%t -> deref() refd %d",this,ref_initializer);
	Ptype t = skiptypedefs();

	switch (t->base) {
	case PTR:
	case RPTR:
	case VEC:
	{
		if (t == Pvoid_type)
			error("void* dereferenced");
		t = Pvec(t)->typ;
		if (ref_initializer == 0) {
			Ptype tt = t->skiptypedefs();
			if (tt->base == COBJ) { 
                        	tt = tt->bname_type();
                        	if (tt && Pclass(tt)->defined&(DEFINED|SIMPLIFIED))
                                	(void) t->tsizeof();
                        }
		}
		// no break
	}
	case ANY:
		return t;
	default:
		error("nonP dereferenced");
		return any_type;
	}
}

Pfct type::memptr()
// is ``this'' a pointer to member function
{
	if (this == 0) return 0;
	Ptype t = skiptypedefs();
	if (t->base != PTR || Pptr(t)->memof==0) return 0;

	t = Pptr(t)->typ->skiptypedefs();
	return (t->base == FCT) ? Pfct(t) : 0;
}

Ptype type::skiptypedefs()
// starting from ``this'', return first non-typedef
{
	register Ptype t = this;
	if(t==0) return t;
	while (t->base == TYPE) {
		t = t->bname_type();
	}
	return t;
}

Ptype type::skiptypedefs(bit &isconst)
// starting from ``this'', return first non-typedef
{
	register Ptype t = this;
	isconst=0;
	if(t==0) return t;
	while (t->base == TYPE) {
		isconst |= Pbase(t)->b_const;
		t = t->bname_type();
	}
	isconst |= Pbase(t)->b_const;
	return t;
}

Ptype type::mkconst()
{
	Ptype r = this;

	switch(base) {
	case TYPE:
	{
		Ptype t = skiptypedefs();
		if(t->base == VEC)
			return t->mkconst();
	}
	case INT:
	case CHAR:
	case SHORT:
	case LONG:
	case VLONG:
	case FLOAT:
	case DOUBLE:
	case LDOUBLE:
	case FIELD:
	case ZTYPE:
	case COBJ:
	case EOBJ:
		if(b_const==0) {
//			r = new basetype(*(Pbase)this); // won't compile with 2.0
			r = new basetype(base,0);
			PERM(r);
			*(Pbase)r = *(Pbase)this;
			r->b_const=1;
		}
		break;
	case PTR:
	case RPTR:
		if(b_const==0) {
//			r = new ptr(*(Pptr)this); // won't compile with 2.0
			r = new ptr(base,0);
			PERM(r);
			*(Pptr)r = *(Pptr)this;
			r->b_const=1;
		}
		break;
	case VEC:
	{
		Ptype t = Pvec(this)->typ->mkconst();
		if(t != Pvec(this)->typ) {
//			r = new vec(*(Pvec)this); // won't compile with 2.0
			r = new vec(Pvec(this)->typ,0);
			PERM(r);
			*(Pvec)r = *(Pvec)this;
			Pvec(r)->typ=t;
		}
		break;
	}

	default:
		error('i',"mkconst for %t",this);
	}

	return r;
}
