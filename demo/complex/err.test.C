/*ident	"@(#)cls4:demo/complex/err.test.C	1.1" */
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

#define	MAX_EXPONENT	88.0
#define	MIN_EXPONENT	-88.0
#define	EXPGOOD		1e7
#define	SINH_GOOD	1e7

/*
 **************************************************************
 * 
 * here we test the handling of functions with arguments that
 * have no defined or representable values.  they should
 * produce a default conventional return value, set errno
 * to a non-zero value, and, in one case, display a message.
 * 
 * the expected results are summarized in the following
 * table:
 * 
 * _____________________________________________________________
 *          type               SING      OVERFLOW     UNDERFLOW
 * _____________________________________________________________
 *          errno              EDOM        ERANGE      ERANGE
 * _____________________________________________________________
 *  EXP:
 *   real too large/small       -        (+-H, +-H)    (0, 0)
 *   imag too large             -          (0, 0)         -
 * _____________________________________________________________
 *  LOG:
 *   arg = (0, 0)           M, (H, 0)        -            -
 * _____________________________________________________________
 *  SINH:
 *   real too large             -        (+-H, +-H)       -
 *   imag too large             -          (0, 0)         -
 * _____________________________________________________________
 *  COSH:
 *   real too large             -        (+-H, +-H)       -
 *   imag too large             -          (0, 0)         -
 * _____________________________________________________________
 * 
 **************************************************************
 */

main()
{
	complex		d;

	errno = 0;

	d = exp(complex(MAX_EXPONENT+1,1));

	if ( errno != ERANGE )
		cout << "*** test failed *** exp(complex(MAX_EXPONENT+1,1))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)-HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)-HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** exp(complex(MAX_EXPONENT+1,1))) = "
			<< d << ", correct answer = "
			<< complex(HUGE,HUGE) << "\n";

	else
		cout << "*** test passed *** exp(complex(MAX_EXPONENT+1,1))) = "
			<< d << "\n";

	errno = 0;

	d = exp(complex(MAX_EXPONENT+1,5*M_PI/4));

	if ( errno != ERANGE )
		cout << "*** test failed *** exp(complex(MAX_EXPONENT+1,5*M_PI/4))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)+HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)+HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** exp(complex(MAX_EXPONENT+1,5*M_PI/4))) = "
			<< d << ", correct answer = "
			<< complex(-HUGE,-HUGE) << "\n";

	else
		cout << "*** test passed *** exp(complex(MAX_EXPONENT+1,5*M_PI/4))) = "
			<< d << "\n";

	errno = 0;

	d = exp(complex(MIN_EXPONENT-1,0));

	if ( errno != ERANGE )
		cout << "*** test failed *** exp(complex(MIN_EXPONENT+1,0))), errno="
			<< errno << "\n";

	else if ( _ABS(real(d)) > X_EPS
				||
		  _ABS(imag(d)) > X_EPS )
		cout << "*** test failed *** exp(complex(MIN_EXPONENT+1,0))) = "
			<< d << ", correct answer = "
			<< complex(0,0) << "\n";

	else
		cout << "*** test passed *** exp(complex(MIN_EXPONENT+1,0))) = "
			<< d << "\n";

	errno = 0;

	d = exp(complex(0,EXPGOOD+1));

	if ( errno != ERANGE )
		cout << "*** test failed *** exp(complex(0,EXPGOOD+1))), errno="
			<< errno << "\n";

	else if ( _ABS(real(d)) > X_EPS
				||
		  _ABS(imag(d)) > X_EPS )
		cout << "*** test failed *** exp(complex(0,EXPGOOD+1))) = "
			<< d << ", correct answer = "
			<< complex(0,0) << "\n";

	else
		cout << "*** test passed *** exp(complex(0,EXPGOOD+1))) = "
			<< d << "\n";

	errno = 0;

	d = log(complex(0,0));

	if ( errno != EDOM )
		cout << "*** test failed *** log(complex(0,0))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)-HUGE)/HUGE) > X_EPS
				||
		  _ABS(imag(d)) > X_EPS )
		cout << "*** test failed *** log(complex(0,0))) = "
			<< d << ", correct answer = "
			<< complex(HUGE,0) << "\n";

	else
		cout << "*** test passed *** log(complex(0,0))) = "
			<< d << "\n";

	errno = 0;

	d = sinh(complex(MAX_EXPONENT+1,1));

	if ( errno != ERANGE )
		cout << "*** test failed *** sinh(complex(MAX_EXPONENT+1,1))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)-HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)-HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** sinh(complex(MAX_EXPONENT+1,1))) = "
			<< d << ", correct answer = "
			<< complex(HUGE,HUGE) << "\n";

	else
		cout << "*** test passed *** sinh(complex(MAX_EXPONENT+1,1))) = "
			<< d << "\n";

	errno = 0;

	d = sinh(complex(MAX_EXPONENT+1,5*M_PI/4));

	if ( errno != ERANGE )
		cout << "*** test failed *** sinh(complex(MAX_EXPONENT+1,5*M_PI/4))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)+HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)+HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** sinh(complex(MAX_EXPONENT+1,5*M_PI/4))) = "
			<< d << ", correct answer = "
			<< complex(-HUGE,-HUGE) << "\n";

	else
		cout << "*** test passed *** sinh(complex(MAX_EXPONENT+1,5*M_PI/4))) = "
			<< d << "\n";

	errno = 0;

	d = sinh(complex(MIN_EXPONENT-1,1));

	if ( errno != ERANGE )
		cout << "*** test failed *** sinh(complex(MIN_EXPONENT-1,1))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)+HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)-HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** sinh(complex(MIN_EXPONENT-1,1))) = "
			<< d << ", correct answer = "
			<< complex(HUGE,HUGE) << "\n";

	else
		cout << "*** test passed *** sinh(complex(MIN_EXPONENT-1,1))) = "
			<< d << "\n";

	errno = 0;

	d = sinh(complex(MIN_EXPONENT-1,5*M_PI/4));

	if ( errno != ERANGE )
		cout << "*** test failed *** sinh(complex(MIN_EXPONENT-1,5*M_PI/4))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)-HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)+HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** sinh(complex(MIN_EXPONENT-1,5*M_PI/4))) = "
			<< d << ", correct answer = "
			<< complex(-HUGE,-HUGE) << "\n";

	else
		cout << "*** test passed *** sinh(complex(MIN_EXPONENT-1,5*M_PI/4))) = "
			<< d << "\n";

	errno = 0;

	d = sinh(complex(0,SINH_GOOD+1));

	if ( errno != ERANGE )
		cout << "*** test failed *** sinh(complex(0,SINH_GOOD+1))), errno="
			<< errno << "\n";

	else if ( _ABS(real(d)) > X_EPS
				||
		  _ABS(imag(d)) > X_EPS )
		cout << "*** test failed *** sinh(complex(0,SINH_GOOD+1))) = "
			<< d << ", correct answer = "
			<< complex(0,0) << "\n";

	else
		cout << "*** test passed *** sinh(complex(0,SINH_GOOD+1))) = "
			<< d << "\n";

	errno = 0;

	d = cosh(complex(MAX_EXPONENT+1,1));

	if ( errno != ERANGE )
		cout << "*** test failed *** cosh(complex(MAX_EXPONENT+1,1))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)-HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)-HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** cosh(complex(MAX_EXPONENT+1,1))) = "
			<< d << ", correct answer = "
			<< complex(HUGE,HUGE) << "\n";

	else
		cout << "*** test passed *** cosh(complex(MAX_EXPONENT+1,1))) = "
			<< d << "\n";

	errno = 0;

	d = cosh(complex(MAX_EXPONENT+1,5*M_PI/4));

	if ( errno != ERANGE )
		cout << "*** test failed *** cosh(complex(MAX_EXPONENT+1,5*M_PI/4))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)+HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)+HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** cosh(complex(MAX_EXPONENT+1,5*M_PI/4))) = "
			<< d << ", correct answer = "
			<< complex(-HUGE,-HUGE) << "\n";

	else
		cout << "*** test passed *** cosh(complex(MAX_EXPONENT+1,5*M_PI/4))) = "
			<< d << "\n";

	errno = 0;

	d = cosh(complex(MIN_EXPONENT-1,1));

	if ( errno != ERANGE )
		cout << "*** test failed *** cosh(complex(MIN_EXPONENT-1,1))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)-HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)+HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** cosh(complex(MIN_EXPONENT-1,1))) = "
			<< d << ", correct answer = "
			<< complex(HUGE,HUGE) << "\n";

	else
		cout << "*** test passed *** cosh(complex(MIN_EXPONENT-1,1))) = "
			<< d << "\n";

	errno = 0;

	d = cosh(complex(MIN_EXPONENT-1,5*M_PI/4));

	if ( errno != ERANGE )
		cout << "*** test failed *** cosh(complex(MIN_EXPONENT-1,5*M_PI/4))), errno="
			<< errno << "\n";

	else if ( _ABS((real(d)+HUGE)/HUGE) > X_EPS
				||
		  _ABS((imag(d)-HUGE)/HUGE) > X_EPS )
		cout << "*** test failed *** cosh(complex(MIN_EXPONENT-1,5*M_PI/4))) = "
			<< d << ", correct answer = "
			<< complex(-HUGE,-HUGE) << "\n";

	else
		cout << "*** test passed *** cosh(complex(MIN_EXPONENT-1,5*M_PI/4))) = "
			<< d << "\n";

	errno = 0;

	d = cosh(complex(0,SINH_GOOD+1));

	if ( errno != ERANGE )
		cout << "*** test failed *** cosh(complex(0,SINH_GOOD+1))), errno="
			<< errno << "\n";

	else if ( _ABS(real(d)) > X_EPS
				||
		  _ABS(imag(d)) > X_EPS )
		cout << "*** test failed *** cosh(complex(0,SINH_GOOD+1))) = "
			<< d << ", correct answer = "
			<< complex(0,0) << "\n";

	else
		cout << "*** test passed *** cosh(complex(0,SINH_GOOD+1))) = "
			<< d << "\n";
	return 0;
}
