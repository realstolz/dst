# ==== Purpose ====
#
# Issues START SLAVE IO_THREAD on the current connection. Then waits
# until the IO thread has started, or until a timeout is reached.
#
# Please use this instead of 'START SLAVE IO_THREAD', to reduce the
# risk of races in test cases.
#
#
# ==== Usage ====
#
# [--let $rpl_channel_name= NAME]
# [--let $slave_timeout= NUMBER]
# [--let $rpl_debug= 1]
# --source include/start_slave_io.inc
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


--let $include_filename= start_slave_io.inc
--source include/begin_include_file.inc


if (!$rpl_debug)
{
  --disable_query_log
}


if ($rpl_channel_name != '')
{
  eval START SLAVE IO_THREAD FOR CHANNEL  '$rpl_channel_name';
}
if ($rpl_channel_name == '')
{
  START SLAVE IO_THREAD;
}

--source include/wait_for_slave_io_to_start.inc


--let $include_filename= start_slave_io.inc
--source include/end_include_file.inc
 