/* @(#) grp.h 1.4 1/27/86 17:46:44 */
/*ident	"@(#)cfront:incl/grp.h	1.4"*/
#ifndef FILE
#       include <stdio.h>
#endif

struct	group {	/* see getgrent(3) */
	char	*gr_name;
	char	*gr_passwd;
	int	gr_gid;
	char	**gr_mem;
};

extern void endgrent ();
extern group *fgetgrent (FILE*);
extern group *getgrent ();
extern group *getgrgid (int);
extern group *getgrnam (const char*);
extern void setgrent ();
