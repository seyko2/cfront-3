/*
 *  functions accessing debugging information in a Solaris a.out file
 *     compiled with -g -xs.
 *  The -xs option causes the linker to extract debugging info from
 *     .o files (section .stab.excl) and place it in a .stab section
 *     in the a.out.
 *  Otherwise a debugger will retrieve debugging info from the .o files.
 *  These are accessed through .stab.index (in the a.out), which contains
 *     pointers to the .o's.
 *
 *  Since the demangler presumably shouldn't rewrite .o files, only files
 *     containing a .stab section (i.e., linked with -xs) are demangled.
 *  .o's are left unchanged.
 *
 *  original source by Dave Kapilow...
 *  modifications by Dennis Mancl - 6/9/93
 *  adapted for demangler by Laura Eaves
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "util.h"
#include "Memspace.h"
#include "sym.h"

#define max(a,b) ((a)>(b) ? (a) : (b))

// symbol entry in .stab* sections
struct nlist {
	union {
		char	*n_name;	/* for use when in-core */
		long	n_strx;		/* index into file string table */
        } n_un;
	unsigned char	n_type;
	unsigned char	n_other;	/* unused */
	unsigned short	n_desc;
	long		n_value;
};

enum NTYPE { /* nlist::n_type -- see printtype() */
	NT_BMOD = 0x00,
	NT_GSYM = 0x20,
	NT_FUN = 0x24,
	NT_STSYM = 0x26,
	NT_LCSYM = 0x28,
	NT_MAIN = 0x2A,
	NT_OBJ = 0x38,
	NT_VERS = 0x3C,
	NT_RSYM = 0x40,
	NT_SLINE = 0x44,
	NT_EMOD = 0x62,
	NT_SO = 0x64,
	NT_LSYM = 0x80,
	NT_BINCL = 0x82,
	NT_SOL = 0x84,
	NT_PSYM = 0xA0,
	NT_EINCL = 0xA2,
	NT_LBRAC = 0xC0,
	NT_EXCL = 0xC2,
	NT_RBRAC = 0xE0
};

static Memspace *filespace; // space management for file

const char* MAGSTRING = "%@#";

static FILE*  tsymfile;
static FILE*  tstrfile;
static Elf32_Ehdr ehdr;
static int	nsects;
static int sectionindex( char* );
int elffd;
Elf32_Shdr *shdr;
char	*secstrings;

int
sectionindex( char *name )
{
	int nbytes, i;

	for(nbytes = strlen(name)+1, i = 0; i < nsects; i++)
		if (!memcmp(&secstrings[shdr[i].sh_name], name, nbytes))
			break;
	return i == nsects ? 0 : i;
}
char *
readsect( int i )
{
	size_t nbytes = (size_t)shdr[i].sh_size;
	DB(if(idebug>=1){ char* sn = &secstrings[shdr[i].sh_name];
		fprintf(dbfile,"reading %s offset==%ld,size==%ld\n",sn,shdr[i].sh_offset,shdr[i].sh_size);
	});

	if (lseek(elffd, shdr[i].sh_offset, SEEK_SET) == -1)
		error("seek error in readsect: offset==%ld\n",shdr[i].sh_offset);
	char *cp = new char[nbytes];
	if (!cp)
		error("can't new section buffer: size==%ld\n",nbytes);
	if (read(elffd, cp, nbytes) != nbytes)
		error("can't read section: size==%ld\n",nbytes);
	return cp;
}

static void do_nlistsect( char *name, char *sname );
static void readheaders();
static void print_ident(Elf32_Ehdr&,FILE*);
static void printtype(int,FILE*);
static void printmachine(int,FILE*);
static void printversion(int,FILE*);
static void printidclass(int,FILE*);
static void printiddata(int,FILE*);

ea earg = "";
void
error(char* fmt, ea a, ea b )
{
	fprintf(stderr,fmt,a,b);
	bombout(0);
}

void
readheaders()
{
	size_t nbytes;

	if (read(elffd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
		error("error reading file header");
	if (ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
	    ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
	    ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
	    ehdr.e_ident[EI_MAG3] != ELFMAG3 ||
	    ehdr.e_ident[EI_CLASS] != ELFCLASS32 ||
	    ehdr.e_ident[EI_DATA] != ELFDATA2MSB ||
	    (ehdr.e_type != ET_EXEC && ehdr.e_type != ET_REL) ||
	    ehdr.e_machine != EM_SPARC) {
		fprintf(stderr,"bad magic:\n");
		print_ident(ehdr,stderr);
		exit(1);
	}
	if (lseek(elffd, ehdr.e_shoff, SEEK_SET) == -1)
		error("error seeking to section headers: offset==%ld\n",ehdr.e_shoff);
	nsects = ehdr.e_shnum;
	nbytes = sizeof(Elf32_Shdr)*nsects;
	shdr = new Elf32_Shdr[nsects];
	if (read(elffd, shdr, nbytes) != nbytes)
		error("error reading section headers: %ld bytes\n",nbytes);
	nbytes = (size_t)shdr[ehdr.e_shstrndx].sh_size;
	if (lseek(elffd, shdr[ehdr.e_shstrndx].sh_offset, SEEK_SET) == -1)
		error("error seeking to section strings: offset==%ld\n",shdr[ehdr.e_shstrndx].sh_offset);
	secstrings = new char[nbytes];
	if (read(elffd, secstrings, nbytes) != nbytes)
		error("error reading section strings: %ld bytes\n",nbytes);
	DB(if(idebug>=1) {
	for(int i = 0; i < nsects; i++) {
		fprintf(dbfile,"shdr[%.3d].sh_name = %s\n", i, &secstrings[shdr[i].sh_name]);
		fprintf(dbfile,"          sh_type = %d\n", shdr[i].sh_type);
		fprintf(dbfile,"          sh_flags = %d\n", shdr[i].sh_flags);
		fprintf(dbfile,"          sh_size = %d\n\n", shdr[i].sh_size);
	}
	fprintf(dbfile,"ehdr.e_shnum = %d\n", ehdr.e_shnum);
	fprintf(dbfile,"ehdr.e_shentsize = %d\n", ehdr.e_shentsize);
	fprintf(dbfile,"ehdr.e_shoff = %d\n", ehdr.e_shoff);
	});

}

static int
db_demangle( char* s, String& demangled )
{
	int notmangled = 1;
	register char* p = s;
	char c;
	for ( ; p && *p && *p!=':'; ++p )
		if ( *p == '_' && p[1] == '_' ) notmangled = 0;
	if ( notmangled && *p && p[1] != 'T' ) return 1;
	c = *p;
	*p = '\0';
// filter out all functions that become C statics
//   C++ file statics, out-lined inlines
//	if ( notmangled || (c==':' && p[1]=='f') ) {
	if ( notmangled ) {
		demangled = s;
		notmangled = 1;
	}
	else {
		fflush(stderr);
		demangled = demangleAndSimplify(s,0,0);
		cerr.flush();
		if ( demangled == s ) notmangled = 1;
	}
	*p = c;

	char* q = p;
	if ( *p && *++p=='T' ) {  // potential enum/struct/union type descriptor
		// apparent enum format:
		//     enumname:T...=eenumerator1:nnn,enumerator2:nnn,;
		// apparent struct format:
		//     structname:T...=s16membername1:...;membername2:...;;
		// apparent union format:
		//     unionname:T...=u16membername1:...;membername2:...;;
		while (*p && *p!='=')
			++p;
		if ( *p ) ++p;
		if ( *p=='e' || *p=='s' || *p=='u' ) { // enum, struct, union
			char delimiter = (*p=='e') ? ',' : ';';
			while (*p && isdigit(*p))
				++p;
			if ( *p=='\0' )
				error("enum/struct/union descriptor format error: %s",s);
			do { // p is at the start of a data member name
				// q is the chunk of string before this name
				// preserve these chars in the total string
				c = *p; *p = '\0'; demangled += q; *p = c;
				// advance q past the name pointed to by p
				for ( q = p;  *q && *q != ':'; ++q ) ;
				c = *q; *q = '\0';
				fflush(stderr);
				demangled += demangleAndSimplify(p,1,0);
				cerr.flush();
				*q = c;
				notmangled = 0;
				// advance p to the next member-name
				p = q;
				while ( *p && *p!=delimiter ) ++p;
				if ( *p ) ++p;
			} while ( *p );
		} else {
			error("type descriptor format error: %s",s);
		}
	}
	demangled += q;
	return notmangled;
}

void
do_nlistsect( char *name, char *sname )
{
	/* Find sections + read them in */
	int sect = sectionindex(name);
	int ssect = sectionindex(sname);
	if (!sect || !ssect)
		return;

	char *strings = readsect(ssect);
	filespace->freeseg( shdr[ssect].sh_offset, shdr[ssect].sh_size );

	nlist *syms = (nlist*)readsect(sect);
	Elf32_Word nsyms = shdr[sect].sh_size/sizeof(nlist);

	DB(if(idebug>=2) {
	/* Print it out */
	fprintf(dbfile,"segment = %s, nsyms = %ld\n", name,nsyms);
	fprintf(dbfile,"     n_value   n_other n_desc n_type  n_un.n_name\n");
	fprintf(dbfile,"                                      [demangledme]\n");
	fprintf(dbfile,"    ---------  ------- ------ ------ -------------\n");
	});

	/* calculate string pointers and demangle */
	char* sp = strings;
	nlist *ne = syms + nsyms;
	MUSTFWRITE(MAGSTRING,4,tstrfile); // sanity check
	ulong ssect_size = 0;
#if 0
	int flag1 = (strcmp(name, ".stab.index") == 0);
	Block<nlist*> b;
	int bsize = 0;
	int adj = 0;
#endif
	register nlist *n;
	nlist* bmod;
	for ( n = syms; n < ne; ) {
		MUSTFWRITE(strings,1,tstrfile);
		ulong bmod_off = 1;
		bmod = n;
		if (n->n_type != NT_BMOD) {
			fprintf(stderr,"type!: ");
			printtype(n->n_type,stderr);
			error("\n");
		}
		for(int i = n->n_desc + 1; i; i--, n++) {
			char* s;
#if 0
			int flag2 = (flag1 && n->n_type==NT_OBJ);
			if (flag2) {
				b.reserve(bsize);
				b[bsize++] = n;
				s = "";
			} else {
#endif
				s = n->n_un.n_strx ? n->n_un.n_strx + sp : 0;
#if 0
			}
#endif
#ifdef IDEBUG
			char* mangled = s ? s : ""; // for debugging prints
#endif
			int l = s ? strlen(s) : 0;
			String demangled = "";
			int notmangled = 1;
			if ( s /*&& !flag2*/ ) n->n_un.n_strx = bmod_off;
			DB(if(idebug>=2) {
			fprintf(dbfile,"%4d ", n - syms);
			fprintf(dbfile,"%08x     %02x    %04x  ", n->n_value, n->n_other, n->n_desc);
			printtype(n->n_type,dbfile);
			fprintf(dbfile,"   %s\n",mangled);
			});
			switch ( n->n_type ) {
			default:
				break;
// filter out global symbols
//			case NT_GSYM:
// filter out functions
//			case NT_FUN:
// filter static symbols
//			case NT_STSYM:
			case NT_LCSYM:
			case NT_RSYM:
			case NT_LSYM:
			case NT_PSYM:
				notmangled = db_demangle(s,demangled);
				if ( notmangled == 0 ) {
					int i = demangled.length();
#if 0
					adj += i-l;
#endif
					if ( i > l ) s = new char[ i + 1 ];
					demangled.dump(s);
					s[i] = '\0';
					l = i;
				}
				break;
			}
			if ( s ) {
				/*if (!flag2)*/ n->n_un.n_strx = bmod_off;
				bmod_off += l + 1;
				MUSTFWRITE(s,l+1,tstrfile);
			}
			DB(if(idebug>=2 && notmangled==0) {
			fprintf(dbfile,"                                      %s\n",s);
			});
		}
		sp += bmod->n_value;
		bmod->n_value = bmod_off; // string table bytes in this BMOD
		ssect_size += bmod_off;
	}
#if 0
	if (flag1) {
		int remaining = shdr[ssect].sh_size - ssect_size + adj;
		int offset = shdr[ssect].sh_size - remaining;
		MUSTFWRITE(strings+offset,remaining,tstrfile);
		for (int i = 0; i<bsize; i++) {
			n = b[i];
			n->n_un.n_strx += adj;
		}
		bmod->n_value += remaining;
		ssect_size += remaining;
	}
#endif
	shdr[ssect].sh_size = ssect_size;
	MUSTFWRITE((char*)syms,(size_t)shdr[sect].sh_size,tsymfile);

	delete syms;
	delete strings;
}

void
printtype( int t, FILE* fd )
{
	char *s;
	putc(' ',fd);

	switch(t) {
	case NT_BMOD:	s = "BMOD";	 break;
	case NT_GSYM:	s = "GSYM";	 break;
	case NT_FUN:	s = "FUN";	 break;
	case NT_STSYM:	s = "STSYM";	 break;
	case NT_LCSYM:	s = "LCSYM";	 break;
	case NT_MAIN:	s = "MAIN";	 break;
	case NT_OBJ:	s = "OBJ";	 break;
	case NT_VERS:	s = "VERS";	 break;
	case NT_RSYM:	s = "RSYM";	 break;
	case NT_SLINE:	s = "SLINE";	 break;
	case NT_EMOD:	s = "EMOD";	 break;
	case NT_SO:	s = "SO";	 break;
	case NT_LSYM:	s = "LSYM";	 break;
	case NT_BINCL:	s = "BINCL";	 break;
	case NT_SOL:	s = "SOL";	 break;
	case NT_PSYM:	s = "PSYM";	 break;
	case NT_EINCL:	s = "EINCL";	 break;
	case NT_LBRAC:	s = "LBRAC";	 break;
	case NT_EXCL:	s = "EXCL";	 break;
	case NT_RBRAC:	s = "RBRAC";	 break;
	default:
		fprintf(fd,"%05x", t);
		return;
	}
	fprintf(fd,"%5s",s);
}

void
printmachine( int t, FILE* fd )
{
	char *s;
	putc(' ',fd);

	switch(t) {
	case EM_NONE:  s = "NONE"; break;
	case EM_M32:   s = "M32"; break;
	case EM_SPARC: s = "SPARC"; break;
	case EM_386:   s = "386"; break;
	case EM_68K:   s = "68K"; break;
	case EM_88K:   s = "88K"; break;
	case EM_486:   s = "486"; break;
	case EM_860:   s = "860"; break;
	case EM_NUM:   s = "NUM"; break;
	default:
		fprintf(fd,"%5d", t);
		return;
	}
	fprintf(fd,"%5s",s);
}

void
printversion( int t, FILE* fd )
{
	char *s;
	putc(' ',fd);

	switch(t) {
	case EV_NONE:    s = "NONE"; break;
	case EV_CURRENT: s = "CURRENT"; break;
	case EV_NUM:     s = "NUM"; break;
	default:
		fprintf(fd,"%7d", t);
		return;
	}
	fprintf(fd,"%7s",s);
}

void
printidclass( int t, FILE* fd )
{
	char *s;
	putc(' ',fd);

	switch(t) {
	case ELFCLASSNONE: s = "NONE"; break;
	case ELFCLASS32:   s = "32"; break;
	case ELFCLASS64:   s = "64"; break;
	case ELFCLASSNUM:  s = "NUM"; break;
	default:
		fprintf(fd,"%4d", t);
		return;
	}
	fprintf(fd,"%4s",s);
}

void
printiddata( int t, FILE* fd )
{
	char *s;
	putc(' ',fd);

	switch(t) {
	case ELFDATANONE: s = "NONE"; break;
	case ELFDATA2LSB: s = "2LSB"; break;
	case ELFDATA2MSB: s = "2MSB"; break;
	case ELFDATANUM:  s = "NUM"; break;
	default:
		fprintf(fd,"%4d", t);
		return;
	}
	fprintf(fd,"%4s",s);
}

void
print_ident( Elf32_Ehdr& ehdr, FILE* fd )
{
	fprintf(fd,"%4s",ehdr.e_ident);
	printidclass( ehdr.e_ident[EI_CLASS], fd );
	printiddata( ehdr.e_ident[EI_DATA], fd );
	putc('\n',fd);
	printtype( ehdr.e_type, fd );
	printmachine( ehdr.e_machine, fd );
	printversion( (int)ehdr.e_version, fd );
	putc('\n',fd);
}

#if 0
// symbol table and hash table objects
// a Symtab is a flat, contiguous table with a string table
// a Hashtab (: public Symtab) is a Symtab with an associated hash table
typedef unsigned long ulong;

class Symtab {
	friend Symtab* do_symsect( char*, char*, char* );
protected:
	int bad;
	char* name;   // table section name
	char* sname;  // string section name
	int sect;     // table section index
	int ssect;    // string section index
	Block<Elf32_Sym> sym;
	Block<char> symstr;
	ulong symcount;
	ulong strsize;
public:
	Symtab( char*, char* );
	~Symtab(){}
	char* symname( ulong i )
	{
		return sym[(int)i].st_name ? sym[(int)i].st_name + symstr : "";
	}
	virtual ulong lookup( const char* );
	virtual ulong insert( const char*, ulong );
	virtual void read();
	virtual void print();
	virtual void overwrite();
};

class Hashtab : public Symtab {
	char* hname;  // name of hash sect
	int hsect;    // index of hash sect
	Elf32_Word nbuckets;
	Elf32_Word nchain;
	Block<ulong> bucket;
	Block<ulong> chain;
public:
	Hashtab( char*, char*, char* );
	~Hashtab(){}
	ulong lookup( const char* );
	ulong insert( const char*, ulong );
	void read();
	void print();
	void overwrite();
};

// the following appears to be the same as elf_hash() in libelf
static unsigned long
elf_Hash( const char* name )
{
	unsigned long h = 0, g;
	while ( *name ) {
		h = (h << 4) + *name++;
		if ( g = h & 0xf0000000 )
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
}

// Symtab functions
Symtab::Symtab( char* n, char* s )
{
	bad = 0;
	name = n;  sname = s;
	/* Find sections + read them in */
	sect = sectionindex(name);
	ssect = sectionindex(sname);
	if (!sect || !ssect)
		bad = 1;
	symcount = 0;
	strsize = 0;
}

ulong
Symtab::lookup( const char* )
{
	return STN_UNDEF;
}

ulong
Symtab::insert( const char* s, ulong old )
{
	// just append
	sym.reserve(symcount);
	sym[(int)symcount] = sym[(int)old];
	long l = strlen(s) + 1;
	symstr.reserve(strsize+l);
	strcpy(symstr+strsize,s);
	sym[(int)symcount].st_name = strsize;
	strsize += l;
	return symcount++;
}

void
Symtab::read()
{
	if ( bad ) return;
	readsect(symstr,ssect);
	readsect(sym,sect);
	symcount = shdr[sect].sh_size/sizeof(Elf32_Sym);
	strsize = shdr[ssect].sh_size;
}

void
Symtab::print()
{
	ulong i;
	fprintf(dbfile,"'%s'[%d] ('%s'[%d]) symcount:%ld strsize:%ld\n",name,sect,sname,ssect,symcount,strsize);
	fprintf(dbfile,"     st_value  st_size st_info st_type st_name\n");
	fprintf(dbfile,"     -------- -------- ------- ------- --------------\n");
	for ( i = 0;  i < symcount;  ++i ) {
		register Elf32_Sym *n = &sym[(int)i];
		char* cp;
		char* s = symname(i);
		fprintf(dbfile,"%4d ", i);
		fprintf(dbfile,"%08x %08x ", n->st_value, n->st_size);
		switch(ELF32_ST_BIND(n->st_info)) {
		case STB_LOCAL:		cp = "LOCAL"; break;
		case STB_GLOBAL:	cp = "GLOBAL"; break;
		case STB_WEAK:		cp = "WEAK"; break;
		case STB_NUM:		cp = "NUM"; break;
		default:		cp = "OTHER"; break;
		}
		fprintf(dbfile,"%6s  ", cp);
		switch(ELF32_ST_TYPE(n->st_info)) {
		case STT_NOTYPE:	cp = "NON"; break;
		case STT_OBJECT:	cp = "OBJ"; break;
		case STT_FUNC:		cp = "FUNC"; break;
		case STT_SECTION:	cp = "SECT"; break;
		case STT_FILE:		cp = "FILE"; break;
		case STT_NUM:		cp = "NUM"; break;
		default:		cp = "OTHER"; break;
		}
		fprintf(dbfile,"%6s  ", cp);
		fprintf(dbfile," %s\n",s);
	}
}

void
Symtab::overwrite()
{
	if ( bad ) return;
	if ( strsize == shdr[ssect].sh_size ) // no change
		return;

	DB(if(idebug>=1) {
		fprintf(dbfile,"overwriting '%s': symcount == %ld\n",name,symcount);
	});
	filespace->freeseg( shdr[sect].sh_offset, shdr[sect].sh_size );
	filespace->freeseg( shdr[ssect].sh_offset, shdr[ssect].sh_size );
	shdr[sect].sh_size = symcount*sizeof(Elf32_Sym);
	shdr[ssect].sh_size = strsize;
	shdr[sect].sh_offset = filespace->allocseg( shdr[sect].sh_size );
	shdr[ssect].sh_offset = filespace->allocseg( shdr[ssect].sh_size );
	MUSTLSEEK(elffd,shdr[sect].sh_offset,SEEK_SET);
	MUSTWRITE(elffd,(Elf32_Sym*)sym,(size_t)shdr[sect].sh_size);
	MUSTLSEEK(elffd,shdr[ssect].sh_offset,SEEK_SET);
	MUSTWRITE(elffd,(char*)symstr,(size_t)shdr[ssect].sh_size);
}

// Hashtab functions
Hashtab::Hashtab( char* n, char* s, char* h ) : Symtab(n,s)
{
	hname = h;
	hsect = sectionindex(hname);
	if ( !hsect ) bad = 1;
}

extern "C" ulong elf_hash( const char* );

ulong
Hashtab::lookup( const char* name )
{
	ulong h = elf_hash( name );
	ulong x = bucket[(h%nbuckets)];
	while ( x != STN_UNDEF ) {
		char* s = symname(x);
		if ( strcmp(s,name) == 0 ) break;
		if ( *s && (h%nbuckets) != (elf_hash(s)%nbuckets) ) {
			fprintf(stderr,"%s collides with %s but has a different hash value (%ld vs %ld, x==%ld, nbuckets==%ld)\n",name,s,ulong(h%nbuckets),ulong(elf_hash(s)%nbuckets),x,nbuckets);
			error("");
		}
		x = chain[x];
	}
	return x;
}

ulong
Hashtab::insert( const char* name, ulong old )
{
	// update both sym tab and hash tab
	ulong h = elf_hash( name );
	ulong x = bucket[(h%nbuckets)];
	ulong y = STN_UNDEF;
	do {
		char* s = symname(x);
		if ( strcmp(s,name) == 0 )
			error("insert(\"%s\") -- name exists!");
		if ( x == STN_UNDEF || chain[x] == STN_UNDEF ) {
			y = Symtab::insert(name,old); // append
			if ( x == STN_UNDEF )
				bucket[(h%nbuckets)] = y;
			else
				chain[x] = y;
			chain.reserve(y);
			chain[y] = STN_UNDEF;
			nchain = y;
			break;
		}
		x = chain[x];
	} while ( x != STN_UNDEF );
	return y;
}

void
Hashtab::read()
{
	Elf32_Word *hashtab;
	if ( bad ) return;
	Symtab::read();
	if ( !bad ) {
		hashtab = (Elf32_Word*)readsect(hsect);
		nbuckets = hashtab[0];
		nchain = hashtab[1];
		bucket.reserve(nbuckets);
		chain.reserve(nchain);
		memcpy((ulong*)bucket,hashtab+2,(size_t)nbuckets*sizeof(ulong));
		memcpy((ulong*)chain,hashtab+2+nbuckets,(size_t)nchain*sizeof(ulong));
	}
}

void
Hashtab::print()
{
	char* s;
	ulong i;
	fprintf(dbfile,"\nhash table('%s'->'%s'->'%s'): hsect==%d, nbuckets==%ld, nchain==%ld\n",hname,name,sname,hsect,nbuckets,nchain);
	for ( i = 0; i<nbuckets; ++i ) {
		fprintf(dbfile,"b[%.4ld]: %.4ld",i,bucket[i]);
		if ( bucket[i] != STN_UNDEF ) {
			s = symname(bucket[i]);
			fprintf(dbfile," '%s'",s);
		}
		fprintf(dbfile,"\n");
	}
	for ( i = 0; i<nchain; ++i ) {
		fprintf(dbfile,"c[%.4ld]: %.4ld ",i,chain[i]);
		if ( chain[i] == STN_UNDEF )
			fprintf(dbfile,"null symbol\n");
		else {
			s = symname(chain[i]);
			fprintf(dbfile,"'%s'\n",s);
		}
	}
	fprintf(dbfile,"--->\n");
	Symtab::print();
}

void
Hashtab::overwrite()
{
	if ( bad ) return;
	if ( strsize == shdr[ssect].sh_size ) // no change
		return;
	filespace->freeseg( shdr[hsect].sh_offset, shdr[hsect].sh_size );
	Symtab::overwrite();
	shdr[hsect].sh_size = (nbuckets + nchain + 2) * sizeof(ulong);
	shdr[hsect].sh_offset = filespace->allocseg( shdr[hsect].sh_size );
	MUSTLSEEK(elffd, shdr[hsect].sh_offset, SEEK_SET);
	MUSTWRITE(elffd, size_t(&nbuckets), sizeof(ulong));
	MUSTWRITE(elffd, size_t(&nchain), sizeof(ulong));
	MUSTWRITE(elffd, (ulong*)bucket, (size_t)nbuckets*sizeof(ulong));
	MUSTWRITE(elffd, (ulong*)chain, (size_t)nchain*sizeof(ulong));
}

Symtab*
do_symsect( char *name, char *sname, char *hname )
{
	Symtab *tab = /*hname ? new Hashtab(name,sname,hname) :*/ new Symtab(name,sname);
	tab->read();
	if ( tab->bad ) return tab;
	//MUSTFWRITE(MAGSTRING,4,tstrfile); // sanity check
	//MUSTFWRITE(strings,1,tstrfile);
	ulong osize = tab->symcount, ix = 0;
	DB(if(readOnly)goto ll;);
	for( ;  ix < osize;  ++ix ) {
//		ulong newx = ix;
		register Elf32_Sym *n = &tab->sym[(int)ix];
		char* s = tab->symname(ix);
		String demangled = "";
		switch(ELF32_ST_TYPE(n->st_info)) {
		case STT_OBJECT:
		case STT_FUNC:
			fflush(stderr);
			demangled = demangleAndSimplify(s,0,0);
			cerr.flush();
			if ( demangled != s ) {
				ulong x = tab->lookup(demangled);
				if ( x != STN_UNDEF )
					fprintf(stderr,"warning: demangled name %s already in symbol table; ignored\n",tab->symname(x));
				else
				if ( x = tab->insert(demangled,ix) ) {
//					newx = x;
//fprintf(stderr,"new symbol %.4ld: %s ([%.4ld] %s)\n",newx,(const char*)demangled,ix,s);
				} else
					fprintf(stderr,"warning: demangled name %s insert returned null\n",(const char*)demangled);
			}
		default:
			break;
		}
	}
	DB(ll:if(idebug>=2) tab->print());
	return tab;
}
#endif

void
overwrite_sect( char* name, char* sname )
{
	int sect, ssect;
	sect = sectionindex(name);
	ssect = sectionindex(sname);
	if (!sect || !ssect)
		return;
	long nbytes = max(shdr[sect].sh_size,shdr[ssect].sh_size);
	long stroff = filespace->allocseg(shdr[ssect].sh_size);
	char buf[4];
	char* sectbuf = new char[nbytes];

	DB(if(idebug>=1)fprintf(dbfile,"overwriting %s offset==%ld,size==%ld\n",name,(long)shdr[sect].sh_offset,(long)shdr[sect].sh_size););
	MUSTFREAD(sectbuf,(size_t)shdr[sect].sh_size,tsymfile);
	MUSTLSEEK(elffd,shdr[sect].sh_offset,SEEK_SET);
	MUSTWRITE(elffd,sectbuf,(size_t)shdr[sect].sh_size);

	DB(if(idebug>=1) {
		fprintf(dbfile,"%s offset: old 0x%lx new 0x%lx; size 0x%lx\n",sname,(long)shdr[ssect].sh_offset,stroff,(long)shdr[ssect].sh_size);
		fprintf(dbfile,"overwriting %s offset==%ld,size==%ld\n",sname,stroff,(long)shdr[ssect].sh_size);
		fprintf(dbfile,"free space in %s:\n",filespace->name());
		filespace->print();
	});
	shdr[ssect].sh_offset = stroff;
	MUSTFREAD(buf,4,tstrfile);
	if ( strncmp(buf,MAGSTRING,4) != 0 ) {
		cerr << "string file out of phase\n" << flush;
		bombout(0);
	}
	MUSTFREAD(sectbuf,(size_t)shdr[ssect].sh_size,tstrfile);
	MUSTLSEEK(elffd,stroff,SEEK_SET);
	MUSTWRITE(elffd,sectbuf,(size_t)shdr[ssect].sh_size);

	delete sectbuf;
}

void
reformat( char* aoutname )
{
	int mode = overwriteAout ? O_RDWR : O_RDONLY;
	struct stat statbuf;
	elffd = open(aoutname, mode);
	if (elffd == -1) {
		cerr << "Can't open " << aoutname << " for " << (mode==O_RDWR?"readwrite" : "reading") << "\n";
		exit(2);
	}
	if ( fstat(elffd,&statbuf) < 0 ) {
		cerr.flush();
		perror("demangle:reformat()");
		fflush(stderr);
		return;
	}
	filespace = new Memspace(statbuf.st_size,aoutname,Memspace::M_dynamic);
	DB( if ( idebug >= 1 ) filespace->set_debug(idebug-1); );
	filespace->allocseg(statbuf.st_size);

	tsymfile = fopen(tsymfilename, "w+");
	if (tsymfile == NULL) {
		cerr << "Can't create temp file " << tsymfilename << "\n" << flush;
		bombout(0);
	}
	tstrfile = fopen(tstrfilename, "w+");
	if (tstrfile == NULL) {
		cerr << "Can't create temp file " << tstrfilename << "\n" << flush;
		bombout(0);
	}

	readheaders();

	// .stab & .stabstr: must be demangled
	do_nlistsect(".stab", ".stabstr");

#if 0
	// .stab.excl & .stab.exclstr
	do_nlistsect(".stab.excl", ".stab.exclstr");

	// .stab.index & .stab.indexstr
	do_nlistsect(".stab.index", ".stab.indexstr");

	// .symtab & .strtab
	Symtab* st = do_symsect(".symtab", ".strtab", 0);

	// .dynsym & .dynstr & .hash
	Symtab* dt = do_symsect(".dynsym", ".dynstr", ".hash");
#endif

	if (overwriteAout) {
		DB(if(idebug>=1) {
			fprintf(dbfile,"free space in %s:\n",aoutname);
			filespace->print();
			fprintf(dbfile,"\n");
		});
		// write symbol table sections to update offsets
		// write new string table sections (may be relocated
		//    in file if size has changed)
		MUSTFSEEK(tsymfile,0,SEEK_SET);
		MUSTFSEEK(tstrfile,0,SEEK_SET);
		overwrite_sect(".stab", ".stabstr");
#if 0
		overwrite_sect(".stab.excl", ".stab.exclstr");
		overwrite_sect(".stab.index", ".stab.indexstr");
		st->overwrite();
		dt->overwrite();
#endif
		// overwrite section headers for new size/offset
		// overwrite all to avoid multiple seeks for individual headers
		DB(if(idebug>=1) fprintf(dbfile,"overwriting headers\n"););
		MUSTLSEEK(elffd, ehdr.e_shoff, SEEK_SET);
		MUSTWRITE(elffd, shdr, nsects*sizeof(Elf32_Shdr));
	}
#if 0
	delete st;
	delete dt;
#endif
	fclose(tsymfile);
	fclose(tstrfile);
}
