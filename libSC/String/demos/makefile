#ident	"@(#)String:demos/makefile	3.1" 
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

include $(SLELIB)/demorules

all:
	$(SHELL) $(SLEBIN)/regress 

replace.r:	replace.E
	echo "foo bar foobar foobarbaz" | ./replace.E bar BAR >replace.r

incore.r:	incore.E
	./incore.E <places >incore.r


clean:
	rm -rf *.ii *.o *.r *.E core ptrepository .variant
