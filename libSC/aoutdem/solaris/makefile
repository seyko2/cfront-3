CC = CC
CCFLAGS = -Iincl
OFILES = Link.o Memspace.o demangle.o sym.o
LIBS = -l++

all: demangle

demangle: $(OFILES)
	$(CC) -o demangle $(CCFLAGS) $(OFILES) ../../demangle/libmangle.a $(LIBS)

Link.o: incl/Link.h incl/idebug.h

Memspace.o: incl/Link.h incl/Memspace.h incl/idebug.h

demangle.o: incl/sym.h incl/idebug.h

sym.o: incl/Link.h incl/Memspace.h incl/idebug.h incl/util.h

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CCFLAGS) $<

clean:
	rm -f $(OFILES)

clobber: clean
	rm -f demangle
	rm -f *.ii
	rm -rf ptrepository
