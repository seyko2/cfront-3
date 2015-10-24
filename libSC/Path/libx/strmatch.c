/*ident	"@(#)Path:libx/strmatch.c	3.1" */
/*
 * D. G. Korn
 * G. S. Fowler
 * AT&T Bell Laboratories
 *
 * match shell file patterns -- derived from Bourne and Korn shell gmatch()
 *
 *	sh pattern	egrep RE	description
 *	----------	--------	-----------
 *	*		.*		0 or more chars
 *	?		.		any single char
 *	[.]		[.]		char class
 *	[!.]		[^.]		negated char class
 *	*(.)		(.)*		0 or more of
 *	+(.)		(.)+		1 or more of
 *	?(.)		(.)?		0 or 1 of
 *	(.)		(.)		1 of
 *	@(.)		(.)		1 of
 *	a|b		a|b		a or b
 *	a&b				a and b
 *	!(.)				none of
 *
 * \ used to escape metacharacters
 *
 *	*, ?, (, |, &, ), [, \ must be \'d outside of [...]
 *	only ] must be \'d inside [...]
 *
 * BUG: unbalanced ) terminates top level pattern
 */

#ifdef MULTIBYTE

#include "national.h"

#define REGISTER

#define C_MASK		(3<<(7*ESS_MAXCHAR))	/* character classes	*/
#define getchar(x)	mb_getchar((unsigned char**)(&(x)))

static int		mb_getchar();

#else

#define REGISTER	register

#define getchar(x)	(*x++)

#endif

#define getsource(s,e)	(((s)>=(e))?0:getchar(s))

static char*		endmatch;
static int		minmatch;

static int		grpmatch();
static int		onematch();
static char*		gobble();

/*
 * strmatch compares the string s with the shell pattern p
 * returns 1 for match 0 otherwise
 */

int
strmatch_Path_ATTLC(s, p)
register char*	s;
char*		p;
{
	minmatch = 0;
	return(grpmatch(s, p, s + strlen(s), (char*)0));
}

/*
 * leading substring match
 * first char after end of substring returned
 * 0 returned if no match
 * m: (0-min, 1-max) match
 */

char*
submatch_Path_ATTLC(s, p, m)
register char*	s;
char*		p;
int		m;
{
	endmatch = 0;
	minmatch = !m;
	(void)grpmatch(s, p, s + strlen(s), (char*)0);
	return(endmatch);
}

/*
 * match any pattern in a group
 * | and & subgroups are parsed here
 */

static int
grpmatch(s, p, e, g)
char*		s;
register char*	p;
char*		e;
char*		g;
{
	register char*	a;

	do
	{
		a = p;
		do
		{
			if (!onematch(s, a, e, g)) break;
		} while (a = gobble(a, '&'));
		if (!a) return(1);
	} while (p = gobble(p, '|'));
	return(0);
}

/*
 * match a single pattern
 * e is the end (0) of the substring in s
 * g marks the start of a repeated subgroup pattern
 */

static int
onematch(s, p, e, g)
char*		s;
REGISTER char*	p;
char*		e;
char*		g;
{
	register int 	pc;
	register int 	sc;
	register int	n;
	char*		olds;
	char*		oldp;

	do
	{
		olds = s;
		sc = getsource(s, e);
		switch (pc = getchar(p))
		{
		case '(':
		case '*':
		case '?':
		case '+':
		case '@':
		case '!':
			if (pc == '(' || *p == '(')
			{
				char*	subp;

				s = olds;
				oldp = p - 1;
				subp = p + (pc != '(');
				if (!(p = gobble(subp, 0))) return(0);
				if (pc == '*' || pc == '?' || pc == '+' && oldp == g)
				{
					if (onematch(s, p, e, (char*)0)) return(1);
					if (!sc || !getsource(s, e)) return(0);
				}
				if (pc == '*' || pc == '+') p = oldp;
				pc = (pc != '!');
				do
				{
					if (grpmatch(olds, subp, s, (char*)0) == pc && onematch(s, p, e, oldp)) return(1);
				} while (s < e && getchar(s));
				return(0);
			}
			else if (pc == '*')
			{
				/*
				 * several stars are the same as one
				 */

				while (*p == '*')
					if (*(p + 1) == '(') break;
					else p++;
				oldp = p;
				switch (pc = getchar(p))
				{
				case '@':
				case '!':
				case '+':
					n = *p == '(';
					break;
				case '(':
				case '[':
				case '?':
				case '*':
					n = 1;
					break;
				case 0:
					endmatch = minmatch ? olds : e;
					/*FALLTHROUGH*/
				case '|':
				case '&':
				case ')':
					return(1);
				case '\\':
					if (!(pc = getchar(p))) return(0);
					/*FALLTHROUGH*/
				default:
					n = 0;
					break;
				}
				p = oldp;
				do
				{
					if ((n || pc == sc) && onematch(olds, p, e, (char*)0)) return(1);
					olds = s;
				} while (sc && (sc = getsource(s, e)));
				return(0);
			}
			else if (pc != '?' && pc != sc) return(0);
			break;
		case 0:
			endmatch = olds;
			if (minmatch) return(1);
			/*FALLTHROUGH*/
		case '|':
		case '&':
		case ')':
			return(!sc);
		case '[':
			{
				int	ok = 0;
				int	invert;

				n = 0;
				if (invert = *p == '!') p++;
				for (;;)
				{
					if (!(pc = getchar(p))) return(0);
					else if (pc == ']' && n)
					{
						if (ok != invert) break;
						return(0);
					}
					else if (pc == '-' && n && *p != ']')
					{
						if (!(pc = getchar(p)) || pc == '\\' && !(pc = getchar(p))) return(0);
#ifdef MULTIBYTE
						/*
						 * must be in same char set
						 */

						if ((n & C_MASK) != (pc & C_MASK))
						{
							if (sc == pc) ok = 1;
						}
						else
#endif
						if (sc >= n && sc <= pc || sc == pc) ok = 1;
					}
					else if (pc == '\\' && !(pc = getchar(p))) return(0);
					else
					{
						if (sc == pc) ok = 1;
						n = pc;
					}
				}
			}
			break;
		case '\\':
			if (!(pc = getchar(p))) return(0);
			/*FALLTHROUGH*/
		default:
			if (pc != sc) return(0);
			break;
		}
	} while (sc);
	return(0);
}

/*
 * gobble chars up to <sub> or ) keeping track of (...) and [...]
 * sub must be one of { '|', '&', 0 }
 * 0 returned if s runs out
 */

static char*
gobble(s, sub)
register char*	s;
register int	sub;
{
	register int	p = 0;
	register char*	b = 0;

	for (;;) switch (getchar(s))
	{
	case '\\':
		if (getchar(s)) break;
		/*FALLTHROUGH*/
	case 0:
		return(0);
	case '[':
		if (!b) b = s;
		break;
	case ']':
		if (b && b != (s - 1)) b = 0;
		break;
	case '(':
		if (!b) p++;
		break;
	case ')':
		if (!b && p-- <= 0) return(sub ? 0 : s);
		break;
	case '&':
		if (!b && !p && sub == '&') return(s);
		break;
	case '|':
		if (!b && !p)
		{
			if (sub == '|') return(s);
			else if (sub == '&') return(0);
		}
		break;
	}
}

#ifdef MULTIBYTE

/*
 * return the next char in (*address) which may be from one to three bytes
 * the character set designation is in the bits defined by C_MASK
 */

static int
mb_getchar(address)
unsigned char**	address;
{
	register unsigned char*	cp = *(unsigned char**)address;
	register int		c = *cp++;
	register int		size;
	int			d;

	if (size = echarset(c))
	{
		d = (size == 1 ? c : 0);
		c = size;
		size = in_csize(c);
		c <<= 7 * (ESS_MAXCHAR - size);
		if (d)
		{
			size--;
			c = (c << 7) | (d & ~HIGHBIT);
		}
		while (size-- > 0)
			c = (c << 7) | ((*cp++) & ~HIGHBIT);
	}
	*address = cp;
	return(c);
}

#endif

