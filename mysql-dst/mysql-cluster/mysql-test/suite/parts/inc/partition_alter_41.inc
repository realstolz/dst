################################################################################
# inc/partition_alter_11.inc                                                   #
#                                                                              #
# Purpose:                                                                     #
#   Check ALTER partitioned table and the state of the table afterwards        #
#   The partitioning function use the column f_int1                            #
#                                                                              #
#   For all partitioning methods                                               #
#        PARTITION BY HASH/KEY/LIST/RANGE                                      #
#        PARTITION BY RANGE/LIST ... SUBPARTITION BY HASH/KEY ...              #
#   do                                                                         #
#     1. Create the partitioned table                                          #
#     2. Execute inc/partition_alter_1.inc, which will                     #
#        - Insert the first half of the table t0_template into t1              #
#        - Execute the ALTER TABLE statement                                   #
#        - Insert the second half of the table t0_template into t1             #
#        - Execute the usability test inc/partition_check.inc              #
#        - Drop the table t1                                                   #
#   done                                                                       #
#                                                                              #
#   The parameters                                                             #
#        $unique -- PRIMARY KEY or UNIQUE INDEXes to be created within the     #
#                   CREATE TABLE STATEMENT                                     #
#        $alter  -- ALTER TABLE statement, which has to be executed            #
#   have to be set before sourcing this routine.                               #
#   Example:                                                                   #
#          let $unique= , UNIQUE INDEX uidx1 (f_int1);                         #
#          let $alter= ALTER TABLE t1 DROP UNIQUE INDEX uidx1;                 #
#          inc/partition_alter1.inc                                        #
#                                                                              #
# Attention: The routine inc/partition_alter_13.inc is very similar        #
#            to this one. So if something has to be changed here it            #
#            might be necessary to do it also there                            #
#                                                                              #
#------------------------------------------------------------------------------#
# Original Author: mleich                                                      #
# Original Date: 2006-03-05                                                    #
# Change Author:                                                               #
# Change Date:                                                                 #
# Change:                                                                      #
################################################################################

--disable_warnings
DROP TABLE IF EXISTS t1;
--enable_warnings

let $partitioning= ;
#----------- PARTITION BY HASH
if ($with_partitioning)
{
let $partitioning= PARTITION BY HASH(f_int1) PARTITIONS 2 (partition part_1, partition part_2);
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc

#----------- PARTITION BY KEY
if ($with_partitioning)
{
let $partitioning= PARTITION BY KEY(f_int1) PARTITIONS 5 (partition part_1, partition part_2, partition part_3, partition part_4, partition part_5);
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc

#----------- PARTITION BY LIST
if ($with_partitioning)
{
let $partitioning= PARTITION BY LIST(MOD(f_int1,4))
(PARTITION part_3 VALUES IN (-3),
 PARTITION part_2 VALUES IN (-2),
 PARTITION part_1 VALUES IN (-1),
 PARTITION part_N VALUES IN (NULL),
 PARTITION part0 VALUES IN (0),
 PARTITION part1 VALUES IN (1),
 PARTITION part2 VALUES IN (2),
 PARTITION part3 VALUES IN (3));
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc

#----------- PARTITION BY RANGE
if ($with_partitioning)
{
--disable_query_log
eval SET @aux = 'PARTITION BY RANGE(f_int1)
(PARTITION parta VALUES LESS THAN (0),
PARTITION part_1 VALUES LESS THAN ($max_row_div4),
PARTITION part_2 VALUES LESS THAN ($max_row_div2),
PARTITION part_3 VALUES LESS THAN ($max_row_div2 + $max_row_div4),
PARTITION part_4 VALUES LESS THAN ($max_row),
PARTITION part_5 VALUES LESS THAN $MAX_VALUE)';
let $partitioning= `SELECT @aux`;
--enable_query_log
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc

#----------- PARTITION BY RANGE -- SUBPARTITION BY HASH
if ($with_partitioning)
{
--disable_query_log
eval SET @aux =
'PARTITION BY RANGE(f_int1 DIV 2) SUBPARTITION BY HASH(f_int1) SUBPARTITIONS 2
(PARTITION part_1 VALUES LESS THAN (0),
PARTITION part_2 VALUES LESS THAN ($max_row_div4),
PARTITION part_3 VALUES LESS THAN ($max_row_div2),
PARTITION part_4 VALUES LESS THAN $MAX_VALUE)';
let $partitioning= `SELECT @aux`;
--enable_query_log
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc

#----------- PARTITION BY RANGE -- SUBPARTITION BY KEY
if ($with_partitioning)
{
--disable_query_log
eval SET @aux = 'PARTITION BY RANGE(f_int1) SUBPARTITION BY KEY(f_int1)
(PARTITION part_1 VALUES LESS THAN (0)
(SUBPARTITION subpart11, SUBPARTITION subpart12),
PARTITION part_2 VALUES LESS THAN ($max_row_div4)
(SUBPARTITION subpart21, SUBPARTITION subpart22),
PARTITION part_3 VALUES LESS THAN ($max_row_div2)
(SUBPARTITION subpart31, SUBPARTITION subpart32),
PARTITION part_4 VALUES LESS THAN $MAX_VALUE
(SUBPARTITION subpart41, SUBPARTITION subpart42))';
let $partitioning= `SELECT @aux`;
--enable_query_log
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc

#----------- PARTITION BY LIST -- SUBPARTITION BY HASH
if ($with_partitioning)
{
let $partitioning= PARTITION BY LIST(ABS(MOD(f_int1,3))) SUBPARTITION BY HASH(f_int1 + 1)
(PARTITION part_1 VALUES IN (0)
  (SUBPARTITION sp11, SUBPARTITION sp12),
 PARTITION part_2 VALUES IN (1)
  (SUBPARTITION sp21, SUBPARTITION sp22),
 PARTITION part_3 VALUES IN (2)
  (SUBPARTITION sp31, SUBPARTITION sp32),
 PARTITION part_4 VALUES IN (NULL)
  (SUBPARTITION sp41, SUBPARTITION sp42));
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc

#----------- PARTITION BY LIST -- SUBPARTITION BY KEY
if ($with_partitioning)
{
--disable_query_log
eval SET @aux =
'PARTITION BY LIST(ABS(MOD(f_int1,2)))
SUBPARTITION BY KEY(f_int1) SUBPARTITIONS $sub_part_no
(PARTITION part_1 VALUES IN (0),
 PARTITION part_2 VALUES IN (1),
 PARTITION part_3 VALUES IN (NULL))';
let $partitioning= `SELECT @aux`;
--enable_query_log
}
eval CREATE TABLE t1 (
$column_list
$unique
)
$partitioning;
--source suite/parts/inc/partition_alter_1.inc
 