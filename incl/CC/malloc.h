#ifndef __MALLOC__H
#define __MALLOC__H 1

#include <stddef.h>

extern "C" {
    #ifndef __GNUC__
	void *malloc(unsigned);
	void free(void *);
	void *realloc(void *, unsigned);
	void *calloc(unsigned, unsigned);
    #else
	#include <stdlib.h>
    #endif
}

#endif
