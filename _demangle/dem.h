/*ident	"@(#)cls4:tools/demangler/dem.h	1.5" */
/*******************************************************************************
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
typedef struct DEMARG DEMARG;
typedef struct DEMCL DEMCL;
typedef struct DEM DEM;

enum DEM_TYPE {
	DEM_NONE,		/* placeholder */
	DEM_STI,		/* static construction function */
	DEM_STD,		/* static destruction function */
	DEM_VTBL,		/* virtual table */
	DEM_PTBL,		/* ptbl vector */
	DEM_FUNC,		/* function */
	DEM_MFUNC,		/* member function */
	DEM_SMFUNC,		/* static member function */
	DEM_CMFUNC,		/* const member function */
	DEM_OMFUNC,		/* conversion operator member function */
	DEM_CTOR,		/* constructor */
	DEM_DTOR,		/* destructor */
	DEM_DATA,		/* data */
	DEM_MDATA,		/* member data */
	DEM_LOCAL,		/* local variable */
	DEM_CTYPE,		/* class type */
	DEM_TTYPE		/* template class type */
};

struct DEMARG {
	char* mods;		/* modifiers and declarators (page 123 in */
				/* ARM), e.g. "CP" */

	long* arr;		/* dimension if mod[i] == 'A' else NULL */

	DEMARG* func;		/* list of arguments if base == 'F' */
				/* else NULL */

	DEMARG* ret;		/* return type if base == 'F' else NULL */

	DEMCL* clname;		/* class/enum name if base == "C" */

	DEMCL** mname;		/* class name if mod[i] == "M" */
				/* in argument list (pointers to members) */

	DEMARG* next;		/* next argument or NULL */

	char* lit;		/* literal value for PT arguments */
				/* e.g. "59" in A<59> */

	char base;		/* base type of argument, */
				/* 'C' for class/enum types */
};

struct DEMCL {
	char* name;		/* name of class or enum without PT args */
				/* e.g. "Vector" */

	DEMARG* clargs;		/* arguments to class, NULL if not PT */

	char* rname;		/* raw class name with __pt__ if PT */
				/* e.g. "A__pt__2_i" */

	DEMCL* next;		/* next class name, NULL if not nested */
};

struct DEM {
	char* f;		/* function or data name;  NULL if type name */
				/* see page 125 of ARM for predefined list */

	char* vtname;		/* if != NULL name of source file for vtbl */

	DEMARG* fargs;		/* arguments of function name if __opargs__ */
				/* else NULL */

	DEMCL* cl;		/* name of relevant class or enum or NULL */
				/* used also for type-name-only input */

	DEMARG* args;		/* args to function, NULL if data or type */

	enum DEM_TYPE type;	/* type of name that was demangled */

	short slev;		/* scope level for local variables or -1 */

	char sc;		/* storage class type 'S' or 'C' or: */
				/* i -> __sti   d --> __std */
				/* b -> __ptbl_vec */
};

#define MAXDBUF 4096

int demangle();
void dem_printarg();
void dem_printarglist();
int dem_print();
void dem_printfunc();
int dem();
void dem_printcl();
char* dem_explain();
