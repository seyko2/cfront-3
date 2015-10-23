/* @(#) ftw.h 1.3 1/27/86 17:46:42 */
/*ident	"@(#)cfront:incl/ftw.h	1.3"*/
/*
 *	Codes for the third argument to the user-supplied function
 *	which is passed as the second argument to ftw
 */

#define	FTW_F	0	/* file */
#define	FTW_D	1	/* directory */
#define	FTW_DNR	2	/* directory without read permission */
#define	FTW_NS	3	/* unknown type, stat failed */

typedef int (*PF) ();
extern int ftw (const char*, PF, int);
