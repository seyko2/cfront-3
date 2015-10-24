/*ident	"@(#)hier:parse.c	3.1" */
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

#ifdef __GNUG__
#pragma implementation "Map.h"
#endif
#include "hier.h"
#include <Map.h>
#include <setjmp.h>

//Mapdeclare(String,TokenType)
//Mapimplement(String,TokenType)
#ifdef __GNUG__
template class Map<String,TokenType>;
#endif

static Map<String,TokenType> aggtyp;
static jmp_buf beginning;
static int nErrors = 0;
static int inRootFile;  // are we in the root file?

// see my comment in ./hier
static String rootFileName;
static int getRootFileName;

static void classHead();
static void templateHead();
static void baseList();
static void baseUnit();
static void parseAFragment();
static int interesting(TokenType);
static void advToMatchingAppendingTo(String &);
static void advToMatchingCollectOnlyClass(String &);

ostream& operator<<(ostream& os, Protection p) 
{
	switch (p)
	{
		case Private: os << "private"; break;
		case Protected: os << "protected"; break;
		case Public: os << "public"; break;
	}
	return os;
}

int parse()
{
	inRootFile = 1;
	rootFileName.make_empty();
	getRootFileName = 1;
	while (TOK != EOFTOK)
	{
		if (setjmp(beginning)) 
		{
			/* parsing returns here when the string is not in our subgrammar */
		}
		else
		{
			while (!interesting(TOK))
				ADV;
			if (TOK != EOFTOK)
				parseAFragment();
		}
	}
	return nErrors;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// note that template is always interesting
// since keyword "class" inside  <> must be consumed. 
static int interesting(TokenType t)
{
	return (t == POUND || 
		t == EOFTOK || 
		(((t == CLASS && doClasses) || 
		  (t == STRUCT && doStructs) || 
		  (t == TEMPLATE && doClasses) ||
		  (t == UNION && doUnions)) 
			&& (inRootFile || !rootFileOnly)));
}

/* advance to t or eof.  returns true if the former.
*/
static int advTo(TokenType t)
{
	TokenType tok;
	while (((tok=TOK) != t) && (tok != EOFTOK))
		ADV;
	return (tok == t);
}

static void cppline()
{
	assert (TOK == POUND);
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
//cerr << "rootFileName " << rootFileName << ", new name " << litstring.thestring << endl;
			if (getRootFileName)
			{
				getRootFileName = 0;
				rootFileName = litstring.thestring;
			}
			else 
			{
				inRootFile = (litstring.thestring == rootFileName);
			}
			ADV;
		}			
	}
	advTo(NL);
	lexer->newlineIsntToken();
	ADV;
}

static void parseAFragment()
{
	switch (TOK) {
	case CLASS: case STRUCT: case UNION:
		classHead();
	break;
	case POUND:
		cppline();
	break;
	case TEMPLATE:
		templateHead();
	break;
	default:
	//cast TOK, some compiler complains ambiguity with <<(o, Protection )
		cerr << "Token type is " << (int)(TOK) << endl;
		assert(0);
	break;
	}
}

static inline void fail()
{
	lexer->melt();
	longjmp(beginning, 1);
}

static void message(char *prefix, char *mess)
{
	cerr << prefix << ": line " << (lexer->emptyWindow()? 1 : LINENO) << " " << lexer->filename();
	if (mess)
		cerr << ": " << mess;
	cerr << endl;
}

/*
static void sorry(char *mess)
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

struct lws {
	const char *s;
	int len;
};

static const lws 
dagkeyword[] = {
	"draw", 4,
	"width", 5,
	"height", 6,
	"as", 2,
	"edge", 4,
	"from", 4,
	"to", 2,
	"path", 4,
	"backpath", 8,
	"backedge", 8,
	"weight", 6,
	"label", 5,
	"solid", 5,
	"dashed", 6,
	"dotted", 6,
	"invis", 5,
	"separate", 8,
	"minimum", 7,
	"maximum", 7,
	"rank", 4,
	"same", 4,
	"nodes", 5,
	"ranks", 5,
	"exactly", 7,
	"equally", 7,
	0, 0,
};

static int mytolower(char c) {
	if (isupper(c))
		return c - 'A' + 'a';
	return c;
}

static int case_insensitive_equal(const char* p, const char* q) {
	assert(strlen(p) == strlen(q));
	//assert(string pointed to by q is all lower case);
	while (*p != '\0') {
		if (mytolower(*p) != *q)
			return 0;
		++p;
		++q;
	}
	assert(*q == '\0');
	return 1;
}
	
static int isdagkeyword(const String &s) {
	const char *p = s;
	for (const lws *key = dagkeyword; key->len != 0; ++key) {
		if ((s.length() == key->len) && (case_insensitive_equal(p, key->s)))
			return 1;
	}
	return 0;
}

class Aggtag {
	String s;
	char isdagkey;
public:
	Aggtag() : isdagkey(0) {}
	Aggtag(const String &s_) : s(s_) { isdagkey = ::isdagkeyword(s_); }
	void operator=(const String &s_) { s = s_; isdagkey = ::isdagkeyword(s_); }
	const String & getString() const { return s; }
	int isdagkeyword() const { return isdagkey; }
};

ostream & operator<<(ostream &os, const Aggtag &a) 
{
	// always put those quotes, since dag will probably
	// choke on nested and parameterized base class names
	//if (a.isdagkeyword()) 
		// that extra space causes dag not to treat it as a keyword
		//              |
		//               ------------
		//                           |
		//                           v
		os << '\"' << a.getString() << " \"";
	return os;
}

static void emitDrawCommand(const Aggtag &tag, TokenType agg)
{
	assert(agg == CLASS || agg == UNION || agg == STRUCT ||agg == TEMPLATE);
	if (agg == STRUCT)
		*dagout << "draw " << tag << " as Box;\n";
	else if (agg == UNION)
		*dagout << "draw " << tag << " as Diamond;\n";
}

static void emitEdge(const Aggtag &from, const Aggtag &to, Protection prot, int isvirtual)
{
	if (asciiOutput)
	{
		*asciiout << to.getString() << ' ' << prot << ' ' << from.getString();
		if (isvirtual)
			*asciiout << " virtual";
	}
	if (backwardEdges)
		*dagout << to << ' ' << from;
	else
		*dagout << from << ' ' << to;
	if (diffInher)
	{
		if (prot != Public)
			*dagout << " dashed";
		if (isvirtual)		
			*dagout << " label \"virtual\"";
	}
	*dagout << ';';
	if (whereFrom)
	{
		*dagout << "\t# " << lexer->lineno() << " \"" << lexer->filename() << '\"';
		if (asciiOutput)
			*asciiout << "\t# " << lexer->lineno() << " \"" << lexer->filename() << '\"';
	}
	if (asciiOutput)
		*asciiout << endl;
	*dagout << '\n';
}

static Aggtag currentTag;
static TokenType currentTagAgg;

static void classHead()
{
	assert (TOK == CLASS || TOK == STRUCT || TOK == UNION);
	currentTagAgg = TOK;
	ADV;
	if (TOK == LC)
	{
		ADV;
	}
	else
	{
		currentTag = LEXEME;
		if (diffAggs && !isolateds)  // then we have to mess with the map.
		{
			aggtyp[currentTag.getString()] = currentTagAgg;
		}
		match(ID);
		if (TOK == COLON)
		{
			baseList();
			match(LC);
		}
		else
		{
			// LC or anything else
			ADV;
		}
		if (diffAggs && isolateds)	// then we don't have to mess with the map.
		{
			emitDrawCommand(currentTag, currentTagAgg);
		}
		if (isolateds)
		{
			if (asciiOutput)
				*asciiout << currentTag << endl;
			*dagout << currentTag << ";\n";
		}
	}
}

// consumed <>, adv to the word "class", call baseList()
// template is always remembered as it is. and treated similar to class
static void templateHead()
{
	String	tempName;
	
	currentTagAgg = TOK;
	match(TEMPLATE);

	if (TOK == LANGLE)
	{
		// consume <class...>
		advToMatchingCollectOnlyClass(tempName);

		ADV;
		if( TOK != CLASS ) return;//reading function template
		ADV;

		currentTag = LEXEME;//name
		tempName =  LEXEME + tempName;

		if (diffAggs && !isolateds )
		{
			aggtyp[ currentTag.getString() ] = currentTagAgg;
		}
		match(ID);

		if( TOK == COLON )
		{
			currentTag = tempName;
			baseList();
			match(LC);
		}
		else
			// LC or anything else
			ADV;

		if( diffAggs && isolateds )
			//emitDrawCommand( currentTag, currentTagAgg );
			emitDrawCommand( tempName, currentTagAgg );

                if (isolateds)
                {
                        if (asciiOutput)
                                *asciiout << (Aggtag)tempName << endl;
                        *dagout << (Aggtag)tempName << ";\n";
                }
	}
	else
	{
		syntaxError("Missing <");
	}
}
static void baseList()		
{
	match(COLON);

	baseUnit();
	while (TOK == COMMA)
	{
		match(COMMA);
		baseUnit();
	}
}

static void foundAnInheritanceEdge(const String &basetype, Protection prot, int isvirtual)
{
	Aggtag base(basetype);
	emitEdge(base, currentTag, prot, isvirtual);
	if (diffAggs && !isolateds)  // then we have to mess with the map.
	{
		emitDrawCommand(currentTag, currentTagAgg);
		
		// for template, only use the name to index
		int	i = base.getString().index( '<' );

		if ( i != -1 )
		{
			String name = base.getString().chunk( 0, i );

			emitDrawCommand(base, aggtyp[ name ]);

		}
		else
		if (aggtyp.element(base.getString()))
		{
			emitDrawCommand(base, aggtyp[base.getString()]);
		}
		else
		{
			cerr << "hier: error: class " << currentTag.getString();
			cerr << " trying to inherit from undefined class " << base.getString() << '.' << endl;
		}
	}
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

// adv to the right grouping token and collect the dummy arg after class in the
// angle bracket
// also put the argument on a list
static void advToMatchingCollectOnlyClass(String &s)
{
        TokenType left = TOK;
        TokenType right = rightGroupTok(left);

        int depth = 1;
        while (depth > 0 && TOK != EOFTOK)
        {
		if( TOK != CLASS )
		{
			s += LEXEME;
		}

                ADV;
                if (TOK == left) depth++;
                else if (TOK == right) depth--;
		else if (TOK == LC)
			syntaxError("Missing >");  // hack.  currently this function is only called with TOK==<.
        }
        if (TOK == EOFTOK)
                syntaxError("Unexpected end of file: missing >");
	s += LEXWS;
	s += LEXEME;
}


// advance to the right grouping token matching the current token.
// while doing so, append lexemes and whitespace to the string s.
static void advToMatchingAppendingTo(String &s)
{
        TokenType left = TOK;
        TokenType right = rightGroupTok(left);

        int depth = 1;
        while (depth > 0 && TOK != EOFTOK)
        {
		s += LEXWS;
		s += LEXEME;
                ADV;

                if (TOK == left) depth++;
                else if (TOK == right) depth--;
		else if (TOK == LC)
			syntaxError("Missing >");  // hack.  currently this function is only called with TOK==<.
        }
        if (TOK == EOFTOK)
                syntaxError("Unexpected end of file: missing >");
	s += LEXWS;
	s += LEXEME;
}

static String collect_basetype_name()
{
	String s = LEXEME;
	match(ID);
	if (TOK == LANGLE)
	{
		advToMatchingAppendingTo(s);
		ADV;
	}
	while (TOK == QUAL)
	{
		s += "::";
		ADV;
		s += LEXEME;
		match(ID);
	}
	return s;
}		

static void baseUnit()
{
	int isvirtual = 0;
	Protection prot = (currentTagAgg == CLASS ||
			   currentTagAgg == TEMPLATE)? Private: Public;

	if (TOK == VIRTUAL)
	{
		isvirtual = 1;
		ADV;
	}
	switch (TOK) 
	{
		case PUBLIC:
			prot = Public;
			ADV;
			break;
		case PRIVATE:
			prot = Private;
			ADV;
			break;
		case PROTECTED:
			prot = Protected;
			ADV;
			break;
		default:
			break;
	}
	if (!isvirtual && (TOK == VIRTUAL))
	{
		isvirtual = 1;
		ADV;
	}
	if (TOK == ID)
	{
		String s = collect_basetype_name();
		foundAnInheritanceEdge(s, prot, isvirtual);
	}
}

