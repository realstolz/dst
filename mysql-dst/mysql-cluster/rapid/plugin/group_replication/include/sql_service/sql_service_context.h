/* Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.

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

#ifndef SQL_SERVICE_CONTEXT_INCLUDE
#define SQL_SERVICE_CONTEXT_INCLUDE

#include "sql_service_context_base.h"

class Sql_service_context : public Sql_service_context_base
{
public:
  Sql_service_context(Sql_resultset *rset)
    :resultset(rset)
  {
    if (rset != NULL)
      resultset->clear();
  }

  ~Sql_service_context() {}


  /** Getting metadata **/
  /**
    Indicates start of metadata for the result set

    @param num_cols Number of fields being sent
    @param flags    Flags to alter the metadata sending
    @param resultcs Charset of the result set

    @return
      @retval 1  Error
      @retval 0  OK
  */
  int start_result_metadata(uint num_cols, uint flags,
                            const CHARSET_INFO *resultcs);

  /**
    Field metadata is provided via this callback

    @param field   Field's metadata (see field.h)
    @param charset Field's charset

    @return
      @retval 1  Error
      @retval 0  OK
  */
  int field_metadata(struct st_send_field *field,
                     const CHARSET_INFO *charset);

  /**
    Indicates end of metadata for the result set

    @param server_status   Status of server (see mysql_com.h SERVER_STATUS_*)
    @param warn_count      Number of warnings thrown during execution

    @return
      @retval 1  Error
      @retval 0  OK
  */
  int end_result_metadata(uint server_status,
                          uint warn_count);

  /**
    Indicates the beginning of a new row in the result set/metadata

    @return
      @retval 1  Error
      @retval 0  OK
  */
  int start_row();

  /**
    Indicates end of the row in the result set/metadata

    @return
      @retval 1  Error
      @retval 0  OK
  */
  int end_row();

  /**
    An error occured during execution

    @details This callback indicates that an error occureded during command
    execution and the partial row should be dropped. Server will raise error
    and return.
  */
  void abort_row();

  /**
    Return client's capabilities (see mysql_com.h, CLIENT_*)

    @return Bitmap of client's capabilities
  */
  ulong get_client_capabilities();

  /** Getting data **/
  /**
    Receive NULL value from server

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_null();

  /**
    Get TINY/SHORT/LONG value from server

    @param value         Value received

    @note In order to know which type exactly was received, the plugin must
    track the metadata that was sent just prior to the result set.

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_integer(longlong value);

  /**
    Get LONGLONG value from server

    @param value         Value received
    @param is_unsigned   TRUE <=> value is unsigned

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_longlong(longlong value, uint is_unsigned);

  /**
    Receive DECIMAL value from server

    @param value Value received

    @return status
      @retval 1  Error
      @retval 0  OK
   */
  int get_decimal(const decimal_t * value);

  /**
    Receive DOUBLE value from server

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_double(double value, uint32 decimals);

  /**
    Get DATE value from server

    @param value    Value received

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_date(const MYSQL_TIME * value);

  /**
    Get TIME value from server

    @param value    Value received
    @param decimals Number of decimals

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_time(const MYSQL_TIME * value, uint decimals);

  /**
    Get DATETIME value from server

    @param value    Value received
    @param decimals Number of decimals

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_datetime(const MYSQL_TIME * value,
                   uint decimals);

  /**
    Get STRING value from server

    @param value   Value received
    @param length  Value's length
    @param valuecs Value's charset

    @return status
      @retval 1  Error
      @retval 0  OK
  */
  int get_string(const char * const value,
                 size_t length, const CHARSET_INFO * const valuecs);

  /** Getting execution status **/
  /**
    Command ended with success

    @param server_status        Status of server (see mysql_com.h,
                                SERVER_STATUS_*)
    @param statement_warn_count Number of warnings thrown during execution
    @param affected_rows        Number of rows affected by the command
    @param last_insert_id       Last insert id being assigned during execution
    @param message              A message from server
  */
  void handle_ok(uint server_status, uint statement_warn_count,
                 ulonglong affected_rows, ulonglong last_insert_id,
                 const char * const message);

  /**
    Command ended with ERROR

    @param sql_errno Error code
    @param err_msg   Error message
    @param sqlstate  SQL state correspongin to the error code
  */
  void handle_error(uint sql_errno,
                    const char * const err_msg,
                    const char * const sqlstate);

  /**
    Session was shutdown while command was running
  */
  void shutdown(int flag);

private:
  /* executed command result store */
  Sql_resultset *resultset;
};

#endif //SQL_SERVICE_CONTEXT_INCLUDE
 