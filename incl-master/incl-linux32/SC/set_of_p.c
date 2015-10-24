/*ident	"@(#)Set:incl/set_of_p.c	3.1" */
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

#ifndef _SET_OF_P_C_
#define _SET_OF_P_C_

template <class T>
T*
Set_of_p<T>::select() const
{
    pst_iter_old_ATTLC	it(*this);
    return (T*)it.first();
}

#include <stream.h>

template <class T>
ostream&
Set_of_p<T>::print(ostream& os) const
{
    os << "{";
    Set_of_piter<T> it(*this);
    T* p;
    int first=1;

    while ( p = it.next() ) {
	if ( first )
	    first=0;
	else
	    os << ",";

	os << *p;
    }
    os << "}";
    return os;
}

#endif
