/*ident	"@(#)fs:fsippsrc/parse.c	3.1" */
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

#include "fsipp.h"
#include "setjmp.h"

#define SHOWSTREAM 0

static void foundND();
static void protoOrDef();
static void proto(bool_type);
static void def(bool_type, String, bool_type);
static void deleteUse();
static void newUse();
static void cppline();
static void lookForClassHead();

static bool_type doingNew;
static int nErrors;
static jmp_buf beginning;

static void advAfterError()
{
	ADV;  
	ADV;
	ADV;
	while (TOK != LC && TOK != RC && TOK != SEMI && TOK != EOFTOK)
		ADV;
}

static void fail()
{
	lexer->melt();
	advAfterError();
	longjmp(beginning, 1);
}

static void message(char *prefix, char *mess)
{
	nErrors++;
	cerr << "CC -fs: " << prefix << ": line " << (lexer->emptyWindow()? 1 : LINENO) << " " << lexer->filename();
	if (mess)
		cerr << ": " << mess;
	cerr << endl;
	fail();
}

static void sorry(char *mess)
{
	message("Sorry, not implemented", mess);
}

void syntaxError(char *mess)
{
	message("Syntax error", mess);
}

static bool_type builtin(const String & type)
{
	return (type == "int" || type == "char" || type == "short" || type == "long" || type == "float" || type == "double");
}

static const char *size_t_onThisMachine()
{
	int short_size = sizeof(short);
	int int_size = sizeof(int);
	int long_size = sizeof(long);
	int sizeof_size = sizeof(sizeof(char));  /* the type there doesn't matter */
	if (sizeof_size == int_size)
		return "unsigned int";
	else if (sizeof_size == short_size)
		return "unsigned short";
	else if (sizeof_size == long_size)
		return "unsigned long";
	else
	{
		cerr << "Can't determine underlying type of size_t on this machine." << endl;
		exit(1);
		return 0;
	}
}

static void match(TokenType t)
{
	if (TOK == t)
		ADV;
	else
		syntaxError("Unexpected token");
}

/*
static void matchOptional(TokenType t)
{
	if (TOK == t)
		ADV;
}
*/

static void emitLineDirective(int line)
{
	cout << "\n#";
	if (alternateLineDirectiveFormat)
		cout << "line";
	cout << " " << line << " \"" << lexer->filename() << "\"\n";
}

static TokenType rightGroupTok(TokenType left)
{
        if (left == LP)
		return RP;
        else if (left == LC)
                return RC;
        else if (left == LS)
                return RS;
	else if (left == LANGLE)
		return RANGLE;
	assert(0);
	/* NOTREACHED */
	return ID;
}

// advance to the right grouping token matching the current token.
// while doing so, append lexemes and whitespace to the string s, if s!=0.
static void advToMatchingAppendingTo(String *s)
{
        TokenType left = TOK;
        TokenType right = rightGroupTok(left);

        int depth = 1;
        while (depth > 0 && TOK != EOFTOK)
        {
		if (s != 0) 
		{
			*s += LEXWS;
			*s += LEXEME;
		}
                ADV;
                if (TOK == left) depth++;
                else if (TOK == right) depth--;
		else if (TOK == LC)
			syntaxError("Missing right delimiter");
        }
        if (TOK == EOFTOK)
                syntaxError("Unexpected end of file: missing right delimiter");
	if (s != 0)
	{
		*s += LEXWS;
		*s += LEXEME;
	}
}

static void advToMatching()
{
	advToMatchingAppendingTo(0);
}

/* Here's what we do to tokens that get contracted off the lexer window.
*/
void echo(Token *t)
{
	cout << t->ws << t->lexeme;
}

/* Map the function f to each of the tokens i..j in the current window.
*  i and j are absolute positions.
*
* THIS IS INEFFICIENT - it calls windowAbs for every token between i and j.
*
*/
void maptok(void (*f)(Token*), int i, int j)
{
	for (; i<=j; i++)
	{
		(*f)(lexer->windowAbs(i));
	}
}

static bool_type interesting(TokenType t)
{
	return (t == NEW || t == DELETE || t == POUND || t == EOFTOK || t == AGGR || t == LC || t == RC);
}

static void emitMyProtos()
{
	cout << "extern void *_fs_register_ATTLC(int atuse, const char *filename, int lineno, const char *type,\n\t";
	cout << size_t_onThisMachine() << " size, long number, const void *address);\n";
	cout << "extern void _fs_unregister_ATTLC(int atuse, int hasbrackets, const void *address);\n";
}

static void foundSomethingInteresting()
{
	if (TOK == NEW || TOK == DELETE)
		foundND();
	else if (TOK == AGGR)
		lookForClassHead();
	else if (TOK == POUND)
		cppline();
	else if (TOK == LC)
	{
		ADV;
		pushScope();
	}
	else if (TOK == RC)
	{
		ADV;
		popScope();
	}
}

int parse()
{
	lexer->setTrailsize(10);  // magic number.  10 is more than enough for the lookbehinds we need.
	lexer->setContractAction(echo);	// Tokens that fall out of the window just get echoed.
	nErrors = 0;
	emitMyProtos();
	while (TOK != EOFTOK && nErrors <= MAXNERRORS)
	{
#if SHOWSTREAM
		cerr << *lexer->window(0) << endl;
		ADV;
#else
		if (setjmp(beginning)) 
		{
			/* parsing returns here if we encounter a syntax error */
		}
		else
		{
			while (!interesting(TOK)) ADV;
			foundSomethingInteresting();
		}
#endif
	}
	lexer->contractTo(0);
#if SHOWSTREAM
	cerr << *lexer->window(0) << endl;
#else
	cout << LEXWS;
#endif
	if (nErrors > MAXNERRORS)
		cerr << "Sorry, too many errors." << endl;
	return nErrors;
}

/* found a new or delete
*/
static void foundND()
{
	assert (TOK == NEW || TOK == DELETE);

	doingNew = (TOK == NEW);
	if (PREVTOK == OPERATOR)
		protoOrDef();
	else 
	{
		if (TOK == NEW)
			newUse();
		else
			deleteUse();
	}
}

static void findQualAndInline(bool_type &hasQual, String &qual, bool_type &isInline)
{
	// Here's where we do a backwards recursive descent!
	
	// [...] operator n/d
	//                 ^
	int rempos = lexer->posinfile();
	BACKUP;
	BACKUP;
	qual.make_empty();
	if (hasQual = (TOK == QUAL))
	{
		BACKUP;
		// [...] :: operator n/d
		//    ^
		if (TOK == ID)
		{
			// [...] T :: operator n/d
			//       ^
			if (PREVTOK == QUAL)
			{
				sorry("Cannot handle out-of-line definitions of nested operator new/delete");
			}
			qual = LEXEME;
			BACKUP;
		}
		else if (TOK == RANGLE)
		{
			sorry("Cannot handle out-of-line definitions of template operator new/delete");
		}
	}
	if (doingNew && TOK == STAR)
		BACKUP;
	if (TOK != VOID && TOK != ID)  // might be typedef name
		syntaxError(doingNew? "return type of operator new must be void*" : "return type of operator delete must be void");
	BACKUP;
	isInline = (TOK == INLINE);
	lexer->moveInFile(rempos);	
}

/* Found a prototype or definition of new or delete.
*/
static void protoOrDef()
{
	// [...] operator n/d
	//                 ^

	String qual;
	bool_type hasQual, isInline;
	findQualAndInline(hasQual, qual, isInline);

	ADV;
	if (TOK != LP)
		syntaxError(doingNew? "operator new: missing argument list" : "operator delete: missing argument list");
	// [...] operator n/d (
	//                    ^

	lexer->contractTo(0);
	lexer->freeze();
	advToMatching();
	ADV;
	// ( arglist ) TOK
	//              ^

	if (TOK == LC)
		def(hasQual, qual, isInline);
	else if (TOK == SEMI)
		proto(isInline);
	else if (TOK == COMMA)
		sorry("prototypes of operators new and delete cannot be in prototype list");
	else
		syntaxError("I don't understand this.");
	lexer->melt();
}

static void determineClasAndGlobality(bool_type hasQual, String qual, String &clas, bool_type &isglobal)
{
	if (hasQual)
	{
		isglobal = no;
		clas = qual;
	}
	else
	{
		isglobal = !containingClass(clas);
		if (isglobal)
			clas = "?";
	}	
}

static Token *generateId(int i, int line)
{
	Token *id = new Token;
	id->ws = ' ';
	id->lineno = line;
	ostrstream os;
	os << "fsATTLC" << i << ends;
	id->lexeme = os.str();
	delete os.str();
	id->type = ID;
	return id;
}

/* Returns a pointer to the formal argument name appearing to the left of absolute 
* position i in the window.
*
* Inserts a unique argument name into the arglist if an argument name was omitted.
* (This will unfortunately cause a cc "argument not used" warning.  I should
* really delete any inserted tokens immediately after laying down the 
*	static void *_new(
* or 
*	static void _delete(
* )
*/
static Token *argNameToLeft(int i, int &inserted)
{
	Token *prev = lexer->windowAbs(i-1);
	Token *prevprev = lexer->windowAbs(i-2);
	inserted = 0;
	if (prev->type != ID || prevprev->type == COMMA || prevprev->type == LP || builtin(prev->lexeme))
	{
		// arg name was omitted, insert one
		Token *t = generateId(i, prev->lineno);
		lexer->insertBeforeAbs(t, i);
		inserted = 1;
		return t;
	}
	else if (prev->type == ID)	
	{
		// it's probably the arg name
		return prev;
	}
	else	
	{
		// it's probably a syntax error
		syntaxError("I don't understand this parameter list");
		return 0;
	}
}

typedef List_of_p<Token> ArgList;

/* Return a list of pointers to the tokens in the window comprising
* the names of the arguments in the argument list beginning at absolute 
* window position beg and ending at end.
*
* If any of the arguments are missing their names, I'll generate unique ones,
* and insert them into the window.
*/
static ArgList *extractArgs(int beg, int end)
{
	assert(lexer->windowAbs(beg)->type == LP);
	assert(lexer->windowAbs(end)->type == RP);
	
//cerr << "Before extracting args: "; lexer->printWindow();
//cerr << endl;

	ArgList *args = new ArgList;

	int i = beg+1;
	Token *t = lexer->windowAbs(i);
	for (; i <= end; i++, t = lexer->windowAbs(i))
	{
		if (t->type == LP || (t->type == RP && i != end))
			sorry("parameter list is too complicated, try simplifying");

		if (t->type == COMMA || i == end)
		{
			int inserted;
			args->put(argNameToLeft(i, inserted));
			if (inserted)
			{
				i++;
				end++;
			}
		}
	}
//cerr << "After extracting args: "; lexer->printWindow();
//cerr << "\nArgs are: " << *args << endl;
	return args;
}

static void emitInstrumentedDef(String clas, bool_type isglobal, ArgList *args)
{
	// The first argument is the size for new, and the pointer to the object for delete.
	if (args->length() == 0)
		syntaxError("new and delete must each have at least one parameter");
	Token *firstArg;
	// args->peek_next(firstArg);
	List_of_piter<Token> argsi(*args);
	argsi.peek_next(firstArg);

	cout << "{ ";
	if (isglobal) // generate prototype
	{
		cout << "extern void " << (doingNew? "*_new":"_delete");
		maptok(echo, 0, lexer->curpos()-1);	// duplicate the formal argument list
		cout << "; ";
	}
	if (doingNew)
	{	
		cout << "return (_fs_register_ATTLC(0, \"?\", -1, \"" << clas << "\", ";
		cout << firstArg->lexeme << ", " << (isglobal? -2:1) << ", _new(";
	}
	else
	{
		cout << "((_fs_unregister_ATTLC(0, 0, " << firstArg->lexeme << "), _delete(";
	}
	Token *arg;
	for (argsi.reset(); argsi.next(arg); )  // pass all of the parameters through to _new/_delete
	{
		cout << arg->lexeme;
		if (!argsi.at_end())
			cout << ", ";
	}
	cout << "))); }";
}

static void def(bool_type hasQual, String qual, bool_type isInline)
{
//cerr << "found definition on line " << LINENO << endl;
	// ( arglist ) {
	//             ^

	int openingCurlyLine = LINENO;
	String clas;
	bool_type isglobal;
	determineClasAndGlobality(hasQual, qual, clas, isglobal);

	ArgList *args = extractArgs(0, lexer->curpos()-1);
	maptok(echo, 0, lexer->curpos()-1);	// duplicate the arglist
	emitLineDirective(openingCurlyLine);
	emitInstrumentedDef(clas, isglobal, args);
	delete args;
	emitLineDirective(openingCurlyLine);

	if (!isglobal && !hasQual)
		cout << "static ";
	if (isInline)
		cout << "inline ";
	cout << "void ";
	if (doingNew)
		cout << "* ";
	if (hasQual)
		cout << qual << "::";
	cout << (doingNew? "_new" : "_delete");

	lexer->contractTo(0);
	emitLineDirective(openingCurlyLine);
	LEXWS = "";
}

static void proto(bool_type isInline)
{
//cerr << "found prototype on line " << LINENO << endl;

	// ( arglist ) ;
	//             ^

	maptok(echo, 0, lexer->curpos()); // duplicate "( arglist ) ;"
	if (containingClass())
		cout << " static";
	if (isInline)
		cout << " inline";
	cout << " void " << (doingNew? "*_new" : "_delete");
}

/* Initial window:
*  	...tokens [[X]::]n/d tokens...
*	                  ^
*  Final window:
*                 [[X]::]n/d tokens...
*	                  ^
* and (1) the whitespace before the first token in the final window has 
* been printed and reset to empty, and (2) the lexer is frozen.
*/
static void prepareUse()
{
	assert(TOK == NEW || TOK == DELETE);
	int i = 0;
	if (PREVTOK == QUAL)
	{
		BACKUP;
		i++;
		// tokens [X] :: n/d tokens...
		//            ^
		if (PREVTOK == ID)
		{
			i++;
			if (PREVPREVTOK == QUAL)
				sorry("Cannot handle qualified calls to nested operator new/delete");
		}
		else if (PREVTOK == RANGLE)
		{
			sorry("Cannot handle qualified calls to template new/delete");
		}
		ADV;
	}
	lexer->contractTo(i);
	cout << lexer->window(-i)->ws;
	lexer->window(-i)->ws = "";
	lexer->freezeTotally();
}

static bool_type canFollowNewedtype(TokenType t)
{
	return (t == COLON || t == SEMI || t == COMMA || t == RP || t == LS || t == LP);
}

static String collectTypename()
{
	String s = LEXEME;
	match(ID);
	if (TOK == LANGLE)
	{
		advToMatchingAppendingTo(&s);
		ADV;
	}
	while (TOK == QUAL)
	{
		s += "::";
		ADV;
		if (TOK != ID)
			syntaxError("Something's wrong in this qualified type name");
		s += LEXEME;
		ADV;
	}
	return s;
}		

/* Found a use of new.
*/
static void newUse()
{
	assert (TOK == NEW);
	prepareUse();
	// [[X]::] new TOK
	//          ^

	int newlineno = LINENO;
	ADV;  
	if (TOK == LP)
	{
		advToMatching();
		ADV;
	}
	// [[X]::] new [ (expr) ] TOK
	//                         ^

	String aggr;
	if (TOK == AGGR)
	{
		aggr = LEXEME;
		ADV;
	}
	// [[X]::] new [(expr)] [AGGR] type
	//                              ^

	if (TOK != ID)
		sorry("In \"new T\", T cannot be a complex type expression.");

	String type = collectTypename();

//	emitLineDirective(newlineno);

	cout << "(" << aggr << " " << type << "*)_fs_register_ATTLC(1, \"" << lexer->filename() << "\", ";
	cout << newlineno << ", \"" << type << "\", ";
	cout << "sizeof(" << aggr << " " << type << "), ";

	// [[X]::] new [AGGR] type TOK
	//                          ^

	if (!canFollowNewedtype(TOK))
		sorry("In \"new T\", T cannot be a complex type expression.");

	if (TOK == LS)
	{
		ADV;
		// [[X]::] new [AGGR] type '[' expr ']'
		//                             ^

		if ((TOK == ID || TOK == LITINT) && lexer->window(1)->type == RS)
			cout << LEXEME;
		else
			cout << -1;
		cout << ", ";

		BACKUP;
		advToMatching();
		ADV;
		// [[X]::] new [AGGR] type '[' expr ']' TOK   
		//		    	                 ^
	}
	else if (TOK == LP)
	{
		// [[X]::] new [AGGR] type ( constructorArgs )
		//                         ^

		cout << "1, ";

		advToMatching();
		ADV;
		// [[X]::] new [AGGR] type ( constructorArgs ) TOK   
		//          		                        ^
	}
	else
	{
		// [[X]::] new [AGGR] type TOK
		//	                    ^

		cout << "1, ";
	}
	lexer->melt();
	lexer->contractTo(0);
	cout << ")";		

//	emitLineDirective(LINENO);
	LEXWS = "";
}

/* Found a use of delete.
*/
static void deleteUse()
{
	assert (TOK == DELETE);
	prepareUse();
	ADV;  
	// [[X]::] delete TOK
	//                 ^

	int deletePos = lexer->curpos() - 1;
	bool_type hasBrackets = (TOK == LS);

	cout << "(_fs_unregister_ATTLC(1, " << hasBrackets << ", ";

	if (hasBrackets)
	{
		// [[X]::] delete '[' [ignoreable expr] ']' TOK
		//                 ^
		advToMatching();
		ADV;
		// [[X]::] delete '[' [ignoreable expr] ']' TOK
		//                                           ^
	}

	// Now the object being deleted.
	//
	if (TOK != ID || (lexer->window(1)->type != SEMI && lexer->window(1)->type != COMMA))
		sorry("argument of delete too complicated (must be simple identifier)");

	// [[X]::] delete [ '[' [ignoreable expr] ']' ] id ;
	//                                              ^

	cout << LEXEME << "), ";
	maptok(echo, 0, deletePos - 1);  // print the optional qualifier
	cout << "delete ";
	if (hasBrackets)
		cout << "[] ";
	cout << LEXEME << ")";

	ADV;
	// [[X]::] delete [ ... ] id ;
	//                           ^

	lexer->melt();
	lexer->discardTo(0);

	emitLineDirective(LINENO);
	LEXWS = "";
}

/* Found a #.  It's probably a "#[line] lineNumber "fileName"" line.
*  If it's some other kind of preprocessor line, just echo it. 
*/
static void cppline()
{
	lexer->contractTo(0);
	lexer->freeze();

	extern bool_type newlineIsToken;
	newlineIsToken = yes;  // for gettok()
	match(POUND);

	bool_type itsALineDirective = 0;
	if (LEXEME == "line")
		ADV;
	if (TOK == LITINT)
	{
		itsALineDirective = 1;
		Litint litint;
		lexer->get(litint);
			// The -1 counteracts the newline coming up 
			// at the end of this preprocessor line.
		lexer->setLineno(litint.intval - 1);  
		ADV;
		if (TOK == LITSTRING)
		{
			Litstring litstring;
			lexer->get(litstring);
			lexer->setFilename(litstring.thestring);
			ADV;
		}			
		emitLineDirective(litint.intval);
	}

	// get to the end of the preprocessor line.
	while (TOK != NL && TOK != EOFTOK) 
		ADV;
	if (itsALineDirective)  // then discard the whole line, we've already emitted the directive
	{
		lexer->discardTo(0);
		LEXWS = "";
		LEXEME = "";
	}
	lexer->melt();
	newlineIsToken = no;
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

static void lookForClassHead()
{
	String tag;
	match(AGGR);
	if (TOK == ID)
	{
		tag = LEXEME;
		ADV;
	}
	if (TOK == COLON)
		gobbleBaseList();
	if (TOK == LC)
	{
		// found a class header.
		pushClassScope(tag);
		ADV;
	}
}


