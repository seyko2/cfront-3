/*ident	"@(#)publik:parse.c	3.1" */
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

#include "publik.h"

static void cppline();
static void classHead();
static void templateHead();
static int nErrors;
static jmp_buf beginning;


static inline void fail()
{
	lexer->melt();
	longjmp(beginning, 1);
}
static inline void message(char *prefix, char *mess)
{
	cerr << prefix << ": line " << (lexer->emptyWindow()? 1 : LINENO) << " " << lexer->filename();
	if (mess)
		cerr << ": " << mess;
	cerr << endl;
}
/*
static inline void sorry(char *mess)
{
	nErrors++;
	message("Sorry, not implemented", mess);
	fail();
}
*/
void syntaxError(char *mess)
{
	nErrors++;
	message("Syntax error", mess);
	fail();
}
void syntaxErrorNoFailure(char *mess)
{
	nErrors++;
	message("Syntax error", mess);
}
/*
static inline void warning(char *mess)
{
	message("Warning", mess);
}
*/
static inline void match(TokenType t)
{
	if (TOK == t)
		ADV;
	else
		syntaxError(NULL);
}
/*
static inline void matchOptional(TokenType t)
{
	if (TOK == t)
		ADV;
}
*/


/* Advance the current token to the right paren/brace/bracket which 
*  matches the left one we're currently at.
*/
static void advToMatching()
{
	TokenType left = TOK;
	TokenType right;
	if (left == LP)
		right = RP;
	else if (left == LC)
		right = RC;
	else if (left == LS)
		right = RS;
	else
		assert(0);

	int depth = 1;
	while (depth > 0 && TOK != EOFTOK)
	{
		ADV;
		if (TOK == left) depth++;
		else if (TOK == right) depth--;
	}
	if (TOK == EOFTOK)
		syntaxError("unexpected end of file: missing right delimeter");
}


void echo(Token *t)
{
	cout << t->ws << t->lexeme;
}
static bool_type amPrinting()
{
	return (lexer->getContractAction() == echo);
}
static void printingOn()
{
	lexer->setContractAction(echo);
}
static void printingOff()
{
	lexer->setContractAction(0);
}
static void seeIfPrinting()
{
	if (inUpwardlyPublicPartOfClass())
		printingOn();
	else
		printingOff();
}


static void prModifier(TokenType mod)
{
	if (!inClass()) // handles program errors, and also the case when a *struct*
			// privately or publicly inherits from someone
	{
		ADV;
	}
	else
	{
		changePr(mod);
		if (mod == PUBLIC && showModifiers)
		{
			seeIfPrinting();
			ADV;
			match(COLON);
		}
		else
		{
			printingOff();
			ADV;
			match(COLON);
			seeIfPrinting();
		}
	}
}


/* advance to t or eof.  returns true if the former.
*/
static bool_type advTo(TokenType t)
{
	TokenType tok;
	while (((tok=TOK) != t) && (tok != EOFTOK))
		ADV;
	return (tok == t);
}


static void frend()
{
	if (!inClass())
	{
		ADV;
	}
	else
	{
		printingOn();
		while ((TOK != LC) && (TOK != SEMI) && (TOK != EOFTOK))
			ADV;
		if (TOK == LC)
		{
			cout << ";";
			printingOff();
			advToMatching();
		}
		ADV;
		seeIfPrinting();
	}
}


static void rightCurly()
{
	bool_type wasInClass = inClass();
	popScope();
	if (wasInClass && !inClass())
		cout << "\n};" << endl;
	seeIfPrinting();
	ADV;
}

static void gobbleMemberInitializerList()
{
	while (TOK != LC && TOK != SEMI && TOK != EOFTOK)
		ADV;
	if (TOK != LC)
	{
		seeIfPrinting();
		syntaxError("missing body of member function");
	}
}

static void rightParen()
{
	ADV;
	if (TOK == CONST) 
		ADV;
	if (inClass() && (TOK == COLON || TOK == LC))
	{
		if (!showMemberDefs && amPrinting())
		{
			cout << ";";
			printingOff();
		}
		if (TOK == COLON)
			gobbleMemberInitializerList();
		if (TOK == LC)
			advToMatching();
		ADV;
		seeIfPrinting();
	}
}


static inline bool_type interesting(TokenType t)
{
	return (t == FRIEND || 
		t == CLASS ||
		t == TEMPLATE ||
		t == POUND ||
		t == PUBLIC ||
		t == PRIVATE ||
		t == PROTECTED ||
		t == RP ||
		t == LC ||
		t == RC ||
		t == EOFTOK);
}

static void parseAFragment()
{
	switch (TOK) {
	case FRIEND:
		frend();
	esac
	case CLASS:
		classHead();
	esac
	case TEMPLATE:
		templateHead();
	esac
	case POUND:
		cppline();
	esac
	case PUBLIC: 
	case PRIVATE: 
	case PROTECTED:
		prModifier(TOK);
	esac
	case RP:
		rightParen();
	esac
	case LC:
		pushOtherScope();
		ADV;
	esac
	case RC:  // all scopes, including class, popped here
		rightCurly();
	esac
	default:
		cerr << "Token type is " << TOK << endl;
		assert(0);
	esac
	}
}

int parse()
{
	printingOff();
	nErrors = 0;
	while (TOK != EOFTOK && nErrors <= MAXNERRORS)
	{
		if (setjmp(beginning)) 
		{
			/* parsing returns here if we encounter a syntax error */
		}
		else
		{
			while (!interesting(TOK))
				ADV;
			if (TOK != EOFTOK)
				parseAFragment();
		}
	}
	lexer->contractTo(0);
	if (nErrors > MAXNERRORS)
		cerr << "Sorry, too many errors." << endl;
	return nErrors;
}


static void cppline()
{
	lexer->newlineIsToken();
	ADV;
	if (TOK == LITINT)
	{
		Litint litint;
		lexer->get(litint);
				// The -1 counteracts the newline coming up at the end of this preprocessor line.
		lexer->setLineno(litint.intval - 1);  
		ADV;
		if (TOK == LITSTRING)
		{
			Litstring litstring;
			lexer->get(litstring);
			lexer->setFilename(litstring.thestring);
			ADV;
		}			
	}
	advTo(NL);
	lexer->newlineIsntToken();
	ADV;
}

static void gobbleBaseList()		
{
	match(COLON);
	while (TOK != LC && TOK != SEMI && TOK != EOFTOK)
	{
		ADV;
	}
	if (TOK != LC)
		syntaxError("Something's wrong in this base class list");
}

static inline void emitLineDirective(int line)
{
	cout << "# " << line << " \"" << lexer->filename() << "\"\n";
}

static void classHead()
{
	lexer->freeze();
	Token *classKeyword = lexer->window();
	ADV;
	String tag;
	if (TOK == ID)
	{
		tag = LEXEME;
		ADV;
	}
	if (TOK == COLON)
		gobbleBaseList();
	if (TOK == LC)
	{
		// entering a class definition

		ADV;
		if (!inClass())
		{
			static bool_type firstOne = 1;
			if (!firstOne)
				cout << "\n";
			firstOne = 0;
			if (showWhere)
				emitLineDirective(classKeyword->lineno);
			classKeyword->ws = "";
			printingOn();
		}
		lexer->melt();
		lexer->contractTo(0);
		printingOff(); // class members are by default private
		pushClassScope(tag);
	}
	lexer->melt();
}


static void templateHead()
{
	lexer->freeze();
	Token *classKeyword = lexer->window();
	match( TEMPLATE );
	match( LANGLE );
	match( CLASS );
	advTo( RANGLE );
	ADV;

	if (TOK != CLASS )// template function
	{
		lexer->melt();
		return;
	}

	ADV;

	String tag;
	if (TOK == ID)
	{
		tag = LEXEME;
		ADV;
	}
	if (TOK == COLON)
		gobbleBaseList();
	if (TOK == LC)
	{
		// entering a class template definition

		ADV;
		if (!inClass())
		{
			static bool_type firstOne = 1;
			if (!firstOne)
				cout << "\n";
			firstOne = 0;
			if (showWhere)
				emitLineDirective(classKeyword->lineno);
			classKeyword->ws = "";
			printingOn();
		}
		lexer->melt();
		lexer->contractTo(0);
		printingOff(); // class members are by default private
		pushClassScope(tag);
	}
	lexer->melt();
}


