/* Copyright (c) 2002, 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */


#include "sp_cache.h"

#include "my_atomic.h"
#include "sp_head.h"


/*
  Cache of stored routines.
*/

extern "C"
{
  static uchar *hash_get_key_for_sp_head(const uchar *ptr, size_t *plen,
                                         my_bool first)
  {
    sp_head *sp= (sp_head *)ptr;
    *plen= sp->m_qname.length;
    return (uchar*) sp->m_qname.str;
  }


  static void hash_free_sp_head(void *p)
  {
    sp_head *sp= (sp_head *)p;
    delete sp;
  }
}


class sp_cache
{
public:
  sp_cache()
  {
    my_hash_init(&m_hashtable, system_charset_info, 0, 0, 0,
                 hash_get_key_for_sp_head, hash_free_sp_head, 0,
                 key_memory_sp_cache);
  }

  ~sp_cache()
  {
    my_hash_free(&m_hashtable);
  }

  /**
   Inserts a sp_head object into a hash table.

   @returns Success status
     @return TRUE Failure
     @return FALSE Success
  */
  bool insert(sp_head *sp)
  {
    return my_hash_insert(&m_hashtable, (const uchar *)sp);
  }

  sp_head *lookup(char *name, size_t namelen)
  {
    return (sp_head *) my_hash_search(&m_hashtable, (const uchar *)name,
                                      namelen);
  }

  void remove(sp_head *sp)
  {
    my_hash_delete(&m_hashtable, (uchar *)sp);
  }

  /**
    Remove all elements from a stored routine cache if the current
    number of elements exceeds the argument value.

    @param[in] upper_limit_for_elements  Soft upper limit of elements that
                                         can be stored in the cache.
  */
  void enforce_limit(ulong upper_limit_for_elements)
  {
    if (m_hashtable.records > upper_limit_for_elements)
      my_hash_reset(&m_hashtable);
  }

private:
  /* All routines in this cache */
  HASH m_hashtable;
}; // class sp_cache


static int64 volatile Cversion= 0;


/*
  Clear the cache *cp and set *cp to NULL.

  SYNOPSIS
    sp_cache_clear()
    cp  Pointer to cache to clear

  NOTE
    This function doesn't invalidate other caches.
*/

void sp_cache_clear(sp_cache **cp)
{
  sp_cache *c= *cp;

  if (c)
  {
    delete c;
    *cp= NULL;
  }
}


/*
  Insert a routine into the cache.

  SYNOPSIS
    sp_cache_insert()
     cp  The cache to put routine into
     sp  Routine to insert.
      
  TODO: Perhaps it will be more straightforward if in case we returned an 
        error from this function when we couldn't allocate sp_cache. (right
        now failure to put routine into cache will cause a 'SP not found'
        error to be reported at some later time)
*/

void sp_cache_insert(sp_cache **cp, sp_head *sp)
{
  sp_cache *c;

  if (!(c= *cp))
  {
    if (!(c= new sp_cache()))
      return;                                   // End of memory error
  }
  sp->set_sp_cache_version(sp_cache_version());
  DBUG_PRINT("info",("sp_cache: inserting: %.*s", (int) sp->m_qname.length,
                     sp->m_qname.str));
  c->insert(sp);
  *cp= c;                                       // Update *cp if it was NULL
}


/* 
  Look up a routine in the cache.
  SYNOPSIS
    sp_cache_lookup()
      cp    Cache to look into
      name  Name of rutine to find
      
  NOTE
    An obsolete (but not more obsolete then since last
    sp_cache_flush_obsolete call) routine may be returned.

  RETURN 
    The routine or
    NULL if the routine not found.
*/

sp_head *sp_cache_lookup(sp_cache **cp, sp_name *name)
{
  sp_cache *c= *cp;
  if (! c)
    return NULL;
  return c->lookup(name->m_qname.str, name->m_qname.length);
}


/*
  Invalidate all routines in all caches.

  SYNOPSIS
    sp_cache_invalidate()
      
  NOTE
    This is called when a VIEW definition is created or modified (and in some
    other contexts). We can't destroy sp_head objects here as one may modify
    VIEW definitions from prelocking-free SPs.
*/

void sp_cache_invalidate()
{
  DBUG_PRINT("info",("sp_cache: invalidating"));
  my_atomic_add64(&Cversion, 1);
}


/**
  Remove an out-of-date SP from the cache.

  @param[in] cp  Cache to flush
  @param[in] sp  SP to remove.

  @note This invalidates pointers to sp_head objects this thread
  uses. In practice that means 'dont call this function when
  inside SP'.
*/

void sp_cache_flush_obsolete(sp_cache **cp, sp_head **sp)
{
  if ((*sp)->sp_cache_version() < sp_cache_version() && !(*sp)->is_invoked())
  {
    (*cp)->remove(*sp);
    *sp= NULL;
  }
}


/**
  Return the current global version of the cache.
*/

int64 sp_cache_version()
{
  return my_atomic_load64(&Cversion);
}


/**
  Enforce that the current number of elements in the cache don't exceed
  the argument value by flushing the cache if necessary.

  @param[in] c  Cache to check
  @param[in] upper_limit_for_elements  Soft upper limit for number of sp_head
                                       objects that can be stored in the cache.
*/
void
sp_cache_enforce_limit(sp_cache *c, ulong upper_limit_for_elements)
{
 if (c)
   c->enforce_limit(upper_limit_for_elements);
}
 