/*
 * munch -- look for static constructors and destructors
 * in a C++ program.
 * this version assumes output has been run through gre '__st[di]__'
 */

#include <u.h>
#include <libc.h>
#include <bio.h>
#include <ctype.h>

#define isident(c) (isalnum(c) || c == '_')

typedef struct Strs Strs;

struct Strs {
	Strs	*next;
	char	str[100];
};

Strs *
mkStrs(Strs *n, char *s)
{
	Strs *new = malloc(sizeof(Strs));
	char *t;
	char c;

	new->next = n;
	strcpy(new->str, s);
	return new;
}

Strs	*dtors;		/* list of destructors */
Strs	*ctors;		/* list of constructors */

int
main(void)
{
	char *s;
	char *stid;
	char kind;
	Strs *p;
	Biobuf bin;

	Binit(&bin, OREAD, 0);
	while ((s = Brdline(&bin, '\n')) && *s) {
		s[Blinelen(&bin)-1] = 0;

		look:
		while (*s && *s++ != '_')
			;
		if(*s == 0)
			continue;
		if(s[0] != '_' || s[1] != 's' || s[2] != 't'
		|| (s[3] != 'i' && s[3] != 'd') || s[4] != '_' || s[5] != '_')
			goto look;
		stid = s - 1;
		for (s += 5; isident(*s); s++)
			;
		*s = '\0';
		if(stid[4] == 'i')
			ctors = mkStrs(ctors, stid);
		else
			dtors = mkStrs(dtors, stid);
	}

	print("typedef int (*PFV)();\n");
	if(ctors)
		for (p = ctors; p; p = p->next)
			print("int %s();\n",p->str);
	print("extern PFV _ctors[];\nPFV _ctors[] = {\n");
	if(ctors)
		for (p = ctors; p; p = p->next)
			print("\t%s,\n", p->str);

	print("\t0\n};\n");

	if(dtors)
		for (p = dtors; p; p=p->next)
			print("int %s();\n",p->str);
	print("extern PFV _dtors[];\nPFV _dtors[] = {\n");
	if(dtors)
		for (p = dtors; p; p=p->next)
			print("\t%s,\n",p->str);

	print("\t0\n};\n");


	exits(nil);
}
