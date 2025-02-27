/* Copyright (c) 2011, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; version 2 of the
   License.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
   02110-1301 USA */

#include "rpl_gtid.h"

#include "rpl_rli.h"                          // Relay_log_info
#include "sql_class.h"                        // THD
#include "sql_parse.h"                        // stmt_causes_implicit_commit

#include "pfs_transaction_provider.h"
#include "mysql/psi/mysql_transaction.h"



bool set_gtid_next(THD *thd, const Gtid_specification &spec)
{
  DBUG_ENTER("set_gtid_next");

  spec.dbug_print();
  global_sid_lock->assert_some_lock();
  int lock_count= 1;
  bool ret= true;

  // we may acquire and release locks throughout this function; this
  // variable tells the error handler how many are left to release

  // Check that we don't own a GTID or ANONYMOUS.
  if (thd->owned_gtid.sidno > 0 ||
      thd->owned_gtid.sidno == THD::OWNED_SIDNO_ANONYMOUS)
  {
    char buf[Gtid::MAX_TEXT_LENGTH + 1];
    if (thd->owned_gtid.sidno > 0)
    {
#ifndef DBUG_OFF
      global_sid_lock->unlock();
      global_sid_lock->wrlock();
      DBUG_ASSERT(gtid_state->get_owned_gtids()->
                  thread_owns_anything(thd->thread_id()));
#endif
      thd->owned_gtid.to_string(thd->owned_sid, buf);
    }
    else
    {
      DBUG_ASSERT(gtid_state->get_anonymous_ownership_count() > 0);
      strcpy(buf, "ANONYMOUS");
    }
    my_error(ER_CANT_SET_GTID_NEXT_WHEN_OWNING_GTID, MYF(0), buf);
    goto err;
  }

  // At this point we should not own any GTID.
  DBUG_ASSERT(thd->owned_gtid.is_empty());

  if (spec.type == AUTOMATIC_GROUP)
  {
    thd->variables.gtid_next.set_automatic();
  }
  else if (spec.type == ANONYMOUS_GROUP)
  {
    if (get_gtid_mode(GTID_MODE_LOCK_SID) == GTID_MODE_ON)
    {
      my_error(ER_CANT_SET_GTID_NEXT_TO_ANONYMOUS_WHEN_GTID_MODE_IS_ON, MYF(0));
      goto err;
    }

    thd->variables.gtid_next.set_anonymous();
    thd->owned_gtid.sidno= THD::OWNED_SIDNO_ANONYMOUS;
    thd->owned_gtid.gno= 0;
    gtid_state->acquire_anonymous_ownership();
  }
  else
  {
    DBUG_ASSERT(spec.type == GTID_GROUP);
    DBUG_ASSERT(spec.gtid.sidno >= 1);
    DBUG_ASSERT(spec.gtid.gno >= 1);
    while (true)
    {
      // loop invariant: we should always hold global_sid_lock.rdlock
      DBUG_ASSERT(lock_count == 1);
      global_sid_lock->assert_some_lock();

      if (get_gtid_mode(GTID_MODE_LOCK_SID) == GTID_MODE_OFF)
      {
        my_error(ER_CANT_SET_GTID_NEXT_TO_GTID_WHEN_GTID_MODE_IS_OFF, MYF(0));
        goto err;
      }

      // acquire lock before checking conditions
      gtid_state->lock_sidno(spec.gtid.sidno);
      lock_count= 2;

      // GTID already logged
      if (gtid_state->is_executed(spec.gtid))
      {
        thd->variables.gtid_next= spec;
        /*
          Don't skip the statement here, skip it in
          gtid_pre_statement_checks.
        */
        break;
      }

      // GTID not owned by anyone: acquire ownership
      if (!gtid_state->is_owned(spec.gtid))
      {
        // acquire_ownership can't fail
        gtid_state->acquire_ownership(thd, spec.gtid);
        thd->variables.gtid_next= spec;
        DBUG_ASSERT(thd->owned_gtid.sidno >= 1);
        DBUG_ASSERT(thd->owned_gtid.gno >= 1);
        break;
      }
      // GTID owned by someone (other thread)
      else
      {
        // The call below releases the read lock on global_sid_lock and
        // the mutex lock on SIDNO.
        gtid_state->wait_for_gtid(thd, spec.gtid);

        // global_sid_lock and mutex are now released
        lock_count= 0;

        // Check if thread was killed.
        if (thd->killed || abort_loop)
        {
          goto err;
        }
#ifdef HAVE_REPLICATION
        // If this thread is a slave SQL thread or slave SQL worker
        // thread, we need this additional condition to determine if it
        // has been stopped by STOP SLAVE [SQL_THREAD].
        if ((thd->system_thread &
             (SYSTEM_THREAD_SLAVE_SQL | SYSTEM_THREAD_SLAVE_WORKER)) != 0)
        {
          // TODO: error is *not* reported on cancel
          DBUG_ASSERT(thd->rli_slave!= NULL);
          Relay_log_info *c_rli= thd->rli_slave->get_c_rli();
          if (c_rli->abort_slave)
          {
            goto err;
          }
        }
#endif // HAVE_REPLICATION
        global_sid_lock->rdlock();
        lock_count= 1;
      }
    }
  }

  ret= false;

err:
  if (lock_count == 2)
    gtid_state->unlock_sidno(spec.gtid.sidno);

  if (lock_count >= 1)
    global_sid_lock->unlock();

  if (!ret)
    gtid_set_performance_schema_values(thd);
  thd->owned_gtid.dbug_print(NULL, "Set owned_gtid in set_gtid_next");

  DBUG_RETURN(ret);
}


/**
  Acquire ownership of all groups in a Gtid_set.  This is used to
  begin a commit-sequence when @@SESSION.GTID_NEXT_LIST != NULL.
*/
#ifdef HAVE_GTID_NEXT_LIST
int gtid_acquire_ownership_multiple(THD *thd)
{
  const Gtid_set *gtid_next_list= thd->get_gtid_next_list_const();
  rpl_sidno greatest_sidno= 0;
  DBUG_ENTER("gtid_acquire_ownership_multiple");
  // first check if we need to wait for any group
  while (true)
  {
    Gtid_set::Gtid_iterator git(gtid_next_list);
    Gtid g= git.get();
    my_thread_id owner= 0;
    rpl_sidno last_sidno= 0;
    global_sid_lock->rdlock();
    while (g.sidno != 0)
    {
      // lock all SIDNOs in order
      if (g.sidno != last_sidno)
        gtid_state->lock_sidno(g.sidno);
      if (!gtid_state->is_executed(g))
      {
        owner= gtid_state->get_owner(g);
        // break the do-loop and wait for the sid to be updated
        if (owner != 0)
        {
          DBUG_ASSERT(owner != thd->id);
          break;
        }
      }
      last_sidno= g.sidno;
      greatest_sidno= g.sidno;
      git.next();
      g= git.get();
    }

    // we don't need to wait for any groups, and all SIDNOs in the
    // set are locked
    if (g.sidno == 0)
      break;

    // unlock all previous sidnos to avoid blocking them
    // while waiting.  keep lock on g.sidno
    for (rpl_sidno sidno= 1; sidno < g.sidno; sidno++)
      if (gtid_next_list->contains_sidno(sidno))
        gtid_state->unlock_sidno(sidno);

    // wait. this call releases the read lock on global_sid_lock and
    // the mutex lock on SIDNO
    gtid_state->wait_for_gtid(thd, g);

    // global_sid_lock and mutex are now released

    // at this point, we don't hold any locks. re-acquire the global
    // read lock that was held when this function was invoked
    if (thd->killed || abort_loop)
      DBUG_RETURN(1);
#ifdef HAVE_REPLICATION
    // If this thread is a slave SQL thread or slave SQL worker
    // thread, we need this additional condition to determine if it
    // has been stopped by STOP SLAVE [SQL_THREAD].
    if ((thd->system_thread &
         (SYSTEM_THREAD_SLAVE_SQL | SYSTEM_THREAD_SLAVE_WORKER)) != 0)
    {
      DBUG_ASSERT(thd->rli_slave != NULL);
      Relay_log_info *c_rli= thd->rli_slave->get_c_rli();
      if (c_rli->abort_slave)
        DBUG_RETURN(1);
    }
#endif // HAVE_REPLICATION
  }

  // global_sid_lock is now held
  thd->owned_gtid_set.ensure_sidno(greatest_sidno);

  /*
    Now the following hold:
     - None of the GTIDs in GTID_NEXT_LIST is owned by any thread.
     - We hold a lock on global_sid_lock.
     - We hold a lock on all SIDNOs in GTID_NEXT_LIST.
    So we acquire ownership of all groups that we need.
  */
  int ret= 0;
  Gtid_set::Gtid_iterator git(gtid_next_list);
  Gtid g= git.get();
  do
  {
    if (!gtid_state->is_executed(g))
    {
      if (gtid_state->acquire_ownership(thd, g) != RETURN_STATUS_OK)
      {
        /// @todo release ownership on error
        ret= 1;
        break;
      }
      thd->owned_gtid_set._add_gtid(g);
    }
    git.next();
    g= git.get();
  } while (g.sidno != 0);

  // unlock all sidnos
  rpl_sidno max_sidno= gtid_next_list->get_max_sidno();
  for (rpl_sidno sidno= 1; sidno <= max_sidno; sidno++)
    if (gtid_next_list->contains_sidno(sidno))
      gtid_state->unlock_sidno(sidno);

  global_sid_lock->unlock();

  /*
    TODO: If this code is enabled, set the GTID in the Performance Schema,
    similar to set_gtid_next().
  */

  DBUG_RETURN(ret);
}
#endif


/**
  Check if current transaction should be skipped, that is, if GTID_NEXT
  was already logged.

  @param  thd    The calling thread.

  @retval true   Transaction was already logged.
  @retval false  Transaction must be executed.
*/
static inline bool is_already_logged_transaction(const THD *thd)
{
  DBUG_ENTER("is_already_logged_transaction");

  const Gtid_specification *gtid_next= &thd->variables.gtid_next;
  const Gtid_set *gtid_next_list= thd->get_gtid_next_list_const();

  if (gtid_next_list == NULL)
  {
    if (gtid_next->type == GTID_GROUP)
    {
      if (thd->owned_gtid.sidno == 0)
        DBUG_RETURN(true);
      else
        DBUG_ASSERT(thd->owned_gtid.equals(gtid_next->gtid));
    }
    else
      DBUG_ASSERT(thd->owned_gtid.sidno == 0 ||
                  thd->owned_gtid.sidno == THD::OWNED_SIDNO_ANONYMOUS);
  }
  else
  {
#ifdef HAVE_GTID_NEXT_LIST
    if (gtid_next->type == GTID_GROUP)
    {
      DBUG_ASSERT(gtid_next_list->contains_gtid(gtid_next->gtid));
      if (!thd->owned_gtid_set.contains_gtid(gtid_next->gtid))
        DBUG_RETURN(true);
    }
#else
    DBUG_ASSERT(0);/*NOTREACHED*/
#endif
  }

  DBUG_RETURN(false);
}


/**
  Debug code executed when a transaction is skipped.

  @param  thd     The calling thread.
*/
static inline void skip_statement(const THD *thd)
{
  DBUG_ENTER("skip_statement");

  DBUG_PRINT("info", ("skipping statement '%s'. "
                      "gtid_next->type=%d sql_command=%d "
                      "thd->thread_id=%u",
                      thd->query().str,
                      thd->variables.gtid_next.type,
                      thd->lex->sql_command,
                      thd->thread_id()));

#ifndef DBUG_OFF
  const Gtid_set* executed_gtids= gtid_state->get_executed_gtids();
  global_sid_lock->rdlock();
  gtid_state->lock_sidno(thd->variables.gtid_next.gtid.sidno);
  DBUG_ASSERT(executed_gtids->contains_gtid(thd->variables.gtid_next.gtid));
  gtid_state->unlock_sidno(thd->variables.gtid_next.gtid.sidno);
  global_sid_lock->unlock();
#endif

  DBUG_VOID_RETURN;
}


bool gtid_reacquire_ownership_if_anonymous(THD *thd)
{
  DBUG_ENTER("gtid_reacquire_ownership_if_anonymous(THD *)");
  Gtid_specification *gtid_next= &thd->variables.gtid_next;
  /*
    When the slave applier thread executes a
    Format_description_log_event originating from a master
    (corresponding to a new master binary log), it sets gtid_next to
    NOT_YET_DETERMINED_GROUP.  This allows any following
    Gtid_log_event to set the GTID appropriately, but if there is no
    Gtid_log_event, gtid_next will be converted to ANONYMOUS.
  */
  DBUG_PRINT("info", ("gtid_next->type=%d gtid_mode=%s",
                      gtid_next->type,
                      get_gtid_mode_string(GTID_MODE_LOCK_NONE)));
  if (gtid_next->type == NOT_YET_DETERMINED_GROUP ||
      (gtid_next->type == ANONYMOUS_GROUP && thd->owned_gtid.sidno == 0))
  {
    Gtid_specification spec;
    spec.set_anonymous();
    DBUG_PRINT("info", ("acquiring ANONYMOUS ownership"));

    global_sid_lock->rdlock();
    // set_gtid_next releases global_sid_lock
    if (set_gtid_next(thd, spec))
      // this can happen if gtid_mode=on
      DBUG_RETURN(true);

#ifdef HAVE_REPLICATION
    thd->set_currently_executing_gtid_for_slave_thread();
#endif
  }
  DBUG_RETURN(false);
}


/**
  Return true if the statement does not invoke any stored function,
  and is one of the following:
  - SET (except SET PASSWORD)
  - SHOW
  - SELECT
  - DO
  - An empty statement because of a skipped version comment
  That means it is guaranteed not to cause any changes in the
  database.
*/
static bool is_stmt_innocent(const THD *thd)
{
  LEX *lex= thd->lex;
  enum_sql_command sql_command= lex->sql_command;
  bool is_show=
    (sql_command_flags[sql_command] & CF_STATUS_COMMAND) &&
    (sql_command != SQLCOM_BINLOG_BASE64_EVENT);
  bool is_set=
    (sql_command == SQLCOM_SET_OPTION) && !lex->is_set_password_sql;
  bool is_select= (sql_command == SQLCOM_SELECT);
  bool is_do= (sql_command == SQLCOM_DO);
  bool is_empty= (sql_command == SQLCOM_EMPTY_QUERY);
  bool is_use= (sql_command == SQLCOM_CHANGE_DB);
  return
    (is_set || is_select || is_do || is_show || is_empty ||
     is_use) &&
    !lex->uses_stored_routines();
}


enum_gtid_statement_status gtid_pre_statement_checks(THD *thd)
{
  DBUG_ENTER("gtid_pre_statement_checks");

  Gtid_specification *gtid_next= &thd->variables.gtid_next;

  DBUG_PRINT("info", ("gtid_next->type=%d "
                      "owned_gtid.{sidno,gno}={%d,%lld}",
                      gtid_next->type,
                      thd->owned_gtid.sidno, thd->owned_gtid.gno));
  DBUG_ASSERT(gtid_next->type != AUTOMATIC_GROUP ||
              thd->owned_gtid.is_empty());

  if ((stmt_causes_implicit_commit(thd, CF_IMPLICIT_COMMIT_BEGIN) ||
       thd->lex->sql_command == SQLCOM_BEGIN) &&
      thd->in_active_multi_stmt_transaction() &&
      gtid_next->type == GTID_GROUP)
  {
    my_error(ER_CANT_DO_IMPLICIT_COMMIT_IN_TRX_WHEN_GTID_NEXT_IS_SET, MYF(0));
    DBUG_RETURN(GTID_STATEMENT_CANCEL);
  }

  /*
    Always allow:
    - BEGIN/COMMIT/ROLLBACK;
    - innocent statements, i.e., SET/SHOW/DO/SELECT which don't invoke
      stored functions.

    @todo: add flag to sql_command_flags to detect if statement
    controls transactions instead of listing the commands in the
    condition below

    @todo: figure out how to handle SQLCOM_XA_*
  */
  const enum_sql_command sql_command= thd->lex->sql_command;
  if (sql_command == SQLCOM_COMMIT || sql_command == SQLCOM_BEGIN ||
      sql_command == SQLCOM_ROLLBACK || is_stmt_innocent(thd))
    DBUG_RETURN(GTID_STATEMENT_EXECUTE);

  /*
    If a transaction updates both non-transactional and transactional
    table; or if it updates more than one non-transactional tables;
    then the transaction must be stopped.  This is the case when on
    master all updated tables are transactional but on slave at least
    one is non-transactional, e.g.:

    On master, tables are transactional:
      CREATE TABLE t1 (a INT) Engine=InnoDB;
      CREATE TABLE t2 (a INT) Engine=InnoDB;
    On slave, one table is non-transactional:
      CREATE TABLE t1 (a INT) Engine=MyISAM;
      CREATE TABLE t2 (a INT) Engine=InnoDB;
    On master, user executes:
      BEGIN;
      INSERT INTO t1 VALUES (1);
      INSERT INTO t2 VALUES (1);
      COMMIT;
    On slave, the second statement must error due to a second statement
    being executed after a statement that updated a non-transactional
    table.
  */
  if (UNDEFINED_GROUP == gtid_next->type)
  {
    char buf[Gtid::MAX_TEXT_LENGTH + 1];
    global_sid_lock->rdlock();
    gtid_next->to_string(global_sid_map, buf);
    global_sid_lock->unlock();
    my_error(ER_GTID_NEXT_TYPE_UNDEFINED_GROUP, MYF(0), buf);
    DBUG_RETURN(GTID_STATEMENT_CANCEL);
  }

  const Gtid_set *gtid_next_list= thd->get_gtid_next_list_const();

  DBUG_PRINT("info", ("gtid_next_list=%p gtid_next->type=%d "
                      "thd->owned_gtid.gtid.{sidno,gno}={%d,%lld} "
                      "thd->thread_id=%u",
                      gtid_next_list, gtid_next->type,
                      thd->owned_gtid.sidno,
                      thd->owned_gtid.gno,
                      thd->thread_id()));

  const bool skip_transaction= is_already_logged_transaction(thd);
  if (gtid_next_list == NULL)
  {
    if (skip_transaction)
    {
      skip_statement(thd);
      DBUG_RETURN(GTID_STATEMENT_SKIP);
    }
    DBUG_RETURN(GTID_STATEMENT_EXECUTE);
  }
  else
  {
#ifdef HAVE_GTID_NEXT_LIST
    switch (gtid_next->type)
    {
    case AUTOMATIC_GROUP:
      my_error(ER_GTID_NEXT_CANT_BE_AUTOMATIC_IF_GTID_NEXT_LIST_IS_NON_NULL,
               MYF(0));
      DBUG_RETURN(GTID_STATEMENT_CANCEL);
    case GTID_GROUP:
      if (skip_transaction)
      {
        skip_statement(thd);
        DBUG_RETURN(GTID_STATEMENT_SKIP);
      }
      /*FALLTHROUGH*/
    case ANONYMOUS_GROUP:
      DBUG_RETURN(GTID_STATEMENT_EXECUTE);
    case INVALID_GROUP:
      DBUG_ASSERT(0);/*NOTREACHED*/
    }
#else
    DBUG_ASSERT(0);/*NOTREACHED*/
#endif
  }
  DBUG_ASSERT(0);/*NOTREACHED*/
  DBUG_RETURN(GTID_STATEMENT_CANCEL);
}


bool gtid_pre_statement_post_implicit_commit_checks(THD *thd)
{
  DBUG_ENTER("gtid_pre_statement_post_implicit_commit_checks");

  /*
    Ensure that we hold anonymous ownership before executing any
    statement, if gtid_next=anonymous or not_yet_determined.  But do
    not re-acquire anonymous ownership if the statement is 'innocent'.
    Innocent commands are those that cannot get written to the binary
    log and cannot commit any ongoing transaction, i.e., one of the
    SET/SELECT/DO/SHOW statements, as long as it does not invoke a
    stored function.

    It is important that we don't try to reacquire ownership for
    innocent commands: SET could be used to set GTID_NEXT to
    UUID:NUMBER; if anonymous was acquired before this then it would
    result in an error.  SHOW/SELECT/DO can be useful for testing
    ownership logic, e.g., to read @@session.gtid_owned or to read
    warnings using SHOW WARNINGS, and to test this properly it is
    important to not affect the ownership status.
  */
  if (!is_stmt_innocent(thd))
    if (gtid_reacquire_ownership_if_anonymous(thd))
      // this can happen if gtid_mode is on
      DBUG_RETURN(true);

  if (!thd->is_ddl_gtid_compatible())
    DBUG_RETURN(true);

  DBUG_RETURN(false);
}


void gtid_set_performance_schema_values(const THD *thd)
{
  DBUG_ENTER("gtid_set_performance_schema_values");
#ifdef HAVE_PSI_TRANSACTION_INTERFACE
  if (thd->m_transaction_psi != NULL)
  {
    Gtid_specification spec;

    // Thread owns GTID.
    if (thd->owned_gtid.sidno >= 1)
    {
      spec.type= GTID_GROUP;
      spec.gtid= thd->owned_gtid;
    }

    // Thread owns ANONYMOUS.
    else if (thd->owned_gtid.sidno == THD::OWNED_SIDNO_ANONYMOUS)
    {
      spec.type= ANONYMOUS_GROUP;
    }

    // Thread does not own anything.
    else
    {
      DBUG_ASSERT(thd->owned_gtid.sidno == 0);
      spec.type= AUTOMATIC_GROUP;
    }
    MYSQL_SET_TRANSACTION_GTID(thd->m_transaction_psi, &thd->owned_sid, &spec);
  }
#endif
  DBUG_VOID_RETURN;
}
 