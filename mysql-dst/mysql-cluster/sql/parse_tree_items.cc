/* Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved.

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

#include "parse_tree_items.h"

#include "parse_tree_nodes.h"
#include "item_cmpfunc.h"          // Item_func_eq

/**
  Helper to resolve the SQL:2003 Syntax exception 1) in <in predicate>.
  See SQL:2003, Part 2, section 8.4 <in predicate>, Note 184, page 383.
  This function returns the proper item for the SQL expression
  <code>left [NOT] IN ( expr )</code>
  @param pc the current parse context
  @param left the in predicand
  @param equal true for IN predicates, false for NOT IN predicates
  @param expr first and only expression of the in value list
  @return an expression representing the IN predicate.
*/
static Item* handle_sql2003_note184_exception(Parse_context *pc, Item* left,
                                              bool equal, Item *expr)
{
  /*
    Relevant references for this issue:
    - SQL:2003, Part 2, section 8.4 <in predicate>, page 383,
    - SQL:2003, Part 2, section 7.2 <row value expression>, page 296,
    - SQL:2003, Part 2, section 6.3 <value expression primary>, page 174,
    - SQL:2003, Part 2, section 7.15 <subquery>, page 370,
    - SQL:2003 Feature F561, "Full value expressions".

    The exception in SQL:2003 Note 184 means:
    Item_singlerow_subselect, which corresponds to a <scalar subquery>,
    should be re-interpreted as an Item_in_subselect, which corresponds
    to a <table subquery> when used inside an <in predicate>.

    Our reading of Note 184 is reccursive, so that all:
    - IN (( <subquery> ))
    - IN ((( <subquery> )))
    - IN '('^N <subquery> ')'^N
    - etc
    should be interpreted as a <table subquery>, no matter how deep in the
    expression the <subquery> is.
  */

  Item *result;

  DBUG_ENTER("handle_sql2003_note184_exception");

  if (expr->type() == Item::SUBSELECT_ITEM)
  {
    Item_subselect *expr2 = (Item_subselect*) expr;

    if (expr2->substype() == Item_subselect::SINGLEROW_SUBS)
    {
      Item_singlerow_subselect *expr3 = (Item_singlerow_subselect*) expr2;
      st_select_lex *subselect;

      /*
        Implement the mandated change, by altering the semantic tree:
          left IN Item_singlerow_subselect(subselect)
        is modified to
          left IN (subselect)
        which is represented as
          Item_in_subselect(left, subselect)
      */
      subselect= expr3->invalidate_and_restore_select_lex();
      result= new (pc->mem_root) Item_in_subselect(left, subselect);

      if (! equal)
        result = negate_expression(pc, result);

      DBUG_RETURN(result);
    }
  }

  if (equal)
    result= new (pc->mem_root) Item_func_eq(left, expr);
  else
    result= new (pc->mem_root) Item_func_ne(left, expr);

  DBUG_RETURN(result);
}


bool PTI_table_wild::itemize(Parse_context *pc, Item **item)
{
  if (super::itemize(pc, item))
    return true;

  schema=
    pc->thd->get_protocol()->has_client_capability(CLIENT_NO_SCHEMA) ? NullS : schema;
  *item= new (pc->mem_root) Item_field(POS(), schema, table, "*");
  if (*item == NULL || (*item)->itemize(pc, item))
    return true;
  pc->select->with_wild++;
  return false;
}


bool PTI_comp_op::itemize(Parse_context *pc, Item **res)
{
  if (super::itemize(pc, res) ||
      left->itemize(pc, &left) || right->itemize(pc, &right))
    return true;

  *res= (*boolfunc2creator)(0)->create(left, right);
  return *res == NULL;
}


bool PTI_comp_op_all::itemize(Parse_context *pc, Item **res)
{
  if (super::itemize(pc, res) ||
      left->itemize(pc, &left) || subselect->contextualize(pc))
    return true;

  *res= all_any_subquery_creator(left, comp_op, is_all, subselect->value);
  return false;
}


bool PTI_udf_expr::itemize(Parse_context *pc, Item **res)
{
  if (super::itemize(pc, res) || expr->itemize(pc, &expr))
    return true;
  /*
   Use Item::name as a storage for the attribute value of user
   defined function argument. It is safe to use Item::name
   because the syntax will not allow having an explicit name here.
   See WL#1017 re. udf attributes.
  */
  if (select_alias.str)
  {
    expr->item_name.copy(select_alias.str, select_alias.length,
                         system_charset_info, false);
  }
  /* 
     A field has to have its proper name in order for name
     resolution to work, something we are only guaranteed if we
     parse it out. If we hijack the input stream with
     [@1.cpp.start ... @1.cpp.end) we may get quoted or escaped names.
  */
  else if (expr->type() != Item::FIELD_ITEM &&
           expr->type() != Item::REF_ITEM /* For HAVING */ )
    expr->item_name.copy(expr_loc.start, expr_loc.length(), pc->thd->charset());
  *res= expr;
  return false;
};


bool PTI_singlerow_subselect::itemize(Parse_context *pc, Item **res)
{
  if (super::itemize(pc, res) || subselect->contextualize(pc))
    return true;
  *res= new (pc->mem_root) Item_singlerow_subselect(subselect->value);
  return *res == NULL;
}


bool PTI_exists_subselect::itemize(Parse_context *pc, Item **res)
{
  if (super::itemize(pc, res) || subselect->contextualize(pc))
    return true;
  *res= new (pc->mem_root) Item_exists_subselect(subselect->value);
  return *res == NULL;
}


bool PTI_handle_sql2003_note184_exception::itemize(Parse_context *pc,
                                                   Item **res)
{
  if (super::itemize(pc, res) || left->itemize(pc, &left) ||
      right->itemize(pc, &right))
    return true;
  *res= handle_sql2003_note184_exception(pc, left, is_negation, right);
  return *res == NULL;
}


bool PTI_expr_with_alias::itemize(Parse_context *pc, Item **res)
{
  if (super::itemize(pc, res) || expr->itemize(pc, &expr))
    return true;

  if (alias.str)
  {
    if (pc->thd->lex->sql_command == SQLCOM_CREATE_VIEW &&
        check_column_name(alias.str))
    {
      my_error(ER_WRONG_COLUMN_NAME, MYF(0), alias.str);
      return true;
    }
    expr->item_name.copy(alias.str, alias.length, system_charset_info, false);
  }
  else if (!expr->item_name.is_set())
  {
    expr->item_name.copy(expr_loc.start, (uint) (expr_loc.end - expr_loc.start),
                         pc->thd->charset());
  }
  *res= expr;
  return false;
}


bool PTI_simple_ident_ident::itemize(Parse_context *pc, Item **res)
{
  if (super::itemize(pc, res))
    return true;

  THD *thd= pc->thd;
  LEX *lex= thd->lex;
  sp_pcontext *pctx = lex->get_sp_current_parsing_ctx();
  sp_variable *spv;

  if (pctx && (spv= pctx->find_variable(ident, false)))
  {
    sp_head *sp= lex->sphead;

    DBUG_ASSERT(sp);

    /* We're compiling a stored procedure and found a variable */
    if (! lex->parsing_options.allows_variable)
    {
      my_error(ER_VIEW_SELECT_VARIABLE, MYF(0));
      return true;
    }

    *res=
      create_item_for_sp_var(
        thd, ident, spv,
        sp->m_parser_data.get_current_stmt_start_ptr(),
        raw.start,
        raw.end);
    lex->safe_to_cache_query= false;
  }
  else
  {
    if ((pc->select->parsing_place != CTX_HAVING) ||
        (pc->select->get_in_sum_expr() > 0))
    {
      *res= new (pc->mem_root) Item_field(POS(), NullS, NullS, ident.str);
    }
    else
    {
      *res= new (pc->mem_root) Item_ref(POS(), NullS, NullS, ident.str);
    }
    if (*res == NULL || (*res)->itemize(pc, res))
      return true;
  }
  return *res == NULL;
}


bool PTI_simple_ident_q_2d::itemize(Parse_context *pc, Item **res)
{
  THD *thd= pc->thd;
  LEX *lex= thd->lex;
  sp_head *sp= lex->sphead;

  /*
    FIXME This will work ok in simple_ident_nospvar case because
    we can't meet simple_ident_nospvar in trigger now. But it
    should be changed in future.
  */
  if (sp && sp->m_type == SP_TYPE_TRIGGER &&
      (!my_strcasecmp(system_charset_info, table, "NEW") ||
       !my_strcasecmp(system_charset_info, table, "OLD")))
  {
    if (Parse_tree_item::itemize(pc, res))
      return true;

    bool new_row= (table[0]=='N' || table[0]=='n');

    if (sp->m_trg_chistics.event == TRG_EVENT_INSERT &&
        !new_row)
    {
      my_error(ER_TRG_NO_SUCH_ROW_IN_TRG, MYF(0), "OLD", "on INSERT");
      return true;
    }

    if (sp->m_trg_chistics.event == TRG_EVENT_DELETE &&
        new_row)
    {
      my_error(ER_TRG_NO_SUCH_ROW_IN_TRG, MYF(0), "NEW", "on DELETE");
      return true;
    }

    DBUG_ASSERT(!new_row ||
                (sp->m_trg_chistics.event == TRG_EVENT_INSERT ||
                 sp->m_trg_chistics.event == TRG_EVENT_UPDATE));
    const bool read_only=
      !(new_row && sp->m_trg_chistics.action_time == TRG_ACTION_BEFORE);
    Item_trigger_field *trg_fld= new (pc->mem_root)
               Item_trigger_field(POS(),
                                  new_row ? TRG_NEW_ROW : TRG_OLD_ROW,
                                  field,
                                  SELECT_ACL,
                                  read_only);
    if (trg_fld == NULL || trg_fld->itemize(pc, (Item **) &trg_fld))
      return true;
    DBUG_ASSERT(trg_fld->type() == TRIGGER_FIELD_ITEM);

    /*
      Let us add this item to list of all Item_trigger_field objects
      in trigger.
    */
    lex->sphead->m_cur_instr_trig_field_items.link_in_list(
      trg_fld, &trg_fld->next_trg_field);

    *res= trg_fld;
  }
  else
  {
    if (super::itemize(pc, res))
      return true;
  }
  return false;
}
 