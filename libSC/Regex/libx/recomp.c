/*ident	"@(#)Regex:libx/recomp.c	3.1" */
/*
 * AT&T Bell Laboratories
 *
 * regular expression compiler
 *
 * derived from the 9th edition regexp(3):
 *
 *	\[0-9] sub-expression references allowed in patterns
 *
 *	8 bit transparent
 *
 *	ed(1) style syntax supported as option
 */

#include "relib.h"

typedef struct Node			/* parser info			*/
{
	Inst	*first;
	Inst	*last;
} Node;

#define	NSTACK	20			/* misc stack depth		*/

static Node	andstack[NSTACK];
static Node	*andp;
static int	atorstack[NSTACK];
static int	*atorp;
static int	subidstack[NSTACK];	/* parallel to atorstack	*/
static int	*subidp;
static int	cursubid;		/* current sub-expression id	*/
static int	refsubid;		/* reference sub-expression id	*/
static int	lastwasand;		/* last token was operand	*/
static int	nbra;
static unsigned char	*exprp;		/* next source expression char	*/
static int	nclass;
static Class	*classp;
static Inst	*freep;
static Subexp	*mp;
static int	errors;
static int	flags;			/* RE_MATCH if '\[0-9]'		*/

static Prog 	*alloc_prog();
void refree_Regex_ATTLC();
static int	operator();
static int	pushand();
static int	pushator();
static int	evaluntil();
static char	*newclass();
static int	bldcclass();

#if DEBUG
static unsigned char *addr0;
#endif

static char errbuf[100];

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

static void
rcerror(s)
char	*s;
{
	errors++;
	strcpy(errbuf, s);
}

static void
reerr2(s, c)
register char	*s;
int		c;
{
	char	buf[100];

	sprintf(buf, "%s%c", s, (char)c);
	rcerror(buf);
}

static void
reerr2int(s, c)
register char	*s;
int		c;
{
	char	buf[100];

	sprintf(buf, "%s%d", s, c);
	rcerror(buf);
}

static void
cant(s)
char	*s;
{
	char	buf[100];

	sprintf(buf, "internal error: %s", s);
	rcerror(buf);
}

static Inst*
newinst(t)
int	t;
{
	freep->type = t;
	freep->left = 0;
	freep->right = 0;
	return(freep++);
}

static int
operand(t)
int	t;
{
	register Inst	*i;

	/*
	 * catenate is implicit
	 */

	if (lastwasand) 
		if (!operator(CAT))
			return 0;
	i = newinst(t);
	switch (t)
	{
	case CCLASS:
		i->cclass = classp[nclass - 1].map;
		break;
	case SUBEXPR:
		i->subid = refsubid;
		break;
	}
	if (!pushand(i, i))
		return 0;
	lastwasand = 1;
	return 1;
}

static int
operator(t)
register int	t;
{
	register int	thisisand = 0;

	switch (t)
	{
	case LBRA:
		if (++cursubid >= RE_NMATCH) {
			reerr2int("too many sub-expressions -- limit is ", RE_NMATCH-1);
			return 0;
		}
#if DEBUG
printf("subexpression %d start is at index %d\n", cursubid, exprp-addr0-1);
#endif
if (mp) mp->m[cursubid].sp = (char*)(exprp-1);
		nbra++;
		if (lastwasand) 
			if (!operator(CAT))
				return 0;
		if (!pushator(t))
			return 0;
		lastwasand = 0;
		break;
	case RBRA:
		if (--nbra < 0) {
			rcerror("unmatched )");
			return 0;
		}
		if (!evaluntil(t))
			return 0;
		lastwasand = 1;
		break;
	case STAR:
	case QUEST:
	case PLUS:
		thisisand = 1;
		/* fall through ... */
	default:
		if (!evaluntil(t) || !pushator(t))
			return 0;
		lastwasand = thisisand;
		break;
	}
	return 1;
}

static int
pushand(f, l)
Inst	*f;
Inst	*l;
{
	if (andp >= &andstack[NSTACK]) {
		cant("operand stack overflow");
		return 0;
	}
	andp->first = f;
	andp->last = l;
	andp++;
	return 1;
}

static int
pushator(t)
int	t;
{
	if (atorp >= &atorstack[NSTACK]) {
		cant("operator stack overflow");
		return 0;
	}
	*atorp++ = t;
	*subidp++ = cursubid;
	return 1;
}

static Node*
popand(op)
int	op;
{
	register Inst	*inst;

	if (andp <= &andstack[0])
	{
		reerr2("missing operand for ", op);
		return 0;
		/*
		inst = newinst(NOP);
		pushand(inst, inst);
		*/
	}
	return(--andp);
}

static int
popator()
{
	if (atorp <= &atorstack[0]) {
		cant("operator stack underflow");
		return BAD;
	}
	subidp--;
	return(*--atorp);
}

static int
evaluntil(pri)
register int	pri;
{
	register Node	*op1, *op2;
	register Inst	*inst1, *inst2;

	while (pri == RBRA || atorp[-1] >= pri)
	{
		switch(popator())
		{
		case BAD:
			return 0;
		case LBRA:
			/*
			 * must have been RBRA
			 */

			op1 = popand('(');
			if (op1 == 0)
				return 0;
			inst2 = newinst(RBRA);
			inst2->subid = *subidp;
#if DEBUG
printf("subexpression %d end is at index %d\n", *subidp, exprp-addr0-1);
#endif
if (mp) mp->m[*subidp].ep = (char*)exprp;
			op1->last->next = inst2;
			inst1 = newinst(LBRA);
			inst1->subid = *subidp;
			inst1->next = op1->first;
			if (!pushand(inst1, inst2))
				return 0;
			return 1;
		case OR:
			op2 = popand('|');
			if (op2 == 0)
				return 0;
			op1 = popand('|');
			if (op1 == 0)
				return 0;
			inst2 = newinst(NOP);
			op2->last->next = inst2;
			op1->last->next = inst2;
			inst1 = newinst(OR);
			inst1->right = op1->first;
			inst1->left = op2->first;
			if (!pushand(inst1, inst2))
				return 0;
			break;
		case CAT:
			op2 = popand(0);
			if (op2 == 0)
				return 0;
			op1 = popand(0);
			if (op1 == 0)
				return 0;
			op1->last->next = op2->first;
			if (!pushand(op1->first, op2->last))
				return 0;
			break;
		case STAR:
			op2 = popand('*');
			if (op2 == 0)
				return 0;
			inst1 = newinst(OR);
			op2->last->next = inst1;
			inst1->right = op2->first;
			if (!pushand(inst1, inst1))
				return 0;
			break;
		case PLUS:
			op2 = popand('+');
			if (op2 == 0)
				return 0;
			inst1 = newinst(OR);
			op2->last->next = inst1;
			inst1->right = op2->first;
			if (!pushand(op2->first, inst1))
				return 0;
			break;
		case QUEST:
			op2 = popand('?');
			if (op2 == 0)
				return 0;
			inst1 = newinst(OR);
			inst2 = newinst(NOP);
			inst1->left = inst2;
			inst1->right = op2->first;
			op2->last->next = inst2;
			if (!pushand(inst1, inst2))
				return 0;
			break;
		default:
			cant("unknown operator in evaluntil()");
			return 0;
			break;
		}
	}
	return 1;
}

static void
optimize(pp)
Prog	*pp;
{
	register Inst	*inst, *target;

	for (inst = pp->firstinst; inst->type != END; inst++)
	{
		target = inst->next;
		while (target->type == NOP) target = target->next;
		inst->next = target;
	}
}

#if DEBUG
static void
dumpstack()
{
	Node	*stk;
	int	*ip;

	printf("operators\n");
	for (ip = atorstack; ip < atorp; ip++)
		printf("0%o\n", *ip);
	printf("operands\n");
	for (stk = andstack; stk < andp; stk++)
		printf("0%o\t0%o\n", stk->first->type, stk->last->type);
}

static void
dump(pp)
Prog	*pp;
{
	Inst	*l;

	l = pp->firstinst;
	do
	{
		printf("%d:\t0%o\t%d\t%d\n",
			l-pp->firstinst, l->type,
			l->left-pp->firstinst, l->right-pp->firstinst);
	} while (l++->type);
}
#endif

static char*
newclass()
{
	register char	*p;
	register int	n;

	if (nclass >= NCLASS) {
		reerr2int("too many character classes -- limit is ", NCLASS);
		return 0;
	}
	p = classp[nclass++].map;
	for (n = 0; n < elements(classp[0].map); n++)
		p[n] = 0;
	return(p);
}

static int
lex()
{
	register int	c;

	switch(c = *exprp++)
	{
	case 0:
		c = END;
		exprp--;
		break;
	case '\\':
		switch (c = *exprp++)
		{
		case 0:
			exprp--;
			rcerror("trailing \\ is invalid");
			return BAD;
			break;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if ((c - '0') > cursubid) {
				reerr2("invalid sub-expression reference \\", c);
				return BAD;
			}
			else
			{
				refsubid = c - '0';
				flags |= RE_MATCH;
				c = SUBEXPR;
			}
			break;
		case '?':
			if (flags & RE_EDSTYLE) c = QUEST;
			break;
		case '+':
			if (flags & RE_EDSTYLE) c = PLUS;
			break;
		case '|':
			if (flags & RE_EDSTYLE) c = OR;
			break;
		case '(':
			if (flags & RE_EDSTYLE) c = LBRA;
			break;
		case ')':
			if (flags & RE_EDSTYLE) c = RBRA;
			break;
		}
		break;
	case '*':
		c = STAR;
		break;
	case '.':
		c = ANY;
		break;
	case '^':
		c = BOL;
		break;
	case '$':
		c = EOL;
		break;
	case '[':
		c = CCLASS;
		if (!bldcclass())
			return BAD;
		break;
	case '?':
		if (!(flags & RE_EDSTYLE)) c = QUEST;
		break;
	case '+':
		if (!(flags & RE_EDSTYLE)) c = PLUS;
		break;
	case '|':
		if (!(flags & RE_EDSTYLE)) c = OR;
		break;
	case '(':
		if (!(flags & RE_EDSTYLE)) c = LBRA;
		break;
	case ')':
		if (!(flags & RE_EDSTYLE)) c = RBRA;
		break;
	}
	return(c);
}

static int
nextc()
{
	register int	c;

	switch (c = *exprp++)
	{
	case 0:
		rcerror("missing ] in character class");
		return BAD;
		break;
	case '\\':
		if (!(c = *exprp++))
		{
			exprp--;
			rcerror("trailing \\ is invalid");
			return BAD;
		}
		break;
	case ']':
		c = 0;
		break;
	}
	return(c);
}

static int
bldcclass()
{
	register int	c1, c2;
	register char	*map;
	register int	negate;

	map = newclass();
	if (map == 0)
		return 0;
	/*
	 * we have already seen the '['
	 */

	if (*exprp == '^')
	{
		exprp++;
		negate = 1;
	}
	else negate = 0;
	if (*exprp == ']')
	{
		exprp++;
		setbit(map, ']');
	}
	if (*exprp == '-')
	{
		exprp++;
		setbit(map, '-');
	}
	while (c1 = c2 = nextc())
	{
		if (c1 == BAD)
			return 0; 
		if (*exprp == '-')
		{
			exprp++;
			c2 = nextc();
			if (c2 == BAD)
				return 0;
		}
		for (; c1 <= c2; c1++) setbit(map, c1);
	}
	if (negate)
		for (c1 = 0; c1 < elements(classp[0].map); c1++)
			map[c1] = ~map[c1];

	/*
	 * always exclude '\0'
	 */

	clrbit(map, 0);
	return 1;
}

#ifdef DEBUG
jump()
{
	int i;
	for (i=0; i<_JBLEN; i++)
		printf("%x ", beginning[i]);
	printf("\n");
}
#endif

reprogram*
recomp_Regex_ATTLC(s, slen, reflags, errmess)
char	*s;
size_t	slen;
int	reflags;
char	**errmess;
{
	register int	token;
	Prog		*pp;

#if DEBUG
	addr0 = s;
#endif
	*errmess = errbuf;

	/* get memory for the program
	 */
	pp = alloc_prog(slen);

	freep = pp->firstinst;
	classp = pp->class;
	mp = (reflags & RE_MATCH) ? &pp->subexp : 0;
	memset(&pp->subexp, '0', sizeof(Subexp));
	errors = 0;
	flags = reflags & ((1<<RE_EXTERNAL) - 1);

	/*
	 * go compile the sucker
	 */

	exprp = (unsigned char*)s;
	nclass = 0;
	nbra = 0;
	atorp = atorstack;
	andp = andstack;
	subidp = subidstack;
	lastwasand = 0;
	cursubid = 0;

	/*
	 * start with a low priority operator to prime parser
	 */

	if (!pushator(START - 1)) {
		refree_Regex_ATTLC(pp);
		return 0;
	}
	while ((token = lex()) != END)
	{
		if (token == BAD) {
			refree_Regex_ATTLC(pp);
			return 0;
		}
		else if (token >= OPERATOR) {
			if (!operator(token)) {
				refree_Regex_ATTLC(pp);
				return 0;
			}
		}
		else if (!operand(token)) {
			refree_Regex_ATTLC(pp);
			return 0;
		}
	}

	/*
	 * close with a low priority operator
	 */

	if (!evaluntil(START)) {
		refree_Regex_ATTLC(pp);
		return 0;
	}
	/*
	 * force END
	 */

	if (!operand(END) || !evaluntil(START)) {
		refree_Regex_ATTLC(pp);
		return 0;
	}
#if DEBUG
	dumpstack();
#endif
	if (nbra) {
		rcerror("unmatched (");
		refree_Regex_ATTLC(pp);
		return 0;
	}
	andp--;

	/*
	 * andp points to first and only operand
	 */

	pp->startinst = andp->first;
	pp->flags = flags;
#if DEBUG
	dump(pp);
#endif
	optimize(pp);
#ifdef DEBUG
	printf("start: %d\n", andp->first-pp->firstinst);
	dump(pp);
#endif
	if (errors)
	{
		refree_Regex_ATTLC(pp);
		return 0;
	}
	if (mp) 
	{
		mp->m[0].sp = s;
		mp->m[0].ep = (char*)exprp;
	}
	return((reprogram*)pp);
}

/*
 * hide the size of the program in the size_t preceding it,
 * so redupprog knows how big it is -- martin
 */

static Prog *
alloc_prog(slen)
size_t slen;
{
	size_t memsize = sizeof(size_t) + sizeof(Prog) + 3 * sizeof(Inst) * slen;
	char *mem = xmalloc(memsize);
	*(size_t*)mem = memsize;
	return (Prog*)(mem + sizeof(size_t));
}

void
refree_Regex_ATTLC(re)
reprogram	*re;
{
	char *mem = (char*)re - sizeof(size_t);
	free(mem);
}

#if 0
/* THIS DOESN'T WORK, since pointers in new prog will point into old prog!
*/
reprogram*
redupprog_Regex_ATTLC(re)
reprogram *re;
{
	size_t memsize;
	char *newmem, *mem;
	if (re == 0) return 0;
	mem = (char*)re - sizeof(size_t);
	memsize = *(size_t*)mem;
	newmem = xmalloc(memsize);
	memcpy(newmem, mem, memsize);
	return (reprogram*)(newmem + sizeof(size_t));
}
#endif

