CXX	?=	g++
CCFLAGS	=	-I../incl

#MPMACRO = PATCH
MPMACRO = MUNCH

OFILES	=	Bits.o alloc.o block.o dcl.o dcl2.o dcl3.o dcl4.o \
		del.o discrim.o error.o expand.o expr.o expr2.o \
		expr3.o find.o hash.o lalex.o lex.o main.o norm.o \
		norm2.o print.o print2.o repr.o simpl.o simpl2.o \
		size.o table.o template.o tree_copy.o tree_walk.o \
		typ.o typ2.o y.tab.o _stdio.o


COMDEP0	= 	szal.result Makefile \
		Bits.h Block.h cfront.h hash.h node_classes.h overload.h \
		sdelicense.h sdetoolnum.h size.h template.h token.h \
		tqueue.h tree_copy.h tree_walk.h typedef.h

COMDEP	=	$(COMDEP0) yystype.h

cfront:	$(OFILES)
	$(CXX)	-o cfront $(OFILES)
	-@ test -x ../cfront && mv ../cfront ../cfront.old
	cp cfront ..

szal.result: szal/szal.c
	gcc -o szal/szal szal/szal.c
	./szal/szal > szal.result

y.tab.C yystype.h: gram.y $(COMDEP0)
	yacc -d gram.y
	mv y.tab.c y.tab.C
	mv y.tab.h yystype.h
	sed -i -e 's/^#ifdef __cplusplus/#if 0/' y.tab.C

alloc.o: alloc.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c alloc.C 

Bits.o:	Bits.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c Bits.C 

block.o: block.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c block.C 

dcl.o:	dcl.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c dcl.C 

dcl2.o:	dcl2.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c dcl2.C 

dcl3.o:	dcl3.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c dcl3.C 

dcl4.o:	dcl4.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c dcl4.C 

del.o:	del.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c del.C 

discrim.o: discrim.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c discrim.C

error.o: error.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c error.C 

expand.o: expand.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c expand.C 

expr.o:	expr.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c expr.C 

expr2.o: expr2.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c expr2.C 

expr3.o: expr3.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c expr3.C 

find.o:	find.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c find.C 

hash.o: hash.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c hash.C 

lalex.o:lalex.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c lalex.C

lex.o:	lex.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c lex.C 

main.o:	main.C $(COMDEP)
	$(CXX) -D$(MPMACRO) $(DENSE) $(CCFLAGS) -c main.C 

norm.o:	norm.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c norm.C 

norm2.o: norm2.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c norm2.C 

print.o: print.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c print.C 

print2.o: print2.C $(COMDEP)
	$(CXX) $(DENSE) $(CCFLAGS) -c print2.C 

repr.o:	repr.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c repr.C 

simpl.o: simpl.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c simpl.C 

simpl2.o: simpl2.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c simpl2.C 

size.o:	size.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c size.C 

table.o: table.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c table.C 

template.o: template.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c template.C 

tree_copy.o: tree_copy.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c tree_copy.C 

tree_walk.o: tree_walk.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c tree_walk.C 

typ.o: typ.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c typ.C 

typ2.o:	typ2.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c typ2.C 

y.tab.o: y.tab.C $(COMDEP)
	$(CXX) $(CCFLAGS) -c y.tab.C 

_stdio.o: _stdio.c
	cc -c _stdio.c

clean:
	-@rm szal.result szal/szal
	-@rm y.tab.C yystype.h
	-@rm *.o cfront *..c
