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

#include "trigger.h"

using namespace Mysql::Tools::Dump;

Trigger::Trigger(uint64 id, const std::string& name, const std::string& schema,
  const std::string& sql_formatted_definition, const Table* defined_table)
  : Abstract_plain_sql_object_dump_task(id, name, schema,
    sql_formatted_definition),
    m_defined_table(defined_table)
{}

const Table* Trigger::get_defined_table()
{
  return m_defined_table;
}
 