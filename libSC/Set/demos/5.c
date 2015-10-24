/*ident	"@(#)Set:demos/5.c	3.1" */
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

#include <Set.h>

Set_or_Bag_hashval Set<int>::hash(const int& t)
{
	if ( t % 2 == 0 )
	{
	    return 2;
	}
	else if ( t < 0 )
	{
	    return 1;
	}
	else
	{
	    return t+2;
	}
}

main()
{
	Set<int> s;

	for( int i = -20 ; i<1000 ; i++ )
	{
	    s.insert(i);
	}
	return 0;
}
