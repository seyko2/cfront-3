#ident	"@(#)Regex:libx/makefile	3.1" 
###############################################################################
#
# C++ Standard Components, Release 3.0.
#
# Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
# Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
#
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
# Laboratories, Inc.  The copyright notice above does not evidence
# any actual or intended publication of such source code.
#
###############################################################################


CC=cc
CFLAGS=-I. -g
OBS=recomp.o reexec.o resub.o reerror.o

all:	libre.a t

t:	t.o libre.a
	$(CC) -o t t.o libre.a

libre.a: $(OBS)
	ar cr libre.a $(OBS)
	if test -x /usr/bin/ranlib; then ranlib libre.a ; fi

