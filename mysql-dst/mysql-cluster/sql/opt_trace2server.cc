/* Copyright (c) 2011, 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA  */

/**
   @file
   Implementation of the Optimizer trace API (WL#5257)
   Helpers connecting the optimizer trace to THD or Information Schema. They
   are dedicated "to the server" (hence the file's name).
   In order to create a unit test of the optimizer trace without defining
   Item_field (and all its parent classes), st_select_lex..., these helpers
   are defined in opt_trace2server.cc.
*/

#include "opt_trace.h"

#include "auth_common.h" // check_table_access
#include "sql_show.h"    // schema_table_stored_record
#include "sql_parse.h"   // sql_command_flags
#include "sp_head.h"     // sp_head

#ifdef OPTIMIZER_TRACE

namespace {

const char I_S_table_name[]= "OPTIMIZER_TRACE";

/* Standalone functions */

/**
   Whether a list of tables contains information_schema.OPTIMIZER_TRACE.
   @param  tbl  list of tables
   @note this does not catch that a stored routine or view accesses
   the OPTIMIZER_TRACE table. So using a stored routine or view to read
   OPTIMIZER_TRACE will overwrite OPTIMIZER_TRACE as it runs and provide
   uninteresting info.
*/
bool list_has_optimizer_trace_table(const TABLE_LIST *tbl)
{
  for( ; tbl ; tbl= tbl->next_global)
  {
    if (tbl->schema_table &&
        0 == strcmp(tbl->schema_table->table_name, I_S_table_name))
      return true;
  }
  return false;
}


/**
   Whether a SQL command qualifies for optimizer tracing.
   @param  sql_command  the command
*/
inline bool sql_command_can_be_traced(enum enum_sql_command sql_command)
{
  /*
    Tracing is limited to a few SQL commands only.

    Reasons to not trace other commands:
    - it reduces the range of potential unknown bugs and misuse
    - they probably don't have anything interesting optimizer-related
    - select_lex for them might be uninitialized and unprintable.
    - SHOW WARNINGS would create an uninteresting trace and thus overwrite the
      previous interesting one.

    About prepared statements: note that we don't turn on tracing for
    SQLCOM_PREPARE (respectively SQLCOM_EXECUTE), because we don't know yet
    what command is being prepared (resp. executed). We turn tracing on later,
    if the prepared (resp. executed) command is in the allowed set above, in
    check_prepared_statement() (resp. mysql_execute_command() called by
    Prepared_statement::execute()).
    PREPARE SELECT is worth tracing as it does permanent query
    transformations.

    Note that SQLCOM_SELECT includes EXPLAIN.
  */
  return (sql_command_flags[sql_command] & CF_OPTIMIZER_TRACE);
}


/// @returns whether this command is "SET ... @@@@OPTIMIZER_TRACE=..."
bool sets_var_optimizer_trace(enum enum_sql_command sql_command,
                              List<set_var_base> *set_vars)
{
  if (sql_command == SQLCOM_SET_OPTION)
  {
    List_iterator_fast<set_var_base> it(*set_vars);
    const set_var_base *var;
    while ((var= it++))
      if (var->is_var_optimizer_trace())
        return true;
  }
  return false;
}

void opt_trace_disable_if_no_tables_access(THD *thd, TABLE_LIST *tbl);

} // namespace


Opt_trace_start::Opt_trace_start(THD *thd, TABLE_LIST *tbl,
                                 enum enum_sql_command sql_command,
                                 List<set_var_base> *set_vars,
                                 const char *query, size_t query_length,
                                 sp_printable *instr,
                                 const CHARSET_INFO *query_charset)
  : ctx(&thd->opt_trace)
{
  DBUG_ENTER("opt_trace_start");

  /*
    By default, we need an optimizer trace:
    - if the user asked for it or
    - if we are using --debug (because the trace serves as a relay for it, for
    optimizer debug printouts).
  */
  const ulonglong var= thd->variables.optimizer_trace;
  bool support_I_S= false, support_dbug_or_missing_priv= false;

  /* This will be triggered if --debug or --debug=d:opt_trace is used */
  DBUG_EXECUTE("opt", support_dbug_or_missing_priv= true;);

  // First step, decide on what type of I_S support we want
  if (unlikely(var & Opt_trace_context::FLAG_ENABLED))
  {
    if (sql_command_can_be_traced(sql_command) &&           // (1)
        !sets_var_optimizer_trace(sql_command, set_vars) && // (2)
        !list_has_optimizer_trace_table(tbl) &&             // (3)
        !thd->system_thread)                                // (4)
    {
      /*
        (1) This command is interesting Optimizer-wise.

        (2) This command is not "SET ... @@optimizer_trace=...". Otherwise,
        this simple usage:
        a) enable opt trace with SET
        b) run SELECT query of interest
        c) disable opt trace with SET
        d) read OPTIMIZER_TRACE table
        would not work: (c) would be traced which would delete the trace of
        (b).

        (3) If a SELECT of I_S.OPTIMIZER_TRACE were traced, it would overwrite
        the interesting trace of the previous statement. Note that
        list_has_optimizer_trace_table() is an expensive function (scanning
        the list of all used tables, doing checks on their names) but we call
        it only if @@optimizer_trace has enabled=on.

        (4) Usage of the trace in a system thread would be
        impractical. Additionally:
        - threads of the Events Scheduler have an unusual security context
        (thd->m_main_security_ctx.priv_user==NULL, see comment in
        Security_context::change_security_context()), so we can do no security
        checks on them, so cannot safely enable tracing.
        - statement-based replication of
        "INSERT INTO real_table SELECT * FROM I_S.OPTIMIZER_TRACE" is
        anyway impossible as @@optimizer_trace* are not replicated, and trace
        would be different between master and slave unless data and engines
        and version of the optimizer are strictly identical.
        - row-based replication of the INSERT SELECT above is still allowed,
        it does not require enabling optimizer trace on the slave.
      */
      support_I_S= true;
    }
    else
    {
      /*
        - statement will not be traced in I_S,
        - if it uses a subquery, this subquery will not be traced,
        - if it uses a stored routine, this routine's substatements may be
        traced.
      */
    }
    /*
      We will do security checks. This is true even in the exceptions
      (1)...(3) above. Otherwise, in:
        SET OPTIMIZER_TRACE="ENABLED=ON";
        SELECT stored_func() FROM INFORMATION_SCHEMA.OPTIMIZER_TRACE;
      (exception 2), we would not check for privilege to do SHOW CREATE on
      stored_func, then we would enter a substatement, which would be traced,
      and would expose the function's body.
      So we will do security checks. So need to inform the trace system that
      it should be ready for a possible call to missing_privilege() later:
    */
    support_dbug_or_missing_priv= true;
  }

  error= ctx->start(support_I_S, support_dbug_or_missing_priv,
                    thd->variables.end_markers_in_json,
                    (var & Opt_trace_context::FLAG_ONE_LINE),
                    thd->variables.optimizer_trace_offset,
                    thd->variables.optimizer_trace_limit,
                    thd->variables.optimizer_trace_max_mem_size,
                    thd->variables.optimizer_trace_features);

  if (likely(!error))
  {
    if (unlikely(support_I_S) && ctx->is_started())
    {
      if (instr != NULL)
      {
        String buffer;
        buffer.set_charset(system_charset_info);
        instr->print(&buffer);
        ctx->set_query(buffer.ptr(), buffer.length(), query_charset);
      }
      else
        ctx->set_query(query, query_length, query_charset);
    }
  }
  opt_trace_disable_if_no_tables_access(thd, tbl);
  DBUG_VOID_RETURN;
}


Opt_trace_start::~Opt_trace_start()
{
  DBUG_ENTER("~opt_trace_start");
  if (likely(!error))
    ctx->end();
  DBUG_VOID_RETURN;
}


void opt_trace_print_expanded_query(THD *thd, st_select_lex *select_lex,
                                    Opt_trace_object *trace_object)

{
  Opt_trace_context * const trace= &thd->opt_trace;
  /**
     It's hard to prove that st_select_lex::print() doesn't modify any of its
     Item-s in a dangerous way. Item_int::print(), for example, modifies its
     internal str_value.
     To make the danger rare, we print the expanded query as rarely as
     possible: only if I_S output is needed. If only --debug is on, we don't
     print it.
     See also the corresponding call to "set_items_ref_array" at end of
     JOIN::exec().
  */
  if (likely(!trace->support_I_S()))
    return;
  char buff[1024];
  String str(buff, sizeof(buff), system_charset_info);
  str.length(0);
  /*
    If this statement is not SELECT, what is shown here can be inexact.
    INSERT SELECT is shown as SELECT. DELETE WHERE is shown as SELECT WHERE.
    This is acceptable given the audience (developers) and the goal (the
    inexact parts are irrelevant for the optimizer).
  */
  select_lex->print(thd, &str, enum_query_type(QT_TO_SYSTEM_CHARSET |
                                               QT_SHOW_SELECT_NUMBER |
                                               QT_NO_DEFAULT_DB));
  trace_object->add_utf8("expanded_query", str.ptr(), str.length());
}


void opt_trace_disable_if_no_security_context_access(THD *thd)
{
#ifndef NO_EMBEDDED_ACCESS_CHECKS
  DBUG_ENTER("opt_trace_check_disable_if_no_security_context_access");
  if (likely(!(thd->variables.optimizer_trace &
               Opt_trace_context::FLAG_ENABLED)) || // (1)
      thd->system_thread)                           // (2)
  {
    /*
      (1) We know that the routine's execution starts with "enabled=off".
      If it stays so until the routine ends, we needn't do security checks on
      the routine.
      If it does not stay so, it means the definer sets it to "on" somewhere
      in the routine's body. Then it is his conscious decision to generate
      traces, thus it is still correct to skip the security check.

      (2) Threads of the Events Scheduler have an unusual security context
      (thd->m_main_security_ctx.priv_user==NULL, see comment in
      Security_context::change_security_context()).
    */
    DBUG_VOID_RETURN;
  }
  Opt_trace_context * const trace= &thd->opt_trace;
  if (!trace->is_started())
  {
    /*
      @@optimizer_trace has "enabled=on" but trace is not started.
      Either Opt_trace_start ctor was not called for our statement (3), or it
      was called but at that time, the variable had "enabled=off" (4).

      There are no known cases of (3).

      (4) suggests that the user managed to change the variable during
      execution of the statement, and this statement is using
      view/routine (note that we have not been able to provoke this, maybe
      this is impossible). If it happens it is suspicious.

      We disable I_S output. And we cannot do otherwise: we have no place to
      store a possible "missing privilege" information (no Opt_trace_stmt, as
      is_started() is false), so cannot do security checks, so cannot safely
      do tracing, so have to disable I_S output. And even then, we don't know
      when to re-enable I_S output, as we have no place to store the
      information "re-enable tracing at the end of this statement", and we
      don't even have a notion of statement here (statements in the optimizer
      trace world mean an Opt_trace_stmt object, and there is none here). So
      we must disable for the session's life.

      COM_FIELD_LIST opens views, thus used to be a case of (3). To avoid
      disabling I_S output for the session's life when this command is issued
      (like in: "SET OPTIMIZER_TRACE='ENABLED=ON';USE somedb;" in the 'mysql'
      command-line client), we have decided to create a Opt_trace_start for
      this command. The command itself is not traced though
      (SQLCOM_SHOW_FIELDS does not have CF_OPTIMIZER_TRACE).
    */
    DBUG_ASSERT(false);
    trace->disable_I_S_for_this_and_children();
    DBUG_VOID_RETURN;
  }
  /*
    Note that thd->m_main_security_ctx.master_access is probably invariant
    accross the life of THD: GRANT/REVOKE don't affect global privileges of an
    existing connection, per the manual.
  */
  if (!(thd->m_main_security_ctx.check_access(GLOBAL_ACLS & ~GRANT_ACL)) &&
      (0 != strcmp(thd->m_main_security_ctx.priv_user().str,
                   thd->security_context()->priv_user().str) ||
       0 != my_strcasecmp(system_charset_info,
                          thd->m_main_security_ctx.priv_host().str,
                          thd->security_context()->priv_host().str)))
    trace->missing_privilege();
  DBUG_VOID_RETURN;
#endif
}


void opt_trace_disable_if_no_stored_proc_func_access(THD *thd, sp_head *sp)
{
#ifndef NO_EMBEDDED_ACCESS_CHECKS
  DBUG_ENTER("opt_trace_disable_if_no_stored_proc_func_access");
  if (likely(!(thd->variables.optimizer_trace &
               Opt_trace_context::FLAG_ENABLED)) || thd->system_thread)
    DBUG_VOID_RETURN;
  Opt_trace_context * const trace= &thd->opt_trace;
  if (!trace->is_started())
  {
    DBUG_ASSERT(false);
    trace->disable_I_S_for_this_and_children();
    DBUG_VOID_RETURN;
  }
  bool full_access;
  Security_context * const backup_thd_sctx= thd->security_context();
  DBUG_PRINT("opt", ("routine: '%s'", sp->m_name.str));
  thd->set_security_context(&thd->m_main_security_ctx);
  const bool rc= sp->check_show_access(thd, &full_access) ||
    !full_access;
  thd->set_security_context(backup_thd_sctx);
  if (rc)
    trace->missing_privilege();
  DBUG_VOID_RETURN;
#endif
}


void opt_trace_disable_if_no_view_access(THD *thd, TABLE_LIST *view,
                                         TABLE_LIST *underlying_tables)
{
#ifndef NO_EMBEDDED_ACCESS_CHECKS
  DBUG_ENTER("opt_trace_disable_if_no_view_access");
  if (likely(!(thd->variables.optimizer_trace &
               Opt_trace_context::FLAG_ENABLED)) || thd->system_thread)
    DBUG_VOID_RETURN;
  Opt_trace_context * const trace= &thd->opt_trace;
  if (!trace->is_started())
  {
    DBUG_ASSERT(false);
    trace->disable_I_S_for_this_and_children();
    DBUG_VOID_RETURN;
  }
  DBUG_PRINT("opt", ("view: '%s'", view->table_name));
  Security_context * const backup_table_sctx= view->security_ctx;
  Security_context * const backup_thd_sctx= thd->security_context();
  const GRANT_INFO backup_grant_info= view->grant;

  view->security_ctx= NULL;                   // no SUID context for view
  // no SUID context for THD
  thd->set_security_context(&thd->m_main_security_ctx);
  const int rc= check_table_access(thd, SHOW_VIEW_ACL, view, false, 1, true);

  view->security_ctx= backup_table_sctx;
  thd->set_security_context(backup_thd_sctx);
  view->grant= backup_grant_info;

  if (rc)
  {
    trace->missing_privilege();
    DBUG_VOID_RETURN;
  }
  /*
    We needn't check SELECT privilege on this view. Some
    opt_trace_disable_if_no_tables_access() call has or will check it.

    Now we check underlying tables/views of our view:
  */
  opt_trace_disable_if_no_tables_access(thd, underlying_tables);
  DBUG_VOID_RETURN;
#endif
}


namespace {

/**
   If tracing is on, checks additional privileges on a list of tables/views,
   to make sure that the user has the right to do SHOW CREATE TABLE/VIEW and
   "SELECT *". For that:
   - this functions checks table-level SELECT
   - which is sufficient for SHOW CREATE TABLE and "SELECT *", if a base table
   - if a view, if the view has not been identified as such then
   opt_trace_disable_if_no_view_access() will be later called and check SHOW
   VIEW; other we check SHOW VIEW here; SHOW VIEW + SELECT is sufficient for
   SHOW CREATE VIEW.
   If a privilege is missing, notifies the trace system.

   @param thd
   @param tbl list of tables to check
*/
void opt_trace_disable_if_no_tables_access(THD *thd, TABLE_LIST *tbl)
{
#ifndef NO_EMBEDDED_ACCESS_CHECKS
  DBUG_ENTER("opt_trace_disable_if_no_tables_access");
  if (likely(!(thd->variables.optimizer_trace &
               Opt_trace_context::FLAG_ENABLED)) || thd->system_thread)
    DBUG_VOID_RETURN;
  Opt_trace_context * const trace= &thd->opt_trace;
  if (!trace->is_started())
  {
    DBUG_ASSERT(false);
    trace->disable_I_S_for_this_and_children();
    DBUG_VOID_RETURN;
  }
  Security_context * const backup_thd_sctx= thd->security_context();
  thd->set_security_context(&thd->m_main_security_ctx);
  const TABLE_LIST * const first_not_own_table=
    thd->lex->first_not_own_table();
  for (TABLE_LIST *t= tbl;
       t != NULL && t != first_not_own_table;
       t= t->next_global)
  {
    DBUG_PRINT("opt", ("table: '%s'", t->table_name));
    /*
      Anonymous derived tables (as in
      "SELECT ... FROM (SELECT ...)") don't have their grant.privilege set.
    */
    if (!t->is_derived())
    {
      const GRANT_INFO backup_grant_info= t->grant;
      Security_context * const backup_table_sctx= t->security_ctx;
      t->security_ctx= NULL;
      /*
        (1) check_table_access() fills t->grant.privilege.
        (2) Because SELECT privileges can be column-based,
        check_table_access() will return 'false' as long as there is SELECT
        privilege on one column. But we want a table-level privilege.
      */

      bool rc=
        check_table_access(thd, SELECT_ACL, t, false, 1, true) || // (1)
        ((t->grant.privilege & SELECT_ACL) == 0); // (2)
      if (t->is_view())
      {
        /*
          It's a view which has already been opened: we are executing a
          prepared statement. The view has been unfolded in the global list of
          tables. So underlying tables will be automatically checked in the
          present function, but we need an explicit check of SHOW VIEW:
        */
        rc|= check_table_access(thd, SHOW_VIEW_ACL, t, false, 1, true);
      }
      t->security_ctx= backup_table_sctx;
      t->grant= backup_grant_info;
      if (rc)
      {
        trace->missing_privilege();
        break;
      }
    }
  }
  thd->set_security_context(backup_thd_sctx);
  DBUG_VOID_RETURN;
#endif
}

} // namespace


int fill_optimizer_trace_info(THD *thd, TABLE_LIST *tables, Item *cond)
{
  TABLE *table= tables->table;
  Opt_trace_info info;

  /*
    When executing a routine which is SQL SECURITY DEFINER, opt-trace specific
    checks are done with the connected user's privileges; this isn't
    respecting the meaning of SQL SECURITY DEFINER. If a highly privileged
    user doesn't know that, he may confidently execute a routine, while this
    routine nastily uses the connected user's privileges to be allowed to do
    tracing and gain knowledge about secret objects.
    This possibility is prevented, by making I_S.OPTIMIZER_TRACE look empty
    when read from a security context which isn't the connected user's
    context; with an exception if the SUID security context has all
    global privileges (in which case the nasty definer has anyway all rights
    to trace everything).

    Objects which are SQL SECURITY INVOKER are not considered here: with or
    without optimizer trace, a highly privileged user must always inspect the
    body of such object before invoking it.
  */
  if (!(thd->security_context()->check_access(GLOBAL_ACLS & ~GRANT_ACL)) &&
      (0 != strcmp(thd->m_main_security_ctx.priv_user().str,
                   thd->security_context()->priv_user().str) ||
       0 != my_strcasecmp(system_charset_info,
                          thd->m_main_security_ctx.priv_host().str,
                          thd->security_context()->priv_host().str)))
    return 0;
  /*
    The list must not change during the iterator's life time. This is ok as
    the life time is only the present block which cannot change the list.
  */
  for (Opt_trace_iterator it(&thd->opt_trace) ; !it.at_end() ; it.next())
  {
    it.get_value(&info);
    restore_record(table, s->default_values);
    /*
      We will put the query, which is in character_set_client, into a column
      using character_set_client; this is better than UTF8 (see BUG#57306).
      When literals with introducers are used, see "LiteralsWithIntroducers"
      in this file.
    */
    table->field[0]->store(info.query_ptr,
                           static_cast<uint>(info.query_length),
                           info.query_charset);
    table->field[1]->store(info.trace_ptr,
                           static_cast<uint>(info.trace_length),
                           system_charset_info);
    table->field[2]->store(info.missing_bytes, true);
    table->field[3]->store(info.missing_priv, true);
    if (schema_table_store_record(thd, table))
      return 1;
  }

  return 0;
}

#endif // OPTIMIZER_TRACE

ST_FIELD_INFO optimizer_trace_info[]=
{
  /* name, length, type, value, maybe_null, old_name, open_method */
  {"QUERY", 65535, MYSQL_TYPE_STRING, 0, false, NULL, SKIP_OPEN_TABLE},
  {"TRACE", 65535, MYSQL_TYPE_STRING, 0, false, NULL, SKIP_OPEN_TABLE},
  {"MISSING_BYTES_BEYOND_MAX_MEM_SIZE", 20, MYSQL_TYPE_LONG,
   0, false, NULL, SKIP_OPEN_TABLE},
  {"INSUFFICIENT_PRIVILEGES", 1, MYSQL_TYPE_TINY,
   0, false, NULL, SKIP_OPEN_TABLE},
  {NULL, 0,  MYSQL_TYPE_STRING, 0, true, NULL, 0}
};


/*
  LiteralsWithIntroducers :

  They may be significantly altered; but this isn't specific to the optimizer
  trace, it also happens with SHOW PROCESSLIST, and is deemed a not too
  important problem.

  Consider
  mysql> set names latin1;
  mysql> SELECT 'í', _cp850'í';
  | í | Ý |
  This sends the binary string:
  SELECT <0xED>, _cp850<0xED>
  to the server (í is 0xED in latin1).
  Now we put this into OPTIMIZER_TRACE.QUERY, using latin1
  (character_set_client), and the client has switched to utf8: we convert the
  query from latin1 to utf8 when sending to client, which receives:
  SELECT <0xC3><0xAD>, _cp850<0xC3><0xAD>
  (í is <0xC3><0xAD> in utf8).
  But <0xC3><0xAD> in _cp850 means a completely different character:
  mysql> set names utf8;
  mysql> SELECT 'í', _cp850'í';
  | í  | ├¡    |

  If the client had originally issued
  SELECT 'í', _cp850 0xED;
  there would be no problem ('0', 'x', 'E', and 'D' are identical in latin1
  and utf8: they would be preserved during conversion).
*/
 