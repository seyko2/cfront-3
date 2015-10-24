/*ident	"@(#)fs:libsrc/reg.c	3.1" */
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

/* Include fs.h to make sure the prototypes it contains match 
*  the definitions in this file.
*/
#include <fs.h>

#include "fslib.h"
#include <assert.h>
#include <stdlib.h>
#include <new.h>
#include <malloc.h>
#include <stdio.h>


/* THE CODE IN THIS FILE CANNOT (and currently does not) RELY ON STATIC 
*  CONSTRUCTORS HAVING ALREADY BEEN CALLED.  That is, ALL THE CODE IN 
*  THIS FILE MUST BE (and is, as it is currently written) READY TO EXECUTE 
*  UPON PROGRAM LOAD.  This is to enable the freestore manager to manage 
*  objects that are newed during static construction.
*/

/* Every newed object gets a number, in sequence.  
*  Counter is the number of the *next* object to be newed.
*/
static int counter = 0;

/* fs_descriptor_ATTLC for an object on the free store.
*/
class fs_descriptor_ATTLC {
public:
	// don't call program's operator new, it will confuse user
	void *operator new(size_t) { return malloc(sizeof(fs_descriptor_ATTLC)); }
	void operator delete(void *p) { if (p) free((char*)p); }

	const char *type;	// Type of object.
	long number;	// Number of objects.  
			//	Possible values: 
			//	-2: no idea
			//	-1: it's an array, but couldn't figure out how many
			//	>=0: that many
			//
	size_t size;	// Sizeof (single) object.
	const void *addr;	// Memory address of object.
	const char *file;	// File and line number newed on.
	int line;	//
	long onum;	// Object number.  Onums steadily increase and are never reused.
	int atuse;	// After registering, this is true if we registered at use of new; 
			// after unregistering, this is true if we unregistered at use of new.
	int watched;	// Is there a delete-watch on this object?
			//
	fs_descriptor_ATTLC *prev, *next;  // Previous and next fs_descriptor_ATTLC in implicit container of all fs_descriptor_ATTLCs.

	void print(FILE *f);
};

void fs_descriptor_ATTLC::print(FILE *f)
{
	fprintf(f, "#%d", onum);
	if (watched)
		fprintf(f, " (watched)");
	fprintf(f, ": %s", type);
	if (number == -1)  // it's an array, but don't know how many
		fprintf(f, "[]");
	else if (number > 1)
		fprintf(f, "[%d]", number);
	if (line >= 0)
		fprintf(f, ", newed on line `%s`%d", file, line);
	fprintf(f, " (size %d at %lx)", size, addr);
	if (number == -2)
		fprintf(f, ", alert!");
	fprintf(f, "\n");
}

/* First and last fs_descriptor_ATTLC in implicit container of all fs_descriptor_ATTLCs.
*/
static fs_descriptor_ATTLC *first = 0;
static fs_descriptor_ATTLC *last = 0;

/* Are all objects being delete-watched?
*  Is the freestore being new-watched?
*/
static int dwatch = 0;
static int nwatch = 0;


/* The mapping from freestore locations to fs_descriptor_ATTLCs is implemented
*  by the following functions:
*	_fs_insert_des_ATTLC
*	_fs_remove_des_ATTLC
*	_fs_get_des_ATTLC
*/

/* The variable regAndUnreg determines whether newed objects get registered
*  and deleted objects get deregistered.
*
*  Every object that's newed gets a fs_descriptor_ATTLC, and every object that's deleted
*  loses its fs_descriptor_ATTLC.  The exceptions are the objects newed by the freestore
*  manager itself.  Two reasons for not fs_descriptor_ATTLCing these objects: (1) the user 
*  doesn't want to see them, and (2) if we fs_descriptor_ATTLCed them, there'd be an 
*  infinite regression of fs_descriptor_ATTLCs.
*/

static int regAndUnreg = 1;

static int verbose = 1;
extern void fs_verbose()
{
	verbose = 1;
}
extern void fs_terse()
{
	verbose = 0;
}


/* The user is supposed to set a breakpoint on this function at the beginning of debugging.
*  The breakpoint will have to be reset whenever the program is reloaded into the debugger.
*/
extern void fs_break()
{
}

static void setDes(fs_descriptor_ATTLC *dp, int atuse, const char *file, int line, const char *type, 
	size_t size, const void *p, long onum, int watched, long number)
{
	dp->file = file;
	dp->line = line;
	dp->type = type;
	dp->size = size;
	dp->addr = p;
	dp->onum = onum;
	dp->watched = watched;
	dp->atuse = atuse;
	dp->number = number;
}

static void addToImplicitContainer(fs_descriptor_ATTLC *dp)
{
	dp->next = 0;
	dp->prev = last;
	if (first == 0)	first = dp;
	else		last->next = dp;
	last = dp;
}

static void delFromImplicitContainer(fs_descriptor_ATTLC *dp)
{
	if (dp->prev)	dp->prev->next = dp->next;
	else		first = dp->next;
	if (dp->next) 	dp->next->prev = dp->prev;
	else		last = dp->prev;
}


static void checkForSizeProblems(long defnumber, size_t defsize, long usenumber, size_t usesize)
{
	if (usenumber >= 0 /*&& usesize >= 0*/ && defsize != usesize * usenumber)
	{
		if (usenumber == 1)
		{
			fprintf(stderr, "freestore: Something appears very wrong to me.  The size of an object\n");
			fprintf(stderr, "just created doesn't match the size of its type.\n");
		}
		else
		{
			fprintf(stderr, "freestore: Something appears very wrong to me.  The size of an array\n");
			fprintf(stderr, "of objects just created doesn't match the size of its type times\n");
			fprintf(stderr, "the number of elements in the array.\n");
		}
		fs_break();
	}
	if (defnumber >= 0 && defnumber != usenumber)
	{
		fprintf(stderr, "freestore: Something appears very wrong to me.  The number of objects\n");
		fprintf(stderr, "just created doesn't match the number requested.\n");
		fs_break();
	}
}


/* The following function returns true iff it looks like we just a moment 
*  ago registered an object at the definition of operator new, and now 
*  we're trying to register it again at the corresponding use of new.  
*  dp is the fs_descriptor_ATTLC that was created at the definition registration.
*
*  The algorithm I use to decide this condition isn't perfect.  In fact,
*  no algorithm can be, since the following sequence of events is possible,
*  but not detectable:
*
*	1. Object O1 is newed, and registered at the definition of new.
*	2. O1 is not registered at the use of new.
*	3. O1 is deleted, but not unregistered (either at the use or the
*	   definition of delete).
*	4. Object O2 is newed, but not registered at the definition of 
*	   new.  Further, the memory location of O2 is the same as O1.
*	5. Object O2 is registered at the use of new.
*
* In this sequence of events, there is no way to tell that O1 and O2 are 
* different objects.  My algorithm, like all others, will think they're
* the same object, that is, that step 5 is simply the use-reg of the object
* that was def-reg'd in step 1.  
*/
static int useregOfSameObject(
	fs_descriptor_ATTLC *	dp, 
	int 		atuse, 
	size_t 		size, 
	long 		number, 
	const void *	p)
{
	/* This is how I test the condition, fallible though it may be.
	*/
	int cond = ((!dp->atuse && atuse) 	// if we were at a def, and now we're at a use,
		&& (dp->addr == p));		// and if the "two" objects are at the same location,
						// THEN it's high probability that we're now use-reg'ing 
						// the object we already def-reg'd.

	if (cond) 
		checkForSizeProblems(dp->number, dp->size, number, size);
	return cond;
}


static void checkForNBreak(fs_descriptor_ATTLC *)
{
	if (nwatch)
	{
		fprintf(stderr, "freestore: An object has been allocated, and is about to be initialized.\n");
		fs_break();
	}
}


static void do_registerConsultingfs_descriptor_ATTLC(fs_descriptor_ATTLC *dp, int atuse, const char *file, 
	int line, const char *type, size_t size, long number, const void *p)
{
	if (useregOfSameObject(dp, atuse, size, number, p))
	{
		/* remember that the type specified in the use of new should (and 
		* does) override the type specified in the definition of new, 
		* since we might have used an inherited definition of new.
		*/
		setDes(dp, atuse, file, line, type, size, p, dp->onum, dp->watched, number);
	}
	else
	{
//		if (dp->watched || dwatch || verbose)
		{
			fprintf(stderr, "freestore: Something is fishy.  You are about to construct\n");
			fprintf(stderr, "an object into the memory location that I believe object #%d\n", dp->onum);
			fprintf(stderr, "is currently occupying.  Either I didn't detect the deletion\n");
			fprintf(stderr, "of object #%d (which would happen only if both the use and\n", dp->onum);
			fprintf(stderr, "the def of the instance of operator delete which deleted \n");
			fprintf(stderr, "object #%d were not compiled with -fs), or you really did\n", dp->onum);
			fprintf(stderr, "forget to destroy object #%d.  I will assume the former.\n", dp->onum);
			fs_break();
		}

		/* to keep the container sorted by onum
		*/
		delFromImplicitContainer(dp);
		addToImplicitContainer(dp);

		setDes(dp, atuse, file, line, type, size, p, counter++, 0, number);
		checkForNBreak(dp);
	}
}


static void do_register(int atuse, const char *file, int line, const char *type, size_t size, long number, const void *p)
{
	fs_descriptor_ATTLC *dp;
	if ((dp = _fs_get_des_ATTLC(p)) != NOT_FOUND)
		do_registerConsultingfs_descriptor_ATTLC(dp, atuse, file, line, type, size, number, p);
	else
	{
		dp = new fs_descriptor_ATTLC;
		setDes(dp, atuse, file, line, type, size, p, counter++, 0, number);
		addToImplicitContainer(dp);
		_fs_insert_des_ATTLC(p, dp);
		checkForNBreak(dp);
	}
}


/* Registration and deregistration routines used by all instrumented 
*  uses and defs of all news and deletes (not just the default global ones above).
*/
extern void *_fs_register_ATTLC(int atuse, const char *file, int line, const char *type, size_t size, long number, const void *p)
{
	if (regAndUnreg)
	{
		regAndUnreg = 0;
		do_register(atuse, file, line, type, size, number, p);
		regAndUnreg = 1;
	}
	return (void*)p;
}


/* Check to see if we should break before deleting this object.
*/
static void checkForDBreak(fs_descriptor_ATTLC *dp)
{
	if (dp->watched || dwatch) 
	{
		fprintf(stderr, "freestore: Object #%d is about to be deleted.\n", dp->onum);
		fs_break();				
	}
}


static void do_unregister(int atuse, const void *p)
{	
	fs_descriptor_ATTLC *dp;
	if ((dp = _fs_remove_des_ATTLC(p)) != NOT_FOUND)
	{
		dp->atuse = atuse;
		checkForDBreak(dp);
		delFromImplicitContainer(dp);
		delete dp;
	}
	// else no fs_descriptor_ATTLC for this object.
/*
*	It's tough to tell if we are attempting a definition unregistration
*	of an object we just use unregistered.  E.g., the following sequence 
*	of events is possible:
*	
*		unreg x at use of delete
*		call delete x
*		first destroy x
*		delete (and thus unreg) an instance variable of x
*		unreg x at definition of delete
*		really delete x
*
*	So I decided just not to inform the user when she's about to delete 
*	an object I didn't know about.
*/
}

extern void _fs_unregister_ATTLC(int atuse, int /*hasbrackets*/, const void *p)
{
	if (regAndUnreg && p)
	{
		regAndUnreg = 0;
		do_unregister(atuse, p);
		regAndUnreg = 1;
	}
}


extern int fs_showall()
{
	int sv = regAndUnreg;
	regAndUnreg = 0;
	int i = 0;
	for (fs_descriptor_ATTLC *dp = first; dp; dp = dp->next, i++)
		dp->print(stderr);
	regAndUnreg = sv;
	return i;
}

struct Mark
{
	int time;
	struct Mark *next, *prev;

	// don't call program's operator new, it will confuse user
	void *operator new(size_t) { return malloc(sizeof(Mark)); }
	void operator delete(void *p) { if (p) free((char*)p); }
};

static Mark *firstMark = 0, *lastMark = 0;

static void pushMark(int t)
{
	int sv = regAndUnreg;
	regAndUnreg = 0;

	if (firstMark == 0)
	{
		firstMark = lastMark = new Mark;
		lastMark->prev = 0;
	}
	else
	{
		lastMark->next = new Mark;
		lastMark->next->prev = lastMark;
		lastMark = lastMark->next;
	}
	lastMark->time = t;
	lastMark->next = 0;

	regAndUnreg = sv;
}
	
static void popMark()
{
	int sv = regAndUnreg;
	regAndUnreg = 0;

	lastMark = lastMark->prev;
	if (lastMark == 0)
	{
		delete firstMark;
		firstMark = 0;
	}
	else
	{
		delete lastMark->next;
		lastMark->next = 0;
	}

	regAndUnreg = sv;
}

extern int fs_mark()
{
	if (lastMark == 0 || (lastMark->time != counter-1))  // don't push multiple marks for the same time
	{
		pushMark(counter-1);
	}
	return counter-1;
}

extern void fs_unmark()
{
	if (lastMark != 0)
		popMark();
}

extern int fs_sincen(int t)
{
	int sv = regAndUnreg;
	regAndUnreg = 0;

	fs_descriptor_ATTLC *dp = 0;

	// First get to the right spot in the container.
	if (last && t < last->onum)
		for (dp = last; dp->prev && t < dp->prev->onum; dp = dp->prev)
			;

	// Now show 'em.
	int i;
	for (i = 0; dp; dp = dp->next, i++)
		dp->print(stderr);

	regAndUnreg = sv;
	return i;
}

extern int fs_since()
{
	if (lastMark == 0)
		return fs_sincen(-1);
	else
		return fs_sincen(lastMark->time);
}

static void showmarks()
{
	int sv = regAndUnreg;
	regAndUnreg = 0;

 	if (firstMark != 0)
	{
		if (firstMark->next != 0)	
			fprintf(stderr, "Marks after: ");
		else	
			fprintf(stderr, "Mark after: ");
		for (Mark *m = firstMark; m != 0; m = m->next)
		{
			fprintf(stderr, "%d", m->time);
			if (m->next)
				fprintf(stderr, ", ");
		}
		fprintf(stderr, "\n");
	}
	regAndUnreg = sv;
}


static int showwatches()
{
	int i = 0;
	for (fs_descriptor_ATTLC *dp = first; dp; dp = dp->next)
	{
		if (dp->watched)
		{
			if (i++ == 0)
				fprintf(stderr, "Delete watches on: ");
			else
				fprintf(stderr, ", ");
			fprintf(stderr, "#%d (%lx)", dp->onum, dp->addr);
		}
	}
	if (i > 0)
		fprintf(stderr, "\n");
	return i;
}

extern void fs_status()
{
	int sv = regAndUnreg;
	regAndUnreg = 0;

	if (!verbose) 
		fprintf(stderr, "Terse.\n");
	if (nwatch) 
		fprintf(stderr, "Watching for news.\n");
	if (dwatch) 
		fprintf(stderr, "Watching for deletes.\n");
	showwatches();
	showmarks();

	regAndUnreg = sv;
}

extern int fs_show(void *p)
{
	int sv = regAndUnreg;
	regAndUnreg = 0;

	fs_descriptor_ATTLC *dp = _fs_get_des_ATTLC(p);
	if (dp != NOT_FOUND)
		dp->print(stderr);
	else
		fprintf(stderr, "No object that I'm aware of at %lx\n", p);

	regAndUnreg = sv;
	return (dp != NOT_FOUND);
}

static int searchFromBeginning(int onum, int w)
{
	for (fs_descriptor_ATTLC *dp = first; dp; dp = dp->next)
	{
		if (dp->onum == onum) 
		{
			dp->watched = w;
			return 1;
		}
		else if (onum < dp->onum)
		{
			fprintf(stderr, "Object #%d has already been deleted.\n", onum);
			return 0;
		}
	}
	return 0;
}

static int searchFromEnd(int onum, int w)
{
	for (fs_descriptor_ATTLC *dp = last; dp; dp = dp->prev)
	{
		if (dp->onum == onum) 
		{
			dp->watched = w;
			return 1;
		}
		else if (onum > dp->onum)
		{
			fprintf(stderr, "Object #%d has already been deleted.\n", onum);
			return 0;
		}
	}
	return 0;
}

static int _fsowatch(int onum, int w)
{
	int sv = regAndUnreg;
	regAndUnreg = 0;

	int ret = 0;
	if (last == 0 || onum > last->onum || onum < first->onum)
	{
		fprintf(stderr, "No object #%d.\n", onum);
	}
	else
	{
		/* find the fs_descriptor_ATTLC for object #onum.  
		* search from closest end of container.
		*/
		if (onum < (last->onum + first->onum)/2)  
			ret = searchFromBeginning(onum, w);
		else
			ret = searchFromEnd(onum, w);
	}

	regAndUnreg = sv;
	return ret;
}

extern void fs_watchall_d()
{
	dwatch = 1;
}
extern void fs_unwatchall_d()
{
	dwatch = 0;
}
extern void fs_watchall_n()
{
	nwatch = 1;
}
extern void fs_unwatchall_n()
{
	nwatch = 0;
}
extern int fs_watch(int onum)
{
	return _fsowatch(onum, 1);
}
extern int fs_unwatch(int onum)
{
	return _fsowatch(onum, 0);
}
extern void fs_help()
{
	fprintf(stderr, "fs_showall() - show information for all objects currently in the freestore\n");
	fprintf(stderr, "fs_show(void *p) - show information for object at memory location p\n");
	fprintf(stderr, "fs_mark() - lay down a mark at the current time\n");
	fprintf(stderr, "fs_unmark() - delete the last mark\n");
	fprintf(stderr, "fs_since() - show information for all objects newed since the last mark\n");
	fprintf(stderr, "fs_sincen(int i) - show information for all objects newed since object number i\n");
	fprintf(stderr, "fs_watch(int i) - set a watch on object number i\n");
	fprintf(stderr, "fs_unwatch(int i) - delete the watch on object number i\n");
	fprintf(stderr, "fs_watchall_d() - watch the freestore for any delete operation\n");
	fprintf(stderr, "fs_unwatchall_d() - stop watching the freestore for delete operations\n");
	fprintf(stderr, "fs_watchall_n() - watch the freestore for any new operation\n");
	fprintf(stderr, "fs_unwatchall_n() - stop watching the freestore for new operations\n");
	fprintf(stderr, "fs_break() - function with empty body, called whenever freestore wants to break\n");
	fprintf(stderr, "fs_status() - show the current settings of marks and watches\n");
	fprintf(stderr, "fs_dbxinit() - print a .dbxinit file which incorporates freestore's functionality\n");
	fprintf(stderr, "              into dbxtool(1)'s button panel\n");
	fprintf(stderr, "fs_help() - this list\n\n");
	fprintf(stderr, "The \"alert!\" modifier on object descriptions means that the object was allocated\n");
	fprintf(stderr, "by a global operator new and is either in the process of being initialized, or\n");
	fprintf(stderr, "is initialized but the line containing the call to new was not compiled with -fs.\n");
	fprintf(stderr, "\n");
}
extern void fs_dbxinit()
{
	fprintf(stderr, "unbutton print\n");
	fprintf(stderr, "unbutton next\n");
	fprintf(stderr, "unbutton step\n");
	fprintf(stderr, "unbutton stop at\n");
	fprintf(stderr, "unbutton stop in\n");
	fprintf(stderr, "unbutton clear\n");
	fprintf(stderr, "unbutton cont\n");
	fprintf(stderr, "unbutton run\n");
	fprintf(stderr, "unbutton where\n");
	fprintf(stderr, "unbutton up\n");
	fprintf(stderr, "unbutton down\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "#button expand print *\n");
	fprintf(stderr, "button ignore next \n");
	fprintf(stderr, "button ignore up\n");
	fprintf(stderr, "button ignore where\n");
	fprintf(stderr, "button lineno stop at\n");
	fprintf(stderr, "button ignore fs\n");
	fprintf(stderr, "button ignore mark\n");
	fprintf(stderr, "button ignore wd\n");
	fprintf(stderr, "button ignore wn\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "button expand print\n");
	fprintf(stderr, "button ignore step\n");
	fprintf(stderr, "button ignore down\n");
	fprintf(stderr, "button ignore dump\n");
	fprintf(stderr, "button lineno clear\n");
	fprintf(stderr, "button ignore since\n");
	fprintf(stderr, "button ignore unmark\n");
	fprintf(stderr, "button ignore unwd\n");
	fprintf(stderr, "button ignore unwn\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "button expand display \n");
	fprintf(stderr, "button ignore cont\n");
	fprintf(stderr, "button ignore \"\"\n");
	fprintf(stderr, "button ignore \"\"\n");
	fprintf(stderr, "button ignore run\n");
	fprintf(stderr, "button ignore \"\"\n");
	fprintf(stderr, "button ignore \"\"\n");
	fprintf(stderr, "button ignore fsstat\n");
	fprintf(stderr, "button ignore setfsbr\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "alias mark \"call fs_mark()\"\n");
	fprintf(stderr, "alias unmark \"call fs_unmark()\"\n");
	fprintf(stderr, "alias fs \"call fs_showall()\"\n");
	fprintf(stderr, "alias since \"call fs_since()\"\n");
	fprintf(stderr, "alias fsstat \"call fs_status()\"\n");
	fprintf(stderr, "alias wd \"call fs_watchall_d()\"\n");
	fprintf(stderr, "alias unwd \"call fs_unwatchall_d()\"\n");
	fprintf(stderr, "alias wn \"call fs_watchall_n()\"\n");
	fprintf(stderr, "alias unwn \"call fs_unwatchall_n()\"\n");
	fprintf(stderr, "alias setfsbr \"stop in fs_break\"");
	fprintf(stderr, "\n");
}

/*
static void showem()
{
	for (fs_descriptor_ATTLC *dp = first; dp; dp = dp->next)
		fprintf(stderr, "%lx\n", dp->addr);
}
*/
