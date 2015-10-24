#include "idebug.h"
#include "util.h"

template<class T>
T*
readsect( Block<T>& b, int i )
{
	size_t nbytes = (size_t)shdr[i].sh_size;
	DB(if(idebug>=1){ char* sn = &secstrings[shdr[i].sh_name];
		fprintf(dbfile,"reading %s offset==%ld,size==%ld into block\n",sn,shdr[i].sh_offset,shdr[i].sh_size);
	});
	b.reserve(nbytes/sizeof(T));
	if (lseek(elffd, shdr[i].sh_offset, SEEK_SET) == -1)
		error("seek error in readsect: offset==%ld\n",shdr[i].sh_offset);
	if (read(elffd, (char*)(T*)b, nbytes) != nbytes)
		error("can't read section: size==%ld\n",nbytes);
	return (T*)b;
}
