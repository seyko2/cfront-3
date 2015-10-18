#ifndef _SDELICENSE_H
#define _SDELICENSE_H

#ident "@(#)  SID: 1.1.1.2  C module: sdelicense.h  last changed: 5/5/94  14:47:17"

/**********************************************************************
 *
 *	AT&T - PROPRIETARY (RESTRICTED)
 *
 *	SDELIC is fully owned and protected by AT&T Bell Laboratories
 *
 *	Solely for authorized persons having a need to know
 *	pursuant to Company Instructions.
 *
 *	All rights reserved
 *	Copyright (c) 1993 AT&T
 *
 *	AT&T Bell Laboratories
 *	Department BL0591120
 *	AI and OO Technology Group
 *
 *
 * FILE:		sdelicense.h
 *
 * AUTHOR:		Christopher A. Rath
 *
 * CREATED ON:		04/29/93
 *
 * TYPE:		C header
 *
 * CONTENTS:
 *	Cleaned up version of sdelicense that is portable to
 *	architectures other than suns.  Original by Terry Anderson
 */

/* 
 *	Constants for flags
 *
 *	 flags && NOEXIT -> will not exit on error
 *	 flags && NOMSG  -> will not display msgs
 */

#define NOEXIT 1
#define NOMSG 2

/*	
 *	Constants for return codes from sde_check_license()
 *
 *	SDE_EXPIRE_WARNING -> ok today, will expire within time
 *			      specifed by $SDE_LICENSE_WARNING
 *
 *	SDE_ERROR	   -> Can't find license file or can't
 *			      find entry for tool and network
 *	SDE_EXPIRED	   -> license found but has expired
 *
 *	SDE_OKAY	   -> ok today and within expiration warning
 */

#define SDE_EXPIRE_WARNING  1
#define SDE_ERROR	   -1
#define SDE_EXPIRED	   -2
#define SDE_OKAY	    0

/*
 *	Function prototypes
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if (defined(__cplusplus) || defined(__STDC__))
const char *sde_license_msg();
      int   sde_check_license(const char *tool, int tnum, int flags);
#else
      char *sde_license_msg();
      int   sde_check_license();
#endif /* (defined(__cplusplus) || defined(__STDC__)) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SDELICENSE_H */
