/*ident	"@(#)cls4:demo/stream/lengthdemo.C	1.1" */
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
#include "lengthdemo.h"

int xunit ;

static const int max_unit_name_len = 32 ; 
	// Used to size an array for extracting units.

const Unit Unit::tbl[] = {	// Standard Unit is first element

	// Warning: max_unit_name_len must as great as maximum length
	// of string used as a unit.

	{ "in", 1. },		// inches
	{ "feet", 12.},		// feet
	{ "mi", 5280. * 12. },	// miles
	{ "m", 39.37079 },	// meters
} ;

Length::Length(float v,const Unit* u)
	: standard( u ? v * u->scale : v )
{	} 

Length::Length() { }

float Length::value(const Unit* u)
{
	return u ? standard / u->scale : standard ;
	}

const Unit* Unit::lookup(char* u)
{
	// Looks up u, returns a pointer to the Unit designated by
	// u or 0 if u does not designate a Unit.

	if ( !u ) return tbl ;
	for ( int x = 0 ; x < sizeof(tbl)/sizeof(*tbl) ; ++x ) {
		if ( strcmp(tbl[x].name,u) == 0 ) return &tbl[x] ;
		}
	return 0 ;
	}

ostream& operator<<(ostream& out, Length len)
{
	// Inserts the Length len, using the streams default unit.

	const Unit* u = (const Unit*)out.pword(xunit) ;
	out << (double)len.value(u) ;
	if ( u ) out << u->name ;
	return out ;
	}

istream& operator>>(istream& in, Length& len)
{
	float v ;			// Floating number in istream
	char str[max_unit_name_len] ;	// Unit in istream

	const Unit* u = (const Unit*)in.pword(xunit) ;
					// Default unit
	in>>v ;

	if ( in && !isspace(in.peek()) ) {
		// The next character is not whitespace, so it should
		// be a unit.

		// Note use of setw to avoid overflow of str in case
		// input is bad.

		in >> setw(sizeof(str)) >> str ;

		u = Unit::lookup(str) ;

		// If Unit wasn't legal (u==0), we consider it a formating
		// failure.  We use "clear" to set the error state of "in".
		// "in.rdstate()" is the current value.  We or in
		// "ios::failbit".  The previous state might have been
		// set non-zero when we read str. 

		if ( !u ) in.clear(ios::failbit|in.rdstate()) ;
		}

	len = Length(v,u) ;
	return in ;
	}

static ios& setufct(ios& s, const Unit* u) {
	// function to set the default unit of s.
	s.pword(xunit) = (void*)u ;
	return s ;
	}

SMANIP(pcUnit) setu(const Unit* u) {
	// Convert the function to a "manipulator".  setufct can
	// be used directly, but a manipulator is often more convenient.
	return SMANIP(pcUnit)(setufct,u) ;
	}

static Iostream_init* iosi ;
	// The iostream library uses the initialization technique
	// implemented by Length_init.  Namely, construction of
	// an Iostream_init causes it to be initialized.

int Length_init::count = 0 ;

Length_init::Length_init() 
{
	if ( count++ > 0 ) return ;
	// do these things only the first time this constructor is
	// called.

	iosi = new Iostream_init ;	// initialize iostream library
	xunit = ios::xalloc() ;	
		// get a unique index for the default unit. Using
		// ios::xalloc() to get an index instead of picking
		// one arbitrarily allows the Length library to coexist
		// with other libraries with user defined state variables.

	// Make sure state variable is initialized in predefined streams
	cin.pword(xunit) = 0 ;
	cout.pword(xunit) = 0 ;
	cerr.pword(xunit) = 0 ;
	clog.pword(xunit) = 0 ;
	} 

Length_init::~Length_init()
{
	if ( --count > 0 ) return ;
	
	// If we assume that a the Length_init destructor is called
	// for every Length_init that is ever constructed then we
	// reach here exactly once.  Namely when every Length_init
	// that is constructed has been unconstructed.  This should be
	// after any possible use of the library. 

	delete iosi ;
		// don't forget to unconstruct the Iostream_init.
		// The predefined streams are closed by the last
		// call to the Iostream_init destructor, so we have
		// to make sure they balance.
	} 


/**************
 *
 * What follows is a sample program.  It takes Lengths from its
 * command arguments or from cin. It prints each length on its
 * cout.
 *
 **************/

const Unit* feet = Unit::lookup("feet") ;
const Unit* meter = Unit::lookup("m") ;

void outlen(Length len)
{
	// Print len in various units.

	cout	<< setu(0);
	cout	<< len; 
	cout << " = "; 
	cout << setu(feet);
	cout << len;
	cout << " = ";
	cout << setu(meter);
	cout << len;
	cout << endl;

	// endl both inserts new line and flushes cout.  
	}

int main(int argc, char** argv )
{
	// This main is just a sample of how the above might be
	// used.  If it has arguments, it prints them.  If it
	// doesn't, then it reads Lengths from standard input and
	// prints them.
	
	Length len ;

	cout.precision(4) ;
		// Output with low precision to avoid machine dependencies.

	for ( int arg = 1 ; arg < argc ; ++arg ) {
		// We will use an "in core string stream" to do
		// the conversion

		istrstream a(argv[arg]) ;
		a >> len ;
		if ( !a ) {
			cerr << "cannot interpret " << argv[arg] << endl ;
			return 1 ;
			}
		outlen(len) ;
		} ;

	if ( argc <= 1 ) {
		// No arguments, so read from cin.

		while ( cin >> len ) outlen(len) ;

		// Normally we stop at end of file with both ios::eofbit
		// and ios::badbit set in the error state of cin.
		// The former because we reached eof, the latter because
		// the last call to the extractor didn't store a good
		// value into len.

		if ( cin.fail() && !cin.eof() ) {
			cerr << "badly formatted input" << endl ;
			return 1 ;
			}
		}
	
	if ( !cout ) {
		cerr << "output error" << endl ;
		return 1 ;
		}

	return 0 ;
	}

