/*ident	"@(#)Block:demos/Blockset.h	3.1" */
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

/*
 *  Blockset - implementation of parameterized Set class.
 *
 *  This implementation is based on Block Algorithm functions.
 *  The code in this example is taken directly from the tutorial
 *  "No more array errors" by John Isner.
 */
#ifndef BLOCKSETH
#define BLOCKSETH

#include <Block.h>
#include <Array_alg.h>
#include <stream.h>
#include <assert.h>

template <class T> class Blocksetiter;

template <class T> class Blockset
{
	friend class Blocksetiter<T>;
private:
	Block<T> b;
	unsigned n;
	void check();
public:

	//  Constructors, destructor

	Blockset() : b(100), n(0) { }
	Blockset(const T& t0) : b(100), n(1) {
		set_insert(t0, &b[0], &b[0]);
	}
	Blockset(const T& t0, const T& t1) : b(100), n(2) {
		set_insert(t0, &b[0], &b[0]);
		set_insert(t1, &b[0], &b[1]);
	}
	Blockset(const T& t0, const T& t1, const T& t2) : b(100), n(3) {
		set_insert(t0, &b[0], &b[0]);
		set_insert(t1, &b[0], &b[1]);
		set_insert(t2, &b[0], &b[2]);
	}
	Blockset(const T& t0, const T& t1, const T& t2, const T& t3):b(100), n(4) {
		set_insert(t0, &b[0], &b[0]);
		set_insert(t1, &b[0], &b[1]);
		set_insert(t2, &b[0], &b[2]);
		set_insert(t3, &b[0], &b[3]);
	}
	Blockset(const Blockset<T>& s) : b(s.b), n(s.n) { }
	~Blockset(){ }

	//  Size

	int size() const {
		return n;
	}
	int size_unique() const {
		return size();
	}
	operator const void*() const {
		return n>0?this:0;
	}

	//  Assignment

	Blockset<T>& operator=(const Blockset<T>& s) {
		b = s.b;
		n = s.n;
		return *this;
	}

	//  Relations

	int operator==(const Blockset<T>& s) const {
		return (n==s.n && mismatch(&b[0], &b[n], &(s.b[0]), &(b[s.n]))==0);
	}
	int operator!=(const Blockset<T>& s) const {
		return !(*this==s);
	}
	int operator<=(const Blockset<T>& s) const {
		return n<=s.n && mismatch(&b[0], &b[n], &(s.b[0]), &(s.b[s.n]))==0;
	}
	int operator<(const Blockset<T>& s) const {
		return n<s.n && mismatch(&b[0], &b[n], &(s.b[0]), &(s.b[s.n]))==0;
	}
	int operator>=(const Blockset<T>& s) const {
		return !(*this<s);
	}
	int operator>(const Blockset<T>& s) const {
		return !(*this<=s);
	}

	//  Membership

	const T* contains(const T& t) const {
		return bin_search(t, &b[0], &b[n]);
	}
	unsigned count(const T& t) const {
		return bin_search(t, &b[0], &b[n])!=0;
	}

	//  Insert and remove

	const T* insert(const T& t, unsigned count=1) {
		const T* result=0;
		if( count>0 )
		{
			b.reserve(n);
			result=set_insert(t, &b[0], &b[(int)n]);
			n++;
		}
		return result;
	}
	unsigned remove(const T& t, unsigned count=1) {
		unsigned result=0;
		if( count>0 && set_remove(t, &b[0], &b[n])<&b[n] ) {
			n--;
			result=1;
		}
		return result;
	}
	unsigned remove_all(const T& t) {
		return remove(t, 1);
	}
	unsigned remove_all() {
		unsigned result = n;
		n = 0;
		return result;
	}

	//  Select an arbitrary element

	const T* select() const
	{
		return random(&b[0], &b[n]);
	}

	//  Blockset algebra

	Blockset<T> operator|(const Blockset<T>& s) const
	{
		Blockset<T> result;
		result.b.reserve(n + s.n - 1);
		result.n = set_union(&b[0], &b[n], &(s.b[0]), &(s.b[s.n]),
			   	     &(result.b[0])) - &(result.b[0]);
		return result;
	}
	Blockset<T> operator-(const Blockset<T>& s) const
	{
		Blockset<T> result;
		result.b.reserve(n-1);
		result.n = set_diff(&b[0], &b[n], &(s.b[0]), &(s.b[s.n]),
			   	     &(result.b[0])) - &(result.b[0]);
		return result;
	}
	Blockset<T> operator&(const Blockset<T>& s) const
	{
		Blockset<T> result;
		result.b.reserve(n + s.n -1);
		result.n = set_inter(&b[0], &b[n], &(s.b[0]), &(s.b[s.n]),
			   	     &(result.b[0])) - &(result.b[0]);
		return result;
	}
	Blockset<T> operator^(const Blockset<T>& s) const
	{
		Blockset<T> result;
		result.b.reserve(n + s.n -1);
		result.n = set_sdiff(&b[0], &b[n], &(s.b[0]), &(s.b[s.n]),
			   	     &(result.b[0])) - &(result.b[0]);
		return result;
	}
};

//  Blockset iterators

template <class T>
class Blocksetiter
{
	const Blockset<T>* sp;
	unsigned i;
public:
	Blocksetiter(const Blockset<T>& s) : sp(&s), i(0) { }
	const T* next()
	{
		return (i<sp->n) ? &(((Blockset<T>*)sp)->b[(int)i++]) : 0;
	}
	void reset()
	{
		i=0;
	}
};

template <class T>
ostream& operator<<(ostream& os, const Blockset<T>& s){
    os << "{";
    Blocksetiter<T> it(s);
    const T* tp;
    int first=1;

    while( tp = it.next() ){
        if( first ){
            first=0;
        }else{
            os << ',';
        }
        os << *tp;
    }
    os << '}';
    return os;
}

#endif
