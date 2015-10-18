/*ident	"@(#)cls4:src/Block.h	1.4" */
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
#ifndef BLOCK_H
#define BLOCK_H 1
#ifndef GENERICH
#include <generic.h>
#endif

#define Block(T)name2(Block_,T)

#define Blockdeclare(T)							\
									\
class Block(T) {							\
public:									\
	unsigned size() const { return n; }				\
	unsigned size(unsigned k) { move(new T[k], k); return n; }	\
	Block(T)() { n = 0; p = 0; }					\
	Block(T)(unsigned k) { n = 0; p = 0; size(k); }			\
	Block(T)(const Block(T)& b) { copy(b); }			\
	~Block(T)() { delete[/*n*/] p; }					\
	Block(T)& operator=(const Block(T)& b) {			\
		delete[/*n*/] p;						\
		copy(b);						\
		return *this;						\
	}								\
	operator T*() { return p; }					\
	operator const T*() const { return p; }				\
	T* end() { return p + n; }					\
	const T* end() const { return p + n; }				\
	T& operator[](int i) { return p[i]; }				\
	const T& operator[](int i) const { return p[i]; }		\
	int reserve(unsigned k) { return k<n || grow(k); }		\
	void swap(Block(T)& b);						\
private:								\
	T* p;								\
	unsigned n;							\
	void move(T*, unsigned);					\
	void transfer(T*, unsigned);					\
	void clear(T*, unsigned);					\
	void copy(const Block(T)&);					\
	unsigned grow(unsigned);					\
};									\
									\

#define Blockimplement(T)						\
									\
static T name2(Block(T),_def);						\
									\
/* Clear k elements starting at v */					\
void									\
Block(T)::clear(T* v, unsigned k)					\
{									\
	register T* p = v;						\
	register T* lim = v + k;					\
	while (p < lim)							\
		*p++ = name2(Block(T),_def);				\
}									\
									\
/* This Block was uninitialized, is now a copy of b */			\
void									\
Block(T)::copy(const Block(T)& b)					\
{									\
	p = new T[b.n];							\
	if (p) {							\
		n = b.n;						\
		transfer(b.p, n);					\
	} else								\
		n = 0;							\
}									\
									\
/* Grow this Block by 1.5 until it can contain at least k+1 */		\
unsigned								\
Block(T)::grow(unsigned k)						\
{									\
	unsigned nn = n;						\
	if (nn == 0)							\
		nn++;							\
	while (nn <= k)							\
		nn += (nn >> 1) + 1;					\
	T* np = new T[nn];						\
	if (!np) {							\
		nn = k+1;						\
		np = new T[nn];						\
	}								\
	move(np, nn);							\
	return n;							\
}									\
									\
/* Transfer len (or fewer) elements into this Block. */			\
void									\
Block(T)::transfer(T* source, unsigned len)				\
{									\
	register T* plim;						\
	register T* pp = p;						\
	register T* q = source;						\
									\
	if (n > len) {							\
		plim = p + len;						\
		clear(plim, n - len);					\
	} else								\
		plim = p + n;						\
									\
	while (pp < plim)						\
		*pp++ = *q++;						\
}									\
									\
/*									\
 * The contents of this Block now live in memory starting at np		\
 * If np is 0, null out this Block.					\
 */									\
void									\
Block(T)::move(T* np, unsigned nn)					\
{									\
	T* oldp = p;							\
	unsigned oldn = n;						\
	p = np;								\
	if (np) {							\
		n = nn;							\
		transfer(oldp, oldn);					\
	} else								\
		n = 0;							\
	delete[/*oldn*/] oldp;						\
}									\
									\
/* Exchange the contents of this Block with another Block */		\
void									\
Block(T)::swap(Block(T)& b)						\
{									\
	T* bp = b.p;							\
	unsigned bn = b.n;						\
	b.p = p;							\
	b.n = n;							\
	p = bp;								\
	n = bn;								\
}									\
									\


#endif
