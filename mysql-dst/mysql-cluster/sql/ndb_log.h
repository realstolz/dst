/*
   Copyright (c) 2014, 2016, Oracle and/or its affiliates. All rights reserved.

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

#ifndef NDB_LOG_H
#define NDB_LOG_H

#include <my_global.h>

// Returns the current verbose level
unsigned ndb_log_get_verbose_level(void);

/*
  Write messages to the MySQL Servers error log(s)

  NOTE! Messages will always be prefixed with "NDB:" and
  "NDB <prefix>" if one of the prefix functions are used
*/
void ndb_log_info(const char* fmt, ...)
  MY_ATTRIBUTE((format(printf, 1, 2)));

void ndb_log_warning(const char* fmt, ...)
  MY_ATTRIBUTE((format(printf, 1, 2)));

void ndb_log_error(const char* fmt, ...)
  MY_ATTRIBUTE((format(printf, 1, 2)));

void ndb_log_verbose(unsigned verbose_level, const char* fmt, ...)
  MY_ATTRIBUTE((format(printf, 2, 3)));

enum ndb_log_loglevel {
   NDB_LOG_ERROR_LEVEL=       0,
   NDB_LOG_WARNING_LEVEL=     1,
   NDB_LOG_INFORMATION_LEVEL= 2
};
void ndb_log_print(enum ndb_log_loglevel loglevel,
                   const char* prefix, const char* fmt, va_list va_args);

#endif
 