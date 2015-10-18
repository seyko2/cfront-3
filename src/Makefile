CC	=	g++
CCFLAGS	=	-I../incl

#MPMACRO = PATCH
MPMACRO = MUNCH

OFILES	=	Bits.o alloc.o block.o dcl.o dcl2.o dcl3.o dcl4.o \
		del.o discrim.o error.o expand.o expr.o expr2.o \
		expr3.o find.o hash.o lalex.o lex.o main.o norm.o \
		norm2.o print.o print2.o repr.o simpl.o simpl2.o \
		size.o table.o template.o tree_copy.o tree_walk.o \
		typ.o typ2.o y.tab.o


COMDEP0	= 	szal.result Makefile \
		Bits.h Block.h cfront.h hash.h node_classes.h overload.h \
		sdelicense.h sdetoolnum.h size.h template.h token.h \
		tqueue.h tree_copy.h tree_walk.h typedef.h

COMDEP	=	$(COMDEP0) yystype.h

cfront:	$(OFILES)
	$(CC)	-o cfront $(OFILES)
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
	$(CC) $(CCFLAGS) -c alloc.C 

Bits.o:	Bits.C $(COMDEP)
	$(CC) $(CCFLAGS) -c Bits.C 

block.o: block.C $(COMDEP)
	$(CC) $(CCFLAGS) -c block.C 

dcl.o:	dcl.C $(COMDEP)
	$(CC) $(CCFLAGS) -c dcl.C 

dcl2.o:	dcl2.C $(COMDEP)
	$(CC) $(CCFLAGS) -c dcl2.C 

dcl3.o:	dcl3.C $(COMDEP)
	$(CC) $(CCFLAGS) -c dcl3.C 

dcl4.o:	dcl4.C $(COMDEP)
	$(CC) $(CCFLAGS) -c dcl4.C 

del.o:	del.C $(COMDEP)
	$(CC) $(CCFLAGS) -c del.C 

discrim.o: discrim.C $(COMDEP)
	$(CC) $(CCFLAGS) -c discrim.C

error.o: error.C $(COMDEP)
	$(CC) $(CCFLAGS) -c error.C 

expand.o: expand.C $(COMDEP)
	$(CC) $(CCFLAGS) -c expand.C 

expr.o:	expr.C $(COMDEP)
	$(CC) $(CCFLAGS) -c expr.C 

expr2.o: expr2.C $(COMDEP)
	$(CC) $(CCFLAGS) -c expr2.C 

expr3.o: expr3.C $(COMDEP)
	$(CC) $(CCFLAGS) -c expr3.C 

find.o:	find.C $(COMDEP)
	$(CC) $(CCFLAGS) -c find.C 

hash.o: hash.C $(COMDEP)
	$(CC) $(CCFLAGS) -c hash.C 

lalex.o:lalex.C $(COMDEP)
	$(CC) $(CCFLAGS) -c lalex.C

lex.o:	lex.C $(COMDEP)
	$(CC) $(CCFLAGS) -c lex.C 

main.o:	main.C $(COMDEP)
	$(CC) -D$(MPMACRO) $(DENSE) $(CCFLAGS) -c main.C 

norm.o:	norm.C $(COMDEP)
	$(CC) $(CCFLAGS) -c norm.C 

norm2.o: norm2.C $(COMDEP)
	$(CC) $(CCFLAGS) -c norm2.C 

print.o: print.C $(COMDEP)
	$(CC) $(CCFLAGS) -c print.C 

print2.o: print2.C $(COMDEP)
	$(CC) $(DENSE) $(CCFLAGS) -c print2.C 

repr.o:	repr.C $(COMDEP)
	$(CC) $(CCFLAGS) -c repr.C 

simpl.o: simpl.C $(COMDEP)
	$(CC) $(CCFLAGS) -c simpl.C 

simpl2.o: simpl2.C $(COMDEP)
	$(CC) $(CCFLAGS) -c simpl2.C 

size.o:	size.C $(COMDEP)
	$(CC) $(CCFLAGS) -c size.C 

table.o: table.C $(COMDEP)
	$(CC) $(CCFLAGS) -c table.C 

template.o: template.C $(COMDEP)
	$(CC) $(CCFLAGS) -c template.C 

tree_copy.o: tree_copy.C $(COMDEP)
	$(CC) $(CCFLAGS) -c tree_copy.C 

tree_walk.o: tree_walk.C $(COMDEP)
	$(CC) $(CCFLAGS) -c tree_walk.C 

typ.o: typ.C $(COMDEP)
	$(CC) $(CCFLAGS) -c typ.C 

typ2.o:	typ2.C $(COMDEP)
	$(CC) $(CCFLAGS) -c typ2.C 

y.tab.o: y.tab.C $(COMDEP)
	$(CC) $(CCFLAGS) -c y.tab.C 

clean:
	-@rm szal.result szal/szal
	-@rm y.tab.C yystype.h
	-@rm *.o cfront
