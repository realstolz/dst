/* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; version 2 of the
   License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA */

#include <stdio.h>
#include <string.h>
#include <mysql/plugin.h>
#include <mysql/plugin_audit.h>
#include <mysql/service_my_plugin_log.h>
#include <mysql/service_security_context.h>

static MYSQL_THDVAR_STR(get_field,
                        PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_MEMALLOC,
                        "Get specified security context field.", NULL, NULL, NULL);

static MYSQL_THDVAR_STR(get_value,
                        PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_MEMALLOC,
                        "Get specified security context field value.", NULL, NULL, NULL);

/**
  Tests the security context service

  Do not run this in multple concurrent threads !

  @param thd           the thread to work with
  @param event_class   audit event class
  @param event         event data
*/
static int
test_security_context_notify(MYSQL_THD thd,
                             mysql_event_class_t event_class,
                             const void *event)
{
  if (event_class != MYSQL_AUDIT_COMMAND_CLASS)
  {
    return 0;
  }

  const struct mysql_event_command *event_command=
                                    (const struct mysql_event_command *)event;

  if (event_command->command_id != COM_STMT_PREPARE &&
      event_command->command_id != COM_QUERY)
  {
    return 0;
  }

  int result= 0;
  const char *get_field= (const char *)THDVAR(thd, get_field);
  const char *get_value= (const char *)THDVAR(thd, get_value);
  MYSQL_LEX_CSTRING field_value= { NULL, 0 };
  MYSQL_SECURITY_CONTEXT orig_thd_ctx;
  MYSQL_SECURITY_CONTEXT new_thd_ctx;

  if (get_field == NULL)
  {
    return 0;
  }

  if (thd_get_security_context(thd, &orig_thd_ctx))
  {
    result = 1;
  }

  if (result == 0 && strcmp(get_field, "sec_ctx_test") == 0)
  {
    result= 1;

    /* Security Context Creation. */
    if (!security_context_create(&new_thd_ctx))
    {
      if (!security_context_destroy(new_thd_ctx))
      {
        if (!security_context_copy(orig_thd_ctx, &new_thd_ctx))
        {
          if (!security_context_destroy(new_thd_ctx))
            result= 0;
        }
      }
    }

    THDVAR(thd, get_field)= 0;
    THDVAR(thd, get_value)= 0;

    return result;
  }

  if (result == 0 &&
      security_context_get_option(orig_thd_ctx, get_field, &field_value))
  {
    result= 1;
  }

  if (result == 0 &&
      get_value != NULL && strcmp(field_value.str, get_value) != 0)
  {
    result= 1;
  }

  THDVAR(thd, get_field)= 0;
  THDVAR(thd, get_value)= 0;

  return result;
}


/** Plugin descriptor structure */

static struct st_mysql_audit test_security_context_descriptor=
{
  MYSQL_AUDIT_INTERFACE_VERSION,                    /* interface version    */
  NULL,                                             /* release_thd function */
  test_security_context_notify,                     /* notify function      */
  { 0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (unsigned long) MYSQL_AUDIT_COMMAND_START, }      /* class mask           */
};



static struct st_mysql_sys_var* system_variables[] = {

  MYSQL_SYSVAR(get_field),
  MYSQL_SYSVAR(get_value),
  NULL
};

/** Plugin declaration */

mysql_declare_plugin(test_security_context)
{
  MYSQL_AUDIT_PLUGIN,                 /* type                            */
  &test_security_context_descriptor,  /* descriptor                      */
  "test_security_context",            /* name                            */
  "Oracle Corp",                      /* author                          */
  "Test security context service",    /* description                     */
  PLUGIN_LICENSE_GPL,
  NULL,                               /* init function (when loaded)     */
  NULL,                               /* deinit function (when unloaded) */
  0x0001,                             /* version                         */
  NULL,                               /* status variables                */
  system_variables,                   /* system variables                */
  NULL,
  0,
}
mysql_declare_plugin_end;
 