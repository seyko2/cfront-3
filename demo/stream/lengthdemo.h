/*ident	"@(#)cls4:demo/stream/lengthdemo.h	1.1" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/

#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <string.h>
#include <ctype.h>

struct Unit {
	char*		name ;
	float		scale ;
	static const Unit*
			lookup(char* u) ;
				// The unit designated by u. 0 designates
				// a standard unit. 
			
				// If u does not designate a unit, returns 0
	static const Unit
			tbl[] ;
} ;
	
class Length {
public:
			Length() ;
				// Unitialized variable 

			Length(float v,const Unit* u = 0 ) ;
				// If u is null, v is taken to be in
				// The standard unit.

	float		value(const Unit* u = 0) ;
				// Size in units of u.
				// If u == 0, then size in standard unit.


private:
	float		standard ;  
				// Length is normalized for storage.
	

} ;

// Inserter and extractor for Length
 
ostream& operator<<(ostream&, Length) ;
istream& operator>>(istream&, Length& ) ;

// We want to extend streams to contain a "state variable" that
// points to a Unit.  The value of this state variable will be
// called the "default unit" of the stream.


extern int xunit  ;
	
	// User defined state variables (such as the default unit)
	// are accessed via an index.  This variable will be initialized
	// to hold the index.

typedef const Unit* pcUnit ;
	// Macros for declaring manipulators require type to be a 
	// simple name

IOMANIPdeclare(pcUnit)
	// Declare various data structures associated with "pcUnit"
	// manipulators.

SMANIP(pcUnit) setu(const Unit*) ;

	// setu(u) is a manipulator. That is it can be inserted into
	// an ostream and extracted from an istream.  The effect of
	// such operations is to set the default unit to u.

// Constructing a Length_init causes the Length library to be initialized.
// No functions from that library should be called before such a value
// is constructed.  It is allowed to construct more than one.

class Length_init {
public:		Length_init() ;
		~Length_init() ;
private:
	static int	count ;
			// keeps track of how many times constructor
			// has been called.
} ;
static Length_init length_init ;

			// If this really were in a header, each file that
			// included the header would get its own variable
			// named "length_init".  Further that variable 
			// would be constructed before things declared lower
			// in the file than Length.  This tends to cause
			// the library to be initialized before any uses
			// without any explicit programmer actions.
