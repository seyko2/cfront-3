/* @(#) fcntl.h 1.2 1/27/86 17:46:41 */
/*ident	"@(#)cfront:incl/fcntl.h	1.2"*/
/* Flag values accessible to open(2) and fcntl(2) */
/*  (The first three can only be set by open) */
#define	O_RDONLY 0
#define	O_WRONLY 1
#define	O_RDWR	 2
#define	O_NDELAY 04	/* Non-blocking I/O */
#define	O_APPEND 010	/* append (writes guaranteed at the end) */
#define O_SYNC	 020	/* synchronous write option */

/* Flag values accessible only to open(2) */
#define	O_CREAT	00400	/* open with file create (uses third open arg)*/
#define	O_TRUNC	01000	/* open with truncation */
#define	O_EXCL	02000	/* exclusive open */

/* fcntl(2) requests */
#define	F_DUPFD	0	/* Duplicate fildes */
#define	F_GETFD	1	/* Get fildes flags */
#define	F_SETFD	2	/* Set fildes flags */
#define	F_GETFL	3	/* Get file flags */
#define	F_SETFL	4	/* Set file flags */
#define	F_GETLK	5	/* Get file lock */
#define	F_SETLK	6	/* Set file lock */
#define	F_SETLKW	7	/* Set file lock and wait */

/* file segment locking set data type - information passed to system by user */
struct flock {
	short	l_type;
	short	l_whence;
	long	l_start;
	long	l_len;		/* len = 0 means until end of file */
        short   l_pid;
        short   l_sysid;
};

/* file segment locking types */
	/* Read lock */
#define	F_RDLCK	01
	/* Write lock */
#define	F_WRLCK	02
	/* Remove lock(s) */
#define	F_UNLCK	03

extern int fcntl (int, int, int);
