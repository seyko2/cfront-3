/*ident	"@(#) @(#)compiler_type.h	1.1.1.2" */
/******************************************************************************
*
* C++ Standard Library
*
* Copyright (c) 1996  Lucent Technologies.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Lucent Technologies.
* The copyright notice above does not evidence any actual or
* intended publication of such source code.
*
******************************************************************************/

#ifndef COMPILERTYPEEDG
#define COMPILERTYPEEDG

#if defined(__edg_att_40) || defined(__edg_lucent_40) || defined(__edg_lucent_41)

/* for EDG compilers, we will set NAMESPACES_LUCENT
   and EXCEPTION_LUCENT appropriately */

#define COMPILER_IS_EDG_LUCENT
#if defined(__edg_lucent_41)
#define NAMESPACES_LUCENT
#if !defined(NO_EXCEPTION)
#define EXCEPTION_LUCENT
#endif
#endif

#else

/* for non-EDG compilers */

#if defined(_MSC_VER) || defined(__TCPLUSPLUS__)
#define COMPILER_IS_PC_LUCENT
#else
#define COMPILER_IS_CFRONT_COMPATIBLE_LUCENT
#define CFRONT_COMPATIBLE_LUCENT
#endif

#endif

#endif
