--source include/have_ndb.inc

# Test does not need to run with embedded
# since mysqld/libmysqld is not involved
--source include/not_embedded.inc

# Test configs should be cleared and
# servers should be restarted in case of retries
--source include/force_restart.inc

#create database and table so that test can
#access them through a mysql client
CREATE DATABASE TEST_DB;
CREATE TABLE TEST_DB.T1(
  col1 INT PRIMARY KEY,
  col2 FLOAT,
  col3 CHAR(10),
  col4 VARCHAR(100),
  col5 BINARY(10),
  col6 VARBINARY(10),
  col7 INT UNSIGNED,
  col8 INT UNSIGNED
)ENGINE=NDBCLUSTER;

 