/*ident	"@(#)Regex:libx/relib.h	3.1" */
/*
 * AT&T Bell Laboratories
 *
 * regular expression library private definitions
 */

#include <re.h>
#include <limits.h>
#include <sys/types.h>
#include <stdio.h>
#include <stddef.h>

#define NCLASS		16		/* max # [...] expressions	*/

#define elements(x)	(sizeof(x)/sizeof(x[0]))

#define clrbit(set,bit)	(set[(bit)/CHAR_BIT]&=~(1<<((bit)%CHAR_BIT)))
#define setbit(set,bit)	(set[(bit)/CHAR_BIT]|=(1<<((bit)%CHAR_BIT)))
#define tstbit(set,bit)	((set[(bit)/CHAR_BIT]&(1<<((bit)%CHAR_BIT)))!=0)

/* The layout of Subexp must be the same as the
* layout of reprogram (in re.h)
*/
typedef struct			/* sub-expression match table		*/
{
	rematch		m[RE_NMATCH + 1];
} Subexp;

typedef struct			/* character class bit vector		*/
{
	char		map[UCHAR_MAX / CHAR_BIT + 1];
} Class;

typedef struct Inst		/* machine instruction			*/
{
	int		type;	/* <TOKEN ==> literal, otherwise action	*/
	union
	{

	int		sid;	/* sub-expression id for RBRA and LBRA	*/
	struct Inst	*other;	/* for right child			*/
	char		*cls;	/* CCLASS bit vector			*/

	} u;
	struct Inst	*left;	/* left child				*/
} Inst;

#define	next	left
#define	subid	u.sid
#define right	u.other
#define cclass	u.cls

/*
 * NOTE: subexp must be the first element to match reprogram.match
 */

typedef struct				/* compiled program		*/
{
	Subexp		subexp;		/* sub-expression matches	*/
	int		flags;		/* RE_* flags			*/
	Inst		*startinst;	/* start pc			*/
	Class		class[NCLASS];	/* .data			*/
	Inst		firstinst[5];	/* .text			*/
} Prog;

/*
 * tokens and actions
 *
 *	TOKEN<=x<OPERATOR are tokens, i.e. operands for operators
 *	>=OPERATOR are operators, value == precedence
 */

#define BAD		-1
#define TOKEN		(UCHAR_MAX+1)
#define	ANY		(UCHAR_MAX+1)	/* `.' any character		*/
#define	NOP		(UCHAR_MAX+2)	/* no operation (internal)	*/
#define	BOL		(UCHAR_MAX+3)	/* `^' beginning of line	*/
#define	EOL		(UCHAR_MAX+4)	/* `$' end of line		*/
#define	CCLASS		(UCHAR_MAX+5)	/* `[]' character class		*/
#define SUBEXPR		(UCHAR_MAX+6)	/* `\#' sub-expression		*/
#define	END		(UCHAR_MAX+7)	/* terminate: match found	*/

#define	OPERATOR	(UCHAR_MAX+11)
#define	START		(UCHAR_MAX+11)	/* start, stack marker		*/
#define	RBRA		(UCHAR_MAX+12)	/* `)' right bracket		*/
#define	LBRA		(UCHAR_MAX+13)	/* `(' left bracket		*/
#define	OR		(UCHAR_MAX+14)	/* `|' alternation		*/
#define	CAT		(UCHAR_MAX+15)	/* concatentation (implicit)	*/
#define	STAR		(UCHAR_MAX+16)	/* `*' closure			*/
#define	PLUS		(UCHAR_MAX+17)	/* a+ == aa*			*/
#define	QUEST		(UCHAR_MAX+18)	/* a? == 0 or 1 a's		*/

extern char *malloc();
