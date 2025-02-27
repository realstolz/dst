#ifndef SQL_VIEW_INCLUDED
#define SQL_VIEW_INCLUDED

/* -*- C++ -*- */
/* Copyright (c) 2004, 2015, Oracle and/or its affiliates. All rights reserved.

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

#include "my_global.h"
#include "sql_lex.h"           // enum_view_create_mode

/* Forward declarations */

class File_parser;


/* Function declarations */

bool create_view_precheck(THD *thd, TABLE_LIST *tables, TABLE_LIST *view,
                          enum_view_create_mode mode);

bool mysql_create_view(THD *thd, TABLE_LIST *view,
                       enum_view_create_mode mode);

bool mysql_drop_view(THD *thd, TABLE_LIST *view, enum_drop_mode drop_mode);

bool check_key_in_view(THD *thd, TABLE_LIST *view, const TABLE_LIST *table_ref);

bool insert_view_fields(THD *thd, List<Item> *list, TABLE_LIST *view);

int view_checksum(THD *thd, TABLE_LIST *view);

extern TYPELIB updatable_views_with_limit_typelib;

bool check_duplicate_names(List<Item>& item_list, bool gen_unique_view_names);
bool mysql_rename_view(THD *thd, const char *new_db, const char *new_name,
                       TABLE_LIST *view);

bool open_and_read_view(THD *thd, TABLE_SHARE *share,
                        TABLE_LIST *view_ref);

bool parse_view_definition(THD *thd, TABLE_LIST *view_ref);

#define VIEW_ANY_ACL (SELECT_ACL | UPDATE_ACL | INSERT_ACL | DELETE_ACL)

extern const LEX_STRING view_type;

#endif /* SQL_VIEW_INCLUDED */
 