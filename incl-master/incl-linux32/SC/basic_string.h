/*ident	"@(#) @(#)basic_string.h	1.1.1.2" */
/******************************************************************************
*
* C++ Standard Library
*
* Copyright (c) 1996  Lucent Technologies.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Lucent Technologies.
* The copyright notice above does not evidence any actual or
* intended publication of such source code.
*
******************************************************************************/

#ifndef STDBASICSTRINGH 
#define STDBASICSTRINGH

#include "compiler_type.h"
#include <string.h>
#include <sysent.h>
#include <stdio.h>
#include <iostream.h>
#include <memory.h>
#include <ctype.h>
#if defined(CFRONT_COMPATIBLE_LUCENT)
typedef char *iterator;
typedef char *reverse_iterator;
typedef const char *const_iterator;
typedef const char *const_reverse_iterator;
typedef int bool;
#else
#include <iterator.h>
#include <defalloc.h>
#endif

class ostream;
class istream;

#ifdef NAMESPACES_LUCENT
namespace std {
#endif
template <class charT>
struct string_char_traits {
	typedef charT char_type;
	typedef istream istream_type;
	typedef ostream ostream_type;
	static void assign(charT& c1, const charT &c2);
	static bool eq(const charT &c1, const charT &c2);
	static bool ne(const charT &c1, const charT &c2);
	static bool lt(const charT &c1, const charT &c2);
	static charT eos();
	static istream &char_in(istream &is, charT &a);
	static ostream &char_out(ostream &os, charT a);
	static bool is_del(charT a);
	static int compare(const charT *s1, const charT *s2, size_t n);
	static size_t length(const charT *s);
	static charT *copy(charT *s1, const charT *s2, size_t n);
	static charT *move(charT *s1, const charT *s2, int n);
};
template <class charT>
inline void string_char_traits<charT>::assign(charT& c1, const charT &c2) { c1 = c2; }
template <class charT>
inline bool string_char_traits<charT>::eq(const charT &c1, const charT &c2) { return (c1 == c2); }
template <class charT>
inline bool string_char_traits<charT>::ne(const charT &c1, const charT &c2) { return !(c1 == c2); }
template <class charT>
inline bool string_char_traits<charT>::lt(const charT &c1, const charT &c2) { return (c1 < c2); }
template <class charT>
inline charT string_char_traits<charT>::eos() { return char_type(); }
template <class charT>
inline bool string_char_traits<charT>::is_del(charT a) { return isspace(a); }
template <class charT>
inline int string_char_traits<charT>::compare(const charT *s1, const charT *s2, size_t n) {
	int result = 0;
	size_t i = 0;
	while (i < n) {
		if (!eq(*s1, *s2)) {
			if (lt(*s1, *s2)) result = -1;
			else result = 1;
			break;
		}
		i++; s1++; s2++;
	}
	return result;
}
template <class charT>
inline size_t string_char_traits<charT>::length(const charT *s) {
	size_t result = 0;
	while (!eq(*s++, eos())) result++;
	return (result);
}
template <class charT>
inline charT *string_char_traits<charT>::copy(charT *s1, const charT *s2, size_t n) {
	char_type* result = s1;
	size_t i = 0;
	while (i < n) {
		assign(*s1++, *s2++);
		i++;
	}
	return (result);
}
template <class charT>
inline charT *string_char_traits<charT>::move(charT *s1, const charT *s2, int n) {
	char_type* result = s1;
	if (s1 < s2) copy(s1, s2, n);
	else {
		size_t i = 0;
		s1 += n;
		s2 += n;
		while (i < n) {
			assign(*--s1, *--s2);
			i++;
		}
	}
	return (result);
}

/* specializations of string_char_traits for char and wchar_t */
inline char string_char_traits<char>::
  eos()  { return (char)0; }
inline istream &string_char_traits<char>::
  char_in(istream &is, char &a) { return (is >> a); }
inline ostream &string_char_traits<char>::
  char_out(ostream &os, char a) { return (os << a); }
inline bool string_char_traits<char>::
  is_del(char a) { return (isspace(a)); }
inline int string_char_traits<char>::
  compare(const char *s1, const char *s2, size_t n) {
	return (memcmp(s1, s2, n)); }
inline size_t string_char_traits<char>::
  length(const char *s) { return (strlen(s)); }
inline char *string_char_traits<char>::
  copy(char *s1, const char *s2, size_t n) {
	return ((char *)memcpy(s1, s2, n)); }
inline char *string_char_traits<char>::
  move(char *s1, const char *s2, int n) {
	return ((char *)memcpy(s1, s2, n)); }

inline wchar_t string_char_traits<wchar_t>::
  eos()  { return (wchar_t)0; }
inline istream &string_char_traits<wchar_t>::
  char_in(istream &is, wchar_t &a) {
	char first, second;
	is >> first >> second;
	a = (first << 8) | (second & 0377);
	return is; }
inline ostream &string_char_traits<wchar_t>::
  char_out(ostream &os, wchar_t a) {
	char first, second;
	first = (a >> 8) & 0377;
	second = a & 0377;
	return (os << first << second); }
inline bool string_char_traits<wchar_t>::
  is_del(wchar_t a) {
	return (eq(a, L' ') || eq(a, L'\t') ||
	    eq(a, L'\n') || eq(a, L'\r') || eq(a, eos()));
}
inline wchar_t *string_char_traits<wchar_t>::
  copy(wchar_t *s1, const wchar_t *s2, size_t n) {
	return ((wchar_t *)memcpy(s1, s2, n*sizeof(wchar_t))); }
inline wchar_t *string_char_traits<wchar_t>::
  move(wchar_t *s1, const wchar_t *s2, int n) {
	return ((wchar_t *)memcpy(s1, s2, n*sizeof(wchar_t))); }


class Block_header_std_LUCENT {
	Block_header_std_LUCENT* next;
	friend class Block_pool_std_LUCENT;
};

class Block_pool_std_LUCENT {
	size_t sz;
	unsigned cnt;
	Block_header_std_LUCENT* head;
	Block_pool_std_LUCENT(const Block_pool_std_LUCENT&);	// copy protection
	void operator= (const Block_pool_std_LUCENT&);	// copy protection
public:
	size_t size() const { return sz; }
	unsigned count() const { return cnt; }
	Block_pool_std_LUCENT(size_t);
	~Block_pool_std_LUCENT();
	char *expand();
	void free();
};

class Pool_std_element_header {
	Pool_std_element_header *next;
	friend class Pool_std;
};

class Pool_std: private Block_pool_std_LUCENT {
	size_t elsize;
	unsigned count;
	Pool_std_element_header *head;
	Pool_std(const Pool_std&);		// copy protection
	void operator= (Pool_std&);		// copy protection
	void grow();
public:
	Pool_std(size_t n);
	void *alloc() {
		if (!head)
			grow();
		register Pool_std_element_header *p = head;
		head = p->next;
		return p;
	}
	void free(void *b) {
		register Pool_std_element_header *p = (Pool_std_element_header*) b;
		p->next = head;
		head = p;
	}
	void purge() {
		Block_pool_std_LUCENT::free();
		head = 0;
	}
};

//template <class charT, class traits = string_char_traits<charT>, class Allocator = allocator>
template <class charT, class traits, class Allocator>
class basic_string {		//  the string
public:
	class Srep_LUCENT {         // String Representation
	public:
	    enum { MAXS = 8, MAXL = 64 };
	    static Srep_LUCENT* nullrep_; // nullrep always has refc == 0 and max_size == 0
	    static Pool_std* Reelp;  // Pool for "reel" small strings
	    static Pool_std* SPoolp; // Pool for smallish strings
	    static void initialize() { if (Reelp == 0) doinitialize(); }
	    static void doinitialize();
	    static Srep_LUCENT* nullrep() { initialize(); return nullrep_; }
	    static Srep_LUCENT *new_srep(size_t);
	    static Srep_LUCENT *get_long(size_t);
	    size_t len;
	    size_t max_size;
	    size_t refc;
	    charT str[MAXS];
	    Srep_LUCENT() { len = 0; max_size = 0; refc = 0; }
	    void rcdec() { if(refc && --refc==0)  delete_srep(); }
	    void rcinc() { if(refc) refc++; }
	    void delete_srep();
	};
public:
	typedef	traits traits_type;
	typedef	traits::char_type value_type;
	typedef size_t size_type;
	typedef Allocator::difference_type difference_type;
	typedef Allocator::reference reference;
	typedef Allocator::const_reference const_reference;
	typedef Allocator::pointer pointer;
	typedef Allocator::const_pointer const_pointer;
	typedef Allocator::pointer iterator;
	typedef Allocator::const_pointer const_iterator;
#if !defined(CFRONT_COMPATIBLE_LUCENT)
	typedef reverse_iterator<const_iterator, value_type,
		const_reference, difference_type> const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type,
		reference, difference_type> reverse_iterator;
#endif

	static const size_t npos;

	/* constructors and destructor */
	basic_string() { d = Srep_LUCENT::nullrep(); }
	basic_string(const basic_string<charT,traits,Allocator>& s,
	    size_t = 0, size_t = npos);
	basic_string(const charT*, size_t);
	basic_string(const charT*);
	basic_string(size_t, charT);
	//template <class InputIterator> basic_string(InputIterator, InputIterator);
	~basic_string() { d->rcdec(); }

	/* assignment */
	inline basic_string<charT,traits,Allocator>& operator=(const basic_string<charT,traits,Allocator>&);
	basic_string<charT,traits,Allocator>& operator=(const charT*);
	inline basic_string<charT,traits,Allocator>& operator=(charT);
	
	/* iterators */
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;

	size_t length() const { return d->len; } 
	size_t size() const { return d->len; }
	size_t max_size() const { return d->max_size; } 
	void resize(size_t, charT = traits::eos());
	size_t capacity() const { return d->max_size; }
	void reserve(size_t n) {
		if (n >= d->max_size) reserve_grow(n);
	}

	bool empty() const { return d->len ? 0 : 1; } 

	inline charT operator[](size_t) const;
	reference operator[](size_t);
	inline const_reference at(size_t) const;
	reference at(size_t);

	/* concatenation */
	basic_string<charT,traits,Allocator>&
	    operator+=(const basic_string<charT,traits,Allocator>&);
	basic_string<charT,traits,Allocator>&
	    operator+=(const charT*);
	inline basic_string<charT,traits,Allocator>&
	    operator+=(charT);
	basic_string<charT,traits,Allocator>&
	    append(const basic_string<charT,traits,Allocator> &,
		size_t pos = 0, size_t n = npos);
	basic_string<charT,traits,Allocator>&
	    append(const charT*, size_t len);
	basic_string<charT,traits,Allocator>&
	    append(const charT*);
	basic_string<charT,traits,Allocator>&
	    append(size_t, charT = traits::eos());
	//template <class InputIterator> basic_string<charT,traits,Allocator>& append(InputIterator, InputIterator);

	basic_string<charT,traits,Allocator>& assign(const basic_string<charT,traits,Allocator> &, size_t pos = 0, size_t n = npos);
	basic_string<charT,traits,Allocator>& assign(const charT *, size_t);
	basic_string<charT,traits,Allocator>& assign(const charT *);
	basic_string<charT,traits,Allocator>& assign(size_t, charT = traits::eos());
	//template <class InputIterator> basic_string<charT,traits,Allocator>& assign(InputIterator, InputIterator);
	
	basic_string<charT,traits,Allocator>& insert(size_t, const basic_string<charT,traits,Allocator> &, size_t = 0, size_t = npos);
	basic_string<charT,traits,Allocator>& insert(size_t, const charT *, size_t);
	basic_string<charT,traits,Allocator>& insert(size_t, const charT *);
	basic_string<charT,traits,Allocator>& insert(size_t, size_t, charT = traits::eos());
	iterator insert(iterator, charT = traits::eos());
	iterator insert(iterator, size_t, charT = traits::eos());
	//template <class InputIterator> void insert(iterator, InputIterator, InputIterator);
	
	basic_string<charT,traits,Allocator>& remove(size_t pos = 0, size_t n = npos);
	basic_string<charT,traits,Allocator>& remove(iterator);
	basic_string<charT,traits,Allocator>& remove(iterator, iterator);


	basic_string<charT,traits,Allocator>& replace(size_t, size_t, const basic_string<charT,traits,Allocator> &, size_t = 0, size_t = npos);
	basic_string<charT,traits,Allocator>& replace(size_t, size_t, const charT *, size_t);
	basic_string<charT,traits,Allocator>& replace(size_t, size_t, const charT *);
	basic_string<charT,traits,Allocator>& replace(size_t, size_t, charT);
	basic_string<charT,traits,Allocator>& replace(iterator, iterator, const basic_string<charT,traits,Allocator> &);
	basic_string<charT,traits,Allocator>& replace(iterator, iterator, const charT *, size_t);
	basic_string<charT,traits,Allocator>& replace(iterator, iterator, const charT *);
	basic_string<charT,traits,Allocator>& replace(iterator, iterator, charT);
	//template <class InputIterator> basic_string<charT,traits,Allocator>& replace(iterator, iterator, InputIterator, InputIterator);
	
	size_t copy(charT *, size_t, size_t = 0);
	void swap(basic_string<charT,traits,Allocator> &);
	inline const charT *c_str() const;
	inline const charT *data() const;
	
	/* find functions */
	size_t find(const basic_string<charT,traits,Allocator> &, size_t = 0) const;
	size_t find(const charT *, size_t, size_t) const;
	size_t find(const charT *, size_t = 0) const;
	size_t find(charT, size_t = 0) const;
	size_t rfind(const basic_string<charT,traits,Allocator> &, size_t = npos) const;
	size_t rfind(const charT *, size_t, size_t) const;
	size_t rfind(const charT *, size_t = npos) const;
	size_t rfind(charT, size_t = npos) const;
	size_t find_first_of(const basic_string<charT,traits,Allocator> &, size_t = 0) const;
	size_t find_first_of(const charT *, size_t, size_t) const;
	size_t find_first_of(const charT *, size_t = 0) const;
	size_t find_first_of(charT, size_t = 0) const;
	size_t find_last_of(const basic_string<charT,traits,Allocator> &, size_t = npos) const;
	size_t find_last_of(const charT *, size_t, size_t) const;
	size_t find_last_of(const charT *, size_t = npos) const;
	size_t find_last_of(charT, size_t = npos) const;
	size_t find_first_not_of(const basic_string<charT,traits,Allocator> &, size_t = 0) const;
	size_t find_first_not_of(const charT *, size_t, size_t) const;
	size_t find_first_not_of(const charT *, size_t = 0) const;
	size_t find_first_not_of(charT, size_t = 0) const;
	size_t find_last_not_of(const basic_string<charT,traits,Allocator> &, size_t = npos) const;
	size_t find_last_not_of(const charT *, size_t, size_t) const;
	size_t find_last_not_of(const charT *, size_t = npos) const;
	size_t find_last_not_of(charT, size_t = npos) const;

	basic_string<charT,traits,Allocator> substr(size_t = 0, size_t = npos) const;

	int compare(const basic_string<charT,traits,Allocator> &, size_t = 0, size_t = npos) const;
	int compare(const charT *, size_t, size_t) const;
	int compare(const charT *, size_t = 0) const;
	
private:
	Srep_LUCENT *d;
	basic_string<charT,traits,Allocator>(Srep_LUCENT* r) { d = r; }
	basic_string<charT,traits,Allocator>& newcopy(charT);
	void reserve_grow(int);
	inline void uniq();
public:
	/* ADDITION */
	friend basic_string<charT,traits,Allocator> operator+(const basic_string<charT,traits,Allocator>&, const basic_string<charT,traits,Allocator>&);
	friend basic_string<charT,traits,Allocator> operator+(const basic_string<charT,traits,Allocator>&, const charT*);
	friend basic_string<charT,traits,Allocator> operator+(const basic_string<charT,traits,Allocator>&, charT);
	friend basic_string<charT,traits,Allocator> operator+(const charT*, const basic_string<charT,traits,Allocator>&);
	friend basic_string<charT,traits,Allocator> operator+(charT, const basic_string<charT,traits,Allocator>&);
	//int nrefs() const { return d->refc; }
	//const Srep_LUCENT *rep() const { return d; }
	/* COMPARISON OPERATORS */
	friend inline bool operator==(const basic_string<charT,traits,Allocator>&, const basic_string<charT,traits,Allocator>&);
	friend inline bool operator==(const basic_string<charT,traits,Allocator>&, const charT*);
#if 0
	friend inline bool operator!=(const basic_string<charT,traits,Allocator>&, const basic_string<charT,traits,Allocator>&);
	friend inline bool operator!=(const basic_string<charT,traits,Allocator>&, const charT*);
#endif
	/* INPUT/OUTPUT */
	friend /*inline*/ ostream& operator<<(ostream&, const basic_string<charT,traits,Allocator>&);
	friend istream& operator>>(istream&, basic_string<charT,traits,Allocator>&);

	/* special function just for basic_string<char,...> 
	   and basic_char<wchar_t,...> at present */
	friend istream& getline(istream&,
	    basic_string<charT,traits,Allocator> &,
	    charT delim);
};



// inline functions

template <class charT, class traits, class Allocator>
inline
const charT *basic_string<charT,traits,Allocator>::data() const {
	if (d->len == 0) return 0;
	else {
		if (d->len >= d->max_size && d->len)
			((basic_string<charT,traits,Allocator> *) this)->reserve_grow(d->len + 1);
		*(d->str + d->len) = traits::eos(); 
		return (d->str);
	}
}
template <class charT, class traits, class Allocator>
inline
const charT *basic_string<charT,traits,Allocator>::c_str() const {
	if (d->len >= d->max_size && d->len)
		((basic_string<charT,traits,Allocator> *) this)->reserve_grow(d->len + 1);
	*(d->str + d->len) = traits::eos(); 
	return (d->str);
}

template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>&
basic_string<charT,traits,Allocator>::operator=(const basic_string<charT,traits,Allocator>& r) {
        r.d->rcinc();
        d->rcdec();
        d = r.d;
        return *this;
}

template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>&
basic_string<charT,traits,Allocator>::operator=(charT c) {
	if(d->refc!=1) {
		d->rcdec();
		d = Srep_LUCENT::new_srep(1);
	}
	d->str[0] = c;
	d->len = 1;
	return *this;
}
template <class charT, class traits, class Allocator>
inline bool
operator==(const basic_string<charT,traits,Allocator>& t, const basic_string<charT,traits,Allocator>& s) {
	register int dlen = t.d->len;
	register bool retval;
	if ((dlen != s.d->len) ||
	    (dlen && !traits::eq(t.d->str[0], s.d->str[0])) ||
	    (dlen>1 && !traits::eq(t.d->str[1], s.d->str[1])))
		retval = 0;
	else
		retval = !traits::compare(t.d->str, s.d->str, dlen);
	return retval;
}
template <class charT, class traits, class Allocator>
inline bool
operator==(const basic_string<charT,traits,Allocator>& t, const charT* s) {
	register int dlen = t.d->len;
	register bool retval;
	if (!s || (dlen && !traits::eq(t.d->str[0], s[0])) ||
	          (dlen>1 &&
	           !traits::eq(s[0], traits::eos()) &&
	           !traits::eq(t.d->str[1], s[1])))
		retval = 0;
	else
		retval = (dlen != traits::length(s)) ? 0 : !traits::compare(t.d->str, s, dlen);
	return retval;
}

template <class charT, class traits, class Allocator>
inline bool 
operator==(const charT* s, const basic_string<charT,traits,Allocator>& t) { return t==s; }

#if 0
/* The operator!=(), operator<=(), operator>(), and operator>=() functions
   are defined as parameterized functions in the STL header file function.h,
   so these definitions are unnecessary (and in fact, will cause a
   compile-time error. */
template <class charT, class traits, class Allocator>
inline bool
operator!=(const basic_string<charT,traits,Allocator>& t, const basic_string<charT,traits,Allocator>& s) {
	register int dlen = t.d->len;
	register bool retval;
	if ((dlen != s.d->len) ||
	    (dlen && !traits::eq(t.d->str[0], s.d->str[0])) ||
	    (dlen>1 && !traits::eq(t.d->str[1], s.d->str[1])))
		retval = 1;
	else
		retval = traits::compare(t.d->str, s.d->str, dlen);
	return retval;
}
template <class charT, class traits, class Allocator>
inline bool
operator!=(const basic_string<charT,traits,Allocator>& t, const charT* s) {
	register int dlen = t.d->len;
	register bool retval;
	if (!s ||
	    (dlen && !traits::eq(t.d->str[0],s[0])) ||
	    (dlen>1 &&
	     !traits::eq(s[0],traits::eos()) &&
	     !traits::eq(t.d->str[1],s[1])))
		retval = 1;
	else
		retval = (dlen != traits::length(s)) ? 1 : traits::compare(t.d->str, s, dlen);
	return retval;
}

template <class charT, class traits, class Allocator>
inline bool 
operator!=(const charT* s, const basic_string<charT,traits,Allocator>& t) { return t!=s; }
template <class charT, class traits, class Allocator>
inline bool 
operator>=(const basic_string<charT,traits,Allocator>& s, const basic_string<charT,traits,Allocator>& t) { return s.compare(t)>=0; }
template <class charT, class traits, class Allocator>
inline bool 
operator>=(const basic_string<charT,traits,Allocator>& t, const charT* s) { return t.compare(s)>=0; }
template <class charT, class traits, class Allocator>
inline bool 
operator>=(const charT* s, const basic_string<charT,traits,Allocator>& t) { return t.compare(s)<=0; }
template <class charT, class traits, class Allocator>
inline bool 
operator>(const basic_string<charT,traits,Allocator>& s, const basic_string<charT,traits,Allocator>& t) { return s.compare(t)>0; }
template <class charT, class traits, class Allocator>
inline bool 
operator>(const basic_string<charT,traits,Allocator>& t, const charT* s) { return t.compare(s)>0; }
template <class charT, class traits, class Allocator>
inline bool 
operator>(const charT* s, const basic_string<charT,traits,Allocator>& t) { return t.compare(s)<0; }
template <class charT, class traits, class Allocator>
inline bool 
operator<=(const basic_string<charT,traits,Allocator>& s, const basic_string<charT,traits,Allocator>& t) { return s.compare(t)<=0; }
template <class charT, class traits, class Allocator>
inline bool 
operator<=(const basic_string<charT,traits,Allocator>& t, const charT* s) { return t.compare(s)<=0; }
template <class charT, class traits, class Allocator>
inline bool 
operator<=(const charT* s, const basic_string<charT,traits,Allocator>& t) { return t.compare(s)>=0; }
#endif
template <class charT, class traits, class Allocator>

inline bool
operator<(const basic_string<charT,traits,Allocator>& s, const basic_string<charT,traits,Allocator>& t) { return s.compare(t)<0; }
template <class charT, class traits, class Allocator>

inline bool
operator<(const basic_string<charT,traits,Allocator>& t, const charT* s) { return t.compare(s)<0; }
template <class charT, class traits, class Allocator>

inline bool
operator<(const charT* s, const basic_string<charT,traits,Allocator>& t) { return t.compare(s)>0; }

template <class charT, class traits, class Allocator>
inline charT
basic_string<charT,traits,Allocator>::operator[](size_t i) const {
	return d->str[i];
}
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::const_reference
basic_string<charT,traits,Allocator>::at(size_t i) const {
	return d->str[i];
}

template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>&
basic_string<charT,traits,Allocator>::operator+=(charT c) {
	int append_to_current = 1;
	if (d->refc != 1 || d->len == d->max_size - 1)
		append_to_current = 0;
	else
		traits::assign(d->str[d->len++], c);
	basic_string<charT,traits,Allocator> &s = append_to_current ? *this : newcopy(c);
	return s;
}

template <class charT, class traits, class Allocator>
inline void
basic_string<charT,traits,Allocator>::uniq() {
	register Srep_LUCENT* x;
	if(d->refc > 1) {
		x = Srep_LUCENT::new_srep(d->len);
		traits::copy(x->str, d->str, d->len);
		d->rcdec();
		d = x;
	}
}
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::iterator
basic_string<charT,traits,Allocator>::begin() {
	uniq(); return d->str; }
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::const_iterator
basic_string<charT,traits,Allocator>::begin() const {
	return d->str; }
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::iterator
basic_string<charT,traits,Allocator>::end() {
	uniq(); return d->str + d->len; }
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::const_iterator
basic_string<charT,traits,Allocator>::end() const {
	return d->str + d->len; }
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::reverse_iterator
basic_string<charT,traits,Allocator>::rbegin() {
	uniq(); return d->str + d->len; }
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::const_reverse_iterator
basic_string<charT,traits,Allocator>::rbegin() const {
	return d->str + d->len; }
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::reverse_iterator
basic_string<charT,traits,Allocator>::rend() {
	uniq(); return d->str; }
template <class charT, class traits, class Allocator>
inline basic_string<charT,traits,Allocator>::const_reverse_iterator
basic_string<charT,traits,Allocator>::rend() const {
	return d->str; }

typedef basic_string<char,string_char_traits<char>,allocator<char> > string;
typedef basic_string<wchar_t,string_char_traits<wchar_t>,allocator<wchar_t> > wstring;

extern istream& getline(istream&, string&, char delim = '\n');
extern istream& getline(istream&, wstring&, wchar_t delim = L'\n');

#ifdef NAMESPACES_LUCENT
}
#endif

#if defined(EXCEPTION_LUCENT)
#include <stringexcept.h>
#endif

#endif
