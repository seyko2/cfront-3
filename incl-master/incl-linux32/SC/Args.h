/*ident	"@(#)Args:incl/Args.h	3.1" */
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

#ifndef ARGSH
#define ARGSH

#include <Objection.h>

class Opt {
public:
	inline char flag() const;
	inline char chr() const;
	inline const char* keyword() const;
	inline const char* value() const;
private:	
	friend class Args;
	friend class Optsiter;
	friend class Suboptsiter;
	char flag_;
	char chr_;
	const char* key;
	const char* value_;
	Opt* next;
	Opt* prev;
	char* buf;
	class Subopt* firstsubopt;
	class Subopt* lastsubopt;
	void build_subopts();
	void addsubopt(char* name, char* value);
	inline int is(char opt);
	int is(const char* opt);
	Opt(char flag_, char option_, const char* value_);
	Opt(char flag_, const char* option_, const char* value_);
        Opt(const Opt&);
	~Opt();
};

class Subopt {
public:
	inline const char* name() const;
	inline const char* value() const;
private:
	friend class Opt;
	friend class Args;
	friend class Suboptsiter;
	const char* name_;
	const char* value_;
	Subopt* next;
	Subopt* prev;
	Subopt(const char* name, const char* value);
        Subopt(const Subopt& curr) : name_(curr.name_), value_(curr.value_),
            next(curr.next), prev(curr.prev) { }
	inline  ~Subopt();
};

class Args {
public:
	enum { intermix = 0x1, plusoptions = 0x2 };
	enum Opttype { illegalopt = 0, simpleopt, valueopt, suboptionsopt };
	Args(int argc, const char*const* argv, const char* optstr, int bits = 0, const char*const* keywords = 0);
	Args(const Args&);
	~Args();
	int isoptset(char opt) const;
	int isoptset(const char* key) const;
	int isset(char opt) const;
	int isset(const char* key) const;
	char flag(char opt) const;
	char flag(const char* key) const;
	const char* value(char opt) const;
	const char* value(const char* key) const;
	const char* subvalue(char opt, const char* name) const;
	const char* subvalue(const char* key, const char* name) const;
	const char* arg(int nbr) const;
	inline int nargs() const;
	inline const char* progname() const;
	inline int error() const;

	static Objection unexpected_opt;
	static Objection missing_opt;
	static Objection missing_val;
	static Objection opt_as_val;
private:
        // Make private so it cannot be called
        Args& operator=(const Args&);
	friend class Optsiter;
	friend class Argsiter;
	struct Arg {
		const char* p;
		Arg* next;
		Arg(const char* p_) : p(p_), next(0) {};
		Arg(const Arg& curr) : p(curr.p), next(curr.next) { };
		~Arg() {};
	};
	int opterr;
	int plus;
	Opt* firstopt;
	Opt* lastopt;
	Arg* firstarg;
	Arg* lastarg;
	const char* optstr;
	const char* progname_;
	int nargs_;
	void parse(int argc, const char*const* argv, int intermix, const char*const* keywords);
	void addarg(const char* p);
	int iskeyword_option(const char* opt, const char*const* keywords, Opttype&);
	void parse_block(int argc, const char*const* argv, int& i, const char*const* keywords);
	void crack_block(int argc, const char*const* argv, int& i, char flag, const char* ap);
	void addopt(char flag, const char* option, const char* value, int iskey);
	void addopt_(Opt* opt);
	Opttype opttype(char flag, char option);
	const char* getvalue(int argc, const char*const* argv, int& i, char flag, const char* ap, int iskey);
	int has_suboptions(char option);
	int isflag(char c) const;
	void process_option(int argc, const char*const* argv, int& i, char flag, const char* ap, Opttype, int iskey);
	void unexpected_opt_err(char flag, char option);
	void missing_val_err(char flag, const char* option, int iskey);
	void opt_as_val_err(const char* value);
	void missing_opt_err();
};

class Optsiter {
public:
	Optsiter(const Args& args);
	int next(const Opt*&);
	inline const Args* the_args() const;
private:
	//friend class Suboptsiter;
	const Args* args;
	int begin;
	const Opt* opt;
};

class Suboptsiter {
public:
	Suboptsiter(const Opt& opt);
	int next(const Subopt*&);
	inline const Opt* the_opt() const;
private:
	const Opt* opt;
	int begin;
	const Subopt* subopt;
};

class Argsiter {
public:
	Argsiter(const Args& args);
	int next(const char*&);
	inline const Args* the_args() const;
private:
	const Args* args;
	int begin;
	const Args::Arg* arg;
};

// inlines

inline int 
Args::nargs() const {
	return nargs_;
}

inline const char* 
Args::progname() const {
	return progname_;
}

inline char
Opt::chr() const {
	return chr_;
}

inline const char*
Opt::keyword() const {
	return key;
}

inline char 
Opt::flag() const {
	return flag_;
}

inline const char*
Opt::value() const {
	return value_;
}

inline const char* 
Subopt::name() const {
	return name_;
}

inline const char* 
Subopt::value() const {
	return value_;
}

inline int
Args::error() const {
	return opterr;
}

inline const Args* 
Optsiter::the_args() const {
	return args;
}

inline const Opt*
Suboptsiter::the_opt() const {
	return opt;
}

inline const Args* 
Argsiter::the_args() const {
	return args;
}

inline
Subopt::~Subopt() {
}

inline int 
Opt::is(char opt) {
	return key == 0 && opt == chr_;
}

#endif
