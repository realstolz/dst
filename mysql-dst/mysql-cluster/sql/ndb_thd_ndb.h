/*
   Copyright (c) 2011, 2016, Oracle and/or its affiliates. All rights reserved.

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

#ifndef NDB_THD_NDB_H
#define NDB_THD_NDB_H

#include <my_global.h>
#include <my_base.h>          // ha_rows
#include <sql_list.h>         // List<>
#include <hash.h>             // HASH

#include "ndb_share.h"

#include <kernel/ndb_limits.h> // MAX_NDB_NODES

/*
  Place holder for ha_ndbcluster thread specific data
*/

enum THD_NDB_TRANS_OPTIONS
{
  TNTO_INJECTED_APPLY_STATUS= 1 << 0
  ,TNTO_NO_LOGGING=           1 << 1
  ,TNTO_TRANSACTIONS_OFF=     1 << 2
};

class Thd_ndb 
{
  THD* const m_thd;

  Thd_ndb(THD*);
  ~Thd_ndb();
  const bool m_slave_thread; // cached value of thd->slave_thread

  uint32 options;
public:
  static Thd_ndb* seize(THD*);
  static void release(Thd_ndb* thd_ndb);

  void init_open_tables();

  class Ndb_cluster_connection *connection;
  class Ndb *ndb;
  class ha_ndbcluster *m_handler;
  ulong count;
  uint lock_count;
  uint start_stmt_count;
  uint save_point_count;
  class NdbTransaction *trans;
  bool m_error;
  bool m_slow_path;
  bool m_force_send;

  enum Options
  {
    NO_LOG_SCHEMA_OP=  1 << 0,
    /* 
      This Thd_ndb is a participant in a global schema distribution.
      Whenver a GSL lock is required, it is acquired by the coordinator.
      The participant can then assume that the GSL lock is already held
      for the schema operation it is part of. Thus it should not take
      any GSL locks itself.
    */
    IS_SCHEMA_DIST_PARTICIPANT= 1 << 1,

    /*
      Gives special priorites to this Thd_ndb, allowing it to create
      schema distribution event ops before ndb_schema_dist_is_ready()
     */
    ALLOW_BINLOG_SETUP= 1 << 2,

    /* Skip binlog setup in ndbcluster_find_files() */
    SKIP_BINLOG_SETUP_IN_FIND_FILES = 1 << 3
  };

  // Check if given option is set
  bool check_option(Options option) const;
  // Set given option
  void set_option(Options option);

  // Guard class for automatically restoring the state of
  // Thd_ndb::options when the guard goes out of scope
  class Options_guard
  {
    Thd_ndb* const m_thd_ndb;
    const uint32 m_save_options;
  public:
    Options_guard(Thd_ndb* thd_ndb)
      : m_thd_ndb(thd_ndb),
        m_save_options(thd_ndb->options)
    {
      assert(sizeof(m_save_options) == sizeof(thd_ndb->options));
    }
    ~Options_guard()
    {
      // Restore the saved options
      m_thd_ndb->options= m_save_options;
    }
    void set(Options option)
    {
      m_thd_ndb->set_option(option);
    }
  };

  uint32 trans_options;
  void transaction_checks(void);
  List<NDB_SHARE> changed_tables;
  HASH open_tables;
  /*
    This is a memroot used to buffer rows for batched execution.
    It is reset after every execute().
  */
  MEM_ROOT m_batch_mem_root;
  /*
    Estimated pending batched execution bytes, once this is > BATCH_FLUSH_SIZE
    we execute() to flush the rows buffered in m_batch_mem_root.
  */
  uint m_unsent_bytes;
  uint m_batch_size;
  bool add_row_check_if_batch_full(uint size);

  uint m_execute_count;

  uint m_scan_count;
  uint m_pruned_scan_count;

  /** This is the number of sorted scans (via ordered indexes).*/
  uint m_sorted_scan_count;

  /** This is the number of NdbQueryDef objects that the handler has created.*/
  uint m_pushed_queries_defined;
  /** 
    This is the number of cases where the handler decided not to use an 
    NdbQuery object that it previously created for executing a particular 
    instance of a query fragment. This may happen if for examle the optimizer
    decides to use another type of access operation than originally assumed.
  */
  uint m_pushed_queries_dropped;
  /**
    This is the number of times that the handler instantiated an NdbQuery object
    from a NdbQueryDef object and used the former for executing an instance
    of a query fragment.
   */
  uint m_pushed_queries_executed;
  /**
    This is the number of lookup operations (via unique index or primary key)
    that were eliminated by pushing linked operations (NdbQuery) to the data 
    nodes.
   */
  uint m_pushed_reads;

  uint m_transaction_no_hint_count[MAX_NDB_NODES];
  uint m_transaction_hint_count[MAX_NDB_NODES];

  NdbTransaction *global_schema_lock_trans;
  uint global_schema_lock_count;
  uint global_schema_lock_error;
  uint schema_locks_count; // Number of global schema locks taken by thread
  bool has_required_global_schema_lock(const char* func) const;

  /**
     Epoch of last committed transaction in this session, 0 if none so far
   */
  Uint64 m_last_commit_epoch_session;

  unsigned m_connect_count;
  bool valid_ndb(void) const;
  bool recycle_ndb(void);

  bool is_slave_thread(void) const { return m_slave_thread; }
};

#endif
 