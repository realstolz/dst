################################################################################
# inc/partition_layout_check1.inc                                          #
#                                                                              #
# Purpose:                                                                     #
#   Store the SHOW CREATE TABLE output and the list of files belonging to      #
#   this table + print this into the protocol                                  #
#   This script is only usefule when sourced within the partitioning tests.    #
#                                                                              #
# Attention: The routine inc/partition_layout_check2.inc is very similar   #
#            to this one. So if something has to be changed here it            #
#            might be necessary to do it also there                            #
#                                                                              #
#------------------------------------------------------------------------------#
# Original Author: mleich                                                      #
# Original Date: 2006-03-05                                                    #
# Change Author: pcrews                                                        #
# Change Date: 2008-04-15                                                      #
# Change: Added --replace_result to account for Windows' use of '\r'           #
################################################################################

if ($no_debug)
{
--disable_query_log
}
# Clean the table holding the definition of t1
DELETE FROM t0_definition;

# Get the MySQL Servers datadir without ending slash
let $MYSQLD_DATADIR= `select LEFT(@@datadir, LENGTH(@@datadir)-1)`;
#echo MYSQLD_DATADIR: $MYSQLD_DATADIR;

# Save the current definition of the table t1
# - SHOW CREATE TABLE t1 is at least currently most probably more reliable than
#   the corresponding SELECT on the INFORMATION_SCHEMA
let $show_create= `SHOW CREATE TABLE t1`;
if ($do_file_tests)
{
  # List the files belonging to the table t1
  let $ls_file= $MYSQLD_DATADIR/test/tmp2;
  --list_files_write_file $ls_file $MYSQLD_DATADIR/test t1*
  --chmod 0644 $ls_file
  if ($with_directories)
  {
    --list_files_append_file $ls_file $MYSQLTEST_VARDIR/mysql-test-data-dir t1*
    --list_files_append_file $ls_file $MYSQLTEST_VARDIR/mysql-test-idx-dir t1*
  }
  eval SET @aux = load_file('$ls_file');

  # clean up
  --remove_file $ls_file
}
if (!$do_file_tests)
{
  SET @aux = '--- not determined ---';
}

# Insert the current definition of the table t1 into t0_definition
eval INSERT INTO t0_definition SET state = 'old',
     create_command = "$show_create",
     file_list      = @aux;

# Print the create table statement into the protocol
# Added the concat to avoid changing the result files
--replace_result $MYSQLTEST_VARDIR MYSQLTEST_VARDIR '\r' ''
SELECT concat('SHOW CREATE TABLE t1;\nTable\tCreate Table\n',create_command,'\n') as `create_command` FROM t0_definition WHERE state = 'old';
if ($do_file_tests)
{
   # We stored the list of files, therefore printing the content makes sense
   if ($ls)
   {
      # Print the list of files into the protocol
      replace_result $MYSQLD_DATADIR MYSQLD_DATADIR $MYSQLTEST_VARDIR MYSQLTEST_VARDIR #p# #P# #sp# #SP# part_n part_N;
      SELECT file_list AS "unified filelist"
       FROM t0_definition WHERE state = 'old';
   }
}
--enable_query_log
 