/*ident	"@(#)Args:Args.c	3.1" */
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

#include "Argslib.h"

Args::Args(int argc, const char*const* argv, const char* optstr_, int bits, const char*const* keywords) :
	opterr(0),
	firstopt(0),
	lastopt(0),
	firstarg(0),
	lastarg(0),
	optstr(optstr_),
	progname_(argv[0]),
	nargs_(0),
	plus(bits & plusoptions) {
	parse(argc, argv, bits & intermix, keywords);
}

int
Args::isflag(char c) const {
	return (c == '-' || (plus && c == '+'));
}

void
Args::parse(int argc, const char*const* argv, int mix, const char*const* keywords) {
	int i;
	for (i = 1; i < argc; ++i) {
		const char* ap = argv[i];
		if (strcmp(ap, "--") == 0) {
			++i;
			break;
		}
		if (isflag(*ap))
			parse_block(argc, argv, i, keywords);
		else if (mix)
			addarg(argv[i]);
		else
			break;
	}
	for (; i < argc; ++i) {
		addarg(argv[i]);
	}
}

Args::~Args() {
	for (Opt* opt = firstopt; opt != 0; ) {
		Opt* next = opt->next;
		delete opt;
		opt = next;
	}
	for (Arg* arg = firstarg; arg != 0; ) {
		Arg* next = arg->next;
		delete arg;
		arg = next;
	}
	firstopt = lastopt = 0;
	firstarg = lastarg = 0;
	nargs_ = 0;
}

Subopt::Subopt(const char* n, const char* v) :
	name_(n),
	value_(v),
	next(0),
	prev(0) {
}

void
Args::addarg(const char* p) {
	++nargs_;
	Arg* arg = new Arg(p);
	if (lastarg == 0) {
		assert(firstarg == 0);
		firstarg = arg;
	}
	else {
		lastarg->next = arg;
	}
	lastarg = arg;
}

void
Args::addopt_(Opt* opt) {
	opt->prev = lastopt;
	if (lastopt == 0) {
		assert(firstopt == 0);
		firstopt = opt;
	}
	else {
		lastopt->next = opt;
	}
	lastopt = opt;
}

void 
Args::addopt(char flag, const char* option, const char* value, int iskey) {
	if (iskey)
		addopt_(new Opt(flag, option, value));
	else
		addopt_(new Opt(flag, *option, value));
}

// warning: name and value might not yet be null terminated
// (but they will eventually be by the caller)
void
Opt::addsubopt(char* name, char* value) {
	Subopt* subopt = new Subopt(name, value);
	subopt->prev = lastsubopt;
	if (lastsubopt == 0) {
		assert(firstsubopt == 0);
		firstsubopt = subopt;
	}
	else {
		lastsubopt->next = subopt;
	}
	lastsubopt = subopt;
}

Args::Opttype
Args::opttype(char flag, char option) {
	const char* p = strchr(optstr, option);
	if (p == 0) {
		unexpected_opt_err(flag, option);
		return illegalopt;
	}
	else {
		switch (p[1]) {
		case valchar:	return valueopt;
		case subchar:	return suboptionsopt;
		default:	return simpleopt;
		}
	}
}

// gets the value for option at ap in argv[i]
// increments i if the value is argv[i+1]
const char*
Args::getvalue(int argc, const char*const* argv, int& i, char flag, const char* ap, int iskey) {
	if (!iskey && ap[1] != nul) {
		return ap+1;
	}
	else {
		if (++i >= argc) {
			missing_val_err(flag, ap, iskey);
			return 0;
		}
		const char* value = argv[i];
		assert(value != 0);
		if (isflag(*value))
			opt_as_val_err(value);
		return value;
	}
}

static char
scanto(char*& p, const char* chars) {
	char* first = strpbrk(p, chars);
	if (first == 0)
		p += strlen(p);
	else
		p = first;
	return *p;
}

// build this option's suboptions from its value
void
Opt::build_subopts() {
	buf = new char [strlen(value_) + 1];
	strcpy(buf, value_);
	for (char* p = buf; *p != nul; ) {
		char* name = p;
		char* subvalue;
		switch (scanto(p, ",=")) {
		case ',': 
			addsubopt(name, p);
			*p++ = nul;
			break;
		case nul:
			addsubopt(name, p);
			break;
		case '=':
			*p++ = nul;
			subvalue = p;
			scanto(p, ",");
			addsubopt(name, subvalue);
			if (*p == ',') {
				*p++ = nul;
			}
			break;
		}
	}
}

void
Args::process_option(int argc, const char*const* argv, int& i, char flag, const char* ap, Opttype ot, int iskey) {
	assert(ot != illegalopt);
	if (ot == simpleopt) {
		addopt(flag, ap, 0, iskey);
	}
	else {
		const char* value = getvalue(argc, argv, i, flag, ap, iskey);
		if (value != 0) {
			addopt(flag, ap, value, iskey);
			if (ot == suboptionsopt)
				lastopt->build_subopts();
		}
	}
}

int 
Args::iskeyword_option(const char* opt, const char*const* keywords, Opttype& ot) {
	if (keywords == 0)
		return 0;
	int len = strlen(opt);
	assert(len > 0);
	int i = 0;
	const char* p;
	while ((p = keywords[i++]) != 0) {
		if (memcmp(opt, p, len) == 0) {
			switch(p[len]) {
			default:
				break;
			case nul:
				if (p[len-1] != valchar && p[len-1] != subchar) {
					ot = simpleopt;
					return 1;
				}
				break;
			case valchar:
				if (p[len+1] == nul) {
					ot = valueopt;
					return 1;
				}
				break;
			case subchar:
				if (p[len+1] == nul) {
					ot = suboptionsopt;
					return 1;
				}
				break;
			}
		}
	}
	return 0;
}

// parse argv[i]
// increments i if argv[i+1] was an option value
void
Args::parse_block(int argc, const char*const* argv, int& i, const char*const* keywords) {
	Opttype ot;
	const char* ap = argv[i];
	char flag = *ap++;
	if (*ap == nul)
		missing_opt_err();
	else if (iskeyword_option(ap, keywords, ot))
		process_option(argc, argv, i, flag, ap, ot, 1);
	else
		crack_block(argc, argv, i, flag, ap);
}

void
Args::crack_block(int argc, const char*const* argv, int& i, char flag, const char* ap) {
	for (; *ap != nul; ++ap) {
		Opttype ot = opttype(flag, *ap);;
		if (ot) {
			if (ot != simpleopt && ap > argv[i]+1) {
				// value-taking options cannot appear in option blocks
				missing_val_err(flag, ap, 0);
			}
			else {
				process_option(argc, argv, i, flag, ap, ot, 0);
				if (ot != simpleopt)
					return;
			}
		}
	}
}
		
///////////////////////
// put here to overcome
// cfront ={0} bug
//
Objection Args::unexpected_opt;
Objection Args::missing_val;
Objection Args::missing_opt;
Objection Args::opt_as_val;
//
///////////////////////

#if 0

#include <stream.h>
ostream& operator<<(ostream& os, const Args& a) {
	os << "options: ";
	for (Args::Opt* opt = a.firstopt; opt != 0; opt = opt->next) {
		os << "<" << opt->flag << opt->option << ", " << opt->value_ << "> ";
	}
	os << "\narguments: ";
	for (Args::Arg* arg = a.firstarg; arg != 0; arg = arg->next) {
		os << arg->p << " ";
	}
	return os;
}
#endif

Args::Args(const Args &curr) : optstr(curr.optstr), progname_(curr.progname_), 
firstopt(0), lastopt(0), firstarg(0), lastarg(0)
{
	opterr = curr.opterr;
	plus = curr.plus;
	nargs_ = curr.nargs_;

	for (Opt* opt = curr.firstopt; opt != 0; ) {
		Opt *tempopt = new Opt (*opt);
		
        	tempopt->prev = lastopt;
        	if (lastopt == 0) {
                	firstopt = tempopt;
        	}
        	else {
                	lastopt->next = tempopt;
        	}
        	lastopt = tempopt;
		opt = opt->next;
        }
	for (Arg* arg = curr.firstarg; arg != 0; ) {
		Arg *temparg = new Arg (*arg);
		
        	if (lastarg == 0) {
                	firstarg = temparg;
        	}
        	else {
                	lastarg->next = temparg;
        	}
        	lastarg = temparg;
		arg = arg->next;
        }
}
 
Opt::Opt(const Opt &curr) : key(curr.key), value_(curr.value_), next(0), prev(0), buf(0), firstsubopt(0),
lastsubopt(0)
{ 
	flag_ = curr.flag_;
	chr_ = curr.chr_;
	if (curr.buf) {
		buf = new char[strlen(curr.buf)+1];
		strcpy(buf, curr.buf);
	}

	for (Subopt* subopt = curr.firstsubopt; subopt != 0; ) {
		Subopt *tempsubopt = new Subopt (*subopt);
        	tempsubopt->prev = lastsubopt;
        	if (lastsubopt == 0) {
                	firstsubopt = tempsubopt;
        	}
        	else {
                	lastsubopt->next = tempsubopt;
        	}
        	lastsubopt = tempsubopt;
		subopt = subopt->next;
	}
}


