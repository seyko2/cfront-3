#ident	"@(#)G2++:makefile	3.4" 
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

CC=CC

#############################################################
# You shouldn't have to change anything below this line.
#############################################################
O=-O
CCFLAGS=$(O)
LDFLAGS=$(O)
LIBS=-l++

all:	libg2++.a g2++comp

libg2++.a:	force
	cd g2++lib; $(MAKE) CC=$(CC) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SYS="$(SYS)"
	rm -f libg2++.a; ln g2++lib/libg2++.a .

g2++comp:	force
	cd compsrc; $(MAKE) CC=$(CC) CCFLAGS="$(CCFLAGS) -I../incl" LDFLAGS="$(LDFLAGS)" LIBS="../g2++lib/libg2++.a $(LIBS)" g2++comp
	rm -f g2++comp; mv compsrc/g2++comp .


demos:	force
	cd g2++lib/demos; $(MAKE) CC=$(CC) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"
	cd compsrc/demos; $(MAKE) CC=$(CC) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"

tests:	force
	cd compsrc/tests; $(MAKE) CC=$(CC) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"

clean:	force
	cd g2++lib; $(MAKE) clean
	cd compsrc; $(MAKE) clean
	rm -f g2++comp libg2++.a

clobber:force
	cd g2++lib; $(MAKE) clobber
	cd compsrc; $(MAKE) clobber
	rm -f g2++comp libg2++.a

force:

