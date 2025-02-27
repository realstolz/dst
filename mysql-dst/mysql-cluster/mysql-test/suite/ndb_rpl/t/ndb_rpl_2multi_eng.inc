#######################################
# Author: JBM                         #
# Date: 2006-02-23                    #
# Purpose: See if replication between #
# NDB -> MyISAM and InnoDB works.     #
# and if                              #   
# MyISAM and InnoDB -> NDB works.     #
#######################################
# By JBM                              #
# Date 2006-02-28                     #
# Change: Implemented review comments #
#######################################

--echo --- Doing pre test cleanup --- 

connection master;
--disable_warnings
DROP TABLE IF EXISTS t1;
--enable_query_log

--echo --- Start test 1 Basic testing ---
--echo --- Create Table Section ---

#################################################
# Requirment: Create basic table, replicate     #
# basice operations such at insert, update      #
# delete between 2 different storage engines    #
# Alter table and ensure table is handled       #
# Correctly on the slave                        #
#################################################

CREATE TABLE t1 (id MEDIUMINT NOT NULL, b1 BIT(8), vc VARCHAR(255), 
                 bc CHAR(255), d DECIMAL(10,4) DEFAULT 0, 
                 f FLOAT DEFAULT 0, total BIGINT UNSIGNED, 
                 y YEAR, t DATE,PRIMARY KEY(id));

--echo --- Show table on master ---

SHOW CREATE TABLE t1;

--echo --- Show table on slave ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

# Okay lets see how it holds up to table changes
--echo --- Check that simple Alter statements are replicated correctly --

ALTER TABLE t1 DROP PRIMARY KEY, ADD PRIMARY KEY(id, total);
ALTER TABLE t1 MODIFY vc TEXT;

--echo --- Show the new improved table on the master ---

SHOW CREATE TABLE t1;

--echo --- Make sure that our tables on slave are still same engine ---
--echo --- and that the alter statements replicated correctly ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly --- 

--source include/rpl_multi_engine3.inc

--echo --- End test 1 Basic testing ---
--echo --- Do Cleanup --

DROP TABLE IF EXISTS t1;

#################################################################

--echo --- Start test 2 partition RANGE testing --
--echo --- Do setup --


#################################################
# Requirment: Create table that is partitioned  #
# by range on year i.e. year(t) and replicate   #
# basice operations such at insert, update      #
# delete between 2 different storage engines    #
# Alter table and ensure table is handled       #
# Correctly on the slave                        #
#################################################

CREATE TABLE t1 (id MEDIUMINT NOT NULL, b1 BIT(8), vc VARCHAR(255), 
                 bc CHAR(255), d DECIMAL(10,4) DEFAULT 0, 
                 f FLOAT DEFAULT 0, total BIGINT UNSIGNED, 
                 y YEAR, t DATE, primary key(t))
                 PARTITION BY RANGE (YEAR(t)) 
                (PARTITION p0 VALUES LESS THAN (1901), 
                 PARTITION p1 VALUES LESS THAN (1946),  
                 PARTITION p2 VALUES LESS THAN (1966), 
                 PARTITION p3 VALUES LESS THAN (1986), 
                 PARTITION p4 VALUES LESS THAN (2005), 
                 PARTITION p5 VALUES LESS THAN MAXVALUE);

--echo --- Show table on master ---

SHOW CREATE TABLE t1;

--echo --- Show table on slave --

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

--echo --- Check that simple Alter statements are replicated correctly ---

ALTER TABLE t1 DROP PRIMARY KEY, ADD PRIMARY KEY(t, id);
ALTER TABLE t1 MODIFY vc TEXT;

--echo --- Show the new improved table on the master ---

SHOW CREATE TABLE t1;

--echo --- Make sure that our tables on slave are still same engine ---
--echo --- and that the alter statements replicated correctly ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---
--enable_query_log

--source include/rpl_multi_engine3.inc

--echo --- End test 2 partition RANGE testing ---
--echo --- Do Cleanup ---

DROP TABLE IF EXISTS t1;

########################################################

--echo --- Start test 3 partition LIST testing ---
--echo --- Do setup ---
#################################################
# Requirment: Create table that is partitioned  #
# by list on id i.e. (2,4). Pretend that we     #
# missed one and alter to add. Then  replicate  #
# basice operations such at insert, update      #
# delete between 2 different storage engines    #
# Alter table and ensure table is handled       #
# Correctly on the slave                        #
#################################################


CREATE TABLE t1 (id MEDIUMINT NOT NULL, b1 BIT(8), vc VARCHAR(255), 
                 bc CHAR(255), d DECIMAL(10,4) DEFAULT 0, 
                 f FLOAT DEFAULT 0, total BIGINT UNSIGNED, 
                 y YEAR, t DATE, primary key(id))
                 PARTITION BY LIST(id) 
                (PARTITION p0 VALUES IN (2, 4), 
                 PARTITION p1 VALUES IN (42, 142));

--echo --- Test 3 Alter to add partition ---

ALTER TABLE t1 ADD PARTITION (PARTITION p2 VALUES IN (412));

--echo --- Show table on master ---

SHOW CREATE TABLE t1;

--echo --- Show table on slave ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

--echo --- Check that simple Alter statements are replicated correctly ---

ALTER TABLE t1 MODIFY vc TEXT;

--echo --- Show the new improved table on the master ---

SHOW CREATE TABLE t1;

--echo --- Make sure that our tables on slave are still same engine ---
--echo --- and that the alter statements replicated correctly ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

--echo --- End test 3 partition LIST testing ---
--echo --- Do Cleanup --

DROP TABLE IF EXISTS t1;

########################################################

--echo --- Start test 4 partition HASH testing ---
--echo --- Do setup ---
#################################################
# Requirment: Create table that is partitioned  #
# by hash on year i.e. YEAR(t). Then replicate  #
# basice operations such at insert, update      #
# delete between 2 different storage engines    #
# Alter table and ensure table is handled       #
# Correctly on the slave                        #
#################################################


CREATE TABLE t1 (id MEDIUMINT NOT NULL, b1 BIT(8), vc VARCHAR(255), 
                 bc CHAR(255), d DECIMAL(10,4) DEFAULT 0, 
                 f FLOAT DEFAULT 0, total BIGINT UNSIGNED, 
                 y YEAR, t DATE, primary key(t))
                 PARTITION BY HASH( YEAR(t) ) 
                 PARTITIONS 4; 

--echo --- show that tables have been created correctly ---

SHOW CREATE TABLE t1;
sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

--echo --- Check that simple Alter statements are replicated correctly ---

ALTER TABLE t1 DROP PRIMARY KEY, ADD PRIMARY KEY(t,id);
ALTER TABLE t1 MODIFY vc TEXT;

--echo --- Show the new improved table on the master ---

SHOW CREATE TABLE t1;

--echo --- Make sure that our tables on slave are still same engine ---
--echo --- and that the alter statements replicated correctly ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

--echo --- End test 4 partition HASH testing ---
--echo --- Do Cleanup --

DROP TABLE IF EXISTS t1;

########################################################

--echo --- Start test 5 partition by key testing ---
--echo --- Create Table Section ---

#################################################
# Requirment: Create table that is partitioned  #
# by key on id with 4 parts.    Then replicate  #
# basice operations such at insert, update      #
# delete between 2 different storage engines    #
# Alter table and ensure table is handled       #
# Correctly on the slave                        #
#################################################

CREATE TABLE t1 (id MEDIUMINT NOT NULL, b1 BIT(8), vc VARCHAR(255), 
                 bc CHAR(255), d DECIMAL(10,4) DEFAULT 0, 
                 f FLOAT DEFAULT 0, total BIGINT UNSIGNED, 
                 y YEAR, t DATE,PRIMARY KEY(id))
                 PARTITION BY KEY() 
                 PARTITIONS 4;

--echo --- Show that tables on master are ndbcluster tables ---

SHOW CREATE TABLE t1;

--echo --- Show that tables on slave ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

# Okay lets see how it holds up to table changes
--echo --- Check that simple Alter statements are replicated correctly ---

ALTER TABLE t1 DROP PRIMARY KEY, ADD PRIMARY KEY(id, total);

--echo --- Show the new improved table on the master ---

SHOW CREATE TABLE t1;

--echo --- Make sure that our tables on slave are still right type ---
--echo --- and that the alter statements replicated correctly ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

--echo --- Check that simple Alter statements are replicated correctly ---

ALTER TABLE t1 MODIFY vc TEXT;

--echo --- Show the new improved table on the master ---

SHOW CREATE TABLE t1;

--echo --- Make sure that our tables on slave are still same engine ---
--echo --- and that the alter statements replicated correctly ---

sync_slave_with_master;
SHOW CREATE TABLE t1;

--echo --- Perform basic operation on master ---
--echo --- and ensure replicated correctly ---

--source include/rpl_multi_engine3.inc

--echo --- End test 5 key partition testing ---
--echo --- Do Cleanup ---

DROP TABLE IF EXISTS t1;
sync_slave_with_master;

# End of 5.1 test case
 