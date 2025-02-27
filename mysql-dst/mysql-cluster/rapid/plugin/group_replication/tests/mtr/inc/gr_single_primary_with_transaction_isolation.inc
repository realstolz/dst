# ==== Purpose ====
#
# This include file is a parametric test that does following:
# 1) Configure two servers (server1, server2) to be in group replication
# 2) Verifies group_replication_role on both servers
# 3) create a table on primary node
# 4) tries to insert data into the table, depending on server1 configuration
#    this operation could fail
#
# ==== Usage ====
#
#   --let $test_parameter_enforce_checks= TRUE|FALSE
#   --let $test_parameter_single_primary_mode= TRUE|FALSE
#   --let $test_parameter_tx_isolation= READ-UNCOMMITTED|READ-COMMITTED|REPEATABLE-READ|SERIALIZABLE
#   --let $test_parameter_sql_execution_error= N|ERROR_NAME
#   --let $test_parameter_primary_uuid= UUID
#   --source ../inc/gr_single_primary_with_transaction_isolation.inc
#
# Parameters:
#   $test_parameter_enforce_checks
#     Set value to group_replication_enforce_update_everywhere_checks system variable
#
#   $test_parameter_single_primary_mode
#     Set value to group_replication_single_primary_mode system variable
#
#   $test_parameter_tx_isolation
#     Set value to tx_isolation session system variable
#
#   $test_parameter_sql_execution_error
#     Should the test expect an error while executing INSERT statement. If not
#     the test is going to perform additional check to verify data consistency
#     between server1 and server2
#
#   $test_parameter_primary_uuid
#     Test verifies if the group_replication_primary_member has this uuid
#

--let $include_filename= gr_single_primary_with_transaction_isolation.inc
--source include/begin_include_file.inc

--let $_saved_connection= $CURRENT_CONNECTION

--echo # CONFIGURE SERVER 1
--let $rpl_connection_name= server1
--source include/rpl_connection.inc
--eval SET GLOBAL group_replication_enforce_update_everywhere_checks= $test_parameter_enforce_checks
--eval SET GLOBAL group_replication_single_primary_mode= $test_parameter_single_primary_mode
--eval SET SESSION tx_isolation=$test_parameter_tx_isolation

--source ../inc/start_and_bootstrap_group_replication.inc

--let $group_replication_expected_uuid= $test_parameter_primary_uuid
--source ../inc/gr_wait_primary_member_uuid.inc

--echo # CONFIGURE SERVER 2
--let $rpl_connection_name= server2
--source include/rpl_connection.inc
--eval SET GLOBAL group_replication_enforce_update_everywhere_checks= $test_parameter_enforce_checks
--eval SET GLOBAL group_replication_single_primary_mode= $test_parameter_single_primary_mode
--eval SET SESSION tx_isolation=$test_parameter_tx_isolation
--source include/start_group_replication.inc

--let $group_replication_expected_uuid= $test_parameter_primary_uuid
--source ../inc/gr_wait_primary_member_uuid.inc

--echo # WRITE DATA ON SERVER 1 (ISOLATION LEVEL $test_parameter_tx_isolation)
--echo # AND VERIFY PRESENCE OF THE DATA ON SERVER 2
--let $rpl_connection_name= server1
--source include/rpl_connection.inc
CREATE TABLE first(c1 INT NOT NULL AUTO_INCREMENT PRIMARY KEY);

--error $test_parameter_sql_execution_error
INSERT INTO first(c1) VALUES(2),(4);
if ($test_parameter_sql_execution_error == 0)
{
  --source include/rpl_sync.inc
  --let $rpl_connection_name= server2
  --source include/rpl_connection.inc

  --let $assert_text= All data inserted on server1 should be present on server2
  --let $assert_cond= [SELECT COUNT(*) as count FROM first, count, 1] = 2
  --source include/assert.inc

  SELECT * FROM first;
}

--let $rpl_connection_name= server1
--source include/rpl_connection.inc
DROP TABLE first;

--let $rpl_connection_name= server2
--source include/rpl_connection.inc
--source include/stop_group_replication.inc

--let $rpl_connection_name= server1
--source include/rpl_connection.inc
--source include/stop_group_replication.inc

--connection $_saved_connection

--let $test_parameter_primary_uuid=

--let $include_filename= gr_single_primary_with_transaction_isolation.inc
--source include/end_include_file.inc
 