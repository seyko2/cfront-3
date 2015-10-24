/*ident "@(#)Path:ksh/io.c	3.1" */
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Rewritten by David Korn
 * Stolen by Martin Carroll
 * AT&T Bell Laboratories
 *
 */

#include	"defs.h"

/*
 * returns access information on open file <fd>
 * returns -1 for failure, 0 for success
 * <mode> is the same as for access()
 */

io_access(fd,mode)
register int mode;
{
	register int flags;
	register struct fileblk *fp;
#ifndef F_GETFL
	struct stat statb;
#endif /* F_GETFL */
	if(mode==X_OK)
		return(-1);
#if 0
	if(fp=io_ftable[fd])
	{
		if(mode==F_OK)
			return(0);
		if(mode==R_OK && fp->flag&(IORW|IOREAD))
			return(0);
		if(mode==W_OK && fp->flag&(IORW|IOWRT))
			return(0);
		return(-1);
	}
#endif /* 0 */
#ifdef F_GETFL
	flags = fcntl(fd,F_GETFL,0);
#else
	flags = fstat(fd,&statb);
#endif /* F_GETFL */
	if(flags < 0)
		return(-1);
#ifdef F_GETFL
	if(mode==R_OK && (flags&1))
		return(-1);
	if(mode==W_OK && !(flags&3))
		return(-1);
#endif /* F_GETFL */
	return(0);
}
