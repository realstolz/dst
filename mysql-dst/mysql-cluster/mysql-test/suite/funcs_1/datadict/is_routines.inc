# suite/funcs_1/datadict/is_routines.inc
#
# Check the layout of information_schema.routines and the impact of
# CREATE/ALTER/DROP PROCEDURE/FUNCTION ... on it.
#
# Note:
#    This test is not intended
#    - to show information about the all time existing routines (there are no
#      in the moment) within the databases information_schema and mysql
#    - for checking storage engine properties
#
# Author:
# 2008-01-23 mleich WL#4203 Reorganize and fix the data dictionary tests of
#                           testsuite funcs_1
#                   Create this script based on older scripts and new code.
# Last Change:
# 2008-06-11 mleich Move t/is_routines.test to this file and
#                   create variants for embedded/non embedded server.
#

let $is_table = ROUTINES;

# The table INFORMATION_SCHEMA.TABLES must exist
eval SHOW TABLES FROM information_schema LIKE '$is_table';

--echo #######################################################################
--echo # Testcase 3.2.1.1: INFORMATION_SCHEMA tables can be queried via SELECT
--echo #######################################################################
# Ensure that every INFORMATION_SCHEMA table can be queried with a SELECT
# statement, just as if it were an ordinary user-defined table.
#
--source suite/funcs_1/datadict/is_table_query.inc


--echo #########################################################################
--echo # Testcase 3.2.8.1: INFORMATION_SCHEMA.ROUTINES layout
--echo #########################################################################
# Ensure that the INFORMATION_SCHEMA.ROUTINES table has the following columns,
# in the following order:
#
# SPECIFIC_NAME (shows the name of an accessible stored procedure, or routine),
# ROUTINE_CATALOG (always shows NULL),
# ROUTINE_SCHEMA (shows the database, or schema, in which the routine resides),
# ROUTINE_NAME (shows the same stored procedure name),
# ROUTINE_TYPE (shows whether the stored procedure is a procedure or a function),
# DTD_IDENTIFIER (shows, for a function, the complete data type definition of
#         the value the function will return; otherwise NULL),
# ROUTINE_BODY (shows the language in which the stored procedure is written;
#         currently always SQL),
# ROUTINE_DEFINITION (shows as much of the routine body as is possible in the
#         allotted space),
# EXTERNAL_NAME (always shows NULL),
# EXTERNAL_LANGUAGE (always shows NULL),
# PARAMETER_STYLE (shows the routine's parameter style; always SQL),
# IS_DETERMINISTIC (shows whether the routine is deterministic),
# SQL_DATA_ACCESS (shows the routine's defined sql-data-access clause value),
# SQL_PATH (always shows NULL),
# SECURITY_TYPE (shows whether the routine's defined security_type is 'definer'
#         or 'invoker'),
# CREATED (shows the timestamp of the time the routine was created),
# LAST_ALTERED (shows the timestamp of the time the routine was last altered),
# SQL_MODE (shows the sql_mode setting at the time the routine was created),
# ROUTINE_COMMENT (shows the comment, if any, defined for the routine;
#         otherwise NULL),
# DEFINER (shows the user who created the routine).
# Starting with MySQL 5.1
# CHARACTER_SET_CLIENT
# COLLATION_CONNECTION
# DATABASE_COLLATION
#
--source suite/funcs_1/datadict/datadict_bug_12777.inc
eval DESCRIBE          information_schema.$is_table;
--source suite/funcs_1/datadict/datadict_bug_12777.inc
--replace_result InnoDB TMP_TABLE_ENGINE MyISAM TMP_TABLE_ENGINE 
eval SHOW CREATE TABLE information_schema.$is_table;
--source suite/funcs_1/datadict/datadict_bug_12777.inc
eval SHOW COLUMNS FROM information_schema.$is_table;

USE test;
--disable_warnings
DROP PROCEDURE IF EXISTS sp_for_routines;
DROP FUNCTION  IF EXISTS function_for_routines;
--enable_warnings
CREATE PROCEDURE sp_for_routines()      SELECT 'db_datadict';
CREATE FUNCTION function_for_routines() RETURNS INT RETURN 0;

# Show that the column values of
#   ROUTINE_CATALOG, EXTERNAL_NAME, EXTERNAL_LANGUAGE, SQL_PATH are always NULL
# and
#   ROUTINE_BODY, PARAMETER_STYLE are 'SQL'
# and
#   SPECIFIC_NAME = ROUTINE_NAME.
SELECT specific_name,routine_catalog,routine_schema,routine_name,routine_type,
       routine_body,external_name,external_language,parameter_style,sql_path
FROM information_schema.routines
WHERE routine_schema = 'test' AND
   (routine_catalog   IS NOT NULL OR external_name   IS NOT NULL
   OR external_language IS NOT NULL OR sql_path        IS NOT NULL
   OR routine_body      <> 'SQL'    OR parameter_style <> 'SQL'
   OR specific_name     <> routine_name);

DROP PROCEDURE sp_for_routines;
DROP FUNCTION  function_for_routines;


--echo ################################################################################
--echo # Testcase 3.2.8.2 + 3.2.8.3: INFORMATION_SCHEMA.ROUTINES accessible information
--echo ################################################################################
# 3.2.8.2:  Ensure that the table shows the relevant information on every SQL-invoked
#           routine (i.e. stored procedure) which is accessible to the current user
#           or to PUBLIC.
# 3.2.8.3:  Ensure that the table does not show any information on any stored procedure
#           that is not accessible to the current user or PUBLIC.
#
--disable_warnings
DROP DATABASE IF EXISTS db_datadict;
DROP DATABASE IF EXISTS db_datadict_2;
--enable_warnings

CREATE DATABASE db_datadict;
USE db_datadict;
--replace_result $other_engine_type <other_engine_type>
eval
CREATE TABLE res_6_408002_1(f1 CHAR(3), f2 TEXT(25), f3 DATE, f4 INT)
ENGINE = $other_engine_type;
INSERT INTO res_6_408002_1(f1, f2, f3, f4)
VALUES('abc', 'xyz', '1989-11-09', 0815);
--disable_warnings
DROP PROCEDURE IF EXISTS sp_6_408002_1;
--enable_warnings
delimiter //;
CREATE PROCEDURE sp_6_408002_1()
BEGIN
   SELECT * FROM db_datadict.res_6_408002_1;
END//
delimiter ;//

CREATE DATABASE db_datadict_2;
USE db_datadict_2;
--replace_result $other_engine_type <other_engine_type>
eval
CREATE TABLE res_6_408002_2(f1 CHAR(3), f2 TEXT(25), f3 DATE, f4 INT)
ENGINE = $other_engine_type;
INSERT INTO res_6_408002_2(f1, f2, f3, f4)
VALUES('abc', 'xyz', '1990-10-03', 4711);
--disable_warnings
DROP PROCEDURE IF EXISTS sp_6_408002_2;
--enable_warnings
delimiter //;
CREATE PROCEDURE sp_6_408002_2()
BEGIN
   SELECT * FROM db_datadict_2.res_6_408002_2;
END//
delimiter ;//

--error 0,ER_CANNOT_USER
DROP   USER 'testuser1'@'localhost';
CREATE USER 'testuser1'@'localhost';
--error 0,ER_CANNOT_USER
DROP   USER 'testuser2'@'localhost';
CREATE USER 'testuser2'@'localhost';
--error 0,ER_CANNOT_USER
DROP   USER 'testuser3'@'localhost';
CREATE USER 'testuser3'@'localhost';


GRANT SELECT  ON db_datadict_2.* TO 'testuser1'@'localhost';
GRANT EXECUTE ON db_datadict_2.* TO 'testuser1'@'localhost';

GRANT EXECUTE ON db_datadict.*   TO 'testuser1'@'localhost';
GRANT SELECT  ON db_datadict.*   TO 'testuser2'@'localhost';

GRANT EXECUTE ON PROCEDURE db_datadict_2.sp_6_408002_2
TO 'testuser2'@'localhost';
GRANT EXECUTE ON db_datadict_2.* TO 'testuser2'@'localhost';
FLUSH PRIVILEGES;

--echo # Establish connection testuser1 (user=testuser1)
--replace_result $MASTER_MYPORT MYSQL_PORT $MASTER_MYSOCK MYSQL_SOCK
connect (testuser1, localhost, testuser1, , db_datadict);
--replace_column 24 "YYYY-MM-DD hh:mm:ss" 25 "YYYY-MM-DD hh:mm:ss"
SELECT * FROM information_schema.routines;

--echo # Establish connection testuser2 (user=testuser2)
--replace_result $MASTER_MYPORT MYSQL_PORT $MASTER_MYSOCK MYSQL_SOCK
connect (testuser2, localhost, testuser2, , db_datadict);
--replace_column 24 "YYYY-MM-DD hh:mm:ss" 25 "YYYY-MM-DD hh:mm:ss"
SELECT * FROM information_schema.routines;

--echo # Establish connection testuser3 (user=testuser3)
--replace_result $MASTER_MYPORT MYSQL_PORT $MASTER_MYSOCK MYSQL_SOCK
connect (testuser3, localhost, testuser3, , test);
--replace_column 24 "YYYY-MM-DD hh:mm:ss" 25 "YYYY-MM-DD hh:mm:ss"
SELECT * FROM information_schema.routines;

# Cleanup
--echo # Switch to connection default and close connections testuser1,testuser2,testuser3
connection default;
disconnect testuser1;
disconnect testuser2;
disconnect testuser3;

DROP USER 'testuser1'@'localhost';
DROP USER 'testuser2'@'localhost';
DROP USER 'testuser3'@'localhost';

USE test;
DROP DATABASE db_datadict;
DROP DATABASE db_datadict_2;


--echo #########################################################################
--echo # 3.2.1.13+3.2.1.14+3.2.1.15: INFORMATION_SCHEMA.ROUTINES modifications
--echo #########################################################################
# 3.2.1.13: Ensure that the creation of any new database object (e.g. table or
#           column) automatically inserts all relevant information on that
#           object into every appropriate INFORMATION_SCHEMA table.
# 3.2.1.14: Ensure that the alteration of any existing database object
#           automatically updates all relevant information on that object in
#           every appropriate INFORMATION_SCHEMA table.
# 3.2.1.15: Ensure that the dropping of any existing database object
#           automatically deletes all relevant information on that object from
#           every appropriate INFORMATION_SCHEMA table.
#
# Some more tests are in t/information_schema_routines.test which exists
# in MySQL 5.1 and up only.
#
--disable_warnings
DROP DATABASE IF EXISTS db_datadict;
--enable_warnings
CREATE DATABASE db_datadict;

SELECT * FROM information_schema.routines WHERE routine_schema = 'db_datadict';
USE db_datadict;
CREATE PROCEDURE sp_for_routines()      SELECT 'db_datadict';
CREATE FUNCTION function_for_routines() RETURNS INT RETURN 0;
--vertical_results
--replace_column 24 <created> 25 <modified>
SELECT * FROM information_schema.routines WHERE routine_schema = 'db_datadict'
ORDER BY routine_name;
--horizontal_results

ALTER PROCEDURE sp_for_routines SQL SECURITY INVOKER;
ALTER FUNCTION function_for_routines COMMENT 'updated comments';
--vertical_results
--replace_column 24 <created> 25 <modified>
SELECT * FROM information_schema.routines WHERE routine_schema = 'db_datadict'
ORDER BY routine_name;
--horizontal_results

DROP PROCEDURE sp_for_routines;
DROP FUNCTION function_for_routines;
SELECT * FROM information_schema.routines WHERE routine_schema = 'db_datadict';

CREATE PROCEDURE sp_for_routines()      SELECT 'db_datadict';
CREATE FUNCTION function_for_routines() RETURNS INT RETURN 0;
--vertical_results
--replace_column 24 <created> 25 <modified>
SELECT * FROM information_schema.routines WHERE routine_schema = 'db_datadict'
ORDER BY routine_name;
--horizontal_results
use test;
DROP DATABASE db_datadict;
SELECT * FROM information_schema.routines WHERE routine_schema = 'db_datadict';


--echo #########################################################################
--echo # 3.2.8.4: INFORMATION_SCHEMA.ROUTINES routine body too big for
--echo #          ROUTINE_DEFINITION column
--echo #########################################################################
# Ensure that a stored procedure with a routine body that is too large to fit
# into the INFORMATION_SCHEMA.ROUTINES.ROUTINE_DEFINITION column correctly shows
# as much of the information as is possible within the allotted size.
#
--disable_warnings
DROP DATABASE IF EXISTS db_datadict;
--enable_warnings
CREATE DATABASE db_datadict;
USE db_datadict;
#
--replace_result $other_engine_type <other_engine_type>
eval
CREATE TABLE db_datadict.res_6_408004_1
       (f1 LONGTEXT , f2 MEDIUMINT , f3 LONGBLOB , f4 REAL , f5 YEAR)
ENGINE = $other_engine_type;
INSERT INTO db_datadict.res_6_408004_1
VALUES ('abc', 98765 , 99999999 , 98765, 10);
#
--replace_result $other_engine_type <other_engine_type>
eval
CREATE TABLE db_datadict.res_6_408004_2
       (f1 LONGTEXT , f2 MEDIUMINT , f3 LONGBLOB , f4 REAL , f5 YEAR)
ENGINE = $other_engine_type;
INSERT INTO db_datadict.res_6_408004_2
VALUES ('abc', 98765 , 99999999 , 98765, 10);

--echo # Checking the max. possible length of (currently) 4 GByte is not
--echo # in this environment here.

delimiter //;
CREATE PROCEDURE sp_6_408004 ()
BEGIN
   DECLARE done INTEGER DEFAULt 0;
   DECLARE variable_number_1 LONGTEXT;
   DECLARE variable_number_2 MEDIUMINT;
   DECLARE variable_number_3 LONGBLOB;
   DECLARE variable_number_4 REAL;
   DECLARE variable_number_5 YEAR;
   DECLARE cursor_number_1 CURSOR FOR SELECT * FROM res_6_408004_1 LIMIT 0, 10;
   DECLARE cursor_number_2 CURSOR FOR SELECT * FROM res_6_408004_1 LIMIT 0, 10;
   DECLARE cursor_number_3 CURSOR FOR SELECT * FROM res_6_408004_1 LIMIT 0, 10;
   DECLARE cursor_number_4 CURSOR FOR SELECT * FROM res_6_408004_1 LIMIT 0, 10;
   DECLARE cursor_number_5 CURSOR FOR SELECT * FROM res_6_408004_1 LIMIT 0, 10;
   DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET done = 1;
   BEGIN
      OPEN cursor_number_1;
      WHILE done <> 1 DO
         FETCH cursor_number_1
         INTO variable_number_1, variable_number_2, variable_number_3,
              variable_number_4, variable_number_5;
         IF done <> 0 THEN
            INSERT INTO res_6_408004_2
            VALUES (variable_number_1, variable_number_2, variable_number_3,
                    variable_number_4, variable_number_5);
         END IF;
      END WHILE;
      BEGIN
         BEGIN
            SET done = 0;
            OPEN cursor_number_2;
            WHILE done <> 1 DO
               FETCH cursor_number_2
               INTO variable_number_1, variable_number_2, variable_number_3,
                    variable_number_4, variable_number_5;
               IF done <> 0 THEN
                  INSERT INTO res_6_408004_2
                  VALUES(variable_number_1, variable_number_2, variable_number_3,
                         variable_number_4, variable_number_5);
               END IF;
            END WHILE;
         END;
         SET done = 0;
         OPEN cursor_number_3;
         WHILE done <> 1 DO
            FETCH cursor_number_3
            INTO variable_number_1, variable_number_2, variable_number_3,
                 variable_number_4, variable_number_5;
            IF done <> 0 THEN
               INSERT INTO res_6_408004_2
               VALUES(variable_number_1, variable_number_2, variable_number_3,
                      variable_number_4, variable_number_5);
            END IF;
         END WHILE;
      END;
   END;
   BEGIN
      SET done = 0;
      OPEN cursor_number_4;
      WHILE done <> 1 DO
         FETCH cursor_number_4
         INTO variable_number_1, variable_number_2, variable_number_3,
              variable_number_4, variable_number_5;
         IF done <> 0 THEN
            INSERT INTO res_6_408004_2
            VALUES (variable_number_1, variable_number_2, variable_number_3,
                    variable_number_4, variable_number_5);
         END IF;
      END WHILE;
   END;
   BEGIN
      SET @a='test row';
      SELECT @a;
      SELECT @a;
      SELECT @a;
   END;
   BEGIN
      SET done = 0;
      OPEN cursor_number_5;
      WHILE done <> 1 DO
         FETCH cursor_number_5
         INTO variable_number_1, variable_number_2, variable_number_3,
              variable_number_4, variable_number_5;
         IF done <> 0 THEN
            INSERT INTO res_6_408004_2
            VALUES (variable_number_1, variable_number_2, variable_number_3,
                    variable_number_4, variable_number_5);
         END IF;
      END WHILE;
   END;
   BEGIN
      SET @a='test row';
      SELECT @a;
      SELECT @a;
      SELECT @a;
   END;
END//
delimiter ;//

CALL db_datadict.sp_6_408004 ();
SELECT * FROM db_datadict.res_6_408004_2;

--vertical_results
--replace_column 24 "YYYY-MM-DD hh:mm:ss" 25 "YYYY-MM-DD hh:mm:ss"
SELECT *, LENGTH(routine_definition) FROM information_schema.routines
WHERE routine_schema = 'db_datadict';
--horizontal_results

# Cleanup
DROP DATABASE db_datadict;
# ----------------------------------------------------------------------------------------------


--echo ########################################################################
--echo # Testcases 3.2.1.3-3.2.1.5 + 3.2.1.8-3.2.1.12: INSERT/UPDATE/DELETE and
--echo #           DDL on INFORMATION_SCHEMA table are not supported
--echo ########################################################################
# 3.2.1.3:  Ensure that no user may execute an INSERT statement on any
#           INFORMATION_SCHEMA table.
# 3.2.1.4:  Ensure that no user may execute an UPDATE statement on any
#           INFORMATION_SCHEMA table.
# 3.2.1.5:  Ensure that no user may execute a DELETE statement on any
#           INFORMATION_SCHEMA table.
# 3.2.1.8:  Ensure that no user may create an index on an INFORMATION_SCHEMA table.
# 3.2.1.9:  Ensure that no user may alter the definition of an
#           INFORMATION_SCHEMA table.
# 3.2.1.10: Ensure that no user may drop an INFORMATION_SCHEMA table.
# 3.2.1.11: Ensure that no user may move an INFORMATION_SCHEMA table to any
#           other database.
# 3.2.1.12: Ensure that no user may directly add to, alter, or delete any data
#           in an INFORMATION_SCHEMA table.
#
--disable_warnings
DROP DATABASE IF EXISTS db_datadict;
--enable_warnings
CREATE DATABASE db_datadict;
USE db_datadict;
CREATE PROCEDURE sp_for_routines() SELECT 'db_datadict';
USE test;

# Note(mleich):
# 1. We can get here different error messages.
# 2. We do not want to unify the individual messages to the far to unspecific
#    'Got one of the listed errors'.
let $my_error_message =
##### The previous statement must fail ######
# Server type  | expected error name      | expected error message
# --------------------------------------------------------------------------------------------------------------------
# not embedded | ER_DBACCESS_DENIED_ERROR | ERROR 42000: Access denied for user 'root'@'localhost' to database 'information_schema'
# embedded     | ER_NON_INSERTABLE_TABLE  | ERROR HY000: The target table schemata of the INSERT is not insertable-into
#              | or similar               |              or similar
;

--disable_abort_on_error
INSERT INTO information_schema.routines (routine_name, routine_type )
VALUES ('p2', 'procedure');
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
UPDATE information_schema.routines SET routine_name = 'p2'
WHERE routine_body = 'sql';
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
DELETE FROM information_schema.routines ;
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
TRUNCATE information_schema.routines ;
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
CREATE INDEX i7 ON information_schema.routines (routine_name);
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
ALTER TABLE information_schema.routines  ADD f1 INT;
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
ALTER TABLE information_schema.routines  DISCARD TABLESPACE;
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
DROP TABLE information_schema.routines ;
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
ALTER TABLE information_schema.routines RENAME db_datadict.routines;
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
ALTER TABLE information_schema.routines RENAME information_schema.xroutines;
if (!$mysql_errno)
{
   --echo $my_error_message
   exit;
}
--enable_abort_on_error

# Cleanup
DROP DATABASE db_datadict;

 