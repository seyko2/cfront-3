/* @(#) pwd.h 1.4 1/27/86 17:46:54 */
/*ident	"@(#)cfront:incl/pwd.h	1.4"*/
#ifndef FILE
#       include <stdio.h>
#endif

struct passwd {
	char	*pw_name;
	char	*pw_passwd;
	int	pw_uid;
	int	pw_gid;
	char	*pw_age;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

struct comment {
	char	*c_dept;
	char	*c_name;
	char	*c_acct;
	char	*c_bin;
};

extern passwd *getpwent ();
extern passwd *getpwuid ();
extern passwd *getpwnam (const char*);
extern passwd *fgetpwent (FILE*);

extern void setpwent ();
extern void endpwent ();
extern int putpwent (const passwd*, FILE*);
