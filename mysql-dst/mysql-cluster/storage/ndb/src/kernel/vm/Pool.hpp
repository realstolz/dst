/*
   Copyright (c) 2006, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef NDB_POOL_HPP
#define NDB_POOL_HPP

#include <ndb_global.h>
#include <kernel_types.h>

#define JAM_FILE_ID 315


/**
 * Type bits
 *
 * Type id is 11 bits record type, and 5 bits resource id
 *   -> 2048 different kind of records and 32 different resource groups
 * 
 * Resource id is used to handle configuration parameters
 *
 * see blocks/records_types.hpp
 */
#define RG_BITS 5
#define RG_MASK ((1 << RG_BITS) - 1)
#define MAKE_TID(TID,RG) Uint32((TID << RG_BITS) | RG)

/**
 * Page bits
 */
#define POOL_RECORD_BITS 13
#define POOL_RECORD_MASK ((1 << POOL_RECORD_BITS) - 1)

/**
 * Record_info
 *
 */
struct Record_info
{
  Uint16 m_size;
  Uint16 m_type_id;
  Uint16 m_offset_next_pool;
  Uint16 m_offset_magic;
};

/**
 * Resource_limit
 */
struct Resource_limit
{
  Uint32 m_min;
  Uint32 m_max;
  Uint32 m_curr;
  Uint32 m_spare;
  Uint32 m_resource_id;
  Uint32 m_spare_pct;
};

struct Pool_context
{
  Pool_context() {}
  class SimulatedBlock* m_block;

  /**
   * Get mem root
   */
  void* get_memroot() const;
  
  /**
   * Alloc consekutive pages
   *
   *   @param i   : out : i value of first page
   *   @return    : pointer to first page (NULL if failed)
   *
   * Will handle resource limit 
   */
  void* alloc_page19(Uint32 type_id, Uint32 *i);
  void* alloc_page27(Uint32 type_id, Uint32 *i);
  void* alloc_page30(Uint32 type_id, Uint32 *i);
  void* alloc_page32(Uint32 type_id, Uint32 *i);
  
  /**
   * Release pages
   * 
   *   @param i   : in : i value of first page
   *   @param p   : in : pointer to first page
   */
  void release_page(Uint32 type_id, Uint32 i);
  
  /**
   * Alloc consekutive pages
   *
   *   @param cnt : in/out : no of requested pages, 
   *                return no of allocated (undefined return NULL)
   *                out will never be > in
   *   @param i   : out : i value of first page
   *   @param min : in : will never allocate less than min
   *   @return    : pointer to first page (NULL if failed)
   *
   * Will handle resource limit 
   */
  void* alloc_pages(Uint32 type_id, Uint32 *i, Uint32 *cnt, Uint32 min =1);
  
  /**
   * Release pages
   * 
   *   @param i   : in : i value of first page
   *   @param p   : in : pointer to first page
   *   @param cnt : in : no of pages to release
   */
  void release_pages(Uint32 type_id, Uint32 i, Uint32 cnt);

  /**
   * Abort
   */
  void handleAbort(int code, const char* msg) const ATTRIBUTE_NORETURN;
};

template <typename T>
struct Ptr 
{
  typedef Uint32 I;
  T * p;
  Uint32 i;

  static Ptr get(T* _p, Uint32 _i) { Ptr x; x.p = _p; x.i = _i; return x; }

  /**
    Initialize to ffff.... in debug mode. The purpose of this is to detect
    use of uninitialized values by causing an error. To maximize performance,
    this is done in debug mode only (when asserts are enabled).
   */
  Ptr(){assert(memset(this, 0xff, sizeof(*this)));};
  Ptr(T* pVal, Uint32 iVal):p(pVal), i(iVal){};


  bool isNull() const 
  { 
    assert(i <= RNIL);
    return i == RNIL; 
  }

  inline void setNull()
  {
    i = RNIL;
  }
};

template <typename T>
struct ConstPtr 
{
  const T * p;
  Uint32 i;

  static ConstPtr get(T const* _p, Uint32 _i) { ConstPtr x; x.p = _p; x.i = _i; return x; }

  /**
    Initialize to ffff.... in debug mode. The purpose of this is to detect
    use of uninitialized values by causing an error. To maximize performance,
    this is done in debug mode only (when asserts are enabled).
   */
  ConstPtr(){assert(memset(this, 0xff, sizeof(*this)));};
  ConstPtr(T* pVal, Uint32 iVal):p(pVal), i(iVal){};

  bool isNull() const 
  { 
    assert(i <= RNIL);
    return i == RNIL; 
  }

  inline void setNull()
  {
    i = RNIL;
  }
};

#ifdef XX_DOCUMENTATION_XX
/**
 * Any pool should implement the following
 */
struct PoolImpl
{
  Pool_context m_ctx;
  Record_info m_record_info;
  
  void init(const Record_info& ri, const Pool_context& pc);
  void init(const Record_info& ri, const Pool_context& pc);
  
  bool seize(Ptr<void>&);
  void release(Ptr<void>);
  void * getPtr(Uint32 i) const;
};
#endif

struct ArenaHead; // forward decl.
class ArenaAllocator; // forward decl.

template <typename P, typename T = typename P::Type>
class RecordPool {
public:
  typedef T Type;
  RecordPool();
  ~RecordPool();
  
  void init(Uint32 type_id, const Pool_context& pc);
  void wo_pool_init(Uint32 type_id, const Pool_context& pc);
  void arena_pool_init(ArenaAllocator*, Uint32 type_id, const Pool_context& pc);
  
  /**
   * Update p value for ptr according to i value 
   */
  void getPtr(Ptr<T> &) const;
  void getPtr(ConstPtr<T> &) const;
  
  /**
   * Get pointer for i value
   */
  T * getPtr(Uint32 i) const;
  const T * getConstPtr(Uint32 i) const;

  /**
   * Update p & i value for ptr according to <b>i</b> value 
   */
  void getPtr(Ptr<T> &, Uint32 i) const;
  void getPtr(ConstPtr<T> &, Uint32 i) const;

  /**
   * Allocate an object from pool - update Ptr
   *
   * Return i
   */
  bool seize(Ptr<T> &);

  /**
   * Allocate object from arena - update Ptr
   */
  bool seize(ArenaHead&, Ptr<T>&);

  /**
   * Return an object to pool
   */
  void release(Uint32 i);

  /**
   * Return an object to pool
   */
  void release(Ptr<T>);
private:
  P m_pool;
};

template <typename P, typename T>
inline
RecordPool<P, T>::RecordPool()
{
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::init(Uint32 type_id, const Pool_context& pc)
{
  T tmp;
  const char * off_base = (char*)&tmp;
  const char * off_next = (char*)&tmp.nextPool;
  const char * off_magic = (char*)&tmp.m_magic;

  Record_info ri;
  ri.m_size = sizeof(T);
  ri.m_offset_next_pool = Uint32(off_next - off_base);
  ri.m_offset_magic = Uint32(off_magic - off_base);
  ri.m_type_id = type_id;
  m_pool.init(ri, pc);
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::wo_pool_init(Uint32 type_id, const Pool_context& pc)
{
  T tmp;
  const char * off_base = (char*)&tmp;
  const char * off_magic = (char*)&tmp.m_magic;
  
  Record_info ri;
  ri.m_size = sizeof(T);
  ri.m_offset_next_pool = 0;
  ri.m_offset_magic = Uint32(off_magic - off_base);
  ri.m_type_id = type_id;
  m_pool.init(ri, pc);
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::arena_pool_init(ArenaAllocator* alloc,
                                  Uint32 type_id, const Pool_context& pc)
{
  T tmp;
  const char * off_base = (char*)&tmp;
  const char * off_next = (char*)&tmp.nextPool;
  const char * off_magic = (char*)&tmp.m_magic;

  Record_info ri;
  ri.m_size = sizeof(T);
  ri.m_offset_next_pool = Uint32(off_next - off_base);
  ri.m_offset_magic = Uint32(off_magic - off_base);
  ri.m_type_id = type_id;
  m_pool.init(alloc, ri, pc);
}


template <typename P, typename T>
inline
RecordPool<P, T>::~RecordPool()
{
}

  
template <typename P, typename T>
inline
void
RecordPool<P, T>::getPtr(Ptr<T> & ptr) const
{
  ptr.p = static_cast<T*>(m_pool.getPtr(ptr.i));
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::getPtr(ConstPtr<T> & ptr) const 
{
  ptr.p = static_cast<const T*>(m_pool.getPtr(ptr.i));
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::getPtr(Ptr<T> & ptr, Uint32 i) const
{
  ptr.i = i;
  ptr.p = static_cast<T*>(m_pool.getPtr(ptr.i));  
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::getPtr(ConstPtr<T> & ptr, Uint32 i) const 
{
  ptr.i = i;
  ptr.p = static_cast<const T*>(m_pool.getPtr(ptr.i));  
}
  
template <typename P, typename T>
inline
T * 
RecordPool<P, T>::getPtr(Uint32 i) const
{
  return static_cast<T*>(m_pool.getPtr(i));  
}

template <typename P, typename T>
inline
const T * 
RecordPool<P, T>::getConstPtr(Uint32 i) const 
{
  return static_cast<const T*>(m_pool.getPtr(i)); 
}
  
template <typename P, typename T>
inline
bool
RecordPool<P, T>::seize(Ptr<T> & ptr)
{
  Ptr<T> tmp;
  bool ret = m_pool.seize(tmp);
  if(likely(ret))
  {
    ptr.i = tmp.i;
    ptr.p = static_cast<T*>(tmp.p);
  }
  return ret;
}

template <typename P, typename T>
inline
bool
RecordPool<P, T>::seize(ArenaHead & ah, Ptr<T> & ptr)
{
  Ptr<T> tmp;
  bool ret = m_pool.seize(ah, tmp);
  if(likely(ret))
  {
    ptr.i = tmp.i;
    ptr.p = static_cast<T*>(tmp.p);
  }
  return ret;
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::release(Uint32 i)
{
  Ptr<T> ptr;
  ptr.i = i;
  ptr.p = m_pool.getPtr(i);
  m_pool.release(ptr);
}

template <typename P, typename T>
inline
void
RecordPool<P, T>::release(Ptr<T> ptr)
{
  Ptr<T> tmp;
  tmp.i = ptr.i;
  tmp.p = ptr.p;
  m_pool.release(tmp);
}


#undef JAM_FILE_ID

#endif
 