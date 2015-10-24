/*ident	"@(#)Args:demos/example.c	3.1" */
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

#include <Args.h>
#include <string.h>
#include <stdlib.h>
#include <stream.h>
	
int compile_only = 0;
int optimize = 0;
int allow_anachronisms = 1;
int profiling = 0;
const char* output_filename = "a.out";
int dryrun = 0;

void add_define(const char* name, const char* value) {
	cout << "add_define(" << name << ", " << value << ")\n";
}
void add_includedir(const char* dir) {
	cout << "add_includedir(" << dir << ")\n";
}
void align(const char* symbol) {
	cout << "align(" << symbol << ")\n";
}
void compile(const char* file) {
	cout << "compile(" << file << ")\n";
}

static void 
getdefines(const Opt* opt) {
	Suboptsiter si(*opt);
	const Subopt* subopt;
	while (si.next(subopt))
		add_define(subopt->name(), subopt->value());
}

static void 
process_letter_option(const Opt* opt) {
	switch (opt->chr()) {
	case 'I':
		add_includedir(opt->value());
		break;
	case 'D':
		getdefines(opt);
		break;
	case 'c':
		compile_only = 1;
		break;
	case 'O':
		optimize = 1;
		break;
	case 'o':
		output_filename = opt->value();
		break;
	case 'p':
		if (opt->flag() == '+')
			allow_anachronisms = 0;
		else
			profiling = 1;	
		break;
	}
}

static void 
process_option(const Opt* opt) {
	const char* key = opt->keyword();
	if (key != 0) {
		if (strcmp(key, "align") == 0)
			align(opt->value());
		else
			dryrun = 1;
	}
	else 
		process_letter_option(opt);
}

static void
process_nonoption_args(const Args& args) {
	Argsiter ai(args);
	const char* filename;
	while (ai.next(filename))
		compile(filename);
}

const char* keywords[] = {
	"dryrun",
	"align:",
	0
};

void show() {
	cout << "compile_only = " << compile_only << "\n";
	cout << "optimize = " << optimize << "\n";
	cout << "allow_anachronisms = " << allow_anachronisms << "\n";
	cout << "profiling = " << profiling << "\n";
	cout << "output_filename = " << output_filename << "\n";
	cout << "dryrun = " << dryrun << "\n";
}

main(int argc, const char*const* argv) {
	Args args(argc, argv, "co:OI:D;p", Args::intermix, keywords);
	if (args.error()) 
		exit(1);
	Optsiter oi(args);
	const Opt* opt;
	while (oi.next(opt))
		process_option(opt);
	process_nonoption_args(args);

	show();

	return 0;
}
