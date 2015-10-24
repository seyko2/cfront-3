/*ident	"@(#)Regex:libx/reerror.c	3.1" */
/*
 * AT&T Bell Laboratories
 *
 * regular expression error routine
 */

#include <re.h>
#include <error.h>

void
reerror_Regex_ATTLC(s)
char*	s;
{
	printf("Regex error is %s\n", s);
}
