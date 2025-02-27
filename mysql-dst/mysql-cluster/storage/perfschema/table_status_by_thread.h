/* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef TABLE_STATUS_BY_THREAD_H
#define TABLE_STATUS_BY_THREAD_H

/**
  @file storage/perfschema/table_status_by_thread.h
  Table STATUS_BY_THREAD (declarations).
*/

#include "pfs_column_types.h"
#include "pfs_engine_table.h"
#include "pfs_instr_class.h"
#include "pfs_instr.h"
#include "table_helper.h"
#include "pfs_variable.h"
#include "pfs_buffer_container.h"
/**
  @addtogroup Performance_schema_tables
  @{
*/

/**
  A row of table
  PERFORMANCE_SCHEMA.STATUS_BY_THREAD.
*/
struct row_status_by_thread
{
  /** Column THREAD_ID. */
  ulonglong m_thread_internal_id;
  /** Column VARIABLE_NAME. */
  PFS_variable_name_row m_variable_name;
  /** Column VARIABLE_VALUE. */
  PFS_variable_value_row m_variable_value;
};

/**
  Position of a cursor on
  PERFORMANCE_SCHEMA.STATUS_BY_THREAD.
  Index 1 on thread (0 based)
  Index 2 on status variable (0 based)
*/
struct pos_status_by_thread
: public PFS_double_index
{
  pos_status_by_thread()
    : PFS_double_index(0, 0)
  {}

  inline void reset(void)
  {
    m_index_1= 0;
    m_index_2= 0;
  }

  inline bool has_more_thread(void)
  { return (m_index_1 < global_thread_container.get_row_count()); }

  inline void next_thread(void)
  {
    m_index_1++;
    m_index_2= 0;
  }
};

/**
  Store and retrieve table state information for queries that reinstantiate
  the table object.
*/
class table_status_by_thread_context : public PFS_table_context
{
public:
  table_status_by_thread_context(ulonglong current_version, bool restore) :
    PFS_table_context(current_version, global_thread_container.get_row_count(), restore, THR_PFS_SBT) { }
};

/** Table PERFORMANCE_SCHEMA.STATUS_BY_THREAD. */
class table_status_by_thread : public PFS_engine_table
{
  typedef pos_status_by_thread pos_t;

public:
  /** Table share */
  static PFS_engine_table_share m_share;
  static PFS_engine_table* create();
  static int delete_all_rows();
  static ha_rows get_row_count();

  virtual int rnd_init(bool scan);
  virtual int rnd_next();
  virtual int rnd_pos(const void *pos);
  virtual void reset_position(void);

protected:
  virtual int read_row_values(TABLE *table,
                              unsigned char *buf,
                              Field **fields,
                              bool read_all);
  table_status_by_thread();

public:
  ~table_status_by_thread()
  {}

protected:
  int materialize(PFS_thread *thread);
  void make_row(PFS_thread *thread, const Status_variable *status_var);

private:
  /** Table share lock. */
  static THR_LOCK m_table_lock;
  /** Fields definition. */
  static TABLE_FIELD_DEF m_field_def;

  /** Current THD variables. */
  PFS_status_variable_cache m_status_cache;
  /** Current row. */
  row_status_by_thread m_row;
  /** True if the current row exists. */
  bool m_row_exists;
  /** Current position. */
  pos_t m_pos;
  /** Next position. */
  pos_t m_next_pos;

  /** Table context with global status array version and map of materialized threads. */
  table_status_by_thread_context *m_context;
};

/** @} */
#endif
 