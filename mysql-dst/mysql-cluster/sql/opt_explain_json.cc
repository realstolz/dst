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

#include "opt_trace.h"
#include "opt_explain_json.h"

/**
  Property names, former parts of traditional "extra" column

  This array must be in sync with Extra_tag enum.
*/
static const char *json_extra_tags[ET_total]=
{
  NULL,                                 // ET_none
  "using_temporary_table",              // ET_USING_TEMPORARY
  "using_filesort",                     // ET_USING_FILESORT
  "index_condition",                    // ET_USING_INDEX_CONDITION
  NULL,                                 // ET_USING
  "range_checked_for_each_record",      // ET_RANGE_CHECKED_FOR_EACH_RECORD
  "pushed_condition",                   // ET_USING_WHERE_WITH_PUSHED_CONDITION
  "using_where",                        // ET_USING_WHERE
  "not_exists",                         // ET_NOT_EXISTS
  "using_MRR",                          // ET_USING_MRR
  "using_index",                        // ET_USING_INDEX
  "full_scan_on_NULL_key",              // ET_FULL_SCAN_ON_NULL_KEY
  "skip_open_table",                    // ET_SKIP_OPEN_TABLE
  "open_frm_only",                      // ET_OPEN_FRM_ONLY
  "open_full_table",                    // ET_OPEN_FULL_TABLE
  "scanned_databases",                  // ET_SCANNED_DATABASES
  "using_index_for_group_by",           // ET_USING_INDEX_FOR_GROUP_BY
  "distinct",                           // ET_DISTINCT
  "loosescan",                          // ET_LOOSESCAN
  NULL,                                 // ET_START_TEMPORARY
  NULL,                                 // ET_END_TEMPORARY
  "first_match",                        // ET_FIRST_MATCH
  NULL,                                 // ET_MATERIALIZE
  NULL,                                 // ET_START_MATERIALIZE
  NULL,                                 // ET_END_MATERIALIZE
  NULL,                                 // ET_SCAN
  "using_join_buffer",                  // ET_USING_JOIN_BUFFER 
  "const_row_not_found",                // ET_CONST_ROW_NOT_FOUND
  "unique_row_not_found",               // ET_UNIQUE_ROW_NOT_FOUND
  "impossible_on_condition",            // ET_IMPOSSIBLE_ON_CONDITION
  "pushed_join",                        // ET_PUSHED_JOIN
  "ft_hints"                            // ET_FT_HINTS
};


// JSON key names
static const char K_ACCESS_TYPE[]=                  "access_type";
static const char K_ATTACHED_CONDITION[]=           "attached_condition";
static const char K_ATTACHED_SUBQUERIES[]=          "attached_subqueries";
static const char K_BUFFER_RESULT[]=                "buffer_result";
static const char K_CACHEABLE[]=                    "cacheable";
static const char K_DEPENDENT[]=                    "dependent";
static const char K_DUPLICATES_REMOVAL[]=           "duplicates_removal";
static const char K_FILTERED[]=                     "filtered";
static const char K_GROUPING_OPERATION[]=           "grouping_operation";
static const char K_GROUP_BY_SUBQUERIES[]=          "group_by_subqueries";
static const char K_HAVING_SUBQUERIES[]=            "having_subqueries";
static const char K_KEY[]=                          "key";
static const char K_KEY_LENGTH[]=                   "key_length";
static const char K_MATERIALIZED_FROM_SUBQUERY[]=   "materialized_from_subquery";
static const char K_MESSAGE[]=                      "message";
static const char K_NESTED_LOOP[]=                  "nested_loop";
static const char K_OPTIMIZED_AWAY_SUBQUERIES[]=    "optimized_away_subqueries";
static const char K_ORDERING_OPERATION[]=           "ordering_operation";
static const char K_ORDER_BY_SUBQUERIES[]=          "order_by_subqueries";
static const char K_PARTITIONS[]=                   "partitions";
static const char K_POSSIBLE_KEYS[]=                "possible_keys";
static const char K_QUERY_BLOCK[]=                  "query_block";
static const char K_QUERY_SPECIFICATIONS[]=         "query_specifications";
static const char K_REF[]=                          "ref";
static const char K_SELECT_ID[]=                    "select_id";
static const char K_SELECT_LIST_SUBQUERIES[]=       "select_list_subqueries";
static const char K_TABLE[]=                        "table";
static const char K_TABLE_NAME[]=                   "table_name";
static const char K_UNION_RESULT[]=                 "union_result";
static const char K_UPDATE_VALUE_SUBQUERIES[]=      "update_value_subqueries";
static const char K_USED_KEY_PARTS[]=               "used_key_parts";
static const char K_USING_FILESORT[]=               "using_filesort";
static const char K_USING_TMP_TABLE[]=              "using_temporary_table";

static const char K_ROWS[]=                         "rows_examined_per_scan";
static const char K_PREFIX_ROWS[]=                  "rows_produced_per_join";

static const char K_COST_INFO[]=                    "cost_info";
static const char K_READ_TIME[]=                    "read_cost";
static const char K_PREFIX_COST[]=                  "prefix_cost";
static const char K_COND_COST[]=                    "eval_cost";
static const char K_SORT_COST[]=                    "sort_cost";
static const char K_QUERY_COST[]=                   "query_cost";
static const char K_DATA_SIZE_QUERY[]=              "data_read_per_join";
static const char K_USED_COLUMNS[]=                 "used_columns";

static const char *mod_type_name[]=
{
  "", "insert", "update", "delete", "replace"
};

/*
  see commentary at the beginning of opt_trace.cc
*/
namespace opt_explain_json_namespace
{

class joinable_ctx;
class sort_ctx;
class subquery_ctx;
class union_result_ctx;

/**
  @note Keep in sync with the @c list_names array.
*/
enum subquery_list_enum
{
  SQ_SELECT_LIST,  ///< SELECT list subqueries
  SQ_UPDATE_VALUE, ///< UPDATE ... SET field=(subquery)
  SQ_HAVING,       ///< HAVING clause subqueries
  SQ_OPTIMIZED_AWAY,///< "optimized_away_subqueries"
  //--------------
  SQ_toplevel,     ///< SQ array size for unit_ctx
  //--------------
  SQ_ORDER_BY,     ///< ORDER BY clause subqueries
  SQ_GROUP_BY,     ///< GROUP BY clause subqueries
  //--------------
  SQ_total
};

/**
  @note Keep in sync with @c subquery_list_enum.
*/
static const char *list_names[SQ_total]=
{ 
  K_SELECT_LIST_SUBQUERIES,
  K_UPDATE_VALUE_SUBQUERIES,
  K_HAVING_SUBQUERIES,
  K_OPTIMIZED_AWAY_SUBQUERIES,
  "",
  K_ORDER_BY_SUBQUERIES,
  K_GROUP_BY_SUBQUERIES,
};


/**
  Base class for all intermediate tree nodes
*/

class context : public Explain_context
{
protected:
  const char *name;

public:
  context *parent; ///< link to parent node or NULL

  context(enum_parsing_context type_arg, const char *name_arg,
          context *parent_arg)
  : Explain_context(type_arg),
    name(name_arg),
    parent(parent_arg)
  {}

  virtual ~context() {}

  /**
    Pass the node with its child nodes to a JSON formatter

    @param json         Formatter

    @retval false       Ok
    @retval true        Error

    @note The @c join_ctx class overloads this function. 
  */
  virtual bool format(Opt_trace_context *json)
  {
    Opt_trace_object obj(json, name);
    return format_body(json, &obj);
  }

  bool is_query_block() const { return name == K_QUERY_BLOCK; }

private:
  /**
    Format JSON object body

    @param json         Formatter
    @param obj          Object of this body

    @retval false       Ok
    @retval true        Error
  */
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)= 0;

public:
  /**
    Analogue of the "id" column in the traditional EXPLAIN output

    @param hide         if true, ban the output of K_SELECT_ID JSON property
                        in the underlying table_with_where_and_derived_ctx
                        objects

    @returns "Select number" that is associated with this node
  */
  virtual size_t id(bool hide= false)= 0;

  virtual bool cacheable() { DBUG_ASSERT(0); return true; }
  virtual bool dependent() { DBUG_ASSERT(0); return false; }

  virtual class qep_row *entry() { DBUG_ASSERT(0); return NULL; }
  virtual enum_mod_type get_mod_type() { return MT_NONE; }

  /**
    Associate a child node with this node

    This function is to be overloaded by subquery_ctx.
  */
  virtual void set_child(context *child) {}

  /// associate CTX_UNION_RESULT node with CTX_UNION node
  virtual void set_union_result(union_result_ctx *ctx) { DBUG_ASSERT(0); }

  /**
    Append a subquery node to the specified list of the unit node

    @param subquery_type    Describes the Item tree where the subquery exists
    @param ctx              Subquery node

    @retval false           Ok
    @retval true            Error
  */
  virtual bool add_subquery(subquery_list_enum subquery_type,
                            subquery_ctx *ctx) { DBUG_ASSERT(0);return true; }
  /**
    Format nested loop join subtree (if any) to JSON formatter

    @param json                 Formatter

    @retval false               Ok
    @retval true                Error
  */
  virtual bool format_nested_loop(Opt_trace_context *json)
  { DBUG_ASSERT(0); return true; }

  /**
    Add a CTX_QEP_TAB node to a CTX_JOIN node

    @param ctx          CTX_QEP_TAB node

    @retval false           Ok
    @retval true            Error
  */
  virtual bool add_join_tab(joinable_ctx *ctx) { DBUG_ASSERT(0);return true; }

  /**
    Set nested ORDER BY/GROUP BY/DISTINCT node to @c ctx

    @param json                 Formatter

    @retval false               Ok
    @retval true                Error
  */
  virtual void set_sort(sort_ctx *ctx) { DBUG_ASSERT(0); }

  /**
    Add a query specification node to the CTX_UNION node

    @param ctx              query specification node

    @retval false           Ok
    @retval true            Error
  */
  virtual bool add_query_spec(context *ctx) { DBUG_ASSERT(0); return true; }

  /**
    Try to associate a derived subquery node with this or underlying node

    @param subquery     Derived subquery node

    @retval true        Success
    @retval false       Can't associate: this node or its child nodes are not
                        derived from the subquery
  */
  virtual bool find_and_set_derived(context *subquery)
  {
    DBUG_ASSERT(0);
    return false;
  }

  /**
    Associate WHERE subqueries of given context and unit with this object

    @param ctx          Context of WHERE subquery
    @param subquery     For CTX_QEP_TAB: match given unit with a previously
                        collected by the register_where_subquery function.
    @returns
      -1   subquery wasn't found
       0   subqusery were added
       1   error occured
  */
  virtual int add_where_subquery(subquery_ctx *ctx,
                                  SELECT_LEX_UNIT *subquery)
  {
    DBUG_ASSERT(0);
    return false;
  }

  /// Helper function to format output for derived subquery if any
  virtual bool format_derived(Opt_trace_context *json) { return false; }

  /// Helper function to format output for associated WHERE subqueries if any
  virtual bool format_where(Opt_trace_context *json) { return false; }

  /// Helper function to format output for HAVING, ORDER/GROUP BY subqueries
  virtual bool format_unit(Opt_trace_context *json) { return false; }
};


/**
  Node class to wrap a subquery node tree

  Implements CTX_WHERE, CTX_HAVING, CTX_ORDER_BY_SQ, CTX_GROUP_BY_SQ and
  CTX_OPTIMIZED_AWAY_SUBQUERY context nodes.
  This class hosts underlying join_ctx or uion_ctx.
*/

class subquery_ctx : virtual public context, public qep_row
{
  /*
    TODO: After the conversion from multiple inheritace to templates
    convert "context" to "unit_ctx" (common base of uion_ctx & join_ctx).
  */
  context *subquery; ///< hosted subquery tree: CTX_JOIN or CTX_UNION

public:
  subquery_ctx(enum_parsing_context type_arg,
               const char *name_arg, context *parent_arg)
  : context(type_arg, name_arg, parent_arg),
    subquery(NULL)
  {}

  virtual qep_row *entry() { return this; }

  /*
    Materialized subquery statuses of dependency on the outer query and
    cacheability may differ from the source subquery, for example, if
    we "push down" the outer look up value for SJ.
    Thus, for materialized subqueries return direct is_cacheable and
    is_dependent values instead of source subquery statuses:
  */
  virtual bool cacheable()
  {
    return is_materialized_from_subquery ? is_cacheable : subquery->cacheable();
  }
  virtual bool dependent()
  {
    return is_materialized_from_subquery ? is_dependent : subquery->dependent();
  }

  virtual bool format(Opt_trace_context *json)
  {
    if (name)
      return context::format(json);
    else
    {
      /*
        Subquery is always a homogeneous array element,
        create anonymous  wrapper object:
      */
      Opt_trace_object anonymous_wrapper(json);
      return format_body(json, &anonymous_wrapper);
    }
  }

private:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    if (type == CTX_DERIVED)
    {
      obj->add(K_USING_TMP_TABLE, true);
      obj->add(K_DEPENDENT, dependent());
      obj->add(K_CACHEABLE, cacheable());
      return subquery->format(json);
    }
    else if (using_temporary)
    {
      if (!is_materialized_from_subquery)
      {
        obj->add(K_USING_TMP_TABLE, true);
        obj->add(K_DEPENDENT, dependent());
        obj->add(K_CACHEABLE, cacheable());
      }

      {
        Opt_trace_object tmp_table(json, K_TABLE);

        if (!col_table_name.is_empty())
          obj->add_utf8(K_TABLE_NAME, col_table_name.str);
        if (!col_join_type.is_empty())
          tmp_table.add_alnum(K_ACCESS_TYPE, col_join_type.str);
        if (!col_key.is_empty())
          tmp_table.add_utf8(K_KEY, col_key.str);
        if (!col_key_len.is_empty())
          obj->add_alnum(K_KEY_LENGTH, col_key_len.str);
        if (!col_rows.is_empty())
          tmp_table.add(K_ROWS, col_rows.value);

        if (is_materialized_from_subquery)
        {
          Opt_trace_object materialized(json, K_MATERIALIZED_FROM_SUBQUERY);
          obj->add(K_USING_TMP_TABLE, true);
          obj->add(K_DEPENDENT, dependent());
          obj->add(K_CACHEABLE, cacheable());
          return format_query_block(json);
        }
      }
      return format_query_block(json);
    }
    else
    {
      obj->add(K_DEPENDENT, dependent());
      obj->add(K_CACHEABLE, cacheable());
      return subquery->format(json);
    }
  }

  bool format_query_block(Opt_trace_context *json)
  {
    if (subquery->is_query_block())
      return subquery->format(json);

    Opt_trace_object query_block(json, K_QUERY_BLOCK);
    return subquery->format(json);
  }


public:
  virtual void set_child(context *child)
  {
    DBUG_ASSERT(subquery == NULL);
    DBUG_ASSERT(child->type == CTX_JOIN || child->type == CTX_UNION);
    subquery= child;
  }

  virtual size_t id(bool hide) { return subquery->id(hide); }
};


/**
  Helper function to pass a subquery list to a JSON formatter

  @param json         output formatter
  @param subqueries   subquery list to output
  @param name         name for the output section

  @retval false       Ok
  @retval true        Error
*/
static bool format_list(Opt_trace_context *json,
                        List<subquery_ctx> &subqueries,
                        const char *name)
{
  if (!subqueries.is_empty())
  {
    Opt_trace_array subs(json, name);

    List_iterator<subquery_ctx> it(subqueries);
    subquery_ctx *t;
    while ((t= it++))
    {
      // Homogeneous array: additional anonymous wrapper object is not needed
      if (t->format(json))
        return true;
    }
  }
  return false;
}


/**
  Helper base class to host HAVING, ORDER BY and GROUP BY subquery nodes
*/
class unit_ctx : virtual public context
{
  List<subquery_ctx> subquery_lists[SQ_total];

public:
  unit_ctx(enum_parsing_context type_arg, const char *name_arg,
           context *parent_arg)
  : context(type_arg, name_arg, parent_arg)
  {}

  /**
    Helper function to distinguish subquery-less nodes

    @retval true        Node hosts no subqueries
    @retval false       Node hosts some subqueries
  */
  bool has_no_subqueries() const
  {
    for (size_t i= 0; i < SQ_total; i++)
    {
      if (!subquery_lists[i].is_empty())
        return false;
    }
    return true;
  }

  virtual bool format_unit(Opt_trace_context *json)
  {
    for (size_t i= 0; i < SQ_total; i++)
    {
      if (format_list(json, subquery_lists[i], list_names[i]))
        return true;
    }
    return false;
  }

  virtual bool add_subquery(subquery_list_enum subquery_type,
                            subquery_ctx *ctx)
  {
    return subquery_lists[subquery_type].push_back(ctx);
  }
};


class table_base_ctx : virtual public context, virtual public qep_row
{
protected:
  bool is_hidden_id; //< if true, don't output K_SELECT_ID property

public:
  table_base_ctx(enum_parsing_context type_arg,
                 const char *name_arg, context *parent_arg)
  : context(type_arg, name_arg, parent_arg), is_hidden_id(false)
  {}

  virtual qep_row *entry() { return this; }

protected:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj);

public:
  virtual size_t id(bool hide)
  {
    return col_id.is_empty() ? 0 : col_id.value;
  }

  virtual bool cacheable() { return is_cacheable; }
  virtual bool dependent() { return is_dependent; }
};


static void add_string_array(Opt_trace_context *json, const char *list_name,
                             List<const char> &strings)
{
  if (!strings.is_empty())
  {
    Opt_trace_array extra(json, list_name);

    List_iterator<const char> it(strings);
    const char *s;
    while ((s= it++))
      extra.add_utf8(s);
  }
}

static void print_cost(char *buf, uint buf_len, double cost)
{
  if (cost < 100000000000000.0)
    my_snprintf(buf, buf_len, "%.2f", cost);
  else
    my_snprintf(buf, buf_len, "%.14g", cost);
}

static void print_filtered(char *buf, uint buf_len, double filtered)
{
  my_snprintf(buf, buf_len, "%.2f", filtered);
}


bool table_base_ctx::format_body(Opt_trace_context *json, Opt_trace_object *obj)
{
  StringBuffer<64> buff;

  if (mod_type != MT_NONE)
    obj->add(mod_type_name[mod_type], true);

  if (!col_id.is_empty() && !is_hidden_id)
    obj->add(K_SELECT_ID, col_id.value);

  if (!col_table_name.is_empty())
    obj->add_utf8(K_TABLE_NAME, col_table_name.str);

  add_string_array(json, K_PARTITIONS, col_partitions);

  if (!col_join_type.is_empty())
    obj->add_alnum(K_ACCESS_TYPE, col_join_type.str);

  add_string_array(json, K_POSSIBLE_KEYS, col_possible_keys);

  if (!col_key.is_empty())
    obj->add_utf8(K_KEY, col_key.str);

  if (!col_key_parts.is_empty())
    add_string_array(json, K_USED_KEY_PARTS, col_key_parts);

  if (!col_key_len.is_empty())
    obj->add_alnum(K_KEY_LENGTH, col_key_len.str);

  add_string_array(json, K_REF, col_ref);

  if (!col_rows.is_empty())
    obj->add(K_ROWS, col_rows.value);
  if (!col_prefix_rows.is_empty())
    obj->add(K_PREFIX_ROWS, col_prefix_rows.value);

  if (!col_filtered.is_empty())
  {
    char buf[32];                         // 32 is enough for digits of a double
    print_filtered(buf, sizeof(buf), col_filtered.value);
    obj->add_utf8(K_FILTERED, buf);
  }

  if (!col_extra.is_empty())
  {
    List_iterator<qep_row::extra> it(col_extra);
    qep_row::extra *e;
    while ((e= it++))
    {
      DBUG_ASSERT(json_extra_tags[e->tag] != NULL);
      if (e->data)
        obj->add_utf8(json_extra_tags[e->tag], e->data);
      else
        obj->add(json_extra_tags[e->tag], true);
    }
  }

  if (!col_read_cost.is_empty())
  {
    Opt_trace_object cost_info(json, K_COST_INFO);
    char buf[32];                         // 32 is enough for digits of a double

    print_cost(buf, sizeof(buf), col_read_cost.value);
    cost_info.add_utf8(K_READ_TIME, buf);

    if (!col_cond_cost.is_empty())
    {
      print_cost(buf, sizeof(buf), col_cond_cost.value);
      cost_info.add_utf8(K_COND_COST, buf);
    }
    if (!col_prefix_cost.is_empty())
    {
      print_cost(buf, sizeof(buf), col_prefix_cost.value);
      cost_info.add_utf8(K_PREFIX_COST, buf);
    }
    if (!col_data_size_query.is_empty())
      cost_info.add_utf8(K_DATA_SIZE_QUERY, col_data_size_query.str);
  }

  if (!col_used_columns.is_empty())
    add_string_array(json, K_USED_COLUMNS, col_used_columns);

  if (!col_message.is_empty() && type != CTX_MESSAGE)
  {
    DBUG_ASSERT(col_extra.is_empty());
    obj->add_alnum(K_MESSAGE, col_message.str);
  }

  { // Keep together for better output readability
    if (!col_attached_condition.is_empty())
      obj->add_utf8(K_ATTACHED_CONDITION, col_attached_condition.str);
    if (format_where(json))
      return true;
  }

  return format_derived(json) || format_unit(json);
}


/**
  Node class for the CTX_UNION_RESULT
*/
class union_result_ctx : public table_base_ctx, public unit_ctx
{
  List<context> *query_specs; ///< query specification nodes (inner selects)
  List<subquery_ctx> order_by_subqueries;
  List<subquery_ctx> optimized_away_subqueries;
  joinable_ctx *message;

public:
  explicit union_result_ctx(context *parent_arg)
  : context(CTX_UNION_RESULT, K_UNION_RESULT, parent_arg),
    table_base_ctx(CTX_UNION_RESULT, K_UNION_RESULT, parent_arg),
    unit_ctx(CTX_UNION_RESULT, K_UNION_RESULT, parent_arg),
    message(NULL)
  {}

  // Remove warnings: 'inherits ... from ... via dominance'
  virtual size_t id(bool hide) { return table_base_ctx::id(hide); }
  virtual bool cacheable()     { return table_base_ctx::cacheable(); }
  virtual bool dependent()     { return table_base_ctx::dependent(); }
  virtual qep_row *entry()     { return table_base_ctx::entry(); }
  virtual bool format_unit(Opt_trace_context *json)
  { return table_base_ctx::format_unit(json); }

  void push_down_query_specs(List<context> *specs) { query_specs= specs; } 

  virtual bool add_subquery(subquery_list_enum subquery_type,
                            subquery_ctx *ctx)
  {
    switch (subquery_type) {
    case SQ_ORDER_BY:
      return order_by_subqueries.push_back(ctx);
    case SQ_OPTIMIZED_AWAY:
      return optimized_away_subqueries.push_back(ctx);
    default:
      DBUG_ASSERT(!"Unknown query type!");
      return false; // ignore in production
    }
  }

  virtual bool add_join_tab(joinable_ctx *ctx)
  {
    DBUG_ASSERT(!message);
    message= ctx;
    return false;
  }

  virtual bool format(Opt_trace_context *json);

  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj);
};


/**
  Common part of CTX_QEP_TAB and CTX_MESSAGE nodes

  This class implements functionality for WHERE and derived subqueries that
  are associated with the table node.
*/
class table_with_where_and_derived : public table_base_ctx
{
public:
  List<subquery_ctx> where_subqueries; ///< associated WHERE clause subqueries

  table_with_where_and_derived(enum_parsing_context type_arg,
                                 const char *name_arg, context *parent_arg)
  : context(type_arg, name_arg, parent_arg),
    table_base_ctx(type_arg, name_arg, parent_arg)
  {}

  virtual size_t id(bool hide)
  {
    if (hide)
      is_hidden_id= true;
    return table_base_ctx::id(hide);
  }

  virtual bool format_where(Opt_trace_context *json)
  {
    return format_list(json, where_subqueries, K_ATTACHED_SUBQUERIES);
  }

  virtual bool format_derived(Opt_trace_context *json)
  {
    if (derived_from.elements == 0)
      return false;
    else if (derived_from.elements == 1)
      return derived_from.head()->format(json);
    else
    {
      Opt_trace_array loops(json, K_NESTED_LOOP);

      List_iterator<context> it(derived_from);
      context *c;
      while((c= it++))
      {
        Opt_trace_object anonymous_wrapper(json);
        if (c->format(json))
          return true;
      }
    }
    return false;
  }
};


/**
  Base for CTX_QEP_TAB, CTX_DUPLICATES_WEEDOUT and CTX_MATERIALIZATION nodes

  This class implements a base to explain individual JOIN_TABs as well
  as JOIN_TAB groups like in semi-join materialization.
*/
class joinable_ctx : virtual public context
{
public:
  joinable_ctx(enum_parsing_context type_arg, const char *name_arg,
               context *parent_arg)
  : context(type_arg, name_arg, parent_arg)
  {}
};


/**
  Node class for CTX_MESSAGE

  This class is designed to represent fake tables with some messages in the
  "extra" column ("Impossible where" etc).
  We do EXPLAIN of these fake tables to replace explanation of:
    1) usual actual JOIN_TABs of the whole JOIN or
    2) a modifying TABLE of single-table UPDATE/DELETE/etc.
  So, message_ctx always represent a single half-empty fake table in a
  "query_block" node with optional subqueries.
*/
class message_ctx : public joinable_ctx,
                    public table_with_where_and_derived
{
public:
  explicit message_ctx(context *parent_arg)
  : context(CTX_MESSAGE, K_TABLE, parent_arg),
    joinable_ctx(CTX_MESSAGE, K_TABLE, parent_arg),
    table_with_where_and_derived(CTX_MESSAGE, K_TABLE, parent_arg)
  {}

  // Remove warnings: 'inherits ... from ... via dominance'
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  { return table_base_ctx::format_body(json, obj); }
  virtual size_t id(bool hide)
  { return table_with_where_and_derived::id(hide); }
  virtual bool cacheable()     { return table_base_ctx::cacheable(); }
  virtual bool dependent()     { return table_base_ctx::dependent(); }
  virtual qep_row *entry()     { return table_base_ctx::entry(); }
  virtual bool format_derived(Opt_trace_context *json)
  { return table_with_where_and_derived::format_derived(json); }
  virtual bool format_where(Opt_trace_context *json)
  { return table_with_where_and_derived::format_where(json); }

  virtual bool find_and_set_derived(context *subquery)
  {
    /*
      message_ctx is designed to represent a single fake JOIN_TAB in the JOIN,
      so if the JOIN have a derived table, then this message_ctx represent this
      derived table.
      Unconditionally add subquery:
    */
    derived_from.push_back(subquery);
    return true;
  }

  virtual int add_where_subquery(subquery_ctx *ctx,
                                  SELECT_LEX_UNIT *subquery)
  {
    return where_subqueries.push_back(ctx);
  }
};


/**
  Node class for the CTX_QEP_TAB context
*/
class join_tab_ctx : public joinable_ctx,
                     public table_with_where_and_derived
{
  /**
    Subquery units that are associated with this JOIN_TAB's condition

    This list is used to match with the @c subquery parameter of 
    the @c add_where_subquery function.
  */
  List<SELECT_LEX_UNIT> where_subquery_units;

public:
  join_tab_ctx(enum_parsing_context type_arg, context *parent_arg)
  : context(type_arg, K_TABLE, parent_arg),
    joinable_ctx(type_arg, K_TABLE, parent_arg),
    table_with_where_and_derived(type_arg, K_TABLE, parent_arg)
  {}

  // Remove warnings: 'inherits ... from ... via dominance'
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  { return table_base_ctx::format_body(json, obj); }
  virtual size_t id(bool hide)
  { return table_with_where_and_derived::id(hide); }
  virtual bool cacheable()     { return table_base_ctx::cacheable(); }
  virtual bool dependent()     { return table_base_ctx::dependent(); }
  virtual qep_row *entry()     { return table_base_ctx::entry(); }
  virtual bool format_derived(Opt_trace_context *json)
  { return table_with_where_and_derived::format_derived(json); }
  virtual bool format_where(Opt_trace_context *json)
  { return table_with_where_and_derived::format_where(json); }

  virtual void register_where_subquery(SELECT_LEX_UNIT *subquery)
  {
    List_iterator<SELECT_LEX_UNIT> it(where_subquery_units);
    SELECT_LEX_UNIT *u;
    while ((u= it++))
    {
      /*
        The server may transform (x = (SELECT FROM DUAL)) to
        (x <=> (SELECT FROM DUAL) AND x = (SELECT FROM DUAL)),
        so ignore duplicates:
      */
      if(u == subquery)
        return;
    }
    where_subquery_units.push_back(subquery);
  }

  virtual int add_where_subquery(subquery_ctx *ctx,
                                  SELECT_LEX_UNIT *subquery)
  {
    List_iterator<SELECT_LEX_UNIT> it(where_subquery_units);
    SELECT_LEX_UNIT *u;
    while ((u= it++))
    {
      if (u == subquery)
        return where_subqueries.push_back(ctx);
    }
    return -1;
  }

  virtual bool find_and_set_derived(context *subquery)
  {
    if (query_block_id == subquery->id())
    {
      derived_from.push_back(subquery);
      return true;
    }
    return false;
  }
  virtual enum_mod_type get_mod_type() { return entry()->mod_type; }
};


/**
  Base class for CTX_ORDER_BY, CTX_GROUP_BY and node class for CTX_DISTINCT

  This class represents context for simple ORDER BY/GROUP BY/DISTINCT clauses
  (the clause is effective for the single JOIN_TAB).
*/

class simple_sort_ctx : public joinable_ctx
{
protected:
  joinable_ctx *join_tab; //< single JOIN_TAB that we sort

private:
  const bool using_tmptable; //< true if the clause creates intermediate table
  const bool using_filesort; //< true if the clause uses filesort

public:
  simple_sort_ctx(enum_parsing_context type_arg, const char *name_arg,
                  context *parent_arg,
                  const Explain_format_flags *flags,
                  Explain_sort_clause clause)
  : context(type_arg, name_arg, parent_arg),
    joinable_ctx(type_arg, name_arg, parent_arg),
    join_tab(NULL),
    using_tmptable(flags->get(clause, ESP_USING_TMPTABLE)),
    using_filesort(flags->get(clause, ESP_USING_FILESORT))
  {}

  virtual bool add_join_tab(joinable_ctx *ctx)
  {
    join_tab= ctx;
    return false;
  }

  virtual int add_where_subquery(subquery_ctx *ctx,
                                  SELECT_LEX_UNIT *subquery)
  {
    return join_tab->add_where_subquery(ctx, subquery);
  }

  virtual bool find_and_set_derived(context *subquery)
  {
    return join_tab->find_and_set_derived(subquery);
  }

  virtual size_t id(bool hide) { return join_tab->id(hide); }
  virtual bool cacheable() { return join_tab->cacheable(); }
  virtual bool dependent() { return join_tab->dependent(); }

protected:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    if (using_tmptable)
      obj->add(K_USING_TMP_TABLE, true);
    obj->add(K_USING_FILESORT, using_filesort);
    return join_tab->format(json);
  }
};


/**
  Node class for "simple" CTX_ORDER_BY and CTX_GROUP_BY

  This class represents context for simple ORDER BY or GROUP BY clauses
  (the clause is effective for the single JOIN_TAB).
*/

class simple_sort_with_subqueries_ctx : public simple_sort_ctx
{
  const subquery_list_enum subquery_type; //< type of this clause subqueries
  List<subquery_ctx> subqueries;

public:
  simple_sort_with_subqueries_ctx(enum_parsing_context type_arg,
                                  const char *name_arg,
                                  context *parent_arg,
                                  subquery_list_enum subquery_type_arg,
                                  const Explain_format_flags *flags,
                                  Explain_sort_clause clause)
  : context(type_arg, name_arg, parent_arg),
    simple_sort_ctx(type_arg, name_arg, parent_arg, flags, clause),
    subquery_type(subquery_type_arg)
  {}

  virtual bool add_subquery(subquery_list_enum subquery_type_arg,
                            subquery_ctx *ctx)
  {
    if (subquery_type != subquery_type_arg)
      return simple_sort_ctx::add_subquery(subquery_type_arg, ctx);
    else
      return subqueries.push_back(ctx);
  }
  
private:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    return (simple_sort_ctx::format_body(json, obj) ||
            (format_list(json, subqueries, list_names[subquery_type])));
  }
};


/**
  Node class for the CTX_JOIN context
*/

class join_ctx : public unit_ctx, virtual public qep_row
{
protected:
  List<joinable_ctx> join_tabs; ///< hosted JOIN_TAB nodes
  sort_ctx *sort;

public:
  join_ctx(enum_parsing_context type_arg, const char *name_arg,
             context *parent_arg)
  : context(type_arg, name_arg, parent_arg),
    unit_ctx(type_arg, name_arg, parent_arg),
    sort(0)
  {}

  virtual bool add_join_tab(joinable_ctx *ctx)
  {
    return join_tabs.push_back(ctx);
  }

  virtual void set_sort(sort_ctx *ctx)
  {
    DBUG_ASSERT(!sort);
    sort= ctx;
  }
  virtual qep_row *entry() { return this; }

  /**
    Associate a CTX_DERIVED node with its CTX_QEP_TAB node

    @param subquery     derived subquery tree
  */
  virtual bool find_and_set_derived(context *subquery);

  virtual bool add_subquery(subquery_list_enum subquery_type,
                            subquery_ctx *ctx);
protected:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj);
  bool format_body_inner(Opt_trace_context *json, Opt_trace_object *obj);
  virtual const char *get_cost_tag() { return K_QUERY_COST; }

public:
  virtual bool format_nested_loop(Opt_trace_context *json);
  virtual size_t id(bool hide);
  virtual bool cacheable();
  virtual bool dependent();
  virtual int add_where_subquery(subquery_ctx *ctx, SELECT_LEX_UNIT *subquery);
};


/**
  Node class for CTX_SIMPLE_ORDER_BY, CTX_SIMPLE_GROUP_BY and CTX_SIMPLE_DISTINCT

  CTX_JOIN context (see join_ctx class) may contain nested loop join node *or*
  ORDER BY/GROUP BY/DISTINCT node that is represented by this class:

    join: { nested_loop: [ ... ] }
  or
    join: { order_by|group_by|distinct : { ... } }

  CTX_ORDER_BY may contain nested loop join tree *or* GROUP BY/DISTINCT node:

    order_by: { nested_loop|group_by|distinct: ... }

  CTX_DISTINCT context structure:

    distinct: { nested_loop|group_by: ... }

  CTX_GROUP_BY:

    group_by: { nested_loop: [ ... ] }

  I.e. the most complex CTX_JOIN may have such a structure of JSON output as:

    join: {
      order_by: {
        distinct: {
          group_by: {
            nested_loop: [ ... ]
          }
        }
      }
    }
TODO
*/

class sort_ctx : public join_ctx
{
  const bool using_tmptable; //< the clause creates temporary table
  const bool using_filesort; //< the clause uses filesort

public:
  sort_ctx(enum_parsing_context type_arg, const char *name_arg,
           context *parent_arg,
           const Explain_format_flags *flags,
           Explain_sort_clause clause)
  : context(type_arg, name_arg, parent_arg),
    join_ctx(type_arg, name_arg, parent_arg),
    using_tmptable(flags->get(clause, ESP_USING_TMPTABLE)),
    using_filesort(flags->get(clause, ESP_USING_FILESORT))
  {}

protected:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    DBUG_ASSERT(!sort || join_tabs.is_empty());

    if (using_tmptable)
      obj->add(K_USING_TMP_TABLE, true);
    if (type != CTX_BUFFER_RESULT)
      obj->add(K_USING_FILESORT, using_filesort);

    return join_ctx::format_body(json, obj);
  }
  const char *get_cost_tag() { return K_SORT_COST; }
};


class sort_with_subqueries_ctx : public sort_ctx
{
  const subquery_list_enum subquery_type; //< subquery type for this clause
  List<subquery_ctx> subqueries;

public:
  sort_with_subqueries_ctx(enum_parsing_context type_arg, const char *name_arg,
                           context *parent_arg,
                           subquery_list_enum subquery_type_arg,
                           const Explain_format_flags *flags,
                           Explain_sort_clause clause)
  : context(type_arg, name_arg, parent_arg),
    sort_ctx(type_arg, name_arg, parent_arg, flags, clause),
    subquery_type(subquery_type_arg)
  {}

  virtual bool add_subquery(subquery_list_enum subquery_type_arg,
                            subquery_ctx *ctx)
  {
    if (subquery_type_arg != subquery_type)
      return sort_ctx::add_subquery(subquery_type_arg, ctx);
    else
      return subqueries.push_back(ctx);
    return false;
  }
    
private:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    return  (sort_ctx::format_body(json, obj) ||
             format_list(json, subqueries, list_names[subquery_type]));
  }
};


bool join_ctx::find_and_set_derived(context *subquery)
{
  DBUG_ASSERT(subquery->id() != 0);

  if (sort)
    return sort->find_and_set_derived(subquery);

  List_iterator<joinable_ctx> it(join_tabs);
  joinable_ctx *t;
  while ((t= it++))
  {
    if (t->find_and_set_derived(subquery))
      return true;
  }
  return false;
}


bool join_ctx::add_subquery(subquery_list_enum subquery_type,
                          subquery_ctx *ctx)
{
  if (sort)
    return sort->add_subquery(subquery_type, ctx);
  if (subquery_type > SQ_toplevel)
  {
    List_iterator<joinable_ctx> it(join_tabs);
    joinable_ctx *j;
    while ((j= it++))
    {
      switch (j->type) {
      case CTX_ORDER_BY:
      case CTX_DISTINCT:
      case CTX_GROUP_BY:
      case CTX_SIMPLE_ORDER_BY:
      case CTX_SIMPLE_DISTINCT:
      case CTX_SIMPLE_GROUP_BY:
        return j->add_subquery(subquery_type, ctx);
      case CTX_MESSAGE: // The 'no plan' case
        DBUG_ASSERT(subquery_type == SQ_ORDER_BY ||
                    subquery_type == SQ_GROUP_BY);
        return unit_ctx::add_subquery(subquery_type, ctx);
      default:
        DBUG_ASSERT(0); /* purecov: inspected */
      }
    }
  }
  else
    return unit_ctx::add_subquery(subquery_type, ctx);
  return true; /* purecov: inspected */
}


bool join_ctx::format_body(Opt_trace_context *json, Opt_trace_object *obj)
{
  if (type == CTX_JOIN)
    obj->add(K_SELECT_ID, id(true));
  if (!col_read_cost.is_empty())
  {
    char buf[32];                         // 32 is enough for digits of a double

    Opt_trace_object cost_info(json, K_COST_INFO);
    print_cost(buf, sizeof(buf), col_read_cost.value);
    cost_info.add_utf8(get_cost_tag(), buf);
  }
  // Print target table for INSERT/REPLACE SELECT outside of nested loop
  if (join_tabs.elements &&
      (join_tabs.head()->get_mod_type() == MT_INSERT ||
       join_tabs.head()->get_mod_type() == MT_REPLACE))
  {
    join_tabs.head()->format(json);
    if (sort || join_tabs.elements > 1)
    {
      Opt_trace_object insert_from(json, "insert_from");
      if (format_body_inner(json, obj))
        return true; /* purecov: inspected */
    }
  }
  else if (format_body_inner(json, obj))
    return true; /* purecov: inspected */
  return format_unit(json);
}


bool join_ctx::format_body_inner(Opt_trace_context *json, Opt_trace_object *obj)
{
  if (sort)
  {
    if (sort->format(json))
      return true; /* purecov: inspected */
  }
  else if (join_tabs.elements && join_tabs.head()->type == CTX_MESSAGE)
  {
    // Could be only 1 message per join
    DBUG_ASSERT(join_tabs.elements == 1);
    message_ctx *msg= (message_ctx*)join_tabs.head();
    obj->add_alnum(K_MESSAGE, msg->entry()->col_message.str);
    if (msg->derived_from.elements)
      msg->format(json);
    else if (msg->where_subqueries.elements)
      msg->format_where(json);
  }
  else if (format_nested_loop(json))
    return true;
  return false;
}


bool join_ctx::format_nested_loop(Opt_trace_context *json)
{
  List_iterator<joinable_ctx> it(join_tabs);
  uint join_tab_num= join_tabs.elements;
  DBUG_ASSERT(join_tabs.elements > 0);

  if (join_tabs.head()->get_mod_type() == MT_INSERT ||
      join_tabs.head()->get_mod_type() == MT_REPLACE)
  {
    it++;
    join_tab_num--;
  }
  /*
    For single table skip "nested_loop" object creation and
    format its contents only (the 1st join_tab).
  */
  if (join_tab_num == 1)
    return (it++)->format(json);

  Opt_trace_array loops(json, K_NESTED_LOOP);

  joinable_ctx *t;
  while ((t= it++))
  {
    Opt_trace_object anonymous_wrapper(json);
    if (t->format(json))
      return true;
  }
  return false;
}


bool union_result_ctx::format_body(Opt_trace_context *json,
                                   Opt_trace_object *obj)
{
  obj->add(K_USING_TMP_TABLE, true);

  if (table_base_ctx::format_body(json, obj))
    return true; /* purecov: inspected */

  if (message)
  {
    message_ctx *msg= (message_ctx*)message;
    obj->add_alnum(K_MESSAGE, msg->entry()->col_message.str);
  }

  Opt_trace_array specs(json, K_QUERY_SPECIFICATIONS);

  List_iterator<context> it(*query_specs);
  context *ctx;
  while ((ctx= it++))
  {
    if (ctx->format(json))
      return true; /* purecov: inspected */
  }
  return false;
}


/**
  Auxiliary function to walk through the list and propagate "hide" value

  @param list   list of context (*_ctx)  objects
  @param hide   if true, ban the output of K_SELECT_ID JSON property
                in the underlying table_with_where_and_derived_ctx
                and materialize_ctx objects

  @return       id of underlying objects
*/
template<typename T>
static size_t get_id(List<T> &list, bool hide)
{
  if (!hide)
    return list.head()->id();
    
  List_iterator<T> it(list);
  T *j;
  size_t ret= 0;
  while ((j= it++))
    ret= j->id(hide);
  return ret;
}


size_t join_ctx::id(bool hide)
{
  return sort ? sort->id(hide) : get_id(join_tabs, hide);
}


bool join_ctx::cacheable()
{
  return sort ? sort->cacheable() : join_tabs.head()->cacheable();
}


bool join_ctx::dependent()
{
  return sort ? sort->dependent() : join_tabs.head()->dependent();
}


int join_ctx::add_where_subquery(subquery_ctx *ctx,
                                  SELECT_LEX_UNIT *subquery)
{
  if (sort)
    return sort->join_ctx::add_where_subquery(ctx, subquery);

  List_iterator<joinable_ctx> it(join_tabs);
  joinable_ctx *j;
  bool found= false;
  while ((j= it++))
  {
    int ret= j->add_where_subquery(ctx, subquery);
    if (ret > 0)
      return true;
    found|= (ret == 0);
  }
  if (!found)
    return add_subquery(SQ_OPTIMIZED_AWAY, ctx);
  return false;
}


/**
  Context class to group materialized JOIN_TABs to "matirealized" array
*/

class materialize_ctx : public joinable_ctx, public join_ctx,
                        public table_base_ctx
{
public:
  explicit materialize_ctx(context *parent_arg)
  : context(CTX_MATERIALIZATION, K_TABLE, parent_arg),
    joinable_ctx(CTX_MATERIALIZATION, K_TABLE, parent_arg),
    join_ctx(CTX_MATERIALIZATION, K_TABLE, parent_arg),
    table_base_ctx(CTX_MATERIALIZATION, K_TABLE, parent_arg)
  {}

  virtual size_t id(bool hide)
  {
    if (hide)
    {
     is_hidden_id= true;
     /* Set the materizlize table's id to hide */
     join_ctx::id(hide);
    }
    return table_base_ctx::id(hide);
  }
  virtual bool cacheable() { return join_ctx::cacheable(); }
  virtual bool dependent() { return join_ctx::dependent(); }

  // Remove warnings: 'inherits ... from ... via dominance'
  virtual qep_row *entry()     { return table_base_ctx::entry(); }
  virtual bool add_subquery(subquery_list_enum subquery_type, subquery_ctx *ctx)
  { return join_ctx::add_subquery(subquery_type, ctx); }
  virtual bool add_join_tab(joinable_ctx *ctx)
  { return join_ctx::add_join_tab(ctx); }
  virtual int add_where_subquery(subquery_ctx *ctx, SELECT_LEX_UNIT *subquery)
  { return join_ctx::add_where_subquery(ctx, subquery); }
  virtual bool find_and_set_derived(context *subquery)
  { return join_ctx::find_and_set_derived(subquery); }
  virtual bool format_unit(Opt_trace_context *json)
  { return unit_ctx::format_unit(json); }
  virtual bool format_nested_loop(Opt_trace_context *json)
  { return join_ctx::format_nested_loop(json); }
  virtual void set_sort(sort_ctx *ctx)
  { return join_ctx::set_sort(ctx); }

private:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    DBUG_ASSERT(!col_join_type.is_empty());

    if (!col_table_name.is_empty())
      obj->add_utf8(K_TABLE_NAME, col_table_name.str);

    obj->add_alnum(K_ACCESS_TYPE, col_join_type.str);

    if (!col_key.is_empty())
      obj->add_utf8(K_KEY, col_key.str);

    if (!col_key_len.is_empty())
      obj->add_alnum(K_KEY_LENGTH, col_key_len.str);

    add_string_array(json, K_REF, col_ref);

    if (!col_rows.is_empty())
      obj->add(K_ROWS, col_rows.value);

    /*
      Currently K-REF/col_ref is not shown; it would always be "func", since
      {subquery,semijoin} materialization use store_key_item; using
      get_store_key() instead would allow "const" and outer column's name,
      if applicable.
      The looked up expression can anyway be inferred from the condition:
    */
    if (!col_attached_condition.is_empty())
      obj->add_utf8(K_ATTACHED_CONDITION, col_attached_condition.str);
    if (format_where(json))
      return true;

    Opt_trace_object m(json, K_MATERIALIZED_FROM_SUBQUERY);
    obj->add(K_USING_TMP_TABLE, true);
    Opt_trace_object q(json, K_QUERY_BLOCK);
    return format_nested_loop(json);
  }
};


/**
  Context class to represent JOIN_TABs in duplication weedout sequence
*/

class duplication_weedout_ctx : public joinable_ctx, public join_ctx
{
public:
  explicit duplication_weedout_ctx(context *parent_arg)
  : context(CTX_DUPLICATES_WEEDOUT, K_DUPLICATES_REMOVAL, parent_arg),
    joinable_ctx(CTX_DUPLICATES_WEEDOUT, K_DUPLICATES_REMOVAL, parent_arg),
    join_ctx(CTX_DUPLICATES_WEEDOUT, K_DUPLICATES_REMOVAL, parent_arg)
  {}

  virtual size_t id(bool hide) { return join_ctx::id(hide); }
  virtual bool cacheable() { return join_ctx::cacheable(); }
  virtual bool dependent() { return join_ctx::dependent(); }

  // Remove warnings: 'inherits ... from ... via dominance'
  virtual bool add_join_tab(joinable_ctx *ctx)
  { return join_ctx::add_join_tab(ctx); }
  virtual bool add_subquery(subquery_list_enum subquery_type, subquery_ctx *ctx)
  { return join_ctx::add_subquery(subquery_type, ctx); }
  virtual int add_where_subquery(subquery_ctx *ctx, SELECT_LEX_UNIT *subquery)
  { return join_ctx::add_where_subquery(ctx, subquery); }
  virtual bool find_and_set_derived(context *subquery)
  { return join_ctx::find_and_set_derived(subquery); }
  virtual bool format_nested_loop(Opt_trace_context *json)
  { return join_ctx::format_nested_loop(json); }
  virtual bool format_unit(Opt_trace_context *json)
  { return unit_ctx::format_unit(json); }
  virtual void set_sort(sort_ctx *ctx)
  { return join_ctx::set_sort(ctx); }
  virtual qep_row *entry()
  { return join_ctx::entry(); }

private:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    obj->add(K_USING_TMP_TABLE, true);
    return format_nested_loop(json);
  }
};


/**
  Node class for UNION (query expression)
*/

class union_ctx : public unit_ctx
{
  union_result_ctx *union_result; ///< associated CTX_UNION_RESULT node
  List<context> query_specs; ///< query specification nodes (inner selects)

public:
  explicit union_ctx(context * parent_arg)
  : context(CTX_UNION, K_QUERY_BLOCK, parent_arg),
    unit_ctx(CTX_UNION, K_QUERY_BLOCK, parent_arg),
    union_result(NULL)
  {}

private:
  virtual bool format_body(Opt_trace_context *json, Opt_trace_object *obj)
  {
    if (union_result)
      return (union_result->format(json)) || format_unit(json);
    else
    {
      /*
        UNION without temporary table. There is no union_result since
        there is no fake_select_lex.
      */
      Opt_trace_object union_res(json, K_UNION_RESULT);
      union_res.add(K_USING_TMP_TABLE, false);
      Opt_trace_array specs(json, K_QUERY_SPECIFICATIONS);
      List_iterator<context> it(query_specs);
      context *ctx;
      while ((ctx= it++))
      {
        if (ctx->format(json))
          return true; /* purecov: inspected */
      }
      return format_unit(json);
    }
  }

public:
  virtual size_t id(bool hide) { return get_id(query_specs, hide); }
  virtual bool cacheable() { return query_specs.head()->cacheable(); }
  virtual bool dependent() { return query_specs.head()->dependent(); }

  virtual void set_union_result(union_result_ctx *ctx)
  {
    DBUG_ASSERT(union_result == NULL);
    union_result= ctx;
    union_result->push_down_query_specs(&query_specs);
  }
  virtual bool add_query_spec(context *ctx)
  { return query_specs.push_back(ctx); }
};

bool union_result_ctx::format(Opt_trace_context *json)
{
  if (order_by_subqueries.is_empty() && optimized_away_subqueries.is_empty())
    return table_base_ctx::format(json);

  Opt_trace_object order_by(json, K_ORDERING_OPERATION);

  order_by.add(K_USING_FILESORT, !order_by_subqueries.is_empty());

  if (table_base_ctx::format(json))
    return true; /* purecov: inspected */

  if (!order_by_subqueries.is_empty() && 
      format_list(json, order_by_subqueries, K_ORDER_BY_SUBQUERIES))
    return true; /* purecov: inspected */

  if (!optimized_away_subqueries.is_empty() &&
      format_list(json, optimized_away_subqueries,
                  K_OPTIMIZED_AWAY_SUBQUERIES))
    return true; /* purecov: inspected */

  return false;
}

} // namespace


qep_row *Explain_format_JSON::entry()
{
  return current_context->entry();
}


bool Explain_format_JSON::begin_context(enum_parsing_context ctx,
                                        SELECT_LEX_UNIT *subquery,
                                        const Explain_format_flags *flags)
{
  using namespace opt_explain_json_namespace;

  context *prev_context= current_context;
  switch(ctx) {
  case CTX_JOIN:
    DBUG_ASSERT(current_context == NULL ||
                // subqueries:
                current_context->type == CTX_SELECT_LIST ||
                current_context->type == CTX_UPDATE_VALUE_LIST ||
                current_context->type == CTX_DERIVED ||
                current_context->type == CTX_OPTIMIZED_AWAY_SUBQUERY ||
                current_context->type == CTX_WHERE ||
                current_context->type == CTX_HAVING ||
                current_context->type == CTX_ORDER_BY_SQ ||
                current_context->type == CTX_GROUP_BY_SQ ||
                current_context->type == CTX_QUERY_SPEC);
    if ((current_context=
         new join_ctx(CTX_JOIN, K_QUERY_BLOCK, current_context)) == NULL)
      return true;
    break;
  case CTX_ORDER_BY:
    { 
      DBUG_ASSERT(current_context->type == CTX_JOIN);
      sort_ctx *ctx= new sort_with_subqueries_ctx(CTX_ORDER_BY,
                                                  K_ORDERING_OPERATION,
                                                  current_context,
                                                  SQ_ORDER_BY, flags,
                                                  ESC_ORDER_BY);
      if (ctx == NULL)
        return true;
      current_context->set_sort(ctx);
      current_context= ctx;
      break;
    }
  case CTX_GROUP_BY:
    { 
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT);
      sort_ctx *ctx= new sort_with_subqueries_ctx(CTX_GROUP_BY,
                                                  K_GROUPING_OPERATION,
                                                  current_context,
                                                  SQ_GROUP_BY, flags,
                                                  ESC_GROUP_BY);
      if (ctx == NULL)
        return true;
      current_context->set_sort(ctx);
      current_context= ctx;
      break;
    }
  case CTX_DISTINCT:
    { 
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_ORDER_BY);
      sort_ctx *ctx= new sort_ctx(CTX_DISTINCT, K_DUPLICATES_REMOVAL,
                                  current_context, flags, ESC_DISTINCT);
      if (ctx == NULL)
        return true;
      current_context->set_sort(ctx);
      current_context= ctx;
      break;
    }
  case CTX_BUFFER_RESULT:
    { 
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT ||
                  current_context->type == CTX_GROUP_BY);
      sort_ctx *ctx= new sort_ctx(CTX_BUFFER_RESULT, K_BUFFER_RESULT,
                                  current_context, flags, ESC_BUFFER_RESULT);
      if (ctx == NULL)
        return true;
      current_context->set_sort(ctx);
      current_context= ctx;
      break;
    }
  case CTX_QEP_TAB:
    {
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_MATERIALIZATION ||
                  current_context->type == CTX_DUPLICATES_WEEDOUT ||
                  current_context->type == CTX_GROUP_BY ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT ||
                  current_context->type == CTX_BUFFER_RESULT ||
                  current_context->type == CTX_SIMPLE_GROUP_BY ||
                  current_context->type == CTX_SIMPLE_ORDER_BY ||
                  current_context->type == CTX_SIMPLE_DISTINCT);
      join_tab_ctx *ctx= new join_tab_ctx(CTX_QEP_TAB, current_context);
      if (ctx == NULL || current_context->add_join_tab(ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_SIMPLE_ORDER_BY:
    {
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_MATERIALIZATION ||
                  current_context->type == CTX_DUPLICATES_WEEDOUT ||
                  current_context->type == CTX_GROUP_BY ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_BUFFER_RESULT ||
                  current_context->type == CTX_DISTINCT);
      simple_sort_ctx *ctx=
        new simple_sort_with_subqueries_ctx(CTX_SIMPLE_ORDER_BY,
                                            K_ORDERING_OPERATION,
                                            current_context, SQ_ORDER_BY, flags,
                                            ESC_ORDER_BY);
                                                          
      if (ctx == NULL || current_context->add_join_tab(ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_SIMPLE_GROUP_BY:
    {
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_MATERIALIZATION ||
                  current_context->type == CTX_DUPLICATES_WEEDOUT ||
                  current_context->type == CTX_GROUP_BY ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT ||
                  current_context->type == CTX_BUFFER_RESULT ||
                  current_context->type == CTX_SIMPLE_ORDER_BY ||
                  current_context->type == CTX_SIMPLE_DISTINCT);
      simple_sort_ctx *ctx=
        new simple_sort_with_subqueries_ctx(CTX_SIMPLE_GROUP_BY,
                                            K_GROUPING_OPERATION,
                                            current_context, SQ_GROUP_BY, flags,
                                            ESC_GROUP_BY);
      if (ctx == NULL || current_context->add_join_tab(ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_SIMPLE_DISTINCT:
    {
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_MATERIALIZATION ||
                  current_context->type == CTX_DUPLICATES_WEEDOUT ||
                  current_context->type == CTX_GROUP_BY ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT ||
                  current_context->type == CTX_BUFFER_RESULT ||
                  current_context->type == CTX_SIMPLE_ORDER_BY);
      simple_sort_ctx *ctx=
        new simple_sort_ctx(CTX_SIMPLE_DISTINCT, K_DUPLICATES_REMOVAL,
                            current_context, flags, ESC_DISTINCT);
      if (ctx == NULL || current_context->add_join_tab(ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_MATERIALIZATION:
    {
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_GROUP_BY ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT ||
                  current_context->type == CTX_BUFFER_RESULT ||
                  current_context->type == CTX_DUPLICATES_WEEDOUT);
      materialize_ctx *ctx= new materialize_ctx(current_context);
      if (ctx == NULL || current_context->add_join_tab(ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_DUPLICATES_WEEDOUT:
    {
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_GROUP_BY ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT ||
                  current_context->type == CTX_BUFFER_RESULT ||
                  current_context->type == CTX_MATERIALIZATION);
      duplication_weedout_ctx *ctx=
        new duplication_weedout_ctx(current_context);
      if (ctx == NULL || current_context->add_join_tab(ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_SELECT_LIST:
    {
      subquery_ctx *ctx= new subquery_ctx(CTX_SELECT_LIST, NULL,
                                          current_context);
      if (ctx == NULL ||
          current_context->add_subquery(SQ_SELECT_LIST, ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_UPDATE_VALUE_LIST:
    {
      subquery_ctx *ctx= new subquery_ctx(CTX_UPDATE_VALUE_LIST, NULL,
                                          current_context);
      if (ctx == NULL ||
          current_context->add_subquery(SQ_UPDATE_VALUE, ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_DERIVED:
    {
      current_context= new subquery_ctx(CTX_DERIVED,
                                        K_MATERIALIZED_FROM_SUBQUERY,
                                        current_context);
      if (current_context == NULL)
        return true;
      break;
    }
  case CTX_OPTIMIZED_AWAY_SUBQUERY:
    {
      subquery_ctx *ctx= new subquery_ctx(CTX_OPTIMIZED_AWAY_SUBQUERY, NULL,
                                          current_context);
      if (ctx == NULL ||
          current_context->add_subquery(SQ_OPTIMIZED_AWAY, ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_WHERE:
    {
      DBUG_ASSERT(subquery != NULL);
      subquery_ctx *ctx= new subquery_ctx(CTX_WHERE, NULL, current_context);
      if (ctx == NULL ||
          current_context->add_where_subquery(ctx, subquery))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_HAVING:
    {
      subquery_ctx *ctx= new subquery_ctx(CTX_HAVING, NULL, current_context);
      if (ctx == NULL || current_context->add_subquery(SQ_HAVING, ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_ORDER_BY_SQ:
    {
      subquery_ctx *ctx= new subquery_ctx(CTX_ORDER_BY_SQ, NULL,
                                              current_context);
      if (ctx == NULL || current_context->add_subquery(SQ_ORDER_BY, ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_GROUP_BY_SQ:
    {
      subquery_ctx *ctx= new subquery_ctx(CTX_GROUP_BY_SQ, NULL,
                                          current_context);
      if (ctx == NULL || current_context->add_subquery(SQ_GROUP_BY, ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_UNION:
    DBUG_ASSERT(current_context == NULL ||
                // subqueries:
                current_context->type == CTX_SELECT_LIST ||
                current_context->type == CTX_UPDATE_VALUE_LIST ||
                current_context->type == CTX_DERIVED ||
                current_context->type == CTX_OPTIMIZED_AWAY_SUBQUERY ||
                current_context->type == CTX_WHERE ||
                current_context->type == CTX_HAVING ||
                current_context->type == CTX_ORDER_BY_SQ ||
                current_context->type == CTX_GROUP_BY_SQ ||
                current_context->type == CTX_QUERY_SPEC);
    current_context= new union_ctx(current_context);
    if (current_context == NULL)
      return true;
    break;
  case CTX_UNION_RESULT:
    {
      DBUG_ASSERT(current_context->type == CTX_UNION);
      union_result_ctx *ctx= new union_result_ctx(current_context);
      if (ctx == NULL)
        return true;
      current_context->set_union_result(ctx);
      current_context= ctx;
      break;
    }
  case CTX_QUERY_SPEC:
    {
      DBUG_ASSERT(current_context->type == CTX_UNION);
      subquery_ctx *ctx= new subquery_ctx(CTX_QUERY_SPEC, NULL,
                                          current_context);
      if (ctx == NULL || current_context->add_query_spec(ctx))
        return true;
      current_context= ctx;
      break;
    }
  case CTX_MESSAGE:
    {
      /*
        Like CTX_QEP_TAB:
      */
      DBUG_ASSERT(current_context->type == CTX_JOIN ||
                  current_context->type == CTX_MATERIALIZATION ||
                  current_context->type == CTX_DUPLICATES_WEEDOUT ||
                  current_context->type == CTX_GROUP_BY ||
                  current_context->type == CTX_ORDER_BY ||
                  current_context->type == CTX_DISTINCT ||
                  current_context->type == CTX_BUFFER_RESULT ||
                  current_context->type == CTX_SIMPLE_GROUP_BY ||
                  current_context->type == CTX_SIMPLE_ORDER_BY ||
                  current_context->type == CTX_SIMPLE_DISTINCT ||
                  current_context->type == CTX_UNION_RESULT);
      joinable_ctx *ctx= new message_ctx(current_context);
      if (ctx == NULL || current_context->add_join_tab(ctx))
        return true;
      current_context= ctx;
      break;
    }
  default:
    DBUG_ASSERT(!"Unknown EXPLAIN context!");
    return true;
  }

  if (prev_context)
    prev_context->set_child(current_context);

  return false;
}


bool Explain_format_JSON::end_context(enum_parsing_context ctx)
{
  DBUG_ASSERT(current_context->type == ctx);

  bool ret= false;
  if (current_context->parent == NULL)
  {
    Item* item;
#ifdef OPTIMIZER_TRACE
    Opt_trace_context json; 
    const size_t max_size= ULONG_MAX;
    if (json.start(true,           // support_I_S (enable JSON generation)
                   false,          // support_dbug_or_missing_priv
                   current_thd->variables.end_markers_in_json, // end_marker
                   false,          // one_line
                   0,              // offset
                   1,              // limit
                   max_size,       // max_mem_size
                   Opt_trace_context::MISC))
      return true;

    {
      Opt_trace_object braces(&json);

      if (current_context->format(&json))
        return true;
    }
    json.end();

    Opt_trace_iterator it(&json);
    if (!it.at_end())
    {
      Opt_trace_info info;
      it.get_value(&info);
      item= new Item_string(info.trace_ptr,
                            static_cast<uint>(info.trace_length),
                            system_charset_info);
    }
    else
#endif
      item= new Item_null();

    List<Item> field_list;
    ret= (item == NULL ||
          field_list.push_back(item) ||
          output->send_data(field_list));
  }
  else if (ctx == CTX_DERIVED)
  {
    if (!current_context->parent->find_and_set_derived(current_context))
    {
      DBUG_ASSERT(!"No derived table found!");
      return true;
    }
  }

  current_context= current_context->parent;
  return ret;
}


bool Explain_format_JSON::send_headers(Query_result *result)
{
  output= result;
  if (Explain_format::send_headers(result))
    return true;

  List<Item> field_list;
  Item *item= new Item_empty_string("EXPLAIN", 78, system_charset_info);
  if (item == NULL || field_list.push_back(item))
    return true;
  return result->send_result_set_metadata(field_list,
                                          Protocol::SEND_NUM_ROWS |
                                          Protocol::SEND_EOF);
}

 