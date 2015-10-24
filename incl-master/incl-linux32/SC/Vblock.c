/*ident	"@(#)Vblock.c	1.1.2.4" */
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

#ifndef _VBLOCK_C_
#define _VBLOCK_C_

template <class T>
Vblock<T>::~Vblock()
{
}

template <class T>
unsigned Vblock<T>::size() const {
	return B.size();
}

template <class T>
unsigned Vblock<T>::size(unsigned k) {
	return B.size(k);
}

template <class T>
void* Vblock<T>::elem(int i) {
	return (void*)&B[i];
}

template <class T>
void* Vblock<T>::beginning() {
	return (T*)(*this);
}

template <class T>
void* Vblock<T>::finish() {
	return end();
}

#endif
