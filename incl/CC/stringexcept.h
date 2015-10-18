/*ident	"@(#) @(#)stringexcept.h	1.1.1.2" */
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

#ifndef STRINGEXCEPTH 
#define STRINGEXCEPTH

#include "compiler_type.h"

#include <stdexcept.h>
#include <std_string.h>

#ifdef NAMESPACES_LUCENT
namespace std {
#endif

class logic_error : public exception {
public:
	logic_error(const string& what_arg);
	~logic_error() throw();
	const char* what() const throw();
protected:
	string strval;
};

class out_of_range : public logic_error {
public:
	out_of_range(const string& what_arg);
	~out_of_range() throw();
	const char* what() const throw();
};

class length_error : public logic_error {
public:
	length_error(const string& what_arg);
	~length_error() throw();
	const char* what() const throw();
};

#ifdef NAMESPACES_LUCENT
}
#endif

#endif
