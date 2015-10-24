#ifndef UTIL_H
#define UTIL_H

#include <elf.h>
#include "Block.h"

// make these extern and in header so instantiation will work
struct ea {
	union {
		long l;
		char* s;
	};
	ea(long  i) : l(i) {}
	ea(const char* p) : s((char*)p) {}
};
extern ea earg;
extern void error( char*, ea = earg, ea = earg );
extern int elffd;
extern Elf32_Shdr *shdr;
extern char	*secstrings;
template <class T> T* readsect( Block<T>&, int );

#endif
