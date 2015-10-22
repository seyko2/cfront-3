/*ident	"@(#)cls4:lib/stream/cstreams.c	1.3" */
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
#include <fstream.h>
#include <new.h>
#include "streamdefs.h"

int Iostream_init::stdstatus = 0 ;
int Iostream_init::initcount = 0 ;

istream_withassign cin ;
ostream_withassign cout ;
ostream_withassign cerr ;
ostream_withassign clog ;

	// The value of Iostream_init::stdstatus describes the
	// The std stream variables

	// 0 means istream::stdin, ostream::stdout, ostream::stderr, and
	//	ostream::stdlog are unitialized
	// 1 means they are initialized to filebufs
	// 2 means they are initialized to stdiobufs

Iostream_init::Iostream_init() 
{
	++initcount ;
	if ( initcount > 1 ) return ;

	// Be careful.  Initialization occurs before constructors
	// of the standard streams are called.
	new (&cin) istream_withassign ;
	new (&cout) ostream_withassign ;
	new (&cerr) ostream_withassign ;
	new (&clog) ostream_withassign ;
	cin = new filebuf(0) ;
	cout = new filebuf(1) ;
	cerr = new filebuf(2) ;
	clog = new filebuf(2) ;

	cin.setf(~0L,ios::skipws) ;
	cerr.setf(~0L,ios::unitbuf) ;
	cin.tie(&cout) ;
	cerr.tie(&cout) ;
	clog.tie(&cout) ;
	stdstatus = 1 ;
	}

Iostream_init::~Iostream_init()
{
	--initcount ;
	if ( initcount > 0 ) return ;
	cout.flush() ;
	cerr.flush() ;
	clog.flush() ;
	}
