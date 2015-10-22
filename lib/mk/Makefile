#ident	"@(#)cls4:lib/mk/makefile	1.3"
###############################################################################
#
# C++ source for the C++ Language System, Release 3.0.  This product
# is a new release of the original cfront developed in the computer
# science research center of AT&T Bell Laboratories.
#
# Copyright (c) 1993  UNIX System Laboratories, Inc.
# Copyright (c) 1991, 1992   AT&T and UNIX System Laboratories, Inc.
# Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.
#
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
#	
# Laboratories, Inc.  The copyright notice above does not evidence
# any actual or intended publication of such source code.
#
###############################################################################

O_GENERIC	=	generic.o
O_NEW		=	_new.o _delete.o _handler.o _vec.o placenew.o _arr_map.o
O_STATIC	=	_ctor.o _dtor.o _main.o dtors.o pure.o
O_STREAM	=	in.o out.o stream.o fstream.o manip.o cstreams.o \
flt.o strstream.o rawin.o intin.o stdiobuf.o streambuf.o \
filebuf.o  oldformat.o sbuf.dbp.o swstdio.o ios_compat.o _stdio.o 	# exit.o 

OFILES	=	$(O_GENERIC) $(O_NEW) $(O_STATIC) $(O_STREAM) 
CC	=	CC
CCFLAGS	=-O -I../../incl

# have everything depend on this makefile to force recompilation
# when going from patch to munch
DEPEND	= Makefile

#MPDIR	= _patch
MPDIR	= _munch

libC.a:	$(OFILES)
	ar r libC.a $(OFILES)

_stdio.o: _stdio.c
	cc -c _stdio.c

generic.o:	../generic/generic.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../generic/generic.C

_delete.o:	../new/_delete.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../new/_delete.C

_handler.o:	../new/_handler.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../new/_handler.C

_new.o:	../new/_new.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../new/_new.C

_vec.o:	../new/_vec.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../new/_vec.C

placenew.o: ../new/placenew.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../new/placenew.C

_arr_map.o: ../new/_arr_map.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../new/_arr_map.C

_ctor.o:	../static/_ctor.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../static/_ctor.C

_dtor.o:	../static/_dtor.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../static/_dtor.C

_main.o:	../../$(MPDIR)/_main.c $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../../$(MPDIR)/_main.c

dtors.o:	../../$(MPDIR)/dtors.c $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../../$(MPDIR)/dtors.c

exit.o:	../static/exit.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../static/exit.C

pure.o: ../static/pure.C $(DEPEND)
	$(CC) $(CCFLAGS)  -c ../static/pure.C

in.o:	../stream/in.C  $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/in.C

out.o:	../stream/out.C  ../stream/streamdefs.h $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/out.C

stream.o:	../stream/stream.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/stream.C

fstream.o:	../stream/fstream.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/fstream.C

manip.o:	../stream/manip.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/manip.C

cstreams.o:	../stream/cstreams.C ../stream/streamdefs.h $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/cstreams.C

flt.o:	../stream/flt.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/flt.C

strstream.o:	../stream/strstream.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/strstream.C

rawin.o:	../stream/rawin.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/rawin.C

intin.o:	../stream/intin.C  $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/intin.C

stdiobuf.o:	../stream/stdiobuf.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/stdiobuf.C

streambuf.o:	../stream/streambuf.C ../stream/streamdefs.h $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/streambuf.C

filebuf.o:	../stream/filebuf.C ../stream/streamdefs.h $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/filebuf.C

oldformat.o:	../stream/oldformat.C ../stream/streamdefs.h $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/oldformat.C

sbuf.dbp.o:	../stream/sbuf.dbp.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/sbuf.dbp.C

swstdio.o:	../stream/swstdio.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/swstdio.C

ios_compat.o:	../stream/ios_compat.C $(DEPEND)
	$(CC) $(CCFLAGS) -I../stream -c ../stream/ios_compat.C
