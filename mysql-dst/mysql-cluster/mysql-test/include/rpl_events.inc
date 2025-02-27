##################################################################
# Author: Giuseppe, Chuck Bell                                   #
# Date: 17-January-2007                                          #
# Purpose: To test that event effects are replicated             #
# in both row based and statement based format                   #
##################################################################

--disable_warnings
DROP EVENT IF EXISTS test.justonce;
drop table if exists t1,t2;
--enable_warnings

# first, we need a table to record something from an event

eval CREATE TABLE `t1` (
  `id` INT(10) UNSIGNED NOT NULL,
  `c` VARCHAR(50) NOT NULL,
  `ts` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=$engine_type DEFAULT CHARSET=utf8;

INSERT INTO t1 (id, c) VALUES (1, 'manually');

# We create the event so that it inserts exactly 1 row in the table
# A recuring event is used so that we can be sure the event will
# fire regardless of timing delays on the server. Otherwise, it is
# possible for the event to timeout before it has inserted a row.
--echo "Creating event test.justonce on the master"
CREATE EVENT test.justonce ON SCHEDULE EVERY 2 SECOND DO 
  INSERT IGNORE INTO t1 (id, c) VALUES (2, 'from justonce');

# Show the event is alive and present on master
--echo "Checking event is active on master"
SELECT db, name, status, originator FROM mysql.event WHERE db = 'test' AND name = 'justonce';

# Wait until event has fired. We know this because t1 will contain 
# the row from the event.
let $wait_condition=
  SELECT COUNT(*) = 1 FROM t1 WHERE c = 'from justonce';
--source include/wait_condition.inc

# check that table t1 contains something
--echo "Checking event data on the master"
let $events_done=`SELECT count(*)  FROM t1 id`;
--disable_query_log
eval SELECT $events_done > 0 as ONE;
--enable_query_log

sync_slave_with_master;

--echo "Checking event data on the slave"
--disable_query_log
eval SELECT count(*) - $events_done as ZERO FROM t1 id;
--enable_query_log

--echo "Checking event is inactive on slave"
SELECT db, name, status, originator FROM mysql.event WHERE db = 'test' AND name = 'justonce';

# Create an event on the slave and check to see what the originator is.
--echo "Dropping event test.slave_once on the slave"
--disable_warnings
DROP EVENT IF EXISTS test.slave_once;
--enable_warnings

# Create an event on slave and check its state. An event shouldn't be executed 
# so set start time in 1 hour.
CREATE EVENT test.slave_once ON SCHEDULE EVERY 5 MINUTE STARTS CURRENT_TIMESTAMP + INTERVAL 1 HOUR DO 
  INSERT IGNORE INTO t1(id, c) VALUES (3, 'from slave_once');

--echo "Checking event status on the slave for originator value = slave's server_id"
SELECT db, name, status, originator FROM mysql.event WHERE db = 'test' AND name = 'slave_once';

--echo "Dropping event test.slave_once on the slave"
--disable_warnings
DROP EVENT IF EXISTS test.slave_once;
--enable_warnings

connection master;

# BUG#20384 - disable events on slave
--echo "Dropping event test.justonce on the master"
--disable_warnings
DROP EVENT IF EXISTS test.justonce;
--enable_warnings

# Create an event on master and check its state on slave. An event shouldn't be executed 
# so set start time in 1 hour. Check that changes of event statement replicated to slave

--echo "Creating event test.er on the master"
CREATE EVENT test.er ON SCHEDULE EVERY 3 SECOND STARTS CURRENT_TIMESTAMP + INTERVAL 1 HOUR DO 
  INSERT IGNORE INTO t1(id, c) VALUES (4, 'from er');

--echo "Checking event status on the master"
SELECT db, name, status, originator, body FROM mysql.event WHERE db = 'test' AND name = 'er';

sync_slave_with_master;

--echo "Checking event status on the slave"
SELECT db, name, status, originator, body FROM mysql.event WHERE db = 'test' AND name = 'er';

connection master;
--echo "Altering event test.er on the master"
ALTER EVENT test.er ON SCHEDULE EVERY 5 SECOND STARTS CURRENT_TIMESTAMP + INTERVAL 1 HOUR DO 
  INSERT IGNORE INTO t1(id, c) VALUES (5, 'from alter er');

--echo "Checking event status on the master"
SELECT db, name, status, originator, body FROM mysql.event WHERE db = 'test' AND name = 'er';

sync_slave_with_master;

--echo "Checking event status on the slave"
SELECT db, name, status, originator, body FROM mysql.event WHERE db = 'test' AND name = 'er';

connection master;
--echo "Dropping event test.er on the master"
DROP EVENT test.er;

--echo "Checking event status on the master"
SELECT db, name, status, originator FROM mysql.event WHERE db = 'test';

--disable_info

sync_slave_with_master;

--echo "Checking event status on the slave"
SELECT db, name, status, originator FROM mysql.event WHERE db = 'test';

# test the DISABLE ON SLAVE for setting event SLAVESIDE_DISABLED as status
# on CREATE EVENT

# Create an event on slave and check its status. An event shouldn't be executed 
# so set start time in 1 hour.

--echo "Creating event test.slave_terminate on the slave"
CREATE EVENT test.slave_terminate ON SCHEDULE EVERY 3 SECOND STARTS CURRENT_TIMESTAMP + INTERVAL 1 HOUR DO 
  INSERT IGNORE INTO t1(id, c) VALUES (6, 'from slave_terminate');

--echo "Checking event status on the slave"
SELECT db, name, status, originator FROM mysql.event WHERE db = 'test' AND name = 'slave_terminate';

--echo "Dropping event test.slave_terminate on the slave"
DROP EVENT test.slave_terminate;

--echo "Creating event test.slave_terminate with DISABLE ON SLAVE on the slave"
CREATE EVENT test.slave_terminate ON SCHEDULE EVERY 3 SECOND DISABLE ON SLAVE DO 
  INSERT IGNORE INTO t1(c) VALUES (7, 'from slave_terminate');

--echo "Checking event status on the slave"
SELECT db, name, status, originator FROM mysql.event WHERE db = 'test' AND name = 'slave_terminate';

--echo "Dropping event test.slave_terminate on the slave"
DROP EVENT test.slave_terminate;

--echo "Cleanup"
connection master;
DROP TABLE t1;
sync_slave_with_master;
connection master;

 