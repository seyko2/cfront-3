/*ident	"@(#)Regex:incl/Regex.h	3.1" */
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

#ifndef _Regex_H
#define _Regex_H

#include <stddef.h>
#include <String.h>

class Substrinfo {
public:
	int i;
	size_t len;
	operator void* () { return (i<0? 0 : this); }
	int operator!() { return (i<0? 1 : 0); }
};

class Regex {
	friend class Regexiter;
public:
        enum sensitivity { case_sensitive = 1, case_insensitive = 0 };
	enum { max_num_subexes = 10 };
private:
	String pat;
	const char *subexbase;
	sensitivity sens;
	struct reprogram *prog;
	String error;

	void compile();
public:
	static Regex Float, Int, Double, Alpha, Alphanumeric, Identifier;

	Regex(const String &pattern, sensitivity = case_sensitive);
	~Regex();

	Regex(const Regex &);
	Regex & operator=(const Regex& r);
	void assign(const String &pattern, sensitivity = case_sensitive);

	operator void*() const;
	int operator!() const;
	String the_error() const;

	Substrinfo match(const char *target) const;
	Substrinfo match(const char *target, String &the_substr) const;
	Substrinfo match(const char *target, class Subex &, int at_bol=1) const;
	Substrinfo match(const char *target, class Subex &, String &the_substr) const;

	Substrinfo subex(unsigned int i) const;
	Substrinfo subex(unsigned int i, String &the_subex) const;
	
	friend int operator==(const Regex&, const Regex&);
	friend int operator!=(const Regex&, const Regex&);

	String the_pattern() const; 
	sensitivity the_sensitivity() const;
	void set_sensitivity(sensitivity);
};

class Subex {
	friend class Regex;
	friend class Regexiter;
	const char *base;
	reprogram *prog;
	static Substrinfo dosubex(unsigned int, reprogram *, const char *);
public:
	Subex();
	~Subex();

	Substrinfo operator()(unsigned int i) const;
	Substrinfo operator()(unsigned int i, String &the_substr) const;

	const char *the_target() const;
};

class Regexiter {
public:
	enum style { overlapping = 1, nonoverlapping = 0 };
private:
	const Regex &r;
	const char *start;
	const char *p;  // where next call to next() should start looking
	style s;
public:
	Regexiter(const Regex &, const char *target, style = nonoverlapping);
	~Regexiter();
	
	Substrinfo next();
	Substrinfo next(Subex &);
	Substrinfo next(String &the_substr);
	Substrinfo next(Subex &, String &the_substr);

	const char *the_target();
	const Regex &the_regex();
	style the_style();
};

// Compatibility
typedef Regexiter Regex_iter;

#endif

