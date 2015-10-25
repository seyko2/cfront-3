/*ident	"@(#)cls4:demo/complex/polar.test.C	1.1" */
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
 * we test the polar conversion function by supplying
 * the polar coordinates of the nine standard test cases
 * from each quadrant, and expect to see their Cartesian forms.
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
	struct {
		double	x,y;
	} poles[NUM_TESTS];

	complex	ans[NUM_TESTS];

	complex	c;

	poles[0].x = 0;		poles[0].y = 0;		ans[0] = complex(0, 0);
	poles[1].x = 1;		poles[1].y = 0;		ans[1] = complex(1, 0);
	poles[2].x = M_SQRT2;	poles[2].y = M_PI_4;	ans[2] = complex(1, 1);
	poles[3].x = 1;		poles[3].y = M_PI_2;	ans[3] = complex(0, 1);
	poles[4].x = M_SQRT2;	poles[4].y = 3*M_PI_4;	ans[4] = complex(-1, 1);
	poles[5].x = 1;		poles[5].y = M_PI;	ans[5] = complex(-1, 0);
	poles[6].x = M_SQRT2;	poles[6].y = 5*M_PI_4;	ans[6] = complex(-1, -1);
	poles[7].x = 1;		poles[7].y = 3*M_PI_2;	ans[7] = complex(0, -1);
	poles[8].x = M_SQRT2;	poles[8].y = 7*M_PI_4;	ans[8] = complex(1, -1);

	for ( int i = 0; i < NUM_TESTS; i++ ) {

		errno = 0;

		c = polar(poles[i].x,poles[i].y);

		if ( errno ) {
			cout << "*** test failed *** polar("
				<< poles[i].x << ", " << poles[i].y  << "),errno ="
				<< errno << "\n";
			continue;
		}

		if ( _ABS(real(c) - real(ans[i])) > X_EPS
				||
		     _ABS(imag(c) - imag(ans[i])) > X_EPS ) {

			cout << "*** test failed *** polar("
				<< poles[i].x << ", " << poles[i].y  << ") = " << c
				<< ", correct answer = " << ans[i] << "\n";
		}
		else
			cout << "*** test passed *** polar("
				<< poles[i].x << ", " << poles[i].y  << ") = "
				<< c << "\n";
	}
	return 0;
}
