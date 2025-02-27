/* Copyright (c) 2011, 2015, Oracle and/or its affiliates. All rights reserved.

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

#include "opt_explain_traditional.h"

/**
  Heads of "extra" column parts

  This array must be in sync with Extra_tag enum.
*/
static const char *traditional_extra_tags[ET_total]=
{
  NULL,                                // ET_none
  "Using temporary",                   // ET_USING_TEMPORARY
  "Using filesort",                    // ET_USING_FILESORT
  "Using index condition",             // ET_USING_INDEX_CONDITION
  "Using",                             // ET_USING
  "Range checked for each record",     // ET_RANGE_CHECKED_FOR_EACH_RECORD
  "Using where with pushed condition", // ET_USING_WHERE_WITH_PUSHED_CONDITION
  "Using where",                       // ET_USING_WHERE
  "Not exists",                        // ET_NOT_EXISTS
  "Using MRR",                         // ET_USING_MRR
  "Using index",                       // ET_USING_INDEX
  "Full scan on NULL key",             // ET_FULL_SCAN_ON_NULL_KEY
  "Skip_open_table",                   // ET_SKIP_OPEN_TABLE
  "Open_frm_only",                     // ET_OPEN_FRM_ONLY
  "Open_full_table",                   // ET_OPEN_FULL_TABLE
  "Scanned",                           // ET_SCANNED_DATABASES
  "Using index for group-by",          // ET_USING_INDEX_FOR_GROUP_BY
  "Distinct",                          // ET_DISTINCT
  "LooseScan",                         // ET_LOOSESCAN
  "Start temporary",                   // ET_START_TEMPORARY
  "End temporary",                     // ET_END_TEMPORARY
  "FirstMatch",                        // ET_FIRST_MATCH
  "Materialize",                       // ET_MATERIALIZE
  "Start materialize",                 // ET_START_MATERIALIZE
  "End materialize",                   // ET_END_MATERIALIZE
  "Scan",                              // ET_SCAN
  "Using join buffer",                 // ET_USING_JOIN_BUFFER 
  "const row not found",               // ET_CONST_ROW_NOT_FOUND
  "unique row not found",              // ET_UNIQUE_ROW_NOT_FOUND
  "Impossible ON condition",           // ET_IMPOSSIBLE_ON_CONDITION
  "",                                  // ET_PUSHED_JOIN
  "Ft_hints:"                          // ET_FT_HINTS
};

static const char *mod_type_name[]=
{
  "NONE", "INSERT", "UPDATE", "DELETE", "REPLACE"
};

bool Explain_format_traditional::send_headers(Query_result *result)
{
  return ((nil= new Item_null) == NULL ||
          Explain_format::send_headers(result) ||
          current_thd->send_explain_fields(output));
}

static bool push(List<Item> *items, qep_row::mem_root_str &s,
                 Item_null *nil)
{
  if (s.is_empty())
    return items->push_back(nil);
  Item_string *item= new Item_string(s.str, s.length, system_charset_info);
  return item == NULL || items->push_back(item);
}


static bool push(List<Item> *items, const char *s, size_t length)
{
  Item_string *item= new Item_string(s, length, system_charset_info);
  return item == NULL || items->push_back(item);
}

static bool push(List<Item> *items, List<const char> &c, Item_null *nil)
{
  if (c.is_empty())
    return items->push_back(nil);
    
  StringBuffer<1024> buff;
  List_iterator<const char> it(c);
  const char *s;
  while((s= it++))
  {
    buff.append(s);
    buff.append(",");
  }
  if (!buff.is_empty())
    buff.length(buff.length() - 1); // remove last ","
  Item_string *item= new Item_string(buff.dup(current_thd->mem_root),
                                     buff.length(), system_charset_info);
  return item == NULL || items->push_back(item);
}


static bool push(List<Item> *items, const qep_row::column<uint> &c,
                 Item_null *nil)
{
  if (c.is_empty())
    return items->push_back(nil);
  Item_uint *item= new Item_uint(c.get());
  return item == NULL || items->push_back(item);
}


static bool push(List<Item> *items, const qep_row::column<ulonglong> &c,
                 Item_null *nil)
{
  if (c.is_empty())
    return items->push_back(nil);
  Item_int *item= new Item_int(c.get(), MY_INT64_NUM_DECIMAL_DIGITS);
  return item == NULL || items->push_back(item);
}


static bool push(List<Item> *items, const qep_row::column<float> &c,
                 Item_null *nil)
{
  if (c.is_empty())
    return items->push_back(nil);
  Item_float *item= new Item_float(c.get(), 2);
  return item == NULL || items->push_back(item);
}


bool Explain_format_traditional::push_select_type(List<Item> *items)
{
  DBUG_ASSERT(!column_buffer.col_select_type.is_empty());
  StringBuffer<32> buff;
  if (column_buffer.is_dependent)
  {
    if (buff.append(STRING_WITH_LEN("DEPENDENT "), system_charset_info))
      return true;
  }
  else if (!column_buffer.is_cacheable)
  {
    if (buff.append(STRING_WITH_LEN("UNCACHEABLE "), system_charset_info))
      return true;
  }
  const SELECT_LEX::type_enum sel_type= column_buffer.col_select_type.get();
  const char *type= (column_buffer.mod_type != MT_NONE &&
                     (sel_type == SELECT_LEX::SLT_PRIMARY ||
                      sel_type == SELECT_LEX::SLT_SIMPLE)) ?
    mod_type_name[column_buffer.mod_type] :
    SELECT_LEX::get_type_str(sel_type);

  if (buff.append(type))
    return true;

  Item_string *item= new Item_string(buff.dup(current_thd->mem_root),
                                     buff.length(), system_charset_info);
  return item == NULL || items->push_back(item);
}

class Buffer_cleanup
{
public:
  explicit Buffer_cleanup(qep_row *row)
    : m_row(row)
  {}
  ~Buffer_cleanup()
  {
    m_row->cleanup();
  }
private:
  qep_row *m_row;
};

bool Explain_format_traditional::flush_entry()
{
  Buffer_cleanup bc(&column_buffer); // release column_buffer
  List<Item> items;
  if (push(&items, column_buffer.col_id, nil) ||
      push_select_type(&items) ||
      push(&items, column_buffer.col_table_name, nil) ||
      push(&items, column_buffer.col_partitions, nil) ||
      push(&items, column_buffer.col_join_type, nil) ||
      push(&items, column_buffer.col_possible_keys, nil) ||
      push(&items, column_buffer.col_key, nil) ||
      push(&items, column_buffer.col_key_len, nil) ||
      push(&items, column_buffer.col_ref, nil) ||
      push(&items, column_buffer.col_rows, nil) ||
      push(&items, column_buffer.col_filtered, nil))
    return true;

  if (column_buffer.col_message.is_empty() &&
      column_buffer.col_extra.is_empty())
  {
    if (items.push_back(nil))
      return true;
  }
  else if (!column_buffer.col_extra.is_empty())
  {
    StringBuffer<64> buff(system_charset_info);
    List_iterator<qep_row::extra> it(column_buffer.col_extra);
    qep_row::extra *e;
    while ((e= it++))
    {
      DBUG_ASSERT(traditional_extra_tags[e->tag] != NULL);
      if (buff.append(traditional_extra_tags[e->tag]))
        return true;
      if (e->data)
      {
        bool brackets= false;
        switch (e->tag) {
        case ET_RANGE_CHECKED_FOR_EACH_RECORD:
        case ET_USING_INDEX_FOR_GROUP_BY:
        case ET_USING_JOIN_BUFFER:
        case ET_FIRST_MATCH:
          brackets= true; // for backward compatibility
          break;
        default:
          break;
        }
        if (e->tag != ET_FIRST_MATCH && // for backward compatibility
            e->tag != ET_PUSHED_JOIN &&
            buff.append(" "))
          return true;
        if (brackets && buff.append("("))
          return true;
        if (buff.append(e->data))
          return true;
        if (e->tag == ET_SCANNED_DATABASES &&
            buff.append(e->data[0] == '1' ? " database" : " databases"))
          return true;
        if (brackets && buff.append(")"))
          return true;
      }
      if (buff.append("; "))
        return true;
    }
    if (!buff.is_empty())
      buff.length(buff.length() - 2); // remove last "; "
    if (push(&items, buff.dup(current_thd->mem_root), buff.length()))
      return true;
  }
  else
  {
    if (push(&items, column_buffer.col_message, nil))
      return true;
  }

  if (output->send_data(items))
    return true;
  return false;
}
 