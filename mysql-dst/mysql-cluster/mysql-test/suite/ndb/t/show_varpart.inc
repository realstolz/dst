--disable_query_log
let $dump_file = $MYSQLTEST_VARDIR/tmp/desc.txt;

if (`select LENGTH('$show_varpart_table') = 0`)
{
  # Default is to show properties of t1
  let $show_varpart_table=t1;
}

# Dump the output of ndb_desc to file
--exec $NDB_DESC --no-defaults -d test $show_varpart_table > $dump_file

# Load the file into a temporary table
CREATE TEMPORARY TABLE test.desc (a varchar(512)) ENGINE = HEAP;

# needed for embedded
--chmod 0777 $dump_file
--replace_result $dump_file DUMP_FILE
eval LOAD DATA INFILE '$dump_file' INTO TABLE test.desc;
--remove_file $dump_file

# Query the temporary table
let $value = `SELECT TRIM(TRAILING "\r" FROM a) FROM test.desc
  WHERE a LIKE BINARY "ForceVarPart%"`;
echo $value;

DROP TABLE test.desc;
--enable_query_log
 