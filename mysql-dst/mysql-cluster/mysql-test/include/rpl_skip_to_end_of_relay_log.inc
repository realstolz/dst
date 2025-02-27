# ==== Purpose ====
#
# Execute CHANGE MASTER to make the applier threads skip to the end of
# the relay log.
#
# WARNING! After using this file with auto_position=0, the applier
# thread's master_log_pos is not up to date. This can cause
# MASTER_POS_WAIT to hang. The applier thread's master_log_pos is only
# updated when the applier thread executes events, not when CHANGE
# MASTER is used to fast-forward the position as this script does.
#
# ==== Usage ====
#
# [--let $rpl_channel_name= CHANNEL]
# --source include/rpl_skip_to_end_of_relay_log.inc
#
# Parameters:
#   $rpl_channel_name
#     By default, uses the empty channel (channel name ''). Set this
#     if you want to operate on any other channel.

--let $include_filename= include/rpl_skip_to_end_of_relay_log.inc
--source include/begin_include_file.inc

if (!$rpl_debug)
{
  --disable_query_log
}

--source include/rpl_get_end_of_relay_log.inc

--let $_rsteorl_is_auto_position= query_get_value(SHOW SLAVE STATUS, Auto_Position, 1)

if ($_rsteorl_is_auto_position)
{
  --let $_rsteorl_received_gtid_set= query_get_value(SHOW SLAVE STATUS, Retrieved_Gtid_Set, 1)
  # Find transactions in relay log which are not in gtid_executed.
  --let $_rsteorl_skip_set= `SELECT GTID_SUBTRACT('$_rsteorl_received_gtid_set', @@GLOBAL.GTID_EXECUTED)`
  # Generate empty transactions.
  while ($_rsteorl_skip_set != '')
  {
    --let $_rsteorl_skip_gtid= `SELECT GTID_FROM_GTID_SET('$_rsteorl_skip_set')`
    eval SET GTID_NEXT = '$_rsteorl_skip_gtid';
    COMMIT;
    SET GTID_NEXT = 'AUTOMATIC';
    --let $_rsteorl_skip_set= `SELECT GTID_SUBTRACT('$_rsteorl_skip_set', '$_rsteorl_skip_gtid')`
  }
}

if (!$_rsteorl_is_auto_position)
{
  eval CHANGE MASTER TO RELAY_LOG_FILE = '$relay_log_file',
                        RELAY_LOG_POS = $relay_log_size
                        FOR CHANNEL '$rpl_channel_name';
}

--source include/start_slave_sql.inc

--let $include_filename= include/rpl_skip_to_end_of_relay_log.inc
--source include/end_include_file.inc
 