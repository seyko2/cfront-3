/*ident	"@(#) @(#)std_exc.c	5.1.1.2" */
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

#include "std_string.h"
#if defined(EXCEPTION_LUCENT)

#include "stringexcept.h"

#if defined(__edg_lucent_41)
#define NAMESPACES_LUCENT
#endif

#if defined(NAMESPACES_LUCENT)
namespace std {
#endif

logic_error::logic_error(const string& what_arg) : exception(), strval() {
	strval += what_arg;
}
logic_error::~logic_error() throw() {}
const char* logic_error::what() const throw() {
	return strval.c_str();
}

out_of_range::out_of_range(const string& what_arg) : logic_error(what_arg) {
	strval = string("Out of range:") + strval;
}
out_of_range::~out_of_range() throw() {}
const char* out_of_range::what() const throw() {
	return strval.c_str();
}

length_error::length_error(const string& what_arg) : logic_error(what_arg) {
	strval = string("Length error:") + strval;
}
length_error::~length_error() throw() {}
const char* length_error::what() const throw() {
	return strval.c_str();
}

#if defined(NAMESPACES_LUCENT)
}
#endif

#endif
