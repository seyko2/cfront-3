/*ident	"@(#)cls4:demo/complex/pow.test.C	1.1" */
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
#include	<complex.h>
#include	<values.h>

#define	NUM_TESTS	9

/*
 **************************************************************
 * 
 * here we test the power function on the nine standard test
 * cases from each quadrant by * performing the operation
 * with a single test point, y = ( 2, 0).  however, y is
 * represented in three different ways:  as an int, as a
 * double, and as a complex.
 * 
 * nine test cases:
 *                          |
 *                          |
 *                          |
 *      x (SQRT(2), 3*PI/4) x (1, PI/2)        x (SQRT(2), PI/4)
 *        (-1, 1)           | (0, 1)             (1, 1)
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *      (1, PI)             |(0, 0)          (1, 0)
 * -----x-------------------x-------------------x---------
 *      (-1, 0)             |(0, 0)          (1, 0)
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *      x (SQRT(2), 5*PI/4) x (1, 3*PI/2)       x (SQRT(2), 7*PI/4)
 *        (-1, -1)          | (0, -1)             (1, -1)
 *                          |
 *                          |
 * 					(polar coordinates)
 * 						over
 * 					(cartesian coordinates)
 * 
 **************************************************************
 */

main()
{
	complex	x[NUM_TESTS];

	complex	ans[NUM_TESTS];

	complex	c;

	x[0] = complex(0, 0);		ans[0] = complex(0, 0);
	x[1] = complex(1, 0);		ans[1] = complex(1, 0);
	x[2] = complex(1, 1);		ans[2] = complex(0, 2);
	x[3] = complex(0, 1);		ans[3] = complex(-1, 0);
	x[4] = complex(-1, 1);		ans[4] = complex(0, -2);
	x[5] = complex(-1, 0);		ans[5] = complex(1, 0);
	x[6] = complex(-1, -1);		ans[6] = complex(0, 2);
	x[7] = complex(0, -1);		ans[7] = complex(-1, 0);
	x[8] = complex(1, -1);		ans[8] = complex(0, -2);

	for ( int i = 0; i < NUM_TESTS; i++ ) {

		int y = 2;

		errno = 0;

		c = pow(x[i], y);

		if ( errno ) {
			cout << "*** test failed *** pow("
				<< x[i] << ", (int)" << y  << "), errno = "
				<< errno << "\n";
			continue;
		}

		if ( _ABS(real(c) - real(ans[i])) > X_EPS
				||
		     _ABS(imag(c) - imag(ans[i])) > X_EPS ) {

			cout << "*** test failed *** pow("
				<< x[i] << ", (int)" << y  << ") = " << c
				<< ", correct answer = " << ans[i] << "\n";
		}
		else
			cout << "*** test passed *** pow("
				<< x[i] << ", (int)" << y  << ") = "
				<< c << "\n";
	}

	for ( i = 0; i < NUM_TESTS; i++ ) {

		double y = 2;

		errno = 0;

		c = pow(x[i], y);

		if ( errno ) {
			cout << "*** test failed *** pow("
				<< x[i] << ", (double)" << y  << "), errno = "
				<< errno << "\n";
			continue;
		}

		if ( _ABS(real(c) - real(ans[i])) > X_EPS
				||
		     _ABS(imag(c) - imag(ans[i])) > X_EPS ) {

			cout << "*** test failed *** pow("
				<< x[i] << ", (double)" << y  << ") = " << c
				<< ", correct answer = " << ans[i] << "\n";
		}
		else
			cout << "*** test passed *** pow("
				<< x[i] << ", (double)" << y  << ") = "
				<< c << "\n";
	}

	for ( i = 0; i < NUM_TESTS; i++ ) {

		complex y = complex(2, 0);

		errno = 0;

		c = pow(x[i], y);

		if ( errno ) {
			cout << "*** test failed *** pow("
				<< x[i] << ", " << y  << "), errno = "
				<< errno << "\n";
			continue;
		}

		if ( _ABS(real(c) - real(ans[i])) > X_EPS
				||
		     _ABS(imag(c) - imag(ans[i])) > X_EPS ) {

			cout << "*** test failed *** pow("
				<< x[i] << ", " << y  << ") = " << c
				<< ", correct answer = " << ans[i] << "\n";
		}
		else
			cout << "*** test passed *** pow("
				<< x[i] << ", " << y  << ") = "
				<< c << "\n";
	}
	return 0;
}
