CC=../../CC
LIBS=
CCFLAGS=-DNO_EXCEPTION -I../../incl
#ISTL=--stl -I../../stl
G=
LDFLAGS=$(G)
SHELL=sh

.SUFFIXES: .E
 
.C.E:
	$(CC) $(CCFLAGS) $(ISTL) $(LDFLAGS) -o $*.E $< $(LIBS)

.C.o:
	$(CC) $(CCFLAGS) $(ISTL) -c $<

all:
	$(SHELL) ./regress

clean:
	-@rm -f *.o *.E *.r *.ii core *..c
	-@rm -rf ptrepository
