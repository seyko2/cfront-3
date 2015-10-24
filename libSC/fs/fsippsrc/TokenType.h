/*ident	"@(#)fs:fsippsrc/TokenType.h	3.1" */
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



enum TokenType { BOF, NEW, DELETE, VOID, ID, LP, RP, LC, RC, STAR, 
	QUAL,  OPERATOR, SEMI, COMMA, POUND, LITINT, LITSTRING, 
	EOFTOK, OTHERTOK, COLON, PR, VIRTUAL, AGGR, NL, LS, RS, 
	INLINE, LANGLE, RANGLE };

