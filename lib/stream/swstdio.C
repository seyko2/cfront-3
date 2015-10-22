/*ident	"@(#)cls4:lib/stream/swstdio.c	1.3" */
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


#include <iostream.h>
#include <stdiostream.h>

static void stdioflush()
{
	fflush(stdout) ;
	fflush(stderr) ;
	}

void ios::sync_with_stdio() 
{
	switch ( Iostream_init::stdstatus )
	{
	case 1 : cout.flush() ; cerr.flush() ; clog.flush() ; break ;
	default : return ; }
 
	cin = new stdiobuf(stdin) ;
	cout = new stdiobuf(stdout) ;
	cerr = new stdiobuf(stderr) ;
	clog = cerr ;
	cin.setf(~0L,skipws) ;
	cout.setf(~0L,unitbuf|stdio) ;
	cerr.setf(~0L,unitbuf|stdio) ;
	clog.setf(~0L,unitbuf|stdio) ;
	cin.tie(&cout) ;
	cerr.tie(&cout) ;
	clog.tie(&cout) ;
	Iostream_init::stdstatus = 2 ;
	ios::stdioflush = ::stdioflush ;
	}
