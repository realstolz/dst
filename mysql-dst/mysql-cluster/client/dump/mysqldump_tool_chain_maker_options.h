/*
  Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYSQLDUMP_TOOL_CHAIN_MAKER_OPTIONS_INCLUDED
#define MYSQLDUMP_TOOL_CHAIN_MAKER_OPTIONS_INCLUDED

#include "abstract_data_object.h"
#include "sql_formatter_options.h"
#include "mysql_object_reader_options.h"
#include "object_filter.h"

namespace Mysql{
namespace Tools{
namespace Dump{

class Mysqldump_tool_chain_maker_options
  : public Mysql::Tools::Base::Options::Composite_options_provider
{
public:
  Mysqldump_tool_chain_maker_options(
    const Mysql_chain_element_options* mysql_chain_element_options);

  ~Mysqldump_tool_chain_maker_options();

  void create_options();

  void process_positional_options(std::vector<std::string> positional_options);

  int get_object_queue_id_for_schema(const std::string& schema);

  int get_object_queue_threads_count(int object_queue_id);

  bool is_object_included_in_dump(Abstract_data_object* object);

  int get_parallel_schemas_with_default_thread_count();
  int get_parallel_schemas_thread_count();

  const Mysql_chain_element_options* m_mysql_chain_element_options;
  Sql_formatter_options* m_formatter_options;
  Mysql_object_reader_options* m_object_reader_options;

  bool m_dump_all_databases;
  bool m_dump_selected_databases;
  uint32 m_default_parallelism;
  Mysql::Nullable<std::string> m_result_file;
  Mysql::Nullable<std::string> m_compress_output_algorithm;
  bool m_skip_rows_data;

private:
  void parallel_schemas_callback(char*);

  /**
    Specifies number of threads used by given queue. 0 is set when queue is to
    use --default-parallelism value.
   */
  std::map<int, int> m_object_queue_threads;
  std::map<std::string, int> m_database_to_object_queue_id;
  Mysql::Nullable<std::string> m_parallel_schemas_string;
  int m_last_parallel_schemas_queue_id;
  /* count of parallel-schemas queues with default parallelism */
  int m_def_thread_count;
  /* thread count of parallel-schemas with threads specified */
  int m_parallel_thread_count;
  Object_filter m_object_filter;
};

}
}
}

#endif
 