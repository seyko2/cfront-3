/*ident	"@(#)cls4:tools/demangler/osrc/elf_dem.h	1.1" */

/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/


/* ELF interface header file for the C++ demangler. */

/*
   The C++ demangler is shared by the ELF library and the C++
   translator.  Because the translator is used on many different
   operating systems, not all of which use ELF, we have set up this
   library so that it may be compiled to work with or without libelf.
   The default is *not* to compile for ELF.  Define the macro "ELF" 
   in the makefile to compile for ELF.
*/

/*
   These macros tack an _elf_..._demangle_ATTLC onto all global names
   which should be hidden outside of the demangler module.  The only
   visible names at this time are the functions "demangle" and 
   "demangle_withlen", which are mapped to "elf_demangle" and 
   "elf_demangle_withlen" for the ELF library.  Note that we use
   "_elf_..._demangle_ATTLC" to hide names even for the translator.  
   We could have used just "_..._demangle_ATTLC" but since the names 
   are hidden why should we bother.
*/

#define	app_String	_elf_app_String_demangle_ATTLC
#define	doarg		_elf_doarg_demangle_ATTLC
#define	doargs		_elf_doargs_demangle_ATTLC
#define	findop		_elf_findop_demangle_ATTLC
#define	free_String	_elf_free_String_demangle_ATTLC
#define	mk_String	_elf_mk_String_demangle_ATTLC
#define	napp_String	_elf_napp_String_demangle_ATTLC
#define	nplist		_elf_nplist_demangle_ATTLC
#define	nprep_String	_elf_nprep_String_demangle_ATTLC
#define	prep_String	_elf_prep_String_demangle_ATTLC
#define	set_String	_elf_set_String_demangle_ATTLC
#define trunc_String	_elf_trunc_String_demangle_ATTLC
#define jbuf		_elf_jbuf_demangle_ATTLC
#define strtol		_elf_strtol_demangle_ATTLC

#if defined(ELF)

#define	demangle		elf_demangle
#define	demangle_withlen	elf_demangle_withlen

/* The ELF library lets the programmer redirect all system services.
   The services used by ELF are storage allocation and file I/O.
   Fortunately, the demangler uses only malloc and free.  Actually, it
   also uses realloc too, but ELF doesn't support it.  So we have to
   do it some other way.
*/

#include "data.h"

#define	malloc		(*_elf_malloc_pointer)
#define	free		(*_elf_free_pointer)

/* Make sure that realloc isn't called inadvertantly.
*/
#define realloc		__can_not_use_realloc_in_elf__

#else /* defined(ELF) */

/* not in BSD
#include <malloc.h>
*/
#ifdef __STDC__
#include <stdlib.h>
#else
extern char *malloc();
#endif

#endif	/* defined(ELF) */

#ifdef __STDC__
#	include <limits.h>
#	define NAME_MAX	INT_MAX
#else
	/* The only requirement on this is that it must be greater
	   than the length of any symbol.
	*/
#	define NAME_MAX	30000
#endif
