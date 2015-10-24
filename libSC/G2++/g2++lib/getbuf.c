/*ident	"@(#)G2++:g2++lib/getbuf.c	3.1" */
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

#include <g2debug.h>
#include <g2ctype.h>
#include <g2io.h>
#include <g2manip.h>
#include <stdlib.h>
#include <iostream.h>
#ifdef __GNUG__
#define IOSTREAMH
#endif

static char sccs_id[] = "@(#)G2++ library, version 093096";

extern int seekflag_ATTLC;     // should be in a header file
extern String seekname_ATTLC;  //             "

//  Local functions

static int 
discard(int c, istream& is);
static G2BUF* 
getbody(G2BUF* bp, const String& name, istream& is);
static int 
getchild(G2BUF* bp, int lvl, int peek, istream& is,
  G2NODE *&parent_node, const String& name);
static int xindent; // for passing indent between levels

int getchar_G2_ATTLC(istream& is) {
    int retval;
#ifdef IOSTREAMH
    retval = is.get();
    if(retval==BS_EOF)is.clear(is.rdstate() | ios::failbit);
#else
    unsigned char c_char;
    is.get(c_char);
    if(is) retval = c_char;
    else retval = BS_EOF;
#endif
    return retval;
}

G2BUF* 
getbuf_ATTLC(G2BUF* bp, istream& is){
    DEBUG(cerr << "enter getbuf_ATTLC\n";)
    g2init_ATTLC(bp);

//  If seekflag is set, this should be a call
//  to getbody

    if (seekflag_ATTLC){
	seekflag_ATTLC=0;
	return(getbody(bp,seekname_ATTLC,is));
    }

    String name=getname_ATTLC(is);

    if (name.is_empty()){
	return NULL;
    }
    DEBUG(cerr 
	<< "in getbuf_ATTLC, get name=" 
	<< name 
	<< "\n"
    ;)
    return getbody(bp,name,is);
}

static G2BUF* 
getbody(G2BUF* bp, const String& name, istream& is){
    DEBUG(cerr 
	<< "enter getbody with name=" 
	<< name 
	<< "\n"
    ;)
    int cur;
    G2NODE header_node;
    header_node.child = 0;
    header_node.next = 0;

//  Clear seekflag in case it is set

    seekflag_ATTLC=0;
	
//  Local node buffer allocation pointer  (incremented)

    cur = getchar_G2_ATTLC(is);
    if (cur == BS_EOF){
	return NULL;
    }
    
//  Call getchild

    DEBUG(cerr << "ready to call getchild\n";)
    G2NODE *header_ptr = &header_node;
    cur = getchild(
	bp,               // pointer to G2BUF
	0,                // level
	cur,              // current character
	is,               // input stream 
	header_ptr,       // pointer to parent node
	name              // record name
    );
    
//  Fix up 

    bp->root = header_node.child;
    DEBUG(cerr << "back in getbody, *bp=\n";)
    DEBUG(showbuf_ATTLC(bp);)

//  Process attributes
 
    while (cur == '.'){
	cur = _g2getdot_ATTLC(is);
    }
    DEBUG(cerr 
	<< "after getdot loop, cur = ASCII " 
	<< cur
	<< " ("
	<< char(cur)
	<< ")"
	<< "\n"
    ;)
    if (cur == '\n' || cur == BS_EOF){
	return bp;
    }
    if (isname1_ATTLC(cur)){
	DEBUG(cerr << "isname1_ATTLC(cur) returns 1\n";)
	is.putback(cur);
	return bp;
    }
    _g2sync_ATTLC(cur,is);
    DEBUG(cerr 
	<< "after sync, ready return with cur = ASCII " 
	<< cur
	<< " ("
	<< char(cur)
	<< ")"
	<< "\n"
    ;)
    return bp;
}

static int 
getchild(G2BUF* bp, int level, int c, istream& is,
    G2NODE *&parent_node, const String& name){
    DEBUG(cerr 
	<< "enter getchild with\n"
        << "    level=" 
	<< level 
	<< "\n"
	<< "    c = ASCII "
	<< c
	<< " ("
	<< char(c)
	<< ")"
	<< "\n"
	<< "    parent_node = "
	<< (void*)parent_node
	<< "\n"
	<< "    name="
	<< name
	<< "\n"
    ;)
    int	indent = level; 
    int first = 1;
    G2NODE* curr_node = 0;
    int curr_offset = -1;	// added to solve curr_node error - dmm 5/13/96

    if (level >= G2MAXDEPTH_ATTLC){
	g2error_ATTLC(G2TOODEEP);
	return c;
    }

top:
    DEBUG(cerr << "at label top\n";)

    // added to solve curr_node error - dmm 5/13/96
    if (curr_offset != -1)
	    curr_node = (G2NODE*)((char*)(bp->base) + curr_offset);
    else
	    curr_node = 0;

    for(;;){

	if (c == BS_EOF){
	    DEBUG(cerr << "hit eof -- break\n";)
	    indent = 0;
	    break;
	}

//  Linkage 

	if (bp->ptr >= bp->end){
	    // abort();  // jfi

//  The following code is wrong.  Changing the size of
//  a block relocates the contents, invalidating 
//  pointers (jfi).  For now, we prevent the code
//  from executing by aborting (see above).

//	    bp->buf.size((unsigned int)(1.414*bp->buf.size()));
//	    bp->base = bp->ptr = bp->buf;
//	    DEBUG(cerr 
//		<< "increase node buffer size to " 
//		<< (G2NODE*)bp->buf.end()-(G2NODE*)bp->buf 
//		<< "\n"
//	    ;)

//  The following code should be right.  Changing the
//  size of a block will relocate the contents, so all
//  of the pointers must be modified.  The only pointers
//  into this Block are:
//
//   - the bp->base, bp->ptr, bp->buf, and bp->end pointers
//   - the child and next pointers of the G2NODE objects
//     within the Block
//   - the parent_node pointer and its child and next fields (if level != 0)
//   - the curr_node pointer

	    G2NODE *oldbase = bp->base;
	    G2NODE *oldpnchild = parent_node->child;
	    G2NODE *oldpnnext = parent_node->next;

	    DEBUG(cerr 
		<< "before:\n"
		<< "bp->base=" << (void*)bp->base << "\n"
		<< "bp->ptr=" << (void*)bp->ptr << "\n"
		<< "bp->buf=" << (void*)(G2NODE*)bp->buf << "\n"
		<< "bp->end=" << (void*)bp->end << "\n"
		<< "parent_node->child=" << (void*)parent_node->child << "\n"
		<< "parent_node->next=" << (void*)parent_node->next << "\n"
		<< "curr_node=" << (void*)curr_node << "\n"
	    ;)

	    int oldsize = bp->buf.size();
	    bp->buf.size((unsigned int)(1.414*bp->buf.size()));
	    bp->base = bp->ptr = bp->buf;
	    bp->end = bp->buf.end();
	    DEBUG(cerr 
		<< "increase node buffer size to " 
		<< (G2NODE*)bp->buf.end()-(G2NODE*)bp->buf 
		<< "\n"
	    ;)

	    // this is the amount of the "fixup" that needs to
	    // be applied to all pointers
	    int diff = (char*)bp->base - (char*)oldbase;
	    DEBUG(cerr << "diff=" << diff << "\n" ;)

	    // fix up all of the pointers
	    if (oldsize != 0 && diff != 0) {
		G2NODE *newbase = bp->base;
		if (level != 0) {
		    parent_node = (G2NODE*)((char*)parent_node + diff);
		    //parent_node->child = (G2NODE*)((char*)oldpnchild + diff);
		    //parent_node->next = (G2NODE*)((char*)oldpnnext + diff);
		}

		if (curr_node != 0)    // added to solve curr_node error - dmm 5/13/96
		    curr_node = (G2NODE*)((char*)curr_node + diff);

		int i;
		for (i = 0; i < oldsize; i++) {
		    if (newbase->child != 0)
			newbase->child = (G2NODE*)((char*)newbase->child + diff);
		    else newbase->child = 0;
		    if (newbase->next != 0)
			newbase->next = (G2NODE*)((char*)newbase->next + diff);
		    else newbase->next = 0;
		    newbase++;
		}
		bp->ptr = newbase;
	    }
	    DEBUG(cerr 
		<< "after:\n"
		<< "bp->base=" << (void*)bp->base << "\n"
		<< "bp->ptr=" << (void*)bp->ptr << "\n"
		<< "bp->buf=" << (void*)(G2NODE*)bp->buf << "\n"
		<< "bp->end=" << (void*)bp->end << "\n"
		<< "parent_node->child=" << (void*)parent_node->child << "\n"
		<< "parent_node->next=" << (void*)parent_node->next << "\n"
		<< "curr_node=" << (void*)curr_node << "\n"
	    ;)
////// end of new code for reallocating the Block
	}
	if (first){
	    DEBUG(cerr << "first pass through loop\n";)
	    first = 0;
	    curr_offset = (char*)(bp->ptr) - (char*)(bp->base);
	    curr_node = parent_node->child = bp->ptr++;
	    DEBUG(cerr 
		<< "set curr_node and parent_node->child to " 
		<< (void*)curr_node
		<< "\n"
	    ;)
	}else{
	    DEBUG(cerr << "subsequent pass through loop\n";)
	    curr_offset = (char*)(bp->ptr) - (char*)(bp->base);
	    curr_node = curr_node->next = bp->ptr++;
	    DEBUG(cerr 
		<< "set curr_node and curr_node->next to " 
		<< (void*)curr_node
		<< "\n"
	    ;)
	}
	curr_node->next = NULL;
	curr_node->child = NULL;

//  Get name 

	if (!name.is_empty()){
	    DEBUG(cerr << "name is non-empty\n";)

//  True when the 'name' parameter is non-null 
//  (when getchild is called from getbody)

	    DEBUG(cerr 
		<< "before assignment:\n"
	        << "    name = " 
		<< name 
		<< "\n"
		<< "    curr_node=" 
		<< (void*)curr_node
		<< "\n"
	        << "    curr_node->name=" 
		<< curr_node->name 
		<< "\n"
	    ;)
	    curr_node->name = name;
	    DEBUG(cerr 
		<< "curr_node->name = " 
		<< curr_node->name 
		<< "\n"
	    ;)

	}else{

//  True when 'name' parameter is the empty string
//  (when called recursively).  
//  Get the name from the input file.

	    DEBUG(cerr << "name is empty\n";)

	    curr_node->name="";
	    if (c == '.'){
                curr_node->name=".";
		c = getchar_G2_ATTLC(is);
	    }
	    DEBUG(cerr << "get into curr_node->name: ";)
	    while (isname2_ATTLC(c)){
		DEBUG(cerr << char(c);)
		curr_node->name += c;
		c = getchar_G2_ATTLC(is);
	    }
	    DEBUG(cerr << "\n";)
	    DEBUG(cerr 
		<< "exit from loop with c = ASCII " 
		<< c
		<< " ("
		<< char(c)
		<< ")"
		<< "\n"
	    ;)
	}
	
	curr_node->val=""; // dmm, 11/17/95
	if (c == '\t'){

//  Get value 

	    DEBUG(cerr << "c is a tab -- append value: ";)

	    curr_node->val="";      // jfi
	    for( ;; ){
		c = getchar_G2_ATTLC(is);
		if (!is || !isprint_ATTLC(c)) break;
		curr_node->val += c;
		DEBUG(cerr << char(c);)
	    }
	    DEBUG(cerr << "\n";)
	}
	if (c != '\n' && (c=discard(c,is)) == BS_EOF){
	    indent = 0;
	    break;
	}
	for(;;){

//  Get indent 

	    DEBUG(cerr << "at top of inner for(;;), count tabs\n";)
	    indent = 0;
	    for( ;; ){
		c = getchar_G2_ATTLC(is);
		if( c!= '\t' )break;
		indent++;
	    }
	    DEBUG(cerr 
		<< "indent, c = " 
		<< indent 
		<< "," 
		<< char(c)
		<< "\n"
	    ;)
	    if (isname2_ATTLC(c) || c == '.'){    

//  Decisions
                DEBUG(cerr 
		    << "make decisions for indent, level=" 
		    << indent 
		    << "," 
		    << level 
		    << "\n"
		;)
		if (indent == level+1){

//  This is a child: recurse

		    DEBUG(cerr << "ready to call getchild recursively\n";)
		    c=getchild(
			bp,       // pointer to G2BUF
			level+1,  // level
			c,        // current character
			is,       // input file
			curr_node,       // pointer to parent 
			""        // no name
		    );
		    indent = xindent;
		    DEBUG(cerr 
			<< "back from call to getchild, set indent to " 
			<< xindent 
			<< "\n"
		    ;)

		}
		if (indent <= 0){
//  End of record (?)
		    DEBUG(cerr << "indent<=0 (end of record)\n";)
		    goto out;
		}
		if (indent == level){
//  This is a sibling
		    DEBUG(cerr << "indent==level (a sibling)\n";)
		    goto top;
		}
		if (indent < level){
//  End of siblings
		    DEBUG(cerr << "indent <level (end of siblings)\n";)
		    goto out;
		}
//  (indent > level+1) falls through 
	    }
	    DEBUG(cerr << "indent>level+1 (fall thru)\n";)
	    if (indent == 0){
		DEBUG(cerr << "indent==0\n";)
		if (c == '\n'){
		    DEBUG(cerr << "c is a newline\n";)
		    goto out;
		}
	    }
	    if ((c=discard(c,is)) == BS_EOF){
		indent = 0;
		goto out;
	    }
	}
    }

out:
    if (level == 0)
	parent_node->child = bp->base;

    xindent = indent;
    DEBUG(cerr 
	<< "at label out, return c = ASCII " 
	<< c
	<< " ("
	<< char(c)
	<< ")"
	<< "\n"
    ;)
    return c;
}

static int 
discard(int c, istream& is){
    DEBUG(cerr << "discard characters\n";)
    while (c != '\n' && c != BS_EOF){
	c = getchar_G2_ATTLC(is);
	DEBUG(
	    cerr 
		<< "is.get() returns c = ASCII " 
		<< c 
		<< " ("
		<< char(c)
		<< " )"
		<< "\n"
	    ;
	)
    }
    DEBUG(cerr << "exit from discard loop\n";)
    DEBUG(
	void* test = (void*)is;
	if(test){
	    cerr << "is tests non-null\n";
	}else{
	    cerr << "is tests null\n";
	}
    )
    return c;
}
