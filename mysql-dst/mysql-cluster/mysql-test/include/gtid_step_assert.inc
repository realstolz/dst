# ==== Purpose ====
#
# Assert that exactly one GTID (or a specified number) was added to
# @@GLOBAL.GTID_EXECUTED since last call to gtid_step_reset.inc or
# gtid_step_assert.inc
#
# ==== Usage ====
#
# [--let $gtid_step_count= N]
# [--let $gtid_step_only_count= [0|1]]
# [--let $gtid_step_uuid= UUID]
# [--let $gtid_step_check_table= [0|1]]
# --source include/gtid_step_assert.inc
#
# Parameters:
#   $gtid_step_count
#     Assert that N GTIDs were added to @@GLOBAL.GTID_EXECUTED. If
#     this is not given, the default value is 1.
#
#   $gtid_step_only_count
#     By default, this script checks the numeric component of the
#     GTIDs added to @@GLOBAL.GTID_EXECUTED to see that the correct
#     number was generated.  If this parameter is nonzero, it only
#     counts the number of added GTIDs to see that it is equal to
#     $gtid_step_count, without caring that they were generated in
#     order (or generated on this server).
#
#   $gtid_step_uuid
#     By default, when $gtid_step_count is not set, asserts that the
#     uuid component of all GTIDs added to @@GLOBAL.GTID_EXECUTED is
#     equal to @@GLOBAL.SERVER_UUID.  If this parameter is given, uses
#     that UUID instead.
#
#   $gtid_step_gtid_mode_agnostic
#     If this is set, the script will work also if gtid_mode=off.
#     In that case it will assert that no GTID was generated.
#
#   $gtid_step_check_table
#     This script checks that exactly $_gtid_step_count GTIDs have been
#     added to mysql.gtid_executed since last invocation if this parameter
#     is nonzero. The check is skipped if this parameter is not set or zero.
#
# ==== Example ====
#
# # (1) Remember the current position.
# --source include/gtid_step_reset.inc
# CREATE TABLE t1 (a INT);
# # (2) Assert that exactly one GTID was generated since (1).
# --source include/gtid_step_assert.inc
# INSERT INTO t1 VALUES (1);
# INSERT INTO t1 VALUES (2);
# # (3) Assert that exactly 2 GTIDs were generated since (2).
# --let $gtid_step_count= 2
# --source include/gtid_step_assert.inc
# # (4) Assert that no GTID was generated since (3).
# --let $gtid_step_count= 0
# --source include/gtid_step_assert.inc

if ($uuidf == '')
{
  --die You must source gtid_utils.inc before using gtid_step_assert.inc
}

--let $_gtid_step_count= 1
if ($gtid_step_count != '')
{
  --let $_gtid_step_count= $gtid_step_count
}

--let $_gtid_step_only_count= 0
if ($gtid_step_only_count != '')
{
  --let $_gtid_step_only_count= $gtid_step_only_count
}

--let $_gtid_step_print_count= [count=$_gtid_step_count, only_count=$_gtid_step_only_count]
if ($_gtid_step_dont_print_count)
{
  --let $_gtid_step_print_count=
}

--let $include_filename= gtid_step_assert.inc $_gtid_step_print_count
--source include/begin_include_file.inc

if ($gtid_step_gtid_mode_agnostic)
{
  if (!$gtid_mode_on)
  {
    --let $_gtid_step_count= 0
  }
}

--let $_gtid_step_uuid= $gtid_step_uuid
if ($_gtid_step_uuid == '')
{
  --let $_gtid_step_uuid= `SELECT @@GLOBAL.SERVER_UUID`
}

--let $_gsa_extra_debug_eval_old= $extra_debug_eval
--let $extra_debug_eval= @@GLOBAL.GTID_EXECUTED, GTID_NEXT_GENERATED_MULTIPLE("$gtid_step_last", "$_gtid_step_uuid", $_gtid_step_count), GTID_SUBTRACT(@@GLOBAL.GTID_EXECUTED, "$gtid_step_last")

if ($_gtid_step_only_count)
{
  --let $assert_text= Exactly $_gtid_step_count GTIDs should have been committed since last invocation
  --let $assert_cond= GTID_COUNT(GTID_SUBTRACT(@@GLOBAL.GTID_EXECUTED, "$gtid_step_last")) = $_gtid_step_count
  --source include/assert.inc
}
if (!$_gtid_step_only_count)
{
  --let $assert_text= Exactly $_gtid_step_count GTIDs should have been generated since last invocation
  --let $assert_cond= GTID_NEXT_GENERATED_MULTIPLE("$gtid_step_last", "$_gtid_step_uuid", $_gtid_step_count) = GTID_SUBTRACT(@@GLOBAL.GTID_EXECUTED, "$gtid_step_last")
  --source include/assert.inc
}

if ($gtid_step_check_table)
{
  if ($_gtid_step_only_count)
  {
    --let $assert_text= Exactly $_gtid_step_count GTIDs should have been added to mysql.gtid_executed since last invocation
    --let $assert_cond= GTID_COUNT(GTID_SUBTRACT(GTID_EXECUTED_FROM_TABLE(), "$gtid_step_last")) = $_gtid_step_count
    --source include/assert.inc
  }
  if (!$_gtid_step_only_count)
  {
    --let $assert_text= Exactly $_gtid_step_count GTIDs should have been added to mysql.gtid_executed since last invocation
    --let $assert_cond= GTID_NEXT_GENERATED_MULTIPLE("$gtid_step_last", "$_gtid_step_uuid", $_gtid_step_count) = GTID_SUBTRACT(GTID_EXECUTED_FROM_TABLE(), "$gtid_step_last")
    --source include/assert.inc
  }
}

--let $extra_debug_eval= $_gsa_extra_debug_eval_old

--source include/gtid_step_reset.inc

--let $include_filename= gtid_step_assert.inc
--source include/end_include_file.inc
 