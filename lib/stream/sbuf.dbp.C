/*ident	"@(#)cls4:lib/stream/sbuf.dbp.c	1.3" */
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
#include <stdio.h>
#include <string.h>
#include <osfcn.h>

void streambuf::dbp()
{
	char msg[256] ;
	sprintf(msg,"buf at %#x, base=%#x, ebuf=%#x, ",
				this, base(), ebuf());
	write(1,msg,strlen(msg)) ;
	sprintf(msg,"pptr=%#x, epptr=%#x, ", pptr(),epptr() );
	write(1,msg,strlen(msg)) ;
	sprintf(msg,"eback=%#x, gptr=%#x, egptr=%#x\n",
			eback(), gptr(), egptr() ) ;
	write(1,msg,strlen(msg)) ;

}
