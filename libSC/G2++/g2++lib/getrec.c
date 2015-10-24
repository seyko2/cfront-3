/*ident	"@(#)G2++:g2++lib/getrec.c	3.1" */
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

#include <g2ctype.h>
#include <g2debug.h>
#include <g2io.h>
#include <g2manip.h>
#include <Vblock.h>
#include <assert.h>
#include <iostream.h>
#include <stdlib.h>
#include <osfcn.h>
#include <String.h>
#if !defined(_MSC_VER) && !defined(__TCPLUSPLUS__)
#include <libc.h>
#ifdef _LIBC_INCLUDED
#define HP_NATIVE_COMPILER
#endif
#endif
#ifdef __GNUG__
#define IOSTREAMH
#endif

static char sccs_id[] = "@(#)G2++ library, version 093096";

#ifdef IOSTREAMH
#define	GCOUNT	is.gcount()
#else
#define	GCOUNT	gcount_ATTLC(p)
static int
gcount_ATTLC(const char* p) {
	register int	i=0;
	while(*p != '\0' && *p++ != '\n')
		++i;
	return i;
}
#endif

extern int seekflag_ATTLC;  // should be in a header file

//  Local functions

static int 
discard(int, istream&);
void* 
getbody(void* rec, G2DESC* rd, istream& is);
int  
getchildren(int, int, istream&, void*, G2DESC*);
static int
get_integer(int, int &, istream&);
int  
get_builtin(int, void*, int, istream&);
static void 
clearleaf(void*, int); 

void* 
getrec_ATTLC(void* rec, G2DESC* rd, istream& is) {
	DEBUG(cerr << "enter getrec_ATTLC\n";)

//  If seekflag is set, this should be a call
//  to getbody

	if (seekflag_ATTLC) {
		getbody(rec,rd,is);
	}
	else {
//  Get G2++ record (name and body) skipping 
//  any records of wrong type.
		String name;
		do {
			name=getname_ATTLC(is);
			DEBUG(cerr << "call getname_ATTLC to scan for next record\n";)
			if (name.is_empty()) {
				DEBUG(cerr << "getname_ATTLC returned \"\"; return NULL\n";)
				return NULL;
			}
			DEBUG(cerr << "getname_ATTLC returned name=" 
				<< name << "\n";)
			if (name.is_empty()) {
				return 0;	   // end-of-file
			}
		} while (name!=rd->name);

		DEBUG(cerr << "found the record type we're looking for!\n";)
		DEBUG(cerr << "ready to call getbody\n";)
		getbody(rec, rd, is);
	}
	seekflag_ATTLC=0;
	return rec;
}

//  Get body (less root name) of a G2++ record.  
//  Must be preceded by a call to getname_ATTLC().
void* 
getbody(void* rec, G2DESC* rd, istream& is) {
	DEBUG(cerr << "enter getbody\n";)

//  Clear seekflag in case it is set

	seekflag_ATTLC=0;

//  Set the members of the structure
//  pointed to by rec to their default 
//  initial values

	DEBUG(cerr << "ready to call g2clear_ATTLC\n";)
	g2clear_ATTLC(rd,rec);
	DEBUG(cerr << "back from g2clear_ATTLC\n";)

	int c;
	int indent;

	c = getchar_G2_ATTLC(is);

	DEBUG(cerr << "is.get() returns c = ASCII " << c 
		<< " (" << char(c) << " )" << "\n";)
	DEBUG(void* test = (void*)is;
		if(test){cerr << "is tests non-null\n";}
		else{cerr << "is tests null\n";}
	)
	DEBUG(cerr << "rd->type == '" << char(rd->type) << "'\n";)
	if (rd->type == 'L') {

//  Record is a leaf: load value and return

		DEBUG(cerr << "LEAF\n";)
		if (c == '\t') {
			DEBUG(cerr << "c is tab\n";)
			int size = rd->size;
			if (size >= 0) {
//  User-defined type (case 1)
				DEBUG(cerr << "User-defined type (case 1)\n";)
				rd->gfn(is,rec);
				DEBUG(cerr << "back from call to gfn\n"; )
				c=getchar_G2_ATTLC(is);
				DEBUG(cerr << "is.get() returns c = ASCII " 
					<< c << " (" << char(c)	<< " )"
					<< "\n";)
				DEBUG(void* test = (void*)is;
					if(test){cerr << "is tests non-null\n";}
					else{cerr << "is tests null\n";}
				)
			}
			else {
//  Get a built-in type
				DEBUG(cerr << "size<0\n";)
				c = (get_builtin(size, rec, rd->nel, is));
			}
		}
		if (c != '\n') {
			if ((discard(c, is)) == BS_EOF) {
				DEBUG(cerr << "hit eof - ready to return\n";)
				return rec;
			}
		}
		c = getchar_G2_ATTLC(is);
		DEBUG(cerr << "is.get() returns c = ASCII " << c 
			<< " ("	<< char(c) << " )" << "\n";)
		DEBUG(void* test = (void*)is;
			if(test){cerr << "is tests non-null\n";}
			else{cerr << "is tests null\n";}
		)
	}
	else {
//  Nonleaf: get next indent 
		DEBUG(cerr << "NONLEAF\n";)
		if (rd->type == 'A') {
			DEBUG(cerr << "in top level array case\n";)
			int size1;
			Vb_ATTLC* aptr = (Vb_ATTLC*) rec;
			DEBUG(cerr << "calling get_integer\n";)
			c = get_integer(c, size1, is);
			DEBUG(cerr << "get_integer returns " << size1 << "\n";);
			if (size1 > aptr->size()) {
				DEBUG(cerr << "resizing array to " << size1 << "\n";)
				aptr->size(size1);
			}
		}
		c=discard(c,is);

		if (c != BS_EOF) {
			for (;;){
				DEBUG(cerr << "at top of for(;;) loop\n";)
				indent=0;
				for(;;) {
					c = getchar_G2_ATTLC(is); 
					DEBUG(cerr << "is.get() returns c = ASCII " 
						<< c << " (" << char(c)	<< " )"
						<< "\n";)
					DEBUG(void* test = (void*)is;
						if(test){cerr << "is tests non-null\n";}
						else{cerr << "is tests null\n";}
					)
					if (c != '\t')break;
					indent++;
				}
				DEBUG(cerr << "counted " << indent << " tabs\n";)
				DEBUG(cerr << "broke from loop with c = ASCII " 
					<< c << " (" << char(c)	<< " )"	<< "\n";)
				if (indent==1) {
					DEBUG(cerr << "indent==1\n";)
					if (isname2_ATTLC(c)) {
						DEBUG(cerr << "isname2_ATTLC(c) is true\n";)
						DEBUG(cerr << "ready to call getchildren()\n";)
						// getchildren(level, current character,
						//  input stream, base,
						//  parent's record descriptor)
						c = getchildren(1, c, is, rec, rd);
						DEBUG(cerr <<
							"return from getchildren with c = ASCII " 
							<< c << " (" <<
							char(c)	<< " )"
							<< "\n";)
						if (c == BS_EOF) is.clear(is.rdstate()&~ios::eofbit);
						break;
					}
					else {
						DEBUG(cerr << "isname2_ATTLC(c) is false\n";)
					}
				}
				else if (indent==0) {
					DEBUG(cerr << "indent==0\n";)
//  No numbers at level 0; dot is ok 
					if (c == '\n' || c == '.' || isname1_ATTLC(c)){
						break;
					}
				}
				if ((c=discard(c,is)) == BS_EOF) {
					indent = 0;
					break;
				}
			}
			DEBUG(cerr << "exit from for(;;) loop\n";)
		}
	}
	DEBUG(cerr << "COMMON POSTPROCESSING\n";)
	while (c == '.') {
		DEBUG(cerr << "call _g2getdot_ATTLC\n";)
		c = _g2getdot_ATTLC(is);
	}
	if (c == '\n' || c == BS_EOF) {
		return rec;
	}
	if (isname1_ATTLC(c)) {
		DEBUG(cerr << "is.putback(c = ASCII " << c << " ("
			<< char(c) << " )" << ")\n";)
		is.putback(c);
		return rec;
	}
	DEBUG(cerr << "_g2sync_ATTLC(c,is)\n";)
	_g2sync_ATTLC(c, is);
	return rec;
}

//  Get children 
/* On entry to getchildren(), the file pointer is at the
   first non-tab character on the line after a struct or array
   fieldname. The G2DESC pointer "par" points to the type of
   the parent node (either a struct or an array). */
int 
getchildren(int level, register int c, istream& is, void* base, G2DESC* par) { 
	static int 	xindent; // for passing indent between levels 
	int		indent = level; 
	G2DESC* 	top = par->child;
	G2DESC* 	cur = top;
	G2DESC* 	onetoofar = top + par->nel;
	int		quit=0;
	int 	nel;
	void*	oldbase;

	DEBUG(cerr << "enter getchildren with\n" << "	level=" << level << "\n"
		<< "	c = ASCII " << c << " (" << char(c) << " )"<< "\n"
		<< "	base=" << (void*)base << "\n";)
	if (par->type == 'S') {
		DEBUG(cerr << "par->type is 'S'\n";)
		nel = par->nel;
		DEBUG(cerr << "par->nel=" << par->nel << "\n"
			<< "nel=" << nel << "\n";)
	}
	else if (par->type == 'A') {
		DEBUG(cerr << "par->type is 'A'\n";)

//  Compute nel as the number of block elements,
//  as distinct from par->nel, which is used only
//  pre-allocating space.

		nel = ((Vb_ATTLC*)base)->size();
		DEBUG(cerr << "par->nel=" << par->nel << "\n"
			<< "nel=" << nel << "\n";)

//  Recompute 'base' as the address of the 0th element

		oldbase=base;  // address of vblock
		DEBUG(cerr << "oldbase=" << (void*)oldbase << "\n";)
		base = ((Vb_ATTLC*)base)->beginning();  // start of elts
		DEBUG(cerr << "base=" << (void*)base << "\n";)
	}

//  Loop over siblings

	for( ; ; ){
		String  	name(Stringsize(10));
		int		offset;
		G2DESC*		hit = NULL; 
		G2DESC* 	marker;

		DEBUG(cerr << "AGAIN (at top of sibling for(;;) loop)\n";)
		if (c == BS_EOF) {
			indent = 0;
			break;
		}

//  Get name 

		do {
			name += c;
			c = getchar_G2_ATTLC(is);
			DEBUG(cerr << "is.get() returns c = ASCII " 
				<< c << " (" << char(c)	<< " )"	<< "\n";)
			DEBUG(void* test = (void*)is;
				if(test){cerr << "is tests non-null\n";}
				else{cerr << "is tests null\n";}
			)
		} while(isname2_ATTLC(c));

		DEBUG(cerr << "after do while, name = " << name << "\n";)

		if (par->type == 'A'){
//  Parent is an array -- name must be an index
			DEBUG(cerr << "par->type == 'A' (name should be an index)\n";)
			if (!isdigit_ATTLC(name.char_at(0))) {
				g2error_ATTLC(G2INDEXREQ);
			}
			else {
				hit = top;	// the only one
				DEBUG(cerr << "hit=\n";)
				DEBUG(showdesc_ATTLC(hit);)
//  Decode the index
				register int index = 0;
				for (int j=0;
				     j<name.length() && isdigit_ATTLC(name.char_at(j));
				     j++) {
					index = index*10 + name.char_at(j) -'0';
				}
				DEBUG(cerr << "after j-loop, index=" << index 
					<< "\n"	<< "whereas nel=" << nel
					<< "\n";)
				if (index >= nel) {
//  The index exceeds the current array bound
					if (par->nel==0 || nel<par->nel) {
//  Grow the array
						/* save the old size */
						int old_nel = nel;
						DEBUG(cerr << "must grow the array\n";)
						DEBUG(cerr << "old nel=" 
							<< nel << "\n";)
						if (par->nel>0){ // fixed
							nel = par->nel;
						}
						else {// flexible
							if (nel<=2) {
								nel=(index<10 ?(10):((index+1)*2));
							}
							else {
								while (index>=nel) {
									nel = int(nel*1.414);
								}
							}
						}
						DEBUG(cerr << "new nel=" 
							<< nel << "\n";)
						((Vb_ATTLC*)oldbase)->size(nel);
						base = ((Vb_ATTLC*)oldbase)->beginning();
						DEBUG(cerr << "((Vb_ATTLC*)oldbase)->size()=="
							<< ((Vb_ATTLC*)oldbase)->size()
							<< "\n"
							<< "new value of base="
							<< (void*)base<< "\n";)

			/* the size has been changed, so we might have
			   to initialize new elements in the array */
						if (hit-> size == 0) {
							int lcl_index;
							DEBUG(cerr << "initializing new array elements\n";)
							for (lcl_index = old_nel; lcl_index < nel; lcl_index++) {
								hit->nfn((void*)((char*)base + lcl_index * hit->nel));
							}
						}
					}
					else {
//  The array is fixed -- ignore this element
						DEBUG(cerr << "array is not flexible; "
							<< "ignore this index"
							<< "\n";)
						hit=0;	// jfi
					}
				}
//  Compute the offset

				if (hit) {
					offset = index * REALSIZE(hit->size);
					if (hit->size == 0) {
						offset = index * hit->nel; /* user-defined type */
					}
					DEBUG(cerr << "compute offset:\n"
						<< "	hit->size=" 
						<< hit->size << "\n"
						<< "	REALSIZE(hit->size)=" 
						<< REALSIZE(hit->size) << "\n"
						<< "	offset=" 
						<< offset << "\n";)
				}
			}
		}
		else {
//  Parent is a structure
			DEBUG(cerr << "parent type is 'S'\n";)
//  Lookup: 
//
//	  A circular table of length n is searched
//	  for a match. Regardless of success, the current
//	pointer (cur) is left on the "next" table entry.
//	Assuming that an identical descriptor was used to write
//	the incomming record, for fully populated records
//	the average number of unsuccessful matches will be zero.

			marker = cur;	// lap marker

			do {
				DEBUG(cerr << "at top of lookup loop, cur:\n";)
				DEBUG(showdesc_ATTLC(cur);)

//  Welcome to the inner loop (this stuff should be fast).

				DEBUG(cerr << "compare name '" << name 
					<< "' with cur->name '" << cur->name 
					<< "'\n";)
//  Match?
				if (name==String(cur->name)) {
					DEBUG(cerr << "hit! (the names are equal)\n";)
					offset = cur->offset;
					DEBUG(cerr << "offset=" << offset 
						<< "\n";)
					hit = cur++;
					if (cur >= onetoofar) {
						DEBUG(cerr << "must wrap around\n";)
						cur = top;
					}
					break;
				}
				DEBUG(cerr << "the names are not equal\n";)
				cur++;
//  Wrap? 
				if (cur >= onetoofar) {
					DEBUG(cerr << "must wrap around\n";)
					cur = top;
				}
				DEBUG(cerr << "cur:\n";)
				DEBUG(showdesc_ATTLC(cur);)
			} while (cur != marker);
			DEBUG(cerr << "MATCHED (exit from lookup loop)\n";)
		}

//  For both arrays and structures...

		if (c == '\t' && hit && hit->type == 'L') {
			DEBUG(cerr << "ready to stuff leaf value\n";)

//  Stuff 'hit->size' bytes into record starting at 
//  a location 'cp' computed from base and offset.  

			char* cp = (char*)base + offset;
			DEBUG(cerr << "	base=" << (void*)base << "\n"
				<< "	offset="  << offset << "\n"
				<< "	cp=" << (void*)cp << "\n"
				<< "	hit->size=" << hit->size << "\n";)
			if (hit->size >= 0) {

//  User-defined type (case 2)

				DEBUG(cerr << "User-defined type (case 2)\n";)
				hit->gfn(is,cp);
				DEBUG(cerr << "back from call to gfn\n"; )
				c=getchar_G2_ATTLC(is);
				DEBUG(cerr << "is.get() returns c = ASCII " 
					<< c << " (" << char(c)
					<< " )" << "\n";)
				DEBUG(void* test = (void*)is;
					if(test){cerr << "is tests non-null\n";}
					else{cerr << "is tests null\n";}
				)
			}
			else {
				DEBUG(cerr << "hit->size<0: ready to call get_builtin\n";)
				c = (get_builtin(hit->size, cp, hit->nel, is));
			}
		}
/* This code initializes the size of an
   array based on the size in the G2++
   record */
		else if (hit && hit->type == 'A') {
			DEBUG(cerr << "in array case\n";)
			int size1;
			Vb_ATTLC* aptr = (Vb_ATTLC*) ((char*)base + offset);
			DEBUG(cerr << "calling get_integer\n";)
			c = get_integer(c, size1, is);
			DEBUG(cerr << "get_integer returns " << size1 << "\n";);
			if (size1 > aptr->size()) {
				DEBUG(cerr << "resizing array to " << size1 << "\n";)
				aptr->size(size1);
			}
		}
		
		if (c != '\n' && (c=discard(c,is)) == BS_EOF) {
			indent = 0;
			break;
		}
		for (;;) {
			DEBUG(cerr << "at top of inner for loop\n";)
//  Get indent 
			indent=0;
			for( ;; ) {
				c = getchar_G2_ATTLC(is);
				DEBUG(cerr << "is.get() returns c = ASCII " 
					<< c << " (" << char(c)	<< " )"
					<< "\n";)
				DEBUG(void* test = (void*)is;
					if(test){cerr << "is tests non-null\n";}
					else{cerr << "is tests null\n";}
				)
				if (c!='\t') break;
				indent++;
			}
			DEBUG(cerr << "forloop counted " << indent 
				<< " tabs, whereas level is " << level 
				<< "\n";)
			DEBUG(cerr << "c = ASCII " << c << " ("
				<< char(c) << " )" << "\n";)
			if (isname2_ATTLC(c)) {
//  Decisions 
				DEBUG(cerr << "make decisions for indent, level=" 
					<< indent << "," << level << "\n";)
				if (indent == level+1 && hit) {	
//  Recurse  
if (hit->type == 'A') {
}
					DEBUG(cerr << "indent==level+1 && hit\n";)
					DEBUG(cerr << "call getchildren recursively for level " 
						<< level+1 << "\n";)
					c = getchildren(level+1, c, is, (char*)base+offset, hit);
					DEBUG(cerr << "back from getchildren with c = ASCII " 
						<< c << " ("<< char(c) << " )"
						<< "\n";)
					indent = xindent;
					DEBUG(cerr << "set indent=" 
						<< indent << "\n";)
				}
				if (indent==level) {
//  This is a sibling
					DEBUG(cerr << "indent==level\n";)
					DEBUG(cerr << "the next line should say \"again\"\n";)
					break;
				}
				else if (indent<level) {   
//  End of siblings
					DEBUG(cerr << "indent<level\n";)
					DEBUG(cerr << "the next line should say \"out\"\n";)
					quit=1;
					break;
				}
			}

// (indent > level+1)

			DEBUG(cerr << "(fall thru) assert indent > level+1\n";)
			if (indent == 0) {
				DEBUG(cerr << "indent == 0\n";)
				if (c == '\n' || c == '.') {
					DEBUG(cerr << "the next line should say \"out\"\n";)
					quit=1;
					break;
				}
			}
			if ((c=discard(c,is)) == BS_EOF) {
				DEBUG(cerr << "the next line should say \"out\"\n";)
				indent = 0;
				quit=1;
				break;
			}
		}
		if (quit) {
			break;
		}
	}   
	DEBUG(cerr << "out\n";)
	xindent = indent;
	DEBUG(cerr << "ready to return c == ASCII " << c << " ("
		<< char(c) << " )" << "\n";)
	return c;
}

static int
get_integer(register int c, int &return_int, istream& is) {
	return_int = 0;
	while (c && !isdigit_ATTLC(c) && c != '\n') {
		c = getchar_G2_ATTLC(is);
	}
	while (isdigit_ATTLC(c)) {
		return_int = return_int*10 + c - '0';
		c = getchar_G2_ATTLC(is);
	}
	return c;
}
static int 
discard(register int c, istream& is) {
	DEBUG(cerr << "discard ";)
	while(c != '\n' && c != BS_EOF) {
		c = getchar_G2_ATTLC(is);
		DEBUG(cerr << "is.get() returns c = ASCII " 
			<< c << " (" << char(c)	<< " )"	<< "\n";)
		DEBUG(void* test = (void*)is;
			if(test){cerr << "is tests non-null\n";}
			else{cerr << "is tests null\n";}
		)
	}
	DEBUG(cerr << "\n";)
	return c;
}

int 
get_builtin(int code, void* mem, int max, istream& is) {
	char c; 
	DEBUG(cerr << "enter get_builtin with code=" << code << "\n";)

//  Treat STRING_INT_ATTLC as a special case

	if (code==STRING_INT_ATTLC+1) {
		DEBUG(cerr << "STRING\n"; )
		String& x = *((String*)mem);

		if (max) {
			DEBUG(cerr << "fixed-size string\n";)

//  Note: for fixed-size strings, the field definition
//  should add one character to the expected string
//  length.  For example, if strings of size 2048 are
//  expected, specify the field as....
//
//	  usr
//			name   2049
//
//  This is ugly, but necessary to allow for the
//  null byte stored by get.  

			x.uniq();
			x.reserve(max+1);
			DEBUG(cerr << "after reserve, x.max() = " << x.max()
				<< "\n";)

//  Set the length field to max -- we will shrink it
//  later to reflect the actual number of characters read.

#ifdef HP_NATIVE_COMPILER
			x.pad(max,-1);
#else
			x.g2_pad(max,-1);
#endif
			DEBUG(cerr << "after pad, x.length() = "
				<< x.length()<< "\n";)
			char* p = (char*)(const char*)x;
			DEBUG(cerr << "ready to call "	<< "is.get("
				<< (void*)p << "," << max << ","
				<< "\\n)" << "\n";)

//  is.get(p,max,'\n') extracts characters from istream is
//  and stores them in the byte array beginning at p and 
//  extending for max bytes.  Extraction stops when '\n' 
//  is encountered ('\n' is left in is and not stored),
//  when is has no characters, or when the array has only
//  one byte left.  get() always stores a terminating
//  null, even if it doesn't extract any characters from
//  is because of its error state.  ios::failbit is set 
//  only if get() encounters an end-of-file before it
//  stores any characters.

			is.get(p,max+1,'\n');
			DEBUG(void* test = (void*)is;
				if(test){cerr << "is tests non-null\n";}
				else{cerr << "is tests null\n";}
			)

//  The number of characters actually extracted and
//  the "last" character (needed for return value)

			int count = GCOUNT;
#ifndef IOSTREAMH
			if (count > max+1) count = max+1;
#endif
			DEBUG(cerr << "is.gcount() returns " << count << "\n";)
			//int c;

			if (is.fail()) {
				DEBUG(cerr << "is.fail() returns true\n";)
				c = BS_EOF;
			}
			else {
				DEBUG( cerr << "is.fail() returns false\n";)
				c = *(p+count-1);
			}
			DEBUG(cerr << "set c = ASCII " << c
				<< " ("	<< char(c) << ")" << "\n";)

//  Adjust the String length in case the string contains
//  any non-printable characters

			//int size = 0;
			DEBUG(cerr << "contents of memory after get: ";)

			register char *q;
			for (q = p; isprint_ATTLC(*(unsigned char *)q); q++) {
				DEBUG(cerr << char(*q);)
			}
			DEBUG(cerr << "\n";)
			DEBUG(cerr << "ready to shrink length = " 
				<< (q-p) << "\n";)
#ifdef HP_NATIVE_COMPILER
			x.shrink(q-p);
#else
			x.g2_shrink(q-p);
#endif
			DEBUG(cerr << "after shrink, x = " << x
				<< ", x.length() = " << x.length()<< "\n";)

		}
		else {
			DEBUG(cerr << "variable-size string\n";)
//  Note: we can optimize this code since we know max==0
			int size=0;
			for(;;) {
				c = getchar_G2_ATTLC(is);
				DEBUG(cerr << "is.get() returns c = ASCII " 
					<< c << " (" << char(c)	<< " )"
					<< "\n";)
				DEBUG(void* test = (void*)is;
					if(test){cerr << "is tests non-null\n";}
					else{cerr << "is tests null\n";}
				)
	  
				if(!isprint_ATTLC(c) ||	(max!=0 && ++size > max)){
					break;
				}
				DEBUG(cerr << char(c);)
				*((String*)mem) += c;
			}
			DEBUG(cerr << "\n";)
			DEBUG(cerr << "broke from loop with c = ASCII " 
				<< c << " (" << char(c) << " )" << "\n";)
			DEBUG(cerr << "after loop, *" << (void*)mem << "=" 
				<< *(String*)mem << "\n";)
		}
	}
	else {
		String temp;
		DEBUG(cerr << "code!=STRING_INT_ATTLC+1: stuff characters into temp: ";)
		for( ;; ) {
			c = getchar_G2_ATTLC(is);
			DEBUG(cerr << "is.get() returns c = ASCII " 
				<< c << " (" << char(c)
				<< " )"	<< "\n";)
			DEBUG(void* test = (void*)is;
				if(test){cerr << "is tests non-null\n";}
				else{cerr << "is tests null\n";}
			)
			if( !isprint_ATTLC(c) )break;
			DEBUG(cerr << char(c);)
			temp += c;
		}
		DEBUG(cerr << "\n";)
		DEBUG(cerr << "after loop, temp=" << temp << "\n";)

		switch (code) {
		case LONG_INT_ATTLC+1:{
			DEBUG(cerr << "LONG\n";)
			*(long*)mem = atol(temp);
			DEBUG(cerr << "*" << (void*)mem << " = " 
				<< *(long*)mem 	<< "\n";)
			break;
		}
		case SHORT_INT_ATTLC+1:{
			DEBUG(cerr << "SHORT\n";)
			*(short*)mem = atoi(temp);
			DEBUG(cerr << "*" << (void*)mem << " = " 
				<< *(short*)mem << "\n";)
			break;
		}
		case CHAR_INT_ATTLC+1:{
			DEBUG(cerr << "CHAR\n";)
			if ((temp.char_at(0) == '\\') && 
				temp.length()>1) {
				*(char*)mem = _g2otoi_ATTLC(temp.chunk(1));
			}
			else {
				*(char*)mem = temp.char_at(0);
			}
			DEBUG(cerr << "*" << (void*)mem << " = " 
				<< char(*(char*)mem) << "\n";)
			break;
		}
		}
	}
	return c;
}

//  Set the members of a structure
//  to their default initial values
void 
g2clear_ATTLC(G2DESC* rd, void* p){
	DEBUG(cerr << "enter g2clear_ATTLC with rd=\n";)
	DEBUG(showdesc_ATTLC(rd);)
	DEBUG(cerr << "...and p=" << (void*)p << "\n";)
	if (rd->type == 'L') {

//  Record is a leaf

		DEBUG(cerr << "Record is a leaf\n";)
		if (rd->size == 0) {

//  User-defined type

			DEBUG(cerr << "gonna try to clear a user-defined type:\n";)
			rd->nfn(p);

		}
		else {
//  Its type is builtin
			DEBUG(cerr << "Its type is builtin\n";)
			DEBUG(cerr << "Ready to call clearleaf\n";)
			clearleaf(p,rd->size);
		}

	}
	else if (rd->type == 'A') {
//  Record is an array
//  Note: It might make more sense to reset 
//  this to an array of size zero by invoking
//  ((Vb_ATTLC*)p)->size(0).  The scheme used below
//  retains the current size of the array, which
//  may have been increased by a previous call
//  to getrec_ATTLC().  We reason that past data 
//  offer some indication of the future.

		DEBUG(cerr << "Record is an array\n";)
		int nel = ((Vb_ATTLC*)p)->size();
		DEBUG(cerr << "rd->nel=" << rd->nel << "\n"
			<< "nel=" << nel << "\n";)
		int elt_size = REALSIZE(rd->child->size);
		DEBUG(cerr << "rd->child->size=" << rd->child->size
			<< "\n" << "elt_size=" << elt_size << "\n";)
		if (elt_size == 0) {
			elt_size = rd->child->nel;  /* user-defined type */
		}

		int index;
		void* ep;

		// Each element of this array
		G2DESC *child_ptr = rd->child;
		index = 0;
		ep = ((Vb_ATTLC*)p)->beginning();
		
		// optimization -- avoid recursion if we have an array
		// of leaf nodes
		if (child_ptr->type == 'L') {
			if (child_ptr->size == 0) {
				for ( ; index<nel; index++, ep = (char*)ep + elt_size) {
					child_ptr->nfn(ep); // nullify object of user defined type
				}
			}
			else {
				for ( ; index<nel; index++, ep = (char*)ep + elt_size) {
					clearleaf(ep,child_ptr->size); // nullify object of built-in type
				}
			}
		}
		else {
// in this case, we have an array of structures, so we must call
// g2clear() recursively
			for ( ;
				index<nel;
				index++, ep = (char*)ep + elt_size
			){
				DEBUG(cerr << "in for loop\n" << "	index="
					<< index << "\n" << "	ep="
					<< (void*)ep << "\n";)

//  Call g2clear_ATTLC recursively for this element

				DEBUG(cerr << "call g2clear_ATTLC recursively\n";)
				g2clear_ATTLC(rd->child, ep);
				DEBUG(cerr << "for index " << index << ", ep="
					<< (void*)ep << "\n";)
			}
		}
	}
	else {
//  Record is a structure -- recurse
		DEBUG(cerr << "Record is a structure\n";)
		G2DESC* child = rd->child;
		int nel;
		// for each child of this node
		for (nel=rd->nel; nel>0; nel--,child++) {
			DEBUG(cerr << "in loop, consider child:\n";)
			DEBUG(showdesc_ATTLC(child);)
			void* childbase = (char*)p + child->offset;  // TBD_?
			DEBUG(cerr << "compute childbase:\n"
				<< "	p=" << (void*)p
				<< "\n" << "	child->offset="
				<< child->offset << "\n"
				<< "	childbase=" << (void*)childbase
				<< "\n";)
			// optimization -- avoid recursive call to g2clear
			// for leaf nodes
			if (child->type == 'L') {
				if (child->size == 0) {
					DEBUG(cerr << "gonna try to clear a user-defined type:\n";)
					child->nfn(childbase);
				}
				else {
					DEBUG(cerr << "Its type is builtin\n";)
					DEBUG(cerr << "Ready to call clearleaf\n");
					clearleaf(childbase,child->size);
				}
			}
			else {
				DEBUG(cerr << "ready to call g2clear_ATTLC recursively\n";)
				g2clear_ATTLC(child, childbase);
			}
		}
	}
	DEBUG(cerr << "ready to return from g2clear_ATTLC\n";)
}

//  set a leaf node to its default initial value
static void 
clearleaf(void* p, int code) {
	DEBUG(cerr << "enter clearleaf with\n" << "	p="
		<< (void*)p << "	code=" << code << "\n";)

	switch (code) { 
	case LONG_INT_ATTLC+1:{
		DEBUG(cerr << "LONG" << "\n";)
		*((long*)p)=0;
		break;
	}
	case SHORT_INT_ATTLC+1:{
		DEBUG(cerr << "SHORT" << "\n";)
		*((short*)p)=0;
		break;
	}
	case CHAR_INT_ATTLC+1:{
		DEBUG(cerr << "CHAR" << "\n";)
		*((char*)p)=0;
		break;
	}
	case STRING_INT_ATTLC+1:{
		DEBUG(cerr << "STRING\n";)
		*((String*)p)="";
		break;
	}
	default:{
		g2err_ATTLC = G2BADLEAF;
		break;
	}
	}
	DEBUG(cerr << "ready to return from clearleaf\n";)
} 
