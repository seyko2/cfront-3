/*ident	"@(#)cls4:src/typedef.h	1.3" */
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

typedef unsigned char TOK;
typedef unsigned char bit;	// sometimes used as bits
typedef class node * PP;
typedef int (*PFI)();
typedef void (*PFV)();
typedef class node * Pnode;
typedef struct key * Pkey;
typedef class name * Pname;
typedef class basetype * Pbase;
typedef class basecl* Pbcl;
typedef class type * Ptype;
typedef class fct  * Pfct;
typedef class field * Pfield;
typedef class expr * Pexpr;
typedef class qexpr * Pqexpr;
typedef class texpr * Ptexpr;
typedef class classdef * Pclass;
typedef class enumdef * Penum;
typedef class stmt * Pstmt;
typedef class estmt * Pestmt;
typedef class tstmt * Ptstmt;
typedef class vec * Pvec;
typedef class ptr * Pptr;
typedef class block * Pblock;
typedef class table * Ptable;
typedef class ktable *Pktab;
typedef struct loc Loc;
typedef class call * Pcall;
typedef class gen* Pgen;
typedef class ref * Pref;
typedef class name_list * Plist;
typedef class iline * Pin;
typedef class ia * Pia;
typedef class nlist * Pnlist;
typedef class slist * Pslist;
typedef class elist * Pelist;
typedef class virt * Pvirt;
typedef char* Pchar;

