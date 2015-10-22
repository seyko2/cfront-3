/*ident	"@(#)cls4:lib/new/_vec.c	1.5" */
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



typedef void* PV;
typedef void (*PF)(PV,int,int,int,int,int,int,int,int);
typedef void (*PFI)(PV,int ...);
/*
	ERROR: cannot handle classes with more than 8 virtual bases
*/

// functions that manage the map from arrays to their element counts
extern int	__insert_new_array(PV key, int count);
	// key is a pointer to a new array.  It must
	//	be non-zero
	//	not already be in the table
	// count is the number of elements in the array.  May be zero
extern int	__remove_old_array(PV key);
	// removes an old array from the table.  Returns the count or -1 if not found

static PV	__cache_key;
static int	__cache_count;

#ifdef __cplusplus
extern "C"
#endif

PV __vec_new(PV op, int n, int sz, PV f)
/*
	allocate a vector of "n" elements of size "sz"
	and initialize each by a call of "f"
	op is zero when operator new(size_t) (which may have been
		supplied by the user) is to be used to allocate storage,
		i.e., for  new T[n]
	op is non-zero when another operator new() has already been used
		to allocate storage, i.e.,  new (<args>) T[n]
		or when the array is static/extern or automatic
*/
{
	int es;
	int opf = (op == 0);

	if (op == 0) // allocate storage if not already done
		op = PV( new char[n*sz] );  // this is the global operator new

	if (op) {  // put (array, count) into the cache
		es = 0;
		if (__cache_key)
			es = __insert_new_array(__cache_key, __cache_count);
		if (es == -1) {
			if (opf)
				delete op;
			return 0;
		}
		__cache_key = op;
		__cache_count = n;
	}

	if (op && f) {
		register char* p = (char*) op;
		register char* lim = p + n*sz;
		register PF fp = PF(f);
		while (p < lim) {
			(*fp) (PV(p),0,0,0,0,0,0,0,0);
			p += sz;
		}
	}
	return PV(op);
}

#ifdef __cplusplus
extern "C"
#endif
void __vec_delete(PV op, int n, int sz, PV f, int del, int)
/*
	clean up a vector at op and optionally delete it
	del is nonzero if the vector is to be deleted, i.e., delete [] p
	del is zero if the vector is on the stack or static
	n is the size if given or the vector is automatic or static
	n is -1 if the vector is dynamic, i.e., delete [] p
	sz is sizeof(T)
	f is the destructor
*/
{
	if (op) {
		int	ans = __cache_key == op ?
			((__cache_key = 0), __cache_count) :
			__remove_old_array(op); // -1 if not found
		if (n == -1) n = ans;  // prefer given size
		if (f) {
			register char* cp = (char*) op;
			register char* p = cp;
			register PFI fp = PFI(f);
			p += n*sz;
			while (p > cp) {
				p -= sz;
				(*fp)(PV(p), 2);  // destroy VBC, don't delete
			}
		}
		if (del) delete op;
	}
}


