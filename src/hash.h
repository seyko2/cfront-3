/* ident "@(#)cls4:src/hash.h	1.6" */
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
/******************************************************************************
*    Copyright (c) 1989 by Object Design, Inc., Burlington, Mass.
*    All rights reserved.
*******************************************************************************/
/* 
	Compiler interface to hash tables from odi library. 
*/

#ifndef _HASH_H
#define _HASH_H

#include <string.h>

typedef void (*Error_Proc) (const char*) ;

extern void default_Hash_error_handler (const char*) ;
extern Error_Proc set_Hash_error_handler (Error_Proc f) ;

#ifndef _hash_typedefs
#define _hash_typedefs 1
typedef void (*intProc)(int) ;
#endif

#define DEFAULT_INITIAL_HASH_SIZE 100

struct HashTableEntry
{
  int                   key ;
  int                   cont ;
  char                  status ;
} ;

class HashWalker ;

class Hash
{
  friend class          HashWalker ;

  HashTableEntry*       tab ;
  int                   size ;
  int                   entry_count ;

public:
  unsigned int		(*key_hash_function)(int)  ;
   int            	(*key_key_equality_function) (int, int) ;

  unsigned int          key_hash(int  a) ;
  int                   key_key_eq(int a, int  b);    

                        Hash(int sz) ;
                        Hash(Hash& a) ;
                        ~Hash() ;

  Hash&                 operator= (Hash& a) ;

  int                   count() ;
  int                   empty() ;
  int                   full() ;
  int                   capacity() ;

  void                  clear() ;
  void                  resize(int newsize) ;

  enum insert_action	{ probe, insert, replace };
  void			action (int key, int val, insert_action what,
				int& found, int& old_val);
  int&                  operator [] (int  k) ;
  int                   contains(int  key) ;
  int                   del(int  key) ;

  void                  apply (intProc f) ;
  void                  error(const char* msg) ;
} ;

class HashWalker
{
  Hash*     h ;
  int                   pos ;

public:
                        HashWalker(Hash& l) ;
                        ~HashWalker() ;

  int                   null() ;
  int                   valid() ;
                        operator void* () ;
  int                   operator ! () ;
  void                  advance() ;
  void                  reset() ;
  void                  reset(Hash& l) ;
  const int&            key() ;
  int&                  get() ;
} ;

inline unsigned int Hash::key_hash(int a)
{
#ifdef HASHFUNCTION
  return HASHFUNCTION(a) ;
#else
  return (*key_hash_function)(a) ;
#endif
}

inline int Hash::key_key_eq(int a, int b)
{
#ifdef EQUALITYFUNCTION
  return EQUALITYFUNCTION(a, b) ;
#else
  return (*key_key_equality_function)(a, b) ;
#endif
}


inline Hash::~Hash()
{
  delete [/*size*/] tab ;
}

inline int Hash::count()
{
  return entry_count ;
}

inline int Hash::empty()
{
  return entry_count == 0 ;
}

inline int Hash::full()
{
  return entry_count == size ;
}

inline int Hash::capacity()
{
  return size ;
}
inline HashWalker::HashWalker(Hash& a)
{
  h = &a ;
  reset() ;
}

inline void HashWalker::reset(Hash& a)
{
  h = &a ;
  reset() ;
}


inline HashWalker::~HashWalker() {}

inline int HashWalker::null()
{
  return pos < 0 ;
}

inline int HashWalker::valid()
{
  return pos >= 0 ;
}

inline HashWalker::operator void* ()
{
  return (pos < 0)? 0 : this ;
}

inline int HashWalker::operator ! ()
{
  return (pos < 0) ;
}


inline const int& HashWalker::key()
{
  if (pos < 0)
    h->error("operation on null Walker") ;
  return h->tab[pos].key ;
}

inline int& HashWalker::get()
{
  if (pos < 0)
    h->error("operation on null Walker") ;
  return h->tab[pos].cont ;
}

int pointer_hasheq(int, int);
unsigned int pointer_hash_fcn(int);

class pointer_hash : public Hash {
  public:
    pointer_hash (int sz = 0) : Hash (sz) {
	key_hash_function = pointer_hash_fcn;
	key_key_equality_function = pointer_hasheq;
    }

    pointer_hash (pointer_hash& h) : Hash (h) {};
};

int string_hasheq(int, int);
unsigned int string_hash_fcn(int);

class string_hash : public Hash {
    public:

    string_hash (int sz = 0) : Hash (sz) {
	key_hash_function = string_hash_fcn;
	key_key_equality_function = string_hasheq;
    };

    string_hash (string_hash& h) : Hash (h) {};
};

#endif
