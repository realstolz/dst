#ifndef PERSISTED_RULE_INCLUDED
#define PERSISTED_RULE_INCLUDED
/* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

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

#include "my_config.h"
#include "mysql/service_rules_table.h"
#include "nullable.h"
#include <string>
#include <memory>

namespace rts = rules_table_service;

/**
  @file persisted_rule.h

  The facilities for easily manipulating nullable values from a
  rules_table_service::Cursor.
*/


/// A rule as persisted on disk.
class Persisted_rule
{
public:

  /// The rewrite rule's pattern string.
  Mysql::Nullable<std::string> pattern;

  /// The pattern's current database.
  Mysql::Nullable<std::string> pattern_db;

  /// The rewrite rule's replacement string.
  Mysql::Nullable<std::string> replacement;

  /// True if the rule is enabled.
  bool is_enabled;

  /// The plugin's message, write-only.
  Mysql::Nullable<std::string> message;

  /// The pattern's digest, write-only.
  Mysql::Nullable<std::string> pattern_digest;

  /// The normalized pattern, write-only.
  Mysql::Nullable<std::string> normalized_pattern;

  /**
    Constructs a Persisted_rule object that copies all data into the current
    heap. The interface is constructed this way due to on some OS'es
    (e.g. Windows), every shared library has its own heap.
  */
  explicit Persisted_rule(rts::Cursor *c)
  {
    copy_and_set(&pattern, c, c->pattern_column());
    copy_and_set(&pattern_db, c, c->pattern_database_column());
    copy_and_set(&replacement, c, c->replacement_column());

    const char *is_enabled_c= (c->fetch_string(c->enabled_column()));
    if (is_enabled_c != NULL && is_enabled_c[0] == 'Y')
      is_enabled= true;
    else
      is_enabled= false;
    rts::free_string(is_enabled_c);
  }

  /// Convenience function, may be called with a const char*.
  void set_message(const std::string &message_arg)
  {
    message= Mysql::Nullable<std::string>(message_arg);
  }


  /// Convenience function, may be called with a const char*.
  void set_pattern_digest(const std::string &s)
  {
    pattern_digest= Mysql::Nullable<std::string>(s);
  }

  /// Convenience function, may be called with a const char*.
  void set_normalized_pattern(const std::string &s)
  {
    normalized_pattern= Mysql::Nullable<std::string>(s);
  }


  /**
    Writes the values in this Persisted_rule to the table at the row pointed
    to by the cursor. Values that don't have a corresponding column in the
    table will be ignored.
  */
  bool write_to(rts::Cursor *c)
  {
    c->make_writeable();

    set_if_present(c, c->message_column(), message);
    set_if_present(c, c->pattern_digest_column(), pattern_digest);
    set_if_present(c, c->normalized_pattern_column(), normalized_pattern);

    return c->write();
  }

private:

  /**
    Reads from a Cursor and writes to a property of type Nullable<string>
    after forcing a copy of the string buffer. The function calls a member
    function in Cursor that is located in the server's dynamic library.
  */
  void copy_and_set(Mysql::Nullable<std::string> *property, rts::Cursor *c,
                    int colno)
  {
    const char *value= c->fetch_string(colno);
    if (value != NULL)
    {
      std::string tmp;
      tmp.assign(value);
      *property= tmp;
    }
    rts::free_string(value);
  }

  /// Writes a string value to the cursor's column if it exists.
  void set_if_present(rts::Cursor *cursor, rts::Cursor::column_id column,
                      Mysql::Nullable<std::string> value)
  {
    if (column == rts::Cursor::ILLEGAL_COLUMN_ID)
      return;
    if (!value.has_value())
    {
      cursor->set(column, NULL, 0);
      return;
    }
    const std::string &s= value.value();
    cursor->set(column, s.c_str(), s.length());
  }
};


#endif // PERSISTED_RULE_INCLUDED
 