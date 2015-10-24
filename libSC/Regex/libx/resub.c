/*ident	"@(#)Regex:libx/resub.c	3.1" */
/*
 * AT&T Bell Laboratories
 *
 * regular expression match substitution
 */

#include "relib.h"

static char	*sub();

/*
 * ed(1) style substitute using matches from last reexec()
 */

char*
resub_Regex_ATTLC(re, op, sp, dp, flags)
reprogram		*re;	/* compiled re				*/
register char		*op;	/* original subject string		*/
char			*sp;	/* substitution string			*/
register char		*dp;	/* destination string			*/
int			flags;	/* RE_ALL for all matches		*/
{
	register rematch	*mp;

	mp = re->match;
	do
	{
		while (op < mp->sp) *dp++ = *op++;
		dp = sub(sp, dp, mp);
		op = mp->ep;
	} while ((flags & RE_ALL) && *op && mp->sp != mp->ep && reexec_Regex_ATTLC(re, op));
	while (*dp++ = *op++);
	return(--dp);
}

/*
 * do a single substitution
 */

static char*
sub(sp, dp, mp)
register char		*sp;
register char		*dp;
register rematch	*mp;
{
	register int	i;
	char		*s;

	for (;;) switch (*dp = *sp++)
	{
	case 0:
		return(dp);
	case '\\':
		switch (i = *sp++)
		{
		case 0:
			sp--;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			i -= '0';
			if (mp[i].sp)
			{
				s = mp[i].sp;
				while (s < mp[i].ep) *dp++ = *s++;
			}
			break;
		default:
			*dp++ = i;
			break;
		}
		break;
	case '&':
		if (mp[0].sp)
		{
			s = mp[0].sp;
			while (s < mp[0].ep) *dp++ = *s++;
		}
		break;
	default:
		dp++;
		break;
	}
}
