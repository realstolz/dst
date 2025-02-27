#
# Test for read before write removal(rbwr) optimization in MySQL Cluster
#  - Run one testcase using different setttings
#

let $counter = 2;
while($counter)
{

  # Load data into test table
  eval insert into $test_table select * from data_$test_table;

  # Create reference table
  eval create table ref_$test_table engine=innodb select * from $test_table;

  # Run the query against reference table
  let $test_query_ref =
    `SELECT REPLACE('$test_query', '$test_table', 'ref_$test_table')`;
  eval $test_query_ref;

  # Save ndb_execute_count from before query
  let $before = query_get_value(
    select VARIABLE_VALUE from information_schema.session_status
      where variable_name like 'NDB_EXECUTE_COUNT', VARIABLE_VALUE, 1);

  let $transaction = 0;
  if ($counter == 2)
  {
    # Run within transaction, this may cause one more execute
    let $transaction = 1;
    begin;
  }

  # Run the query
  --enable_info
  eval $test_query;
  --disable_info

  if ($counter == 2)
  {
    # Commit the transaction
    commit;
  }

  # Calculate number of ndb_execute_count's generated by query
  let $diff = query_get_value(
    select VARIABLE_VALUE-$before AS diff from information_schema.session_status
      where variable_name like 'NDB_EXECUTE_COUNT', diff, 1);

  if ($transaction)
  {
    # Check if $diff should be decreased to compensate for the
    # execute caused by using transaction
    if ($diff != $test_execute_count)
    {
      dec $diff;
    }
  }

  if ($diff != $test_execute_count)
  {
    --echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    --echo Unxpected ndb_execute_count detected
    --echo  - actual ndb_execute_count: $diff
    --echo  - expected ndb_execute_count: $test_execute_count
    --echo  - query: '$test_query'
    --echo  - Within transaction: $transaction
    --echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    let $ndb_rbwr_fail = 1;
  }

  # Compare data in test table against reference table
  let $ndb_diff_tables= $test_table, ref_$test_table;
  --source suite/ndb/include/ndb_diff_tables.inc

  # Cleanup
  eval delete from $test_table;
  eval drop table ref_$test_table;

  dec $counter;

}
 