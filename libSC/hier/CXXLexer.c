/*ident	"@(#)hier:CXXLexer.c	3.1" */
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

#include "CXXLexer.h"

#include <stdlib.h>
#include <ctype.h>

extern void syntaxErrorNoFailure(char *mess);


/* mapping from strings to token types
*/
static struct keyword
{
	TokenType type;
	char *lexeme;
	int lexemeLen;
} keys[] = 
{
	{NEW,		"new",		3},
	{DELETE,	"delete",	6},
	{VOID,		"void",		4},
	{OPERATOR,	"operator",	8},	
	{VIRTUAL,	"virtual",	7},
	{CLASS,		"class",	5},
	{STRUCT,	"struct",	6},
	{UNION,		"union",	5},
	{PUBLIC,	"public",	6},
	{PRIVATE,	"private",	7},	
	{PROTECTED,	"protected",	9},
	{CONST,		"const",	5},
	{FRIEND,	"friend",	6},
	{TEMPLATE,	"template",	8},
	{NEW,		NULL,		0}
};


inline static char escape(char c)
{
	return (c - 'a' + '\a');
}


Litint *CXXLexer::getLitint(const Token *base)
{
	Litint *litint = new Litint(*base);
	
	litint->lexeme = la;
	for (in(); isdigit(la); in())
	{
		litint->lexeme += la;
	}
	litint->intval = ::atoi(litint->lexeme);
	return litint;
}


Litstring *CXXLexer::getLitstring(const Token *base)
{
	char closer = la;
	Litstring *litstring = new Litstring(*base);

	litstring->lexeme = la;
	for (in(); la != '\n' && la != 0 && la != closer; in())
	{
		litstring->lexeme += la;
		if (la != '\\') // ordinary character
			litstring->thestring += la;
		else // escape sequence
		{
			in();
//			if (la >= '0' && la <= '7')  // \o[o[o]]
//			{
//			}
//			else if (la == 'x')  // \xh[h[h...]]
//			{
//			}
//			else  // single-character escape sequence
			{
				litstring->lexeme += la;
				litstring->thestring += escape(la);
			}
		}
	}
	if (la != closer)
		syntaxErrorNoFailure("unclosed character or string constant");

	litstring->lexeme += closer;
	if (la != 0) in();
	
	return litstring;
}


/* lent is length of t not counting null
*/
static inline int fastCmp(const String &s, const char *t, int lent)
{
	return	((*(const char*)s != *t) || (s.length() != lent))
		?
		0
		:
		!memcmp((const char*)s, t, lent);
}


void CXXLexer::getKeywordOrId(Token *base)
{
	base->lexeme = la;
	in();
	while (isalnum(la) || la == '_')
	{
		base->lexeme += la;
		in();
	}

	for (keyword *key = keys; key->lexeme != 0; key++)
	{
		if (fastCmp(base->lexeme, key->lexeme, key->lexemeLen))
		{
			base->type = key->type;
			return;
		}
	}

	// if it's not a keyword, it's an id.
	base->type = ID;
}

void CXXLexer::getQualOrColon(Token *base)
{
	in();
	if (la == ':')
	{
		base->lexeme = "::";
		base->type = QUAL;
		in();
	}
	else
	{
		base->lexeme = ':';
		base->type = COLON;
	}
}

void CXXLexer::getAngleOrShift(char c, Token *base)
{
	base->type = (c == '<'? LANGLE : RANGLE); 
	base->lexeme = c;
	in();
	if (la == c)
	{
		base->lexeme += c;
		base->type = OTHERTOK;
		in();
	}
}

/* returns true if it's returning a comment token (in base)
*/
int CXXLexer::gobbleCommentsAndWhitespace(Token *base)
{
start:
	while (isspace(la)) 
	{
		if (la == '\n' && nlIsTok) 
			break;
		base->ws += la;
		in();
	}
	base->lineno = lalineno;
	if (la == '/')  // check for possible comment
	{
		if (peek() == '/')  // C++ style comment
		{
			if (cmtIsTok)
			{
				base->type = CXXCOMMENT;
				base->lexeme = "//";
				for (in(), in(); la != '\n' && la != 0; in())
					base->lexeme += la;
				return 1;
			}
			else
			{
				for (in(), in(); la != '\n' && la != 0; in())
					;
				goto start;
			}
		}
		else if (peek() == '*')  // C style comment
		{
			if (cmtIsTok)
			{
				base->type = CCOMMENT;
				base->lexeme = "/*";
				for (in(), in(); (la != '*' || peek() != '/') && la != 0; in())
					base->lexeme += la;
				base->lexeme += "*/";
			}
			else
			{
				for (in(), in(); (la != '*' || peek() != '/') && la != 0; in())
					;
			}
			if (la == 0)
				syntaxErrorNoFailure("unclosed comment");
			in();
			in();
			if (cmtIsTok)
				return 1;
			goto start;
		}
	}
	return 0;
}


Token *CXXLexer::gettok()
{
	/* Base serves two purposes:
	*	(1) It's a temp var, used to hold stuff until I figure out what to do with it.
	*	(2) It's the next token, unless I decide otherwise (in which case I delete it).
	*/
	Token *base = new Token;

	/* Pointer to the new token, returned to the lexer.  
	*  I'll change what it's pointing to if I decide to 
	*  return something other than base.
	*/
	Token *retval = base;

	if (gobbleCommentsAndWhitespace(base))  // if got a comment token
	{
	}
	else if (la == 0)
	{
		base->type = EOFTOK;		
	}
	else if (la == '\n')
	{
		base->lexeme = la;
		base->type = NL;			
		in();
	}
	else if (la == '#')
	{
		base->lexeme = la;
		base->type = POUND;
		in();
	}	
	else if (la == ',')
	{
		base->lexeme = la;
		base->type = COMMA;
		in();
	}	
	else if (la == '*')
	{
		base->lexeme = la;
		base->type = STAR;
		in();
	}	
	else if (la == ':')
	{
		getQualOrColon(base);
	}	
	else if (la == '<' || la == '>')
	{
		getAngleOrShift(la, base);
	}
	else if (la == ';')
	{
		base->lexeme = la;
		base->type = SEMI;
		in();
	}	
	else if (la == '(')
	{
		base->lexeme = la;
		base->type = LP;
		in();
	}		
	else if (la == ')')
	{
		base->lexeme = la;
		base->type = RP;
		in();
	}
	else if (la == '{')
	{
		base->lexeme = la;
		base->type = LC;
		in();
	}
	else if (la == '}')
	{
		base->lexeme = la;
		base->type = RC;
		in();
	}
	else if (la == '[')
	{
		base->lexeme = la;
		base->type = LS;
		in();
	}
	else if (la == ']')
	{
		base->lexeme = la;
		base->type = RS;
		in();
	}
	else if (la == '"' || la == '\'')
	{
		retval = getLitstring(base);
	}
	else if (isdigit(la))
	{
		retval = getLitint(base);
	}		
	else if (isalpha(la) || la == '_')
	{
		getKeywordOrId(base);
	}
	else
	{
		base->lexeme = la;
		base->type = OTHERTOK;
		in();
	}

	if (retval != base)
		delete base;

	return retval;
}
