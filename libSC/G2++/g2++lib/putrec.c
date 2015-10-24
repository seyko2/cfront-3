/*ident	"@(#)G2++:g2++lib/putrec.c	3.1" */
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
#include <Vblock.h>
#include <stdlib.h>
#include <iostream.h>
#ifdef __GNUG__
#define IOSTREAMH
#endif

static char sccs_id[] = "@(#)G2++ library, version 093096";

#ifdef IOSTREAMH
#define OSWRITE os.write
#else
#define OSWRITE os.put
#endif

static void emit_node(int, void*, G2DESC*, ostream& os);
static void emit_builtin(ostream& os, int, void*, int);

//  Current name hierarchy:

static struct {
	char* name;	       // NULL name implies index 
	int   index;
	int   maxarray;        // if maxarray != 0, it is an array size
} namestk[G2MAXDEPTH_ATTLC];  // TBD_fixed_limit

static int outlevel=0; // depth of name hierarchy 

static int
isnull_leaf(void *vp, G2DESC *rd) {
	int retval;
	if (rd->size == 0) { // user-defined type
		retval = rd->isn(vp);	// run the "is_null" function
					// for the user-defined type
	}
	else { // builtin type
		switch (rd->size) {
		case LONG_INT_ATTLC+1:
			retval = *(long*)vp == 0L;
			break;
		case SHORT_INT_ATTLC+1:
			retval = *(short*)vp == 0;
			break;
		case CHAR_INT_ATTLC+1:
			retval = *(char*)vp == 0;
			break;
		case STRING_INT_ATTLC+1:
		{
			String& temp = *(String*)vp;
			retval = temp.is_empty();
		}
		} /* end switch */
	}
	return (retval);
}

static int
isnull_general(void *base, G2DESC *rd) {
	int retval = 1;
	switch (rd->type) {
	case 'L':
		retval = isnull_leaf(base, rd);
		break;
	case 'S':
	{
		G2DESC *child = rd->child;
		int nel = rd->nel;
		for (; --nel >= 0; child++) {
			if (child->type != 'L') {
				void *vp;
				vp = (char*)base + child->offset;
				if (isnull_general(vp, child) == 0) {
					retval = 0;
					break;
				}
			}
			else {
				void *vp = ((char*)base + child->offset);
				if (!isnull_leaf(vp, child)) {
					retval = 0;
					break;
				}
			}
		}
		break;
	}
	case 'A':
	{
		void* vp = ((Vb_ATTLC*)base)->beginning();
		int nel = ((Vb_ATTLC*)base)->size();
		G2DESC* element = rd->child;
		if (element->type != 'L') {
			int i = 0;
			for (; --nel >= 0; vp = (char*)vp + element->size) {
				if (i >= rd->nel && rd->nel > 0) {
					break;
				}
				if (!isnull_general(vp, element)) {
					retval = 0;
					break;
				}
			}
		}
		else {
			int i = 0;
			for (; --nel >= 0; vp = ((char*)vp + REALSIZE(element->size)),i++) {
				if (i >= rd->nel && rd->nel > 0) {
					break;
				}
				if (!isnull_leaf(vp, element)) {
					retval = 0;
					break;
				}
				if (element->size == 0) {
					vp = (char*)vp + element->nel;
				}
			}
		}
	}
	} /* end switch */
	return (retval);
}
static int getmaxarray(void *base, G2DESC *rd) {
	void* vp = ((Vb_ATTLC*)base)->beginning();
	int nel = ((Vb_ATTLC*)base)->size();
	G2DESC* element = rd->child;
		
	int incr_size;
	if (element->type != 'L') {
		incr_size = element->size;
	}
	else {
		if (element->size == 0) {
			incr_size = element->nel;
		}
		else {
			incr_size = REALSIZE(element->size);
		}
	}
	int i = 0;
	int max_defined = -1;
	for (; --nel >= 0; vp = (char*)vp + incr_size) {
		if (!isnull_general(vp, element)) {
			// os << i << " ";
			if (i > max_defined) {
				max_defined = i;
			}
		}
		i++;
	}
	return (max_defined + 1);
}
	
//  Map a C structure to an output stream
int 
putrec_ATTLC(void* rec, G2DESC* rd, ostream& os) {
	g2err_ATTLC = 0;
	DEBUG(cerr << "\nenter putrec_ATTLC with rd=\n";)
	DEBUG(showdesc_ATTLC(rd);)
	DEBUG(cerr << "rec=" << rec << "\n";)

//  Emit name 

	os << rd->name;		// print the name of the record type
	DEBUG(cerr << "Emit name: " << rd->name << "\n";)
	DEBUG(cerr << "rd->type=" << char(rd->type) << '\n';)

	if (rd->type == 'L') { 
//  Record is a Leaf
		DEBUG(cerr << "Record is a leaf\n";)
		int size = rd->size;
		DEBUG(cerr << "size=" << size << "\n";)

		if (isnull_leaf(rec, rd)) {
			DEBUG(cerr << "null: filter out\n";)
		}
		else {
			if (size == 0) {
				DEBUG(cerr << "emit it by calling pfn\n";)
				rd->pfn(os,rec);
				DEBUG(cerr << "back from call to pfn\n";)
			}
			else {
				DEBUG(cerr << "emit tab\n";)
				os << '\t';
//  Call emit_builtin to emit value
				DEBUG(cerr << "emit value by calling emit_builtin\n";)
				emit_builtin(os, size, rec, rd->nel);
			}
		}
		DEBUG(cerr << "emit newline\n";)
		os << '\n';
	}
	else {
//  Record is a Structure or Array
		DEBUG(cerr << "Record is a Structure or Array\n";)
		if (rd->type == 'A') {
			os << '\t' << getmaxarray(rec, rd);
		}
		DEBUG(cerr << "emit newline\n";)
		os << '\n';
		namestk[0].name = rd->name;
		DEBUG(cerr << "namestk[0].name gets " << rd->name << "\n";)

//  Call emit_node with this record

		DEBUG(cerr << "Call emit_node with this record\n";)
		outlevel = 1;
		// call emit_node with level, base, record descriptor, output
		emit_node(1, rec, rd, os);
	}

//  Calculate checksum and return error flag

	DEBUG(cerr << "Calculate checksum and return error flag\n";)
	if (Cchksum(os)) {
		_g2putdot_ATTLC(os);
	}
	else {
		os << '\n';
	}
	/*Eor(os);*/
	return (Check_error(os)? -1 : 0);
}

//  Map a Structure or Array to the output stream
static void 
emit_node(int level, void* base, G2DESC* rd, ostream& os) {
	DEBUG(cerr << "\nenter emit_node with" << "\n" <<
		"    level=" << level << "\n" <<
		"    base=" << base << "\n";)
	DEBUG(cerr << "rd=\n";)
	DEBUG(showdesc_ATTLC(rd);)

	switch (rd->type) {
	case 'S':
	{
//  This node is a Structure
		DEBUG(cerr << "This node is a Structure\n";)
		G2DESC* child = rd->child;
		int nel = rd->nel;

		// loop through each child of this node
		for (; --nel >= 0; child++) {
			int i;
			DEBUG(cerr << "in loop, consider child:\n";)
			DEBUG(showdesc_ATTLC(child);)
			if (child->type != 'L') {
//  This child is an Array or Structure
				DEBUG(cerr << "This child is an Array "
					<< "or Structure\n";)
				namestk[level].name = child->name;
				DEBUG(cerr << "namestk[" << level 
					<< "].name gets " << child->name
					<< "\n";)
				if (child->type == 'A') {
					namestk[level].maxarray =
					  getmaxarray((char*)base + child->offset, child);
				}
				
//  Calculate vp

				void* vp;
				DEBUG(cerr << "Calculate vp\n";)
				vp = (char*)base + child->offset;
				DEBUG(cerr << "base=" << base << "\n"
					<< "child->offset=" << child->offset
					<< "\n"	<< "vp=" << vp 	<< "\n";)

//  Call emit_node recursively for this child 

				DEBUG(cerr << "Call emit_node recursively\n";)
				emit_node(level+1, vp, child, os);

//  Fiddle with outlevel (?)

				DEBUG(cerr << "Fiddle with outlevel\n";)
				DEBUG(cerr << "return from from emit_node with " 
					<< "outlevel=" << outlevel << "\n";)
				if (outlevel > level) {
					outlevel = level;
					DEBUG(cerr << "adjust outlevel to "
						<< outlevel << "\n";)
				}
			}
			else {

//  This child is a Leaf
//
//  Note: the calculation of vp is common to the then_case
//  and the else_case of this if; factor it out!

				DEBUG(cerr << "The child is a leaf\n";)
				void* vp = ((char*)base + child->offset);
				DEBUG(cerr << "vp=" << vp << "\n";)
//  Filter out nulls
				if (isnull_leaf(vp, child)) {
					DEBUG(cerr << "null: filter out\n";)
					continue;
				}

//  Emit name hierarchy 
				DEBUG(cerr << "Emit name hierarchy\n";)
				// do for each level in outlevel..level-1
				for(; outlevel < level; outlevel++) {
					DEBUG(cerr << "in for loop, outlevel="
						<< outlevel << "\n";)
					i = outlevel;
					DEBUG(cerr << "emit " << i << " tabs\n";)
					while (--i >= 0) {
						os << '\t';
					}
					char* cp = namestk[outlevel].name;
					if (cp) {
//  Name is alphabetic
						DEBUG(cerr << "Name is alphabetic: \n";)
						os << cp;
						DEBUG( cerr << cp << "\n"; )
					}
					else {
//  Name is an index
						DEBUG(cerr << "Name is an index: ";)
						os << namestk[outlevel].index;
						DEBUG(cerr << namestk[outlevel].index << "\n";)
					}
/* new code to put the size of the array in an array record */
					if (namestk[outlevel].maxarray != 0) {
						os << "\t" << namestk[outlevel].maxarray;
						namestk[outlevel].maxarray = 0;
					}
					DEBUG(cerr << "emit newline\n";)
					os << '\n';
				}
//  Emit a name-value pair
				DEBUG(cerr << "ready to emit leaf\n";)
				i = level;

				DEBUG(cerr << "emit " << i << " tabs\n";)
				while (--i >= 0) {
					os << '\t';
				}
				DEBUG(cerr << "emit name: " << child->name << '\n';)
				os << child->name;
				DEBUG(cerr << "emit tab\n";)
				os << '\t';

				if (child->size == 0) {
//  User-defined type (case 3)
					DEBUG(cerr << "User-defined type (case 3)\n";)
					child->pfn(os,vp);
					DEBUG(cerr << "back from call to pfn\n";)
				}
				else {
//  Builtin type
					DEBUG(cerr << "builtin type\n";)
					DEBUG(cerr << "vp=" << vp << "\n";)
//  Call emit_builtin
					emit_builtin(os, child->size, vp, child->nel);
				}
				DEBUG(cerr << "emit newline\n";)
				os << '\n';
			}
		}  
		break;
	}


	case 'A':
	{
//  This node is an Array
		DEBUG(cerr << "This node is an Array\n";)
		void* vp = ((Vb_ATTLC*)base)->beginning();
		DEBUG(cerr << "base=" << base << "\n" << "vp=" << vp << "\n";)
		int nel = ((Vb_ATTLC*)base)->size();
		DEBUG(cerr << "rd->nel=" << rd->nel << "\n" << "nel=" 
			<< nel << "\n";)
		G2DESC* element = rd->child;
		DEBUG(cerr << "element=\n";)
		DEBUG(showdesc_ATTLC(element);)
		
		int incr_size;
		if (element->type != 'L') {
			incr_size = element->size;
		}
		else {
			if (element->size == 0) {
				incr_size = element->nel;
			}
			else {
				incr_size = REALSIZE(element->size);
			}
		}
		int i = 0;
		int max_defined = 0;
		for (; --nel >= 0; vp = (char*)vp + incr_size) {
			if (!isnull_general(vp, element)) {
				// os << i << " ";
				if (i > max_defined) {
					max_defined = i;
				}
			}
			i++;
		}
//		namestk[level].maxarray = max_defined + 1;

		vp = ((Vb_ATTLC*)base)->beginning();
		nel = ((Vb_ATTLC*)base)->size();

		if (element->type != 'L') {
//  Element type is 'A' or 'S
			DEBUG(cerr << "Element type is 'A or 'S'\n";)
			namestk[level].name = NULL;
			DEBUG(cerr << "namestk[" << level
				<< "].name gets NULL\n";)
			int i=0;

			// for each element in the array or structure
			for (; --nel >= 0; vp = (char*)vp + element->size) {
				DEBUG(cerr << "in forloop, " << "vp=" 
					<< vp << "\n";)
//  Truncate fixed arrays if necessary
				if (i>=rd->nel && rd->nel>0) {
					break;
				}
//  Set up namestk[level] for this element
				namestk[level].name = NULL;
				DEBUG(cerr << "namestk[" << level <<
					"].name = NULL\n";)
				DEBUG(cerr << "namestk[" << level <<
					"].index = " << i << "\n";)
				namestk[level].index = i++;
				DEBUG(cerr << "Call emit_node recursively:\n";)
//  Call emit_node recursively for this element
				emit_node(level+1, vp, element, os);
//  Fiddle with outlevel (?)
				DEBUG(cerr << "return from from emit_node with "
					<< "outlevel=" << outlevel << "\n";)
				DEBUG(cerr << "Fiddle with outlevel\n";)
				if (outlevel > level) {
					outlevel = level;
					DEBUG(cerr << "adjust outlevel to "
						<< outlevel << "\n";)
				}
			}
		}
		else {
//  Its element type is a leaf
			DEBUG(cerr << "Its element type is a leaf\n";)
			int i = 0;
			// for each element
			for (; --nel >= 0; vp = ((char*)vp + REALSIZE(element->size)),i++) {
				DEBUG(cerr << "in for loop:\n" << "    nel="
					<< nel << "\n" << "    vp=" 
					<< vp << "\n";)

//  Filter out nulls
				if (isnull_leaf(vp, element)) {
					if (element->size == 0 && element->isn(vp)) {
						vp = (char*)vp + element->nel;
						 /* size of user defined
						    type is in nel */
					}
					continue;
				}
				
//  Emit name hierarchy 
			DEBUG(cerr << "Emit name hierarchy\n";)
			// for each level in outlevel..level-1
			for (; outlevel < level; outlevel++) {
				DEBUG(cerr << "in for loop, outlevel=" 
					<< outlevel << "\n";)
				int j = outlevel;
				DEBUG(cerr << "emit " << j << " tabs\n";)
				while (--j >= 0) {
					os << '\t';
				}
				char* cp = namestk[outlevel].name;
				if (cp) {
//  Name is alphabetic
					DEBUG(cerr << "Name is alphabetic: ";)
					os << cp;
					DEBUG(cerr << cp << "\n";)
				}
				else {
//  Name is an index
					DEBUG(cerr << "Name is an index: ";)
					os << namestk[outlevel].index;
					DEBUG(cerr << namestk[outlevel].index << "\n";)
				}
/* new code to put the size of the array in an array record */
				if (namestk[outlevel].maxarray != 0) {
					os << "\t" << namestk[outlevel].maxarray;
					namestk[outlevel].maxarray = 0;
				}
				DEBUG(cerr << "emit newline\n";)
				os << '\n';
			}
			int j = level;

			DEBUG(cerr << "emit " << j << " tabs\n";)
			while (--j >= 0) {
				os << '\t';
			}
//  Emit builtin
			DEBUG( cerr << "emit builtin: " << i << '\n'; )
			os << i;
			DEBUG(cerr << "emit tab\n";)
			os << '\t';

			if (element->size < 0) {
				emit_builtin(os, element->size, vp, element->nel);
			}
			else if (element->size == 0) {
				element->pfn(os,vp);
				DEBUG(cerr << "back from call to pfn\n";)
				vp = (char*)vp + element->nel;
				 /* size of user defined type is in nel */
			}
			os << '\n';
		}  
	}   
	break;
	} /* end switch */
}
}

//  Emit a value of builtin type
static void 
emit_builtin(ostream& os, int size, void* vp, int max) {
	DEBUG(cerr << "\nenter emit_builtin with vp, size=" << vp 
		<< ", " << size << "\n";)
	switch (size) { 
		char buf[MAXINTSTR_ATTLC];   // TBD_fixed_limit
	case LONG_INT_ATTLC+1:
	{
		DEBUG(cerr << "emit LONG value: " << *((long*)vp) << '\n'; )
		os << *((long*)vp);
		break;
	}
	case SHORT_INT_ATTLC+1:
	{
		DEBUG(cerr << "emit SHORT value: " << *((short*)vp) << '\n'; )
		os << *((short*)vp);
		break;
	}
	case CHAR_INT_ATTLC+1:
	{
		DEBUG(cerr << "CHAR" << '\n';)
		if (isprint_ATTLC(*(unsigned char *)vp)) {
			DEBUG(cerr << "emit printable character: " 
				<< char(*(char*)vp) << "\n";)
			os << (*(char*)vp);
		}
		else {
			DEBUG(cerr << "string begins with unprintable character: \\";)
			os << '\\';
			char* p = _g2ctostr_ATTLC(buf, sizeof(buf), *(unsigned char *)vp);
			while (*p) {
				DEBUG(cerr << *p;)
				os << *p++;
			}
			DEBUG(cerr << "\n";)
		}
		break;
	}
	case STRING_INT_ATTLC+1:
	{
		DEBUG(cerr << "STRING\n";)
		const char* cp = (const char*)(*(String*)vp);
		const char* guard = cp+max;

		const char* p = cp;

		while (*p && (max==0 || p<guard) && isprint_ATTLC(*(unsigned char *)p)) {
			p++;
		}
		OSWRITE(cp,p-cp);
	}
	default:
	{
		g2err_ATTLC = G2BADLEAF;
	}
	break;
	} /* end switch */
}
