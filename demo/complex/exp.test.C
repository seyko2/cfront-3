/*ident	"@(#)cls4:demo/complex/exp.test.C	1.1" */
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

#define	NUM_TESTS	12

/*
 **************************************************************
 * 
 * exp(c), where c = a + bj, is e**(a+bj) = (e**a)*(e**(b*j)).
 * now, e**(j*theta) = cos(theta) + j*sin(theta).
 * so,
 *       c      a+bj      a
 *     e    = e      =  e   * ( cos(b) + j*sin(b) )
 * 
 * hence, for test cases, we'll pick nice values of a,
 * like 0, 1, and -1, and nice values of b, i.e.,
 * where the sine and cosine values * have #define values
 * available, like multiples of PI/4.
 * 
 * twelve test cases:
 *                          |
 *      x                   x                   x         
 *      (-1, PI)            |(0, PI)            (1, PI)
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
 *      x                   x                   x         
 *      (-1, PI/2)          |(0, PI/2)          (1, PI/2)
 *                          |
 *                          |
 *                          |
 *                          |
 *      x                   x                   x         
 *      (-1, PI/4)          |(0, PI/4)          (1, PI/4)
 *                          |
 *                          |
 *                          |
 *                          |
 * -----x-------------------x-------------------x---------
 *      (-1, 0)             |(0, 0)          (1, 0)
 *                          |
 * 					(cartesian coordinates)
 * 
 **************************************************************
 */

main()
{
	complex	c[NUM_TESTS];

	complex	ans[NUM_TESTS];

	complex	d;

	c[0] = complex(0, 0);		ans[0] = complex(1, 0);
	c[1] = complex(0, M_PI_4);	ans[1] = complex(1/M_SQRT2, 1/M_SQRT2);
	c[2] = complex(0, M_PI_2);	ans[2] = complex(0, 1);
	c[3] = complex(0, M_PI);	ans[3] = complex(-1, 0);
	c[4] = complex(1, 0);		ans[4] = complex(M_E, 0);
	c[5] = complex(1, M_PI_4);	ans[5] = complex(M_E/M_SQRT2, M_E/M_SQRT2);
	c[6] = complex(1, M_PI_2);	ans[6] = complex(0, M_E);
	c[7] = complex(1, M_PI);	ans[7] = complex(-M_E, 0);
	c[8] = complex(-1, 0);		ans[8] = complex(1/M_E, 0);
	c[9] = complex(-1, M_PI_4);	ans[9] = complex(1/(M_E*M_SQRT2),
							1/(M_E*M_SQRT2));
	c[10] = complex(-1, M_PI_2);	ans[10] = complex(0, 1/M_E);
	c[11] = complex(-1, M_PI);	ans[11] = complex(-1/M_E, 0);

	for ( int i = 0; i < NUM_TESTS; i++ ) {

		errno = 0;

		d = exp(c[i]);

		if ( errno ) {
			cout << "*** test failed *** exp("
				<< c[i] << "), errno=" << errno << "\n";
			continue;
		}

		if ( _ABS(real(d) - real(ans[i])) > X_EPS
				||
		     _ABS(imag(d) - imag(ans[i])) > X_EPS ) {

			cout << "*** test failed *** exp("
				<< c[i] << ") = " << d << ", correct answer = "
				<< ans[i] << "\n";
		}
		else
			cout << "*** test passed *** exp("
				<< c[i] << ") = " << d << "\n";
	}
	return 0;
}
