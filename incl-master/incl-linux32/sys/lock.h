/* @(#) lock.h 1.2 1/27/86 17:47:07 */
/*ident	"@(#)cfront:incl/sys/lock.h	1.2"*/
/*
 * flags for locking procs and texts
 */
#define	UNLOCK	 0
#define	PROCLOCK 1
#define	TXTLOCK	 2
#define	DATLOCK	 4

extern int plock (int);
