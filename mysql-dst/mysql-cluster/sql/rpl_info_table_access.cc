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

#include "rpl_info_table_access.h"

#include "rpl_info_values.h" // Rpl_info_values
#include "sql_base.h"       // MYSQL_OPEN_IGNORE_FLUSH
#include "sql_class.h"      // THD
#include "sql_parse.h"      // mysql_reset_thd_for_next_command
#include "table.h"          // TABLE


void Rpl_info_table_access::before_open(THD *thd)
{
  DBUG_ENTER("Rpl_info_table_access::before_open");

  m_flags= (MYSQL_OPEN_IGNORE_GLOBAL_READ_LOCK |
            MYSQL_LOCK_IGNORE_GLOBAL_READ_ONLY |
            MYSQL_OPEN_IGNORE_FLUSH |
            MYSQL_LOCK_IGNORE_TIMEOUT |
            MYSQL_LOCK_RPL_INFO_TABLE);

  /*
    This is equivalent to a new "statement". For that reason, we call both
    lex_start() and mysql_reset_thd_for_next_command.
  */
  if (thd->slave_thread || !current_thd)
  {
    lex_start(thd);
    mysql_reset_thd_for_next_command(thd);
  }

  DBUG_VOID_RETURN;
}


/**
  Commits the changes, unlocks the table and closes it. This method
  needs to be called even if the open_table fails, in order to ensure
  the lock info is properly restored.

  @param[in] thd    Thread requesting to close the table
  @param[in] table  Table to be closed
  @param[in] backup Restore the lock info from here
  @param[in] error  If there was an error while updating
                    the table

  If there is an error, rolls back the current statement. Otherwise,
  commits it. However, if a new thread was created and there is an
  error, the transaction must be rolled back. Otherwise, it must be
  committed. In this case, the changes were not done on behalf of
  any user transaction and if not finished, there would be pending
  changes.
*/
void Rpl_info_table_access::close_table(THD *thd, TABLE* table,
                                        Open_tables_backup *backup,
                                        bool error)
{
  DBUG_ENTER("Rpl_info_table_access::close_table");
  System_table_access::close_table(thd, table, backup, error, thd_created);
  DBUG_VOID_RETURN;
}

/**
  Positions the internal pointer of `table` according to the primary
  key.

  If the search succeeds, the table cursor points to the found row.

  @param[in,out]  field_values The sequence of values
  @param[in,out]  table        Table

  @return
    @retval FOUND     The row was found.
    @retval NOT_FOUND The row was not found.
    @retval ERROR     There was a failure.
*/
enum enum_return_id Rpl_info_table_access::find_info(Rpl_info_values *field_values,
                                                     TABLE *table)
{
  KEY* keyinfo= NULL;
  uchar key[MAX_KEY_LENGTH];

  DBUG_ENTER("Rpl_info_table_access::find_info");

  /*
    Checks if the table has a primary key as expected.
  */
  if (table->s->primary_key >= MAX_KEY ||
      !table->s->keys_in_use.is_set(table->s->primary_key))
  {
    /*
      This is not supposed to happen and means that someone
      has changed the table or disabled the keys.
    */
    DBUG_RETURN(ERROR_ID);
  }

  keyinfo= table->s->key_info + table->s->primary_key;
  for (uint idx= 0; idx < keyinfo->user_defined_key_parts; idx++)
  {
    uint fieldnr= keyinfo->key_part[idx].fieldnr - 1;

    /*
      The size of the field must be great to store data.
    */
    if (field_values->value[fieldnr].length() >
        table->field[fieldnr]->field_length)
      DBUG_RETURN(ERROR_ID);

    table->field[fieldnr]->store(field_values->value[fieldnr].c_ptr_safe(),
                                 field_values->value[fieldnr].length(),
                                 &my_charset_bin);
  }
  key_copy(key, table->record[0], table->key_info, table->key_info->key_length);

  if (table->file->ha_index_read_idx_map(table->record[0], 0, key, HA_WHOLE_KEY,
                                         HA_READ_KEY_EXACT))
    DBUG_RETURN(NOT_FOUND_ID);

  DBUG_RETURN(FOUND_ID);
}

/**
  Positions the internal pointer of `table` to the n-instance row.

  @param[in]  table Reference to a table object.
  @param[in]  instance n-instance row.

  The code built on top of this function needs to ensure there is
  no concurrent threads trying to update the table. So if an error
  different from HA_ERR_END_OF_FILE is returned, we abort with an
  error because this implies that someone has manualy and
  concurrently changed something.

  @return
    @retval FOUND     The row was found.
    @retval NOT_FOUND The row was not found.
    @retval ERROR     There was a failure.
*/
enum enum_return_id Rpl_info_table_access::scan_info(TABLE* table,
                                                     uint instance)
{
  int error= 0;
  uint counter= 0;
  enum enum_return_id ret= NOT_FOUND_ID;

  DBUG_ENTER("Rpl_info_table_access::scan_info");

  if ((error= table->file->ha_rnd_init(TRUE)))
    DBUG_RETURN(ERROR_ID);

  do
  {
    error= table->file->ha_rnd_next(table->record[0]);
    switch (error)
    {
      case 0:
        counter++;
        if (counter == instance)
        {
          ret= FOUND_ID;
          error= HA_ERR_END_OF_FILE;
        }
      break;

      case HA_ERR_END_OF_FILE:
        ret= NOT_FOUND_ID;
      break;

      default:
        DBUG_PRINT("info", ("Failed to get next record"
                            " (ha_rnd_next returns %d)", error));
        ret= ERROR_ID;
      break;
    }
  }
  while (!error);

  table->file->ha_rnd_end();

  DBUG_RETURN(ret);
}

/**
  Returns the number of entries in table.

  The code built on top of this function needs to ensure there is
  no concurrent threads trying to update the table. So if an error
  different from HA_ERR_END_OF_FILE is returned, we abort with an
  error because this implies that someone has manualy and
  concurrently changed something.

  @param[in]  table   Table
  @param[out] counter Registers the number of entries.

  @return
    @retval false No error
    @retval true  Failure
*/
bool Rpl_info_table_access::count_info(TABLE* table, uint* counter)
{
  bool end= false;
  int error= 0;

  DBUG_ENTER("Rpl_info_table_access::count_info");

  if ((error= table->file->ha_rnd_init(true)))
    DBUG_RETURN(true);

  do
  {
    error= table->file->ha_rnd_next(table->record[0]);
    switch (error) 
    {
      case 0:
        (*counter)++;
      break;

      case HA_ERR_END_OF_FILE:
        end= true;
      break;

      default:
        DBUG_PRINT("info", ("Failed to get next record"
                            " (ha_rnd_next returns %d)", error));
      break;
    }
  }
  while (!error);

  table->file->ha_rnd_end();

  DBUG_RETURN(end ? false : true);
}

/**
  Reads information from a sequence of fields into a set of LEX_STRING
  structures, where the sequence of values is specified through the object
  Rpl_info_values.

  @param[in] max_num_field Maximum number of fields
  @param[in] fields        The sequence of fields
  @param[in] field_values  The sequence of values

  @return
    @retval FALSE No error
    @retval TRUE  Failure
*/
bool Rpl_info_table_access::load_info_values(uint max_num_field, Field **fields,
                                             Rpl_info_values *field_values)
{
  DBUG_ENTER("Rpl_info_table_access::load_info_values");
  char buff[MAX_FIELD_WIDTH];
  String str(buff, sizeof(buff), &my_charset_bin);

  uint field_idx= 0;
  while (field_idx < max_num_field)
  {
    fields[field_idx]->val_str(&str);
    field_values->value[field_idx].copy(str.c_ptr_safe(), str.length(),
                                        &my_charset_bin);
    field_idx++;
  }

  DBUG_RETURN(FALSE);
}

/**
  Stores information from a sequence of fields into a set of LEX_STRING
  structures, where the sequence of values is specified through the object
  Rpl_info_values.

  @param[in] max_num_field Maximum number of fields
  @param[in] fields        The sequence of fields
  @param[in] field_values  The sequence of values

  @return
    @retval FALSE No error
    @retval TRUE  Failure
 */
bool Rpl_info_table_access::store_info_values(uint max_num_field, Field **fields,
                                              Rpl_info_values *field_values)
{
  DBUG_ENTER("Rpl_info_table_access::store_info_values");
  uint field_idx= 0;

  while (field_idx < max_num_field)
  {
    fields[field_idx]->set_notnull();

    if (fields[field_idx]->store(field_values->value[field_idx].c_ptr_safe(),
                                 field_values->value[field_idx].length(),
                                 &my_charset_bin))
    {
      my_error(ER_RPL_INFO_DATA_TOO_LONG, MYF(0),
               fields[field_idx]->field_name);
      DBUG_RETURN(TRUE);
    }
    field_idx++;
  }

  DBUG_RETURN(FALSE);
}

/**
  Creates a new thread if necessary. In the bootstrap process or in
  the mysqld startup, a thread is created in order to be able to
  access a table. Otherwise, the current thread is used.

  @return
    @retval THD* Pointer to thread structure
*/
THD *Rpl_info_table_access::create_thd()
{
  THD *thd= current_thd;

  if (!thd)
  {
    thd= System_table_access::create_thd();
    thd->system_thread= SYSTEM_THREAD_INFO_REPOSITORY;
    thd_created= true;
  }

  return(thd);
}

/**
  Destroys the created thread if necessary and restores the
  system_thread information.

  @param[in] thd Thread requesting to be destroyed
*/
void Rpl_info_table_access::drop_thd(THD *thd)
{
  DBUG_ENTER("Rpl_info::drop_thd");

  if (thd_created)
  {
    System_table_access::drop_thd(thd);
    thd_created= false;
  }

  DBUG_VOID_RETURN;
}
 