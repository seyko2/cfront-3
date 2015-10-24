/*ident	"@(#)cls4:tools/demangler/osrc/nplist.c	1.1" */

/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/

/*
 * C++ Demangler Source Code
 * @(#)master	1.5
 * 7/27/88 13:54:37
 */
#include "elf_dem.h"

struct name_pair {
	char *s1,*s2;
};

static struct name_pair nplist[100] = {
	{"lt","<"},    {"ls","<<"},      {"dv","/"}, 
	{"gt",">"},    {"rs",">>"},      {"md","%"}, 
	{"le","<="},   {"ml","*"},       {"pl","+"}, 
	{"ge",">="},   {"ad","&"},       {"mi","-"}, 
	{"ne","!="},   {"or","|"},       {"er","^"}, 
	{"aa","&&"},   {"oo","||"},      {"as","="}, 
	{"apl","+="},  {"ami","-="},     {"amu","*="}, 
	{"adv","/="},  {"amd","%="},     {"aad","&="}, 
	{"aor","|="},  {"aer","^="},     {"als","<<="},
	{"ars",">>="}, {"pp","++"},      {"mm","--"},
	{"vc","[]"},   {"cl","()"},      {"rf","->"},
	{"eq","=="},   {"co","~"},       {"nt","!"},
	{"nw"," new"}, {"dl"," delete"}, {"cm",","},
	 {0,0} };

/* This routine demangles an overloaded operator function */
char *
findop(c,oplen)
char *c;
int *oplen;
{
	register int i,opl;
	for(opl=0; c[opl] && c[opl] != '_'; opl++)
		;
	*oplen = opl;
	for(i=0; nplist[i].s1; i++) {
		if(memcmp(nplist[i].s1,c,opl) == 0)
			return nplist[i].s2;
	}
	return 0;
}
