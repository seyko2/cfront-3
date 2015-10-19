#ident	"@(#)cfront:scratch/makefile	1.3"
LIBFILES=_ctor..o _delete..o _dtor..o _handler..o \
_main..o _new..o _vec..o dtors..o _stdio..o #exit..o

SRCFILES=alloc..o dcl..o dcl2..o del..o error..o \
expand..o expr..o expr2..o lalex..o lex..o \
main..o norm..o norm2..o print..o repr..o \
simpl..o size..o table..o typ..o typ2..o \
y.tab..o

BSD=

all:	cfront libC.a munch
	mv cfront libC.a munch ..

libC.a:	$(LIBFILES)
	ar rv libC.a $(LIBFILES)
	if test "$(BSD)"; then ranlib libC.a; fi

munch:	mnch/munch..c libC.a
	cc -o munch mnch/munch..c libC.a

cfront: libC.a $(SRCFILES)
	cc -o cfront $(SRCFILES) libC.a

_stdio..o:	lib/_stdio..c
	cc -c lib/_stdio..c

_ctor..o:	lib/_ctor..c
	cc -c lib/_ctor..c

_delete..o:	lib/_delete..c
	cc -c lib/_delete..c

_dtor..o:	lib/_dtor..c
	cc -c lib/_dtor..c

_handler..o:	lib/_handler..c
	cc -c lib/_handler..c

_main..o:	lib/_main..c
	cc -c lib/_main..c

_new..o:	lib/_new..c
	cc -c lib/_new..c

_vec..o:	lib/_vec..c
	cc -c lib/_vec..c

dtors..o:	lib/dtors..c
	cc -c lib/dtors..c

exit..o:	lib/exit..c
	cc -c lib/exit..c

alloc..o:	src/alloc..c
	cc -c src/alloc..c

dcl..o:	src/dcl..c
	cc -c src/dcl..c

dcl2..o:	src/dcl2..c
	cc -c src/dcl2..c

del..o:	src/del..c
	cc -c src/del..c

error..o:	src/error..c
	cc -c src/error..c

expand..o:	src/expand..c
	cc -c src/expand..c

expr..o:	src/expr..c
	cc -c src/expr..c

expr2..o:	src/expr2..c
	cc -c src/expr2..c

lalex..o:	src/lalex..c
	cc -c src/lalex..c

lex..o:	src/lex..c
	cc -c src/lex..c

main..o:	src/main..c
	cc -c src/main..c

norm..o:	src/norm..c
	cc -c src/norm..c

norm2..o:	src/norm2..c
	cc -c src/norm2..c

print..o:	src/print..c
	cc -c src/print..c

repr..o:	src/repr..c
	cc -c src/repr..c

simpl..o:	src/simpl..c
	cc -c src/simpl..c

size..o:	src/size..c
	cc -c src/size..c

table..o:	src/table..c
	cc -c src/table..c

typ..o:	src/typ..c
	cc -c src/typ..c

typ2..o:	src/typ2..c
	cc -c src/typ2..c

y.tab..o:	src/y.tab..c
	cc -c src/y.tab..c
