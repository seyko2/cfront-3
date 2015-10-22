/*ident	"@(#)cls4:Patch/patch.c	1.3" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
/*patch: Patch an a.out to ensure that static constructors are called.
Currently this is in good-old-C.

 This code uses -lld, the Unix system V functions for accessing COFF files.
	The program is passed one argument, the name of an executable
	C++ program.  It first reads the COFF symbol table, remembering
	all symbols of the name __link.  
	  Each of these symbols points to a structure of the form:
		struct __linkl {
			struct __linkl *next;	//next link in the chain
			int (*ctor)();		//ptr to ctor function
			int (*dtor)();		//ptr to dtor function
		};
	cfront puts one of these in each dot-o.  Patch finds them,
	and chains them together by writing
	(into the actual a.out) values for the "next"
	pointers.  A pointer to the start of the chain is written
	into the struct __linkl pointer named __head.
	 _main will follow this chain at runtime and 
	use the ctor and dtor function pointers to call the static
	ctors and dtors.
***************************************************************/

#include <stdio.h>
#include <filehdr.h>
#include  <scnhdr.h>
#include <syms.h>
#include <ldfcn.h>
typedef long PHSADDR;
char *ldgetname();
void fatal_error();
int debug = 0;
int found_main = 0;	/*1 iff _main() has been seen*/
PHSADDR head = 0;	/*Adress of the __head symbol*/
PHSADDR data_infile;	/*offset in file of the data segment*/
PHSADDR data_pa;	/*physical adress of start of data segment*/
PHSADDR zero = 0;
PHSADDR previous;
char *file;			/*Name of the file being patched*/

#define MAX_FILES 5000
PHSADDR addrs[MAX_FILES];	/*array of addresses of __link symbols*/
PHSADDR *addr_ptr = addrs;

main(argc , argv)
int argc;
char *argv[];
{
	LDFILE *ldptr;
	SCNHDR secthead;
	FILE *fptr;
	PHSADDR *symbol;
	int x_sym, undr;
	if (argc == 3 && strcmp(argv[1], "-d")==0 )
	{
		file = argv[2];
		debug = 1;
	}
	else if (argc == 2)
		file = argv[1];
	else
		fatal_error("usage: patch file");
			/*Try to open the file*/
	if ( (ldptr = ldopen(file, NULL)) == NULL)
		fatal_error("cannot open file");
			/*Find  the beginning of the .data section*/
	if (ldnshread( ldptr, ".data", &secthead) == FAILURE)
		fatal_error("cannot get .data header");
	
			/*remember the start of data section, both the
			  physical address at runtime,
			  and the offset in the file*/
	data_infile = secthead.s_scnptr;
	data_pa = secthead.s_paddr;
	if (debug)
	{
		printf("data in file: 0x%x\n", data_infile);
		printf("\taddr: 0x%x\n", data_pa);
	}
			/*seek to the beginning of the symbol table*/
	if (ldtbseek(ldptr) == FAILURE)
		fatal_error("cannot get symbol table");

			/*Find the magic symbols in the a.out*/
        for ( x_sym = 0; x_sym < HEADER(ldptr).f_nsyms; ++x_sym)
	{
		char *str;
		SYMENT sym;
                        /*read the symbol*/
		if ( FREAD( (char *)&sym, SYMESZ, 1, ldptr) != 1)
			break;
		str = ldgetname(ldptr,&sym);
#if vax || BSD
		str++;		/* skip extra underscore */
#endif

 		undr=0; while ( str[0] == '_' ) { str++; undr++; }

		if (undr == 2) {
			if (strcmp(str, "head") == 0)
			{
				if (debug)
					printf("__head found at 0x%x\n", 
					sym.n_value);
				head = sym.n_value;
			}
			else if (strcmp(str, "link")==0)
			{
				if (addr_ptr >= addrs + MAX_FILES)
					fatal_error(" too many files");
				*addr_ptr++ = sym.n_value;
				if (debug)
					printf("__link found at 0x%x\n",
					       sym.n_value);
			}
		}
		else if (undr == 1 && strcmp(str, "main") == 0)
			found_main++;

			/*Skip auxiliary entries*/
		if (sym.n_numaux)
		{
                        x_sym += sym.n_numaux;
                        FSEEK(ldptr, sym.n_numaux*SYMESZ, 1);
		}
	}
	if (!head)
	{
		if (found_main == 0)
			fatal_error("_main() not found");
		else
		fatal_error("Bad _main() loaded- libC probably not set up for patch");
	}
			/*Now we have all of the __link pointers.
			  close the file, and reopen it for updating to
			  write the patches.  All hell will break loose
			  if someone writes the file in the meantime.*/
	ldclose(ldptr);
			/*If no symbols were found, quit*/
	if ( addr_ptr == addrs)
	{
		if(debug)
			printf("No __links found\n");
		exit(0);
	}

	if ( (fptr=fopen(file, "r+")) == NULL)
		fatal_error(" can't reopen file");

			/*patch the first symbol*/
			/*seek to: physical adr. of symbol
				- physical adr of start of data
				+ file offset of start of data*/

	previous = head - data_pa + data_infile;

			/*Now, go thru the list of symbols. Each is
			  a pointer to the next link. Chain them up*/
			/*For non-obvious reasons, do this backwards.
			  This calls ctors from libraries first.*/

	for (symbol = addr_ptr - 1; symbol >= addrs; symbol --)
	{
			/*Update the previous pointer to point to this one.*/
		if(debug)
			printf("Write 0x%x at offset 0x%x\n",
			*symbol, previous );
		if (fseek(fptr, previous , 0))
			fatal_error("can't seek");
		if( fwrite((char *)symbol, sizeof(PHSADDR), 1, fptr) == 0)
			fatal_error("can't write file");
		previous = *symbol - data_pa + data_infile;
	}
			/*Zero out the last symbol*/
	if(fseek(fptr, previous , 0))
		fatal_error("can't seek");
	if (fwrite((char *)&zero, sizeof(PHSADDR), 1, fptr) == 0)
		fatal_error("can't write");
	if(debug)
		printf("Write 0 at offset 0x%x\n", previous );
	fclose(fptr);
	exit(0);
			  
}
void
fatal_error(message)
char * message;
{
	fprintf(stderr,"patch: file %s: %s\n", file, message);
	exit(-1);
}
