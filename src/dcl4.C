/*ident	"@(#)cls4:src/dcl4.c	1.38" */
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

dcl4.c:
	Declaration of class and enum

*************************************************************************/

#include "cfront.h"
#include "size.h"
#include "template.h"

static Pname get_vf(Pname n_pvf, Pclass cl) {
/*----------------------------------------------------------------------------
	search the base hierarchy of class "cl" if n_pvf (Name of Pure Virtual 
	Function) has a definition.

	returns 0 		if no definition found in the hierarchy.
	returns Pname 		if found.
 *----------------------------------------------------------------------------
 */
	if(!n_pvf || !cl) return 0;

	if (n_pvf->tp  && 
		!cl->has_base(Pfct(n_pvf->tp)->memof)) {
		return 0;	//if pvf is not in a base of "cl"
	}
	Pname n_vf = cl->memtbl->look(n_pvf->string,0);
	if (n_vf  && n_pvf->tp  && n_vf->tp) {
		if (n_vf->tp->base == OVERLOAD) {
			for (Plist gl = Pgen(n_vf->tp)->fct_list; gl; gl = gl->l) {
				if (!Pfct(n_pvf->tp)->check(gl->f->tp, VIRTUAL))
					break;
			}
			if (!gl)
				n_vf = 0;
		}
		else {
			if (Pfct(n_pvf->tp)->check(Pfct(n_vf->tp), VIRTUAL))
				n_vf = 0;
		}
	}
//error('d',"get_vf %n in %t for pure virt. %n",n_vf, cl, n_pvf);
        if (n_vf==0) {
		for (Pbcl b=cl->baselist; b; b = b->next) {
			if (b->base == NAME || b->base == VIRTUAL) {
				n_vf = get_vf(n_pvf,b->bclass);
				if (n_vf && !n_vf->n_initializer) {
				   break;
				} else {
				   n_vf = 0;
				}
			}
    		}  //for (Pbcl b=...
	}  //if (n_vf==0)

	return n_vf;
}

Pname find_vptr(Pclass);
void fix_args(Pfct f, Pclass cl)
/*
	This function is used to cope with the case where cl::cl(cl&) is
	declared AFTER f has been declared
	set n_xref bit for f
*/
{
	for (Pname a = f->argtype; a; a = a->n_list) {
		Pname n = a->tp->is_cl_obj();
		if (n && same_class(Pclass(n->tp),cl))
			a->n_xref = 1;
	}
}

struct typedef_info {
	Pname n;
	typedef_info *next;
};
static typedef_info *typedef_info_head=0, *typedef_info_tail=0;

static Pfct get_fct_node(Pname n)
{
	Ptype t = n->tp ? n->tp->skiptypedefs() : 0;

	if (t && t->base==RPTR)
		t = Pptr(t)->typ->skiptypedefs();

	while (t && t->base==PTR)
		t = Pptr(t)->typ->skiptypedefs();

	return t && t->base==FCT ? Pfct(t) : 0;
}

static void record_typedef(Pname tn)
{
	typedef_info *p = new typedef_info;
	p->n = tn;
	if (typedef_info_head==0) {
		typedef_info_head = typedef_info_tail = p;
	} else {
		typedef_info_tail->next = p;
		typedef_info_tail = p;
	}
}

void typedef_check(Pname n)
{
	Pname cn;
	Pfct f = get_fct_node(n);

	if (!f) return;

	for (Pname a = f->argtype; a; a = a->n_list) {
		cn = a->tp ? a->tp->is_cl_obj() : 0;
		if (!a->n_xref && cn && cn->tp && !(Pclass(cn->tp)->defined&DEFINED)) {
			record_typedef(n);
			return;
		}
	}

	cn = f->returns ? f->returns->is_cl_obj() : 0;
	if ((!f->s_returns || f->s_returns!=void_type) && cn && cn->tp && !(Pclass(cn->tp)->defined&DEFINED))
		record_typedef(n);
}

static void typedef_checkall(Pclass cl)
{
	for (typedef_info *p=typedef_info_head; p; p=p->next) {
		Pfct f = get_fct_node(p->n);
		Pname cn;
		for (Pname a = f->argtype; a; a = a->n_list) {
			cn = a->tp ? a->tp->is_cl_obj() : 0;
			if (!a->n_xref && cn && cn->tp && same_class(Pclass(cn->tp),cl) && cl->has_itor()) {
				error('s', &p->n->where, "C%t used asAT inTdef%n before it was known that%t has a copyK", cl, p->n, cl);
				break;
			}
		}
		cn = f->returns ? f->returns->is_cl_obj() : 0;
		if ((!f->s_returns || f->s_returns!=void_type) && cn && cn->tp && same_class(Pclass(cn->tp),cl) && cl->has_itor())
			error('s', &p->n->where, "C%t used as returnT inTdef%n before it was known that%t has a copyK", cl, p->n, cl);
	}
}

Pname merge_conv(Pname c1, Pname c2)
{
	if (c1 == 0)
		return c2;
	if (c2 == 0)
		return c1;
	if (c1 == c2)
		return c1;
	error('s',"cannot merge lists of conversion functions");
	return c1;
}

static int Eppp;

char* get_classname(char* s)
/*
	retrieve the outermost class name in a vtable name
*/
{
// error('d',"get_classname(%s)",s);
	char* s1 = 0;

	while (*s) {
		s1 = s;
	loop:
		for ( ; s[0] && (s[0] != '_' || s[1] && s[1] != '_'); s++);
		if (*s) {
			if (strncmp(s,"___pt__",7)==0) {
				s+=7; goto loop;
			}
			if (strncmp(s,"__pt__",6)==0) { // parameterized class
				s+=6; goto loop;
			}
			if ( s[1] == 0 ) ++s;
			else s += 2;			// bypass "__"
		}
	}

	return s1;
}

char* drop_classname(char* s)
/*
	retrieve all but the outermost class name in a vtable name
*/
{
// error('d',"drop_classname(%s)",s);
	char* r = new char[strlen(s)+1];
	sprintf(r,s);
	s = r;

	char* s1 = s;
	while (*s) {
loop:
		for ( ; s[0] && (s[0] != '_' || s[1] && s[1] != '_'); s++);
		if (*s) {
			if (strncmp(s,"___pt__",7)==0) {
				s+=7; goto loop;
			}
			if (strncmp(s,"__pt__",6)==0) { // parameterized class
				s+=6; goto loop;
			}
			if ( s[1] == 0 ) { s1 = ++s; break; }
			s1 = s;
			s += 2;			// bypass "__"
		}
	}

	*s1 = '\0';
	return (*r) ? r : 0;
}

bit classdef::has_const_mem()
/*
	does this class have any constant members
*/
{
	int i;
	for (Pname m = memtbl->get_mem(i=1); m; NEXT_NAME(memtbl,m,i))  
		if (m && m->tp) 
			switch (m->tp->base) {
				case FCT: break;
				case OVERLOAD: break;
				case EOBJ: break;
				default: 
					if (m->tp->is_const_object() && m->n_stclass!=STATIC) return 1;
			}
	
	return 0;
}

Pbcl classdef::get_base( char *s )
/*
	Find the base class whose name matches the argument
*/
{
//error('d',"%t::get_base(%s) baselist%d %t",this,s,this?baselist:0,this?baselist->bclass:0);
	for (Pbcl b = baselist; b; b = b->next) {
//error('d',"  b%t s %s",b->bclass,s);
		for (
			char *s1 = s, *s2 = b->bclass->string;
			*s1 && *s2 && *s1 == *s2;
			s1++, s2++
		)
			;

		if (!(*s1 || *s2))
			break;
	}

	return b;
}

static int 
offset_magic_0(Pbcl b, Pbcl bb, short virt_count) 
{
// error('d',"offset_magic_0(%t %t %d)", b->bclass,bb->bclass,virt_count);

	Pclass bcl = bb->bclass;
	int offset = 0;
	for (Pbcl l = b; l; l=l->next) {
		if (l->base == VIRTUAL) continue; 
		Pclass bc = l->bclass;
		for (Pbcl ll = bc->baselist; ll; ll=ll->next) {
			if (ll->base != VIRTUAL) continue;
			int diff = strcmp(bcl->string,ll->bclass->string);
			// if (ll->allocated && !diff) {
			if (ll->allocated == 1 && !diff) {
				offset = ll->obj_offset + l->obj_offset; 
// error('d',"%d offset_magic_0 returning found offset of %t",offset, bcl);
				return offset;
			}
			
			// if (diff && ll->allocated) {
			if (diff && ll->allocated == 1) {
				offset = ll->bclass->has_allocated_base(bcl);
				if (offset) {
					if (!virt_count)
					    offset += l->obj_offset;
// error('d',"%d offset_magic_0 returning allocated offset of %t",offset, bcl);
					return offset;
				}
			}
		}
	}

	if (Nvbc_alloc) {
		offset = bb->obj_offset - Nvbc_alloc->obj_offset;
		if (virt_count) offset += 4;
// error('d',"nvbc: %t %d bb: %d offset: %d",Nvbc_alloc->bclass,Nvbc_alloc->obj_offset,bb->obj_offset,offset);
	}

// error('d',"offset_magic_0 offset: %d virtualBC %t",offset,bcl);
	return offset;
}

static int 
offset_magic_1(Pbcl b, Pbcl bl)
{
// error('d',"offset_magic_1( %t, %t )",b->bclass,bl->bclass);
	char *str = b->bclass->string;
	for (Pbcl bb=bl->bclass->baselist; bb; bb=bb->next) {
		if (bb->base != VIRTUAL) continue;
		char *s = bb->bclass->string;
		if (strcmp(s,str)==0) {
			int offset = bb->obj_offset - bb->ptr_offset +1;
// error('d',"str: %s offset: %d obj_offset: %d",str,offset,bl->obj_offset);
			return offset + bl->obj_offset;					
		}
	}


// error('d',"0 offset_magic_1 for virtualBC%t contained within%t",b->bclass,bl->bclass);
	return 0;
}

// cope with offsets of virtuals in deep nests: promoted, not allocated
extern int link_compat_hack;
static int Voffset;	
bit Vvtab;	
bit Vvbc_alloc;	
bit Vvbc_inher;

int classdef::get_offset(char* s, bit rechk)
/*  Get offset represented by string as viewed from "this" */
{
// error('d',"%s::get_offset(%s %d)",string,s,rechk);
	if (!s) return 0;

        char *str = get_classname(s);
	Pbcl b = get_base(str);

	bit unalloc = 0; 
	char *found_virtual = 0; 

        if (rechk) {
		// unalloc = (b->promoted && (b->allocated == 0));
		unalloc = (b->promoted && (b->allocated != 1));
// error('d',"%s::get_offset unalloc: %d b->alloc: %d",string,unalloc,b->allocated);

		if (!unalloc) return 0;
	 	Vvtab = 0; // promoted virtual base contains vtbl
	 	Vvbc_inher = 0; 
	 	Vvbc_alloc = 0; // promoted virtual base contains vtbl
		Nvbc_alloc = 0; // set by has_allocated_base
		found_virtual = has_allocated_base(str);
	}

	if (unalloc) {
// error('d', "recheck get_offset: voffset: %d found_virtual: %s str: %s ps: %s",Voffset,found_virtual,str);
		if (found_virtual == 0) {
			if (Vvbc_alloc && b->bclass->baselist)
				return b->obj_offset;
			
			if (strcmp(s,str)) { 
				if (Vvbc_inher)
			 	    return 0;
				
				if (Vvtab == 0)
				    return b->bclass->get_offset(drop_classname(s));

				return b->obj_offset + b->bclass->get_offset(drop_classname(s));
			}
			else {
				if (Vvbc_inher) {
			            if (b->allocated == 2)
					link_compat_hack = b->obj_offset;
			 	    return 0;
				}
				return offset_magic_0(baselist->next,b,virt_count);
			}
		}

		if (found_virtual) {
			if (strcmp(s,str)) 
				return offset_magic_1(b,get_base(found_virtual));
			else return b->obj_offset;
		}
	}

	return b->obj_offset + b->bclass->get_offset(drop_classname(s));
}

char* vtbl_str(char* s1, char* s2)
/*
	combine two pieces of a vtbl name
*/
{
// error('d',"vtbl_str(%s,%s)",s1,s2);
	char* s3;
	if (s1)
		if (s2) {
			s3 = new char[strlen(s1)+strlen(s2)+3];
			sprintf(s3,"%s__%s",s1,s2);
			return s3;
		}
		else
			return s1;
	else
		return s2;
}

void classdef::add_vtbl(velem* v, char* s, bit virt_flag, int n_init)
/*
	add vtbl to virt_list
*/
{
// error('d',"%t->add_vtbl(%d,%s)",this,v,s);
	Pvirt vtab = new virt(this, v, s, virt_flag, n_init);

	if (virt_flag)
		has_vvtab = 1;

	if (!virt_list) {
		virt_list = vtab;
		return;
	}

	// If conficting vtable entries are made because of
	// a virtual base class, must be considered an error.
	for (Pvirt vt = virt_list; vt; vt = vt->next )
		if (vt->string && strcmp(vt->string,s)==0) {
			velem* ivec = vt->virt_init;
			Pname on = ivec[0].n;
			Pname nn = v[0].n;
			Pclass ocl,ncl;
			for (int i=0; on && nn; i++,on=ivec[i].n,nn=v[i].n) {
				ocl = Pfct(on->tp)->memof;
				ncl = Pfct(nn->tp)->memof;
				if (on != nn)
					if (!ocl->has_base(ncl))
						if (!ncl->has_base(ocl))
							break;
						else {
							ivec[i].n = nn;
							ivec[i].offset = v[i].offset;
						}
			}
			if (on || nn)
				error("virtualB: ambiguous%n and%n", on, nn);
			return;
		}

	vtab->next = virt_list->next;
	virt_list->next = vtab;
}

static int baseoffset(Pclass b, Pclass d)
/*
	find offset of base class object "b" in derived class object "d"
*/
{
	static int level=0;
	++level;

	if (!b || !d) {
		error ('i', "invalidA to baseoffset()");
	}

	if (same_class(b,d)) {	// b == d
		level--;
		return 0;
	}

	Pbcl bx;

	// is "b" a direct base of "d" or a promoted indirect virtual base ?
	for (bx = d->baselist; bx; bx = bx->next)
	{
		if (same_class(b, bx->bclass) )	{	//(b == bx->bclass)
			level--;
			return bx->obj_offset;
		}
	}

	// recursively search for "b" in bases of "d"
	for (bx = d->baselist; bx; bx = bx->next)
	{
		// don't search indirect virtual bases
		if (bx->promoted)
			continue;
		int found = baseoffset(b, bx->bclass);
		if (found != -1) {
			level--;
			return bx->obj_offset + found;
		}
	}

	if (level-- > 1)
		return -1;	// b!=d and b is not a base of d

	error('i', "fall off end of baseoffset()");
	return 0;
}

int vcounter;
static int vmax;
const int vpChunk = 32;

int classdef::do_virtuals(Pvirt vtab, char* str, int leftmost, bit virt_flag)
/*
	make vtbl for b in "this"
	match up virtuals and assign virtual indices for the base or delegate "bcl"
	first base class shares ``this'' and vtbl with this class
*/
{
	if (vmax<vcounter)
		vmax = vcounter;
	int vpsz = (vmax+vcounter)/vpChunk+1;	// fragmentation prevention
	vpsz *= vpChunk;
	Pname* vp = new Pname[vpsz];
	velem* ivec = vtab ? vtab->virt_init : 0;
	int vo = Voffset;
	int vc = 0;
	int changed = 0;

//error('d',"%t->do_virtuals(%d,%s) voffset %d",this,vtab,str,Voffset);
//error('d',"virt_count %d vpsz %d vcounter %d",virt_count,vpsz,vcounter);

	if (ivec) {	// vtbl replacement for ivec
		if (vtab->is_vbase) {
			str = 0;
			Voffset = get_offset(vtab->string);
// error('d',"vtabl->is_vbase: %s->get_offset(%s), voffset: %d",string,vtab->string,Voffset);
		}
		else
			Voffset = Voffset + vtab->vclass->get_offset(vtab->string);
		Pname vn;
		for (int i=0; vn=ivec[i].n; i++) {
		/*
			go through virtual table's list of virtuals:
			first see if the function is simply inherited
			if not, check for a match
			if not, then add as new
		*/
			if ( i >= vpsz ) {	// resize vp vector
				int tvpsz = vpsz + vpChunk;
				Pname *tvp = new Pname[ tvpsz ];
				for ( int j = 0; j < i; ++j )
					tvp[ j ] = vp[ j ];
				delete [/*vpsz*/] vp;
				vp = tvp;
//error( 'd',"resizing: i: %d vpsz: %d tvpsz: %d", i, vpsz, tvpsz );
				vpsz = tvpsz;
			}

			char* s = vn->n_gen_fct_name;
			Pname n = memtbl->look(s?s:vn->string, 0);

//error('d',"vn %n %s n %n %d",vn,s,n,Voffset);
//error('d',"n %n %k", n, n?n->base:0 );
			if (n == 0 || n->base == PUBLIC ) {	// FCT + FCT
								// base::FCT
			inher:					// inherit
				if (vn->n_initializer) {  
					if (!get_vf(vn,this)) {
						c_abstract = vn;
					}
				}  
				vp[i] = vn;
				if ( ivec[i].offset && vtab->is_vbase ) {
 //error('d',"voffset[1]: %d vo: %d ivec[%d].offset: %d",Voffset,vo,i,ivec[i].offset);
				   vp[i]->n_offset = Voffset -
						     baseoffset(Pfct(vn->tp)->
						     memof, this);
				}
				else vp[i]->n_offset = ivec[i].offset;
				continue;
			}

			Pfct f = Pfct(n->tp);
			if (f == 0 ) continue;

			if (s && f->base==OVERLOAD) {		// OVERLOAD + OVERLOAD
								// vn is overloaded
								// and s is its name
				for (Plist gl=Pgen(f)->fct_list; gl; gl=gl->l)
					if (gl->f == vn)
						goto inher;
			}

			Pfct vnf = Pfct(vn->tp);
			switch (f->base) {			// re-define?
			default:
				error('w',&n->where,"%n hides virtual%n",n,vn);
				vp[i] = vn;	// not a new overloaded: inherit 
				if (vn->n_initializer) {  
					if (!get_vf(vn,this)) {
						c_abstract = vn;
					}
				}  
				if ( ivec[i].offset && vtab->is_vbase ) {
					if (Voffset - vo != ivec[i].offset) {
// error('d',"voffset[2]: %d vo: %d ivec[%d].offset: %d",Voffset,vo,i,ivec[i].offset);

						int noffset = get_offset(vtab->string,1);
						if (noffset && noffset != Voffset) 
							noffset -= vo;
						if (noffset == 0) { 
							if (Vvbc_inher) 
								noffset = link_compat_hack 
									? link_compat_hack - ivec[i].offset
									: ivec[i].offset;
							else noffset = Voffset - vo;
						}
						vp[i]->n_offset = noffset;
					}
					else vp[i]->n_offset = Voffset - vo;
				}
				else vp[i]->n_offset = ivec[i].offset;
				break;
			case FCT:				// derived::FCT
			{
				if (vnf->check(f,VIRTUAL)==0) {	// derived::FCT match base::FCT
								// VTOK: virtual, but no index assigned
								// you can only inherit an index from your first base
					if (Vcheckerror)
						error("bad virtualT match for %n",vn);
					if (f->f_virtual==VTOK)
						f->f_virtual = i+1;
					vp[i] = n;
					vp[i]->n_offset = Voffset;
					changed = 1;
				}
				else {
					if (Vcheckerror)
						error("bad virtualT match for %n",vn);
					else {
						// if (! vn->n_initializer) // not a pure virtual ?????
						// inherits a pure virtual -- make abstract
						if (vn->n_initializer) {  
							if (!get_vf(vn,this)) {
								c_abstract = vn;
							}
						}  
						error('w',&n->where,"%n hides virtual%n",n,vn);
					}
					vp[i] = vn;	// not a new overloaded: inherit
					if ( ivec[i].offset && vtab->is_vbase ) {
						if (Voffset - vo != ivec[i].offset) {
// error('d',"voffset[3]: %d vo: %d ivec[%d].offset: %d",Voffset,vo,i,ivec[i].offset);

							int noffset = get_offset(vtab->string,1);
							if (noffset && noffset != Voffset) 
								noffset -= vo;
							if (noffset == 0) { 
								if (Vvbc_inher) 
									noffset = link_compat_hack 
										? link_compat_hack - ivec[i].offset
										: ivec[i].offset;
								else noffset = Voffset - vo;
							}
							vp[i]->n_offset = noffset;
						}
						else vp[i]->n_offset = Voffset - vo;
					}
					else vp[i]->n_offset = ivec[i].offset;
				}
				break;
			}
			case OVERLOAD:			// derived::OVERLOAD
			{				
				int hit = 0;
				for (Plist gl=Pgen(f)->fct_list; gl; gl=gl->l) {
				// try each fct from derived class
					Pname fn = gl->f;
					Pfct f = Pfct(fn->tp);

					if (f->check(vnf,VIRTUAL) == 0) {	// derived::FCT
						if (Vcheckerror)
							error("bad virtualT match for %n",vn);
						if  (f->f_virtual==VTOK)
							f->f_virtual = i+1;
						vp[i] = fn;
						vp[i]->n_offset = Voffset;
						changed = 1;
						goto found;
					}
					else {
						if (Vcheckerror)
							error("bad virtualT match for %n",vn);
					}
					if (Vcheckerror == 0)
						switch (f->f_virtual) {
						case 0:
						case VTOK:
							hit = 1;
						}
	
				}

				if (hit) {
					// inherits a pure virtual -- make it abstract
					if (vn->n_initializer) {  
						if (!get_vf(vn,this)) {
							c_abstract = vn;
						}
					}  
					error('w',&n->where,"%n hides virtual%n ofT %t",n,vn,vn->tp);
				}

				vp[i] = vn;	// not a new overloaded: inherit
				if ( ivec[i].offset && vtab->is_vbase ) {
					if (Voffset - vo != ivec[i].offset) {
// error('d',"voffset[4]: %d vo: %d ivec[%d].offset: %d",Voffset,vo,i,ivec[i].offset);

						int noffset = get_offset(vtab->string,1);
						if (noffset && noffset != Voffset) 
							noffset -= vo;
						if (noffset == 0) { 
							if (Vvbc_inher) 
								noffset = link_compat_hack 
									? link_compat_hack - ivec[i].offset
									: ivec[i].offset;
							else noffset = Voffset - vo;
						}
						vp[i]->n_offset = noffset;
					}
					else vp[i]->n_offset = Voffset - vo;
				}
				else vp[i]->n_offset = ivec[i].offset;
			found:
				break;
			}
			}
		}

		Voffset = vo;
		vc = i;
	}

// error( 'd', "do_virtuals: out of loop: vc: %d vpsz: %d changed: %d", vc, vpsz,changed );

	if (leftmost) {
		/*
			add new virtuals:
			`VTOK' marks ``new virtual, no index assigned''.
			You can only be new once (no base or first base).
		*/
		int i;
                for (Pname nn=memtbl->get_mem(i=1); nn; NEXT_NAME(memtbl,nn,i) ) {
			if ( nn->base == TNAME ) continue;
			Pfct f = Pfct(nn->tp);

			if ( vc >= vpsz ) {	// resize vp vector
				int tvpsz = vpsz + vpChunk;
				Pname *tvp = new Pname[ tvpsz ];
				for ( int j = 0; j < vc; ++j )
					tvp[ j ] = vp[ j ];
				delete [/*vpsz*/] vp;
				vp = tvp;
				vpsz = tvpsz;
			}

// error('d',"f %n %t",nn,f);
			if (f)
				switch (f->base) {
				case FCT:
//error('d',"fv %d",f->f_virtual);
					if (f->f_virtual == VTOK) {
						// declared virtual, or
						// virtual in some base
						f->f_virtual = ++vc;
						vp[f->f_virtual-1] = nn;
						vp[f->f_virtual-1]->n_offset = 0;
						f->f_vdef = 1;
						changed = 2;
					}
					break;
				case OVERLOAD:
					{
					for (Plist gl=Pgen(f)->fct_list; gl; gl=gl->l) {
						Pname fn = gl->f;
						Pfct f = Pfct(fn->tp);

		 				if ( vc >= vpsz ) {	// resize vp vector
			 				int tvpsz = vpsz + vpChunk;
			 				Pname *tvp = new Pname[ tvpsz ];
			 				for ( int j = 0; j < vc; ++j ) {
				 				tvp[ j ] = vp[ j ];
 							}
			 				delete [/*vpsz*/] vp;
			 				vp = tvp;
			 				vpsz = tvpsz;
		 				}
 
						if (f->f_virtual == VTOK) {
							f->f_virtual = ++vc;
							vp[f->f_virtual-1] = fn;
							vp[f->f_virtual-1]->n_offset = 0;
							f->f_vdef = 1;
							changed = 2;
						}
					}
					break;
					}
				}
		}
// error('d',"%s changed %d has_vvtab %d",string,changed,has_vvtab);
// error('d',"vc %d vpsz %d",vc,vpsz);
		virt_count = 0;
		if (changed)
			virt_count = vc;
		else if (has_vvtab) {
			virt_merge = 1;
			if (vc && vtab->is_vbase)
				leftmost = 0;
		}
	}

// error('d',"vc %d ch %d vp[%d] virt_count %d",vc,changed,vpsz,virt_count);
	if (changed || !leftmost) {
		// vc==0 if all explicit virtuals in fact were declared in base
		velem* v = new velem[vc+1];

		for (int i=0; i<vc; i++) {
			v[i].n = vp[i];
			v[i].offset = vp[i]->n_offset;
		}
		v[vc].n = 0;

		if (leftmost)
			add_vtbl(v,0,0,0);
		else
			add_vtbl(v,vtbl_str(vtab->string,str),virt_flag||vtab->is_vbase,vc+1);
		delete vp;
		vcounter = 0;
		return 1;
	}

	delete vp;
	vcounter = 0;
	return 0;
}

int classdef::all_virt(Pclass bcl, char* s, int leftmost, bit virt_flag)
{
//error('d',"%t->all_virt( %t %s leftmost: %d",this,bcl,s,leftmost);
	int i = 0;
	if (bcl->virt_count) {
		for (Pvirt blist = bcl->virt_list; blist; blist = blist->next) {
			if (
				virt_merge
				&&
				virt_flag==0
				&&
				blist->is_vbase==0
			)
				continue;

			i += do_virtuals(blist, s, leftmost, virt_flag);
			if (i==0 && leftmost && virt_merge==0)
				return 0;

			leftmost = 0;
		}
	}

	// finding virt_list stops recursive step
	// if vtables found and updated, return number
	if (i)
		return i;

	for (Pbcl b = bcl->baselist; b; b = b->next) {
		if (b->promoted)
			continue;
// error('d',"all_virt b %t vl %d bl %d",b->bclass,b->bclass->virt_list,b->bclass->baselist);
		if (leftmost && b->base == VIRTUAL) {
			i += do_virtuals(0, 0, 1, 0);
			if (i==0 && virt_merge==0) {
				// be careful to propagate class abstraction
				if (bcl == this && b->bclass->c_abstract) 
					c_abstract = b->bclass->c_abstract; 
				return 0;
			}
			leftmost = 0;
		}
		int vo = Voffset;
		if (b->base == VIRTUAL)
		{
			Voffset = baseoffset(b->bclass,this);
		}
		else
		{
			Voffset += b->obj_offset;
		}
// error('d',"all_virt obj_offset %t voffset %d vo %d  %d",b->bclass,Voffset,vo);

		if (b->base==VIRTUAL)
			i += all_virt(b->bclass, b->bclass->string, leftmost, 1);
		else 	i += all_virt(b->bclass, vtbl_str(b->bclass->string,s), leftmost, virt_flag);

		if (i==0 && leftmost && virt_merge==0) {
			// be careful to propagate class abstraction
			if (bcl==this && !c_abstract && b->next) { 
				for (Pbcl bb = b; bb; bb = bb->next) { 
					if (bb->promoted) continue;
					if (bb->bclass->c_abstract) 
						c_abstract = bb->bclass->c_abstract; 
				}
			}
			return 0;
		}

		Voffset = vo;
		leftmost = 0;
	}

	// if recursion updated vtables, return number
	if (i)
		return i;

	// no vtables updated in recursion
	// look for new virtuals
	if (leftmost)
		return do_virtuals(0, 0, 1, 0);
	else
		return 0;
}

Pexpr copy_obj(Pexpr l, Pexpr r, int sz)
/*
	generate:
	struct _s { char[sz]; };
	*(struct _s*)this->m = *(struct _s*)arg.mem;
*/
{
	if ( !sz ) sz = 1;
// error('d',"copy_obj(%d)",sz);
	char* s = make_name('S');
	fprintf(out_file,"struct %s { char v[%d]; };\n",s,sz);
	Pname n = new name(s);
	Ptype t = new basetype(COBJ,n);
	t = new ptr(PTR,t);

	l = new texpr(G_CAST,t,l);	//new cast(t,l);
	l = l->contents();

	r = new texpr(G_CAST,t,r);	//new cast(t,r);
	r = r->contents();

	return new expr(ASSIGN,l,r);
}

Ptype find_arg_type(Pclass cl)
// first determine argument type
{
	int i;
	int mod = 0;
	for (Pbcl b = cl->baselist; b; b = b->next) {
		Pclass bcl = b->bclass;
		switch (b->base) {
		case VIRTUAL:
		case NAME:		// generate :b(*(b*)&arg)
		{
			Pname itor = bcl->has_itor();
			if (itor && itor->tp->base==FCT) {
				Pname a = Pfct(itor->tp)->argtype;
				Pptr p = a->tp->is_ref();
				if (p && p->typ->tconst()==0) {
					mod = 1;
					goto ll1;
				}
			}
		}
		}
	}
ll1:
	if (mod == 0) {
		for (Pname m=cl->memtbl->get_mem(i=1); m; NEXT_NAME(cl->memtbl,m,i) ) {
			if ( m->base == TNAME ) continue;
			if (m->n_evaluated || m->n_stclass==STATIC)	// ignore static members
				continue;
			Pname cln = ( m->base==PUBLIC ? m->n_qualifier : m )->tp->is_cl_obj();
			if ( cln ) {
				Pname itor = Pclass(cln->tp)->has_itor();
				if (itor && itor->tp->base==FCT) {
					Pname a = Pfct(itor->tp)->argtype;
					Pptr p = a->tp->is_ref();
					if (p && p->typ->tconst()==0) {
						mod = 1;
						goto ll2;
					}
				}
			}
		}
	}
ll2:
//error('d',"mod %d",mod);
	Pbase bp = new basetype(INT,0);
	*bp = *Pbase(Pptr(cl->this_type)->typ);
	if (mod == 0) bp->b_const = 1;
	return new ptr(RPTR,bp);
}

Pname classdef::make_itor(int def)
/*
	make cn::cn(const cn&) :bases_and_members_of_cn {}
*/
{
// error('d',"%t->make_itor(%d) %d",this,def,obj_size);
	Pstmt s;
	Pname e;
	int i;
	Pname arg = new name(make_name('A'));
	arg->tp = find_arg_type(this);

	c_xref |= C_XREF;	// now it has X(X&)

	if (def) {		// define itor
		int slow = 0;	// slow==0 => copy using vector copy
		int first = 1;
		Pexpr es = 0;
		s = new estmt(SM,no_where,0,0);
		e = 0;

		if (warning_opt && 128<obj_size)
			error('w',"copying a %d byte object (ofC %s)",obj_size,string);

		if (baselist)
			slow = 1;

		// initialize bases:
		if (slow) {
			for (Pbcl b = baselist; b; b = b->next) {
				Pclass bcl = b->bclass;
				Ptype pt = bcl->this_type;
				if (b->base==VIRTUAL || b->base==NAME) {	// generate :b(*(b*)&arg)
					Pexpr b2 = new name(arg->string);
					b2 = b2->address();
					b2 = new texpr(G_CAST,pt,b2);//new cast(pt,b2);
					b2->i2 = 1;
					b2 = b2->contents();

					Pname ee = new name(bcl->string);
					ee->base = TNAME;
					ee->n_initializer = b2;
					if (e)
						ee->n_list = e;
					e = ee;
				}
			}
		}

		for (Pname mm=memtbl->get_mem(i=1); mm; NEXT_NAME(memtbl,mm,i)) {	
			Ptype t = mm->tp;
			if (t && (t=t->skiptypedefs())->base==COBJ) {
					Pclass mcl = Pclass(Pbase(t)->b_name->tp);
					if (mcl->c_xref&(C_VBASE|C_XREF)) slow=1;
			}
		}


		for (Pname m=memtbl->get_mem(i=1); m; NEXT_NAME(memtbl,m,i)) {	// initialize members
			if ( m->base == TNAME ) continue;
			if (m->n_evaluated || m->n_stclass==STATIC)		// ignore static members
				continue;

			if (strcmp(m->string,"__vptr")==0) {
				if (slow==0 && first==0 && m->n_offset) {
					Pexpr th = new expr(THIS,0,0);
					Pexpr a = new name(arg->string);
					a = a->address();
					Pexpr ee = copy_obj(th,a,m->n_offset);
					es = es ? new expr(CM,es,ee) : ee;
				}
				slow = 1;
				first = 0;
				continue;
			}
			Ptype mt = m->tp;
			if ( mt )
		tx:	switch (mt->base) {
			case TYPE:
				mt = Pbase(mt)->b_name->tp;
				goto tx;
			case VEC:
			{
				Pname cn = Pvec(mt)->typ->is_cl_obj();
				if (cn && Pclass(cn->tp)->c_xref&(C_XREF|C_VBASE)) {
					error('s',"copy of %n[], no memberwise copy for%n",cn,cn);
					slow = 1;	// make sure an assignment operator
							// is generated so that there will
							// be no more error messages
				}

				if (slow && mt->tsizeof()) {
				/*
					generate:
					struct _s { char[sizeof(m)]; };
					*(struct _s*)this->m = *(struct _s*)arg.mem;
				*/
					Pexpr l = new name(m->string);
					Pexpr r = new name(m->string);
					r = new ref(DOT,new name(arg->string),r);
					Pexpr ee = copy_obj(l,r,mt->tsizeof());
					es = es ? new expr(CM,es,ee) : ee;
					break;
				}
			}
			case FCT:
			case OVERLOAD:
			case CLASS:
			case ENUM:
				break;
			case COBJ:
			{
				Pclass mcl = Pclass(Pbase(mt)->b_name->tp);
				if (
					slow==0		// if slow, previous members have already been copied
					&&
					mcl->c_xref&(C_VBASE|C_XREF)
				) {
					slow = 1;
					if (first==0 && m->n_offset) {
							//AAA copy up to here
						Pexpr th = new expr(THIS,0,0);
						Pexpr a = new name(arg->string);
						a = a->address();
						Pexpr ee = copy_obj(th,a,m->n_offset);
						es = es ? new expr(CM,es,ee) : ee;
					}
				}
				// no break
			}

			default:
				if (slow) {
// error('d',"slow %s %s",m->string,arg->string);
					Pname ee = new name(m->string);
					ee->n_initializer = new ref(DOT,new name(arg->string),new name(m->string));
					if (e)
						ee->n_list = e;
					e = ee;
				}
			}
			first = 0;
		}

		if (slow == 0) {
		/*	really simple just copy:
			the only problem was a vptr which can be
			ignored since X(X&) is going to reset it anyway
			don't use assignment of this struct to avoid operator=
		*/
			Pexpr th = new expr(THIS,0,0);
			Pexpr a = new name(arg->string);
			a = a->address();
			Pexpr ee = copy_obj(th,a,obj_size);
			es = es ? new expr(CM,es,ee) : ee;
		}

		s->e = es;
		Pname cn = this->k_tbl->find_cn(string);//SYM
		if (cn)
			cn = Pbase(cn->tp)->b_name;
		cc->stack();
		cc->not = cn;
		cc->cot = this;
	}
	Pname fn = new name(string);
	Pfct f = new fct(defa_type,arg,1);
	fn->tp = f;
	fn->n_oper = TNAME;

	Pfct(f)->f_inline = def?1:ITOR;		// ITOR means ``define itor() if used''
	Pfct(f)->f_is_inline = 1;
	if (def) {
		f->body = new block(curloc,0,s);
		f->f_init = e;
	}

	Pname nn = fn->dcl(memtbl,PUBLIC);
	delete fn;

	if (def) {
		cc->unstack();
		nn->simpl();
		if (debug_opt)
			nn->dcl_print(0);
	}
	return nn;
}

Ptype is_op_ass(Pfct f, Pclass tcl)
/*	is the operator an assignment operator? */
{
 	Ptype t = f->argtype->tp;
 	Pname an = t->is_cl_obj();        // X::operator=(X)
 	if (an==0 && (t=t->is_ref())) {   // X::operator=(X&)
 		t = Pptr(t)->typ;
 
 		Ptype nt = t;
 		while (nt->base == TYPE)
 			nt = Pbase(nt)->b_name->tp;
 
 		if (nt->base == COBJ)
 			an = Pbase(nt)->b_name;
 	}
	if (an && same_class(Pclass(an->tp),tcl)) return t;
 	return 0;
}

int make_assignment(Pname cn)
/*
	write the function:

	X& X::operator=(const X&) { assign all bases and members }

	return 1 is a function is really synthesized
*/
{
	Pclass cl = Pclass(cn->tp);
	Pstmt s = new estmt(SM,no_where,0,0);
	Pexpr e = 0;
	Pname arg = new name(make_name('A'));
	arg->tp = find_arg_type(cl);
//error('d',"make_assignment %t %d",cl,cl->obj_size);

	if (warning_opt && 128<cl->obj_size)
		error('w',"copying a %d byte object (ofC %s)", cl->obj_size,cl->string);
	int slow = 0;			// slow==0 => copy using vector copy
	int first = 1;			// first==1 => first member of (derived) class
	bit unconst=0;

	if (cl->baselist)
		slow = 1;		// be dumb and safe

	if (slow) {
		for (Pbcl b = cl->baselist; b; b = b->next) {
			Pclass bcl = b->bclass;
			Ptype pt = bcl->this_type;
			switch (b->base) {
			case NAME:
			{	// generate: *(bcl*)this = *(bcl*)&arg;
//error('d',"base %t",bcl);
				Pexpr b1 = new expr(THIS,0,0);
				b1 = new texpr(G_CAST,pt,b1);
				b1 = b1->contents();

				Pexpr b2 = new name(arg->string);
				b2 = b2->address();
				b2 = new texpr(G_CAST,pt,b2);
				b2->i2 = 1;
				b2 = b2->contents();

				Pexpr ee = new expr(ASSIGN,b1,b2);
				e = e ? new expr(CM,e,ee) : ee;
				break;
			}
			case VIRTUAL:
				if (warning_opt)
					error('w',"copying an object ofC%n with a virtualBC",cn);
				if (b->ptr_offset) {
					// copy object, but not pointer
					// generate: *(bcl*)this->Pw = *(bcl*)arg->Pw;

					// I don't know how to avoid copying the object
					// once for each pointer

					Pexpr b1 = new expr(THIS,0,0);
					b1 = new mdot(bcl->string,b1);
					b1->i1 = 3;
					b1->tp = pt;
					b1 = new expr(DEREF,b1,0);//b1->contents();

					Pexpr b2 = new name(arg->string);
					b2 = b2->address();
					b2 = new mdot(bcl->string,b2);
					b2->i1 = 3;
					b2->tp = pt;
					b2 = new expr(DEREF,b2,0);//b2->contents();

					Pexpr ee = new expr(ASSIGN,b1,b2);
					e = e ? new expr(CM,e,ee) : ee;
				}
				break;
			}
		}
	}

	int i;
	for (Pname m=cl->memtbl->get_mem(i=1); m; NEXT_NAME(cl->memtbl,m,i) ) {
		if ( m->base == TNAME ) continue;
		// ignore static members
		if (m->n_evaluated || m->n_stclass==STATIC)
			continue;

		if (strcmp(m->string,"__vptr")==0) {	// don't copy vptrs
							// we may be copying
							// into a base class object
			if (first==0 && m->n_offset && !e) {
						// copy up to (but not including) vptr
						// don't copy if first member
					Pexpr th = new expr(THIS,0,0);
					Pexpr a = new name(arg->string);
					a = a->address();
					e = copy_obj(th,a,m->n_offset);
			}
			slow = 1;
			first = 0;
			continue;
		}
		Ptype mt = m->tp;
		if (mt)
	tx:	switch (mt->base) {
		case TYPE:
			mt = Pbase(mt)->b_name->tp;
			goto tx;
		case VEC:
		{
			Pname cn = Pvec(mt)->typ->is_cl_obj();
			if (cn && Pclass(cn->tp)->c_xref&(C_ASS|C_VBASE)) {
				error('s',"copy of %n[], no memberwise copy for%n",cn,cn);
				slow = 1;	// make sure an assignment operator
						// is generated so that there will
						// be no more error messages
			}

			if (slow && mt->tsizeof()) {
			/*
				protect against sizeof(mt)==0: char[]
				generate:
					struct _s { char[sizeof(m)]; };
					*(struct _s*)this->m = *(struct _s*)arg.mem;
			*/	
				Pexpr l = new name(m->string);
				Pexpr r = new name(m->string);
				r = new ref(DOT,new name(arg->string),r);
				Pexpr ee = copy_obj(l,r,mt->tsizeof());
				e = e ? new expr(CM,ee,e) : ee;
				break;
			}
		}
		case FCT:
		case OVERLOAD:
		case CLASS:
		case ENUM:
			break;
		case COBJ:
//error('d',"cobj %n %d %d",m,slow,Pclass(Pbase(mt)->b_name->tp)->c_xref);
			{
			Pclass tempcl=Pclass(Pbase(mt)->b_name->tp);
			if (
				slow==0
				&&
				Pclass(Pbase(mt)->b_name->tp)->c_xref&(C_VBASE|C_ASS)
			) {					// must use its assignment operation
				if (first==0 && m->n_offset) {	// copy up to this member 
					Pexpr th = new expr(THIS,0,0);
					Pexpr a = new name(arg->string);
					a = a->address();
					e = copy_obj(th,a,m->n_offset);
				}
				slow = 1;
			}

			suppress_error++;
			Pname nn = tempcl->has_oper(ASSIGN);
			suppress_error--;
 			if( nn && nn->tp->base == FCT ) {
 			Pptr atp=Pptr(nn->fct_type()->argtype->tp);
 				if( atp && atp->typ && atp->typ->b_const==0) {
 					unconst = 1;
 				}
 			}
 			else if (nn && nn->tp->base == OVERLOAD) {
 				Plist pl = Pgen(nn->tp)->fct_list;
 				while (pl) {
 					Pfct ff = pl->f->fct_type();
					Ptype t = is_op_ass(ff,tempcl);
 				        if(t && t->b_const==0) {
 					    unconst = 1;
 					    break;
 					}
 					pl = pl->l;
 				}
 			}
			}
			// no break: copy cobj itself
		case RPTR:
			if (mt->base==RPTR)
				if (cl->c_xref!=C_REFM) {
					error("cannot assignC%t:RM%n",cl,m);
					break;
				}
				else slow=1;
		default:
 //error('d',"defa %n %d",m,slow);
			if (slow) {
				if (m->tp->tconst())
					error("cannot assignC%t: const M%n",cl,m);
				Pname ms = new name(m->string);
				Pname as = new name(arg->string);
				Pexpr ee = new ref(DOT,as,new name(m->string));
				ee = new expr(ASSIGN,ms,ee);
				e = e ? new expr(CM,e,ee) : ee;
			}
		}
		first = 0;
	}

	if (slow == 0) {		// really simple just copy:
		return 0;
	}

	Pexpr rv = new expr(THIS,0,0);
	rv = new expr(DEREF,rv,0);	//b1->contents();
	
	s->e = e ? new expr(CM,e,rv) : e;
	s->s_list = new estmt(RETURN,no_where,rv,0);

	cc->stack();
	cc->not = cn;
	cc->cot = cl;

	cl->c_xref |= C_ASS;		// now it has X::operator=(const X&)

	Pname fn = new name(oper_name(ASSIGN));
	Pfct f = new fct(new ptr(RPTR,Pptr(cl->this_type)->typ),arg,1);
	f->f_inline = 1;
	f->f_is_inline = 1;
	if (unconst)
		Pptr(f->argtype->tp)->typ->b_const=0;
	fn->tp = f;
	fn->n_oper = ASSIGN;
	fn->n_sto = STATIC;
	Pname nn = fn->dcl(cl->memtbl,PUBLIC);
	delete fn;
	Pfct(nn->tp)->body = new block(curloc,0,s);
	Pfct(nn->tp)->dcl(nn);

	cc->unstack();
	nn->simpl();
//error('d',"make_assign->");
	return 1;
}

void prnt_all_in_scope(Pclass cl) {
//
// Lays out all members  (visible from an inner _nested_ class)
// traversing the in_class chain  (up to the outer most class).
//
int   i;

   if (cl->in_class)
      prnt_all_in_scope(cl->in_class);
    	for (Pname nn=cl->memtbl->get_mem(i=1); nn; NEXT_NAME(cl->memtbl,nn,i))
        {
		if ( nn->base == TNAME ) continue;
		if (
			((nn->base) && nn->base==NAME)
			&&
			(nn->n_anon==0)
			&&
			(nn->n_stclass == STATIC)
			&&
                        ((nn->tp) &&
                         (
			  nn->tp->base!=FCT
			  &&
			  nn->tp->base!=OVERLOAD
			  &&
			  nn->tp->base!=CLASS
			  &&
			  nn->tp->base!=ENUM
                         )
                        )
		   ) {
			nn->dcl_print(0);
		     }
	}
}

bit simpl_friend;

void classdef::dcl(Pname cname, Ptable tbl)
{
	int bvirt = 0;
	int dvirt = 0;
	int scope = PUBLIC;
	int protect = 0;
	int st = 1;		// nothing private or protected seen: a struct

	int byte_old = byte_offset;
	int bit_old = bit_offset;
	int max_old = max_align;
	int boff = 0;

	int in_union = 0;
	int usz;
	int make_ctor = 0;
	int make_dtor = 0;

	if (this == 0)		// this is the place for paranoia
		error('i',"0->Cdef::dcl(%p)",tbl);
	if (base != CLASS)
		error('i',"Cdef::dcl(%d)",base);
	if (cname == 0)
		error('i',"unNdC");
	if (cname->tp != this)
		error('i',"badCdef");
	if (tbl == 0)
		error('i',"Cdef::dcl(%n,0)",cname);
	if (tbl->base != TABLE)
		error('i',"Cdef::dcl(%n,tbl=%d)",cname,tbl->base);
	DB( if(Ddebug>=1) error('d',&cname->where,"classdef::dcl %s tbl %d gtbl %d",string,tbl,gtbl); );
// error('d',&cname->where,"classdef::dcl %s tbl %d gtbl %d",string,tbl,gtbl); 

	switch (csu) {
	case UNION:
		in_union = UNION;
		break;
	case ANON:
		in_union = ANON;
		break;
	case CLASS:
		scope = 0;
	}

	max_align = AL_STRUCT;

	if ( local_sig ) {
		c_context = tbl;
		in_fct = cc->nof;
	} else in_fct = 0;

	if (baselist) {
		/*
			check base classes.
			duplicates were removed in start_cl() in norm.c.
			remove bad classes.
			add virtual bases from bases to the list.

			check against
				class b : a {}
				class c : a, b {}	// first a inaccessible
		*/
		
		Pbcl ll = 0;
		Pbcl lll = 0;
		Pbcl vlist = 0;
		for (Pbcl lx, l=baselist; l; l=lx) {	// remove bad bases
			Pclass cl = l->bclass;

			lx = l->next;

//error('d',"base1 %t %k init %d",cl,l->ppp,l->init);
			if (l->ppp == 0) {		// ``class'' => private base ``struct'' => public base
				l->ppp = csu==CLASS ? PRIVATE : PUBLIC;
#ifndef OLD
				if (l->ppp == PRIVATE)
					error('w',"B%t private by default: please be explicit ``: private%t",cl,cl);
#endif
			}
			// if you have a ``class'' as base you cannot remain a ``mere struct''
			if (cl && cl->csu == CLASS)
				st = 0;

			if ((cl->defined&(DEFINED|SIMPLIFIED)) == 0) {
				error("BC %tU",cl);
				continue;
			}
			else
				(void)cl->tsizeof();	// ensure printout

			if (cl->csu==UNION || cl->csu==ANON) {
				error("C derived from union");
				continue;
			}

			if (in_union) {
				error("derived union");
				continue;
			}

			if (warning_opt &&
			    (l->base==VIRTUAL || l != baselist) &&
			    cl->has_dtor() && cl->has_dtor()->tp &&
			    !Pfct(cl->has_dtor()->tp)->f_virtual)
				error('w', "second or virtualBC%tWout virtual destructor", cl);

			if (l->base==VIRTUAL) {		// order of virtual classes doesn't matter
				l->next = vlist;
				vlist = l;
			}
			else {				// keep ordinary base classes in order
				if (ll == 0) {
					lll = l;
					l->next = 0;
				}
				else
					ll->next = l;
				ll = l;
			}
		}
		if (ll) {
			ll->next = vlist;	// put virtual bases at end
			baselist = lll;
		}
		else
			baselist = vlist;

		lll = 0;
		for (l=baselist; l; l=l->next) {	// detect unmanageable duplicates
			Pclass b = l->bclass;

			for (ll=baselist; ll; ll=ll->next)
				if (!::same_class(b,ll->bclass) && ll->bclass->check_dup(b,l->base)) {
					if (lll)
						lll->next = l->next;
					else
						baselist = l->next;
					goto mmm;
				}

			lll = l;
			mmm:;
		}

		for (l=baselist; l; l=l->next) {	// promote virtual bases
			Pclass b = l->bclass;
			for (ll=b->baselist; ll; ll=ll->next) {
				if (ll->base == VIRTUAL) {
					Pclass v = ll->bclass;
  					for (Pbcl lll=baselist; lll; lll=lll->next) 
						if (::same_class(lll->bclass,v)) {
  							// promoted virtual base class
  							// takes on most accessible level
  							if (ll->ppp < lll->ppp) 
  								lll->ppp = ll->ppp;
							goto nnn;
						}
					baselist = new basecl(v,baselist);
					baselist->base = VIRTUAL;
					baselist->promoted = 1;
// needs a more complete check of visibility rules
//error('d',"promote %t %k",v,ll->ppp);
					baselist->ppp = ll->ppp;
				}
				nnn:;
			}
		}

		ll = 0;
		lll = 0;
		l=baselist;
		baselist = 0;
		vlist = 0;
		for (; l; l=lx) {
					// sort virtual bases so that no virtual base
					// is ahead of its own virtual base
			lx = l->next;
			if (l->base == VIRTUAL) { // add to sorted vlist
						  // each class before its bases
				if (vlist == 0) {
					vlist = l;
					l->next = 0;
				}
				else {
					Pclass lb = l->bclass;
					Pbcl v_prev = 0;
					for (Pbcl vx, v = vlist; v; v=vx) {
						Pclass vb = v->bclass;
						vx = v->next;

						if (lb->has_base(vb)) {	// put l ahead of v
							l->next = v;
							if (v_prev)
								v_prev->next = l;
							else
								vlist = l;
							break;
						}
						if (vx == 0) {	// stick l at end
							v->next = l;
							l->next = 0;
							break;
						}	
						v_prev = v;
					}
				}
			}
			else {			// keep in order
				if (ll == 0) {
					lll = l;
					l->next = 0;
				}
				else
					ll->next = l;
				ll = l;
			}
		}

		if (ll) {
			ll->next = vlist;	// put virtual bases at end
			baselist = lll;
		}
		else
			baselist = vlist;

		for (l=baselist; l; l=l->next) {	// allocate base class objects
			Pclass cl = l->bclass;
// error('d',"classdef::dcl base %t %k %k",cl,l->ppp,l->base);
			if (l->base == VIRTUAL) {	// : virtual bclass
							// pointer and object for virtual base MAY
							// be allocated at the end - but not here
				c_xref |= C_VBASE;
				dvirt += cl->virt_count;
			}
			else {				// : bclass =>allocate
				int ba = cl->align();
				if (max_align<ba)
					max_align = ba;

				if (::same_class(cl,baselist->bclass)) {   // pad to ensure alignment:
					boff = cl->real_size;
								// not obj_size-real_size, we can
								// optimize vbase object away
					int xtra = boff%ba;
								// align
					if (xtra)
						boff += ba-xtra;
				}
				else {				// let C handle the padding:
					int xtra = boff%ba;
					if (xtra)
						boff += ba-xtra;	// align
					l->obj_offset = boff;
								// don't use waste
					boff += cl->obj_size;
				}
				bvirt += cl->virt_count;
			}

			if (cl->has_vvtab)
				has_vvtab = 1;
			c_xref |= cl->c_xref;

//error('d',"%t: base %t conv %d base conv %d",this,cl,conv,cl->conv);
			conv = merge_conv(conv,cl->conv);
		}
	}

	memtbl->set_name(cname);

	int nmem = 0;
	int fct_mem = 0;
	{
		for (Pname m = mem_list; m; m=m->n_list) {
			nmem++;
			if (m->tp && m->tp->base==FCT)
				fct_mem++;
			else if (m->base == TNAME)
				;
			else { 
				if (m->tp && m->tp->base==RPTR && c_xref==0)
					c_xref=C_REFM;
				if (ansi_opt && m->tp && m->tp->tconst() && m->n_sto!=STATIC)
					Pbase(m->tp)->ansi_const=1;
			}
		}
	}

	if (nmem)
		memtbl->grow((nmem<=2)?3:nmem);

	cc->stack();
	cc->not = cname;
	cc->cot = this;

	byte_offset = usz = boff;
	bit_offset = 0;
	
	int real_virts = 0;
	Pbase bt = new basetype(COBJ,cname);
	bt->b_table = memtbl;
	Ptype cct = bt->addrof();
						// for strict opt type of `this': X *const
						// '2' distinguishes this case from a real constant object
	if (strict_opt && perf_opt)
		Pptr(cct)->b_const = 2;
	this_type = cc->tot = cct;
	this_type->ansi_const = 1;
	PERM(cct);
	PERM(bt);

	for (Pname px, p=mem_list; p; p=px) {
	/*
		look at each member;
		declare it and determine its visibility
		calculate offsets and sizes
	*/
		px = p->n_list;
// error( 'd', "p: %n %k n_scope: %d", p, p->base, p->n_scope );

		switch (p->base) {
		case PUBLIC:
			scope = PUBLIC;
			protect = 0;
			goto prpr;

		case PRIVATE:
			scope = 0;
			protect = 0;
			goto prpr;

		case PROTECTED:
			scope = 0;
			protect = PROTECTED;
		prpr:
			if (in_union == ANON)
				error(&p->where,"%k in anonymous unionD",p->base);
			continue;

		case PR:	// visibility control:	C::M
		{
			char* qs = p->n_qualifier->string;
			char* ms = p->string;
			TOK ppp = scope?PUBLIC:(protect?PROTECTED:PRIVATE);

			p->base = NAME;
			p->n_scope = scope;
			p->n_protect = protect;

			if (strcmp(ms,qs) == 0)
				ms = "__ct";

			ppbase = PUBLIC;

			if (is_base(qs) == 0) {
				error("%kQr %s not aBC of %t",ppp,qs,this);
				continue;
			}

			mex = 1; // print error messages for access violations 
			tcl = mec = this;
			Pname os = Cdcl;
			Cdcl = p; // for line #s in error messages 
			Pexpr ee = find_name(ms,0,1);
			Cdcl = os;

// error('d', "ee: %k ", ee->base );

			if ( ee && ee->base != NAME && ee->base != TNAME )
			    error('i',"bad returnE%k from %t->find_name(%s,0,1)",ee->base,this,ms);
			Pname mx = Pname(ee);

// error('d', "ee: %k mx: %n", ee->base, mx );

			if (mx == 0) {
				error("C %s does not have aM %s",qs,ms);
				continue;
			}

			if (mx->tp->base == OVERLOAD) {
				error('s',"%k specification of overloaded%n",ppp,mx);
				continue;
			}

			TOK pp = mx->n_scope?PUBLIC:mx->n_protect?PROTECTED:PRIVATE;
			
// error('d',"mx %n pp %k ppp %k",mx,pp,ppp);
			if (ppp != pp) {
				error(&p->where,"%kM%n specified%k",pp,mx,ppp);
				continue;
			}	

			p->n_qualifier = mx;
			Pname m = memtbl->insert(p,0);
			m->base = PUBLIC;
			if (Nold)
				error("twoDs ofCM%n",p);
			continue;
		}
		}

// error('d',"mem%n tp %d %k scope %d",p,p->tp->base,p->tp->base,scope);

/* XXXXX: 
		if (class_base == INSTANTIATED && templ_base == CL_TEMPLATE) {
			if ( p->base == TNAME || p->tp->base == CLASS
    				|| p->tp->base == ENUM )
					error('s',"nestedT%nWin specializedYC%t",p,this);
		}
*/

		if (scope==0) { 
			if (p->n_sto != STATIC)
				st = 0;
		}
		else if ( p->tp->base == TYPE ) {
			Pname nn = p->tp->is_cl_obj();
			if (nn) { 
// error( 'd', "nn: %n %k tp %t %k", nn, nn->base, nn->tp, nn->tp->base );
				if (
					Pclass(nn->tp)->csu == CLASS
					&&
					strcmp(this->string,nn->string)
				)
					st = 0;
			}
		}

//error('d',&p->where,"p%n base%k tp%k: ll %d",p,p->base,p->tp->base,p->lex_level);
//if(px)error('d',&p->where,"px%n base%k tp%k:%t",px,px->base,px->tp->base,px->tp);
		//if ( p->tp->base == CLASS && Pclass(p->tp)->csu == ANON ) {
		//    if ( px == 0
		//    ||   px->tp->base != COBJ
		//    ||   px->tp->classtype() != p->tp )
		//	error('i',&p->where,"missing anon object");
		//}
		if ( in_union == ANON ) {
		    if ( p->tp->base == CLASS && Pclass(p->tp)->csu == ANON ) {
			// anon inside an anon; lift members to enclosing class
			Pname pu = Pclass(p->tp)->mem_list;
			while ( pu ) {
//error('d',&p->where,"pu%n base%k tp%k: ll %d",pu,pu->base,pu->tp->base,pu->lex_level);
				Ptype tx = 0;
				if ( pu->base == TNAME ) tx = pu->tpdef;
				else if ( pu->tp->base == CLASS 
				     ||   pu->tp->base == ENUM )
					tx = pu->tp;
				else --pu->lex_level;
				if ( tx ) {
					tx->in_class = this;
					delete tx->nested_sig;
					tx->nested_sig = make_nested_name(pu->string,this);
				}
				if ( !pu->n_list ) break;
				pu = pu->n_list;
			}
			if ( pu ) {
				pu->n_list = px->n_list;
				px = Pclass(p->tp)->mem_list;
			}
			Pclass(p->tp)->mem_list = 0;
			continue;
		    } // p is anon class
		    if ( p->base != TNAME 
		    &&   p->tp->base != CLASS
		    &&   p->tp->base != ENUM )
			--p->lex_level;
		} // if this is anon
//error('d',&p->where," -- p%n base%k tp%k: ll %d",p,p->base,p->tp->base,p->lex_level);

		if (p->base == TNAME) {
			p->dcl(memtbl,scope);
			// typedefs need to be generated outside class
			if ( p->tp && Pbase(p->tp)->base != COBJ ) {
				if (p->n_dcl_printed == 2)
					; // don't print
			        else p->dcl_print(0);
			}
			continue;
		} // if TNAME 

		if (p->tp->base == FCT) {
			int ff = 0;
			Pfct f = Pfct(p->tp);
			Pblock b = f->body;
			f->body = 0;
			if (b) {
				f->f_inline = 1;
				f->f_is_inline = 1;
			}
			else if (lex_level) 
				error('w',&p->where,"non-inlineMF%n in localC %t",p,this);
			else if (csu != ANON && string && string[0] == '_' && string[1] == '_' && string[2] == 'C')
				error("non-inlineMF%n in unnamedC", p);
			switch (p->n_sto) {
			case FRIEND:
				ff = 1;
				break;
			case STATIC:	// accept static member functions
//error('d',"inline %d",f->f_inline);
				if (lex_level) 
					error('w',&p->where,"staticMF%n in localC %t",p,this);
				f->f_static = 1;
				p->n_sto = 0;
				break;
			case AUTO:
			case REGISTER:
			case EXTERN:
				error(&p->where,"M%n cannot be%k",p,p->n_sto);
				p->n_sto = 0;
			}

			if (f->f_virtual) real_virts++;
			if (ff && f->fct_base==FCT_TEMPLATE) continue; // already handled

			Pname m = p->dcl(memtbl,scope);
			if (m == 0 || m->tp->base != FCT)
				continue;
			if (m->n_initializer) {
				c_abstract = m;
				if (m->n_oper == DTOR)
					error('w',"please provide an out-of-line definition: %n {}; which is needed by derived classes",m);
			}
			if (ff == 0)
				m->n_protect = protect;
			if (b) {
				if (m->tp->defined&DEFINED || Pfct(m->tp)->body )
					error(&p->where,"two definitions of%n",m);
				else {
					Pfct(m->tp)->body = b;
					Pfct(m->tp)->def_context = f->def_context;
				}
			}
			if (ff==0 && p->tp!=m->tp)
				error(&p->where,"%n cannot be redeclared inCD",p);
		}
		else {
			Eppp = scope?scope:protect?protect:0;

			if (p->n_initializer) {
				error(&p->where,"Ir forM%n",p);
				p->n_initializer = 0;
			}

			if (p->tp->base==OVERLOAD) {
				for (Plist gl=Pgen(p->tp)->fct_list; gl; gl=gl->l) { 
					Pname nn = gl->f;
					Pfct ff = Pfct(nn->tp);
					if ( ff->f_virtual ) real_virts++;
				}
			}

// error('d',"lex_level %d p %n lex %d nested_scope: %k",lex_level,p,p->lex_level, nested_scope);

			TOK is_friend = p->n_sto;
			if (
				protect
				&&
				p->tp->base == COBJ
				&&
				Pclass(Pbase(p->tp)->b_name->tp)->csu == ANON
			)
				p->n_protect = protect;

			Pname m = p->dcl(memtbl,scope);
			Eppp = 0;
			if (m == 0)
				continue;

			m->n_protect = protect;

			if (m->n_stclass==STATIC) {
				if (in_union && is_friend != FRIEND)
					error(&m->where,"staticM%n in union",m);
				if ( is_friend != FRIEND ) {
					if (lex_level)
						error(&m->where,"staticM%n in localC %t",m,this);
				}
				m->n_sto = EXTERN;
				if (tbl == gtbl)
					stat_mem_list = new name_list(m,stat_mem_list);
				if (m->n_initializer)
					error('s',"staticM%nWIr",m);
			}

			if (in_union) {
				if (usz < byte_offset)
					usz = byte_offset;
				byte_offset = 0;
				if(in_union==ANON)
					m->n_offset+=byte_old;
			}
		}
	}

	if (st && csu==CLASS)
		csu = STRUCT;	// nothing private => STRUCT
	if (st==0 && csu==STRUCT)
		csu = CLASS;	// all is not public => CLASS

	if (in_union)
		byte_offset = usz;

	// now look look at the members

	Pname ct = has_ctor();
	Pname dt = has_dtor();

	int i;

	int omex = mex; mex = 0;
	Pname on = has_oper(NEW);
	Pname od = has_oper(DELETE);
	mex = omex;

	if (dt && ct==0 && Pfct(dt->tp)->f_virtual == 0 )
		error('w',"%t has%n but noK",this,dt);
	if (on && od==0)
		error('w',"%t has%n but no operator delete()",this,on);
	if (od && on==0)
		error('w',"%t has%n but no operator new()",this,od);

	if (dt==0 && od && od && od->n_table==memtbl)
		make_dtor = 1;

	for (Pname m=memtbl->get_mem(i=1); m; NEXT_NAME(memtbl,m,i) ) {
		if ( m->base == TNAME ) continue;
	/*
		The members have been declared.
		now look at each to see if it needs defining
	*/
		Ptype t = m->tp;

		if (t == 0)
			continue;	// public declarations
// error('d', "m %n %d", m, m->n_scope );

		switch (t->base) {
		default:
			if (
				ct==0
				&&
				m->n_stclass!=ENUM
				&&
				m->n_stclass!=STATIC
			) {
				if (t->is_ref())
					error("R%n inC%tWoutK",m,this);
				if (
					t->tconst()
					&&
					vec_const==0
					&&
					m->n_evaluated==0
				)
					error("const%n inC%tWoutK",m,this);
			}
			// no break
		case VEC:
			break;

		case FCT:
		case OVERLOAD:
		case CLASS:
		case ENUM:
			continue;
		}

		Pname cn = t->is_cl_obj();
		if (cn == 0)
			cn = cl_obj_vec;
		if (cn == 0)
			continue;

		Pclass cl = Pclass(cn->tp);
		c_xref |= cl->c_xref;
//error('d',"m %n %t %d",m,cl,cl->c_xref);
		if (cl->has_ctor()) {
			if (m->n_stclass == STATIC)
				;
			else if (in_union) {
				if (strncmp("__C",string,3) == 0)
					error("M %s ofC%tWK in union",m->string,cl); // tagless union
				else
					error("M%n ofC%tWK in union",m,cl);
			}
			else if (ct == 0) {
				// does mctor have a default constructor?
				if (make_ctor==0 && cl->has_ictor()==0)
					error("%t needs aK; it has aMWK requiringAs",this);
				make_ctor = 1;
			}
		}
		if (in_union && (c_xref & C_ASS))
			error("M%n ofC%tW assignment operator in union",m,cl);

		if (cl->has_dtor()) {
			if (m->n_stclass==STATIC)
				;
			else if (in_union) {
				if (strncmp("__C",string,3) == 0) // tagless union
					error("M %s ofC%tW destructor in union",m->string,cl);
				else
					error("M%n ofC%tW destructor in union",m,cl);
			}
			else if (dt==0)
				make_dtor = 1;
		}
	}

	if (has_dtor()==0 && make_dtor==0) {
		// can dtor be inherited (from single base class)?
		Pclass bcl = 0;
		for (Pbcl b = baselist; b; b = b->next) {
			switch (b->base) {
			case NAME:
				if (bcl == 0) {
					Pname d = b->bclass->has_dtor();
					if (d==0)
						break;
/*
this breaks CC884232
					if (strict_opt == 0) {
						if (
							d->n_scope==PUBLIC
							&&
							Pfct(d->tp)->f_virtual==0
						) {
							bcl = b->bclass;
							break;
						}
					}
*/
				}
				// two dtors or non-public dtors force creation
				make_dtor = 1;
				goto zbzb;
			case VIRTUAL:
				if (b->bclass->has_dtor())
					make_dtor = 1;
				goto zbzb;
			}
		}
		if (bcl)
			c_dtor = bcl->has_dtor();
	zbzb:;
	}

	if (make_dtor && dt==0) {	// make x::~x() {}
		// must be done before vtbls are made in case dtor is virtual
//error('d',"%t: make_dtor",this);
		Pname n = new name(string);
		Pfct f = new fct(defa_type,0,1);
		f->f_inline = 1;
		f->f_is_inline = 1;
		n->tp = f;
		n->n_oper = DTOR;
		dt = n->dcl(memtbl,PUBLIC);
		delete n;
		Pfct(dt->tp)->body = new block(curloc,0,0);
		dt = has_dtor();
	}

	if (virt_count && find_vptr(this)==0) {
		// we only need a vptr if this class has virtual
		// functions and none of its first bases have vptrs
//error('d',"%s virt_count %d",string,virt_count);
		Pname vp = new name("__vptr");
		vp->tp = Pvptr_type;
		(void) vp->dcl(memtbl,0);
		delete vp;
		c_xref |= C_VPTR;
	}
	else {
//error("byte_offset %d bit_offset %d bitsinbyte %d",byte_offset,bit_offset,BI_IN_BYTE);
//error("   div %d mod %d",bit_offset/BI_IN_BYTE,bit_offset%BI_IN_BYTE);
		// no more bit fields. absorb bit_offset
		if (bit_offset) {
			byte_offset += (bit_offset/BI_IN_BYTE + (bit_offset%BI_IN_BYTE?1:0));
			bit_offset = 0;
		}
	}
//error("byte_offset %d bit_offset %d",byte_offset,bit_offset);

	for (Pbcl b = baselist; b; b = b->next) { // allocate virtual base pointers
		if (b->base != VIRTUAL)
			continue;
		Pclass bcl = b->bclass;

		// search non-virtual bases for this virtual base
		for (Pbcl bb = baselist; bb; bb = bb->next)
			if (bb->base == NAME) {
				for (Pbcl l = bb->bclass->baselist; l; l=l->next)
					if (l->base==VIRTUAL && ::same_class(l->bclass,bcl))
						goto eee;
			}

		{
// error('d',"classdef::dcl vbc* allocation: bcl: %t bb: %t",bcl,bb?bb->bclass:0);
			if (int(obj_align)<AL_WPTR)
				obj_align = AL_WPTR;
			if (max_align<AL_WPTR)
				max_align = AL_WPTR;
			int waste = byte_offset%AL_WPTR;
			if (waste)
				byte_offset += AL_WPTR-waste;		// align
			b->ptr_offset = byte_offset+1;			// ensure != 0
			byte_offset += SZ_WPTR;
		}
	eee:;
	}

	real_size = byte_offset;	// the rest may be optimized away

	for (b = baselist; b; b = b->next) {	// allocate virtual class objects
		if (b->base != VIRTUAL)
			continue;
		Pclass bcl = b->bclass;

		// if necessary
		link_compat_hack = 0;
		if (b->obj_offset = has_allocated_base(bcl))
			continue;

// error('d',"%t->dcl: allocating vbc %t link_compat_hack: %d",this,bcl,link_compat_hack);
		int ba = bcl->align();
		if (int(obj_align)<ba)
			obj_align = ba;
		if (max_align<ba)
			max_align = ba;
		int waste = byte_offset%ba;
		if (waste)
			byte_offset += ba-waste;	// align
		b->obj_offset = byte_offset;		// offset in this
		b->allocated = 1 + link_compat_hack;
		link_compat_hack = 0;
// error('d',"virtual object allocation %t in %t at %d",bcl,this,b->obj_offset);
		byte_offset += bcl->tsizeof();
	}

	// no more data members.
	// pad object (so that copying into a base object
	// doesn't destroy derevid class members):
	if (byte_offset==0) {	// empty struct: waste a member
		Pname c = new name (make_name('W'));
		c->where = cname->where;
		c->tp = char_type;
		(void) c->dcl(memtbl,0);
		real_size = byte_offset = 1;
	}
	if (byte_offset < SZ_STRUCT)
		byte_offset = SZ_STRUCT;
	int waste = byte_offset%max_align;
//error('d',"max_align %d waste %d byte_offset %d",max_align,waste,byte_offset);
	if (waste)
		byte_offset += max_align-waste;
	obj_size = byte_offset;
	obj_align = max_align;

	// make vtbls
	// this cannot be done until the bases
	// have been allocated in this class
	// so that the offsets (deltas) are known
	if (all_virt(this,0,1,0)) {
		if (has_ctor()==0)
			make_ctor = 1;
	}
	else if (has_vvtab)
		error("virtualB: conflicting vtable initialization");

// error('d',"%t->classdef: virt_count: %d virt_merge: %d",this,virt_count,virt_merge);


	Pname hito = has_itor();

	if (hito)
		c_xref |= C_XREF;	// has user defined X(X&)
//error('d',"%t hito %d ctor %d",this,hito,make_ctor);
	if (hito==0 && c_xref&(C_VPTR|C_VBASE|C_XREF)) {
						// X(X&) needed if bitwise copy is illegal
						// or if any constructor is defined

		hito = make_itor(0);
						// if the base has B::B(void)
						// the derived should have D::D(void)
		if (baselist) {
			int mc = 1;		// can make and ictor
			for (Pbcl b = baselist; b; b = b->next) {
				if (b->bclass->has_ctor() && b->bclass->has_ictor()==0)
						mc = 0;
			}
			make_ctor = mc;
		}
		else
			make_ctor =  1;
	}

	if (c_ctor==0 && make_ctor==0) {	// can ctor be inherited (from single base class)?
		Pname btor = 0;
		Pclass bc = 0;
		for (Pbcl b = baselist; b; b = b->next) {
			switch (b->base) {
			case NAME:
			{
				Pname c = b->bclass->has_ctor();
				if (c == 0)
					break;
				if (c->n_scope==PUBLIC && b==baselist) {
					bc = b->bclass;
					btor = bc->has_ictor();
					break;
				}
			}
				// no break: two bases: needs ctor
			case VIRTUAL:
				make_ctor = 1;	// virtual base: need ctor
				goto zaza;
			}
		}
//error('d',"btor %n",btor);
		if (bc) {
			if (btor)
				make_ctor = 1;
			else
				error("K needed for%t, BC%t hasK",this,bc);
		}
	zaza:;
	}

	if (make_ctor && ct==0) {	// make x::x() {}
//error('d',"%t: make_ctor",this);
		Pname n = new name(string);
		Pfct f = new fct(defa_type,0,1);
		f->f_inline = 1;
		f->f_is_inline = 1;
		n->tp = f;
		n->n_oper = TNAME;
		ct = n->dcl(memtbl,PUBLIC);
		delete n;
		Pfct(ct->tp)->body = new block(curloc,0,0);
	}


	if (pt_opt && !nested_sig && !local_sig && !is_probably_temp(string)
		&& !tempdcl) 
		if (csu==UNION)
			fprintf(pt_file,"u %s %s\n",string,curr_filename());
		else if (csu==STRUCT) {
			fprintf(pt_file,"s %s %s\n",string,curr_filename());
		}
		else if (csu==CLASS)
			fprintf(pt_file,"c %s %s\n",string,curr_filename());


//	if (ansi_opt && (defined&(DEFINED))==0) {
//		char* s = csu==UNION || csu==ANON ? "union" : "struct";
//		if ( nested_sig )
//			fprintf(out_file,"%s __%s;\n",s,nested_sig);
//		else
//			fprintf(out_file,"%s %s;\n",s,local_sig?local_sig:string);
//	}

	defined |= DEFINED;
	typedef_checkall(this);

//error('d',"defined %s",string);

	// fix argument lists for inlines
	for (p=memtbl->get_mem(i=1); p; NEXT_NAME(memtbl,p,i)) {
		if ( p->base == TNAME ) continue;
		Pfct f = Pfct(p->tp);
		if (f==0)
			continue;	// public declarations

		switch (f->base) {
		case FCT:
			//SYM remove transitional stuff
			if (hito && f->argtype)
				fix_args(f,this);
			if (p->n_oper == CTOR)
				f->s_returns = this_type;
			if (f->body)
				p->n_sto = STATIC;
			break;
		case OVERLOAD:
		{
			Pgen g = Pgen(f);
			for (Plist gl=g->fct_list; gl; gl=gl->l) {
				Pname n = gl->f;
				Pfct f = Pfct(n->tp);

				if (hito && f->argtype)
					fix_args(f,this);
				if (n->n_oper == CTOR)
					f->s_returns = this_type;
				if (f->body)
					n->n_sto = STATIC;
			}
		}
		}
	}

	// define members defined inline
	for (p=memtbl->get_mem(i=1); p; NEXT_NAME(memtbl,p,i)) {
		if ( p->base == TNAME ) continue;
		Pfct f = Pfct(p->tp);
		if (f==0)
			continue;	// public declarations

		switch (f->base) {
		case FCT:
			if (f->body) {
				f->dcl(p);
				p->simpl();
			}
			break;
		case OVERLOAD:
		{
			Pgen g = Pgen(f);
			for (Plist gl=g->fct_list; gl; gl=gl->l) {
				Pname n = gl->f;
				Pfct f = Pfct(n->tp);

				if (f->body) {
					f->dcl(n);
					n->simpl();
				}
			}
		}
		}
	}

	byte_offset = byte_old;
	bit_offset = bit_old;
	max_align = max_old;

	cc->unstack();			// friends are not in class scope

					// fix arguments lists for friends defined inline 
	// set global context to match current behavior in dcl.c 
	//    for declaration of friends (needed for find_name())
	cc->stack();
	cc->cot = 0; cc->not = 0; cc->tot = 0; cc->c_this = 0;
	for (Plist fl=friend_list; fl; fl=fl->l) {
		Pname p = fl->f;
		Pfct f = Pfct(p->tp);

		switch (f->base) {
		case FCT:
			if (hito && f->argtype)
				fix_args(f,this);
			if (f->body && (f->defined&(DEFINED|SIMPLIFIED)) == 0) 
				p->n_sto = STATIC;
			else if (p->n_scope == STATIC)
				error(strict_opt?0:'w',"static%n declared friend toC%t",p,this);
			break;
		case OVERLOAD:
		{
			Pgen g = Pgen(f);
			for (Plist gl=g->fct_list; gl; gl=gl->l) {
				Pname n = gl->f;
				Pfct f = Pfct(n->tp);

				if (hito && f->argtype)
					fix_args(f,this);
				if (f->body && (f->defined&(DEFINED|SIMPLIFIED)) == 0) 
					n->n_sto = STATIC;
				else if (p->n_scope == STATIC)
					error(strict_opt?0:'w',"static%n declared friend toC%t",p,this);
			}
		}
		}
	}

	// define friends defined inline and modify return types if necessary
	simpl_friend=0;
	for (fl=friend_list; fl; fl=fl->l) {
		Pname p = fl->f;
		Pfct f = Pfct(p->tp);
		simpl_friend=1;

		switch (f->base) {
		case FCT:
			if (f->body && (f->defined&(DEFINED|SIMPLIFIED)) == 0) {
				f->dcl(p);
				p->simpl();
			}
			break;
		case OVERLOAD:
		{
			Pgen g = Pgen(f);
			for (Plist gl=g->fct_list; gl; gl=gl->l) {
				Pname n = gl->f;
				Pfct f = Pfct(n->tp);

				if (f->body && (f->defined&(DEFINED|SIMPLIFIED)) == 0) {
					f->dcl(n);
					n->simpl();
				}
			}
		}
		}
	}
	simpl_friend=0;
	cc->unstack(); // global context for friends

	if (tbl != gtbl)
		this->simpl();

	//SYM removed statStat stuff

// error('d',"classdef::dcl defined: %d",defined);
	// catch refs to this class in body of nested class function
	// (i.e., ref to member of this class...)
	if ( (defined&REF_SEEN) != 0 )
		dcl_print(0);

	if ( debug_opt ) {
           if (nested_sig) {
              prnt_all_in_scope(in_class);
           }
	   void dump_func(Pclass);
	   if (in_fct || !in_class)
	      dump_func(this);
	}

	void check_abst(Pclass);
	if (!in_class)
		check_abst(this);
}

void dump_func(Pclass th)
{
	int i;
	for (Pname p=th->memtbl->get_mem(i=1); p; NEXT_NAME(th->memtbl,p,i)) {
		if (p->base == TNAME || !p->tp) continue;
		Ptype t = p->tp->skiptypedefs();
		if (t->base == CLASS) {
			dump_func(Pclass(t));
			continue;
		}
		Pfct f = Pfct(t);
		if (f==0)
			continue;	// public declarations

		switch (f->base) {
			case FCT:
				if (f->body) {
					if ( th->c_body == 1 ) {
						if (th->class_base == INSTANTIATED)	
							current_instantiation = th;
						th->dcl_print(0);
						if (th->class_base == INSTANTIATED)	
							current_instantiation = 0;
					}
					p->dcl_print(0);
				}
				break;
			case OVERLOAD: 
			{	
				Pgen g = Pgen(f);
				for (Plist gl=g->fct_list; gl; gl=gl->l) {
					Pname n = gl->f;
					Pfct f = Pfct(n->tp);
					if (f->body) {
						if ( th->c_body == 1 ) {
							if (th->class_base == INSTANTIATED)	
								current_instantiation = th;
							th->dcl_print(0);
							if (th->class_base == INSTANTIATED)	
								current_instantiation = 0;
						}
						p->dcl_print(0);
					}
				}
			}
		}
	}

	for (Plist fl=th->friend_list; fl; fl=fl->l) {
		Pname p = fl->f;
		Pfct f = Pfct(p->tp);

		switch (f->base) {
			case FCT:
				if (f->body && (f->defined&(DEFINED|SIMPLIFIED)) == 0) 
					p->dcl_print(0);
				break;
			case OVERLOAD:
			{	
				Pgen g = Pgen(f);
				for (Plist gl=g->fct_list; gl; gl=gl->l) {
					Pname n = gl->f;
					Pfct f = Pfct(n->tp);

					if (f->body && (f->defined&(DEFINED|SIMPLIFIED)) == 0) 
						p->dcl_print(0);
				}
			}
		}
	}
}

void check_abst(Pclass th)
{
	int i;
	for (Pname p=th->memtbl->get_mem(i=1); p; NEXT_NAME(th->memtbl,p,i)) {
		if (p->base == TNAME || !p->tp) continue;
		Ptype t = p->tp->skiptypedefs();
		if (t->base == CLASS) {
			check_abst(Pclass(t));
			continue;
		}
		Pfct f = Pfct(t);
		void check_abst_func(Pname, Pfct);
		if (f == 0)
			continue;
		switch (f->base) {
			case FCT:
				check_abst_func(p, f);
				break;
			case OVERLOAD: {
				Pgen g = Pgen(f);
				for (Plist gl = g->fct_list; gl; gl = gl->l) {
					Pfct f = Pfct(gl->f->tp);
					check_abst_func(gl->f, f);
				}
				break;
			}
		}
	}

}

void check_abst_func(Pname p, Pfct f)
{
	if (f->returns->is_cl_obj()) {
		Pclass cl = f->returns->skiptypedefs()->classtype();
		if (cl->c_abstract) {
			error(&p->where, "abstractC%t cannot be used as aF returnT", cl);
			error('C', &p->where, "\t%a is a pure virtualF ofC%t\n", cl->c_abstract, cl);
		}
	}

	for (Pname a = f->argtype; a; a = a->n_list) {
		if (a->tp && a->tp->is_cl_obj()) {
			Pclass cl = a->tp->skiptypedefs()->classtype();
			if (cl->c_abstract) {
				error(&p->where, "abstractC%t cannot be used as anAT", cl);
				error('C', &p->where, "\t%a is a pure virtualF ofC%t\n", cl->c_abstract, cl);
			}
		}
	}
}

void enumdef::dcl(Pname n, Ptable tbl)
{
	Pname px;
	Pname p = mem;
	DB( if(Ddebug>=1) error('d',&n->where,"enumdef(%n,%d)",n,tbl); );
	Pbase b = new basetype(EOBJ,n);
	b->b_const = 1;

	if ( local_sig ) {
		in_fct = cc->nof;
	} else in_fct = 0;

#define FIRST_ENUM 0
	int enum_count = FIRST_ENUM;
	no_of_enumerators = mem->no_of_names();
	int largest = 0;

	if (p == 0)
		mem = new name(make_name('e'));

	for (; p; p=px) {
		px = p->n_list;
// error( 'd', "p %n", p );
		if (p->n_initializer) {
			Pexpr i = p->n_initializer->typ(tbl);
			Neval = 0;
			long ii = i->eval();
//			if (largest_int<ii)
//				error("long enumerator");
			if (i->tp) {
				bit u = i->tp->skiptypedefs()->is_unsigned();
				bit ok = 0;
				int smallest_int = -largest_int;
				switch (i->tp->skiptypedefs()->base) {
#define CHECKU(t) {ok = (unsigned t)ii <= largest_int;}
#define CHECKS(t) {ok = (t)ii >= smallest_int && (t)ii <= largest_int;}
					case CHAR:
						if (u)
							CHECKU(char)
						else
							CHECKS(char)
						break;
					case SHORT:
						if (u)
							CHECKU(short)
						else
							CHECKS(short)
						break;
					case ZTYPE:
					case INT:
					case ENUM:
					case EOBJ:
						if (u)
							CHECKU(int)
						else
							CHECKS(int)
						break;
					case LONG:
						if (u)
							CHECKU(long)
						else
							CHECKS(long)
						break;
					default:
						// for error checking
						// enum E {e = (void)0};
						ok = 1;
						break;
				}
				if (!ok)
					error("long enumerator");
			}
			enum_count = int(ii);
			if (Neval)
				error(&p->where,"%s",Neval);
			DEL(i);
			p->n_initializer = 0;
		}
		p->n_evaluated = 1;
		largest |= enum_count;
		if (enum_count == largest_int && px && !px->n_initializer)
			error("enumerator too large");
		p->n_val = enum_count++;
		p->tp = b;

		Pname nn;
		if ( tbl == gtbl || tbl->t_name )
			nn = tbl->look(p->string,0);
		else
			nn = curr_block->memtbl->look(p->string,0);
		if (
			nn
			&&
			( tbl==gtbl || tbl->t_name
			  ||
			   nn->n_table->real_block==curr_block->memtbl->real_block
			   && nn->lex_level == p->lex_level
			)
		) {
			if (nn->n_stclass == ENUM)
				error("enumerator%n declared twice",nn);
			else
				error("incompatibleDs of%n",nn);
		}
		else {
			nn = tbl->insert(p,0);
			nn->n_stclass = ENUM; // no store will be allocated
			if (Eppp == PROTECTED)
				nn->n_protect = PROTECTED;
			else if (Eppp == PUBLIC)
				nn->n_scope = PUBLIC;
		}
		p->string = nn->string;
	}

	e_type = int_type;

	defined |= DEFINED;

	if (pt_opt && !nested_sig && !local_sig && !is_probably_temp(string)) 
		fprintf(pt_file,"e %s %s\n",string,curr_filename());
}
