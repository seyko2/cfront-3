/* @(#) generic.c 1.2 1/27/86 17:47:45 */
/*ident	"@(#)cfront:lib/generic/generic.c	1.2"*/
#include <stdio.h>

extern
genericerror(int n, char* s)
{
	fprintf(stderr,"%s\n",s?s:"error in generic library function",n);
	abort();
	return 0;
};
