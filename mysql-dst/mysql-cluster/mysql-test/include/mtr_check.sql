-- Copyright (c) 2008, 2015, Oracle and/or its affiliates. All rights reserved.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; version 2 of the License.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software Foundation,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

delimiter ||;

use mtr||

CREATE DEFINER=root@localhost PROCEDURE check_testcase_perfschema()
BEGIN
  IF ((SELECT count(*) from information_schema.engines
       where engine='PERFORMANCE_SCHEMA' and support='YES') = 1) THEN
  BEGIN

    BEGIN
      -- For tests tampering with performance_schema table structure
      DECLARE CONTINUE HANDLER for SQLEXCEPTION
      BEGIN
      END;

      -- Leave the instruments in the same state
      SELECT * from performance_schema.setup_instruments
        where enabled='NO' order by NAME;
    END;

    -- Leave the consumers in the same state
    SELECT * from performance_schema.setup_consumers
      order by NAME;

    -- Leave the actors setup in the same state
    SELECT * from performance_schema.setup_actors
      order by USER, HOST;

    -- Leave the objects setup in the same state
    SELECT * from performance_schema.setup_objects
      order by OBJECT_TYPE, OBJECT_SCHEMA, OBJECT_NAME;
  END;
  END IF;
END||

--
-- Procedure used to check if server has been properly
-- restored after testcase has been run
--
CREATE DEFINER=root@localhost PROCEDURE check_testcase()
BEGIN

  CALL check_testcase_perfschema();

  -- Dump all global variables except those that may change.
  -- timestamp changes if time passes. server_uuid changes if server restarts.
  SELECT * FROM INFORMATION_SCHEMA.GLOBAL_VARIABLES
    WHERE variable_name NOT IN ('timestamp', 'server_uuid',
                                'innodb_file_format_max',
                                'gtid_executed', 'gtid_purged',
                                'group_replication_group_name')
    ORDER BY VARIABLE_NAME;

  -- Dump all databases, there should be none
  -- except those that was created during bootstrap
  SELECT * FROM INFORMATION_SCHEMA.SCHEMATA;

  -- The test database should not contain any tables
  SELECT table_name AS tables_in_test FROM INFORMATION_SCHEMA.TABLES
    WHERE table_schema='test';

  -- Show "mysql" database, tables and columns
  SELECT CONCAT(table_schema, '.', table_name) AS tables_in_mysql
    FROM INFORMATION_SCHEMA.TABLES
      WHERE table_schema='mysql' AND table_name != 'ndb_apply_status'
        ORDER BY tables_in_mysql;
  SELECT CONCAT(table_schema, '.', table_name) AS columns_in_mysql,
         column_name, ordinal_position, column_default, is_nullable,
         data_type, character_maximum_length, character_octet_length,
         numeric_precision, numeric_scale, character_set_name,
         collation_name, column_type, column_key, extra, column_comment
    FROM INFORMATION_SCHEMA.COLUMNS
      WHERE table_schema='mysql' AND table_name != 'ndb_apply_status'
        ORDER BY columns_in_mysql;

  -- Dump all events, there should be none
  SELECT * FROM INFORMATION_SCHEMA.EVENTS;
  -- Dump all triggers except mtr internals, only those in the sys schema should exist
  -- do not select the CREATED column however, as tests like mysqldump.test / mysql_ugprade.test update this
  SELECT TRIGGER_CATALOG, TRIGGER_SCHEMA, TRIGGER_NAME, EVENT_MANIPULATION,
         EVENT_OBJECT_CATALOG, EVENT_OBJECT_SCHEMA, EVENT_OBJECT_TABLE, ACTION_ORDER, ACTION_CONDITION,
         ACTION_STATEMENT, ACTION_ORIENTATION, ACTION_TIMING ACTION_REFERENCE_OLD_TABLE, ACTION_REFERENCE_NEW_TABLE,
         ACTION_REFERENCE_OLD_ROW, ACTION_REFERENCE_NEW_ROW, SQL_MODE, DEFINER CHARACTER_SET_CLIENT,
         COLLATION_CONNECTION, DATABASE_COLLATION
    FROM INFORMATION_SCHEMA.TRIGGERS
   WHERE TRIGGER_NAME NOT IN ('gs_insert', 'ts_insert');
  -- Dump all created procedures, only those in the sys schema should exist
  -- do not select the CREATED or LAST_ALTERED columns however, as tests like mysqldump.test / mysql_ugprade.test update this
  SELECT SPECIFIC_NAME,ROUTINE_CATALOG,ROUTINE_SCHEMA,ROUTINE_NAME,ROUTINE_TYPE,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH,
         CHARACTER_OCTET_LENGTH,NUMERIC_PRECISION,NUMERIC_SCALE,DATETIME_PRECISION,CHARACTER_SET_NAME,COLLATION_NAME,
         DTD_IDENTIFIER,ROUTINE_BODY,ROUTINE_DEFINITION,EXTERNAL_NAME,EXTERNAL_LANGUAGE,PARAMETER_STYLE,
         IS_DETERMINISTIC,SQL_DATA_ACCESS,SQL_PATH,SECURITY_TYPE,SQL_MODE,ROUTINE_COMMENT,DEFINER,
         CHARACTER_SET_CLIENT,COLLATION_CONNECTION,DATABASE_COLLATION
    FROM INFORMATION_SCHEMA.ROUTINES;
  -- Dump all views, only those in the sys schema should exist
  SELECT * FROM INFORMATION_SCHEMA.VIEWS;

  SHOW GLOBAL STATUS LIKE 'slave_open_temp_tables';

  -- Checksum system tables to make sure they have been properly
  -- restored after test
  -- skip mysql.proc however, as created timestamps may have been updated by mysqldump.test / mysql_ugprade.test
  -- the above SELECT on I_S.ROUTINES ensures consistency across runs instead
  checksum table
    mysql.columns_priv,
    mysql.db,
    mysql.func,
    mysql.help_category,
    mysql.help_keyword,
    mysql.help_relation,
    mysql.host,
    mysql.procs_priv,
    mysql.tables_priv,
    mysql.time_zone,
    mysql.time_zone_leap_second,
    mysql.time_zone_name,
    mysql.time_zone_transition,
    mysql.time_zone_transition_type,
    mysql.user;

END||

--
-- Procedure used by test case used to force all
-- servers to restart after testcase and thus skipping
-- check test case after test
--
CREATE DEFINER=root@localhost PROCEDURE force_restart()
BEGIN
  SELECT 1 INTO OUTFILE 'force_restart';
END||
 