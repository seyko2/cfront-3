/* @(#) io.c 1.2 1/27/86 17:47:36 */
/*ident	"@(#)cfront:lib/complex/io.c	1.2"*/
#include "complex.h"

ostream&
operator<<(ostream& s, complex a) 
{
	return s<<"( "<<real(a)<<", "<<imag(a)<<")";
}

istream&
operator>>(istream& s, complex& a)
/*
	f
	( f )
	( f , f )
*/
{
	double	re = 0, im = 0;
	char 	c = 0;

	s>>c;
	if (c == '(') {
		s>>re>>c;
		if (c == ',') s>>im>>c;
		if (c != ')') s.clear(_bad);
	}
	else {
		s.putback(c);
		s>>re;
	}

	if (s) a = complex(re,im);
	return s;
}
