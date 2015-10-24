/*ident	"@(#)Block:incl/Block.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#ifndef _BLOCK_C_
#define _BLOCK_C_

#include <stream.h>

// This exists to cut the instantiation dimension from 5 to 2
// by "calling" all the functions that are used for any Block.
template <class T>
void Block<T>::dummy_fcn()
{
	Block<T> foo;
	foo.size(0);
	foo.move(0,0);
	foo.transfer(0,0);
	foo.clear(0,0);
	foo.copy(foo);
	foo.grow(0);
	(void)default_value();
}

template <class T>
unsigned Block<T>::size(unsigned k)
{
	if ( k != n ) 			// Move, even when shrinking, since
		move(new T[k], k);	// the unused space cannot be reused

	return n;
}

template <class T>
Block<T>::~Block()
{
	if (p) delete[] p;
}

template <class T>
Block<T>& Block<T>::operator=(const Block<T>& b)
{
	if ( this != &b ) {
		if (p) delete[] p;
		copy(b);
	}

	return *this;
}


// Clear k elements starting at v

template <class T>
void Block<T>::clear(T* v, unsigned k)
{
	register T* _p = v;
	register T* lim = v + k;
	T* valptr = default_value();

	while ( _p < lim )
		*_p++ = *valptr;
}

// Make this a copy of b

template <class T>
void Block<T>::copy(const Block<T>& b)
{
	// assert (p is 0 or garbage)
	p = new T[b.n];
	if ( p ) {
		n = b.n;
		transfer(b.p, n);
	} else
		n = 0;
}

// Grow this Block by 1.5 until it can contain at least k+1

template <class T>
unsigned Block<T>::grow(unsigned k)
{
	unsigned nn = n;

	if ( nn == 0 )
		nn++;

	while ( nn <= k )
		nn += (nn >> 1) + 1;

	T* np = new T[nn];
	if ( !np ) {
		nn = k+1;
		np = new T[nn];
	}

	move(np, nn);	// takes care of case when np == 0

	return n;
}

// Transfer len (or fewer) elements into this Block.

template <class T>
void Block<T>::transfer(T* source, unsigned len)
{
	register T* plim;
	register T* pp = p;
	register T* q = source;

	if ( n > len ) {
		plim = p + len;
		clear(plim, n - len);
	} else
		plim = p + n;

	while ( pp < plim )
		*pp++ = *q++;
}

// The contents of this Block now live in memory starting at np
// If np is 0, null out this Block.

template <class T>
void Block<T>::move(T* np, unsigned nn)
{
	T* oldp = p;
	unsigned oldn = n;

	p = np;
	if ( np ) {
		n = nn;
		transfer(oldp, oldn);
	} else
		n = 0;

	if (oldp) delete[] oldp;
}

// Exchange the contents of this Block with another Block

template <class T>
void Block<T>::swap(Block<T>& b)
{
	T* bp = b.p;
	unsigned bn = b.n;
	b.p = p;
	b.n = n;
	p = bp;
	n = bn;
}

template <class T>
T* Block<T>::default_value()
{
#ifdef CENTERLINE_CLPP
	static T* default_item;
	if (!default_item) default_item = new T;
	return default_item;
#else
	static T default_item;
	return(&default_item);
#endif
}

template <class T>
ostream& operator<<(ostream& os, const Block<T>& b)
{
	os << '[';

	if ( b.n > 0 )
		os << b.p[0];

	for ( int i=1 ; i<b.n ; i++ )
		os << "," << b.p[i];

	os << ']';
	return os;
}

#endif
