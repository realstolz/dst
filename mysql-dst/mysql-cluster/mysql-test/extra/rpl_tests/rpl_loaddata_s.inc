# See if the slave logs (in its own binlog, with --log-slave-updates) a
# replicated LOAD DATA INFILE correctly when it has binlog_*_db rules.
# This is for BUG#1100 (LOAD DATA INFILE was half-logged).

-- source include/have_binlog_format_mixed_or_statement.inc
-- source include/master-slave.inc

connection slave;
# Not sure why we connect to slave and then try to reset master, but I will leave it [JBM]
reset master;

connection master;
# 'test' is the current database
create table test.t1(a int, b int, unique(b));
load data infile '../../std_data/rpl_loaddata.dat' into table test.t1;

# Test logging on slave;
--source include/sync_slave_sql_with_master.inc
select count(*) from test.t1; # check that LOAD was replicated
source include/show_binlog_events.inc;

# Cleanup
connection master;
drop table test.t1;
--source include/sync_slave_sql_with_master.inc
--source include/rpl_end.inc
 