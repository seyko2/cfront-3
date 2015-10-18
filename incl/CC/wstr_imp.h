/*ident	"@(#) @(#)wstr_imp.h	1.1.1.2" */
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

#ifndef STDWSTRINGH 
#define STDWSTRINGH

#include "compiler_type.h"
#include <string.h>
#if !defined(COMPILER_IS_PC_LUCENT)
#include <sysent.h>
#endif

#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#if defined(CFRONT_COMPATIBLE_LUCENT)
//typedef wchar_t *iterator;
//typedef wchar_t *reverse_iterator;
//typedef const wchar_t *const_iterator;
//typedef const wchar_t *const_reverse_iterator;
//typedef int bool;
#else
#include <iterator.h>
#include <defalloc.h>
#endif

class ostream;
class istream;

#ifdef NAMESPACES_LUCENT
namespace std {
#endif
class Pool_std;

inline void assign_wchar_t(wchar_t& c1, const wchar_t &c2) { c1 = c2; }
inline bool eq_wchar_t(const wchar_t &c1, const wchar_t &c2) { return (c1 == c2); }
inline bool ne_wchar_t(const wchar_t &c1, const wchar_t &c2) { return !(c1 == c2); }
inline bool lt_wchar_t(const wchar_t &c1, const wchar_t &c2) { return (c1 < c2); }
inline wchar_t eos_wchar_t()  { return (wchar_t)0; }
extern
int compare_wchar_t(const wchar_t *s1, const wchar_t *s2, size_t n);
extern
size_t length_wchar_t(const wchar_t *s);
extern
istream &char_in_wchar_t(istream &is, wchar_t &a);
extern
ostream &char_out_wchar_t(ostream &os, wchar_t a);
inline bool is_del_wchar_t(wchar_t a) {
	int first, second;
	first = a & 0177400;
	second = a & 0377;
	return (first == 0 && isspace(second));
}
inline wchar_t *copy_wchar_t(wchar_t *s1, const wchar_t *s2, size_t n) {
	return ((wchar_t *)memcpy(s1, s2, n*sizeof(wchar_t))); }
inline wchar_t *move_wchar_t(wchar_t *s1, const wchar_t *s2, int n) {
	return ((wchar_t *)memcpy(s1, s2, n*sizeof(wchar_t))); }


struct string_wchar_traits_LUCENT {
	typedef wchar_t char_type;
	typedef istream istream_type;
	typedef ostream ostream_type;
	static void assign(wchar_t& c1, const wchar_t &c2) { c1 = c2; }
	static bool eq(const wchar_t &c1, const wchar_t &c2) { return (c1 == c2); }
	static bool ne(const wchar_t &c1, const wchar_t &c2) { return (c1 != c2); }
	static bool lt(const wchar_t &c1, const wchar_t &c2) { return (c1 < c2); }
	static wchar_t eos() { return (wchar_t)0; }
	static istream &char_in(istream &is, wchar_t &a) { return (char_in_wchar_t(is, a)); }
	static ostream &char_out(ostream &os, wchar_t a) { return (char_out_wchar_t(os, a)); }
	static bool is_del(wchar_t a) { return (is_del_wchar_t(a)); }
	static int compare(const wchar_t *s1, const wchar_t *s2, size_t n) {
		return (compare_wchar_t(s1, s2, n)); }
	static size_t length(const wchar_t *s) { return (length_wchar_t(s)); }
	static wchar_t *copy(wchar_t *s1, const wchar_t *s2, size_t n) {
		return (copy_wchar_t(s1, s2, n)); }
	static wchar_t *move(wchar_t *s1, const wchar_t *s2, int n) {
		return (move_wchar_t(s1, s2, n)); }
};

class wstring {		//  the string
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
	    wchar_t str[MAXS];
	    Srep_LUCENT() { len = 0; max_size = 0; refc = 0; }
	    void rcdec() { if(refc && --refc==0)  delete_srep(); }
	    void rcinc() { if(refc) refc++; }
	    void delete_srep();
	};
public:
	typedef	string_wchar_traits_LUCENT traits_type;
	typedef	string_wchar_traits_LUCENT::char_type value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef wchar_t &reference;
	typedef const wchar_t &const_reference;
	typedef wchar_t *pointer;
	typedef const wchar_t *const_pointer;
	typedef wchar_t *iterator;
	typedef const wchar_t *const_iterator;
#if !defined(CFRONT_COMPATIBLE_LUCENT)
	typedef reverse_iterator<const_iterator, value_type,
		const_reference, difference_type> const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type,
		reference, difference_type> reverse_iterator;
#else
	typedef const wchar_t *const_reverse_iterator;
	typedef wchar_t *reverse_iterator;
#endif
	static const size_t npos;

	/* constructors and destructor */
	wstring() { d = Srep_LUCENT::nullrep(); }
	wstring(const wstring& s,
	    size_t = 0, size_t = npos);
	wstring(const wchar_t*, size_t);
	wstring(const wchar_t*);
	wstring(size_t, wchar_t);
	~wstring() { d->rcdec(); }

	/* assignment */
	inline wstring& operator=(const wstring&);
	wstring& operator=(const wchar_t*);
	inline wstring& operator=(wchar_t);
	
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
	void resize(size_t, wchar_t = eos_wchar_t());
	size_t capacity() const { return d->max_size; }
	void reserve(size_t n) {
		if (n >= d->max_size) reserve_grow(n);
	}

	bool empty() const { return d->len ? 0 : 1; } 

	inline wchar_t operator[](size_t) const;
	reference operator[](size_t);
	inline const_reference at(size_t) const;
	reference at(size_t);

	/* concatenation */
	wstring&
	    operator+=(const wstring&);
	wstring&
	    operator+=(const wchar_t*);
	inline wstring&
	    operator+=(wchar_t);
	wstring&
	    append(const wstring &,
		size_t pos = 0, size_t n = npos);
	wstring&
	    append(const wchar_t*, size_t len);
	wstring&
	    append(const wchar_t*);
	wstring&
	    append(size_t, wchar_t = eos_wchar_t());

	wstring& assign(const wstring &, size_t pos = 0, size_t n = npos);
	wstring& assign(const wchar_t *, size_t);
	wstring& assign(const wchar_t *);
	wstring& assign(size_t, wchar_t = eos_wchar_t());
	
	wstring& insert(size_t, const wstring &, size_t = 0, size_t = npos);
	wstring& insert(size_t, const wchar_t *, size_t);
	wstring& insert(size_t, const wchar_t *);
	wstring& insert(size_t, size_t, wchar_t = eos_wchar_t());
	iterator insert(iterator, wchar_t = eos_wchar_t());
	iterator insert(iterator, size_t, wchar_t = eos_wchar_t());
	
	wstring& remove(size_t pos = 0, size_t n = npos);
	wstring& remove(iterator);
	wstring& remove(iterator, iterator);


	wstring& replace(size_t, size_t, const wstring &, size_t = 0, size_t = npos);
	wstring& replace(size_t, size_t, const wchar_t *, size_t);
	wstring& replace(size_t, size_t, const wchar_t *);
	wstring& replace(size_t, size_t, wchar_t);
	wstring& replace(iterator, iterator, const wstring &);
	wstring& replace(iterator, iterator, const wchar_t *, size_t);
	wstring& replace(iterator, iterator, const wchar_t *);
	wstring& replace(iterator, iterator, wchar_t);
	
	size_t copy(wchar_t *, size_t, size_t = 0);
	void swap(wstring &);
	inline const wchar_t *c_str() const;
	inline const wchar_t *data() const;
	
	/* find functions */
	size_t find(const wstring &, size_t = 0) const;
	size_t find(const wchar_t *, size_t, size_t) const;
	size_t find(const wchar_t *, size_t = 0) const;
	size_t find(wchar_t, size_t = 0) const;
	size_t rfind(const wstring &, size_t = npos) const;
	size_t rfind(const wchar_t *, size_t, size_t) const;
	size_t rfind(const wchar_t *, size_t = npos) const;
	size_t rfind(wchar_t, size_t = npos) const;
	size_t find_first_of(const wstring &, size_t = 0) const;
	size_t find_first_of(const wchar_t *, size_t, size_t) const;
	size_t find_first_of(const wchar_t *, size_t = 0) const;
	size_t find_first_of(wchar_t, size_t = 0) const;
	size_t find_last_of(const wstring &, size_t = npos) const;
	size_t find_last_of(const wchar_t *, size_t, size_t) const;
	size_t find_last_of(const wchar_t *, size_t = npos) const;
	size_t find_last_of(wchar_t, size_t = npos) const;
	size_t find_first_not_of(const wstring &, size_t = 0) const;
	size_t find_first_not_of(const wchar_t *, size_t, size_t) const;
	size_t find_first_not_of(const wchar_t *, size_t = 0) const;
	size_t find_first_not_of(wchar_t, size_t = 0) const;
	size_t find_last_not_of(const wstring &, size_t = npos) const;
	size_t find_last_not_of(const wchar_t *, size_t, size_t) const;
	size_t find_last_not_of(const wchar_t *, size_t = npos) const;
	size_t find_last_not_of(wchar_t, size_t = npos) const;

	wstring substr(size_t = 0, size_t = npos) const;

	int compare(const wstring &, size_t = 0, size_t = npos) const;
	int compare(const wchar_t *, size_t, size_t) const;
	int compare(const wchar_t *, size_t = 0) const;
	
private:
	Srep_LUCENT *d;
	wstring(Srep_LUCENT* r) { d = r; }
	wstring& newcopy(wchar_t);
	void reserve_grow(int);
	inline void uniq();
public:
	/* ADDITION */
	friend wstring operator+(const wstring&, const wstring&);
	friend wstring operator+(const wstring&, const wchar_t*);
	friend wstring operator+(const wstring&, wchar_t);
	friend wstring operator+(const wchar_t*, const wstring&);
	friend wstring operator+(wchar_t, const wstring&);
	//int nrefs() const { return d->refc; }
	//const Srep_LUCENT *rep() const { return d; }
	/* COMPARISON OPERATORS */
	friend inline bool operator==(const wstring&, const wstring&);
	friend inline bool operator==(const wstring&, const wchar_t*);
	friend inline bool operator!=(const wstring&, const wstring&);
	friend inline bool operator!=(const wstring&, const wchar_t*);
	/* INPUT/OUTPUT */
	friend /*inline*/ ostream& operator<<(ostream&, const wstring&);
	friend istream& operator>>(istream&, wstring&);

	/* special function just for basic_string<char,...> at present */
	friend istream& getline(istream&, wstring&, wchar_t delim = 012);

#if defined(COMPILER_IS_PC_LUCENT)
#if !defined(_MSC_VER) || _MSC_VER < 1000
private:
	static ostream &char_out(ostream &os, wchar_t a) {
	    return char_out_wchar_t(os, a); }
	static bool is_del(wchar_t a) { return is_del_wchar_t(a); }
	static wchar_t *copy(wchar_t *s1, const wchar_t *s2, size_t n) {
	    return copy_wchar_t(s1, s2, n); }
	static size_t length(const wchar_t *s) {
	    return length_wchar_t(s); }
#endif
#endif
};



// inline functions

inline
const wchar_t *wstring::data() const {
	if (d->len == 0) return 0;
	else {
		if (d->len >= d->max_size && d->len)
			((wstring *) this)->reserve_grow(d->len + 1);
		*(d->str + d->len) = eos_wchar_t(); 
		return (d->str);
	}
}
inline
const wchar_t *wstring::c_str() const {
	if (d->len >= d->max_size && d->len)
		((wstring *) this)->reserve_grow(d->len + 1);
	*(d->str + d->len) = eos_wchar_t(); 
	return (d->str);
}

inline wstring&
wstring::operator=(const wstring& r) {
        r.d->rcinc();
        d->rcdec();
        d = r.d;
        return *this;
}

inline wstring&
wstring::operator=(wchar_t c) {
	if(d->refc!=1) {
		d->rcdec();
		d = Srep_LUCENT::new_srep(1);
	}
	d->str[0] = c;
	d->len = 1;
	return *this;
}
inline bool
operator==(const wstring& t, const wstring& s) {
	register int dlen = t.d->len;
	register bool retval;
	if ((dlen != s.d->len) ||
	    (dlen && t.d->str[0] != s.d->str[0]) ||
	    (dlen>1 && t.d->str[1] != s.d->str[1]))
		retval = 0;
	else
		retval = !compare_wchar_t(t.d->str, s.d->str, dlen);
	return retval;
}
inline bool
operator==(const wstring& t, const wchar_t* s) {
	register int dlen = t.d->len;
	register bool retval;
	if (!s || (dlen && t.d->str[0] != s[0]) ||
	          (dlen>1 &&
	           s[0] != eos_wchar_t() &&
	           t.d->str[1] != s[1]))
		retval = 0;
	else
		retval = (dlen != length_wchar_t(s)) ? 0 : !compare_wchar_t(t.d->str, s, dlen);
	return retval;
}

inline bool 
operator==(const wchar_t* s, const wstring& t) { return t==s; }

inline bool
operator!=(const wstring& t, const wstring& s) {
	register int dlen = t.d->len;
	register bool retval;
	if ((dlen != s.d->len) ||
	    (dlen && t.d->str[0] != s.d->str[0]) ||
	    (dlen>1 && t.d->str[1] != s.d->str[1]))
		retval = 1;
	else
		retval = compare_wchar_t(t.d->str, s.d->str, dlen);
	return retval;
}
inline bool
operator!=(const wstring& t, const wchar_t* s) {
	register int dlen = t.d->len;
	register bool retval;
	if (!s ||
	    (dlen && t.d->str[0] != s[0]) ||
	    (dlen>1 &&
	     s[0] != eos_wchar_t() &&
	     t.d->str[1] != s[1]))
		retval = 1;
	else
		retval = (dlen != length_wchar_t(s)) ? 1 : compare_wchar_t(t.d->str, s, dlen);
	return retval;
}

inline bool 
operator!=(const wchar_t* s, const wstring& t) { return t!=s; }
inline bool 
operator>=(const wstring& s, const wstring& t) { return s.compare(t)>=0; }
inline bool 
operator>=(const wstring& t, const wchar_t* s) { return t.compare(s)>=0; }
inline bool 
operator>=(const wchar_t* s, const wstring& t) { return t.compare(s)<=0; }
inline bool 
operator>(const wstring& s, const wstring& t) { return s.compare(t)>0; }
inline bool 
operator>(const wstring& t, const wchar_t* s) { return t.compare(s)>0; }
inline bool 
operator>(const wchar_t* s, const wstring& t) { return t.compare(s)<0; }
inline bool 
operator<=(const wstring& s, const wstring& t) { return s.compare(t)<=0; }
inline bool 
operator<=(const wstring& t, const wchar_t* s) { return t.compare(s)<=0; }
inline bool 
operator<=(const wchar_t* s, const wstring& t) { return t.compare(s)>=0; }
inline bool 
operator<(const wstring& s, const wstring& t) { return s.compare(t)<0; }
inline bool 
operator<(const wstring& t, const wchar_t* s) { return t.compare(s)<0; }
inline bool 
operator<(const wchar_t* s, const wstring& t) { return t.compare(s)>0; }

inline wchar_t
wstring::operator[](size_t i) const {
	return d->str[i];
}
inline wstring::const_reference
wstring::at(size_t i) const {
	return d->str[i];
}

inline wstring&
wstring::operator+=(wchar_t c) {
	int append_to_current = 1;
	if (d->refc != 1 || d->len == d->max_size - 1)
		append_to_current = 0;
	else
		assign_wchar_t(d->str[d->len++], c);
	wstring &s = append_to_current ? *this : newcopy(c);
	return s;
}

inline void
wstring::uniq() {
	register Srep_LUCENT* x;
	if(d->refc > 1) {
		x = Srep_LUCENT::new_srep(d->len);
		copy_wchar_t(x->str, d->str, d->len);
		d->rcdec();
		d = x;
	}
}
inline wstring::iterator wstring::begin() { uniq(); return d->str; }
inline wstring::const_iterator wstring::begin() const { return d->str; }
inline wstring::iterator wstring::end() { uniq(); return d->str + d->len; }
inline wstring::const_iterator wstring::end() const { return d->str + d->len; }
inline wstring::reverse_iterator wstring::rbegin() { uniq(); return d->str + d->len; }
inline wstring::const_reverse_iterator wstring::rbegin() const { return d->str + d->len; }
inline wstring::reverse_iterator wstring::rend() { uniq(); return d->str; }
inline wstring::const_reverse_iterator wstring::rend() const { return d->str; }

#ifdef NAMESPACES_LUCENT
}
#endif

#endif
