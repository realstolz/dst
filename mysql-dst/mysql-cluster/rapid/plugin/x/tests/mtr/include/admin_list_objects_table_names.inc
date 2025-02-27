## Simple list objects with case sensitive names

## Usage
#
#--let $expect_case_sensitive_variant= 0|1|2
#--let $expect_table_error= ER_SUCCESS|ER_TABLE_EXISTS_ERROR
#--let $expect_db_error= ER_SUCCESS|ER_DB_CREATE_EXISTS
#--source ../include/admin_list_objects_table_names.inc

## Test starts here
--let $xtest_file= $MYSQL_TMP_DIR/admin_list_objects_table_names.tmp
--write_file $xtest_file
-->quiet
## Test data

-->stmtsql SELECT @@lower_case_table_names AS Value;
-->recvtovar %VALUE% Value
-->assert_eq %EXPECT_CASE%	%VALUE%

-->macro Admin_list_objects_case_sensitive  %SCHEMA%  %ERROR0%  %ERROR1%  %ERROR2%  %ERROR3%
-->title -Creating collection in %SCHEMA%
-->stmtsql CREATE SCHEMA %SCHEMA% DEFAULT CHARSET 'utf8mb4';
-->expecterror %ERROR0%
-->recvresult
-->stmtadmin create_collection	{"schema":"%SCHEMA%", "name":"FRUIT"}
-->expecterror %ERROR1%
-->recvresult
-->stmtadmin create_collection	{"schema":"%SCHEMA%", "name":"fruit"}
-->expecterror %ERROR2%
-->recvresult
-->stmtadmin create_collection	{"schema":"%SCHEMA%", "name":"FrUiT"}
-->expecterror %ERROR3%
-->recvresult
-->stmtadmin list_objects	{"schema":"%SCHEMA%"}
-->recvresult
-->endmacro

-->callmacro Admin_list_objects_case_sensitive XTEST	ER_SUCCESS	ER_SUCCESS	%EXPECT_TABLE_ERROR%	%EXPECT_TABLE_ERROR%
-->callmacro Admin_list_objects_case_sensitive xtest	%EXPECT_DB_ERROR%	%EXPECT_TABLE_ERROR%	%EXPECT_TABLE_ERROR%	%EXPECT_TABLE_ERROR%
-->callmacro Admin_list_objects_case_sensitive xTeSt	%EXPECT_DB_ERROR%	%EXPECT_TABLE_ERROR%	%EXPECT_TABLE_ERROR%	%EXPECT_TABLE_ERROR%

-->sql
DROP SCHEMA IF EXISTS xtest;
DROP SCHEMA IF EXISTS XTEST;
DROP SCHEMA IF EXISTS xTeSt;
-->endsql
EOF

exec $MYSQLXTEST
  -v%EXPECT_CASE%=$expect_case_sensitive_variant
  -v%EXPECT_TABLE_ERROR%=$expect_table_error
  -v%EXPECT_DB_ERROR%=$expect_db_error
  -uroot
  --password=''
  --file=$xtest_file 2>&1;

## Postamble
--remove_file $xtest_file

 