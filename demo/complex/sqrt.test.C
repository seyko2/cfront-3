/*ident	"@(#)cls4:demo/complex/sqrt.test.C	1.1" */
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

#define	NUM_TESTS	5

/*
 **************************************************************
 * 
 * where c = r*cos(theta) + j*r*sin(theta),
 * 
 *    sqrt(c) = sqrt(r)*(cos(theta/2) + j*sin(theta/2)).
 * 
 * thus, we pick test points so that theta/2 has nice
 * sine and cosine values, especially when multiplied
 * by sqrt(r).
 * 
 * five test cases:
 *                          |
 *                          x (2, PI/2)
 *                          | (0, 2)
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *               (1, PI)    |(0, 0)    (1, 0)
 * --------------x----------x----------x------------------
 *               (-1, 0)    |(0, 0)    (1, 0)
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          |
 *                          x (2, 3PI/2)
 *                          | (0, -2)
 * 					(polar coordinates)
 * 						over
 * 					(cartesian coordinates)
 * 
 **************************************************************
 */

main()
{
	complex	c[NUM_TESTS];

	complex	ans[NUM_TESTS];

	complex	d;

	c[0] = complex(0, 0);		ans[0] = complex(0, 0);
	c[1] = complex(1, 0);		ans[1] = complex(1, 0);
	c[2] = complex(0, 2);		ans[2] = complex(1, 1);
	c[3] = complex(-1, 0);		ans[3] = complex(0, 1);
	c[4] = complex(0, -2);		ans[4] = complex(1, -1);

	for ( int i = 0; i < NUM_TESTS; i++ ) {

		errno = 0;

		d = sqrt(c[i]);

		if ( errno ) {
			cout << "*** test failed *** sqrt("
				<< c[i] << "), errno=" << errno << "\n";
			continue;
		}

		if ( _ABS(real(d) - real(ans[i])) > X_EPS
					||
		     _ABS(imag(d) - imag(ans[i])) > X_EPS ) {
			cout << "*** test failed *** sqrt("
				<< c[i] << ") = " << d << ", correct answer = "
				<< ans[i] << "\n";
		}
		else
			cout << "*** test passed *** sqrt("
				<< c[i] << ") = " << d << "\n";
	}
	return 0;
}
