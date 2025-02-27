# ==== Purpose ====
#
# Issues STOP SLAVE IO_THREAD on the current connection. Then waits
# until the IO thread has stopped, or until a timeout is reached.
#
# This will fail if the slave IO thread has an error. If you expect an
# error in the IO thread, use include/wait_for_slave_io_error.inc
# instead.
#
# Please use this instead of 'STOP SLAVE IO_THREAD', to reduce the
# risk of races in test cases.
#
#
# ==== Usage ====
#
# [--let $rpl_channel_name= NAME]
# [--let $slave_timeout= NUMBER]
# [--let $rpl_debug= 1]
# --source include/stop_slave_io.inc
#
# Parameters:
#   $rpl_channel_name
#     If this is not '', a FOR CHANNEL  '$rpl_channel_name' is added.
#
#   $slave_timeout
#     See include/wait_for_slave_param.inc
#
#   $rpl_debug
#     See include/rpl_init.inc


--let $include_filename= stop_slave_io.inc
--source include/begin_include_file.inc


if (!$rpl_debug)
{
  --disable_query_log
}


if ($rpl_channel_name != '')
{
  eval STOP SLAVE IO_THREAD FOR CHANNEL  '$rpl_channel_name';
}
if ($rpl_channel_name == '')
{
  STOP SLAVE IO_THREAD;
}

--source include/wait_for_slave_io_to_stop.inc


--let $include_filename= stop_slave_io.inc
--source include/end_include_file.inc
 