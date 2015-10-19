#ident	"@(#)cfront:lib/mk/makefile	1.6"
O_COMPLEX	=	abs.o arg.o cos.o error.o exp.o io.o log.o oper.o \
			polar.o pow.o sin.o sqrt.o
O_GENERIC	=	generic.o
O_NEW		=	_delete.o _handler.o _new.o _vec.o
O_STATIC       =       _ctor.o _dtor.o _main.o dtors.o _stdio.o #exit.o
O_STREAM	=	circbuf.o filebuf.o in.o out.o streambuf.o
O_TASK		=	obj.o qhead.o qtail.o sched.o sim.o \
			swap.o task.o timer.o
OFILES	=	$(O_COMPLEX) $(O_GENERIC) $(O_NEW) \
                       $(O_STATIC) $(O_STREAM) #$(O_TASK)
CC	=	CC
CCFLAGS	= -I../../incl

libC.a:        $(O_COMPLEX) $(O_GENERIC) $(O_NEW) $(O_STATIC) $(O_STREAM) #Task
	ar r libC.a $(OFILES)
	if test "$(BSD)" ; then ranlib libC.a; fi

_stdio.o : _stdio..c
	# gcc -c $< -o $@
	$(CC) -c $< -o $@

Task:
	if vax; then \
		$(CC) $(CCFLAGS) -c -I../task ../task/*.c ../task/vax_swap.s; \
		mv vax_swap.o swap.o ; \
	elif u3b||u3b5||u3b2 ; then \
		$(CC) $(CCFLAGS) -c -I../task ../task/*.c ../task/3b_swap.s; \
		mv 3b_swap.o swap.o ; \
	else echo "task lib not implemented on this machine"; \
	fi

abs.o:	../complex/abs.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/abs.C

arg.o:	../complex/arg.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/arg.C

cos.o:	../complex/cos.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/cos.C

error.o:	../complex/error.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/error.C

exp.o:	../complex/exp.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/exp.C

io.o:	../complex/io.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/io.C

log.o:	../complex/log.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/log.C

oper.o:	../complex/oper.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/oper.C

polar.o:	../complex/polar.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/polar.C

pow.o:	../complex/pow.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/pow.C

sin.o:	../complex/sin.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/sin.C

sqrt.o:	../complex/sqrt.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/sqrt.C

generic.o:	../generic/generic.c
	$(CC) $(CCFLAGS) -I../generic -c ../generic/generic.c
_delete.o:	../new/_delete.C
	$(CC) $(CCFLAGS) -I../new -c ../new/_delete.C

_handler.o:	../new/_handler.C
	$(CC) $(CCFLAGS) -I../new -c ../new/_handler.C

_new.o:	../new/_new.C
	$(CC) $(CCFLAGS) -I../new -c ../new/_new.C

_vec.o:	../new/_vec.C
	$(CC) $(CCFLAGS) -I../new -c ../new/_vec.C

_ctor.o:	../static/_ctor.C
	$(CC) $(CCFLAGS) -I../static -c ../static/_ctor.C

_dtor.o:	../static/_dtor.C
	$(CC) $(CCFLAGS) -I../static -c ../static/_dtor.C

_main.o:	../static/_main.C
	$(CC) $(CCFLAGS) -I../static -c ../static/_main.C

dtors.o:	../static/dtors.C
	$(CC) $(CCFLAGS) -I../static -c ../static/dtors.C

exit.o:	../static/exit.c
	$(CC) $(CCFLAGS) -I../static -c ../static/exit.c

munch.o:	../static/munch.C
	$(CC) $(CCFLAGS) -I../static -c ../static/munch.C

circbuf.o:	../stream/circbuf.C
	$(CC) $(CCFLAGS) -I../stream -c ../stream/circbuf.C

filebuf.o:	../stream/filebuf.C
	$(CC) $(CCFLAGS) -I../stream -c ../stream/filebuf.C

in.o:	../stream/in.C
	$(CC) $(CCFLAGS) -I../stream -c ../stream/in.C

out.o:	../stream/out.C
	$(CC) $(CCFLAGS) -I../stream -c ../stream/out.C

streambuf.o:	../stream/streambuf.C
	$(CC) $(CCFLAGS) -I../stream -c ../stream/streambuf.C

