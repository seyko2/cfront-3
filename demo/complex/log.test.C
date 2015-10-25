/*ident	"@(#)cls4:demo/complex/log.test.C	1.1" */
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

#define	NUM_TESTS	8

/*
 **************************************************************
 * 
 *                               j*theta
 * log(c), where c = a + bj = r*e       , is
 *
 *                         = log(r) + j*theta.
 * 
 * thus, we want to pick test cases where log(r) is easy
 * to find, like r = e**0 = 1, and r = e**1 = e.
 * 
 * similarily, we pick test cases where the angle theta
 * is a nice multiple of PI/4.
 * 
 * eight test cases:
 *                                  |
 *                                  |
 *                                  x
 *                                  |(0, PI)
 *                                  |
 *                                  |
 *                                  |
 *                                  |
 *                                  |
 *                                  |                 x         
 *                                  |                 (E/sqrt(2), E/sqrt(2))
 *                                  |
 *                                  |
 *                                  |
 *                                  |
 *                                  x        
 *                                  |(0, 1)
 *                                  |
 *                                  |       x         
 *                                  |       (1/sqrt(2), 1/sqrt(2))
 *                                  |
 *                                  |
 *                                  |
 * -----x---------------x-----------|----------x----------------x---------
 *      (-E, 0)          (-1, 0)    |           (1, 0)          (1, 0)
 *                                  |
 *                                  |
 * 					(cartesian coordinates)
 * 
 **************************************************************
 */

main()
{
	complex	c[NUM_TESTS];

	complex	ans[NUM_TESTS];

	complex	d;

	c[0] = complex(1, 0);			 ans[0] = complex(0, 0);
	c[1] = complex(1/M_SQRT2, 1/M_SQRT2);	 ans[1] = complex(0, M_PI_4);
	c[2] = complex(0, 1);			 ans[2] = complex(0, M_PI_2);
	c[3] = complex(-1, 0);			 ans[3] = complex(0, M_PI);
	c[4] = complex(M_E, 0);			 ans[4] = complex(1, 0);
	c[5] = complex(M_E/M_SQRT2, M_E/M_SQRT2);ans[5] = complex(1, M_PI_4);
	c[6] = complex(0, M_E);			 ans[6] = complex(1, M_PI_2);
	c[7] = complex(-M_E, 0);		 ans[7] = complex(1, M_PI);

	for ( int i = 0; i < NUM_TESTS; i++ ) {

		errno = 0;

		d = log(c[i]);

		if ( errno ) {
			cout << "*** test failed *** log("
				<< c[i] << "), errno=" << errno << "\n";
			continue;
		}

		if ( _ABS(real(d) - real(ans[i])) > X_EPS
				||
		     _ABS(imag(d) - imag(ans[i])) > X_EPS ) {

			cout << "*** test failed *** log("
				<< c[i] << ") = " << d << ", correct answer = "
				<< ans[i] << "\n";
		}
		else
			cout << "*** test passed *** log("
				<< c[i] << ") = " << d << "\n";
	}
	return 0;
}
