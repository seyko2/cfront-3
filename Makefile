#Makefile for the CC translator
#	BSD 4.2 or later should first run bsd.sed in the scratch directory
#	and set CCFLAGS=-DBSD
#	also set BSD=1

CCFLAGS=-Os
scratchCC ?= gcc

BSD=
PATH:=$(CURDIR):$(PATH)

#For first make (bootstrap):
#	make scratch		#on system V, BSD 4.1 or earlier
#Otherwise:
#	make
#

CC	=	CC

all:	libC.a munch cfront
	:

libC.a:	always
	cd lib/mk; $(MAKE) CC=$(CC) CCFLAGS="$(CCFLAGS)" BSD=$(BSD)
	mv lib/mk/libC.a .

munch:	lib/static/munch.C
	$(CC) $(CCFLAGS) -o munch lib/static/munch.C

cfront: always
	cd src; $(MAKE) CC=$(CC) CCFLAGS="$(CCFLAGS)"
	mv src/cfront cfront
	
scratch: always
	cd scratch; $(MAKE) CC=$(scratchCC) BSD=$(BSD) CCFLAGS="$(CCFLAGS)"

#This target will populate the scratch directories with good-old-c
#files.  This is used to port to another machine.

fillscratch:
#	make sure the directories exist:
	if test ! -d scratch/src; then mkdir -p scratch/src; fi
	if test ! -d scratch/mnch; then mkdir -p scratch/mnch; fi
	if test ! -d scratch/lib; then mkdir -p scratch/lib; fi
#	cd src; yacc gram.y
	cd scratch/src; $(CC) -I../../src         -I../../incl -Fc -..c ../../src/*.C;
	cd scratch/lib; $(CC) -I../../lib/complex -I../../incl -Fc -..c ../../lib/new/*.C
	cd scratch/lib; $(CC) -I../../lib/complex -I../../incl -Fc -..c ../../lib/static/*.C
	rm scratch/lib/munch..c
#Dont need a real munch here:
	echo "main(){ exit(0); }" >scratch/mnch/munch..c
	chmod +x CC patch/CC scratch/bsd.sed
	cp lib/mk/_stdio..c     scratch/lib/_stdio..c
	rm -f scratch/lib/exit..c

always:	
