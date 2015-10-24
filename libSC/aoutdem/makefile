#ident	"@(#)aoutdem:makefile	3.1" 
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

# Set this to one of {SUNOS, solaris, BSD, SYSV, V9, HPUX, MIPS, svr4}.
OS=SUNOS

# If you chose SYSV, and if you're on a machine that
# prepends an extra initial underscore to all symbol
# table entry names, then set this to 1.
EXTRA__=0

# Change this to 0 if the demangler dumps core when you run it.
# For now, 3B2's are the only machines I know of that should 
# set this to 0.
FAST_NONPORTABLE_STRUCT_ARRAY_READ=1

############################################################################
# You shouldn't have to change anything below this line.
############################################################################
OFILES=demangle.o parse.o under.o
DDS=`if test "${OS}" = solaris -o "${OS}" = HPUX -o "${OS}" = MIPS; then echo "../"; else echo ""; fi`
LIBS=$(DDS)../../lib++.a
LLIBS=$(DDS)../demangle/libmangle.a $(LIBS)

LDFLAGS=$(GFLAG) $(PFLAG)

all:	$(OS)

SUNOS:  demangle

BSD:	demangle

SYSV:	demangle

V9:	demangle

solaris:	force
	cd solaris && $(MAKE) CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"
	cp solaris/demangle .
	rm -f bin/demangle; strip demangle; ln demangle bin

HPUX:	force
	cd hpux && $(MAKE) CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LLIBS)"
	cp hpux/demangle .
	rm -f bin/demangle; strip demangle; ln demangle bin
	-if test ! -f manpage.orig ; then mv demangle.1 manpage.orig 2>/dev/null ; cp -f hpux/hpdemangle.1 . ; fi

MIPS:	force
	cd mips && $(MAKE) CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LLIBS)"
	cp mips/demangle .
	rm -f bin/demangle; strip demangle; ln demangle bin

svr4:	under.o SVR4demangle.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o demangle SVR4demangle.o under.o $(LLIBS) -lelf
	rm -f bin/demangle; strip demangle; ln demangle bin

demangle: $(OFILES)
	$(CC) $(LDFLAGS) $(CCFLAGS) $(CFLAGS) -o demangle $(OFILES) $(LLIBS)
	rm -f bin/demangle; strip demangle; ln demangle bin

.c.o:
	$(CC) -c -D$(OS) $(CCFLAGS) $<

clean:
	cd hpux && $(MAKE) clean	
	rm -f hpdemangle.1
	cd mips && $(MAKE) clean	
	rm -f $(OFILES) SVR4demangle.o
	rm -f demangle bin/demangle
	
clobber: clean
	if test -f manpage.orig ; then mv manpage.orig demangle.1 ; fi
	cd hpux && $(MAKE) clobber
	cd mips && $(MAKE) clobber

tests:	force
	cd tests; $(MAKE) OS=$(OS) CC=$(CC)

force:
