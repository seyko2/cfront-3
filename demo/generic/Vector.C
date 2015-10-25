/*ident	"@(#)cls4:demo/generic/Vector.C	1.1" */
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

#include	<iostream.h>
#include	<vector.h>


static const int VectorSize = 10;	

/*  Instantiate the vector generic class declarations twice, once to create
**  vectors or integers and once for reals.  This is just to keep this
**  example simple.  You can have vectors of any class you declare as long
**  as the class has a default constructor and operator= is defined.
**
**  If this program consisted of more than one file, then each file
**  that used a vector type would need the following declaration for that
**  type.
*/

vectordeclare(int)
vectordeclare(float)

/*  Instantiate the code that actually implements each of the vector types.
**  If this program consisted of more than one file, then the following lines
**  must appear in only one of them.
*/

vectorimplement(int)
vectorimplement(float)


/*  A vector overflow handler.  We will have the vector class call this
**  function on errors instead of using its default action.  The default
**  is to print the error number and message passed to this function and
**  then to abort the process.  Specifying a handler allows you to attempt
**  to recover from errors.
*/

int
gotVectorError(int nbr, char* msgp)
{
	cout << "Caught error \"" << msgp << "\" (" << nbr << ")" << endl;
	return 0;
}


main()
{
	/*  Create a vector of VectorSize integers called "iv" and set
	**  each element of the vector to its subscript value.
	*/

	vector(int)	iv(VectorSize);

	for(int cnt = 0; cnt < VectorSize; cnt++){
		iv[cnt] = cnt;
	}

	/*  Now loop through the vector backwards and print each value.
	*/

	for(cnt = VectorSize - 1; cnt >= 0; cnt--){
		cout << iv[cnt] << " ";
	}
	cout << endl;

	/*  Specify an error handler to replace the default one provided
	**  by the vector(int) class.
	*/

	set_handler(vector,int, gotVectorError);

	/*  Cause a "subscript out of range" error in the vector(int)
	**  class.  If I were to do this in the vector(float) class now,
	**  the default message would print and then the program would
	**  be aborted.
	*/

	iv[VectorSize] = 99;

	/*  Specify an error handler to replace the default one provided
	**  by the vector(float) class.
	*/

	set_handler(vector,float, gotVectorError);

	/*  Now cause an error in the vector(float) class by trying to
	**  create a vector with a negative number of elements.
	*/

	vector(float)	rv(-3);

	/*  O.K. - enough fun for now.
	*/

	return 0;
}
