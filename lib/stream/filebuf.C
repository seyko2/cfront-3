/* @(#) filebuf.c 1.2 1/27/86 17:48:01 */
/*ident	"@(#)cfront:lib/stream/filebuf.c	1.2"*/
#include "stream.h"

/* define some UNIX calls */
extern int open (char *, int);
extern int close (int);
extern long lseek (int, long, int);
extern int read (int, char *, unsigned);
extern int write (int, char *, unsigned);
extern int creat (char *, int);

/*
 *	Open a file with the given mode.
 *	Return:		NULL if failure
 *			this if success
 */
filebuf* filebuf.open (char *name, open_mode om)
{
	switch (om) {
	case input:	
		fd = ::open(name, 0);
		break;
	case output:	
		if ((fd = ::open(name,1)) < 0) fd = creat(name, 664);
		break;
	case append:	
		if ((fd = ::open(name,1)) < 0)
			fd = creat(name, 664);
		else {
			if (lseek(fd, 0, 2) < 0) {
				(void)::close(fd);
				fd = -1;
			}
		}
		break;
	}

	if (fd < 0) return NULL;

	opened = 1;
	return this;
}

/*
 *	Empty an output buffer.
 *	Returns:	EOF on error
 *			0 on success
 */
int filebuf.overflow(int c)
{
	if (!opened || allocate()==EOF) return EOF;

	if (base == eptr) {
		/* unbuffered IO */
		if (c != EOF) {
			*pptr = c;
			if (write(fd, pptr, 1) != 1) return EOF;
		}
	} else {
		/* buffered IO */
		if (pptr > base)
			if (write(fd, base, pptr-base) != pptr-base) return EOF;
		pptr = gptr = base;
		if (c != EOF) *pptr++ = c;
	}
	return c & 0377;
}

/*
 *	Fill an input buffer.
 *	Returns:	EOF on error or end of input
 *			next character on success
 */
int filebuf.underflow()
{
	int count;

	if (!opened || allocate()==EOF) return EOF;

	if ((count=read(fd, base, eptr-base)) < 1) return EOF;

	gptr = base;
	pptr = base + count;
	return *gptr & 0377;
}
