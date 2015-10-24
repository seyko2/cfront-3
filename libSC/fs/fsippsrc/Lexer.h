/*ident	"@(#)fs:fsippsrc/Lexer.h	3.1" */
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

#ifndef FSLEXER_H
#define FSLEXER_H

#include "Token.h"

#include <String.h>
#include <Objection.h>
#include <List.h>
#include <iostream.h>

typedef void (*ContractAction)(Token *);

class Lexer 
{
protected:
	// Returns the absolute index of relative window position i.
	int absI(int i) const		{ return i+curToki; }

	// Returns the relative index of absolute window position i.
	int relI(int i)	const		{ return i-curToki; }

	// Make i a valid absolute position, or raise exception.
	int resize(int i);
public:
	static Objection DestroyingFrozenLexer, ContractingFrozenLexer;
	static Objection UnattachedLexer, DiscardedToken, BadHandshake;

	enum { infinity = -1 };

	Lexer();
	~Lexer();

	// Attach lexer to input character stream
	void attach(istream &f);
	void attach(istream &f, const String &fname);

	// Returns true if there are no tokens in the current window.
	// (True before the lexer is attached to a file.)
	int emptyWindow() const		{ return curToki == -1; }

	// Access the name of the input file.
	void setFilename(const String &s)	{ thefilename = s; }
	String filename() const		{ return thefilename; }

	// Access the trail size.
	void setTrailsize(int l)	{ thetrailsize = l; addBofs(l); }
	int trailsize()	const		{ return thetrailsize; }

	// Move the current token pointer i tokens to the right (left if i is 
	// negative), extending and contracting the window as necessary.
	void move(int i=1);

	// Same as move, but i is relative to beginning of window.
	void moveAbs(int i)	 	{ move(relI(i)); }

	// Ditto, but i is relative to beginning of file.
	void moveInFile(int i)		{ moveAbs(i-left_posinfile); }

	// Add i tokens to the right of the window (but don't move the current token).
	void extend(int i=1);

	// Insert a token into the window after the token at absolute location i.
	// If i==-1, insert before the first token.
	void insertAfterAbs(const Token *t, int i);
	void insertBeforeAbs(const Token *t, int i)
					{ insertAfterAbs(t, i-1); }

	// Remove i tokens from the left of the window, or as many as possible up to the current token.
	// Return the number actually removed.
	int contract(int i=1)		{ return docontract(i, contractAction, 1); }

	// Remove enough tokens from the left of the window so that there are no more than i to the left of the current token.
	int contractTo(int i)		{ return docontract(curToki - i, contractAction, 1); }

	// Same as above two, but don't do contractAction.
	int discard(int i=1)		{ return docontract(i, 0, 1); }
	int discardTo(int i)		{ return docontract(curToki - i, 0, 1); }

	// Disable automatic contracting.
	void freeze()			{ frozen = 1; }

	// Disable all contracting, both explicit and automatic.
	void freezeTotally()		{ frozen = 2; }

	// Enable all contracting.
	void melt()			{ frozen = 0; }

	// 0 is quiet, 1 is "show each token as it becomes current," 
	// >1 is "show each token as it enters window."
	void verbose(int i)		{ verboselevel = i; }

	// Set the contract action.  Return the old value.
	ContractAction setContractAction(ContractAction ca)
					{ ContractAction cur = contractAction; 
					  contractAction = ca;
					  return cur; 
					}

	// Return the current size of the window.
 	int windowSize() const		{ return theWindow.length(); }

	// Return the offset of the current token from the beginning 
	// of the window/begining of the file.
	int curpos() const		{ return curToki; }
	int posinfile() const		{ return (left_posinfile + curToki); }

	// Functions for retreiving information from the window.  
	//
 	// The first two functions return a pointer to the *base part* of the i'th token.
	// These functions are provided as a courtesy to the client, so she can avoid
	// handshaking wherever possible.
	//
	// i is offset relative to the beginning of the window/current token.
	Token *windowAbs(int i);
	Token *window(int i)		{ return windowAbs(absI(i)); }

	// There are also a bunch of functions to get tokens of particular 
	// types from the window.  
	// Since the token types are specific to the particular language 
	// being lexed, these functions must be defined in the derived class.
	// They shouldn't move the current token.

	// Line number access.
	void setLineno(int l)		{ lalineno = l; }
	int lineno() const		{ return lalineno; }

	void printWindow(int verbose=0) /* const */;

protected /* functions */:
	// Add i BOF tokens to left of the current window.
	void addBofs(int i);

	// Returns the number of tokens actually removed.
	int docontract(int i, ContractAction ca, int is_explicit);
	int implicitContract(int i)	{ return docontract(i, contractAction, 0); }

	// The function which actually does a contraction.
	void dropLeftmostToken(ContractAction ca);

	// The user-supplied gettok.  It should return a pointer to the next 
	// Token in the input stream.
	virtual Token *gettok() = 0;

	// Make sure the i'th token has type type.
	int handshake(int type, int i);

	// Get the next character from the input stream, put it in la, and 
	// also return it.  
	char in();

	// Return what the next call to in() will return.
	char peek();

protected /* data */:
	// offset of the leftmost token in window wrt entire file.
	int left_posinfile;

	// true if the leftmost token in current window is first token in file,
	// or if no tokens yet in window.
	int atBof;

	// See above.
	int verboselevel;

	// 1 if automatic contracting is disabled, 2 if all contracting is disabled, 0 otherwise.
	int frozen;

	// User-specified action to perform on token when contracting it out 
	// of window due to exceeding trail size, or explicit request from 
	// client.  
	ContractAction contractAction;

	// Maximum allowed number of tokens in window to the left of the current token.
	int thetrailsize;

	// Input character *following* the current token.
	char la;

	// The input file.
	istream *inf;

	// Name of input stream, and line number of la.
	String thefilename;
	int lalineno;

	// The token window.  curTok points to the current token, and curToki 
	// is its index in the window.

	// For this data structure, we want "a Block that you can shrink or 
	// expand on either side."  
	// For now, let's give up the O(1) time for operator[], and just 
	// implement it with lists.  If we decide to stick with lists, we can 
	// change the use of operator[] in the move function to instead use 
	// the list's "current position".
	//
	List_of_p<Token> theWindow;

	// Pointer to current token.
	Token *curTok;

	// Offset of current token wrt leftmost token in window 
	// (-1 if window is empty).
	int curToki;
};

#define TOK		lexer->window(0)->type
#define NEXTTOK		lexer->window(1)->type
#define NEXTNEXTTOK	lexer->window(2)->type
#define PREVTOK		lexer->window(-1)->type
#define PREVPREVTOK	lexer->window(-2)->type
#define LEXEME	lexer->window(0)->lexeme
#define LINENO	lexer->window(0)->lineno
#define LEXWS	lexer->window(0)->ws
#define ADV	lexer->move(1)
#define BACKUP	lexer->move(-1)

/* Macro used by the derived classes to easily define a get function.
*/
#define DEFINE_GET(a_typename, typecode)				\
	void get(a_typename &x, int i=0)				\
		{ if (resize(absI(i)) && handshake(typecode, i))	\
			x = *(a_typename*)window(i);			\
		}

#endif
