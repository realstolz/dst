#
# Bug #51639
# This test verifies that 'ignore table' filter will follow
# the setting of lower_case_table_name to be case insensitive
# but accent sensitive when setting lower_case_table_name>0
#

-- source include/master-slave.inc
-- source include/have_binlog_format_statement.inc

-- source extra/rpl_tests/rpl_filters.test 
let $replicate_ignore_table= query_get_value(SHOW SLAVE STATUS, Replicate_Ignore_Table, 1);
--echo Replicate_Ignore_Table: $replicate_ignore_table

--source include/rpl_end.inc
 