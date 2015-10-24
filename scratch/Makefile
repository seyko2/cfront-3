#ident	"@(#)cfront:scratch/makefile	1.3"

LIBFILES =	_ctor.o    _delete.o _dtor.o _handler.o \
		_new.o     _vec.o     pure.o \
		placenew.o _arr_map.o \
		_main.o     dtors.o \
		 _stdio.o 		# exit.o 

SRCFILES =	Bits.o   alloc.o   block.o    dcl.o        dcl2.o       dcl3.o     dcl4.o \
		del.o    discrim.o error.o    expand.o     expr.o       expr2.o \
		expr3.o  find.o    hash.o     lalex.o      lex.o        main.o     norm.o \
		norm2.o  print.o   print2.o   repr.o       simpl.o      simpl2.o \
		size.o   table.o   template.o tree_copy.o  tree_walk.o \
		typ.o    typ2.o    y.tab.o    \
		_stdio.o \

#		cfront.cdts.o \

BSD=

CFLAGS	=	-I../incl

all:	cfront libC.a munch
	# mv cfront libC.a munch ..

libC.a:	$(LIBFILES)
	[ -f ../$@ ] && mv ../$@ ../$@.old
	ar r libC.a $(LIBFILES)
	if test "$(BSD)"; then ranlib libC.a; fi
	cp $@ ../$@

munch:		mnch/munch.c
	[ -f ../$@ ] && mv ../$@ ../$@.old
	cc -o munch mnch/munch.c
	cp $@ ../$@

cfront: 	libC.a $(SRCFILES) munch
	[ -f ../$@ ] && mv ../$@ ../$@.old
	../CC -o cfront $(SRCFILES)
	cp $@ ../$@

_main.o:	mnch/_main.c
	cc -c mnch/_main.c

#exit.o:	lib/exit..c
#	cc -c lib/exit..c -o $@

dtors.o:	mnch/dtors.c
	cc -c mnch/dtors.c -o $@

_ctor.o:	lib/_ctor..c
	cc -c lib/_ctor..c -o $@

_dtor.o:	lib/_dtor..c
	cc -c lib/_dtor..c -o $@

_new.o:	lib/_new..c
	cc -c lib/_new..c -o $@

_delete.o:	lib/_delete..c
	cc -c lib/_delete..c -o $@

_handler.o:	lib/_handler..c
	cc -c lib/_handler..c -o $@

_vec.o:	lib/_vec..c
	cc -c lib/_vec..c -o $@

pure.o:	lib/pure..c
	cc -c lib/pure..c -o $@

placenew.o:	lib/placenew..c
	cc -c lib/placenew..c -o $@

_arr_map.o:	lib/_arr_map..c
	cc -c lib/_arr_map..c -o $@


Bits.o:	src/Bits..c
	cc -c src/Bits..c -o $@

alloc.o:	src/alloc..c
	cc -c src/alloc..c -o $@

block.o:	src/block..c
	cc -c src/block..c -o $@

dcl.o:		src/dcl..c
	cc -c src/dcl..c -o $@

dcl2.o:	src/dcl2..c
	cc -c src/dcl2..c -o $@

dcl3.o:	src/dcl3..c
	cc -c src/dcl3..c -o $@

dcl4.o:	src/dcl4..c
	cc -c src/dcl4..c -o $@

del.o:		src/del..c
	cc -c src/del..c -o $@

discrim.o:	src/discrim..c
	cc -c src/discrim..c -o $@

error.o:	src/error..c
	cc -c src/error..c -o $@

expand.o:	src/expand..c
	cc -c src/expand..c -o $@

expr.o:		src/expr..c
	cc -c src/expr..c -o $@

expr2.o:	src/expr2..c
	cc -c src/expr2..c -o $@

expr3.o:	src/expr3..c
	cc -c src/expr3..c -o $@

find.o:		src/find..c
	cc -c src/find..c -o $@

hash.o:		src/hash..c
	cc -c src/hash..c -o $@

lalex.o:	src/lalex..c
	cc -c src/lalex..c -o $@

lex.o:		src/lex..c
	cc -c src/lex..c -o $@

main.o:		src/main..c
	cc -c src/main..c -o $@

norm.o:		src/norm..c
	cc -c src/norm..c -o $@

norm2.o:	src/norm2..c
	cc -c src/norm2..c -o $@

print.o:	src/print..c
	cc -c src/print..c -o $@

print2.o:	src/print2..c
	cc -c src/print2..c -o $@

repr.o:		src/repr..c
	cc -c src/repr..c -o $@

simpl.o:	src/simpl..c
	cc -c src/simpl..c -o $@

simpl2.o:	src/simpl2..c
	cc -c src/simpl2..c -o $@

size.o:		src/size..c
	cc -c src/size..c -o $@

table.o:	src/table..c
	cc -c src/table..c -o $@

template.o:	src/template..c
	cc -c src/template..c -o $@

tree_copy.o:	src/tree_copy..c
	cc -c src/tree_copy..c -o $@

tree_walk.o:	src/tree_walk..c
	cc -c src/tree_walk..c -o $@

typ.o:		src/typ..c
	cc -c src/typ..c -o $@

typ2.o:	src/typ2..c
	cc -c src/typ2..c -o $@

y.tab.o:	src/y.tab..c
	cc -c src/y.tab..c -o $@

_stdio.o:	src/_stdio.c
	cc -c src/_stdio.c -o $@

cfront.cdts.o:	src/cfront.cdts..c
	cc -c src/cfront.cdts..c -o $@

clean:
	-@rm lib/*..c mnch/*..c mnch/*.c src/cfront.cdts..c src/_stdio.c src/*..c
	-@@rm *.o libC.a
	-@rm cfront munch
