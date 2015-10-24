/*ident	"@(#)Regex:libx/reexec.c	3.1" */
/*
 * AT&T Bell Laboratories
 *
 * regular expression executor
 */

#include "relib.h"
#include <ctype.h>

#include <stdio.h>
#include <assert.h>

#define LISTINCREMENT	8

typedef struct List
{
	Inst	*inst;		/* instruction of the thread		*/
	Subexp	se;		/* matched sub-expressions this thread	*/
} List;

static List	*tl, *nl;	/* this list, next list			*/
static List	*tle, *nle;	/* ends of this and next list		*/
static List	*list[2];
static List	*liste[2];
static int	listsize = LISTINCREMENT;

static Subexp	sempty;		/* empty set of matches			*/
static int	match;		/* true if match is found		*/

/*
 * note optimization in addinst:
 * 	*p must be pending when addinst called; if *l has been looked
 *	at already, the optimization is a bug.
 */

static char *
xmalloc(s)
size_t s;
{
	char *p = malloc(s);
	if (p == 0) 
	{
		fprintf(stderr, "malloc failed!\n");
		abort();
	}
	return p;
}

static List*
newthread(p, ip, sep)
register List	*p;		/* list to add to			*/
register Inst	*ip;		/* instruction to add			*/
register Subexp	*sep;		/* pointers to sub-expressions		*/
{

	for (; p->inst; p++)
		if (p->inst == ip)
		{
			if (sep->m[0].sp < p->se.m[0].sp) p->se = *sep;
			return(0);
		}
	p->inst = ip;
	p->se = *sep;
	(++p)->inst = 0;
	return(p);
}

static void
newmatch(mp, np)
register Subexp	*mp;
register Subexp	*np;
{
	if (!mp->m[0].sp || np->m[0].sp < mp->m[0].sp || np->m[0].sp == mp->m[0].sp && np->m[0].ep > mp->m[0].ep)
		*mp = *np;
}

#if DEBUG
static Subexp *gmp;
void marray()
{
	int i;
	for (i=0; i<10; i++)
		printf("%d: sp=%ld, ep=%ld\n", i, gmp->m[i].sp, gmp->m[i].ep);
}
#endif

static int case_sensitive;

/* cmap1 holds the precomputed finite function
*  	char c => islower(c)? toupper(c) : c
*  cmap2 holds the identify function
*  	char c => c
*/
static char cmap1[UCHAR_MAX+1];
static char cmap2[UCHAR_MAX+1];
static char *cmap = 0;

static void 
build_cmap()
{
	int i;

	/* build cmap1 and cmap2 just once */
	if (cmap == 0) {
		for (i=0; i<=UCHAR_MAX; i++) 
			cmap1[i] = cmap2[i] = i;
		for (i=0; i<=UCHAR_MAX; i++)
			if (islower(i))
				cmap1[i] = toupper(i);
	}
	cmap = (case_sensitive? cmap2 : cmap1);
}

static int
incclass(c, map)
char c;
char *map;
{
	/* this is tricky.  if we're doing case insensitive
	 * matching, and if c is alphabetic, then we've
	 * got to check for both lower(c) and upper(c)
	 * in the cclass.  otherwise just check for c.
	 */
	int cx;
	cx = c;
	cx &= 0377;

	if (case_sensitive) {
		return (tstbit(map, cx));
	}
	else {
		if (islower(cx))
			return (tstbit(map, cx) || tstbit(map, toupper(cx)));
		else if (isupper(cx))
			return (tstbit(map, cx) || tstbit(map, tolower(cx)));
		else
			return (tstbit(map, cx)); 
	}
}

int
reexec_Regex_ATTLC(progp, prog2, starts, _case_sensitive, _at_bol)
Prog	*progp;			/* program to run			*/
char	*starts;		/* string to run program on		*/
Prog	*prog2;			/* substring matching info goes here	*/
int	_case_sensitive;	/* case sensitive matching?		*/
int	_at_bol;		/* matching from beginning of line?	*/
{
	register int	flag = 0;
	register Inst	*inst;
	register List	*tlp;
	register char	*s;
	register Subexp *mp = &prog2->subexp;
	int		checkstart, startchar;

	case_sensitive = _case_sensitive;
	build_cmap();
	startchar = progp->startinst->type < TOKEN ? progp->startinst->type : 0;
#if DEBUG
	gmp = mp;
#endif
 Restart:
	match = 0;
	checkstart = startchar;
	sempty.m[0].sp = 0;
	if (mp) mp->m[0].sp = mp->m[0].ep = 0;
	if (!list[0])
	{
		list[0] = (List*)xmalloc(2 * listsize * sizeof(List));
		list[1] = list[0] + listsize;
		liste[0] = list[0] + listsize - 1;
		liste[1] = list[1] + listsize - 1;
	}
	list[0][0].inst = list[1][0].inst = 0;

	/*
	 * execute machine once for each character, including terminal '\0'
	 */

	s = starts;
	do
	{
		/*
		 * fast check for first char
		 */

		if (checkstart && cmap[*s] != cmap[startchar]) continue;
		tl = list[flag];
		tle = liste[flag];
		nl = list[flag ^= 1];
		nle = liste[flag];
		nl->inst = 0;

		/*
		 * add first instruction to this list
		 */

		sempty.m[0].sp = s;
		(void)newthread(tl, progp->startinst, &sempty);

		/*
		 * execute machine until this list is empty
		 */

		for (tlp = tl; inst = tlp->inst; tlp++)
		{
			/*
			 * assignment =
			 */
 Switchstmt:
			switch (inst->type)
			{
			case LBRA:
				tlp->se.m[inst->subid].sp = s;
				inst = inst->next;
				goto Switchstmt;
			case RBRA:
				tlp->se.m[inst->subid].ep = s;
				inst = inst->next;
				goto Switchstmt;
			case ANY:
				goto Addinst;
			case BOL:
				if (s == starts && _at_bol)
				{
					inst = inst->next;
					goto Switchstmt;
				}
				break;
			case EOL:
				if (!*s)
				{
					inst = inst->next;
					goto Switchstmt;
				}
				break;
			case CCLASS:
				if (incclass(*s, inst->cclass))
					goto Addinst;
				break;
			case OR:
				/*
				 * evaluate right choice later
				 */

				if (newthread(tlp, inst->right, &tlp->se) == tle)
					goto Realloc;

				/*
				 * efficiency: advance and re-evaluate
				 */

				inst = inst->left;
				goto Switchstmt;
			case SUBEXPR:
				{
					char	*ss;
					char	*ms = tlp->se.m[inst->subid].sp;
					char	*me = tlp->se.m[inst->subid].ep;

#if DEBUG
					{
						int	c;
						c = *me;
						*me = 0;
						error(-1, "subexpression %d ref=\"%s\"", inst->subid, ms);
						*me = c;
						error(-1, "subexpression %d src=\"%s\"", inst->subid, s);
					}
#endif
					if (ms == me)
					{
						inst = inst->next;
						goto Switchstmt;
					}
					for (ss = s; ms < me && cmap[*ss++] == cmap[*ms]; ms++);
					if (ms == me)
					{
						s = ss - 1;
						goto Addinst;
					}
				}
				break;
			case END:
				/*
				 * match!
				 */

				match = 1;
				tlp->se.m[0].ep = s;
				if (mp) newmatch(mp, &tlp->se);
				break;
			default:
				/*
				 * regular character
				 */

				assert(inst->type < TOKEN);
				if (cmap[inst->type] == cmap[*s])
				{
 Addinst:
					if (newthread(nl, inst->next, &tlp->se) == nle)
						goto Realloc;
				}
				break;
			}
		}
		checkstart = startchar && !nl->inst;
	} while (*s++);
	return(match);
 Realloc:
	free(list[0]);
	list[0] = 0;
	listsize += LISTINCREMENT;
	goto Restart;
}
