/* Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.

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

/**
  @file rules_table_service.cc
  Implementation of the service for accessing the rewrite rules table.
*/

#include <mysql/service_rules_table.h>

#include "thr_lock.h"
#include "table.h"
#include "handler.h"
#include "my_base.h"
#include "my_bitmap.h"
#include "field.h"
#include "sql_string.h"
#include "transaction.h"
#include "sql_base.h"

namespace rules_table_service
{

int MY_ATTRIBUTE((visibility("default"))) 
dummy_function_to_ensure_we_are_linked_into_the_server() { return 1; }

const char *db_name= "query_rewrite";
const char *table_name= "rewrite_rules";

int Cursor::read()
{
  TABLE *table= m_table_list->table;
  m_last_read_status= table->file->ha_rnd_next(table->record[0]);
  if (m_last_read_status != 0)
    m_is_finished= true;
  return m_last_read_status;
}


void free_string(const char *str) { delete [] str; }


static void add_column(MY_BITMAP *map, Cursor::column_id column)
{
  if (column != Cursor::ILLEGAL_COLUMN_ID)
    bitmap_set_bit(map, column);
}


Cursor::Cursor(MYSQL_THD mysql_thd) :
  m_thd(mysql_thd),
  m_table_list(NULL),
  m_is_finished(true),
  m_table_is_malformed(true)
{
  m_table_list= new TABLE_LIST;
  if (m_table_list == NULL)
    return; // Error

  // The below function will memset() the whole object with 0's.
  m_table_list->init_one_table(db_name, strlen(db_name),
                               table_name, strlen(table_name),
                               "alias", TL_WRITE_DEFAULT);

  m_table_list->updating= true;

  if (open_and_lock_tables(m_thd, m_table_list, MYSQL_LOCK_IGNORE_GLOBAL_READ_ONLY))
    return; // Error

  TABLE *table= m_table_list->table;
  if (table == NULL)
    return; // Error

  m_pattern_column= field_index("pattern");
  m_pattern_database_column= field_index("pattern_database");
  m_replacement_column= field_index("replacement");
  m_enabled_column= field_index("enabled");

  // The following columns are not required for the Cursor to work.
  m_message_column= field_index("message");
  m_pattern_digest_column= field_index("pattern_digest");
  m_normalized_pattern_column= field_index("normalized_pattern");

  if (m_pattern_column == ILLEGAL_COLUMN_ID ||
      m_pattern_database_column == ILLEGAL_COLUMN_ID ||
      m_replacement_column == ILLEGAL_COLUMN_ID ||
      m_enabled_column == ILLEGAL_COLUMN_ID)
  {
    trans_rollback_stmt(m_thd);
    close_thread_tables(m_thd);

    /*
      We are never going to use this table, we might as well get rid of the
      reference to it.
    */
    delete m_table_list;
    m_table_list= NULL;
    m_table_is_malformed= true;
    return; // Error
  }
  else
    m_table_is_malformed= false;

  add_column(table->read_set, pattern_column());
  add_column(table->read_set, pattern_database_column());
  add_column(table->read_set, replacement_column());
  add_column(table->read_set, enabled_column());
  add_column(table->read_set, message_column());

  add_column(table->write_set, enabled_column());
  add_column(table->write_set, message_column());
  add_column(table->write_set, pattern_digest_column());    
  add_column(table->write_set, normalized_pattern_column());    

  if (m_table_list->table->file->ha_rnd_init(true) != 0)
    return; // Error

  // No error occured, set this to false.
  m_is_finished= false;

  read();
}


const char *Cursor::fetch_string(int fieldno)
{
  Field **fields= m_table_list->table->field;
  Field *field= fields[fieldno];
  if (field->is_null())
    return NULL;
  String value_buf;
  String *value= field->val_str(&value_buf);
  size_t length= value->length();
  char *res= new char[length + 1];
  strncpy(res, value->ptr(), length);
  res[length]= '\0';
  return res;
}


int Cursor::field_index(const char *field_name)
{
  TABLE *table= m_table_list->table;
  for (uint i= 0; i < table->s->fields; ++i)
    if (strcmp(table->field[i]->field_name, field_name) == 0)
      return i;
  return -1;
}


void Cursor::make_writeable()
{
  TABLE *table= m_table_list->table;
  memcpy(table->record[1], table->record[0], table->s->rec_buff_length);
}


void Cursor::set(int colno, const char* str, size_t length)
{
  TABLE *table= m_table_list->table;
  Field *field= table->field[colno];

  const CHARSET_INFO *charset= &my_charset_utf8_unicode_ci;
  if (str == NULL)
    field->set_null(0);
  else
  {
    field->store(str, length, charset);
    field->set_notnull(0);
  }
}


int Cursor::write()
{
  TABLE *table= m_table_list->table;
  return table->file->ha_update_row(table->record[1], table->record[0]);
}

bool Cursor::had_serious_read_error() const
{
  return
    m_last_read_status != 0 &&
    m_last_read_status != HA_ERR_END_OF_FILE;
}

Cursor::~Cursor()
{
  if (m_table_list != NULL && m_table_list->table != NULL)
    m_table_list->table->file->ha_rnd_end();
  delete m_table_list;
}


Cursor end() { return Cursor(); }

}
 