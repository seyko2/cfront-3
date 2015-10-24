/*ident	"@(#)fs:libsrc/fslib.h	3.1" */
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

#include <sys/types.h>
#include <stddef.h>

#define NOT_FOUND 0

typedef const void*	KEYTYP;	// generic array address
typedef class fs_descriptor_ATTLC *INFOTYP;

void	_fs_insert_des_ATTLC(KEYTYP key, INFOTYP info);
	// key is a pointer to a newed object.  It must
	//	be non-zero
	//	not already be in table
	// info is the pointer to this object's fs_descriptor_ATTLC

INFOTYP	_fs_remove_des_ATTLC(KEYTYP key);
	// removes a <key,info> pair from the table
	// returns the info, or NOT_FOUND

INFOTYP _fs_get_des_ATTLC(KEYTYP key);
	// returns the info, or NOT_FOUND


void *_fs_register_ATTLC(int atuse, const char *file, int line, const char *type, size_t size, long number, const void *addr);
void _fs_unregister_ATTLC(int atuse, int hasbrackets, const void *addr);
void *_new(size_t size);
void  _delete(void *p);

