/* @(#) streambuf.c 1.2 1/27/86 17:48:04 */
/*ident	"@(#)cfront:lib/stream/streambuf.c	1.2"*/
#include "stream.h"

/*
	Allocate some space for the buffer.
	Returns:	EOF on error
			0 on success
*/
int streambuf.allocate()
{
	if (base == NULL) {
		if ((base = new char[BUFSIZE]) != NULL) {
			pptr = gptr = base;
			eptr = base + BUFSIZE;
			alloc = 1;
			return 0;
		} else
			return EOF;
	}
	return 0;
}

/*
	Come here on a put to a full buffer.  Allocate the buffer if 
	it is uninitialized.
	Returns:	EOF on error
			the argument on success
*/
virtual int streambuf.overflow(int c)
{
	if (allocate() == EOF) return EOF;
	if (c != EOF) *pptr++ = c;
	return c&0377;
}

/*
	Fill a buffer.
	Returns:	EOF on error or end of input
			next character on success
*/
virtual int streambuf.underflow()
{
	return EOF;
}
