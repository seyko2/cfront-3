/*
 * patch.c
 *
 * patch together the list of ctors and dtors
 * the order is significant, since we want to
 * initialize libraries first.
 * we assume that symbols are in the same order
 * they were loaded, so we initialize later symbols first.
 */

#include <u.h>
#include <libc.h>
#include <bio.h>
#include <mach.h>

typedef struct Link Link;
typedef void (*Unswab)(ulong, uchar*);

struct Link {
	Link	*next;
	ulong	offset;
};

Link	head;
Link	x;
char	*file;
long	datadel;
int	debug;

void	error(char*);
Link	*findlinks(int, Fhdr*);
Link	*links(Sym*, Link*);
void	patch(int, Link *);
Unswab	nuxi(int, Link);
void	beunswab(ulong, uchar*);
void	leunswab(ulong, uchar*);

void
main(int argc, char *argv[])
{
	Fhdr hdr;
	Link *links;
	int fd;

	argv0 = *argv;
	ARGBEGIN{
	case 'd':
		debug = 1;
		break;
	}ARGEND

	if(argc < 1){
		fprint(2, "usage: patch [-d] file ...\n");
		exits("usage");
	}
	while(argc--){
		file = *argv++;
		fd = open(file, ORDWR);
		if(!crackhdr(fd, &hdr))
			error("can't read header");
		links = findlinks(fd, &hdr);
		patch(fd, links);
		close(fd);
	}
	exits(0);
}

/*
 * find the link and header symbols
 */
Link *
findlinks(int fd, Fhdr *hdr)
{
	Link *link;
	Sym *s;
	int i;
	long nsyms;

	datadel = hdr->datoff - hdr->dataddr;
	nsyms = syminit(fd, hdr);
	if(nsyms <= 0)
		error("can't initialize symbol table");
	s = symbase(&nsyms);
	link = 0;
	head.offset = 0;
	for(i = 0; i < nsyms; i++)
		link = links(s+i, link);
	return link;
}

Link *
links(Sym *s, Link *link)
{
	if(s->type == 'd'){
		if(strcmp("__link", s->name) == 0){
			Link *l;

			l = malloc(sizeof(Link));
			if(l == 0)
				error("out of memory");
			l->next = link;
			l->offset = s->value;
			return l;
		}
		if(strcmp("__head", s->name) == 0)
			head.offset = s->value;
		else if(strcmp("__nuxi", s->name) == 0)
			x.offset = s->value;
	}
	return link;
}

void
patch(int fd, Link *link)
{
	Unswab unswab;
	Link *k;
	uchar next[4];
	ulong crap;

	unswab = nuxi(fd, x);
	memset(next, 0, sizeof next);
	for(k = link; k; k = k->next){
		if(debug){
			seek(fd, k->offset + datadel, 0);
			read(fd, &crap, sizeof crap);
			fprint(2, "link: %lux val %lux\n", k->offset, crap);
		}
		if(seek(fd, k->offset + datadel, 0) < 0)
			error("seek to link failed");
		if(write(fd, next, sizeof(next)) < sizeof(next))
			error("can't write link");
		(*unswab)(k->offset, next);
	}
	if(debug){
		seek(fd, head.offset + datadel, 0);
		read(fd, &crap, sizeof crap);
		fprint(2, "head: %lux val %lux\n", head.offset, crap);
	}
	if(seek(fd, head.offset + datadel, 0) < 0)
		error("seek to head failed\n");
	if(write(fd, next, sizeof(next)) < sizeof(next))
		error("can't write head");
}

Unswab
nuxi(int fd, Link x)
{
	uchar beef[4];

	if(debug)
		fprint(2, "nuxi at %d(%d)\n", x.offset, x.offset + datadel);
	if(seek(fd, x.offset + datadel, 0) < 0)
		error("seek to nuxi failed");
	if(read(fd, beef, sizeof(beef)) < sizeof(beef))
		error("can't read nuxi");
	if(beef[0] == 0xde && beef[1] == 0xad && beef[2] == 0xbe && beef[3] == 0xef)
		return beunswab;
	if(debug)
		fprint(2, "leunswab\n");
	return leunswab;
}

void
beunswab(ulong l, uchar *p)
{
	p[0] = l >> 24;
	p[1] = l >> 16;
	p[2] = l >> 8;
	p[3] = l;
}

void
leunswab(ulong l, uchar *p)
{
	p[0] = l;
	p[1] = l >> 8;
	p[2] = l >> 16;
	p[3] = l >> 24;
}

void
error(char *msg)
{
	fprint(2, "%s: %s for file %s\n", argv0, msg, file);
	exits(msg);
}
