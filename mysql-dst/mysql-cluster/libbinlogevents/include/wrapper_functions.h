/* Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/**
  @file

  @brief Contains wrapper functions for memory allocation and deallocation.
  This includes generic functions to be called from the binlogevent library,
  which call the appropriate corresponding function, depending on whether
  the library is compiled independently, or with the MySQL server.
*/

#ifndef WRAPPER_FUNCTIONS_INCLUDED
#define WRAPPER_FUNCTIONS_INCLUDED

#include "binlog_config.h"
#ifndef STANDALONE_BINLOG
#define HAVE_MYSYS 1
#endif

#if HAVE_MYSYS
#include "my_sys.h"
extern PSI_memory_key key_memory_log_event;
#else
#include <cassert>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <string.h>

#endif

#if !defined(DBUG_OFF)
#if HAVE_MYSYS
#define BAPI_ASSERT(x) DBUG_ASSERT(x)
#else
#define BAPI_ASSERT(x) assert(x)
#endif
#else
#define BAPI_ASSERT(x) do { } while(0)
#endif

/**
  The strndup() function returns a pointer to a new string which is a duplicate
  of the string s, but it only copies at most n bytes. If s is longer than n,
  only n bytes are copied, and a terminating null byte ('\0') is added.
  Memory for the new string is obtained with malloc,
  and can be freed with free.
  @param  s  The string whose copy we want to create
  @param  n  Number of bytes to be copied

  @return    The duplicated string, or NULL if insufficient memory was available.
*/
#ifndef HAVE_STRNDUP
inline char *strndup (const char *s, size_t n)
{
  char *result;
  size_t len = strlen (s);

  if (n < len)
    len = n;

  result = (char *) malloc (len + 1);
  if (!result)
    return 0;

  result[len] = '\0';
  return (char *) memcpy (result, s, len);
}
#endif


/**
  This is a wrapper function, and returns a pointer to a new string which is
  a duplicate of the input string. The terminating Null character is added.

  If compiled with MySQL server,the strndup function from the mysys library is
  called, which allow instrumenting memory allocated. Else, the standard
  string function is called.

  @param destination The string to be duplicated
  @param n           The number of bytes to be copied

  @return The duplicated string, or NULL if insufficient memory was available.
*/
inline const char* bapi_strndup(const char *destination, size_t n)
{
#if HAVE_MYSYS
/* Call the function in mysys library, required for memory instrumentation */
  return my_strndup(key_memory_log_event, destination, n, MYF(MY_WME));
#else
  return strndup(destination, n);
#endif
}


/**
  This is a wrapper function, and returns a pointer to a new memory with the
  contents copied from the input memory pointer, upto a given length

  @param source Pointer to the buffer from which data is to be copied
  @param len Length upto which the source should be copied

  @return dest pointer to a new memory if allocation was successful
          NULL otherwise
*/
inline void* bapi_memdup(const void* source, size_t len)
{
  void* dest;
#if HAVE_MYSYS
  /* Call the function in mysys library, required for memory instrumentation */
  dest= my_memdup(key_memory_log_event, source, len, MYF(MY_WME));
#else
  dest= malloc(len);
  if (dest)
    memcpy(dest, source, len);
#endif
  return dest;
}


/**
  This is a wrapper function inorder to  allocate memory from the heap
  in the binlogevent library.

  If compiled with the MySQL server, and memory is allocated using memory
  allocating methods from the mysys library, my_malloc is called. Otherwise,
  the standard malloc() is called from the function.

  @param size         Size of the memory to be allocated.
  @param key_to_int   A mapping from the PSI_memory_key to an enum
  @param flags        flags to pass to MySQL server my_malloc functions
  @return Void pointer to the allocated chunk of memory
*/
inline void * bapi_malloc(size_t size, int flags)
{
  void * dest= NULL;
#if HAVE_MYSYS
  dest= my_malloc(key_memory_log_event, size, MYF(flags));
#else
  dest= malloc(size);
#endif
  return dest;
}


/**
  This is a wrapper function inorder to free the memory allocated from the heap
  in the binlogevent library.

  If compiled with the MySQL server, and memory is allocated using memory
  allocating methods from the mysys library, my_free is called. Otherwise,
  the standard free() is called from the function.

  @param Pointer to the memory which is to be freed.
*/
inline void bapi_free(void* ptr)
{
#if HAVE_MYSYS
  return my_free(ptr);
#else
  return free(ptr);
#endif
}
#endif
 