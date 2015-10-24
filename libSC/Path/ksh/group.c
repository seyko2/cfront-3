/*ident "@(#)Path:ksh/group.c	3.1" */
#include <stdio.h>

main()
{
	system("groups");
	info();
	setegid(4);
	info();
	setrgid(22);
	info();
	setrgid(13);
	info();
}

info()
{
	int groups[10];
	register int n;
	printf ("real gid is %d, effective gid is %d, groups are:", getgid(), getegid());
	n = getgroups(10,groups);
	while(--n >= 0)
		printf ("\t%d", groups[n]);
	printf ("\n");
}
