/*ident	"@(#)cls4:szal.c	1.1" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

szal.c:

	C program to run on a machine to write a size/align file for
	the C++ translator.

	Most output line are on the form:
		typeX	sizeof(typeX)	alignment_requirement_for_typeX
	assumes

          -	that ``double d0; char c0;'' poses the worst alignment condition
          -	two's complement integer representation
          -	that a word is defined by a :0 field

*****************************************************************************/

#include <stdio.h>

typedef int (*PF)();
struct st1 { char a; };
struct ss {
	double a0; char c0;
	char c1;
	double a00; char c00;
	short s1;
	double a2; char c2;
	int i1;
	double a3; char c3;
	long l1;
	double a4; char c4;
	float f1;
	double a6; char c5;
	double d1;
	double a7; char c6;
	char* p1;
	double a8; char c7;
	struct ss * p2;
	double a9; char c8;
	struct st1 oo;
	double a10; char c9;
	PF pf;
} oo;
struct st5 { char a; int :0; };	/* by definition: a word */
struct st2 { char :2; };
struct st3 { int :2; };
struct st4 { char :2; char :2; };

struct st6 { char v[3]; char : 2; };	/* fits in 4 bytes */
struct st7 { char v[3]; int : 2; };	/* might not */
struct st8 { char v[3]; char : 2; char : 2; };

struct st9 { char v[7]; char : 2; };	/* fits in 8 bytes */
struct st10 { char v[7]; int : 2; };	/* might not */
struct st11 { char v[7]; char : 2; char : 2; };

out(s,a1) char* s; int a1;
{
	printf("%s %d\n",s,a1);
}

outstr(s,str2) char* s; char* str2;
{
	printf("%s \"%s\"\n",s,str2);
}

int a123456789 = 1;	/* if this does not compile get a better C compiler */
int a123456780 = 2;


main()
{
	char largest[50];
	char c = 1;
	int i1 = 0;
	int i2 = 0;
	long i = 1L;
	unsigned int	large;

	if (a123456789 == a123456780)
		fprintf(stderr,"Warning: Your C compiler is dangerous.\nIt strips trailing characters off long identifiers without warning.\nGet a new one\n");

	while (c) { c<<=1; c&=~1; i1++; }	/* i1 = #bits in byte */

	if (sizeof(struct st5) == sizeof(char))	/* i2 = #bits in word  */
		i2 = i1;
	else if (sizeof(struct st5) == sizeof(short)) {
		short i = 1;
		while (i) { i<<=1; i&=~1; i2++; }
	}
	else if (sizeof(struct st5) == sizeof(int))
		while (i) { i<<=1; i&=~1; i2++; }
	else if (sizeof(struct st5) == sizeof(long)) {
		long i = 1;
		while (i) { i<<=1; i&=~1; i2++; }
	}
	else {
		fprintf(stderr,"Warning: Your C compiler probably handles 0 lengths fields wrong\n");
		i = sizeof(int);
	}

	out("#define DBI_IN_WORD",i2);
	out("#define DBI_IN_BYTE",i1);
	out("#define DSZ_CHAR",sizeof(char));
	out("#define DAL_CHAR",(int)&oo.c1-(int)&oo.c0);
	out("#define DSZ_SHORT",sizeof(short));
	out("#define DAL_SHORT",(int)&oo.s1-(int)&oo.c00);
	out("#define DSZ_INT",sizeof(int));
	out("#define DAL_INT",(int)&oo.i1-(int)&oo.c2);
	out("#define DSZ_LONG",sizeof(long));
	out("#define DAL_LONG",(int)&oo.l1-(int)&oo.c3);
	out("#define DSZ_FLOAT",sizeof(float));
	out("#define DAL_FLOAT",(int)&oo.f1-(int)&oo.c4);
	out("#define DSZ_DOUBLE",sizeof(double));
	out("#define DAL_DOUBLE",(int)&oo.d1-(int)&oo.c5);
	/*  next two should just be repeats of above two */
	out("#define DSZ_LDOUBLE",sizeof(double));
	out("#define DAL_LDOUBLE",(int)&oo.d1-(int)&oo.c5);
	i = 1<<(sizeof(char*)*i1-2); 
	if (i<400*1024L)
		fprintf(stderr,"Pointers to data too small to handle C++\n");

	i = 1<<(sizeof(PF)*i1-2); 
	if (i<250*1024L)
		fprintf(stderr,"Pointers to functions too small to handle C++\n");
	/* out("fptr",sizeof(PF),(int)&oo.pf-(int)&oo.c9,0); */
	if (sizeof(PF)!=sizeof(struct ss*))
		fprintf(stderr,"Cannot handle sizeof(pointer to function) != sizeof(pointer to struct)\n");
	out("#define DSZ_STRUCT",sizeof(struct st1));
	out("#define DAL_STRUCT",(int)&oo.oo-(int)&oo.c8);
	out("#define DSZ_WORD",sizeof(struct st5));
	out("#define DSZ_WPTR",sizeof(struct ss *));
	out("#define DAL_WPTR",(int)&oo.p2-(int)&oo.c7);
	out("#define DSZ_BPTR",sizeof(char*));
	out("#define DAL_BPTR",(int)&oo.p1-(int)&oo.c6);
	large = (unsigned)~0;
	large = large >> 1;
	sprintf(largest,"%d",large);	/* largest integer */
	outstr("#define DLARGEST_INT",largest);
	switch (sizeof(struct st1)) {
	case 1:
		i1 = sizeof(struct st2)!=sizeof(struct st3);
		i2 = sizeof(struct st2)==sizeof(struct st4);
		break;
	case 2:
		i1 = sizeof(struct st6)!=sizeof(struct st7);
		i2 = sizeof(struct st6)==sizeof(struct st8);
		break;
	case 4:
		i1 = sizeof(struct st9)!=sizeof(struct st10);
		i2 = sizeof(struct st9)==sizeof(struct st11);
		break;
	default:
		fprintf(stderr,"Cannot figure out if field sizes are sensitive to the type of fields\n");
	}
	out("#define DF_SENSITIVE",i1); /* sensitive to field type */
	out("#define DF_OPTIMIZED",i2); /* packs fields */
}
