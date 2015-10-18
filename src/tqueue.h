/*ident	"@(#)cls4:src/tqueue.h	1.4" */
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

#ifndef EOF
#include <stdio.h>
#endif

struct toknode {
	TOK      tok;			/* token for parser */
	bit      used;			/* token has been processed by lalex() */
	YYSTYPE  retval;			/* $arg */
	Pname    idname;
	loc      place;
	toknode* next;
	toknode* last;
	static toknode* free_toks;
	void*   operator new(size_t);
	void    operator delete(void*,size_t);
		toknode(TOK,YYSTYPE,loc);
};
extern toknode* front;

extern void addtok(TOK,YYSTYPE,loc);	/* add tok to rear of Q */
extern TOK deltok(int);			/* take tok from front of Q */
extern TOK tlex();

// interface to parser
extern TOK lalex();
extern int yychar;
extern YYSTYPE yylval;
extern TOK	la_look();
extern void	la_backup( TOK, YYSTYPE );
extern int	la_cast();
extern void	check_decl();
extern void	check_cast();
extern TOK	lalex();

// save / restore implicit inline functions
extern toknode*	save_text();
extern void	restore_text();

// set scope after qualified declarator
extern Pname SET_SCOPE( Pname );
extern void  UNSET_SCOPE();
