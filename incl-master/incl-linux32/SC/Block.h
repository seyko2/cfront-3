/*ident	"@(#)Block:incl/Block.h	3.1" */
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

#ifndef BLOCK_H
#define BLOCK_H
#ifdef __GNUG__
#pragma interface
#endif

#include <new.h>

class ostream;

template <class T> class Block {
	void dummy_fcn();

public:
	Block() : n(0), p(0) { if (n != 0) dummy_fcn(); }
	Block(unsigned k) : n(0), p(0) { if (n != 0) dummy_fcn(); size(k); }
	Block(const Block<T>& b) { copy(b); }
	~Block();

	Block<T>& operator=(const Block<T>&);

	unsigned size() const { return n; }
	unsigned size(unsigned);

	operator T*() { return p; }
	operator const T*() const { return p; }

	T* end() { return p + n; }
	const T* end() const { return p + n; }

	T& operator[](int i) { return p[i]; }
	const T& operator[](int i) const { return p[i]; }
	T& operator[](unsigned i) { return p[i]; }
	const T& operator[](unsigned i) const { return p[i]; }

	int reserve(unsigned k) { return k<n || grow(k); }
	void swap(Block<T>& b);

#if defined(__SUNPRO_CC) || defined(__GNUG__)
	friend ostream& operator<<(ostream& os, const Block<T>& b);
#else
	template <class Type>
	friend ostream& operator<<(ostream& os, const Block<Type>& b);
#endif

private:
	T* p;
	unsigned n;

	void move(T*, unsigned);
	void transfer(T*, unsigned);
	void clear(T*, unsigned);
	void copy(const Block<T>&);
	unsigned grow(unsigned);
	static T* default_value();
};

#if (defined(__edg_att_40) || defined(__GNUG__)) && !defined(__IMPLICIT_INCLUDE)
#include <Block.c>
#endif
#endif
