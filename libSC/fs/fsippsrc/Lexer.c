/*ident	"@(#)fs:fsippsrc/Lexer.c	3.1" */
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

#include "Lexer.h"

#include <stdlib.h>
#include <osfcn.h>
#include <assert.h>


static void emptywin(List_of_p<Token> &win)
{
	while (win.length() > 0)
		delete win.get();
}

void Lexer::attach(istream &f, const String &s)
{
	attach(f);
	thefilename = s;
}

void Lexer::attach(istream &f)
{
	if (inf)  // if already attached to a file
	{
		lalineno = 1;
		la = 0;
		curTok = 0;
		curToki = -1;
		atBof = 1;
		left_posinfile = 0;
		emptywin(theWindow);
	}
	inf = &f;
	thefilename.make_empty();
	in();
	move();
}

Lexer::Lexer() :
	verboselevel(0),
	thetrailsize(0), 
	contractAction(0), 
	frozen(0), 

	lalineno(1), 
	la(0), 
	curTok(0), 
	curToki(-1), 
	atBof(1), 
	left_posinfile(0), 
	theWindow(),
	inf(0), 
	thefilename()
{
}

Lexer::~Lexer()
{
	if (frozen) 
		DestroyingFrozenLexer.raise("Attempt to destroy a frozen Lexer!"); 
}

char Lexer::peek()
{
	if (inf == 0)
	{
		UnattachedLexer.raise("Attempt to peek() in unattached Lexer!");
		return 0;
	}
	if (inf->eof()) 
		return 0; 
	else 
		return inf->peek(); 
}

int Lexer::resize(int i)
{
	if (i >= 0) 
	{
		extend(i - theWindow.length() + 1);
		return 1;
	}
// This doesn't work, since adding Bofs will throw off any outstanding
// absolute positions, and it won't make i a valid absolute position
// in any case.
//	if (atBof)
//	{
//		addBofs(-i);
//		return 1;
//	}
	DiscardedToken.raise("Attempt to go to/get/insert before a discarded token in Lexer!");
	return 0;
}

// This is the only place in all the code where Tokens 
// are added to the left edge of the window (other than
// the initial put to the empty window upon attachment).
//
void Lexer::addBofs(int i)
{
	while (i-- > 0)
	{
		Token *t = new Token;
		t->type = BOF;
		theWindow.unget(t);
		--left_posinfile;
		++curToki;
	}
}

void Lexer::extend(int i)
{
	while (i-- > 0) 
	{ 
		Token *t = gettok();
		theWindow.put(t); 
		if (verboselevel > 1) cerr << *t << endl;
	}
}

void Lexer::move(int i)
{ 
	if (i != 0 && resize(absI(i))) 
	{
	  	curToki += i; 
	  	curTok = theWindow[(unsigned)curToki]; 
	  	if (thetrailsize != infinity) 
			implicitContract(curToki - thetrailsize);
	  	if (verboselevel == 1) 
			cerr << *curTok << endl;
	}
}

void Lexer::insertAfterAbs(const Token *t, int i)
{ 
	if ((i == -1) || resize(i)) 
	{
		List_of_piter<Token> theWindowp(theWindow);
		theWindowp.reset(i+1);
		theWindowp.insert_next((Token*&)t);
		if (i < curToki) 
			curToki++;
		if (verboselevel > 1) 
			cerr << *t << " (inserted at " << i << ")" << endl;
	}
}

// This is the only place in all the code where Tokens 
// fall off the left edge of the window.
//
void Lexer::dropLeftmostToken(ContractAction ca)
{
	assert(theWindow.length() > 0);
	Token *t = theWindow.get();
	++left_posinfile;
	--curToki;
	if (t->type != BOF)
	{
		if (ca)
			(*ca)(t);
		atBof = 0;
	}
	delete t;
}

int Lexer::docontract(int i, ContractAction ca, int is_explicit)
{
	if (is_explicit && frozen == 2) 
	{
		ContractingFrozenLexer.raise("Attempt to explicitly contract a completely frozen Lexer!");
		return 0;
	}
	int n = 0;
	if (is_explicit || !frozen) 
	{
		while (i-- > 0 && curToki > 0)
		{ 
			dropLeftmostToken(ca);
			n++; 
		}
	}
	return n;
}

Token *Lexer::windowAbs(int i)
{ 
	if (i == curToki) return curTok;
	else if (resize(i)) return theWindow[(unsigned)i]; 
	else return 0;
}

int Lexer::handshake(int type, int i)
{ 
	if (((Token *)(theWindow[(unsigned)absI(i)]))->type != type) 
	{
		BadHandshake.raise("Bad handshake with Lexer!");
		return 0;
	}
	return 1;
}

char Lexer::in()
{ 
	if (inf == 0)
	{
		UnattachedLexer.raise("Attempt to in() in unattached Lexer!");
		return 0;
	}
	if (la == '\n') 
		lalineno++; 
	if (inf->eof() || !inf->get(la))
		la = 0;
	return la;		
}

void Lexer::printWindow(int verbose) //const
{ 
	if (verbose)
		(ostream&)cerr << theWindow << ", current token == " << curToki;
	else {
		List_of_piter<Token> theWindowp(theWindow);
		for (theWindowp.reset(); !theWindowp.at_end(); theWindowp.step_next()) 
		{
			cerr << theWindowp.peek_next()->ws;
			if (theWindowp.position() == curToki) 
			{
				cerr << ">>>>";
				cerr << theWindowp.peek_next()->lexeme;
				cerr << "<<<<";
			} 
			else 
			{
				cerr << theWindowp.peek_next()->lexeme;
			}
		}
	}
}

static int byebye(const char *s)
{
	cout << endl;
	cerr << s << endl;
	abort();
	return 0;
}

Objection Lexer::DestroyingFrozenLexer(byebye);
Objection Lexer::ContractingFrozenLexer(byebye);
Objection Lexer::DiscardedToken(byebye);
Objection Lexer::BadHandshake(byebye);
Objection Lexer::UnattachedLexer(byebye);

