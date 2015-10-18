/*ident	"@(#) @(#)str_imp.h	1.1.1.2" */
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

#ifndef STDSTRINGH 
#define STDSTRINGH

#include "compiler_type.h"
#include <string.h>
#if !defined(COMPILER_IS_PC_LUCENT)
#include <sysent.h>
#endif

#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#if defined(CFRONT_COMPATIBLE_LUCENT)
typedef char *reverse_iterator;
typedef const char *const_reverse_iterator;
#include "std_bool.h"
#else
#include <iterator.h>
#endif

class ostream;
class istream;

#ifdef NAMESPACES_LUCENT
namespace std {
#endif
class Pool_std;

struct string_char_traits_LUCENT {
	typedef char char_type;
	typedef istream istream_type;
	typedef ostream ostream_type;
	static void assign(char& c1, const char &c2) { c1 = c2; }
	static bool eq(const char &c1, const char &c2) { return (c1 == c2); }
	static bool ne(const char &c1, const char &c2) { return (c1 != c2); }
	static bool lt(const char &c1, const char &c2) { return (c1 < c2); }
	static char eos() { return (char)0; }
//	static istream &char_in(istream &is, char &a) { return (is >> a); }
//	static ostream &char_out(ostream &os, char a) { return (os << a); }
	static bool is_del(char a) { return (isspace(a)); }
	static int compare(const char *s1, const char *s2, size_t n) {
		return (strncmp(s1, s2, n)); }
	static size_t length(const char *s) { return (strlen(s)); }
	static char *copy(char *s1, const char *s2, size_t n) {
		return (strncpy(s1, s2, n)); }
	static char *move(char *s1, const char *s2, int n) {
		return (strncpy(s1, s2, n)); }
};

class string {		//  the string
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
	    char str[MAXS];
	    Srep_LUCENT() { len = 0; max_size = 0; refc = 0; }
	    void rcdec() { if(refc && --refc==0)  delete_srep(); }
	    void rcinc() { if(refc) refc++; }
	    void delete_srep();
	};

public:
	typedef	string_char_traits_LUCENT traits_type;
	typedef	string_char_traits_LUCENT::char_type value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef char &reference;
	typedef const char &const_reference;
	typedef char *pointer;
	typedef const char *const_pointer;
	typedef char *iterator;
	typedef const char *const_iterator;
#if !defined(CFRONT_COMPATIBLE_LUCENT)
	typedef reverse_iterator<const_iterator, value_type,
		const_reference, difference_type> const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type,
		reference, difference_type> reverse_iterator;
#else
	typedef char *reverse_iterator;
	typedef const char *const_reverse_iterator;
#endif

	static const size_t npos;

	/* constructors and destructor */
	string() { d = Srep_LUCENT::nullrep(); }
	string(const string& s, size_t = 0, size_t = npos);
	string(const char*, size_t);
	string(const char*);
	string(size_t, char);
	//template <class InputIterator> string(InputIterator, InputIterator);
	~string() { d->rcdec(); }

	/* assignment */
	inline string& operator=(const string&);
	string& operator=(const char*);
	inline string& operator=(char);
	
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
	void resize(size_t, char = '\0');
	size_t capacity() const { return d->max_size; }
	void reserve(size_t n) {
		if (n >= d->max_size) reserve_grow(n);
	}

	bool empty() const { return d->len ? 0 : 1; } 

	inline char operator[](size_t) const;
	char& operator[](size_t);
	inline const char& at(size_t) const;
	char& at(size_t);

	/* concatenation */
	string& operator+=(const string&);
	string& operator+=(const char*);
	inline string& operator+=(char);
	string& append(const string &, size_t pos = 0, size_t n = npos);
	string& append(const char*, size_t len);
	string& append(const char*);
	string& append(size_t, char = '\0');
	//template <class InputIterator> string& append(InputIterator, InputIterator);

	string& assign(const string &, size_t pos = 0, size_t n = npos);
	string& assign(const char *, size_t);
	string& assign(const char *);
	string& assign(size_t, char = '\0');
	//template <class InputIterator> string& assign(InputIterator, InputIterator);
	
	string& insert(size_t, const string &, size_t = 0, size_t = npos);
	string& insert(size_t, const char *, size_t);
	string& insert(size_t, const char *);
	string& insert(size_t, size_t, char = '\0');
	iterator insert(iterator, char = '\0');
	iterator insert(iterator, size_t, char = '\0');
	//template <class InputIterator> void insert(iterator, InputIterator, InputIterator);
	
	string& remove(size_t pos = 0, size_t n = npos);
	string& remove(iterator);
	string& remove(iterator, iterator);


	string& replace(size_t, size_t, const string &, size_t = 0, size_t = npos);
	string& replace(size_t, size_t, const char *, size_t);
	string& replace(size_t, size_t, const char *);
	string& replace(size_t, size_t, char);
	string& replace(iterator, iterator, const string &);
	string& replace(iterator, iterator, const char *, size_t);
	string& replace(iterator, iterator, const char *);
	string& replace(iterator, iterator, char);
	//template <class InputIterator> string& replace(iterator, iterator, InputIterator, InputIterator);
	
	size_t copy(char *, size_t, size_t = 0);
	void swap(string &);
	inline const char *c_str() const;
	inline const char *data() const;
	
	/* find functions */
	size_t find(const string &, size_t = 0) const;
	size_t find(const char *, size_t, size_t) const;
	size_t find(const char *, size_t = 0) const;
	size_t find(char, size_t = 0) const;
	size_t rfind(const string &, size_t = npos) const;
	size_t rfind(const char *, size_t, size_t) const;
	size_t rfind(const char *, size_t = npos) const;
	size_t rfind(char, size_t = npos) const;
	size_t find_first_of(const string &, size_t = 0) const;
	size_t find_first_of(const char *, size_t, size_t) const;
	size_t find_first_of(const char *, size_t = 0) const;
	size_t find_first_of(char, size_t = 0) const;
	size_t find_last_of(const string &, size_t = npos) const;
	size_t find_last_of(const char *, size_t, size_t) const;
	size_t find_last_of(const char *, size_t = npos) const;
	size_t find_last_of(char, size_t = npos) const;
	size_t find_first_not_of(const string &, size_t = 0) const;
	size_t find_first_not_of(const char *, size_t, size_t) const;
	size_t find_first_not_of(const char *, size_t = 0) const;
	size_t find_first_not_of(char, size_t = 0) const;
	size_t find_last_not_of(const string &, size_t = npos) const;
	size_t find_last_not_of(const char *, size_t, size_t) const;
	size_t find_last_not_of(const char *, size_t = npos) const;
	size_t find_last_not_of(char, size_t = npos) const;

	string substr(size_t = 0, size_t = npos) const;

	int compare(const string &, size_t = 0, size_t = npos) const;
	int compare(const char *, size_t, size_t) const;
	int compare(const char *, size_t = 0) const;
	
private:
	Srep_LUCENT *d;
	string(Srep_LUCENT* r) { d = r; }
	string& newcopy(char);
	void reserve_grow(int);
	inline void uniq();
public:
	/* ADDITION */
	friend string operator+(const string&, const string&);
	friend string operator+(const string&, const char*);
	friend string operator+(const string&, char);
	friend string operator+(const char*, const string&);
	friend string operator+(char, const string&);
	//int nrefs() const { return d->refc; }
	//const Srep_LUCENT *rep() const { return d; }
	/* COMPARISON OPERATORS */
	friend inline bool operator==(const string&, const string&);
	friend inline bool operator==(const string&, const char*);
	friend inline bool operator!=(const string&, const string&);
	friend inline bool operator!=(const string&, const char*);
	/* INPUT/OUTPUT */
	friend /*inline*/ ostream& operator<<(ostream&, const string&);
	friend istream& operator>>(istream&, string&);
	friend istream& getline(istream&, string&, char delim = '\n');

#if defined(COMPILER_IS_PC_LUCENT)
#if !defined(_MSC_VER) || _MSC_VER < 1000
private:
	static size_t length(const char *s) { return (strlen(s)); }
	static char *copy(char *s1, const char *s2, size_t n) {
		return (strncpy(s1, s2, n)); }
#endif
#endif
};


// inline functions

inline
const char *string::data() const {
	if (d->len == 0) return 0;
	else {
		if (d->len >= d->max_size && d->len)
			((string *) this)->reserve_grow(d->len + 1);
		*(d->str + d->len) = '\0'; 
		return (d->str);
	}
}
inline
const char *string::c_str() const {
	if (d->len >= d->max_size && d->len)
		((string *) this)->reserve_grow(d->len + 1);
	*(d->str + d->len) = '\0'; 
	return (d->str);
}

inline string&
string::operator=(const string& r) {
        r.d->rcinc();
        d->rcdec();
        d = r.d;
        return *this;
}

inline string&
string::operator=(char c) {
	if(d->refc!=1) {
		d->rcdec();
		d = Srep_LUCENT::new_srep(1);
	}
	d->str[0] = c;
	d->len = 1;
	return *this;
}
inline bool
operator==(const string& t, const string& s) {
	register int dlen = t.d->len;
	register bool retval;
	if ((dlen != s.d->len) || (dlen && t.d->str[0] != s.d->str[0]) || (dlen>1 && t.d->str[1] != s.d->str[1]))
		retval = 0;
	else
		retval = !memcmp(t.d->str, s.d->str, dlen);
	return retval;
}
inline bool
operator==(const string& t, const char* s) {
	register int dlen = t.d->len;
	register bool retval;
	if (!s || (dlen && t.d->str[0]!=s[0]) || (dlen>1 && s[0]!='\0' && t.d->str[1]!=s[1]))
		retval = 0;
	else
		retval = (dlen != strlen(s)) ? 0 : !memcmp(t.d->str, s, dlen);
	return retval;
}

inline bool
operator==(const char* s, const string& t) { return t==s; }

inline int
operator!=(const string& t, const string& s) {
	register int dlen = t.d->len;
	register bool retval;
	if ((dlen != s.d->len) || (dlen && t.d->str[0] != s.d->str[0]) || (dlen>1 && t.d->str[1] != s.d->str[1]))
		retval = 1;
	else
		retval = memcmp(t.d->str, s.d->str, dlen);
	return retval;
}
inline bool
operator!=(const string& t, const char* s) {
	register int dlen = t.d->len;
	register bool retval;
	if (!s || (dlen && t.d->str[0]!=s[0]) || (dlen>1 && s[0]!='\0' && t.d->str[1]!=s[1]))
		retval = 1;
	else
		retval = (dlen != strlen(s)) ? 1 : memcmp(t.d->str, s, dlen);
	return retval;
}

inline bool 
operator!=(const char* s, const string& t) { return t!=s; }
inline bool 
operator>=(const string& s, const string& t) { return s.compare(t)>=0; }
inline bool 
operator>=(const string& t, const char* s) { return t.compare(s)>=0; }
inline bool 
operator>=(const char* s, const string& t) { return t.compare(s)<=0; }
inline bool 
operator>(const string& s, const string& t) { return s.compare(t)>0; }
inline bool 
operator>(const string& t, const char* s) { return t.compare(s)>0; }
inline bool 
operator>(const char* s, const string& t) { return t.compare(s)<0; }
inline bool 
operator<=(const string& s, const string& t) { return s.compare(t)<=0; }
inline bool 
operator<=(const string& t, const char* s) { return t.compare(s)<=0; }
inline bool 
operator<=(const char* s, const string& t) { return t.compare(s)>=0; }
inline bool 
operator<(const string& s, const string& t) { return s.compare(t)<0; }
inline bool 
operator<(const string& t, const char* s) { return t.compare(s)<0; }
inline bool 
operator<(const char* s, const string& t) { return t.compare(s)>0; }

inline char
string::operator[](size_t i) const {
	return d->str[i];
}
inline const char &
string::at(size_t i) const {
	return d->str[i];
}

inline string&
string::operator+=(char c) {
	int append_to_current = 1;
	if (d->refc != 1 || d->len == d->max_size - 1)
		append_to_current = 0;
	else
		d->str[d->len++] = c;
	string &s = append_to_current ? *this : newcopy(c);
	return s;
}

inline void
string::uniq() {
	register Srep_LUCENT* x;
	if(d->refc > 1) {
		x = Srep_LUCENT::new_srep(d->len);
		memcpy(x->str, d->str, d->len);
		d->rcdec();
		d = x;
	}
}
inline string::iterator string::begin() { uniq(); return d->str; }
inline string::const_iterator string::begin() const { return d->str; }
inline string::iterator string::end() { uniq(); return d->str + d->len; }
inline string::const_iterator string::end() const { return d->str + d->len; }
inline string::reverse_iterator string::rbegin() { uniq(); return d->str + d->len; }
inline string::const_reverse_iterator string::rbegin() const { return d->str + d->len; }
inline string::reverse_iterator string::rend() { uniq(); return d->str; }
inline string::const_reverse_iterator string::rend() const { return d->str; }

#ifdef NAMESPACES_LUCENT
}
#endif

#endif
