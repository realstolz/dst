#ifndef ITEM_ROW_INCLUDED
#define ITEM_ROW_INCLUDED

/* Copyright (c) 2002, 2015, Oracle and/or its affiliates. All rights reserved.

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

#include "item.h"  // Item

/**
   Item which stores (x,y,...) and ROW(x,y,...).
   Note that this can be recursive: ((x,y),(z,t)) is a ROW of ROWs.
*/
class Item_row: public Item
{
  typedef Item super;

  Item **items;
  table_map used_tables_cache, not_null_tables_cache;
  uint arg_count;
  bool const_item_cache;
  /**
     If elements are made only of constants, of which one or more are
     NULL. For example, this item is (1,2,NULL), or ( (1,NULL), (2,3) ).
  */
  bool with_null;
public:
  /**
    Row items used for comparing rows and IN operations on rows:

    @param pos    current parse context
    @param head   first column in the row
    @param tail   rest of columns in the row

    @verbatim
    (a, b, c) > (10, 10, 30)
    (a, b, c) = (select c, d, e, from t1 where x=12)
    (a, b, c) IN ((1,2,2), (3,4,5), (6,7,8)
    (a, b, c) IN (select c, d, e, from t1)
    @endverbatim

    @todo
      think placing 2-3 component items in item (as it done for function
  */
  Item_row(const POS &pos, Item *head, List<Item> &tail);
  Item_row(Item *head, List<Item> &tail);
  Item_row(Item_row *item):
    Item(),
    items(item->items),
    used_tables_cache(item->used_tables_cache),
    not_null_tables_cache(0),
    arg_count(item->arg_count),
    const_item_cache(item->const_item_cache),
    with_null(0)
  {}

  virtual bool itemize(Parse_context *pc, Item **res);

  enum Type type() const { return ROW_ITEM; };
  void illegal_method_call(const char *);
  bool is_null() { return null_value; }
  void make_field(Send_field *)
  {
    illegal_method_call((const char*)"make_field");
  };
  double val_real()
  {
    illegal_method_call((const char*)"val");
    return 0;
  };
  longlong val_int()
  {
    illegal_method_call((const char*)"val_int");
    return 0;
  };
  String *val_str(String *)
  {
    illegal_method_call((const char*)"val_str");
    return 0;
  };
  my_decimal *val_decimal(my_decimal *)
  {
    illegal_method_call((const char*)"val_decimal");
    return 0;
  };
  bool get_date(MYSQL_TIME *ltime, my_time_flags_t fuzzydate)
  {
    illegal_method_call((const char *) "get_date");
    return true;
  }
  bool get_time(MYSQL_TIME *ltime)
  {
    illegal_method_call((const char *) "get_time");
    return true;
  }

  bool fix_fields(THD *thd, Item **ref);
  void fix_after_pullout(st_select_lex *parent_select,
                         st_select_lex *removed_select);
  void cleanup();
  void split_sum_func(THD *thd, Ref_ptr_array ref_pointer_array,
                      List<Item> &fields);
  table_map used_tables() const { return used_tables_cache; };
  bool const_item() const { return const_item_cache; };
  enum Item_result result_type() const { return ROW_RESULT; }
  void update_used_tables();
  table_map not_null_tables() const { return not_null_tables_cache; }
  virtual void print(String *str, enum_query_type query_type);

  bool walk(Item_processor processor, enum_walk walk, uchar *arg);
  Item *transform(Item_transformer transformer, uchar *arg);

  uint cols() { return arg_count; }
  Item* element_index(uint i) { return items[i]; }
  Item** addr(uint i) { return items + i; }
  bool check_cols(uint c);
  bool null_inside() { return with_null; };
  void bring_value();
  bool check_gcol_func_processor(uchar *int_arg) {return false; }
};

#endif /* ITEM_ROW_INCLUDED */
 