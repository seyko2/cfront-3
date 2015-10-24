/*ident	"@(#)ipc:ipcmisc.c	3.1" */
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

#include "ipclib.h"

#define FILNMLEN	14

String
ipc_basename_ATTLC(const String& arg)
{
	int	base = arg.strrchr('/') + 1;
	return arg.chunk(base, arg.length()-base);
}

String
ipc_fix_name_ATTLC(const char* p)
{
	return ipc_fix_name_ATTLC(String(p));
}

String
ipc_fix_name_ATTLC(const String& p)
{
	String ans = p;
	int	name_base = ans.strrchr('/') + 1;
	int	name_len = ans.length() - name_base;
	if (name_len == 0)
		ans(name_base,0) = String("ipcattachment");
	else if (name_len >= FILNMLEN)
		ans(name_base,name_len) = ans(name_base, FILNMLEN-1);
	for ( ; name_base < ans.length(); name_base++)
		if (isspace(ans[(unsigned)name_base]))
			ans[(unsigned)name_base] = '_';
	return ans;
}
