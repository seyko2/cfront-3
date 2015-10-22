/*ident	"@(#) @(#)std_misc.c	1.1.1.2" */
/******************************************************************************
*
* C++ Standard Library
*
* Copyright (c) 1996  Lucent Technologies.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Lucent Technologies.
* The copyright notice above does not evidence any actual or
* intended publication of such source code.
*
******************************************************************************/

#include <iostream.h>
#include "std_string.h"
#ifdef NAMESPACES_LUCENT
namespace std {
#endif

int compare_wchar_t(const wchar_t *s1, const wchar_t *s2, size_t n) {
	int result = 0;
	size_t i = 0;
	while (i < n) {
		if (!eq_wchar_t(*s1, *s2)) {
			if (lt_wchar_t(*s1, *s2)) result = -1;
			else result = 1;
			break;
		}
		i++; s1++; s2++;
	}
	return result;
}

size_t length_wchar_t(const wchar_t *s) {
	size_t result = 0;
	while (!eq_wchar_t(*s++, eos_wchar_t())) result++;
	return (result);
}

istream &char_in_wchar_t(istream &is, wchar_t &a) {
	char first, second;
	is >> first >> second;
	a = (first << 8) | (second & 0377);
	return is; }

ostream &char_out_wchar_t(ostream &os, wchar_t a) {
	char first, second;
	first = (a >> 8) & 0377;
	second = a & 0377;
	return (os << first << second); }
#ifdef NAMESPACES_LUCENT
}
#endif
