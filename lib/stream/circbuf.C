/* @(#) circbuf.c 1.2 1/27/86 17:48:00 */
/*ident	"@(#)cfront:lib/stream/circbuf.c	1.2"*/
#include "stream.h"

/*
 *	Come here on a put to a full buffer.  Allocate the buffer if 
 *	it is uninitialized.
 *	Returns:	EOF on error
 *			the input character on success
 */
virtual int circbuf.overflow(int c)
{
	if (allocate() == EOF) return EOF;

	pptr = base;
	if (c != EOF) *pptr++ = c;

	return c & 0377;
}

/*
 *	Fill a buffer.
 *	Returns:	EOF on error or end of input
 *			next character on success
 */
virtual int circbuf.underflow()
{
	return EOF;
}
