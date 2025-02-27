/* Copyright (c) 2010, 2015, Oracle and/or its affiliates. All rights reserved.

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
  @file storage/perfschema/table_ets_global_by_event_name.cc
  Table EVENTS_TRANSACTIONS_SUMMARY_GLOBAL_BY_EVENT_NAME (implementation).
*/

#include "my_global.h"
#include "my_thread.h"
#include "pfs_instr_class.h"
#include "pfs_column_types.h"
#include "pfs_column_values.h"
#include "table_ets_global_by_event_name.h"
#include "pfs_global.h"
#include "pfs_instr.h"
#include "pfs_timer.h"
#include "pfs_visitor.h"
#include "field.h"

THR_LOCK table_ets_global_by_event_name::m_table_lock;

static const TABLE_FIELD_TYPE field_types[]=
{
  {
    { C_STRING_WITH_LEN("EVENT_NAME") },
    { C_STRING_WITH_LEN("varchar(128)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("COUNT_STAR") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("SUM_TIMER_WAIT") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("MIN_TIMER_WAIT") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("AVG_TIMER_WAIT") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("MAX_TIMER_WAIT") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("COUNT_READ_WRITE") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("SUM_TIMER_READ_WRITE") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("MIN_TIMER_READ_WRITE") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("AVG_TIMER_READ_WRITE") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("MAX_TIMER_READ_WRITE") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("COUNT_READ_ONLY") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("SUM_TIMER_READ_ONLY") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("MIN_TIMER_READ_ONLY") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("AVG_TIMER_READ_ONLY") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  },
  {
    { C_STRING_WITH_LEN("MAX_TIMER_READ_ONLY") },
    { C_STRING_WITH_LEN("bigint(20)") },
    { NULL, 0}
  }
};

TABLE_FIELD_DEF
table_ets_global_by_event_name::m_field_def=
{ 16, field_types };

PFS_engine_table_share
table_ets_global_by_event_name::m_share=
{
  { C_STRING_WITH_LEN("events_transactions_summary_global_by_event_name") },
  &pfs_truncatable_acl,
  table_ets_global_by_event_name::create,
  NULL, /* write_row */
  table_ets_global_by_event_name::delete_all_rows,
  table_ets_global_by_event_name::get_row_count,
  sizeof(PFS_simple_index),
  &m_table_lock,
  &m_field_def,
  false, /* checked */
  false  /* perpetual */
};

PFS_engine_table*
table_ets_global_by_event_name::create(void)
{
  return new table_ets_global_by_event_name();
}

int
table_ets_global_by_event_name::delete_all_rows(void)
{
  reset_events_transactions_by_thread();
  reset_events_transactions_by_account();
  reset_events_transactions_by_user();
  reset_events_transactions_by_host();
  reset_events_transactions_global();
  return 0;
}

ha_rows
table_ets_global_by_event_name::get_row_count(void)
{
  return transaction_class_max;
}

table_ets_global_by_event_name::table_ets_global_by_event_name()
  : PFS_engine_table(&m_share, &m_pos),
    m_row_exists(false), m_pos(1), m_next_pos(1)
{}

void table_ets_global_by_event_name::reset_position(void)
{
  m_pos= 1;
  m_next_pos= 1;
}

int table_ets_global_by_event_name::rnd_init(bool scan)
{
  m_normalizer= time_normalizer::get(transaction_timer);
  return 0;
}

int table_ets_global_by_event_name::rnd_next(void)
{
  PFS_transaction_class *transaction_class;

  m_pos.set_at(&m_next_pos);

  transaction_class= find_transaction_class(m_pos.m_index);
  if (transaction_class)
  {
    make_row(transaction_class);
    m_next_pos.set_after(&m_pos);
    return 0;
  }

  return HA_ERR_END_OF_FILE;
}

int
table_ets_global_by_event_name::rnd_pos(const void *pos)
{
  PFS_transaction_class *transaction_class;

  set_position(pos);

  transaction_class=find_transaction_class(m_pos.m_index);
  if (transaction_class)
  {
    make_row(transaction_class);
    return 0;
  }

  return HA_ERR_RECORD_DELETED;
}


void table_ets_global_by_event_name
::make_row(PFS_transaction_class *klass)
{
  m_row.m_event_name.make_row(klass);

  PFS_connection_transaction_visitor visitor(klass);
  PFS_connection_iterator::visit_global(true,  /* hosts */
                                        false, /* users */
                                        true,  /* accounts */
                                        true,  /* threads */
                                        false, /* THDs */
                                        & visitor);

  m_row.m_stat.set(m_normalizer, & visitor.m_stat);
  m_row_exists= true;
}

int table_ets_global_by_event_name
::read_row_values(TABLE *table, unsigned char *, Field **fields,
                  bool read_all)
{
  Field *f;

  if (unlikely(! m_row_exists))
    return HA_ERR_RECORD_DELETED;

  /* Set the null bits */
  DBUG_ASSERT(table->s->null_bytes == 0);

  for (; (f= *fields) ; fields++)
  {
    if (read_all || bitmap_is_set(table->read_set, f->field_index))
    {
      switch(f->field_index)
      {
      case 0: /* NAME */
        m_row.m_event_name.set_field(f);
        break;
      default:
        /**
          Columns COUNT_STAR, SUM/MIN/AVG/MAX_TIMER_WAIT,
          COUNT_READ_WRITE, SUM/MIN/AVG/MAX_TIMER_READ_WRITE,
          COUNT_READ_ONLY, SUM/MIN/AVG/MAX_TIMER_READ_ONLY
        */
        m_row.m_stat.set_field(f->field_index - 1, f);
        break;
      }
    }
  }

  return 0;
}

 