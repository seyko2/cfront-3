/*ident	"@(#)aoutdem:parse.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

/* This is a brute force predictive parser for subgrammar of dbx -g strings 
*  that we care about.  For the whole grammar, see dbx(5).
*/

/* Please excuse anything in this file that strikes you as disgusting -- 
*  I assure you, it's all in the name of speed.
*/

#if defined(BSD) || defined(SUNOS)

#include "aoutdem.h"
#include <setjmp.h>


// not consts since we might have to realloc
static int newDbxStringLen = 100;
static char *newDbxString = new char[newDbxStringLen];
static int available;	// space still open in newDbxString
static char *newi;  	// current location in newDbxString 
			// (i.e., always points to the null at the end of the current string)

static char *oldDbxString;
static char *oldi;  // current location in oldDbxString
static char LA;	    // == *oldi

// not consts since we might have to realloc
static int thenameLen = 50;
static char *thename = new char[thenameLen];


static void doTheRealloc()
{
	available += newDbxStringLen;
	newDbxStringLen <<= 1;
	char *tmp = new char[newDbxStringLen];
	strcpy(tmp, newDbxString);
	delete newDbxString;
	newDbxString = tmp;
	newi = newDbxString + strlen(newDbxString);
}

static inline void checkForRealloc(int len)
{
	if (len >= available)  // i think just ">" would be ok, but i don't trust myself.
	{
		doTheRealloc();
	}
}

static void appendToNewDbxString(const char *s, int slen)
{
	checkForRealloc(slen);
	strcpy(newi, s);
	newi += slen;
	available -= slen;
}

static void appendToNewDbxString(char c)
{
	checkForRealloc(1);
	*newi++ = c;
	available--;
	*newi = NULL;
}

static void initParser(char *oldString)
{
	newi = newDbxString;
	*newi = NULL;
	available = newDbxStringLen - 1;

	oldi = oldDbxString = oldString;
	LA = *oldi;
}

inline static void advance()
{
	if (*oldi != NULL)
	{
		oldi++;
		LA = *oldi;
	}
}

inline static void appendToNewDbxStringThenAdvance()
{
	appendToNewDbxString(LA);
	advance();
}

static jmp_buf beginning;

/* the string is not in our subgrammar, so just collect the rest 
*  of it into newDbxString, then longjmp back to the beginning.
*/
static void fail()
{
	while (LA != NULL)
	{
		appendToNewDbxStringThenAdvance();
	}
	longjmp(beginning, 1);
}

inline static void match(char c)
{
	if (LA == c)
		appendToNewDbxStringThenAdvance();
	else
		fail();
}

/*
static void match(char *s)
{
	while (*s)
		match(*s++);
}
*/

/* match one of the chars in s
*/
/*
static void matchone(char *s)
{
	while (*s)
	{
		if (LA == *s++)
		{
			appendToNewDbxStringThenAdvance();
			return;
		}
	}
	fail();
}
*/

static void matchthru(char c)
{
	while (LA != c && LA != NULL)
		appendToNewDbxStringThenAdvance();
	match(c);
}

static void integer()
{
	if (LA == '-')
		appendToNewDbxStringThenAdvance();

	if (isdigit(LA))
	{
		appendToNewDbxStringThenAdvance();
		while (isdigit(LA))
			appendToNewDbxStringThenAdvance();
	}
	else
		fail();
}

static void aggregateHeader()
{
	match('T');
#ifdef SUNOS
	match('(');
#endif
	integer();
#ifdef SUNOS
	match(',');
	integer();
	match(')');
#endif
	match('=');
	if (LA == 's')
		match('s');
	else
		match('u');
	integer();
}

/* puts name in thename, and returns length of name
*/
static int name()
{
	if (!isalpha(LA) && LA != '_')
	{
		fail();
		return 0;
	}
	else 
	{
		char *from = oldi;
		MARK(nameLoop);
		for (advance(); isalnum(LA) || LA == '_'; advance())
			;
		int len = oldi-from;
		if (len+1 > thenameLen)
		{
			delete thename;
			thename = new char[thenameLen = len+1];
		}
 		strncpy(thename, from, len);
		thename[len] = NULL;
		return len;
	}
}

static int lastEntryWasContinued = 0;

static void aggregateMembers(int stetype)
{
	while (LA != NULL)
	{
		if (isalpha(LA) || LA == '_')
		{
			int namelen = name();
			String s = demangleAndSimplify(String(thename, namelen), 1, stetype);
			appendToNewDbxString(s, s.length());
			match(':');
		}
		else if (LA == '\\')
		{
			lastEntryWasContinued = 1;
			fail();
		}
		matchthru(';');
	}
}

static void parseTheString(int stetype)
{
	if (lastEntryWasContinued)
	{
		lastEntryWasContinued = 0;
		aggregateMembers(stetype);
	}
	else
	{
		int namelen = name();
		if (LA == NULL)  // then it's probably text or data.
		{
			String s = demangleAndSimplify(String(thename, namelen), 0, stetype);
			appendToNewDbxString(s, s.length());
		}
		else if (LA != ':') // heaven only knows what this entry is.  probably a file name.
		{
			appendToNewDbxString(thename, namelen);
			fail();
		}
		else
		{
			String s = demangleAndSimplify(String(thename, namelen), 0, stetype);
			appendToNewDbxString(s, s.length());
			match(':');
			aggregateHeader();
			aggregateMembers(stetype);
		}
	}
}

String parseDbxString(char *s, int stetype)
{
	initParser(s);
	if (setjmp(beginning)) 
	{
		/* parsing returns here on fail()ure */
	}
	else
	{
		parseTheString(stetype);
	}
	if (verboseDbx)
	{
		cerr << "\n\t" << oldDbxString << " =>\n\n\t" << newDbxString << endl;
	}
	return String(newDbxString, newi-newDbxString);
}


#endif
